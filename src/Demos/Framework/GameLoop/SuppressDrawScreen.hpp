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

// Two loop-control methods that exist for opposite reasons:
//
//   Game::SuppressDraw() -- skip the *next* Draw only. For a frame where
//       nothing changed (a paused game, a static menu), this saves the whole
//       render without changing the update rate. It is a one-shot: it must be
//       called again for every frame that should be skipped.
//
//   Game::ResetElapsedTime() -- tell the loop to forget how far behind it is.
//       After a long stall (loading a level, a breakpoint), a fixed-timestep
//       loop would otherwise run Update many times in a row to "catch up".
//       Calling this drops that debt.
//
// SuppressDraw is genuinely observable here: the frame counter keeps rising
// while the drawn counter stops, and the screen visibly freezes on the last
// rendered frame while continuing to update underneath.
class SuppressDrawScreen : public DemoScreen {
public:
    SuppressDrawScreen() : DemoScreen("Game Loop: SuppressDraw & ResetElapsedTime") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            // Suppress a run of frames so the effect lasts long enough to see;
            // one suppressed frame at 60 Hz is 16 ms and invisible.
            suppressFramesLeft_ = 60;
        }
        if (input.IsMenuUp(ControllingPlayer())) {
            GetScreenManager()->getGameProperty().ResetElapsedTime();
            resetCalls_++;
            sinceReset_ = 0;
        }
    }

    void OnDemoUpdate(GameTime& gameTime) override {
        updates_++;
        sinceReset_++;
        lastElapsedMs_ = gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty();

        if (suppressFramesLeft_ > 0) {
            // One-shot per frame: it has to be re-armed every single frame.
            GetScreenManager()->getGameProperty().SuppressDraw();
            suppressFramesLeft_--;
            suppressed_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        draws_++;

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: SuppressDraw() for the next 60 frames");
        lines.push_back("Up: ResetElapsedTime()");
        lines.emplace_back();
        lines.push_back("Update calls: " + std::to_string(updates_));
        lines.push_back("Draw calls:   " + std::to_string(draws_));
        lines.push_back("Difference:   " + std::to_string(updates_ - draws_) +
                        "   (frames updated but never drawn)");
        lines.emplace_back();
        lines.push_back("SuppressDraw() calls: " + std::to_string(suppressed_));
        lines.push_back("Still suppressing:    " + std::to_string(suppressFramesLeft_) + " frame(s)");
        lines.emplace_back();
        lines.push_back("ResetElapsedTime() calls: " + std::to_string(resetCalls_));
        lines.push_back("Updates since last reset: " + std::to_string(sinceReset_));

        char elapsed[64];
        std::snprintf(elapsed, sizeof(elapsed), "%.2f ms", lastElapsedMs_);
        lines.push_back(std::string("Last ElapsedGameTime:     ") + elapsed);
        lines.emplace_back();
        lines.push_back("While suppressed the screen freezes but the Update counter keeps rising --");
        lines.push_back("that gap between the two counters is the whole point of SuppressDraw().");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int updates_ = 0;
    int draws_ = 0;
    int suppressed_ = 0;
    int suppressFramesLeft_ = 0;
    int resetCalls_ = 0;
    int sinceReset_ = 0;
    double lastElapsedMs_ = 0.0;
};

} // namespace CnaExamples::Demos::Framework::GameLoopDemos
