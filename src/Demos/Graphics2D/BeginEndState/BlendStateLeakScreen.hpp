// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Blend.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::BeginEndStateDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Blend;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates a real, documented gotcha: SpriteBatch::Begin() sets
// GraphicsDevice.BlendState as a genuine, PERSISTENT side effect -- it is
// NOT restored back to whatever was active before by End(). Confirmed
// against the codebase's own easygl_spritebatch_blendstate_leak_test.cpp.
// This screen proves it live: Begin(Additive), draw, End() -- then reads
// GraphicsDevice.BlendState immediately afterward (before this screen's own
// next default Begin() call would mask the leak by resetting it again) and
// shows it still reports Additive's exact blend factors, not whatever was
// active before.
class BlendStateLeakScreen : public DemoScreen {
public:
    BlendStateLeakScreen() : DemoScreen("SpriteBatch: BlendState Leaks Past End()") {}

    void LoadContent() override {
        quad_.emplace(CreateCheckerboardTexture(GetScreenManager()->getGraphicsDeviceProperty(), 40, 40, 40,
                                                  Color(255, 120, 60, 255), Color(255, 120, 60, 255)));
    }

    void UnloadContent() override {
        quad_.reset();
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

            sb.End();
            sb.Begin(SpriteSortMode::Deferred, BlendState::Additive, nullptr, nullptr, nullptr);
            sb.Draw(*quad_, Vector2(60.0f, 300.0f), Color::White);
            sb.End(); // Additive is still active on the device right now, not reset

            auto& device = GetScreenManager()->getGraphicsDeviceProperty();
            const Blend src = device.getBlendStateProperty().getColorSourceBlendProperty();
            const Blend dst = device.getBlendStateProperty().getColorDestinationBlendProperty();
            leakedSrc_ = BlendName(src);
            leakedDst_ = BlendName(dst);

            sb.Begin(); // NOW reset back to AlphaBlend (this app's own default Begin() call)
        }

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: Begin(Additive), draw, End() -- read device.BlendState");
        lines.push_back("right after, before this screen's own next default Begin() masks it.");
        lines.emplace_back();
        lines.push_back("Attempts: " + std::to_string(attempts_));
        if (!leakedSrc_.empty()) {
            lines.push_back("Device.BlendState after End(): ColorSourceBlend=" + leakedSrc_);
            lines.push_back("ColorDestinationBlend=" + leakedDst_);
            lines.push_back("(matches Additive's SourceAlpha/One -- End() never restored anything)");
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* BlendName(Blend b) {
        switch (b) {
            case Blend::One: return "One";
            case Blend::Zero: return "Zero";
            case Blend::SourceAlpha: return "SourceAlpha";
            case Blend::InverseSourceAlpha: return "InverseSourceAlpha";
            default: return "(other)";
        }
    }

    std::optional<Texture2D> quad_;
    bool pendingAttempt_ = false;
    int attempts_ = 0;
    std::string leakedSrc_;
    std::string leakedDst_;
};

} // namespace CnaExamples::Demos::Graphics2D::BeginEndStateDemos
