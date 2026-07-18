// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::TouchDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
using Microsoft::Xna::Framework::Input::Touch::TouchLocation;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

// Demonstrates TouchLocation::Equals()/GetHashCode()/ToString() by diffing
// the first active touch (index 0 of TouchPanel::GetState()) against what
// it was last frame -- an invalid (default-constructed) TouchLocation
// before any touch has occurred, matching the "no touch" NOXNA constructor.
class TouchLocationQueryScreen : public DemoScreen {
public:
    TouchLocationQueryScreen() : DemoScreen("Touch: TouchLocation Equals()/ToString()") {}

protected:
    void OnDemoUpdate(GameTime&) override {
        const TouchCollection touches = TouchPanel::GetState();
        previous_ = current_;
        current_ = touches.getCountProperty() > 0 ? touches[0] : TouchLocation();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Touching updates 'current' every frame; 'previous' lags by one frame.");
        lines.emplace_back();
        lines.push_back("previous.ToString(): " + previous_.ToString());
        lines.push_back("current.ToString():  " + current_.ToString());
        lines.emplace_back();
        lines.push_back(std::string("current.Equals(previous): ") +
                        (current_.Equals(previous_) ? "true" : "false"));
        lines.push_back("current.GetHashCode(): " + std::to_string(current_.GetHashCode()));
        lines.push_back("previous.GetHashCode(): " + std::to_string(previous_.GetHashCode()));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    TouchLocation previous_;
    TouchLocation current_;
};

} // namespace CnaExamples::Demos::Input::TouchDemos
