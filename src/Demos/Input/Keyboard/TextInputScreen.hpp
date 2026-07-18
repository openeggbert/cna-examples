// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/TextInputEXT.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/MulticastAction.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::KeyboardDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Input::TextInputEXT;
using SharpRuntime::charcs;
using TextInputToken = System::MulticastAction<charcs>::Token;

// Demonstrates the NOXNA/EXT TextInputEXT API: StartTextInput()/
// StopTextInput() and the TextInput event, which (unlike polling
// Keyboard::GetState()) delivers already-composed, layout-and-IME-aware
// characters -- the correct way to build a text box. Text input is only
// active while this screen is on top, so LoadContent/UnloadContent start
// and stop it, and the event subscription is torn down with it.
class TextInputScreen : public DemoScreen {
public:
    TextInputScreen() : DemoScreen("Keyboard: TextInputEXT") {}

    void LoadContent() override {
        TextInputEXT::StartTextInput();
        subscription_ = TextInputEXT::TextInput.Add([this](charcs c) { OnChar(c); });
    }

    void UnloadContent() override {
        TextInputEXT::TextInput.Remove(subscription_);
        TextInputEXT::StopTextInput();
    }

protected:
    void OnDemoInput(InputState& input) override {
        // A dedicated "clear" gesture: gamepad Y (arbitrary distinct
        // button so it doesn't collide with Back/Select while typing).
        PlayerIndex playerIndex;
        if (input.IsNewButtonPress(Buttons::Y, ControllingPlayer(), playerIndex)) {
            typed_.clear();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Type on a physical keyboard (or the on-screen keyboard on mobile).");
        lines.push_back("Text input is active only while this screen is open.");
        lines.emplace_back();
        lines.push_back("> " + typed_ + "_");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void OnChar(charcs c) {
        if (c == u'\b') {
            if (!typed_.empty()) typed_.pop_back();
            return;
        }
        if (c == u'\r' || c == u'\n') {
            typed_.clear();
            return;
        }
        // Demo-only narrowing: fine for the printable ASCII range a physical
        // keyboard normally sends; a real text box would keep charcs (UTF-16)
        // throughout instead of collapsing to a narrow std::string.
        if (c >= 0x20 && c < 0x7F) {
            typed_ += static_cast<char>(c);
        }
    }

    std::string typed_;
    TextInputToken subscription_ = System::MulticastAction<charcs>::InvalidToken;
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
