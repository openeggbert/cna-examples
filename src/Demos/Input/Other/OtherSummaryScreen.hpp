// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/Haptics.hpp"
#include "CNA/Input/InputDevices.hpp"
#include "CNA/Input/Joysticks.hpp"
#include "CNA/Input/Power.hpp"
#include "CNA/Input/PowerState.hpp"
#include "CNA/Input/Sensors.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::Haptics;
using CNA::Input::InputDevices;
using CNA::Input::Joysticks;
using CNA::Input::Power;
using CNA::Input::PowerStateEXT;
using CNA::Input::Sensors;

// A one-screen "at a glance" dashboard summarizing every subsystem this
// Other category covers -- joysticks, sensors, power, haptics, and device
// enumeration -- each as a single count/state line, tying the category
// together the way Gamepad's DeviceInfoScreen consolidates several minor
// getters into one sheet, but here across subsystems rather than within
// one. Each line links to the dedicated demo that covers it in more depth.
class OtherSummaryScreen : public DemoScreen {
public:
    OtherSummaryScreen() : DemoScreen("Other: Summary") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        int secondsLeft = -1, percent = -1;
        const PowerStateEXT power = Power::GetInfoEXT(secondsLeft, percent);

        std::vector<std::string> lines;
        lines.push_back("Joysticks connected: " + std::to_string(Joysticks::GetJoysticksEXT().size()));
        lines.push_back("Host sensors: " + std::to_string(Sensors::GetSensorsEXT().size()));
        lines.push_back(std::string("Host power state: ") + StateName(power));
        lines.push_back("Haptic devices: " + std::to_string(Haptics::GetHapticsEXT().size()));
        lines.push_back("  + mouse haptic: " + std::string(Haptics::IsMouseHapticEXT() ? "yes" : "no"));
        lines.push_back("Enumerated mice: " + std::to_string(InputDevices::GetMiceEXT().size()));
        lines.push_back("Enumerated keyboards: " + std::to_string(InputDevices::GetKeyboardsEXT().size()));
        lines.push_back("Enumerated touch devices: " + std::to_string(InputDevices::GetTouchDevicesEXT().size()));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* StateName(PowerStateEXT state) {
        switch (state) {
            case PowerStateEXT::Error: return "Error";
            case PowerStateEXT::Unknown: return "Unknown";
            case PowerStateEXT::OnBattery: return "OnBattery";
            case PowerStateEXT::NoBattery: return "NoBattery";
            case PowerStateEXT::Charging: return "Charging";
            case PowerStateEXT::Charged: return "Charged";
            default: return "?";
        }
    }
};

} // namespace CnaExamples::Demos::Input::OtherDemos
