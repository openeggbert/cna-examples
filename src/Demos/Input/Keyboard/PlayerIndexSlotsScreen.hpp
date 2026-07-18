// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::KeyboardDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;

// Demonstrates Keyboard::GetState(PlayerIndex): XNA's keyboard API accepts a
// PlayerIndex the same way GamePad does, for local-multiplayer code that
// loops over all four slots uniformly. Physically there is one keyboard, so
// (matching FNA) every slot reports the same state -- this demo makes that
// explicit rather than surprising.
class PlayerIndexSlotsScreen : public DemoScreen {
public:
    PlayerIndexSlotsScreen() : DemoScreen("Keyboard: GetState(PlayerIndex)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Hold a key -- every PlayerIndex slot reports it (one physical keyboard):");
        lines.emplace_back();

        static constexpr std::array<std::pair<PlayerIndex, const char*>, 4> kSlots{{
            {PlayerIndex::One, "One"}, {PlayerIndex::Two, "Two"},
            {PlayerIndex::Three, "Three"}, {PlayerIndex::Four, "Four"},
        }};

        for (const auto& [index, label] : kSlots) {
            const KeyboardState state = Keyboard::GetState(index);
            lines.push_back(std::string("PlayerIndex::") + label + " -- " +
                            std::to_string(state.GetPressedKeys().size()) + " key(s) down");
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
