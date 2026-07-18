// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanelCapabilities.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::TouchDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

// Demonstrates the NOXNA/EXT TouchDeviceExistsEXT flag alongside a simple
// derived metric this app computes itself (not a direct API call): the
// peak number of simultaneous touches seen since this screen opened,
// against TouchPanelCapabilities::MaximumTouchCount and
// TouchPanel::MAX_TOUCHES -- useful for confirming multi-touch actually
// works up to the hardware's real limit, not just "one finger at a time".
class PeakTouchesScreen : public DemoScreen {
public:
    PeakTouchesScreen() : DemoScreen("Touch: Peak Simultaneous Touches") {}

protected:
    void OnDemoUpdate(GameTime&) override {
        const int count = TouchPanel::GetState().getCountProperty();
        if (count > peakCount_) peakCount_ = count;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const int current = TouchPanel::GetState().getCountProperty();
        const int maxReported = TouchPanel::GetCapabilities().getMaximumTouchCountProperty();

        std::vector<std::string> lines;
        lines.push_back("Try touching with as many fingers at once as you can.");
        lines.emplace_back();
        lines.push_back(std::string("TouchDeviceExistsEXT: ") +
                        (TouchPanel::getTouchDeviceExistsProperty() ? "true" : "false"));
        lines.push_back("Current touches: " + std::to_string(current));
        lines.push_back("Peak touches seen this screen: " + std::to_string(peakCount_));
        lines.push_back("Capabilities.MaximumTouchCount: " + std::to_string(maxReported));
        lines.push_back("TouchPanel::MAX_TOUCHES (API ceiling): " + std::to_string(TouchPanel::MAX_TOUCHES));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int peakCount_ = 0;
};

} // namespace CnaExamples::Demos::Input::TouchDemos
