// SPDX-License-Identifier: MIT
#pragma once

#include <deque>
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

// Demonstrates the gesture-recognition API (TouchPanel::EnabledGestures,
// IsGestureAvailable, ReadGesture()) for the two simplest discrete
// gestures: Tap and DoubleTap. Enables just these two gestures while this
// screen is open, restoring whatever was enabled before on exit -- gesture
// enablement is process-wide state, not per-screen, so this discipline
// matters (a later screen enabling different gestures shouldn't be
// affected by what this one left behind, and vice versa).
class TapAndDoubleTapScreen : public DemoScreen {
public:
    TapAndDoubleTapScreen() : DemoScreen("Touch: Tap & DoubleTap Gestures") {}

    void LoadContent() override {
        previousGestures_ = TouchPanel::getEnabledGesturesProperty();
        TouchPanel::setEnabledGesturesProperty(GestureType::Tap | GestureType::DoubleTap);
    }

    void UnloadContent() override {
        TouchPanel::setEnabledGesturesProperty(previousGestures_);
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        while (TouchPanel::getIsGestureAvailableProperty()) {
            const GestureSample g = TouchPanel::ReadGesture();
            const char* name = g.getGestureTypeProperty() == GestureType::DoubleTap ? "DoubleTap" : "Tap";
            Log(std::string(name) + " at (" + std::to_string((int)g.getPositionProperty().X) + ", " +
               std::to_string((int)g.getPositionProperty().Y) + ")");
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Tap or double-tap the screen; gestures are logged below (newest last).");
        lines.emplace_back();
        for (const auto& line : log_) lines.push_back(line);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void Log(std::string line) {
        log_.push_back(std::move(line));
        while (log_.size() > kMaxLines) log_.pop_front();
    }

    static constexpr std::size_t kMaxLines = 14;

    GestureType previousGestures_ = GestureType::None;
    std::deque<std::string> log_;
};

} // namespace CnaExamples::Demos::Input::TouchDemos
