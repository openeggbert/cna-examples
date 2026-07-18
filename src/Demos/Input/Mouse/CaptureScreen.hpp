// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Mouse.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Mouse;

// Demonstrates the NOXNA/EXT SetCaptureEXT(): while captured, mouse events
// keep being delivered even if the cursor leaves the window bounds (useful
// for click-drag interactions that shouldn't stop at the window edge).
// Space/Enter/A/tap toggles it; always released on exit (UnloadContent) so
// leaving this screen doesn't leave the whole app's mouse capture stuck on.
class CaptureScreen : public DemoScreen {
public:
    CaptureScreen() : DemoScreen("Mouse: Capture (EXT)") {}

    void UnloadContent() override {
        if (captured_) {
            Mouse::SetCaptureEXT(false);
            captured_ = false;
        }
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            lastResult_ = Mouse::SetCaptureEXT(!captured_);
            if (*lastResult_) captured_ = !captured_;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap toggles SetCaptureEXT(). Esc still works to go Back.");
        lines.emplace_back();
        lines.push_back(std::string("Captured: ") + (captured_ ? "yes" : "no"));
        if (lastResult_.has_value()) {
            lines.push_back(std::string("Last SetCaptureEXT() result: ") +
                            (*lastResult_ ? "true" : "false (platform doesn't support capture)"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    bool captured_ = false;
    std::optional<bool> lastResult_;
};

} // namespace CnaExamples::Demos::Input::MouseDemos
