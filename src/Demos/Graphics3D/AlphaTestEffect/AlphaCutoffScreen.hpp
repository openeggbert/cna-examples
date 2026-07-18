// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/AlphaTestEffect.hpp"
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
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Builds a radial gradient texture whose ALPHA channel fades from fully
// opaque at the center to fully transparent at the edges (RGB stays a
// fixed orange so the alpha test's effect on coverage, not color, is what's
// being demonstrated). Same raw-RGBA + Texture2D::CreateFromPixels
// technique as Graphics2D/TextureDemoHelpers.hpp, just with a varying
// alpha channel instead of a fixed 255 -- kept local to this file since
// it's the only screen in this Area that needs a non-uniform alpha.
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
            pixels[i + 0] = 255;
            pixels[i + 1] = 160;
            pixels[i + 2] = 40;
            pixels[i + 3] = (std::uint8_t)(t * 255.0f);
        }
    }
    return Texture2D::CreateFromPixels(device, size, size, pixels);
}

// Demonstrates AlphaTestEffect.ReferenceAlpha: a quad textured with a
// radial alpha gradient (opaque center, transparent edge). Fragments whose
// alpha fails `alpha > ReferenceAlpha/255` (the default AlphaFunction,
// CompareFunction::Greater) are discarded by the GPU, so raising
// ReferenceAlpha shrinks the visible opaque disc live.
class AlphaCutoffScreen : public DemoScreen {
public:
    AlphaCutoffScreen() : DemoScreen("AlphaTestEffect: Reference Alpha Cutoff") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildQuadNormalTextureMesh(2.0f, 2.0f);
        texture_.emplace(CreateRadialAlphaTexture(device, 128));
        effect_.emplace(device);
        effect_->setTextureProperty(&*texture_);
        effect_->setReferenceAlphaProperty(referenceAlpha_);
    }

    void UnloadContent() override {
        effect_.reset();
        texture_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        (void)playerIndex;
        if (input.IsNewKeyPress(Keys::Up, ControllingPlayer(), playerIndex)) {
            referenceAlpha_ = std::min(255, referenceAlpha_ + 32);
            effect_->setReferenceAlphaProperty(referenceAlpha_);
        } else if (input.IsNewKeyPress(Keys::Down, ControllingPlayer(), playerIndex)) {
            referenceAlpha_ = std::max(0, referenceAlpha_ - 32);
            effect_->setReferenceAlphaProperty(referenceAlpha_);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Up/Down: ReferenceAlpha += 32 (0-255). Fragments failing");
        lines.push_back("alpha > ReferenceAlpha/255 are GPU-discarded, not blended.");
        lines.push_back("ReferenceAlpha: " + std::to_string(referenceAlpha_));
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
    int referenceAlpha_ = 96;
};

} // namespace CnaExamples::Demos::Graphics3D::AlphaTestEffectDemos
