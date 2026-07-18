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

// Demonstrates SamplerState addressing (Wrap vs. Clamp): a small 8x8
// texture is drawn with a deliberately OVERSIZED source rectangle
// (32x32, 4x the texture's own size) into a large destination rectangle --
// reading UV coordinates past the texture's own [0,1] range. Under
// PointWrap the texture tiles/repeats to fill the region (visibly 4x4
// repeats of the small pattern); under PointClamp the edge pixels smear/
// stretch outward instead. Point filtering is used for both so the
// tiling/smearing boundary itself stays crisp, not blurred by interpolation.
class AddressModeScreen : public DemoScreen {
public:
    AddressModeScreen() : DemoScreen("SamplerState: Address Mode") {}

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
        lines.push_back("An 8x8 texture, sampled with a 32x32 (4x oversized) source rectangle.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const float y = end.Y + 20.0f;
        const Rectangle oversizedSrc(0, 0, 32, 32);

        SamplerState pointWrap = SamplerState::PointWrap;
        sb.End();
        sb.Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend, &pointWrap, nullptr, nullptr);
        sb.Draw(*small_, Rectangle(40, (int)y, 200, 200), oversizedSrc, Color::White);
        sb.End();

        SamplerState pointClamp = SamplerState::PointClamp;
        sb.Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend, &pointClamp, nullptr, nullptr);
        sb.Draw(*small_, Rectangle(300, (int)y, 200, 200), oversizedSrc, Color::White);
        sb.End();
        sb.Begin();

        const Color tint = mul(Color::White, TransitionAlpha());
        sb.DrawString(font, "PointWrap (tiles 4x4)", Vector2(40.0f, y + 210.0f), tint);
        sb.DrawString(font, "PointClamp (edges smear)", Vector2(300.0f, y + 210.0f), tint);
    }

private:
    std::optional<Texture2D> small_;
};

} // namespace CnaExamples::Demos::Graphics2D::SamplerStateDemos
