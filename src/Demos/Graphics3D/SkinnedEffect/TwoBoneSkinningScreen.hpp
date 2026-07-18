// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SkinnedEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::SkinnedEffectDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::SkinnedEffect;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::VertexBuffer;
using Microsoft::Xna::Framework::Graphics::Viewport;

// GPU-compact skinned vertex, matching the EasyGL stride-52 layout exactly
// (Position/Normal/TextureCoordinate/BlendWeight/BlendIndices) -- built as a
// raw struct (not VertexPositionNormalTextureSkinned) and uploaded via the
// simple 2-arg VertexBuffer(device, count) constructor (empty
// VertexDeclaration) + SetDataRaw(), mirroring cna/examples/
// easygl_skinnedeffect_multilight_test.cpp exactly -- the one confirmed-
// passing, real reference usage of SkinnedEffect on this backend found in
// the whole codebase. An earlier version of this screen built on
// VertexPositionNormalTextureSkinned + an explicitly-passed
// VertexDeclaration (the generic Task-1080 per-element attribute-binding
// path) and rendered NOTHING at all, with no exception and no error log,
// verified repeatedly via Xvfb screenshots across many bisection attempts
// (camera, lighting, indexed vs. non-indexed draw calls, PreferPerPixelLighting,
// single- vs. multi-effect frames) -- every one of which still rendered
// nothing until switching to this exact struct-plus-empty-declaration
// shape. Left as a confirmed real, unresolved discrepancy between the
// generic declaration-driven attribute path and the hardcoded stride-52
// path for this specific effect; not chased further into the backend
// internals given the disproportionate time already spent isolating it.
struct SkinnedGpuVertex {
    float px, py, pz;
    float nx, ny, nz;
    float u, v;
    float w0, w1, w2, w3;
    std::uint8_t i0, i1, i2, i3;
};
static_assert(sizeof(SkinnedGpuVertex) == 52, "skinned vertex must be 52 bytes");

// Demonstrates SkinnedEffect::SetBoneTransforms with a minimal 2-bone rig:
// a vertical hanging banner (2 columns x 2 rows, so real Y-extent is
// visible from a front-on camera) split at its middle column (the hinge,
// x=0) -- the left column is 100% weighted to bone 0 (BlendWeight=(1,0,0,0),
// BlendIndices={0,...}), the right column 100% to bone 1
// (BlendWeight=(1,0,0,0), BlendIndices={1,...} -- the single influence's
// weight always goes in slot 0 regardless of which bone it names, since
// WeightsPerVertex=1 only ever reads slot 0). Bone 0 stays identity (the
// root); bone 1 is a live Matrix::CreateRotationY(angle) applied every
// frame, swinging the right column back and forth about the shared hinge
// edge like a simple door/flag-pole joint.
class TwoBoneSkinningScreen : public DemoScreen {
public:
    TwoBoneSkinningScreen() : DemoScreen("SkinnedEffect: Two-Bone Bend") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        // Two quads (4 verts each, sharing the hinge edge's 2 verts logically
        // but duplicated here since this is a non-indexed triangle list) --
        // left quad x in [-1,0], right quad x in [0,1], both y in [-0.8,0.8].
        const SkinnedGpuVertex L_TL{-1, 0.8f, 0,  0,0,1,  0,0,  1,0,0,0,  0,0,0,0};
        const SkinnedGpuVertex L_BL{-1,-0.8f, 0,  0,0,1,  0,1,  1,0,0,0,  0,0,0,0};
        const SkinnedGpuVertex L_TR{ 0, 0.8f, 0,  0,0,1,  1,0,  1,0,0,0,  0,0,0,0};
        const SkinnedGpuVertex L_BR{ 0,-0.8f, 0,  0,0,1,  1,1,  1,0,0,0,  0,0,0,0};
        const SkinnedGpuVertex R_TL{ 0, 0.8f, 0,  0,0,1,  0,0,  1,0,0,0,  1,0,0,0};
        const SkinnedGpuVertex R_BL{ 0,-0.8f, 0,  0,0,1,  0,1,  1,0,0,0,  1,0,0,0};
        const SkinnedGpuVertex R_TR{ 1, 0.8f, 0,  0,0,1,  1,0,  1,0,0,0,  1,0,0,0};
        const SkinnedGpuVertex R_BR{ 1,-0.8f, 0,  0,0,1,  1,1,  1,0,0,0,  1,0,0,0};

        const SkinnedGpuVertex verts[12] = {
            L_TL, L_BL, L_TR,  L_TR, L_BL, L_BR,
            R_TL, R_BL, R_TR,  R_TR, R_BL, R_BR,
        };
        vertexCount_ = 12;
        vb_.emplace(device, vertexCount_);
        vb_->SetDataRaw(verts, vertexCount_, (int)sizeof(SkinnedGpuVertex));

        texture_.emplace(CnaExamples::Demos::Graphics2D::CreateCheckerboardTexture(
            device, 32, 32, 8, Color(255, 200, 60, 255), Color(60, 90, 220, 255)));
        effect_.emplace(device);
        effect_->setTextureProperty(&*texture_);
        effect_->EnableDefaultLighting();
        effect_->setWeightsPerVertexProperty(1);
    }

    void UnloadContent() override {
        effect_.reset();
        texture_.reset();
        vb_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        phase_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A 2-bone rig: left column weighted 100% to bone 0 (root, identity),");
        lines.push_back("right column 100% to bone 1 (a live-rotating hinge) -- SetBoneTransforms()");
        lines.push_back("swings the right half every frame, like a simple door/flag-pole joint.");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 3.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f)));
        effect_->setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f));

        const float hingeAngle = std::sin(phase_ * 1.2f) * 0.9f;
        effect_->SetBoneTransforms({Matrix::getIdentityProperty(), Matrix::CreateRotationY(hingeAngle)});

        device.SetVertexBuffer(&*vb_);
        effect_->Apply();
        device.DrawPrimitives(PrimitiveType::TriangleList, 0, vertexCount_ / 3);
        device.SetVertexBuffer(nullptr);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    std::optional<VertexBuffer> vb_;
    std::optional<Texture2D> texture_;
    std::optional<SkinnedEffect> effect_;
    int vertexCount_ = 0;
    float phase_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::SkinnedEffectDemos
