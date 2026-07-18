// SPDX-License-Identifier: MIT
#pragma once

#include <deque>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "System/MulticastAction.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Mouse;
using ClickedToken = System::MulticastAction<int>::Token;

// Demonstrates the NOXNA/EXT Mouse::ClickedEXT event -- an event-driven
// alternative to polling MouseState's button properties, firing once per
// physical click (button index: 0=Left, 1=Middle, 2=Right, 3=XButton1,
// 4=XButton2, matching FNA's INTERNAL_onClicked convention). Subscribes in
// LoadContent()/unsubscribes in UnloadContent(), the same
// subscribe-tied-to-screen-lifetime pattern as Keyboard's TextInputScreen.
class ClickedEventScreen : public DemoScreen {
public:
    ClickedEventScreen() : DemoScreen("Mouse: ClickedEXT Event") {}

    void LoadContent() override {
        subscription_ = Mouse::ClickedEXT.Add([this](int button) { OnClicked(button); });
    }

    void UnloadContent() override {
        Mouse::ClickedEXT.Remove(subscription_);
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Click any mouse button; each click is logged below (newest last).");
        lines.emplace_back();
        for (const auto& line : log_) lines.push_back(line);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void OnClicked(int button) {
        static const char* kNames[] = {"Left", "Middle", "Right", "XButton1", "XButton2"};
        const char* name = (button >= 0 && button < 5) ? kNames[button] : "?";
        log_.push_back(std::string("Clicked: ") + name + " (index " + std::to_string(button) + ")");
        while (log_.size() > kMaxLines) log_.pop_front();
    }

    static constexpr std::size_t kMaxLines = 14;

    ClickedToken subscription_ = System::MulticastAction<int>::InvalidToken;
    std::deque<std::string> log_;
};

} // namespace CnaExamples::Demos::Input::MouseDemos
