// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::ViewportScissorDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates GraphicsDevice.Viewport -- shows this app's real, live
// X/Y/Width/Height/AspectRatio, then lets Space/Enter temporarily shrink
// the viewport to a 300x200 sub-rectangle. SpriteBatch sprite coordinates
// are independent of Viewport (it affects which sub-rectangle of the
// render target is actually written to, not sprite positions), so the
// visible effect of shrinking it is that anything drawn outside its new
// bounds is cropped -- including this screen's own title/Back-hint text,
// which is honestly left uncropped-looking-weird as real, first-hand proof
// rather than hidden. The original Viewport is restored on exit.
class ViewportInspectorScreen : public DemoScreen {
public:
    ViewportInspectorScreen() : DemoScreen("Viewport: Inspector") {}

    void LoadContent() override {
        original_ = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
    }

    void UnloadContent() override {
        GetScreenManager()->getGraphicsDeviceProperty().setViewportProperty(original_);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            shrunk_ = !shrunk_;
            auto& device = GetScreenManager()->getGraphicsDeviceProperty();
            device.setViewportProperty(shrunk_ ? Viewport(100, 100, 300, 200) : original_);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Viewport& current = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: toggle a 300x200 sub-viewport (crops render output).");
        lines.emplace_back();
        lines.push_back("X=" + std::to_string(current.getXProperty()) + " Y=" + std::to_string(current.getYProperty()) +
                         " Width=" + std::to_string(current.getWidthProperty()) +
                         " Height=" + std::to_string(current.getHeightProperty()));
        lines.push_back("AspectRatio=" + std::to_string(current.getAspectRatioProperty()));
        lines.push_back(shrunk_ ? "Shrunk -- everything outside 300x200 is now clipped." : "Original (full window).");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    Viewport original_;
    bool shrunk_ = false;
};

} // namespace CnaExamples::Demos::Graphics2D::ViewportScissorDemos
