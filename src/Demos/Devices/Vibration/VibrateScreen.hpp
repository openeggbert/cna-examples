// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Devices/VibrateController.hpp"
#include "System/TimeSpan.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::VibrationDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Devices::VibrateController;
using System::TimeSpan;

// Demonstrates Microsoft::Devices::VibrateController -- the real XNA 4.0/
// Windows Phone 7 vibration API, reached via the single shared
// getDefaultProperty() instance. Space/Enter/A/tap pulses for 0.3s at full
// strength (the plain WP7 Start(TimeSpan) overload); gamepad X pulses the
// CNA-specific StartLeftRight(EXT) two-motor overload instead, so both
// shapes get exercised. Excludes any connected gamepad's own rumble motors
// by design (see the class doc comment) -- a silent no-op is expected on
// most desktops with no phone-like vibration hardware; test this for real
// on Android.
class VibrateScreen : public DemoScreen {
public:
    VibrateScreen() : DemoScreen("Vibration: VibrateController") {}

    void UnloadContent() override {
        VibrateController::getDefaultProperty()->Stop();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            VibrateController::getDefaultProperty()->Start(TimeSpan::FromSeconds(0.3));
            pulseCount_++;
        } else if (input.IsNewButtonPress(Buttons::X, ControllingPlayer(), playerIndex)) {
            VibrateController::getDefaultProperty()->StartLeftRight(1.0f, 0.4f, TimeSpan::FromSeconds(0.3));
            pulseCount_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        VibrateController* controller = VibrateController::getDefaultProperty();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: Start(0.3s). Gamepad X: StartLeftRight(EXT, 0.3s).");
        lines.emplace_back();
        lines.push_back(std::string("IsSupportedEXT: ") + (controller->getIsSupportedProperty() ? "true" : "false"));
        lines.push_back("DeviceNameEXT: " +
                        (controller->getDeviceNameProperty().empty() ? "(none)" : controller->getDeviceNameProperty()));
        lines.push_back("Pulses triggered: " + std::to_string(pulseCount_));
        lines.push_back("(a silent no-op on most desktops -- test this on Android)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int pulseCount_ = 0;
};

} // namespace CnaExamples::Demos::Devices::VibrationDemos
