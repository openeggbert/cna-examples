// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadDeadZone.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadDeadZone;
using Microsoft::Xna::Framework::Input::GamePadState;

// Demonstrates GamePad::GetState(PlayerIndex, GamePadDeadZone): the same
// physical left-stick position read three ways -- None (raw, no
// processing), IndependentAxes (GetState's default -- each axis clamped to
// zero independently), and Circular (the combined vector clamped to zero
// as a whole, better for 2D control-surface use like a first-person look
// stick). Small stick jitter around center is visible in None but not in
// the two processed modes.
class DeadZoneModesScreen : public DemoScreen {
public:
    DeadZoneModesScreen() : DemoScreen("Gamepad: Dead Zone Modes") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("PlayerIndex::One's left stick under each dead zone mode:");
        lines.emplace_back();

        AppendMode(lines, "None", GamePadDeadZone::None);
        AppendMode(lines, "IndependentAxes (default)", GamePadDeadZone::IndependentAxes);
        AppendMode(lines, "Circular", GamePadDeadZone::Circular);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static void AppendMode(std::vector<std::string>& lines, const char* label, GamePadDeadZone mode) {
        const GamePadState state = GamePad::GetState(PlayerIndex::One, mode);
        const auto& left = state.getThumbSticksProperty().getLeftProperty();
        lines.push_back(std::string(label) + ": (" + std::to_string(left.X) + ", " +
                        std::to_string(left.Y) + ")");
    }
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
