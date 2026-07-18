// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <deque>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::KeyboardDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::Keys;

// Demonstrates detecting edge transitions (a key going from up->down or
// down->up between frames) by diffing two KeyboardState snapshots -- the
// same pattern GameStateManagement/InputState.hpp uses internally for
// IsNewKeyPress, shown explicitly here as a scrolling event log.
class KeyTransitionLogScreen : public DemoScreen {
public:
    KeyTransitionLogScreen()
        : DemoScreen("Keyboard: Press/Release Log"), previousState_(Keyboard::GetState()) {}

protected:
    void OnDemoUpdate(GameTime&) override {
        const KeyboardState current = Keyboard::GetState();
        const std::vector<Keys> currentPressed = current.GetPressedKeys();
        const std::vector<Keys> previousPressed = previousState_.GetPressedKeys();

        for (Keys key : currentPressed) {
            if (!Contains(previousPressed, key)) {
                Log("DOWN  " + Keyboard::GetKeyNameEXT(key));
            }
        }
        for (Keys key : previousPressed) {
            if (!Contains(currentPressed, key)) {
                Log("UP    " + Keyboard::GetKeyNameEXT(key));
            }
        }

        previousState_ = current;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Press/release keys; each transition is logged below (newest last).");
        lines.emplace_back();
        for (const auto& line : log_) lines.push_back(line);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static bool Contains(const std::vector<Keys>& keys, Keys key) {
        return std::find(keys.begin(), keys.end(), key) != keys.end();
    }

    void Log(std::string line) {
        log_.push_back(std::move(line));
        while (log_.size() > kMaxLines) {
            log_.pop_front();
        }
    }

    static constexpr std::size_t kMaxLines = 14;

    KeyboardState previousState_;
    std::deque<std::string> log_;
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
