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

// Demonstrates SpriteSortMode::Immediate -- each Draw() call is flushed to
// the GPU right away as its own draw call, instead of being queued until
// End(). For this simple, single-unchanging-state scenario the final image
// is pixel-identical to Deferred (same 3 quads, same submission order, same
// "last submitted wins" overlap result) -- honestly labeled as such rather
// than staged to look different. The real-world reason to reach for
// Immediate is being able to change device state (e.g. BlendState) between
// individual Draw() calls and have each sprite actually observe the state
// active at the moment IT drew, not whatever was last set by End() time.
class ImmediateSortScreen : public DemoScreen {
public:
    ImmediateSortScreen() : DemoScreen("SpriteBatch: SpriteSortMode::Immediate") {}

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
        lines.push_back("Same 3 quads, same submission order -- but each Draw() is its own GPU");
        lines.push_back("call here, not queued. Visually identical to Deferred for this case.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();
        sb.Begin(SpriteSortMode::Immediate, BlendState::AlphaBlend, nullptr, nullptr, nullptr);
        const Color tint = mul(Color::White, TransitionAlpha());
        sb.Draw(*quadA_, Vector2(60.0f, end.Y + 20.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.9f);
        sb.Draw(*quadB_, Vector2(100.0f, end.Y + 50.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.1f);
        sb.Draw(*quadC_, Vector2(80.0f, end.Y + 80.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.5f);
        sb.End();
        sb.Begin();
    }

private:
    std::optional<Texture2D> quadA_;
    std::optional<Texture2D> quadB_;
    std::optional<Texture2D> quadC_;
};

} // namespace CnaExamples::Demos::Graphics2D::SortModesDemos
