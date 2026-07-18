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

// Demonstrates Keyboard::GetState().IsKeyDown(Keys) by rendering a curated
// keyboard-shaped grid of labels that turn yellow while the physical key is
// held. Doubles as the manual hardware-validation tool that
// ../../plan_input.md Phase 11 needed a human for (see cna-examples plan.md
// section 5.1) -- press any listed key here and confirm it lights up.
class LiveKeyStateScreen : public DemoScreen {
public:
    LiveKeyStateScreen() : DemoScreen("Keyboard: Live Key State") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const KeyboardState state = Keyboard::GetState();

        float y = 70.0f;
        for (const auto& row : kRows) {
            float x = 40.0f;
            for (const auto& key : row) {
                const bool down = state.IsKeyDown(key.key);
                sb.DrawString(font, key.label, Vector2(x, y),
                             mul(down ? Color::Yellow : Color(90, 90, 90), TransitionAlpha()));
                x += key.width;
            }
            y += (float)font.getLineSpacingProperty() + 14.0f;
        }
    }

private:
    struct GridKey {
        Keys key;
        const char* label;
        float width;
    };

    // Rows loosely follow a physical keyboard layout, left to right, top to
    // bottom. Not exhaustive (Keys has ~150 values, many obscure/OEM) -- a
    // curated, readable subset is more useful for a live "press me" tester
    // than a full enum dump.
    static inline const std::vector<std::vector<GridKey>> kRows = {
        {{Keys::Escape, "Esc", 60}, {Keys::F1, "F1", 50}, {Keys::F2, "F2", 50}, {Keys::F3, "F3", 50},
         {Keys::F4, "F4", 50}, {Keys::F5, "F5", 50}, {Keys::F6, "F6", 50}},
        {{Keys::D1, "1", 32}, {Keys::D2, "2", 32}, {Keys::D3, "3", 32}, {Keys::D4, "4", 32},
         {Keys::D5, "5", 32}, {Keys::D6, "6", 32}, {Keys::D7, "7", 32}, {Keys::D8, "8", 32},
         {Keys::D9, "9", 32}, {Keys::D0, "0", 32}, {Keys::Back, "Back", 60}},
        {{Keys::Tab, "Tab", 50}, {Keys::Q, "Q", 32}, {Keys::W, "W", 32}, {Keys::E, "E", 32},
         {Keys::R, "R", 32}, {Keys::T, "T", 32}, {Keys::Y, "Y", 32}, {Keys::U, "U", 32},
         {Keys::I, "I", 32}, {Keys::O, "O", 32}, {Keys::P, "P", 32}},
        {{Keys::CapsLock, "Caps", 60}, {Keys::A, "A", 32}, {Keys::S, "S", 32}, {Keys::D, "D", 32},
         {Keys::F, "F", 32}, {Keys::G, "G", 32}, {Keys::H, "H", 32}, {Keys::J, "J", 32},
         {Keys::K, "K", 32}, {Keys::L, "L", 32}, {Keys::Enter, "Enter", 60}},
        {{Keys::LeftShift, "LShift", 70}, {Keys::Z, "Z", 32}, {Keys::X, "X", 32}, {Keys::C, "C", 32},
         {Keys::V, "V", 32}, {Keys::B, "B", 32}, {Keys::N, "N", 32}, {Keys::M, "M", 32},
         {Keys::RightShift, "RShift", 70}},
        {{Keys::LeftControl, "LCtrl", 60}, {Keys::LeftAlt, "LAlt", 55}, {Keys::Space, "Space", 160},
         {Keys::RightAlt, "RAlt", 55}, {Keys::RightControl, "RCtrl", 60}},
        {{Keys::Left, "Left", 50}, {Keys::Up, "Up", 50}, {Keys::Down, "Down", 55}, {Keys::Right, "Right", 55}},
    };
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
