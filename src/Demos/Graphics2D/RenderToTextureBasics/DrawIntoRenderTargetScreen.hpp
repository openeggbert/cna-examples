// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::RenderToTextureBasicsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::RenderTarget2D;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates the classic render-to-texture pattern: a procedural source
// texture is drawn once into a RenderTarget2D, then every frame afterward
// the render target itself is drawn just like an ordinary Texture2D --
// RenderTarget2D publicly derives from Texture2D, so no special API is
// needed to sample it once you've switched back to the backbuffer. The
// render-into step needs its own Begin()/End() while the render target is
// bound; the base class's already-open default batch is suspended first
// and reopened afterward (see BeginEndState/DeferredSortScreen.hpp for the
// same End()/Begin() dance, used there for a different reason).
class DrawIntoRenderTargetScreen : public DemoScreen {
public:
    DrawIntoRenderTargetScreen() : DemoScreen("RenderTarget2D: Draw Into & Sample") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        source_.emplace(CreateCheckerboardTexture(device, 64, 64, 8,
                                                    Color(255, 140, 40, 255), Color(40, 140, 255, 255)));
        target_.emplace(device, 200, 150);

        // Render the source texture into the render target once -- its
        // content persists until something renders into it again. The
        // shared SpriteBatch is idle (not begun) at LoadContent() time, so
        // this Begin()/End() pair is self-contained and leaves it idle
        // again afterward, ready for this screen's own first Draw() call.
        SpriteBatch& sb = GetScreenManager()->getSpriteBatch();
        device.SetRenderTarget(&*target_);
        sb.Begin();
        sb.Draw(*source_, Vector2(0.0f, 0.0f), Color::White);
        sb.Draw(*source_, Vector2(120.0f, 70.0f), Color::White);
        sb.End();
        device.SetRenderTarget(nullptr);
    }

    void UnloadContent() override {
        source_.reset();
        target_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A source texture drawn once into a 200x150 RenderTarget2D, then");
        lines.push_back("sampled every frame like any other Texture2D (it IS one).");
        lines.push_back("RenderTarget2D size: " + std::to_string(target_->getWidthProperty()) + "x" +
                         std::to_string(target_->getHeightProperty()));
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.Draw(*target_, Vector2(60.0f, end.Y + 20.0f), mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Texture2D> source_;
    std::optional<RenderTarget2D> target_;
};

} // namespace CnaExamples::Demos::Graphics2D::RenderToTextureBasicsDemos
