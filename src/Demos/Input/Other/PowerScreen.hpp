// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/Power.hpp"
#include "CNA/Input/PowerState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::Power;
using CNA::Input::PowerStateEXT;

// Demonstrates the NOXNA/EXT CNA::Input::Power API: XNA 4.0 has no power
// API at all -- this is a pure CNA extension backed by SDL3, reporting
// whether the host machine is on battery, how much charge remains, and
// how many seconds of runtime are left (useful for pausing/dimming a game
// as the battery runs low). Distinct from Gamepad::GetPowerInfoEXT (a
// controller's own battery) and JoystickCapabilitiesEXT::powerState (a raw
// joystick's) -- this is the host computer/phone/tablet itself.
class PowerScreen : public DemoScreen {
public:
    PowerScreen() : DemoScreen("Other: Host Power/Battery (EXT)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        int secondsLeft = -1, percent = -1;
        const PowerStateEXT state = Power::GetInfoEXT(secondsLeft, percent);

        std::vector<std::string> lines;
        lines.push_back("Power::GetInfoEXT() for this host machine:");
        lines.emplace_back();
        lines.push_back(std::string("PowerStateEXT: ") + StateName(state));
        lines.push_back("Charge percent: " +
                        (percent < 0 ? std::string("(unknown)") : std::to_string(percent) + "%"));
        lines.push_back("Seconds remaining: " +
                        (secondsLeft < 0 ? std::string("(unknown)") : std::to_string(secondsLeft)));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* StateName(PowerStateEXT state) {
        switch (state) {
            case PowerStateEXT::Error: return "Error";
            case PowerStateEXT::Unknown: return "Unknown";
            case PowerStateEXT::OnBattery: return "OnBattery (discharging)";
            case PowerStateEXT::NoBattery: return "NoBattery (plugged in, no battery)";
            case PowerStateEXT::Charging: return "Charging";
            case PowerStateEXT::Charged: return "Charged (full)";
            default: return "?";
        }
    }
};

} // namespace CnaExamples::Demos::Input::OtherDemos
