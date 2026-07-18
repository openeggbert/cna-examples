// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::TouchDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Touch::GestureSample;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

// Demonstrates the queue-draining contract of IsGestureAvailable()/
// ReadGesture() explicitly: gestures queue up between frames (e.g. several
// quick taps before the next Update), so a game must drain the whole queue
// each frame in a while-loop, not just check once -- this screen counts
// how many were drained this frame vs. the running total, to make that
// queueing behavior visible rather than assumed.
class GestureQueueScreen : public DemoScreen {
public:
    GestureQueueScreen() : DemoScreen("Touch: Gesture Queue / IsGestureAvailable") {}

    void LoadContent() override {
        previousGestures_ = TouchPanel::getEnabledGesturesProperty();
        TouchPanel::setEnabledGesturesProperty(GestureType::Tap | GestureType::FreeDrag);
    }

    void UnloadContent() override {
        TouchPanel::setEnabledGesturesProperty(previousGestures_);
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        drainedThisFrame_ = 0;
        while (TouchPanel::getIsGestureAvailableProperty()) {
            const GestureSample discarded = TouchPanel::ReadGesture();
            (void)discarded;
            drainedThisFrame_++;
            totalDrained_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Tap/drag quickly (Tap + FreeDrag enabled) to queue several gestures");
        lines.push_back("between frames -- watch this frame's drain count jump above 1.");
        lines.emplace_back();
        lines.push_back("IsGestureAvailable() right now: " +
                        std::string(TouchPanel::getIsGestureAvailableProperty() ? "true" : "false"));
        lines.push_back("Drained this frame: " + std::to_string(drainedThisFrame_));
        lines.push_back("Total drained so far: " + std::to_string(totalDrained_));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    GestureType previousGestures_ = GestureType::None;
    int drainedThisFrame_ = 0;
    int totalDrained_ = 0;
};

} // namespace CnaExamples::Demos::Input::TouchDemos
