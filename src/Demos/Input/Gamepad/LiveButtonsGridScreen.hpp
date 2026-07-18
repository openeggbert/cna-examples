// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadState;

// Demonstrates GamePadState::IsButtonDown(Buttons) on PlayerIndex::One's
// controller: a grid of every digital button that lights up while held,
// the same "press me" hardware-validation pattern as Keyboard's
// LiveKeyStateScreen. Requires a real controller in slot One to see
// anything change -- with none connected, GetState() always returns "up"
// for every button, which this screen shows honestly rather than faking.
class LiveButtonsGridScreen : public DemoScreen {
public:
    LiveButtonsGridScreen() : DemoScreen("Gamepad: Live Buttons Grid") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const GamePadState state = GamePad::GetState(PlayerIndex::One);

        std::vector<std::string> lines;
        lines.push_back(std::string("PlayerIndex::One: ") +
                        (state.getIsConnectedProperty() ? "connected" : "not connected"));
        lines.emplace_back();

        float y = 130.0f;
        for (const auto& row : kRows) {
            float x = 40.0f;
            for (const auto& btn : row) {
                const bool down = state.IsButtonDown(btn.button);
                sb.DrawString(font, btn.label, Vector2(x, y),
                             mul(down ? Color::Yellow : Color(90, 90, 90), TransitionAlpha()));
                x += btn.width;
            }
            y += (float)font.getLineSpacingProperty() + 14.0f;
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    struct GridButton {
        Buttons button;
        const char* label;
        float width;
    };

    static inline const std::vector<std::vector<GridButton>> kRows = {
        {{Buttons::DPadUp, "DUp", 80}, {Buttons::DPadDown, "DDown", 95},
         {Buttons::DPadLeft, "DLeft", 90}, {Buttons::DPadRight, "DRight", 95}},
        {{Buttons::A, "A", 40}, {Buttons::B, "B", 40}, {Buttons::X, "X", 40}, {Buttons::Y, "Y", 40},
         {Buttons::BigButton, "Big", 60}},
        {{Buttons::LeftShoulder, "LShoulder", 140}, {Buttons::RightShoulder, "RShoulder", 140}},
        {{Buttons::LeftStick, "LStickClick", 170}, {Buttons::RightStick, "RStickClick", 170}},
        {{Buttons::Start, "Start", 75}, {Buttons::Back, "Back", 75}},
    };
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
