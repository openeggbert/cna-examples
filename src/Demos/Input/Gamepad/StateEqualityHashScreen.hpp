// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadState;

// Demonstrates GamePadState::Equals()/GetHashCode() (and the free
// operator==/operator!=): freezes a snapshot (Space/Enter/A/tap), then
// compares it against the live state every frame.
class StateEqualityHashScreen : public DemoScreen {
public:
    StateEqualityHashScreen()
        : DemoScreen("Gamepad: State Equals()/GetHashCode()"),
          frozen_(GamePad::GetState(PlayerIndex::One)) {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            frozen_ = GamePad::GetState(PlayerIndex::One);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const GamePadState live = GamePad::GetState(PlayerIndex::One);

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap freezes a snapshot of PlayerIndex::One's state.");
        lines.push_back("Move a stick/trigger or press a button to see it diverge.");
        lines.emplace_back();
        lines.push_back("Frozen snapshot hash: " + std::to_string(frozen_.GetHashCode()));
        lines.push_back("Live state hash:      " + std::to_string(live.GetHashCode()));
        lines.emplace_back();
        lines.push_back(std::string("live.Equals(frozen): ") + (live.Equals(frozen_) ? "true" : "false"));
        lines.push_back(std::string("live == frozen:      ") + (live == frozen_ ? "true" : "false"));
        lines.push_back(std::string("live != frozen:      ") + (live != frozen_ ? "true" : "false"));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    GamePadState frozen_;
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
