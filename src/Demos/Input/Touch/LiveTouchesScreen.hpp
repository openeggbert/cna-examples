// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::TouchDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
using Microsoft::Xna::Framework::Input::Touch::TouchLocation;
using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

// Demonstrates TouchPanel::GetState(): the raw per-finger touch collection
// (up to TouchPanel::MAX_TOUCHES), independent of gesture recognition --
// each active finger's id/state/position/pressure, plus a marker drawn at
// its position. The manual hardware-validation "touch the screen" tool for
// this app (see plan.md section 5.1); needs a real touchscreen/Android
// device, not a desktop mouse, to exercise beyond the empty state.
class LiveTouchesScreen : public DemoScreen {
public:
    LiveTouchesScreen() : DemoScreen("Touch: Live Touches") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const TouchCollection touches = TouchPanel::GetState();

        std::vector<std::string> lines;
        lines.push_back("Touch the screen (up to " + std::to_string(TouchPanel::MAX_TOUCHES) + " fingers):");
        lines.push_back("each active touch is listed and marked below.");
        lines.push_back("Active touches: " + std::to_string(touches.getCountProperty()));
        lines.emplace_back();

        for (int i = 0; i < touches.getCountProperty(); ++i) {
            const TouchLocation& t = touches[(std::size_t)i];
            lines.push_back("Id " + std::to_string(t.getIdProperty()) + ": " + StateName(t.getStateProperty()) +
                            " at (" + std::to_string((int)t.getPositionProperty().X) + ", " +
                            std::to_string((int)t.getPositionProperty().Y) + "), pressure " +
                            std::to_string(t.getPressureEXT()));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        for (int i = 0; i < touches.getCountProperty(); ++i) {
            const TouchLocation& t = touches[(std::size_t)i];
            sb.DrawString(font, "+", Vector2(t.getPositionProperty().X - 6.0f, t.getPositionProperty().Y - 14.0f),
                         mul(Color::Yellow, TransitionAlpha()));
        }
    }

private:
    static const char* StateName(TouchLocationState state) {
        switch (state) {
            case TouchLocationState::Pressed: return "Pressed";
            case TouchLocationState::Moved: return "Moved";
            case TouchLocationState::Released: return "Released";
            default: return "Invalid";
        }
    }
};

} // namespace CnaExamples::Demos::Input::TouchDemos
