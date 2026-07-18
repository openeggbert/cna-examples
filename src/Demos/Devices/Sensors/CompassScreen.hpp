// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Devices/Sensors/CalibrationEventArgs.hpp"
#include "Microsoft/Devices/Sensors/Compass.hpp"
#include "Microsoft/Devices/Sensors/SensorFailedException.hpp"
#include "Microsoft/Devices/Sensors/SensorState.hpp"
#include "System/EventArgs.hpp"
#include "System/Object.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::SensorsDemos {

using namespace CnaExamples::GameStateManagement;
using System::Object;
using Microsoft::Devices::Sensors::CalibrationEventArgs;
using Microsoft::Devices::Sensors::Compass;
using Microsoft::Devices::Sensors::SensorFailedException;
using Microsoft::Devices::Sensors::SensorState;

// Demonstrates Microsoft::Devices::Sensors::Compass: IsSupported, Start()/
// Stop(), State, the live reading (MagneticHeading/TrueHeading/
// HeadingAccuracy/MagnetometerReading), and the Calibrate event (raised
// when the platform wants the user to move the device in a figure-8 to
// improve accuracy). Same "test on Android" caveat as Accelerometer/
// Gyroscope.
class CompassScreen : public DemoScreen {
public:
    CompassScreen() : DemoScreen("Sensors: Compass") {}

    void LoadContent() override {
        supported_ = Compass::getIsSupportedProperty();
        if (!supported_) return;
        try {
            sensor_.emplace();
            calibrateToken_ = sensor_->Calibrate.Add(
                [this](Object*, const CalibrationEventArgs&) { calibrateCount_++; });
            sensor_->Start();
        } catch (const SensorFailedException&) {
            startFailed_ = true;
        }
    }

    void UnloadContent() override {
        if (sensor_.has_value()) {
            sensor_->Calibrate.Remove(calibrateToken_);
            sensor_->Stop();
        }
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Compass::IsSupported: " + std::string(supported_ ? "true" : "false"));

        if (!supported_) {
            lines.push_back("(not supported on this platform -- try this on Android)");
        } else if (startFailed_) {
            lines.push_back("Start() threw SensorFailedException.");
        } else {
            lines.push_back(std::string("State: ") + StateName(sensor_->getStateProperty()));
            const auto& reading = sensor_->getCurrentValueProperty();
            lines.push_back("MagneticHeading: " + std::to_string(reading.getMagneticHeadingProperty()) + " deg");
            lines.push_back("TrueHeading: " + std::to_string(reading.getTrueHeadingProperty()) + " deg");
            lines.push_back("HeadingAccuracy: " + std::to_string(reading.getHeadingAccuracyProperty()) + " deg");
            lines.push_back("Calibrate event fired: " + std::to_string(calibrateCount_) + " time(s)");
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
    int calibrateCount_ = 0;
    std::size_t calibrateToken_ = 0;
    std::optional<Compass> sensor_;
};

} // namespace CnaExamples::Demos::Devices::SensorsDemos
