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

// Demonstrates the two-finger Pinch gesture, the only gesture that uses
// GestureSample's second finger fields (Position2/Delta2, and the EXT
// FingerId2) alongside the first (Position/Delta) -- everything else in
// this app's Touch demos only ever needs the first finger's data.
class PinchGestureScreen : public DemoScreen {
public:
    PinchGestureScreen() : DemoScreen("Touch: Pinch Gesture") {}

    void LoadContent() override {
        previousGestures_ = TouchPanel::getEnabledGesturesProperty();
        TouchPanel::setEnabledGesturesProperty(GestureType::Pinch | GestureType::PinchComplete);
    }

    void UnloadContent() override {
        TouchPanel::setEnabledGesturesProperty(previousGestures_);
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        while (TouchPanel::getIsGestureAvailableProperty()) {
            const GestureSample g = TouchPanel::ReadGesture();
            if (g.getGestureTypeProperty() == GestureType::PinchComplete) {
                Log("PinchComplete");
                continue;
            }
            Log("Pinch: finger1 (" + std::to_string((int)g.getPositionProperty().X) + ", " +
               std::to_string((int)g.getPositionProperty().Y) + ") delta (" +
               std::to_string((int)g.getDeltaProperty().X) + ", " + std::to_string((int)g.getDeltaProperty().Y) +
               "); finger2 (" + std::to_string((int)g.getPosition2Property().X) + ", " +
               std::to_string((int)g.getPosition2Property().Y) + ") delta (" +
               std::to_string((int)g.getDelta2Property().X) + ", " + std::to_string((int)g.getDelta2Property().Y) + ")");
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Pinch with two fingers; each sample (both fingers' data) is logged:");
        lines.emplace_back();
        for (const auto& line : log_) lines.push_back(line);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void Log(std::string line) {
        log_.push_back(std::move(line));
        while (log_.size() > kMaxLines) log_.pop_front();
    }

    static constexpr std::size_t kMaxLines = 10;

    GestureType previousGestures_ = GestureType::None;
    std::deque<std::string> log_;
};

} // namespace CnaExamples::Demos::Input::TouchDemos
