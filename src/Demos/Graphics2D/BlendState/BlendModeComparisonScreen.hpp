// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::BlendStateDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates the 4 BlendState presets (Opaque, AlphaBlend, Additive,
// NonPremultiplied) by drawing the SAME two overlapping semi-transparent
// quads under each preset, cycled with B. Where the quads overlap, each
// preset produces a visibly different blended color -- this is the actual
// GraphicsDevice.BlendState in effect, not a simulated comparison.
class BlendModeComparisonScreen : public DemoScreen {
public:
    BlendModeComparisonScreen() : DemoScreen("BlendState: Mode Comparison") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        quadA_.emplace(CreateCheckerboardTexture(device, 100, 100, 100, Color(255, 60, 60, 160), Color(255, 60, 60, 160)));
        quadB_.emplace(CreateCheckerboardTexture(device, 100, 100, 100, Color(60, 120, 255, 160), Color(60, 120, 255, 160)));
    }

    void UnloadContent() override {
        quadA_.reset();
        quadB_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsNewKeyPress(Keys::B, ControllingPlayer(), playerIndex)) {
            presetIndex_ = (presetIndex_ + 1) % 4;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("B: cycle Opaque -> AlphaBlend -> Additive -> NonPremultiplied");
        lines.push_back("Current: " + std::string(PresetName(presetIndex_)));
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();
        sb.Begin(SpriteSortMode::Deferred, CurrentPreset(presetIndex_), nullptr, nullptr, nullptr);
        sb.Draw(*quadA_, Vector2(80.0f, end.Y + 20.0f), Color::White);
        sb.Draw(*quadB_, Vector2(150.0f, end.Y + 60.0f), Color::White);
        sb.End();
        sb.Begin();
    }

private:
    static BlendState CurrentPreset(int index) {
        switch (index) {
            case 0: return BlendState::Opaque;
            case 1: return BlendState::AlphaBlend;
            case 2: return BlendState::Additive;
            default: return BlendState::NonPremultiplied;
        }
    }

    static const char* PresetName(int index) {
        switch (index) {
            case 0: return "Opaque";
            case 1: return "AlphaBlend";
            case 2: return "Additive";
            default: return "NonPremultiplied";
        }
    }

    std::optional<Texture2D> quadA_;
    std::optional<Texture2D> quadB_;
    int presetIndex_ = 1;
};

} // namespace CnaExamples::Demos::Graphics2D::BlendStateDemos
