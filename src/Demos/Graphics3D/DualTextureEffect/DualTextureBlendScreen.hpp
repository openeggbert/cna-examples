// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DualTextureEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::DualTextureEffectDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::DualTextureEffect;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates DualTextureEffect's 2 simultaneous texture layers (Texture +
// Texture2, both real GPU samplers blended per-pixel by the actual
// backend shader -- not composited on the CPU first): a checkerboard base
// layer combined with a gradient second layer on the same cube, so the
// result is genuinely darker/lighter across the gradient's direction while
// still showing the checkerboard pattern underneath.
class DualTextureBlendScreen : public DemoScreen {
public:
    DualTextureBlendScreen() : DemoScreen("DualTextureEffect: 2-Layer Blend") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeTextureMesh(1.0f);
        baseTexture_.emplace(CnaExamples::Demos::Graphics2D::CreateCheckerboardTexture(
            device, 64, 64, 8, Color(255, 255, 255, 255), Color(80, 80, 80, 255)));
        secondTexture_.emplace(CnaExamples::Demos::Graphics2D::CreateGradientTexture(
            device, 64, 64, Color(255, 60, 60, 255), Color(60, 90, 255, 255)));
        effect_.emplace(device);
        effect_->setTextureProperty(&*baseTexture_);
        effect_->setTexture2Property(&*secondTexture_);
    }

    void UnloadContent() override {
        effect_.reset();
        secondTexture_.reset();
        baseTexture_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("DualTextureEffect.Texture (checkerboard) + .Texture2 (red-to-blue");
        lines.push_back("gradient) -- both real GPU samplers, blended per-pixel by the shader.");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 170, original.getWidthProperty(), 400);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->setWorldProperty(Matrix::CreateRotationY(spin_));
        effect_->setViewProperty(Matrix::CreateLookAt(Vector3(0.0f, 1.2f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f)));
        effect_->setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f));

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
    std::optional<Texture2D> baseTexture_;
    std::optional<Texture2D> secondTexture_;
    std::optional<DualTextureEffect> effect_;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::DualTextureEffectDemos
