// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadDPad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadThumbSticks.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadState;

// Demonstrates GamePadState::DPad (four ButtonState values) and ThumbSticks
// (two Vector2, already dead-zone-processed with the default
// IndependentAxes mode -- see DeadZoneModesScreen for the raw/Circular
// comparison). Each stick's position is also drawn as a marker inside a
// small box, the analog equivalent of Mouse's cursor-following marker.
class DPadThumbsticksScreen : public DemoScreen {
public:
    DPadThumbsticksScreen() : DemoScreen("Gamepad: DPad & Thumbsticks") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const GamePadState state = GamePad::GetState(PlayerIndex::One);
        const auto& dpad = state.getDPadProperty();
        const auto& sticks = state.getThumbSticksProperty();

        std::vector<std::string> lines;
        lines.push_back("PlayerIndex::One's DPad and thumbsticks (IndependentAxes dead zone):");
        lines.emplace_back();
        AppendDPad(lines, "DPad Up", dpad.getUpProperty());
        AppendDPad(lines, "DPad Down", dpad.getDownProperty());
        AppendDPad(lines, "DPad Left", dpad.getLeftProperty());
        AppendDPad(lines, "DPad Right", dpad.getRightProperty());
        lines.emplace_back();
        lines.push_back("Left stick:  (" + std::to_string(sticks.getLeftProperty().X) + ", " +
                        std::to_string(sticks.getLeftProperty().Y) + ")");
        lines.push_back("Right stick: (" + std::to_string(sticks.getRightProperty().X) + ", " +
                        std::to_string(sticks.getRightProperty().Y) + ")");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        DrawStickBox(sb, font, Vector2(600.0f, 300.0f), "L", sticks.getLeftProperty());
        DrawStickBox(sb, font, Vector2(750.0f, 300.0f), "R", sticks.getRightProperty());
    }

private:
    static void AppendDPad(std::vector<std::string>& lines, const char* label, ButtonState state) {
        lines.push_back(std::string(label) + ": " +
                        (state == ButtonState::Pressed ? "Pressed" : "Released"));
    }

    // A fixed 80x80 box centered at `center`; the marker offsets within it
    // by up to +-40px per axis (XNA sticks: X right-positive, Y up-positive
    // -- so screen Y is inverted here since screen-down is positive).
    void DrawStickBox(SpriteBatch& sb, SpriteFont& font, Vector2 center, const char* label,
                      Vector2 stick) {
        sb.DrawString(font, label, Vector2(center.X - 40.0f, center.Y - 60.0f),
                     mul(Color(140, 140, 140), TransitionAlpha()));
        const Vector2 markerPos(center.X + stick.X * 40.0f - 4.0f,
                                center.Y - stick.Y * 40.0f - 8.0f);
        sb.DrawString(font, "+", markerPos, mul(Color::Yellow, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
