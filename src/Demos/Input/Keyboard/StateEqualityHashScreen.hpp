// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::KeyboardDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;

// Demonstrates KeyboardState::Equals() and GetHashCode(): freezes a snapshot
// (Space/Enter/gamepad A, or tap), then compares it against the live state
// every frame -- two snapshots with the same keys held compare Equals() and
// produce the same hash; changing what is held makes them diverge.
class StateEqualityHashScreen : public DemoScreen {
public:
    StateEqualityHashScreen()
        : DemoScreen("Keyboard: State Equals()/GetHashCode()"), frozen_(Keyboard::GetState()) {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            frozen_ = Keyboard::GetState();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const KeyboardState live = Keyboard::GetState();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap freezes a snapshot of the held keys.");
        lines.push_back("Change what you hold to see it diverge from the frozen snapshot.");
        lines.emplace_back();
        lines.push_back("Frozen snapshot hash: " + std::to_string(frozen_.GetHashCode()));
        lines.push_back("Live state hash:      " + std::to_string(live.GetHashCode()));
        lines.emplace_back();
        lines.push_back(std::string("live.Equals(frozen): ") +
                        (live.Equals(frozen_) ? "true (same keys held)" : "false (differs)"));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    KeyboardState frozen_;
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
