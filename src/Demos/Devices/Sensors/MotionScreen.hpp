// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Devices/Sensors/Motion.hpp"
#include "Microsoft/Devices/Sensors/SensorFailedException.hpp"
#include "Microsoft/Devices/Sensors/SensorState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::SensorsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Devices::Sensors::Motion;
using Microsoft::Devices::Sensors::SensorFailedException;
using Microsoft::Devices::Sensors::SensorState;

// Demonstrates Microsoft::Devices::Sensors::Motion: a fused sensor
// combining Attitude (Pitch/Roll/Yaw, from accelerometer+gyroscope+
// magnetometer sensor fusion), DeviceAcceleration, DeviceRotationRate, and
// Gravity into one reading -- richer than polling Accelerometer/Gyroscope/
// Compass separately. Same "test on Android" caveat as the other sensors.
class MotionScreen : public DemoScreen {
public:
    MotionScreen() : DemoScreen("Sensors: Motion (fused)") {}

    void LoadContent() override {
        supported_ = Motion::getIsSupportedProperty();
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
        lines.push_back("Motion::IsSupported: " + std::string(supported_ ? "true" : "false"));

        if (!supported_) {
            lines.push_back("(not supported on this platform -- try this on Android)");
        } else if (startFailed_) {
            lines.push_back("Start() threw SensorFailedException.");
        } else {
            lines.push_back(std::string("State: ") + StateName(sensor_->getStateProperty()));
            const auto& reading = sensor_->getCurrentValueProperty();
            const auto& attitude = reading.getAttitudeProperty();
            lines.push_back("Attitude Pitch/Roll/Yaw: " + std::to_string(attitude.getPitchProperty()) + " / " +
                            std::to_string(attitude.getRollProperty()) + " / " +
                            std::to_string(attitude.getYawProperty()));
            const auto& accel = reading.getDeviceAccelerationProperty();
            lines.push_back("DeviceAcceleration: (" + std::to_string(accel.X) + ", " +
                            std::to_string(accel.Y) + ", " + std::to_string(accel.Z) + ")");
            const auto& rot = reading.getDeviceRotationRateProperty();
            lines.push_back("DeviceRotationRate: (" + std::to_string(rot.X) + ", " +
                            std::to_string(rot.Y) + ", " + std::to_string(rot.Z) + ")");
            const auto& gravity = reading.getGravityProperty();
            lines.push_back("Gravity: (" + std::to_string(gravity.X) + ", " +
                            std::to_string(gravity.Y) + ", " + std::to_string(gravity.Z) + ")");
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
    std::optional<Motion> sensor_;
};

} // namespace CnaExamples::Demos::Devices::SensorsDemos
