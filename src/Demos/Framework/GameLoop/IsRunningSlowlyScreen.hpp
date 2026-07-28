// SPDX-License-Identifier: MIT
#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Game.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Framework/FrameworkDemoHelpers.hpp"

namespace CnaExamples::Demos::Framework::GameLoopDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// GameTime::IsRunningSlowly is the fixed-timestep loop telling you it could not
// keep up: Update took longer than TargetElapsedTime, so the loop is now behind
// and is running Update repeatedly to catch up.
//
// It cannot be demonstrated by describing it, so this screen deliberately burns
// CPU inside Update -- a real busy-wait, adjustable from 0 to well past the
// 16.67 ms budget -- and shows the flag reacting. The load is a spin loop rather
// than a sleep on purpose: sleeping would release the core and is not what a
// heavy Update actually does to the loop.
//
// The flag only ever means anything with IsFixedTimeStep == true; a variable
// loop is never "behind", it just produces a bigger delta. The screen shows both
// so the distinction is visible rather than asserted.
class IsRunningSlowlyScreen : public DemoScreen {
public:
    IsRunningSlowlyScreen() : DemoScreen("Game Loop: IsRunningSlowly") {}

    void OnDemoLoad() override {
        Game& game = GameRef();
        originalIsFixed_ = game.getIsFixedTimeStepProperty();
        game.setIsFixedTimeStepProperty(true);
    }

    void OnDemoUnload() override {
        GameRef().setIsFixedTimeStepProperty(originalIsFixed_);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            loadIndex_ = (loadIndex_ + 1) % kLoadCount;
            slowFrames_ = 0;
            totalFrames_ = 0;
            history_.Clear();
        }
        if (input.IsMenuUp(ControllingPlayer())) {
            GameRef().setIsFixedTimeStepProperty(!GameRef().getIsFixedTimeStepProperty());
            slowFrames_ = 0;
            totalFrames_ = 0;
        }
    }

    void OnDemoUpdate(GameTime& gameTime) override {
        BurnMilliseconds(kLoadsMs[loadIndex_]);

        history_.Add(gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty());
        totalFrames_++;
        if (gameTime.getIsRunningSlowlyProperty()) slowFrames_++;
        lastIsRunningSlowly_ = gameTime.getIsRunningSlowlyProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        Game& game = GameRef();
        const double budgetMs = game.getTargetElapsedTimeProperty().getTotalMillisecondsProperty();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: cycle the artificial Update load   Up: toggle IsFixedTimeStep");
        lines.emplace_back();

        char load[96];
        std::snprintf(load, sizeof(load), "%.1f ms of busy-wait per Update", kLoadsMs[loadIndex_]);
        lines.push_back(std::string("Artificial load:   ") + load);
        char budget[96];
        std::snprintf(budget, sizeof(budget), "%.2f ms (TargetElapsedTime)", budgetMs);
        lines.push_back(std::string("Per-frame budget:  ") + budget);
        lines.push_back("IsFixedTimeStep:   " +
                        std::string(game.getIsFixedTimeStepProperty() ? "true" : "false"));
        lines.emplace_back();
        lines.push_back("GameTime::IsRunningSlowly: " +
                        std::string(lastIsRunningSlowly_ ? "TRUE -- the loop is behind"
                                                         : "false -- keeping up"));
        lines.push_back("Slow frames: " + std::to_string(slowFrames_) + " of " +
                        std::to_string(totalFrames_));

        char stats[128];
        std::snprintf(stats, sizeof(stats), "min %.2f  mean %.2f  max %.2f ms",
                      history_.Min(), history_.Mean(), history_.Max());
        lines.push_back(std::string("Frame deltas: ") + stats);
        lines.emplace_back();
        lines.push_back(game.getIsFixedTimeStepProperty()
            ? "Raise the load past the budget and the flag latches on: the loop cannot hold the rate."
            : "With a variable timestep the loop is never 'behind' -- the delta just grows instead.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr int kLoadCount = 4;
    static constexpr double kLoadsMs[kLoadCount] = {0.0, 8.0, 20.0, 40.0};

    Game& GameRef() const { return GetScreenManager()->getGameProperty(); }

    // A real spin, not a sleep: a sleeping Update yields the core, which is not
    // what an overrunning game loop does and would not reproduce the symptom.
    static void BurnMilliseconds(double milliseconds) {
        if (milliseconds <= 0.0) return;
        const auto deadline = std::chrono::steady_clock::now() +
                              std::chrono::duration<double, std::milli>(milliseconds);
        // volatile so the loop cannot be optimised away entirely.
        volatile double sink = 0.0;
        while (std::chrono::steady_clock::now() < deadline) {
            sink = sink + 1.0;
        }
        (void)sink;
    }

    FrameDeltaHistory history_;
    bool originalIsFixed_ = true;
    bool lastIsRunningSlowly_ = false;
    int loadIndex_ = 0;
    int slowFrames_ = 0;
    int totalFrames_ = 0;
};

} // namespace CnaExamples::Demos::Framework::GameLoopDemos
