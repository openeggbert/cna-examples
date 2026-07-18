// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/CubeMapFace.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EnvironmentMapEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCube.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::EnvironmentMapEffectDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::CubeMapFace;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::EnvironmentMapEffect;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
using Microsoft::Xna::Framework::Graphics::TextureCube;
using Microsoft::Xna::Framework::Graphics::Viewport;

namespace detail_env_tuning {
// Same 6-distinct-color procedural cube map technique as
// ReflectiveCubeScreen.hpp -- kept as a separate local copy per this Area's
// self-contained-file convention.
//
// Fills `out` in place via emplace() rather than returning a TextureCube by
// value -- TextureCube.hpp defaults its move constructor inline against a
// forward-declared backend type (ITextureCubeBackend), which only compiles
// where the complete backend type is visible (inside the cna library's own
// translation units); a by-value return from application code requires
// that same defaulted move constructor and fails to compile here with an
// "invalid application of sizeof to incomplete type" error.
inline void CreateSixColorEnvironmentMap(GraphicsDevice& device, int size, std::optional<TextureCube>& out) {
    out.emplace(device, size, false, SurfaceFormat::Color);
    const Color faceColors[6] = {
        Color(255, 60, 60, 255),  Color(60, 255, 90, 255),  Color(60, 110, 255, 255),
        Color(255, 220, 60, 255), Color(220, 60, 255, 255), Color(60, 230, 220, 255),
    };
    const CubeMapFace faces[6] = {
        CubeMapFace::PositiveX, CubeMapFace::NegativeX, CubeMapFace::PositiveY,
        CubeMapFace::NegativeY, CubeMapFace::PositiveZ, CubeMapFace::NegativeZ,
    };
    std::vector<Color> facePixels((std::size_t)size * (std::size_t)size, Color::White);
    for (int f = 0; f < 6; ++f) {
        std::fill(facePixels.begin(), facePixels.end(), faceColors[f]);
        out->SetData(faces[f], facePixels.data(), (int)facePixels.size());
    }
}
} // namespace detail_env_tuning

// Demonstrates EnvironmentMapEffect.EnvironmentMapAmount (blend between the
// diffuse material color and the cube-map reflection) and .FresnelFactor
// (how much the reflection intensifies at oblique viewing angles), both
// live-adjustable so their independent effects are provable: Up/Down
// changes EnvironmentMapAmount, Left/Right changes FresnelFactor.
class EnvironmentMapTuningScreen : public DemoScreen {
public:
    EnvironmentMapTuningScreen() : DemoScreen("EnvironmentMapEffect: Amount & Fresnel") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeNormalTextureMesh(1.0f);
        detail_env_tuning::CreateSixColorEnvironmentMap(device, 8, envMap_);
        effect_.emplace(device);
        effect_->setEnvironmentMapProperty(&*envMap_);
        effect_->setDiffuseColorProperty(Vector3(0.6f, 0.6f, 0.6f));
        effect_->setEnvironmentMapAmountProperty(amount_);
        effect_->setFresnelFactorProperty(fresnel_);
    }

    void UnloadContent() override {
        effect_.reset();
        envMap_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsNewKeyPress(Keys::Up, ControllingPlayer(), playerIndex)) {
            amount_ = std::min(1.0f, amount_ + 0.1f);
            effect_->setEnvironmentMapAmountProperty(amount_);
        } else if (input.IsNewKeyPress(Keys::Down, ControllingPlayer(), playerIndex)) {
            amount_ = std::max(0.0f, amount_ - 0.1f);
            effect_->setEnvironmentMapAmountProperty(amount_);
        } else if (input.IsNewKeyPress(Keys::Right, ControllingPlayer(), playerIndex)) {
            fresnel_ = std::min(4.0f, fresnel_ + 0.5f);
            effect_->setFresnelFactorProperty(fresnel_);
        } else if (input.IsNewKeyPress(Keys::Left, ControllingPlayer(), playerIndex)) {
            fresnel_ = std::max(0.0f, fresnel_ - 0.5f);
            effect_->setFresnelFactorProperty(fresnel_);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Up/Down: EnvironmentMapAmount +-0.1. Left/Right: FresnelFactor +-0.5.");
        lines.push_back("Amount: " + std::to_string(amount_) + "  Fresnel: " + std::to_string(fresnel_));
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
    CubeNormalTextureMesh mesh_;
    std::optional<TextureCube> envMap_;
    std::optional<EnvironmentMapEffect> effect_;
    float spin_ = 0.0f;
    float amount_ = 0.5f;
    float fresnel_ = 1.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::EnvironmentMapEffectDemos
