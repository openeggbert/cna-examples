// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::DepthAndCullingDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates RasterizerState::CullMode -- THE one screen in this Area
// that deliberately relies on real triangle winding (every other screen
// uses CullNone specifically to sidestep this). A single flat quad
// (BuildQuadNormalTextureMesh -- CCW winding as seen from its own +Z
// normal) spins continuously around Y, so both its front and back face
// the camera at different points in the rotation. Space/Enter/A cycles
// RasterizerState: CullNone (both sides always visible, a full 360 spin
// with no vanishing), CullCounterClockwiseFace (real XNA's default -- one
// side is culled), CullClockwiseFace (the other side is culled instead).
// Exact vanishing angle depends on winding-vs-camera geometry that's easy
// to get backwards reasoning about it in the abstract; the screen
// describes the mechanism honestly ("watch it disappear/reappear as it
// spins edge-on") rather than asserting a specific angle number.
class CullModeScreen : public DemoScreen {
public:
    CullModeScreen() : DemoScreen("RasterizerState: CullMode") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildQuadNormalTextureMesh(1.6f, 1.6f);
        texture_.emplace(CnaExamples::Demos::Graphics2D::CreateCheckerboardTexture(
            device, 64, 64, 16, Color(255, 200, 60, 255), Color(60, 90, 220, 255)));
        effect_.emplace(device);
        effect_->setTextureEnabledProperty(true);
        effect_->setTextureProperty(&*texture_);
    }

    void UnloadContent() override {
        effect_.reset();
        texture_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            mode_ = (mode_ + 1) % 3;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        static const char* kModeNames[3] = {"CullNone", "CullCounterClockwiseFace (default)",
                                             "CullClockwiseFace"};
        std::vector<std::string> lines;
        lines.push_back("A single quad spins around Y -- watch it vanish/reappear as it turns");
        lines.push_back("edge-on, depending on which side CullMode discards.");
        lines.push_back("Space/Enter/A: CullMode = " + std::string(kModeNames[mode_]));
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(mode_ == 0 ? RasterizerState::CullNone
                                          : mode_ == 1 ? RasterizerState::CullCounterClockwise
                                                       : RasterizerState::CullClockwise);

        effect_->World = Matrix::CreateRotationY(spin_);
        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
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
    QuadNormalTextureMesh mesh_;
    std::optional<Texture2D> texture_;
    std::optional<BasicEffect> effect_;
    float spin_ = 0.0f;
    int mode_ = 0;
};

} // namespace CnaExamples::Demos::Graphics3D::DepthAndCullingDemos
