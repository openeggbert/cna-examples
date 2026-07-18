// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/Sensors.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::SensorInfoEXT;
using CNA::Input::Sensors;

// Demonstrates the NOXNA/EXT CNA::Input::Sensors API: the host device's own
// motion sensors (phone/tablet/laptop accelerometer and gyroscope) --
// distinct from a gamepad's Get{Gyro,Accelerometer}EXT (see Gamepad's
// DeviceInfo-adjacent demos) and from Microsoft::Devices::Sensors (the
// official XNA 4.0 Windows Phone sensor API, which belongs conceptually to
// the separate Devices area, not here). Mainly a mobile feature -- expect
// an empty enumeration and false return values on most desktops.
class SensorsScreen : public DemoScreen {
public:
    SensorsScreen() : DemoScreen("Other: Host Device Sensors (EXT)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const std::vector<SensorInfoEXT> sensors = Sensors::GetSensorsEXT();

        std::vector<std::string> lines;
        lines.push_back("Mainly a mobile feature; expect none of this on most desktops.");
        lines.push_back("Sensors::GetSensorsEXT() count: " + std::to_string(sensors.size()));
        for (const auto& s : sensors) {
            lines.push_back("  Id " + std::to_string(s.id) + ": " + (s.name.empty() ? "(unnamed)" : s.name));
        }
        lines.emplace_back();

        Microsoft::Xna::Framework::Vector3 accel;
        if (Sensors::GetAccelerometerEXT(accel)) {
            lines.push_back("Accelerometer (m/s^2): (" + std::to_string(accel.X) + ", " +
                            std::to_string(accel.Y) + ", " + std::to_string(accel.Z) + ")");
        } else {
            lines.push_back("Accelerometer: none present");
        }

        Microsoft::Xna::Framework::Vector3 gyro;
        if (Sensors::GetGyroscopeEXT(gyro)) {
            lines.push_back("Gyroscope (rad/s): (" + std::to_string(gyro.X) + ", " +
                            std::to_string(gyro.Y) + ", " + std::to_string(gyro.Z) + ")");
        } else {
            lines.push_back("Gyroscope: none present");
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Input::OtherDemos
