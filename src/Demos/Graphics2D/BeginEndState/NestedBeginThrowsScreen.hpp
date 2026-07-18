// SPDX-License-Identifier: MIT
#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::BeginEndStateDemos {

using namespace CnaExamples::GameStateManagement;

// Demonstrates that calling SpriteBatch::Begin() a second time before
// End() throws std::runtime_error("Begin has been called before calling
// End."). OnDemoInput can't touch `sb` directly (it isn't passed there),
// so a pending-attempt flag defers the actual Begin/End manipulation into
// OnDemoDraw, where `sb` is available.
class NestedBeginThrowsScreen : public DemoScreen {
public:
    NestedBeginThrowsScreen() : DemoScreen("SpriteBatch: Nested Begin() Throws") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            pendingAttempt_ = true;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        if (pendingAttempt_) {
            pendingAttempt_ = false;
            attempts_++;
            sb.End();   // close the base class's already-open default batch
            sb.Begin(); // open our own -- this one succeeds
            try {
                sb.Begin(); // nested -- should throw
                lastResult_ = "nested Begin() returned without throwing (unexpected)";
            } catch (const std::runtime_error& ex) {
                lastResult_ = std::string("nested Begin() threw: ") + ex.what();
            }
            sb.End();   // close the still-open first Begin()
            sb.Begin(); // reopen a default batch for the rest of this frame
        }

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: Begin(), then Begin() again before End()");
        lines.emplace_back();
        lines.push_back("Attempts: " + std::to_string(attempts_));
        if (!lastResult_.empty()) lines.push_back(lastResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    bool pendingAttempt_ = false;
    int attempts_ = 0;
    std::string lastResult_;
};

} // namespace CnaExamples::Demos::Graphics2D::BeginEndStateDemos
