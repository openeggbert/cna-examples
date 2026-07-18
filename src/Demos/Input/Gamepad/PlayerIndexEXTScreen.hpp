// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::GamePad;

// Demonstrates the NOXNA/EXT GetPlayerIndexEXT()/SetPlayerIndexEXT(): the
// controller's own player-number LED (the ring of lights an Xbox/PS pad
// uses to show "you are player 2") -- distinct from the XNA PlayerIndex
// slot this app is polling, which is just which USB/Bluetooth device CNA
// assigned to that slot. Space/Enter/A/tap cycles the LED through 0..3.
class PlayerIndexEXTScreen : public DemoScreen {
public:
    PlayerIndexEXTScreen() : DemoScreen("Gamepad: Player Index LED (EXT)") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            nextLed_ = (nextLed_ + 1) % 4;
            lastSetSucceeded_ = GamePad::SetPlayerIndexEXT(PlayerIndex::One, nextLed_);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const int current = GamePad::GetPlayerIndexEXT(PlayerIndex::One);

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap cycles the controller's player-number LED (0..3).");
        lines.emplace_back();
        lines.push_back("GetPlayerIndexEXT() now: " +
                        (current < 0 ? std::string("-1 (disconnected/unset)") : std::to_string(current)));
        if (lastSetSucceeded_.has_value()) {
            lines.push_back("Last SetPlayerIndexEXT(" + std::to_string(nextLed_) + ") result: " +
                            (*lastSetSucceeded_ ? "true" : "false"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int nextLed_ = -1;
    std::optional<bool> lastSetSucceeded_;
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
