// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/InputDevices.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::InputDeviceInfoEXT;
using CNA::Input::InputDevices;

// Demonstrates the NOXNA/EXT CNA::Input::InputDevices enumeration API: XNA
// 4.0 assumes exactly one keyboard, one mouse, and up to four gamepads,
// with no device list -- this CNA extension enumerates every mouse,
// keyboard, and touch device by id/name. Metadata only: XNA input state
// (Keyboard::GetState(), Mouse::GetState()) stays merged across devices
// regardless of how many this reports; there is no per-device routing.
class InputDeviceEnumerationScreen : public DemoScreen {
public:
    InputDeviceEnumerationScreen() : DemoScreen("Other: Input Device Enumeration (EXT)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        AppendGroup(lines, "Mice", InputDevices::GetMiceEXT());
        lines.emplace_back();
        AppendGroup(lines, "Keyboards", InputDevices::GetKeyboardsEXT());
        lines.emplace_back();
        AppendGroup(lines, "Touch devices", InputDevices::GetTouchDevicesEXT());

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static void AppendGroup(std::vector<std::string>& lines, const char* label,
                            const std::vector<InputDeviceInfoEXT>& devices) {
        lines.push_back(std::string(label) + ": " + std::to_string(devices.size()));
        for (const auto& d : devices) {
            lines.push_back("  Id " + std::to_string(d.id) + ": " + (d.name.empty() ? "(unnamed)" : d.name));
        }
    }
};

} // namespace CnaExamples::Demos::Input::OtherDemos
