// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/CullMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/FillMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::DepthAndCullingDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::CullMode;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::FillMode;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates RasterizerState.FillMode -- confirmed real and functional on
// this backend (not vestigial): a spinning cube toggles between Solid and
// WireFrame via Space/Enter/A. The 3 static RasterizerState presets
// (CullClockwise/CullCounterClockwise/CullNone) have no WireFrame variant,
// so a fresh RasterizerState is built by hand for the WireFrame case.
//
// CRITICAL, more so than any other screen in this Area: RasterizerState is
// NEVER touched by SpriteBatch::Begin() (confirmed by reading
// SpriteBatch.cpp), so forgetting to restore RasterizerState::
// CullCounterClockwise (the real Solid-fill XNA default) before returning
// from OnDemoDraw would leak WireFrame into every subsequent frame's 2D
// rendering too -- this screen's own Back-hint text, and every other
// screen/menu in the whole app after navigating away, would render as
// wireframe outlines instead of filled shapes. The restore below runs on
// every path out of OnDemoDraw (there is only one, at the end -- no early
// returns in this method).
class FillModeWireframeScreen : public DemoScreen {
public:
    FillModeWireframeScreen() : DemoScreen("RasterizerState: FillMode") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeColorMesh(1.0f, {
            Color(220, 60, 60, 255), Color(60, 220, 90, 255), Color(60, 110, 220, 255),
            Color(220, 200, 60, 255), Color(200, 60, 220, 255), Color(60, 220, 210, 255),
        });
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

    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            wireframe_ = !wireframe_;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A: RasterizerState.FillMode = " +
                         std::string(wireframe_ ? "WireFrame" : "Solid"));
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 170, original.getWidthProperty(), 400);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);

        if (wireframe_) {
            RasterizerState wireframeState;
            wireframeState.setCullModeProperty(CullMode::None);
            wireframeState.setFillModeProperty(FillMode::WireFrame);
            device.setRasterizerStateProperty(wireframeState);
        } else {
            device.setRasterizerStateProperty(RasterizerState::CullNone);
        }

        effect_->World = Matrix::CreateRotationY(spin_) * Matrix::CreateRotationX(spin_ * 0.6f);
        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 1.2f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                         (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                         (int)mesh_.indices.size() / 3);

        // Restore to the real Solid-fill XNA default -- see class doc comment on why this
        // matters more here than anywhere else in this Area.
        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    CubeColorMesh mesh_;
    std::optional<BasicEffect> effect_;
    float spin_ = 0.0f;
    bool wireframe_ = false;
};

} // namespace CnaExamples::Demos::Graphics3D::DepthAndCullingDemos
