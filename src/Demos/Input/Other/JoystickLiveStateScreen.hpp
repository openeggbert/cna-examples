// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/JoystickHatPosition.hpp"
#include "CNA/Input/JoystickInfo.hpp"
#include "CNA/Input/JoystickState.hpp"
#include "CNA/Input/Joysticks.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::JoystickHatPositionEXT;
using CNA::Input::JoystickInfoEXT;
using CNA::Input::JoystickStateEXT;
using CNA::Input::Joysticks;

// Demonstrates Joysticks::GetStateEXT(): raw, unmapped axes/buttons/hats/
// trackballs for the first enumerated joystick -- unlike GamePadState,
// nothing here has XNA-style semantic names (no "A button" or
// "LeftThumbstick"), since raw joysticks can be arbitrary HID devices with
// no standard layout.
class JoystickLiveStateScreen : public DemoScreen {
public:
    JoystickLiveStateScreen() : DemoScreen("Other: Joystick Live State (EXT)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const std::vector<JoystickInfoEXT> joysticks = Joysticks::GetJoysticksEXT();

        std::vector<std::string> lines;
        if (joysticks.empty()) {
            lines.push_back("No raw joystick connected. Connect one to see its live state here.");
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        const JoystickInfoEXT& first = joysticks.front();
        const JoystickStateEXT state = Joysticks::GetStateEXT(first.id);

        lines.push_back("First joystick: " + (first.name.empty() ? "(unnamed)" : first.name));
        lines.emplace_back();

        std::string axes = "Axes: ";
        for (std::size_t i = 0; i < state.axes.size(); ++i) {
            if (i) axes += ", ";
            axes += std::to_string(state.axes[i]);
        }
        lines.push_back(axes.size() > 6 ? axes : "Axes: (none)");

        std::string buttons = "Buttons: ";
        for (std::size_t i = 0; i < state.buttons.size(); ++i) {
            buttons += state.buttons[i] ? '1' : '0';
        }
        lines.push_back(state.buttons.empty() ? "Buttons: (none)" : buttons);

        std::string hats = "Hats: ";
        for (std::size_t i = 0; i < state.hats.size(); ++i) {
            if (i) hats += ", ";
            hats += HatName(state.hats[i]);
        }
        lines.push_back(state.hats.empty() ? "Hats: (none)" : hats);

        lines.push_back("Trackballs: " + std::to_string(state.balls.size()));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* HatName(JoystickHatPositionEXT pos) {
        switch (pos) {
            case JoystickHatPositionEXT::Up: return "Up";
            case JoystickHatPositionEXT::Right: return "Right";
            case JoystickHatPositionEXT::Down: return "Down";
            case JoystickHatPositionEXT::Left: return "Left";
            case JoystickHatPositionEXT::RightUp: return "RightUp";
            case JoystickHatPositionEXT::RightDown: return "RightDown";
            default: return "Centered/Other";
        }
    }
};

} // namespace CnaExamples::Demos::Input::OtherDemos
