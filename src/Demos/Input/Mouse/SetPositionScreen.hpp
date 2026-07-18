// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Mouse;
using Microsoft::Xna::Framework::Input::MouseState;

// Demonstrates the write side of the Mouse API: SetPosition(x, y) moves the
// cursor to a window-relative coordinate. Space/Enter/A/tap cycles through a
// few fixed target points (center, corners) so the jump is obvious.
class SetPositionScreen : public DemoScreen {
public:
    SetPositionScreen() : DemoScreen("Mouse: SetPosition()") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            targetIndex_ = (targetIndex_ + 1) % kTargetCount;
            auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
            const int w = viewport.getWidthProperty();
            const int h = viewport.getHeightProperty();
            int x = 0, y = 0;
            switch (targetIndex_) {
                case 0: x = w / 2; y = h / 2; break;         // center
                case 1: x = 40;    y = 40;    break;         // top-left
                case 2: x = w - 40; y = 40;    break;        // top-right
                case 3: x = 40;    y = h - 40; break;        // bottom-left
                case 4: x = w - 40; y = h - 40; break;       // bottom-right
                default: break;
            }
            Mouse::SetPosition(x, y);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const MouseState state = Mouse::GetState();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap warps the cursor: center -> corners, in a loop.");
        lines.emplace_back();
        lines.push_back("Cursor position now: (" + std::to_string(state.getXProperty()) + ", " +
                        std::to_string(state.getYProperty()) + ")");
        lines.push_back(std::string("Last target: ") +
                        (targetIndex_ < 0 ? "(none yet)" : kTargetNames[targetIndex_]));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr int kTargetCount = 5;
    static constexpr const char* kTargetNames[kTargetCount] = {
        "center", "top-left", "top-right", "bottom-left", "bottom-right"};

    int targetIndex_ = -1; // no target warped to yet; first Select lands on index 0 ("center")
};

} // namespace CnaExamples::Demos::Input::MouseDemos
