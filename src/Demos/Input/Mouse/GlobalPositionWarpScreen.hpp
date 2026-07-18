// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Mouse;
using Microsoft::Xna::Framework::Input::MouseState;

// Demonstrates the NOXNA/EXT desktop-coordinate pair: GetGlobalPositionEXT()
// reads the cursor's position on the whole desktop (not just this window),
// and WarpGlobalEXT() moves it there -- useful for multi-window apps or
// tooling that SetPosition()'s window-relative coordinate can't express.
// Space/Enter/A/tap nudges the cursor 100px right in desktop space.
class GlobalPositionWarpScreen : public DemoScreen {
public:
    GlobalPositionWarpScreen() : DemoScreen("Mouse: Global Position & Warp (EXT)") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            int gx = 0, gy = 0;
            Mouse::GetGlobalPositionEXT(gx, gy);
            lastWarpSucceeded_ = Mouse::WarpGlobalEXT(gx + 100, gy);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        int gx = 0, gy = 0;
        Mouse::GetGlobalPositionEXT(gx, gy);
        const MouseState windowState = Mouse::GetState();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap nudges the cursor +100px right in desktop space.");
        lines.emplace_back();
        lines.push_back("Window-relative (GetState): (" + std::to_string(windowState.getXProperty()) +
                        ", " + std::to_string(windowState.getYProperty()) + ")");
        lines.push_back("Desktop-global (GetGlobalPositionEXT): (" + std::to_string(gx) + ", " +
                        std::to_string(gy) + ")");
        if (lastWarpSucceeded_.has_value()) {
            lines.push_back(std::string("Last WarpGlobalEXT() result: ") +
                            (*lastWarpSucceeded_ ? "true" : "false (platform doesn't support it)"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<bool> lastWarpSucceeded_;
};

} // namespace CnaExamples::Demos::Input::MouseDemos
