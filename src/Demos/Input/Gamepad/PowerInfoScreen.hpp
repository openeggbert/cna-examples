// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/PowerState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::PowerStateEXT;
using Microsoft::Xna::Framework::Input::GamePad;

// Demonstrates the NOXNA/EXT GetPowerInfoEXT(): a controller's battery
// charge state, for wireless pads that report one (wired controllers and
// ones without a battery normally report NoBattery/Unknown, not an error).
class PowerInfoScreen : public DemoScreen {
public:
    PowerInfoScreen() : DemoScreen("Gamepad: Power Info (EXT)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        int percent = -1;
        const PowerStateEXT power = GamePad::GetPowerInfoEXT(PlayerIndex::One, percent);

        std::vector<std::string> lines;
        lines.push_back("PlayerIndex::One's battery/power state:");
        lines.emplace_back();
        lines.push_back(std::string("PowerStateEXT: ") + ToString(power));
        lines.push_back("Charge percent: " +
                        (percent < 0 ? std::string("(unknown)") : std::to_string(percent) + "%"));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* ToString(PowerStateEXT state) {
        switch (state) {
            case PowerStateEXT::Error: return "Error (controller disconnected)";
            case PowerStateEXT::Unknown: return "Unknown";
            case PowerStateEXT::OnBattery: return "OnBattery (discharging)";
            case PowerStateEXT::NoBattery: return "NoBattery (wired, no battery)";
            case PowerStateEXT::Charging: return "Charging";
            case PowerStateEXT::Charged: return "Charged (full)";
            default: return "?";
        }
    }
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
