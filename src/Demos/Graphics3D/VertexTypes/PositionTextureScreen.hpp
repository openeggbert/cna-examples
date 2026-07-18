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

namespace CnaExamples::Demos::Graphics3D::VertexTypesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates VertexPositionTexture (Position + UV, no color, no normal) --
// an unlit textured cube. Same 6 faces as PositionColorScreen, this time
// each face samples a procedurally-generated checkerboard via
// BasicEffect.TextureEnabled instead of vertex color.
class PositionTextureScreen : public DemoScreen {
public:
    PositionTextureScreen() : DemoScreen("VertexPositionTexture: Textured Cube") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeTextureMesh(1.0f);
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

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("VertexPositionTexture: Position + UV only, no color/normal -- unlit.");
        lines.push_back("Each face samples a procedural checkerboard (BasicEffect.TextureEnabled).");
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
    CubeTextureMesh mesh_;
    std::optional<Texture2D> texture_;
    std::optional<BasicEffect> effect_;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::VertexTypesDemos
