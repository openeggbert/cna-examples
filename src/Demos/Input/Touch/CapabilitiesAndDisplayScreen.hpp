// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanelCapabilities.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::TouchDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::DisplayOrientation;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;
using Microsoft::Xna::Framework::Input::Touch::TouchPanelCapabilities;

// Demonstrates TouchPanel::GetCapabilities() (is a touch device present, and
// how many simultaneous fingers it supports) plus the display-geometry
// properties (DisplayWidth/Height/Orientation) gesture and normalized-touch
// processing are computed against.
class CapabilitiesAndDisplayScreen : public DemoScreen {
public:
    CapabilitiesAndDisplayScreen() : DemoScreen("Touch: Capabilities & Display") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const TouchPanelCapabilities caps = TouchPanel::GetCapabilities();

        std::vector<std::string> lines;
        lines.push_back("GetCapabilities():");
        lines.push_back("  IsConnected: " + std::string(caps.getIsConnectedProperty() ? "true" : "false"));
        lines.push_back("  MaximumTouchCount: " + std::to_string(caps.getMaximumTouchCountProperty()));
        lines.emplace_back();
        lines.push_back("Display geometry (used for normalized touch/gesture coordinates):");
        lines.push_back("  DisplayWidth:  " + std::to_string(TouchPanel::getDisplayWidthProperty()));
        lines.push_back("  DisplayHeight: " + std::to_string(TouchPanel::getDisplayHeightProperty()));
        lines.push_back(std::string("  DisplayOrientation: ") +
                        OrientationName(TouchPanel::getDisplayOrientationProperty()));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* OrientationName(DisplayOrientation orientation) {
        switch (orientation) {
            case DisplayOrientation::LandscapeLeft: return "LandscapeLeft";
            case DisplayOrientation::LandscapeRight: return "LandscapeRight";
            case DisplayOrientation::Portrait: return "Portrait";
            default: return "Default/Unknown";
        }
    }
};

} // namespace CnaExamples::Demos::Input::TouchDemos
