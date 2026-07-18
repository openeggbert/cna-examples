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

// Demonstrates Hold (press and don't move for a while) and Flick (a fast
// short drag released while still moving, reported with a velocity in
// GestureSample::Delta) -- two gestures with very different timing
// characteristics grouped together since each only needs one field beyond
// Position (Hold: none; Flick: Delta).
class HoldAndFlickScreen : public DemoScreen {
public:
    HoldAndFlickScreen() : DemoScreen("Touch: Hold & Flick Gestures") {}

    void LoadContent() override {
        previousGestures_ = TouchPanel::getEnabledGesturesProperty();
        TouchPanel::setEnabledGesturesProperty(GestureType::Hold | GestureType::Flick);
    }

    void UnloadContent() override {
        TouchPanel::setEnabledGesturesProperty(previousGestures_);
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        while (TouchPanel::getIsGestureAvailableProperty()) {
            const GestureSample g = TouchPanel::ReadGesture();
            if (g.getGestureTypeProperty() == GestureType::Hold) {
                Log("Hold at (" + std::to_string((int)g.getPositionProperty().X) + ", " +
                   std::to_string((int)g.getPositionProperty().Y) + ")");
            } else if (g.getGestureTypeProperty() == GestureType::Flick) {
                Log("Flick, velocity (" + std::to_string((int)g.getDeltaProperty().X) + ", " +
                   std::to_string((int)g.getDeltaProperty().Y) + ") px/s");
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Hold a finger still, or flick quickly; gestures are logged below.");
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
