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

// Game::TargetElapsedTime is the tick length a fixed-timestep loop aims for.
// Cycling it between 60, 30 and 10 Hz shows what actually changes and, just as
// importantly, what does not:
//
//   - ElapsedGameTime follows it exactly (that is the point of a fixed step).
//   - TotalGameTime keeps advancing at the same wall-clock rate: a slower tick
//     means fewer, larger increments, not slower time. Games that tie movement
//     to ElapsedGameTime therefore run at the same speed at any tick rate,
//     which is the whole reason to use it instead of counting frames.
//
// The moving bar is driven by TotalGameTime, so it keeps the same real speed
// while the tick rate changes underneath it -- visibly choppier at 10 Hz, not
// slower.
class TargetElapsedTimeScreen : public DemoScreen {
public:
    TargetElapsedTimeScreen() : DemoScreen("Game Loop: TargetElapsedTime") {}

    void OnDemoLoad() override {
        Game& game = GameRef();
        originalTarget_ = game.getTargetElapsedTimeProperty();
        originalIsFixed_ = game.getIsFixedTimeStepProperty();
        // A target tick length only means anything to a fixed-step loop.
        game.setIsFixedTimeStepProperty(true);
        Apply();
    }

    void OnDemoUnload() override {
        Game& game = GameRef();
        game.setTargetElapsedTimeProperty(originalTarget_);
        game.setIsFixedTimeStepProperty(originalIsFixed_);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            rateIndex_ = (rateIndex_ + 1) % kRateCount;
            Apply();
            history_.Clear();
        }
    }

    void OnDemoUpdate(GameTime& gameTime) override {
        history_.Add(gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty());
        totalSeconds_ = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();
        updates_++;
    }

    void OnDemoDraw(const GameTime& gameTime, SpriteBatch& sb, SpriteFont& font) override {
        Game& game = GameRef();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: cycle TargetElapsedTime (60 / 30 / 10 Hz)");
        lines.emplace_back();
        lines.push_back("TargetElapsedTime: " + FormatMs(game.getTargetElapsedTimeProperty()) +
                        "   (" + FormatHz(game.getTargetElapsedTimeProperty()) + ")");
        lines.push_back("IsFixedTimeStep:   " +
                        std::string(game.getIsFixedTimeStepProperty() ? "true" : "false") +
                        "   (forced on by this screen)");
        lines.emplace_back();
        lines.push_back("ElapsedGameTime: " + FormatMs(gameTime.getElapsedGameTimeProperty()));

        char stats[128];
        std::snprintf(stats, sizeof(stats), "measured mean %.2f ms over %d frames",
                      history_.Mean(), (int)history_.Count());
        lines.push_back(std::string("Measured:        ") + stats);
        lines.push_back("Update calls on this screen: " + std::to_string(updates_));

        char totals[96];
        std::snprintf(totals, sizeof(totals), "%.2f s", totalSeconds_);
        lines.push_back(std::string("TotalGameTime:   ") + totals +
                        "   (advances at wall-clock rate regardless of tick rate)");

        const Vector2 after =
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        DrawMovingBar(sb, font, after.Y + 14.0f);
    }

private:
    static constexpr int kRateCount = 3;
    static constexpr double kRatesHz[kRateCount] = {60.0, 30.0, 10.0};

    Game& GameRef() const { return GetScreenManager()->getGameProperty(); }

    void Apply() {
        GameRef().setTargetElapsedTimeProperty(
            System::TimeSpan::FromSeconds(1.0 / kRatesHz[rateIndex_]));
    }

    // Driven by TotalGameTime, not by a per-frame increment, so it demonstrates
    // the point above: the same real-world speed at every tick rate.
    void DrawMovingBar(SpriteBatch& sb, SpriteFont& font, float top) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const int left = 40;
        const int width = viewport.getWidthProperty() - 2 * left;
        const float alpha = TransitionAlpha();

        sb.DrawString(font, "Position driven by TotalGameTime (same speed at every tick rate):",
                      Vector2((float)left, top), mul(Color(150, 150, 150), alpha));

        const int trackY = (int)top + 44;
        FillRect(sb, Rectangle(left, trackY, width, 3), mul(Color(60, 60, 60), alpha));

        // A 3-second sweep back and forth.
        const double phase = std::fmod(totalSeconds_, 3.0) / 3.0;
        const double bounced = phase < 0.5 ? phase * 2.0 : (1.0 - phase) * 2.0;
        const int x = left + (int)(bounced * (width - 24));
        FillRect(sb, Rectangle(x, trackY - 10, 24, 22), mul(Color(90, 200, 120), alpha));
    }

    FrameDeltaHistory history_;
    System::TimeSpan originalTarget_;
    bool originalIsFixed_ = true;
    int rateIndex_ = 0;
    int updates_ = 0;
    double totalSeconds_ = 0.0;
};

} // namespace CnaExamples::Demos::Framework::GameLoopDemos
