// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Devices/DisplayInfo.hpp"
#include "Microsoft/Xna/Framework/GameWindow.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::SystemAndDisplayDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::DisplayInfo;
using Microsoft::Xna::Framework::GameWindow;
using Microsoft::Xna::Framework::Rectangle;

// Demonstrates CNA::Devices::DisplayInfo -- the two SDL3 display
// capabilities GameWindow itself has no property for: per-window content
// scale (relevant for high-DPI/"Retina" displays) and the window's safe
// interactive area (relevant on phones with notches/curved edges, and on
// TVs with overscan). Genuinely desktop-testable; the safe area is most
// interesting on a real mobile device, though.
class DisplayInfoScreen : public DemoScreen {
public:
    DisplayInfoScreen() : DemoScreen("System & Display: Display Info") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const GameWindow& window = GetScreenManager()->getGameProperty().getWindowProperty();
        const float scale = DisplayInfo::getContentScaleProperty(window);
        const Rectangle safeArea = DisplayInfo::getSafeAreaProperty(window);

        std::vector<std::string> lines;
        lines.push_back("ContentScale: " + std::to_string(scale) +
                        (scale <= 0.0f ? " (could not be determined)" : ""));
        lines.emplace_back();
        lines.push_back("SafeArea (window client coordinates):");
        lines.push_back("  X=" + std::to_string(safeArea.X) + " Y=" + std::to_string(safeArea.Y) +
                        " Width=" + std::to_string(safeArea.Width) + " Height=" + std::to_string(safeArea.Height));
        lines.push_back("(most interesting on a real phone with a notch/curved edge -- try Android)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Devices::SystemAndDisplayDemos
