// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::SortModesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates SpriteSortMode::Deferred -- the default: sprites are queued
// and flushed at End() in submission order. Depth is completely ignored,
// even though each of the 3 overlapping quads below is given a distinct,
// deliberately "wrong-looking" layerDepth -- the final on-top quad is
// always the one submitted LAST (quad 3), regardless of depth.
//
// This screen (and every other screen in this category) needs Begin()'d
// with an explicit SpriteSortMode, but DemoScreen::Draw() already wraps
// OnDemoDraw() in a default-parameter sb.Begin()/sb.End() pair for the
// title text. The fix needs no DemoScreen.hpp change: End() the
// already-open default batch first, run a fully custom Begin()/Draw()/End()
// sequence, then Begin() a fresh default batch again before returning so
// the base class's trailing "Back" hint still has an open batch to draw
// into.
class DeferredSortScreen : public DemoScreen {
public:
    DeferredSortScreen() : DemoScreen("SpriteBatch: SpriteSortMode::Deferred") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        quadA_.emplace(CreateCheckerboardTexture(device, 80, 80, 80, Color(255, 60, 60, 255), Color(255, 60, 60, 255)));
        quadB_.emplace(CreateCheckerboardTexture(device, 80, 80, 80, Color(60, 255, 60, 255), Color(60, 255, 60, 255)));
        quadC_.emplace(CreateCheckerboardTexture(device, 80, 80, 80, Color(60, 60, 255, 255), Color(60, 60, 255, 255)));
    }

    void UnloadContent() override {
        quadA_.reset();
        quadB_.reset();
        quadC_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("3 overlapping quads: red(depth=0.9) -> green(depth=0.1) -> blue(depth=0.5).");
        lines.push_back("Deferred ignores depth entirely: blue (submitted LAST) is always on top.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End(); // close the base class's default-parameter batch (title already flushed)
        sb.Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend, nullptr, nullptr, nullptr);
        const Color tint = mul(Color::White, TransitionAlpha());
        sb.Draw(*quadA_, Vector2(60.0f, end.Y + 20.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.9f);
        sb.Draw(*quadB_, Vector2(100.0f, end.Y + 50.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.1f);
        sb.Draw(*quadC_, Vector2(80.0f, end.Y + 80.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.5f);
        sb.End();
        sb.Begin(); // reopen a default-parameter batch for the base class's trailing Back hint
    }

private:
    std::optional<Texture2D> quadA_;
    std::optional<Texture2D> quadB_;
    std::optional<Texture2D> quadC_;
};

} // namespace CnaExamples::Demos::Graphics2D::SortModesDemos
