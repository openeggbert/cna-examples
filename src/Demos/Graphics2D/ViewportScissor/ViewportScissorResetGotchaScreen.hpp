// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::ViewportScissorDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates a real, EasyGL-test-confirmed gotcha from this app's own
// research: GraphicsDevice::SetRenderTarget()/SetRenderTargets() RESETS
// Viewport and ScissorRectangle back to (0,0,width,height) of the new
// target EVERY time it's called -- a custom Viewport/ScissorRectangle set
// beforehand does NOT survive the switch. No RenderTarget2D is needed to
// show this: SetRenderTarget(nullptr) (switch to the backbuffer) is real,
// always-available, and triggers the exact same reset logic.
class ViewportScissorResetGotchaScreen : public DemoScreen {
public:
    ViewportScissorResetGotchaScreen() : DemoScreen("Viewport: Reset on RT Switch") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            auto& device = GetScreenManager()->getGraphicsDeviceProperty();
            device.setViewportProperty(Viewport(10, 10, 50, 50));
            device.setScissorRectangleProperty(Rectangle(10, 10, 50, 50));
            beforeViewport_ = "Viewport before: X=10 Y=10 Width=50 Height=50 (just set)";
            beforeScissor_ = "ScissorRectangle before: X=10 Y=10 Width=50 Height=50 (just set)";

            device.SetRenderTarget(nullptr); // real, safe, always-valid -- triggers the reset

            const auto& v = device.getViewportProperty();
            const auto s = device.getScissorRectangleProperty();
            afterViewport_ = "Viewport after SetRenderTarget(nullptr): X=" + std::to_string(v.getXProperty()) +
                              " Y=" + std::to_string(v.getYProperty()) + " Width=" + std::to_string(v.getWidthProperty()) +
                              " Height=" + std::to_string(v.getHeightProperty());
            afterScissor_ = "ScissorRectangle after: X=" + std::to_string(s.X) + " Y=" + std::to_string(s.Y) +
                             " Width=" + std::to_string(s.Width) + " Height=" + std::to_string(s.Height);
            attempts_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: set a tiny custom Viewport+Scissor, then");
        lines.push_back("SetRenderTarget(nullptr) (switch to backbuffer) -- watch them reset.");
        lines.emplace_back();
        lines.push_back("Attempts: " + std::to_string(attempts_));
        if (!afterViewport_.empty()) {
            lines.push_back(beforeViewport_);
            lines.push_back(beforeScissor_);
            lines.push_back(afterViewport_);
            lines.push_back(afterScissor_);
            lines.push_back("(both silently reset to the full backbuffer, never asked to be)");
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int attempts_ = 0;
    std::string beforeViewport_;
    std::string beforeScissor_;
    std::string afterViewport_;
    std::string afterScissor_;
};

} // namespace CnaExamples::Demos::Graphics2D::ViewportScissorDemos
