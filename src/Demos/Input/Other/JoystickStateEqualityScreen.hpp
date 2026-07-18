// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/JoystickInfo.hpp"
#include "CNA/Input/JoystickState.hpp"
#include "CNA/Input/Joysticks.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::JoystickInfoEXT;
using CNA::Input::JoystickStateEXT;
using CNA::Input::Joysticks;

// Demonstrates JoystickStateEXT's free operator==/operator!= (a
// whole-struct field comparison, unlike GamePadState's Equals()/
// GetHashCode() member functions -- CNA's raw joystick types are plain
// NOXNA structs with only comparison operators, no XNA-style Equals
// surface, since they have no XNA precedent to match): freezes a snapshot
// of the first joystick's state (Space/Enter/A/tap), then compares it
// against the live state every frame.
class JoystickStateEqualityScreen : public DemoScreen {
public:
    JoystickStateEqualityScreen() : DemoScreen("Other: Joystick State ==/!= (EXT)") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            const auto joysticks = Joysticks::GetJoysticksEXT();
            if (!joysticks.empty()) {
                frozenId_ = joysticks.front().id;
                frozen_ = Joysticks::GetStateEXT(frozenId_);
                haveFrozen_ = true;
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap freezes the first joystick's state, if one is connected.");
        lines.emplace_back();

        if (!haveFrozen_) {
            lines.push_back("(no snapshot frozen yet -- connect a joystick and press Select)");
        } else {
            const JoystickStateEXT live = Joysticks::GetStateEXT(frozenId_);
            lines.push_back(std::string("live == frozen: ") + (live == frozen_ ? "true" : "false"));
            lines.push_back(std::string("live != frozen: ") + (live != frozen_ ? "true" : "false"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    JoystickStateEXT frozen_;
    std::uint32_t frozenId_ = 0;
    bool haveFrozen_ = false;
};

} // namespace CnaExamples::Demos::Input::OtherDemos
