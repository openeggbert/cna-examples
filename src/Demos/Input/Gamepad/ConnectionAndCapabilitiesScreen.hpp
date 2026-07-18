// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadCapabilities.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadCapabilities;

// Demonstrates GamePad::GetState(PlayerIndex).IsConnectedProperty and
// GetCapabilities() across all four player slots -- unlike Keyboard, each
// slot here genuinely can differ (a real second/third/fourth pad may or may
// not be plugged in), so showing all four at once is meaningful.
class ConnectionAndCapabilitiesScreen : public DemoScreen {
public:
    ConnectionAndCapabilitiesScreen() : DemoScreen("Gamepad: Connection & Capabilities") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Connect a controller to see it appear in a slot below.");
        lines.emplace_back();

        static constexpr std::array<std::pair<PlayerIndex, const char*>, 4> kSlots{{
            {PlayerIndex::One, "One"}, {PlayerIndex::Two, "Two"},
            {PlayerIndex::Three, "Three"}, {PlayerIndex::Four, "Four"},
        }};

        for (const auto& [index, label] : kSlots) {
            const GamePadCapabilities caps = GamePad::GetCapabilities(index);
            if (!caps.getIsConnectedProperty()) {
                lines.push_back(std::string("PlayerIndex::") + label + " -- not connected");
                continue;
            }
            const std::string name = GamePad::GetNameEXT(index);
            lines.push_back(std::string("PlayerIndex::") + label + " -- connected: " +
                            (name.empty() ? "(unnamed)" : name));
            std::string caps_line = "  has: ";
            if (caps.getHasAButtonProperty()) caps_line += "A ";
            if (caps.getHasBButtonProperty()) caps_line += "B ";
            if (caps.getHasXButtonProperty()) caps_line += "X ";
            if (caps.getHasYButtonProperty()) caps_line += "Y ";
            if (caps.getHasLeftVibrationMotorProperty()) caps_line += "LVib ";
            if (caps.getHasRightVibrationMotorProperty()) caps_line += "RVib ";
            if (caps.getHasLightBarEXTProperty()) caps_line += "LightBar(EXT) ";
            if (caps.getHasGyroEXTProperty()) caps_line += "Gyro(EXT) ";
            lines.push_back(caps_line);
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
