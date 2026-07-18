// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Mouse;
using Microsoft::Xna::Framework::Input::MouseState;

// Demonstrates MouseState::GetScrollWheelValueProperty() and the NOXNA/EXT
// horizontal counterpart: both are CUMULATIVE since the game started (the
// same XNA convention KeyboardState avoids for keys), so a game normally
// diffs against the previous frame's value to get a per-frame delta -- this
// demo shows both the raw cumulative value and that derived delta.
class ScrollWheelScreen : public DemoScreen {
public:
    ScrollWheelScreen()
        : DemoScreen("Mouse: Scroll Wheel"), previous_(Mouse::GetState()) {}

protected:
    void OnDemoUpdate(GameTime&) override {
        const MouseState current = Mouse::GetState();
        lastDeltaV_ = current.getScrollWheelValueProperty() - previous_.getScrollWheelValueProperty();
        lastDeltaH_ = current.getHorizontalScrollWheelValueEXTProperty() -
                     previous_.getHorizontalScrollWheelValueEXTProperty();
        if (lastDeltaV_ != 0) totalNotchesV_ += lastDeltaV_ / 120;
        if (lastDeltaH_ != 0) totalNotchesH_ += lastDeltaH_ / 120;
        previous_ = current;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Scroll the wheel (vertical and, if available, horizontal/tilt):");
        lines.emplace_back();
        lines.push_back("Cumulative vertical:   " + std::to_string(previous_.getScrollWheelValueProperty()));
        lines.push_back("Last-frame delta:      " + std::to_string(lastDeltaV_));
        lines.push_back("Notches (delta/120):   " + std::to_string(totalNotchesV_));
        lines.emplace_back();
        lines.push_back("Cumulative horizontal (EXT): " +
                        std::to_string(previous_.getHorizontalScrollWheelValueEXTProperty()));
        lines.push_back("Last-frame delta:            " + std::to_string(lastDeltaH_));
        lines.push_back("Notches (delta/120):         " + std::to_string(totalNotchesH_));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    MouseState previous_;
    int lastDeltaV_ = 0;
    int lastDeltaH_ = 0;
    int totalNotchesV_ = 0;
    int totalNotchesH_ = 0;
};

} // namespace CnaExamples::Demos::Input::MouseDemos
