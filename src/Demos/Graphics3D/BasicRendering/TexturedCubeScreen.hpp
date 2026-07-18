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

namespace CnaExamples::Demos::Graphics3D::BasicRenderingDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates BasicEffect::Texture being genuinely swappable at runtime
// (VertexTypes/PositionTextureScreen.hpp already covers the basic "one
// texture" case) -- Space/Enter/A cycles through 3 different procedurally
// generated textures (checkerboard/gradient/grid) on the SAME cube/effect
// instance, proving setTextureProperty() takes effect live, not just at
// LoadContent time.
class TexturedCubeScreen : public DemoScreen {
public:
    TexturedCubeScreen() : DemoScreen("BasicEffect: Live Texture Swap") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeTextureMesh(1.0f);

        textures_[0].emplace(CnaExamples::Demos::Graphics2D::CreateCheckerboardTexture(
            device, 64, 64, 16, Color(255, 200, 60, 255), Color(60, 90, 220, 255)));
        textures_[1].emplace(CnaExamples::Demos::Graphics2D::CreateGradientTexture(
            device, 64, 64, Color(255, 80, 80, 255), Color(80, 80, 255, 255)));
        textures_[2].emplace(CnaExamples::Demos::Graphics2D::CreateGridTexture(
            device, 4, 4, 16, {
                Color(220, 60, 60, 255), Color(60, 220, 90, 255), Color(60, 110, 220, 255),
                Color(220, 200, 60, 255),
            }));

        effect_.emplace(device);
        effect_->setTextureEnabledProperty(true);
        effect_->setTextureProperty(&*textures_[0]);
    }

    void UnloadContent() override {
        effect_.reset();
        textures_[0].reset();
        textures_[1].reset();
        textures_[2].reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            current_ = (current_ + 1) % 3;
            effect_->setTextureProperty(&*textures_[current_]);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        static const char* kNames[3] = {"Checkerboard", "Gradient", "Grid"};
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A: cycle BasicEffect.Texture -- 3 procedural textures on the");
        lines.push_back("SAME cube/effect, proving setTextureProperty() takes effect live.");
        lines.push_back(std::string("Current texture: ") + kNames[current_]);
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
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
    CubeTextureMesh mesh_;
    std::optional<Texture2D> textures_[3];
    std::optional<BasicEffect> effect_;
    int current_ = 0;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::BasicRenderingDemos
