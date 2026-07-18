// SPDX-License-Identifier: MIT
#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadTriggers.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadState;

// Demonstrates GamePadState::Triggers (Left/Right, each 0..1) with a simple
// text-character progress bar -- also demonstrates GamePad::TriggerThreshold
// (the XInput-based "is this trigger meaningfully pressed" cutoff FNA uses
// for IsButtonDown(Buttons::LeftTrigger/RightTrigger)).
class TriggersScreen : public DemoScreen {
public:
    TriggersScreen() : DemoScreen("Gamepad: Triggers") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const GamePadState state = GamePad::GetState(PlayerIndex::One);
        const auto& triggers = state.getTriggersProperty();

        std::vector<std::string> lines;
        lines.push_back("PlayerIndex::One's analog triggers, 0..1:");
        lines.emplace_back();
        AppendBar(lines, "Left ", triggers.getLeftProperty());
        AppendBar(lines, "Right", triggers.getRightProperty());
        lines.emplace_back();

        char buf[64];
        std::snprintf(buf, sizeof(buf), "GamePad::TriggerThreshold = %.4f", GamePad::TriggerThreshold);
        lines.push_back(buf);
        lines.push_back(std::string("Left  IsButtonDown(LeftTrigger):  ") +
                        (state.IsButtonDown(Buttons::LeftTrigger) ? "true" : "false"));
        lines.push_back(std::string("Right IsButtonDown(RightTrigger): ") +
                        (state.IsButtonDown(Buttons::RightTrigger) ? "true" : "false"));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static void AppendBar(std::vector<std::string>& lines, const char* label, float value) {
        constexpr int kWidth = 30;
        const int filled = (int)(value * kWidth + 0.5f);
        std::string bar(filled, '#');
        bar.append(kWidth - filled, '.');
        char buf[96];
        std::snprintf(buf, sizeof(buf), "%s [%s] %.3f", label, bar.c_str(), value);
        lines.push_back(buf);
    }
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
