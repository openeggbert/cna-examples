// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::ScreenTransitionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::RenderTarget2D;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates the classic "render the scene into a RenderTarget2D, then
// draw that RT back over the backbuffer with an animated alpha" pattern
// used for screen-transition/post-effects (the same shape as
// ../cna-samples/samples/ReachGraphicsDemo/src/DemoGame.hpp's
// transitionRenderTarget_, reimplemented fresh here). Every frame, 3
// drifting quads are rendered into the RT, then the RT itself is drawn
// back with a continuously pulsing alpha -- a real, live-animated demo of
// using a render target as an intermediate compositing surface, not just a
// one-shot round trip.
class FadeTransitionScreen : public DemoScreen {
public:
    FadeTransitionScreen() : DemoScreen("RenderTarget2D: Fade Transition Effect") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        quad_.emplace(CreateCheckerboardTexture(device, 40, 40, 40, Color(255, 220, 60, 255), Color(255, 220, 60, 255)));
        target_.emplace(device, 400, 300);
    }

    void UnloadContent() override {
        quad_.reset();
        target_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        phase_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("3 drifting quads rendered into a RenderTarget2D every frame, then drawn");
        lines.push_back("back with a continuously pulsing alpha -- a live post-effect surface.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        // Close the base class's already-open default batch (title/intro
        // text already queued into it, targeting the backbuffer) before
        // switching render targets -- see BeginEndState/DeferredSortScreen
        // .hpp for the same suspend/reopen dance, used there for a
        // different reason.
        sb.End();

        // Render the drifting quads into the render target.
        device.SetRenderTarget(&*target_);
        device.Clear(Color(0, 0, 0, 0));
        sb.Begin();
        for (int i = 0; i < 3; ++i) {
            const float t = phase_ + (float)i * 2.0f;
            const float x = 180.0f + std::sin(t) * 140.0f;
            const float y = 130.0f + std::cos(t * 0.7f) * 90.0f;
            sb.Draw(*quad_, Vector2(x, y), Color::White);
        }
        sb.End();
        device.SetRenderTarget(nullptr);

        // Draw the composited render target back with a pulsing alpha.
        const float pulseAlpha = (std::sin(phase_ * 1.3f) + 1.0f) / 2.0f;
        sb.Begin();
        sb.Draw(*target_, Vector2(40.0f, end.Y + 20.0f), mul(Color::White, pulseAlpha * TransitionAlpha()));
        sb.End();

        sb.Begin(); // reopen a default batch for the base class's trailing Back hint
    }

private:
    std::optional<Texture2D> quad_;
    std::optional<RenderTarget2D> target_;
    float phase_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics2D::ScreenTransitionDemos
