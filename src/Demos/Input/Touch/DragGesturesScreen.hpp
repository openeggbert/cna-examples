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

// Demonstrates the drag gesture family, grouped together since they're one
// continuous interaction reported as several events: HorizontalDrag/
// VerticalDrag (axis-locked once the drag direction is decided),
// FreeDrag (unconstrained), and DragComplete (fired on release, with no
// Position/Delta of its own).
class DragGesturesScreen : public DemoScreen {
public:
    DragGesturesScreen() : DemoScreen("Touch: Drag Gestures") {}

    void LoadContent() override {
        previousGestures_ = TouchPanel::getEnabledGesturesProperty();
        TouchPanel::setEnabledGesturesProperty(
            GestureType::HorizontalDrag | GestureType::VerticalDrag |
            GestureType::FreeDrag | GestureType::DragComplete);
    }

    void UnloadContent() override {
        TouchPanel::setEnabledGesturesProperty(previousGestures_);
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        while (TouchPanel::getIsGestureAvailableProperty()) {
            const GestureSample g = TouchPanel::ReadGesture();
            if (g.getGestureTypeProperty() == GestureType::DragComplete) {
                Log("DragComplete");
                continue;
            }
            Log(std::string(NameOf(g.getGestureTypeProperty())) + " delta (" +
               std::to_string((int)g.getDeltaProperty().X) + ", " +
               std::to_string((int)g.getDeltaProperty().Y) + ") at (" +
               std::to_string((int)g.getPositionProperty().X) + ", " +
               std::to_string((int)g.getPositionProperty().Y) + ")");
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Drag a finger across the screen; gestures are logged below.");
        lines.emplace_back();
        for (const auto& line : log_) lines.push_back(line);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* NameOf(GestureType type) {
        if (type == GestureType::HorizontalDrag) return "HorizontalDrag";
        if (type == GestureType::VerticalDrag) return "VerticalDrag";
        return "FreeDrag";
    }

    void Log(std::string line) {
        log_.push_back(std::move(line));
        while (log_.size() > kMaxLines) log_.pop_front();
    }

    static constexpr std::size_t kMaxLines = 14;

    GestureType previousGestures_ = GestureType::None;
    std::deque<std::string> log_;
};

} // namespace CnaExamples::Demos::Input::TouchDemos
