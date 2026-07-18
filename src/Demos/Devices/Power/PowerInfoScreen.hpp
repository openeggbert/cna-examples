// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Devices/PowerInfo.hpp"
#include "CNA/Devices/PowerState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::PowerDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::PowerInfo;
using CNA::Devices::PowerState;

// Demonstrates CNA::Devices::PowerInfo -- the device's battery/power
// status, no XNA/WP7 equivalent. A close sibling of
// Input::Other::PowerScreen (CNA::Input::Power) -- both wrap SDL3's
// SDL_GetPowerInfo(), but through separate CNA::Devices and CNA::Input
// namespaces since they were added independently; shown here too since the
// Devices Area is the more natural home for "device status" queries, and
// this app aims to demonstrate every real API surface, including
// near-duplicates. Genuinely desktop-testable.
class PowerInfoScreen : public DemoScreen {
public:
    PowerInfoScreen() : DemoScreen("Power: PowerInfo") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back(std::string("PowerState: ") + StateName(PowerInfo::getStateProperty()));
        const int percent = PowerInfo::getBatteryPercentProperty();
        lines.push_back("BatteryPercent: " + (percent < 0 ? std::string("(unknown)") : std::to_string(percent) + "%"));
        const int seconds = PowerInfo::getSecondsRemainingProperty();
        lines.push_back("SecondsRemaining: " + (seconds < 0 ? std::string("(unknown)") : std::to_string(seconds)));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* StateName(PowerState state) {
        switch (state) {
            case PowerState::Error: return "Error";
            case PowerState::Unknown: return "Unknown";
            case PowerState::OnBattery: return "OnBattery";
            case PowerState::NoBattery: return "NoBattery";
            case PowerState::Charging: return "Charging";
            case PowerState::Charged: return "Charged";
            default: return "?";
        }
    }
};

} // namespace CnaExamples::Demos::Devices::PowerDemos
