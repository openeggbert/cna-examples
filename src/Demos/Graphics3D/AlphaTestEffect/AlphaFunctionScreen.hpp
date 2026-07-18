// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/AlphaTestEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/CompareFunction.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::AlphaTestEffectDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::AlphaTestEffect;
using Microsoft::Xna::Framework::Graphics::CompareFunction;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;

namespace detail_alpha_function {
// Same radial alpha-gradient texture as AlphaCutoffScreen.hpp -- kept as a
// separate local copy (not shared) since this Area's convention keeps each
// screen file self-contained; see that file for the technique explanation.
inline Texture2D CreateRadialAlphaTexture(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, int size) {
    std::vector<std::uint8_t> pixels((std::size_t)size * (std::size_t)size * 4);
    const float center = (float)(size - 1) / 2.0f;
    const float maxDist = center;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const float dx = (float)x - center;
            const float dy = (float)y - center;
            const float dist = std::sqrt(dx * dx + dy * dy);
            float t = 1.0f - (dist / maxDist);
            t = std::clamp(t, 0.0f, 1.0f);
            const std::size_t i = ((std::size_t)y * (std::size_t)size + (std::size_t)x) * 4;
            pixels[i + 0] = 60;
            pixels[i + 1] = 160;
            pixels[i + 2] = 255;
            pixels[i + 3] = (std::uint8_t)(t * 255.0f);
        }
    }
    return Texture2D::CreateFromPixels(device, size, size, pixels);
}
} // namespace detail_alpha_function

// Demonstrates AlphaTestEffect.AlphaFunction: the same radial alpha-gradient
// quad from AlphaCutoffScreen, cycling AlphaFunction through 4 real
// CompareFunction values via Space/Enter/A at a FIXED ReferenceAlpha=128,
// so the visual difference is purely the comparison operator, not the
// threshold: Greater (default -- keeps the opaque center, matches
// AlphaCutoffScreen's behavior), Less (inverts it -- keeps the transparent
// RING instead), GreaterEqual (same shape as Greater at this gradient's
// resolution, included to show it's a distinct real enum value), Always
// (keeps everything, alpha test effectively disabled).
class AlphaFunctionScreen : public DemoScreen {
public:
    AlphaFunctionScreen() : DemoScreen("AlphaTestEffect: AlphaFunction") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildQuadNormalTextureMesh(2.0f, 2.0f);
        texture_.emplace(detail_alpha_function::CreateRadialAlphaTexture(device, 128));
        effect_.emplace(device);
        effect_->setTextureProperty(&*texture_);
        effect_->setReferenceAlphaProperty(128);
        effect_->setAlphaFunctionProperty(functions_[index_]);
    }

    void UnloadContent() override {
        effect_.reset();
        texture_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            index_ = (index_ + 1) % (int)functions_.size();
            effect_->setAlphaFunctionProperty(functions_[index_]);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A: cycle AlphaFunction at a fixed ReferenceAlpha=128.");
        lines.push_back("Same gradient, different CompareFunction -- different discard rule.");
        lines.push_back("AlphaFunction: " + names_[index_]);
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 3.5f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f)));
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
    QuadNormalTextureMesh mesh_;
    std::optional<Texture2D> texture_;
    std::optional<AlphaTestEffect> effect_;
    std::array<CompareFunction, 4> functions_ = {
        CompareFunction::Greater, CompareFunction::Less, CompareFunction::GreaterEqual, CompareFunction::Always,
    };
    std::array<std::string, 4> names_ = {"Greater (default)", "Less", "GreaterEqual", "Always"};
    int index_ = 0;
};

} // namespace CnaExamples::Demos::Graphics3D::AlphaTestEffectDemos
