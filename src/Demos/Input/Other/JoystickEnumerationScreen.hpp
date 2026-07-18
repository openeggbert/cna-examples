// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/JoystickCapabilities.hpp"
#include "CNA/Input/JoystickInfo.hpp"
#include "CNA/Input/Joysticks.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::JoystickCapabilitiesEXT;
using CNA::Input::JoystickInfoEXT;
using CNA::Input::Joysticks;

// Demonstrates the NOXNA/EXT CNA::Input::Joysticks raw-joystick API: XNA 4.0
// only ever modeled Xbox-style mapped gamepads (Microsoft::Xna::Framework::
// Input::GamePad); this is SDL3's unmapped view of the same and other
// hardware (flight sticks, wheels, throttles, arbitrary HID devices) --
// GetJoysticksEXT() enumeration plus GetCapabilitiesEXT() for each.
class JoystickEnumerationScreen : public DemoScreen {
public:
    JoystickEnumerationScreen() : DemoScreen("Other: Joystick Enumeration (EXT)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const std::vector<JoystickInfoEXT> joysticks = Joysticks::GetJoysticksEXT();

        std::vector<std::string> lines;
        lines.push_back("Connect a raw joystick/wheel/flight stick to see it listed below.");
        lines.push_back("Joysticks::GetJoysticksEXT() count: " + std::to_string(joysticks.size()));
        lines.emplace_back();

        for (const auto& info : joysticks) {
            const JoystickCapabilitiesEXT caps = Joysticks::GetCapabilitiesEXT(info.id);
            lines.push_back("Id " + std::to_string(info.id) + ": " +
                            (info.name.empty() ? "(unnamed)" : info.name));
            lines.push_back("  axes=" + std::to_string(caps.axisCount) +
                            " buttons=" + std::to_string(caps.buttonCount) +
                            " hats=" + std::to_string(caps.hatCount) +
                            " balls=" + std::to_string(caps.ballCount));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Input::OtherDemos
