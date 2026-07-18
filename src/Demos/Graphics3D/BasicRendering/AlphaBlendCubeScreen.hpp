// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::BasicRenderingDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates BasicEffect::Alpha + BlendState::AlphaBlend on a 3D draw
// call: a translucent front cube (live-adjustable Alpha via Up/Down) drawn
// AFTER an opaque back cube at the same screen position, so the back
// cube's color visibly blends through the front one -- proving the
// translucency is real, not just a dimmer solid color.
//
// GraphicsDevice::BlendState restore discipline, confirmed by reading
// SpriteBatch::Begin()'s source directly: every sb.Begin() call
// (including the default no-arg overload, which passes BlendState::
// AlphaBlend) unconditionally calls setBlendStateProperty() -- so
// BlendState, like DepthStencilState, is automatically reset back to
// AlphaBlend the moment sb.Begin() runs again below. Restored explicitly
// anyway for clarity/defensiveness, matching this Area's established
// discipline for RasterizerState (which Begin() does NOT auto-reset).
class AlphaBlendCubeScreen : public DemoScreen {
public:
    AlphaBlendCubeScreen() : DemoScreen("BasicEffect: Alpha Blending") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        backMesh_ = BuildCubeColorMesh(1.0f, {
            Color(60, 220, 90, 255), Color(60, 220, 90, 255), Color(60, 220, 90, 255),
            Color(60, 220, 90, 255), Color(60, 220, 90, 255), Color(60, 220, 90, 255),
        });
        frontMesh_ = BuildCubeColorMesh(0.9f, {
            Color(220, 60, 60, 255), Color(220, 60, 60, 255), Color(220, 60, 60, 255),
            Color(220, 60, 60, 255), Color(220, 60, 60, 255), Color(220, 60, 60, 255),
        });
        backEffect_.emplace(device);
        backEffect_->VertexColorEnabled = true;
        frontEffect_.emplace(device);
        frontEffect_->VertexColorEnabled = true;
    }

    void UnloadContent() override {
        frontEffect_.reset();
        backEffect_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoInput(InputState& input) override {
        if (input.IsMenuUp(ControllingPlayer())) {
            alpha_ = std::min(1.0f, alpha_ + 0.02f);
        } else if (input.IsMenuDown(ControllingPlayer())) {
            alpha_ = std::max(0.0f, alpha_ - 0.02f);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Up/Down: adjust the front (red) cube's Alpha. The back (green) cube");
        lines.push_back("visibly blends through it -- proving real translucency, not a dim tint.");
        lines.push_back("Alpha: " + std::to_string(alpha_));
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);
        device.setBlendStateProperty(BlendState::AlphaBlend);

        const Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 1.5f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);
        const Matrix spin = Matrix::CreateRotationY(spin_);

        // Opaque back cube drawn first.
        backEffect_->World = Matrix::CreateTranslation(0.3f, 0.0f, -0.6f) * spin;
        backEffect_->View = view;
        backEffect_->Projection = projection;
        backEffect_->setAlphaProperty(1.0f);
        backEffect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, backMesh_.vertices.data(), 0,
                                         (int)backMesh_.vertices.size(), backMesh_.indices.data(), 0,
                                         (int)backMesh_.indices.size() / 3);

        // Translucent front cube drawn second, over the back one.
        frontEffect_->World = Matrix::CreateTranslation(-0.3f, 0.0f, 0.6f) * spin;
        frontEffect_->View = view;
        frontEffect_->Projection = projection;
        frontEffect_->setAlphaProperty(alpha_);
        frontEffect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, frontMesh_.vertices.data(), 0,
                                         (int)frontMesh_.vertices.size(), frontMesh_.indices.data(), 0,
                                         (int)frontMesh_.indices.size() / 3);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        device.setBlendStateProperty(BlendState::Opaque);
        sb.Begin();
    }

private:
    CubeColorMesh backMesh_;
    CubeColorMesh frontMesh_;
    std::optional<BasicEffect> backEffect_;
    std::optional<BasicEffect> frontEffect_;
    float spin_ = 0.0f;
    float alpha_ = 0.5f;
};

} // namespace CnaExamples::Demos::Graphics3D::BasicRenderingDemos
