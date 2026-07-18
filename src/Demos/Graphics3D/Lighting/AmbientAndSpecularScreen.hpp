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

namespace CnaExamples::Demos::Graphics3D::LightingDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates BasicEffect::AmbientLightColor (Up/Down cycles through 4
// preset brightness levels, brightening shadowed faces uniformly) and
// SpecularPower (Left/Right cycles through 4 preset shininess levels,
// tightening/widening the specular highlight) on a single rotating cube
// lit by one bright directional light with a white SpecularColor.
class AmbientAndSpecularScreen : public DemoScreen {
public:
    AmbientAndSpecularScreen() : DemoScreen("BasicEffect: Ambient & Specular") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeNormalTextureMesh(1.0f);
        texture_.emplace(CnaExamples::Demos::Graphics2D::CreateCheckerboardTexture(
            device, 64, 64, 16, Color(230, 230, 230, 255), Color(150, 150, 150, 255)));
        effect_.emplace(device);
        effect_->setTextureEnabledProperty(true);
        effect_->setTextureProperty(&*texture_);
        effect_->setLightingEnabledProperty(true);
        effect_->DirectionalLight0.setEnabledProperty(true);
        effect_->DirectionalLight0.setDirectionProperty(Vector3(-0.4f, -0.5f, -0.7f));
        effect_->DirectionalLight0.setDiffuseColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        effect_->DirectionalLight0.setSpecularColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        effect_->setSpecularColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        ApplyLevels();
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
        if (input.IsMenuUp(ControllingPlayer())) {
            ambientLevel_ = (ambientLevel_ + 1) % 4;
            ApplyLevels();
        } else if (input.IsMenuDown(ControllingPlayer())) {
            ambientLevel_ = (ambientLevel_ + 3) % 4;
            ApplyLevels();
        } else if (input.IsNewKeyPress(Keys::Right, ControllingPlayer(), playerIndex)) {
            specularLevel_ = (specularLevel_ + 1) % 4;
            ApplyLevels();
        } else if (input.IsNewKeyPress(Keys::Left, ControllingPlayer(), playerIndex)) {
            specularLevel_ = (specularLevel_ + 3) % 4;
            ApplyLevels();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        static const float kAmbientLevels[4] = {0.0f, 0.15f, 0.3f, 0.5f};
        static const float kSpecularPowers[4] = {4.0f, 16.0f, 64.0f, 256.0f};

        std::vector<std::string> lines;
        lines.push_back("Up/Down: AmbientLightColor brightness. Left/Right: SpecularPower.");
        lines.push_back("Ambient: " + std::to_string(kAmbientLevels[ambientLevel_]) +
                         "  SpecularPower: " + std::to_string((int)kSpecularPowers[specularLevel_]));
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 170, original.getWidthProperty(), 400);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->World = Matrix::CreateRotationY(spin_);
        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 1.0f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
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
    void ApplyLevels() {
        static const float kAmbientLevels[4] = {0.0f, 0.15f, 0.3f, 0.5f};
        static const float kSpecularPowers[4] = {4.0f, 16.0f, 64.0f, 256.0f};
        const float a = kAmbientLevels[ambientLevel_];
        effect_->setAmbientLightColorProperty(Vector3(a, a, a));
        effect_->setSpecularPowerProperty(kSpecularPowers[specularLevel_]);
    }

    CubeNormalTextureMesh mesh_;
    std::optional<Texture2D> texture_;
    std::optional<BasicEffect> effect_;
    float spin_ = 0.0f;
    int ambientLevel_ = 1;
    int specularLevel_ = 1;
};

} // namespace CnaExamples::Demos::Graphics3D::LightingDemos
