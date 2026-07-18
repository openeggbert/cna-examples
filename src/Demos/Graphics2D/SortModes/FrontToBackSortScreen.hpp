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

// Demonstrates SpriteSortMode::FrontToBack -- sprites are stable-sorted by
// layerDepth ASCENDING before flushing (smallest depth drawn first). Same
// 3 quads, same scrambled submission order, same depths as
// BackToFrontSortScreen -- but here green(0.1) draws first and red(0.9)
// draws LAST, landing on top. The opposite top layer from BackToFront,
// directly comparable by visiting both screens back to back.
class FrontToBackSortScreen : public DemoScreen {
public:
    FrontToBackSortScreen() : DemoScreen("SpriteBatch: SpriteSortMode::FrontToBack") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        red_.emplace(CreateCheckerboardTexture(device, 80, 80, 80, Color(255, 60, 60, 255), Color(255, 60, 60, 255)));
        green_.emplace(CreateCheckerboardTexture(device, 80, 80, 80, Color(60, 255, 60, 255), Color(60, 255, 60, 255)));
        blue_.emplace(CreateCheckerboardTexture(device, 80, 80, 80, Color(60, 60, 255, 255), Color(60, 60, 255, 255)));
    }

    void UnloadContent() override {
        red_.reset();
        green_.reset();
        blue_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Same green(0.1), blue(0.5), red(0.9), same scrambled submission order.");
        lines.push_back("FrontToBack draws smallest depth FIRST: green, then blue, then red on top.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();
        sb.Begin(SpriteSortMode::FrontToBack, BlendState::AlphaBlend, nullptr, nullptr, nullptr);
        const Color tint = mul(Color::White, TransitionAlpha());
        sb.Draw(*green_, Vector2(60.0f, end.Y + 20.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.1f);
        sb.Draw(*blue_, Vector2(100.0f, end.Y + 50.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.5f);
        sb.Draw(*red_, Vector2(80.0f, end.Y + 80.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.9f);
        sb.End();
        sb.Begin();
    }

private:
    std::optional<Texture2D> red_;
    std::optional<Texture2D> green_;
    std::optional<Texture2D> blue_;
};

} // namespace CnaExamples::Demos::Graphics2D::SortModesDemos
