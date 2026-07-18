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

// Demonstrates SpriteSortMode::BackToFront -- sprites are stable-sorted by
// layerDepth DESCENDING before flushing (largest depth drawn first). 3
// overlapping quads are submitted in a scrambled order (green, then blue,
// then red) with depths 0.1/0.5/0.9; BackToFront draws red(0.9) first,
// blue(0.5) next, green(0.1) LAST -- so green ends up visually on top,
// regardless of the scrambled submission order. Compare directly against
// FrontToBackSortScreen, which uses the exact same quads/depths/submission
// order and produces the opposite top layer.
class BackToFrontSortScreen : public DemoScreen {
public:
    BackToFrontSortScreen() : DemoScreen("SpriteBatch: SpriteSortMode::BackToFront") {}

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
        lines.push_back("Submitted green(0.1), blue(0.5), red(0.9) -- scrambled submission order.");
        lines.push_back("BackToFront draws largest depth FIRST: red, then blue, then green on top.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();
        sb.Begin(SpriteSortMode::BackToFront, BlendState::AlphaBlend, nullptr, nullptr, nullptr);
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
