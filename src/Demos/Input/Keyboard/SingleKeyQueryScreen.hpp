// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::KeyboardDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::KeyState;
using Microsoft::Xna::Framework::Input::Keys;

// Demonstrates KeyboardState's single-key query surface -- IsKeyDown(),
// IsKeyUp(), operator[](Keys) and the equivalent getItem(Keys) -- all
// applied to whichever key was most recently pressed, since XNA has no
// "list all keys" call and these four are meant to be used one key at a
// time (e.g. `if (state.IsKeyDown(Keys::Space)) ...`).
class SingleKeyQueryScreen : public DemoScreen {
public:
    SingleKeyQueryScreen()
        : DemoScreen("Keyboard: IsKeyDown / operator[]"),
          previousPressed_(Keyboard::GetState().GetPressedKeys()) {}

protected:
    void OnDemoUpdate(GameTime&) override {
        const std::vector<Keys> current = Keyboard::GetState().GetPressedKeys();
        for (Keys key : current) {
            if (std::find(previousPressed_.begin(), previousPressed_.end(), key) == previousPressed_.end()) {
                trackedKey_ = key;
                haveKey_ = true;
            }
        }
        previousPressed_ = current;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Press a key to track it; release/press it again to watch these update:");
        lines.emplace_back();

        if (!haveKey_) {
            lines.push_back("(no key tracked yet)");
        } else {
            const KeyboardState state = Keyboard::GetState();
            const std::string name = Keyboard::GetKeyNameEXT(trackedKey_);

            lines.push_back("Tracked key: " + name);
            lines.emplace_back();
            lines.push_back(std::string("IsKeyDown(key):     ") + (state.IsKeyDown(trackedKey_) ? "true" : "false"));
            lines.push_back(std::string("IsKeyUp(key):       ") + (state.IsKeyUp(trackedKey_) ? "true" : "false"));
            lines.push_back(std::string("operator[](key):    ") +
                            (state[trackedKey_] == KeyState::Down ? "Down" : "Up"));
            lines.push_back(std::string("getItem(key):       ") +
                            (state.getItem(trackedKey_) == KeyState::Down ? "Down" : "Up"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::vector<Keys> previousPressed_;
    Keys trackedKey_ = Keys::None;
    bool haveKey_ = false;
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
