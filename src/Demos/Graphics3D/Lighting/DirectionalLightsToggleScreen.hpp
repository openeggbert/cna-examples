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

// Demonstrates BasicEffect's 3 independent DirectionalLight slots
// (DirectionalLight0/1/2), each a distinct color, each independently
// toggleable via keys 1/2/3 -- proving all 3 lights are genuinely
// separate, simultaneously-active light sources, not just one light with
// 3 property aliases.
class DirectionalLightsToggleScreen : public DemoScreen {
public:
    DirectionalLightsToggleScreen() : DemoScreen("BasicEffect: 3 Directional Lights") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeNormalTextureMesh(1.0f);
        texture_.emplace(CnaExamples::Demos::Graphics2D::CreateCheckerboardTexture(
            device, 64, 64, 16, Color(230, 230, 230, 255), Color(140, 140, 140, 255)));
        effect_.emplace(device);
        effect_->setTextureEnabledProperty(true);
        effect_->setTextureProperty(&*texture_);
        effect_->setLightingEnabledProperty(true);
        effect_->setAmbientLightColorProperty(Vector3(0.08f, 0.08f, 0.08f));

        effect_->DirectionalLight0.setDirectionProperty(Vector3(-1.0f, -0.3f, 0.0f));
        effect_->DirectionalLight0.setDiffuseColorProperty(Vector3(1.0f, 0.2f, 0.2f));
        effect_->DirectionalLight1.setDirectionProperty(Vector3(1.0f, -0.3f, 0.0f));
        effect_->DirectionalLight1.setDiffuseColorProperty(Vector3(0.2f, 1.0f, 0.2f));
        effect_->DirectionalLight2.setDirectionProperty(Vector3(0.0f, -1.0f, 0.3f));
        effect_->DirectionalLight2.setDiffuseColorProperty(Vector3(0.2f, 0.2f, 1.0f));
        ApplyEnabled();
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
        if (input.IsNewKeyPress(Keys::D1, ControllingPlayer(), playerIndex)) {
            light0On_ = !light0On_;
            ApplyEnabled();
        } else if (input.IsNewKeyPress(Keys::D2, ControllingPlayer(), playerIndex)) {
            light1On_ = !light1On_;
            ApplyEnabled();
        } else if (input.IsNewKeyPress(Keys::D3, ControllingPlayer(), playerIndex)) {
            light2On_ = !light2On_;
            ApplyEnabled();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Keys 1/2/3: toggle DirectionalLight0 (red), 1 (green), 2 (blue) --");
        lines.push_back("independently, proving 3 genuinely separate simultaneous light sources.");
        lines.push_back("Light0(red): " + std::string(light0On_ ? "on" : "off") +
                         "  Light1(green): " + std::string(light1On_ ? "on" : "off") +
                         "  Light2(blue): " + std::string(light2On_ ? "on" : "off"));
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
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
    void ApplyEnabled() {
        effect_->DirectionalLight0.setEnabledProperty(light0On_);
        effect_->DirectionalLight1.setEnabledProperty(light1On_);
        effect_->DirectionalLight2.setEnabledProperty(light2On_);
    }

    CubeNormalTextureMesh mesh_;
    std::optional<Texture2D> texture_;
    std::optional<BasicEffect> effect_;
    float spin_ = 0.0f;
    bool light0On_ = true;
    bool light1On_ = true;
    bool light2On_ = true;
};

} // namespace CnaExamples::Demos::Graphics3D::LightingDemos
