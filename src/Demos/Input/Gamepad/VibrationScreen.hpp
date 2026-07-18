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

// Demonstrates GamePad::SetVibration() and the NOXNA/EXT
// SetTriggerVibrationEXT(): Space/Enter/A/tap pulses both motor pairs at
// full strength for one second, then stops them. Always stops vibration on
// exit (UnloadContent) so leaving this screen doesn't leave a controller
// buzzing.
class VibrationScreen : public DemoScreen {
public:
    VibrationScreen() : DemoScreen("Gamepad: Vibration") {}

    void UnloadContent() override {
        GamePad::SetVibration(PlayerIndex::One, 0.0f, 0.0f);
        GamePad::SetTriggerVibrationEXT(PlayerIndex::One, 0.0f, 0.0f);
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        if (pulseSecondsRemaining_ <= 0.0f) return;
        pulseSecondsRemaining_ -= (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        if (pulseSecondsRemaining_ <= 0.0f) {
            pulseSecondsRemaining_ = 0.0f;
            GamePad::SetVibration(PlayerIndex::One, 0.0f, 0.0f);
            GamePad::SetTriggerVibrationEXT(PlayerIndex::One, 0.0f, 0.0f);
        }
    }

    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            pulseSecondsRemaining_ = 1.0f;
            lastVibrationSupported_ = GamePad::SetVibration(PlayerIndex::One, 1.0f, 1.0f);
            lastTriggerVibrationSupported_ = GamePad::SetTriggerVibrationEXT(PlayerIndex::One, 1.0f, 1.0f);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap pulses both motors and both trigger motors for 1s.");
        lines.emplace_back();
        lines.push_back(std::string("Pulsing: ") + (pulseSecondsRemaining_ > 0.0f ? "yes" : "no"));
        if (lastVibrationSupported_.has_value()) {
            lines.push_back(std::string("SetVibration() supported: ") +
                            (*lastVibrationSupported_ ? "true" : "false"));
        }
        if (lastTriggerVibrationSupported_.has_value()) {
            lines.push_back(std::string("SetTriggerVibrationEXT() supported: ") +
                            (*lastTriggerVibrationSupported_ ? "true" : "false"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    float pulseSecondsRemaining_ = 0.0f;
    std::optional<bool> lastVibrationSupported_;
    std::optional<bool> lastTriggerVibrationSupported_;
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
