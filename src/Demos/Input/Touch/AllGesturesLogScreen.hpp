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

// Enables every GestureType at once and logs whichever fires, by name --
// useful as a single "just show me what this app's touch panel recognizes"
// screen without committing to one gesture family, unlike the more focused
// demos elsewhere in this category.
class AllGesturesLogScreen : public DemoScreen {
public:
    AllGesturesLogScreen() : DemoScreen("Touch: All Gestures Log") {}

    void LoadContent() override {
        previousGestures_ = TouchPanel::getEnabledGesturesProperty();
        TouchPanel::setEnabledGesturesProperty(
            GestureType::Tap | GestureType::DoubleTap | GestureType::Hold |
            GestureType::HorizontalDrag | GestureType::VerticalDrag | GestureType::FreeDrag |
            GestureType::Pinch | GestureType::PinchComplete |
            GestureType::Flick | GestureType::DragComplete);
    }

    void UnloadContent() override {
        TouchPanel::setEnabledGesturesProperty(previousGestures_);
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        while (TouchPanel::getIsGestureAvailableProperty()) {
            const GestureSample g = TouchPanel::ReadGesture();
            Log(NameOf(g.getGestureTypeProperty()));
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Every gesture type is enabled; try any touch interaction.");
        lines.emplace_back();
        for (const auto& line : log_) lines.push_back(line);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* NameOf(GestureType type) {
        switch (type) {
            case GestureType::Tap: return "Tap";
            case GestureType::DoubleTap: return "DoubleTap";
            case GestureType::Hold: return "Hold";
            case GestureType::HorizontalDrag: return "HorizontalDrag";
            case GestureType::VerticalDrag: return "VerticalDrag";
            case GestureType::FreeDrag: return "FreeDrag";
            case GestureType::Pinch: return "Pinch";
            case GestureType::PinchComplete: return "PinchComplete";
            case GestureType::Flick: return "Flick";
            case GestureType::DragComplete: return "DragComplete";
            default: return "None";
        }
    }

    void Log(std::string line) {
        log_.push_back(std::move(line));
        while (log_.size() > kMaxLines) log_.pop_front();
    }

    static constexpr std::size_t kMaxLines = 16;

    GestureType previousGestures_ = GestureType::None;
    std::deque<std::string> log_;
};

} // namespace CnaExamples::Demos::Input::TouchDemos
