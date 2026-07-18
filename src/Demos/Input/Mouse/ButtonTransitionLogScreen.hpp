// SPDX-License-Identifier: MIT
#pragma once

#include <deque>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::Mouse;
using Microsoft::Xna::Framework::Input::MouseState;

// Demonstrates detecting button edge transitions by diffing two MouseState
// snapshots -- the polling equivalent of Mouse::ClickedEXT (see
// ClickedEventScreen), useful when a game already polls MouseState each
// frame for position and doesn't want a second, event-based code path just
// for buttons.
class ButtonTransitionLogScreen : public DemoScreen {
public:
    ButtonTransitionLogScreen()
        : DemoScreen("Mouse: Button Press/Release Log"), previous_(Mouse::GetState()) {}

protected:
    void OnDemoUpdate(GameTime&) override {
        const MouseState current = Mouse::GetState();
        CheckButton("Left", previous_.getLeftButtonProperty(), current.getLeftButtonProperty());
        CheckButton("Right", previous_.getRightButtonProperty(), current.getRightButtonProperty());
        CheckButton("Middle", previous_.getMiddleButtonProperty(), current.getMiddleButtonProperty());
        CheckButton("XButton1", previous_.getXButton1Property(), current.getXButton1Property());
        CheckButton("XButton2", previous_.getXButton2Property(), current.getXButton2Property());
        previous_ = current;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Press/release any mouse button; transitions are logged below (newest last).");
        lines.emplace_back();
        for (const auto& line : log_) lines.push_back(line);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void CheckButton(const char* name, ButtonState before, ButtonState after) {
        if (before == after) return;
        Log(std::string(after == ButtonState::Pressed ? "DOWN  " : "UP    ") + name);
    }

    void Log(std::string line) {
        log_.push_back(std::move(line));
        while (log_.size() > kMaxLines) log_.pop_front();
    }

    static constexpr std::size_t kMaxLines = 14;

    MouseState previous_;
    std::deque<std::string> log_;
};

} // namespace CnaExamples::Demos::Input::MouseDemos
