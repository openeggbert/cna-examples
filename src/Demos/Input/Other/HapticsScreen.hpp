// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/HapticInfo.hpp"
#include "CNA/Input/Haptics.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::HapticInfoEXT;
using CNA::Input::Haptics;

// Demonstrates the NOXNA/EXT CNA::Input::Haptics API: force-feedback
// devices richer than GamePad::SetVibration's simple dual-motor rumble
// (real wheels/HOTAS setups with dedicated actuators). Shows enumeration
// and the two capability-check helpers; does not attempt to build/play a
// HapticEffectEXT -- that needs real force-feedback hardware to be
// meaningful, and most gamepads only support simple rumble, not this API.
class HapticsScreen : public DemoScreen {
public:
    HapticsScreen() : DemoScreen("Other: Haptics (EXT)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const std::vector<HapticInfoEXT> haptics = Haptics::GetHapticsEXT();

        std::vector<std::string> lines;
        lines.push_back("Real force-feedback hardware (a wheel/HOTAS) is rare -- expect none here.");
        lines.push_back("Haptics::GetHapticsEXT() count: " + std::to_string(haptics.size()));
        for (const auto& h : haptics) {
            lines.push_back("  Id " + std::to_string(h.id) + ": " + (h.name.empty() ? "(unnamed)" : h.name));
        }
        lines.emplace_back();
        lines.push_back(std::string("IsMouseHapticEXT(): ") + (Haptics::IsMouseHapticEXT() ? "true" : "false"));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Input::OtherDemos
