// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/GamePadConnectionState.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::GamePadConnectionStateEXT;
using Microsoft::Xna::Framework::Input::GamePad;

// A combined "device info sheet" for several read-only NOXNA/EXT getters
// that each don't warrant a whole screen of their own: GUID, device path,
// serial number, firmware version, Steam Input handle, wired/wireless
// connection state, touchpad count, and the A button's printed glyph label
// (GetButtonLabelEXT -- useful since "A" means different things on Xbox vs.
// Nintendo/PlayStation pads).
class DeviceInfoScreen : public DemoScreen {
public:
    DeviceInfoScreen() : DemoScreen("Gamepad: Device Info (EXT)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        constexpr PlayerIndex kOne = PlayerIndex::One;

        std::vector<std::string> lines;
        lines.push_back("PlayerIndex::One's device identity (all empty/0 if disconnected):");
        lines.emplace_back();
        lines.push_back("GUID: " + Or(GamePad::GetGUIDEXT(kOne), "(none)"));
        lines.push_back("Path: " + Or(GamePad::GetPathEXT(kOne), "(none)"));
        lines.push_back("Serial: " + Or(GamePad::GetSerialEXT(kOne), "(none)"));
        lines.push_back("Firmware version: " + std::to_string(GamePad::GetFirmwareVersionEXT(kOne)));
        lines.push_back("Steam Input handle: " + std::to_string(GamePad::GetSteamHandleEXT(kOne)));
        lines.push_back(std::string("Connection: ") + ConnectionName(GamePad::GetConnectionStateEXT(kOne)));
        lines.push_back("Touchpad count: " + std::to_string(GamePad::GetTouchpadCountEXT(kOne)));
        lines.push_back(std::string("'A' button label: ") + ButtonLabelName(kOne));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static std::string Or(const std::string& s, const char* fallback) {
        return s.empty() ? fallback : s;
    }

    static const char* ConnectionName(GamePadConnectionStateEXT state) {
        switch (state) {
            case GamePadConnectionStateEXT::Wired: return "Wired";
            case GamePadConnectionStateEXT::Wireless: return "Wireless";
            default: return "Unknown";
        }
    }

    static std::string ButtonLabelName(PlayerIndex index) {
        using CNA::Input::GamePadButtonLabelEXT;
        switch (GamePad::GetButtonLabelEXT(index, Buttons::A)) {
            case GamePadButtonLabelEXT::A: return "A";
            case GamePadButtonLabelEXT::B: return "B";
            case GamePadButtonLabelEXT::X: return "X";
            case GamePadButtonLabelEXT::Y: return "Y";
            case GamePadButtonLabelEXT::Cross: return "Cross";
            case GamePadButtonLabelEXT::Circle: return "Circle";
            case GamePadButtonLabelEXT::Square: return "Square";
            case GamePadButtonLabelEXT::Triangle: return "Triangle";
            default: return "Unknown";
        }
    }
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
