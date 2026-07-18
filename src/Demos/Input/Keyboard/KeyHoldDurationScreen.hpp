// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <cstdio>
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
using Microsoft::Xna::Framework::Input::Keys;

// Demonstrates a practical pattern built on IsKeyDown() + GameTime rather
// than a single API call: measuring how long a key has been held, and
// deriving a typematic "key repeat" (first repeat after an initial delay,
// then at a fixed interval) the way a text field or held-direction-repeat
// UI would. XNA's KeyboardState has no built-in repeat/hold-duration -- this
// is how a game implements it on top.
class KeyHoldDurationScreen : public DemoScreen {
public:
    KeyHoldDurationScreen()
        : DemoScreen("Keyboard: Hold Duration & Repeat"),
          previousPressed_(Keyboard::GetState().GetPressedKeys()) {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        const std::vector<Keys> current = Keyboard::GetState().GetPressedKeys();
        for (Keys key : current) {
            if (std::find(previousPressed_.begin(), previousPressed_.end(), key) == previousPressed_.end()) {
                trackedKey_ = key;
                haveKey_ = true;
                heldSeconds_ = 0.0;
                repeatCount_ = 0;
                sinceLastRepeat_ = 0.0;
            }
        }
        previousPressed_ = current;

        if (!haveKey_) return;

        const bool down = std::find(current.begin(), current.end(), trackedKey_) != current.end();
        const double elapsed = gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        if (!down) {
            heldSeconds_ = 0.0;
            return;
        }

        heldSeconds_ += elapsed;

        if (heldSeconds_ >= kInitialDelaySeconds) {
            sinceLastRepeat_ += elapsed;
            while (sinceLastRepeat_ >= kRepeatIntervalSeconds) {
                sinceLastRepeat_ -= kRepeatIntervalSeconds;
                repeatCount_++;
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Hold a key down; watch the hold timer and typematic repeat count.");
        lines.emplace_back();

        if (!haveKey_) {
            lines.push_back("(no key tracked yet)");
        } else {
            char buf[64];
            std::snprintf(buf, sizeof(buf), "Tracked key: %s", Keyboard::GetKeyNameEXT(trackedKey_).c_str());
            lines.push_back(buf);
            std::snprintf(buf, sizeof(buf), "Held for: %.2f s", heldSeconds_);
            lines.push_back(buf);
            lines.push_back("Repeats fire after 0.5s, then every 0.1s:");
            lines.push_back("Repeat count: " + std::to_string(repeatCount_));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr double kInitialDelaySeconds = 0.5;
    static constexpr double kRepeatIntervalSeconds = 0.1;

    std::vector<Keys> previousPressed_;
    Keys trackedKey_ = Keys::None;
    bool haveKey_ = false;
    double heldSeconds_ = 0.0;
    double sinceLastRepeat_ = 0.0;
    int repeatCount_ = 0;
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
