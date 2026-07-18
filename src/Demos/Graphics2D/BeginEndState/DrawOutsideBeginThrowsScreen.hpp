// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::BeginEndStateDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates that Draw() (and DrawString()) throw std::runtime_error if
// called while no Begin()/End() session is open.
class DrawOutsideBeginThrowsScreen : public DemoScreen {
public:
    DrawOutsideBeginThrowsScreen() : DemoScreen("SpriteBatch: Draw Outside Begin() Throws") {}

    void LoadContent() override {
        dummy_.emplace(CreateCheckerboardTexture(GetScreenManager()->getGraphicsDeviceProperty(), 8, 8, 4,
                                                   Color::White, Color::Black));
    }

    void UnloadContent() override {
        dummy_.reset();
    }

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
            sb.End(); // now genuinely not begun
            try {
                sb.Draw(*dummy_, Vector2(0.0f, 0.0f), Color::White);
                lastResult_ = "Draw() returned without throwing (unexpected)";
            } catch (const std::runtime_error& ex) {
                lastResult_ = std::string("Draw() threw: ") + ex.what();
            }
            sb.Begin(); // restore an open batch for the rest of this frame
        }

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: End(), then Draw() with no Begin() open");
        lines.emplace_back();
        lines.push_back("Attempts: " + std::to_string(attempts_));
        if (!lastResult_.empty()) lines.push_back(lastResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Texture2D> dummy_;
    bool pendingAttempt_ = false;
    int attempts_ = 0;
    std::string lastResult_;
};

} // namespace CnaExamples::Demos::Graphics2D::BeginEndStateDemos
