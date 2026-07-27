// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Game.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Framework/FrameworkDemoHelpers.hpp"

namespace CnaExamples::Demos::Framework::GameLoopDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// Game::IsFixedTimeStep is the single most consequential switch in the whole
// game loop, and the difference is only visible in the *spread* of successive
// frame deltas, not in any one of them.
//
//   fixed (XNA's default, true): Update is called with a constant
//       ElapsedGameTime of exactly TargetElapsedTime, and the loop sleeps to
//       hold that rate. Deltas cluster tightly around 16.67 ms.
//   variable (false): Update is called as fast as the loop goes round, and
//       ElapsedGameTime reports however long the last frame really took.
//
// This screen toggles the flag live and shows min/mean/max over a rolling
// window of the last 60 frames, so the clustering (or lack of it) is the thing
// on screen rather than a claim in a comment.
class FixedVsVariableTimeStepScreen : public DemoScreen {
public:
    FixedVsVariableTimeStepScreen() : DemoScreen("Game Loop: Fixed vs Variable Time Step") {}

    void OnDemoLoad() override {
        originalIsFixed_ = GameRef().getIsFixedTimeStepProperty();
    }

    void OnDemoUnload() override {
        // IsFixedTimeStep is global to the Game; leaving it flipped would change
        // how every other demo behaves for the rest of the session.
        GameRef().setIsFixedTimeStepProperty(originalIsFixed_);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            GameRef().setIsFixedTimeStepProperty(!GameRef().getIsFixedTimeStepProperty());
            history_.Clear();   // old samples describe the other mode
        }
    }

    void OnDemoUpdate(GameTime& gameTime) override {
        history_.Add(gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty());
        totalFrames_++;
    }

    void OnDemoDraw(const GameTime& gameTime, SpriteBatch& sb, SpriteFont& font) override {
        Game& game = GameRef();
        const bool fixed = game.getIsFixedTimeStepProperty();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: toggle Game::IsFixedTimeStep");
        lines.emplace_back();
        lines.push_back(std::string("IsFixedTimeStep:   ") + (fixed ? "true (XNA default)" : "false"));
        lines.push_back("TargetElapsedTime: " + FormatMs(game.getTargetElapsedTimeProperty()) +
                        "   (" + FormatHz(game.getTargetElapsedTimeProperty()) + ")");
        lines.emplace_back();
        lines.push_back("GameTime this frame:");
        lines.push_back("  ElapsedGameTime: " +
                        FormatMs(gameTime.getElapsedGameTimeProperty()));
        lines.push_back("  TotalGameTime:   " +
                        FormatMs(gameTime.getTotalGameTimeProperty()));
        lines.push_back("  IsRunningSlowly: " +
                        std::string(gameTime.getIsRunningSlowlyProperty() ? "true" : "false"));
        lines.emplace_back();

        char stats[128];
        std::snprintf(stats, sizeof(stats), "  min %.2f   mean %.2f   max %.2f   spread %.2f ms",
                      history_.Min(), history_.Mean(), history_.Max(),
                      history_.Max() - history_.Min());
        lines.push_back("Last " + std::to_string((int)history_.Count()) + " frame deltas:");
        lines.push_back(stats);
        lines.push_back("Frames on this screen: " + std::to_string(totalFrames_));
        lines.emplace_back();
        lines.push_back(fixed
            ? "Fixed: ElapsedGameTime is pinned to TargetElapsedTime, so the spread stays small."
            : "Variable: ElapsedGameTime is whatever the frame really took, so the spread grows.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    Game& GameRef() const { return GetScreenManager()->getGameProperty(); }

    FrameDeltaHistory history_;
    bool originalIsFixed_ = true;
    int totalFrames_ = 0;
};

} // namespace CnaExamples::Demos::Framework::GameLoopDemos
