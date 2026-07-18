// SPDX-License-Identifier: MIT
#pragma once

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

// Demonstrates KeyboardState::GetPressedKeys(), which returns every key
// currently down as a std::vector<Keys> -- useful for "how many keys are
// held right now" (n-key rollover) style checks that IsKeyDown(one key at a
// time) can't show at a glance.
class PressedKeysListScreen : public DemoScreen {
public:
    PressedKeysListScreen() : DemoScreen("Keyboard: GetPressedKeys()") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const KeyboardState state = Keyboard::GetState();
        const std::vector<Keys> pressed = state.GetPressedKeys();

        std::vector<std::string> lines;
        lines.push_back("Hold down keys to see them listed below.");
        lines.push_back("Currently down: " + std::to_string(pressed.size()) + " key(s)");
        lines.emplace_back();

        if (pressed.empty()) {
            lines.push_back("(none)");
        } else {
            // Wrap into rows so a full n-key-rollover press doesn't run off
            // the screen.
            std::string row;
            for (std::size_t i = 0; i < pressed.size(); ++i) {
                const std::string name = Keyboard::GetKeyNameEXT(pressed[i]);
                if (!row.empty()) row += ", ";
                row += name;
                if (row.size() > 48) {
                    lines.push_back(row);
                    row.clear();
                }
            }
            if (!row.empty()) lines.push_back(row);
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
