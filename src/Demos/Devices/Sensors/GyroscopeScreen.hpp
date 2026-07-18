// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Devices/Sensors/Gyroscope.hpp"
#include "Microsoft/Devices/Sensors/SensorFailedException.hpp"
#include "Microsoft/Devices/Sensors/SensorState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::SensorsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Devices::Sensors::Gyroscope;
using Microsoft::Devices::Sensors::SensorFailedException;
using Microsoft::Devices::Sensors::SensorState;

// Demonstrates Microsoft::Devices::Sensors::Gyroscope: IsSupported, Start()/
// Stop(), State, and the live reading (RotationRate in radians/second). Same
// "test on Android" caveat as Accelerometer -- desktops normally report
// IsSupported=false.
class GyroscopeScreen : public DemoScreen {
public:
    GyroscopeScreen() : DemoScreen("Sensors: Gyroscope") {}

    void LoadContent() override {
        supported_ = Gyroscope::getIsSupportedProperty();
        if (!supported_) return;
        try {
            sensor_.emplace();
            sensor_->Start();
        } catch (const SensorFailedException&) {
            startFailed_ = true;
        }
    }

    void UnloadContent() override {
        if (sensor_.has_value()) sensor_->Stop();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Gyroscope::IsSupported: " + std::string(supported_ ? "true" : "false"));

        if (!supported_) {
            lines.push_back("(not supported on this platform -- try this on Android)");
        } else if (startFailed_) {
            lines.push_back("Start() threw SensorFailedException.");
        } else {
            lines.push_back(std::string("State: ") + StateName(sensor_->getStateProperty()));
            lines.push_back(std::string("IsDataValid: ") + (sensor_->getIsDataValidProperty() ? "true" : "false"));
            const auto& reading = sensor_->getCurrentValueProperty();
            const auto& r = reading.getRotationRateProperty();
            lines.push_back("RotationRate (rad/s): (" + std::to_string(r.X) + ", " + std::to_string(r.Y) +
                            ", " + std::to_string(r.Z) + ")");
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* StateName(SensorState state) {
        switch (state) {
            case SensorState::NotSupported: return "NotSupported";
            case SensorState::Ready: return "Ready";
            case SensorState::Initializing: return "Initializing";
            case SensorState::NoData: return "NoData";
            case SensorState::NoPermissions: return "NoPermissions";
            case SensorState::Disabled: return "Disabled";
            default: return "?";
        }
    }

    bool supported_ = false;
    bool startFailed_ = false;
    std::optional<Gyroscope> sensor_;
};

} // namespace CnaExamples::Demos::Devices::SensorsDemos
