// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Devices/SystemInfo.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::SystemAndDisplayDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::SystemInfo;

// Demonstrates CNA::Devices::SystemInfo -- basic CPU/RAM hardware info, no
// XNA/WP7 equivalent. Deliberately minimal (see the class doc comment):
// just what a game would plausibly branch on for thread-pool sizing or a
// "low-end device" quality preset, not every SIMD capability flag SDL3
// exposes. Genuinely desktop-testable, unlike most of this Area.
class SystemInfoScreen : public DemoScreen {
public:
    SystemInfoScreen() : DemoScreen("System & Display: System Info") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Logical CPU cores: " + std::to_string(SystemInfo::getLogicalCpuCoreCountProperty()));
        lines.push_back("System RAM: " + std::to_string(SystemInfo::getSystemRamMegabytesProperty()) + " MB");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Devices::SystemAndDisplayDemos
