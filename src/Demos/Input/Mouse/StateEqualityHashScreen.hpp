// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Mouse;
using Microsoft::Xna::Framework::Input::MouseState;

// Demonstrates MouseState::Equals()/GetHashCode() (and the free operator==/
// operator!=): freezes a snapshot (Space/Enter/A/tap), then compares it
// against the live state every frame. Note the cumulative ScrollWheelValue
// participates in equality -- scrolling at all after freezing is enough to
// make live != frozen even if the cursor hasn't moved.
class StateEqualityHashScreen : public DemoScreen {
public:
    StateEqualityHashScreen()
        : DemoScreen("Mouse: State Equals()/GetHashCode()"), frozen_(Mouse::GetState()) {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            frozen_ = Mouse::GetState();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const MouseState live = Mouse::GetState();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap freezes a snapshot (position, buttons, scroll value).");
        lines.push_back("Move/click/scroll to see it diverge from the frozen snapshot.");
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
    MouseState frozen_;
};

} // namespace CnaExamples::Demos::Input::MouseDemos
