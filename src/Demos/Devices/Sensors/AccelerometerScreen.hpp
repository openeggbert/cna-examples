// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Devices/Sensors/Accelerometer.hpp"
#include "Microsoft/Devices/Sensors/AccelerometerFailedException.hpp"
#include "Microsoft/Devices/Sensors/SensorState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::SensorsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Devices::Sensors::Accelerometer;
using Microsoft::Devices::Sensors::AccelerometerFailedException;
using Microsoft::Devices::Sensors::SensorState;

// Demonstrates Microsoft::Devices::Sensors::Accelerometer, the real XNA 4.0/
// Windows Phone 7 API (not a CNA extension): IsSupported, Start()/Stop(),
// State, and the live reading (Acceleration in g-forces). Desktop machines
// normally report IsSupported=false -- this is a "test on Android" demo,
// per the project owner's explicit ask, not a desktop-verifiable one.
class AccelerometerScreen : public DemoScreen {
public:
    AccelerometerScreen() : DemoScreen("Sensors: Accelerometer") {}

    void LoadContent() override {
        supported_ = Accelerometer::getIsSupportedProperty();
        if (!supported_) return;
        try {
            sensor_.emplace();
            sensor_->Start();
        } catch (const AccelerometerFailedException&) {
            startFailed_ = true;
        }
    }

    void UnloadContent() override {
        if (sensor_.has_value()) sensor_->Stop();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Accelerometer::IsSupported: " + std::string(supported_ ? "true" : "false"));

        if (!supported_) {
            lines.push_back("(not supported on this platform -- try this on Android)");
        } else if (startFailed_) {
            lines.push_back("Start() threw AccelerometerFailedException.");
        } else {
            lines.push_back(std::string("State: ") + StateName(sensor_->getStateProperty()));
            lines.push_back(std::string("IsDataValid: ") + (sensor_->getIsDataValidProperty() ? "true" : "false"));
            const auto& reading = sensor_->getCurrentValueProperty();
            const auto& a = reading.getAccelerationProperty();
            lines.push_back("Acceleration (g): (" + std::to_string(a.X) + ", " + std::to_string(a.Y) +
                            ", " + std::to_string(a.Z) + ")");
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
    std::optional<Accelerometer> sensor_;
};

} // namespace CnaExamples::Demos::Devices::SensorsDemos
