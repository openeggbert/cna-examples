// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::SamplerStateDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::SamplerState;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates SamplerState::PointClamp vs. LinearClamp -- a real,
// EasyGL-pixel-verified difference (this app's own research confirmed
// Filter is genuinely applied to SpriteBatch draws, not a no-op). A small
// 8x8 checkerboard is drawn heavily upscaled to 200x200 under each filter:
// PointClamp stays crisp/blocky (hard pixel edges), LinearClamp interpolates
// into a smooth/blurry result.
class FilterComparisonScreen : public DemoScreen {
public:
    FilterComparisonScreen() : DemoScreen("SamplerState: Filter Comparison") {}

    void LoadContent() override {
        small_.emplace(CreateCheckerboardTexture(GetScreenManager()->getGraphicsDeviceProperty(), 8, 8, 2,
                                                   Color(255, 200, 60, 255), Color(60, 90, 255, 255)));
    }

    void UnloadContent() override {
        small_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("The same 8x8 texture, blown up to 200x200, under 2 different filters.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const float y = end.Y + 20.0f;

        SamplerState pointClamp = SamplerState::PointClamp;
        sb.End();
        sb.Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend, &pointClamp, nullptr, nullptr);
        sb.Draw(*small_, Rectangle(40, (int)y, 200, 200), Color::White);
        sb.End();

        SamplerState linearClamp = SamplerState::LinearClamp;
        sb.Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend, &linearClamp, nullptr, nullptr);
        sb.Draw(*small_, Rectangle(300, (int)y, 200, 200), Color::White);
        sb.End();
        sb.Begin();

        const Color tint = mul(Color::White, TransitionAlpha());
        sb.DrawString(font, "PointClamp", Vector2(40.0f, y + 210.0f), tint);
        sb.DrawString(font, "(crisp/blocky)", Vector2(40.0f, y + 235.0f), tint);
        sb.DrawString(font, "LinearClamp", Vector2(300.0f, y + 210.0f), tint);
        sb.DrawString(font, "(smooth/blurry)", Vector2(300.0f, y + 235.0f), tint);
    }

private:
    std::optional<Texture2D> small_;
};

} // namespace CnaExamples::Demos::Graphics2D::SamplerStateDemos
