// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::VertexTypesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates VertexPositionColor -- the simplest 3D vertex type (Position +
// Color only, no normal/UV) -- via a spinning 6-color cube drawn with
// DrawUserIndexedPrimitives and BasicEffect.VertexColorEnabled. This is the
// FIRST 3D Graphics screen and establishes the pattern every other screen in
// this Area reuses:
//
// 1. sb.End() BEFORE touching any GraphicsDevice state (Viewport/
//    DepthStencilState/RasterizerState) -- SpriteBatch's default Deferred
//    sort mode only rasterizes queued sprites (here: this screen's title,
//    already queued by the base class before OnDemoDraw ran) at End() time,
//    so changing device state first would retroactively corrupt it (the
//    exact class of bug found and fixed in 2D Graphics'
//    ViewportScissor/ScissorClippingScreen.hpp).
// 2. A restricted Viewport for the 3D content only (below the title text,
//    above the Back hint) -- otherwise 3D draws would overwrite the
//    already-rendered title pixels.
// 3. DepthStencilState::Default (depth test+write ON) and
//    RasterizerState::CullNone (both faces always visible, so a geometry
//    winding mistake never looks like "missing geometry" -- only the
//    dedicated CullMode demo in DepthAndCulling relies on real winding).
// 4. Draw via effect.Apply() (confirmed the real, tested pattern for a
//    direct custom draw call -- see cna/examples/
//    easygl_basiceffect_combinations_test.cpp; EffectPass::Apply() just
//    forwards to the same Effect::Apply() internally, so looping
//    CurrentTechnique's Passes -- the idiom ModelMesh::Draw() uses
//    internally for multi-part models -- is unnecessary ceremony here),
//    then a typed DrawUserIndexedPrimitives overload (never the raw void*
//    overload, which assumes a hardcoded packed stride, nor the
//    VertexDeclaration-based overload with a struct whose stride doesn't
//    match a backend-recognized layout -- both are confirmed silent-
//    corruption traps, not just theoretical ones).
// 5. Restore Viewport/RasterizerState/DepthStencilState, THEN sb.Begin()
//    again so the base class's trailing Back-hint text renders normally.
//    RasterizerState in particular is NEVER touched by SpriteBatch::Begin()
//    (confirmed by reading SpriteBatch.cpp) -- forgetting to restore it
//    would leak (e.g. WireFrame) into every future frame, this screen's own
//    Back-hint text included.
class PositionColorScreen : public DemoScreen {
public:
    PositionColorScreen() : DemoScreen("VertexPositionColor: Colored Cube") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const std::array<Color, 6> faceColors = {
            Color(220, 60, 60, 255), Color(60, 220, 90, 255), Color(60, 110, 220, 255),
            Color(220, 200, 60, 255), Color(200, 60, 220, 255), Color(60, 220, 210, 255),
        };
        mesh_ = BuildCubeColorMesh(1.0f, faceColors);
        effect_.emplace(device);
        effect_->VertexColorEnabled = true;
    }

    void UnloadContent() override {
        effect_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("VertexPositionColor: Position + Color only, no normal/UV -- unlit.");
        lines.push_back("Each of the 6 faces is a flat color (BasicEffect.VertexColorEnabled).");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 170, original.getWidthProperty(), 400);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->World = Matrix::CreateRotationY(spin_) * Matrix::CreateRotationX(spin_ * 0.6f);
        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 1.5f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                         (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                         (int)mesh_.indices.size() / 3);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    CubeColorMesh mesh_;
    std::optional<BasicEffect> effect_;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::VertexTypesDemos
