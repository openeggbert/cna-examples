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

// Demonstrates the classic XNA premultiplied-alpha gotcha: BlendState::
// AlphaBlend assumes the source texture's RGB is already multiplied by its
// own alpha ("premultiplied"); BlendState::NonPremultiplied performs that
// multiplication itself instead. Two textures are built at half alpha (128):
// one with RGB manually pre-multiplied by alpha/255, one left at full RGB
// intensity ("naive"). Paired correctly (premultiplied+AlphaBlend,
// naive+NonPremultiplied) both look identical and correct; paired backwards
// (swapped), both look visibly wrong -- too dark or washed out.
class PremultipliedAlphaGotchaScreen : public DemoScreen {
public:
    PremultipliedAlphaGotchaScreen() : DemoScreen("BlendState: Premultiplied Alpha") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        const int size = 64;
        const std::uint8_t alpha = 128;
        const Color baseColor(255, 200, 40, 255); // opaque orange, before alpha is applied

        // Premultiplied: RGB scaled by alpha/255 ahead of time.
        {
            auto pixels = MakePixelBuffer(size, size);
            const Color premul(
                (std::uint8_t)((int)baseColor.getRProperty() * alpha / 255),
                (std::uint8_t)((int)baseColor.getGProperty() * alpha / 255),
                (std::uint8_t)((int)baseColor.getBProperty() * alpha / 255),
                alpha);
            for (int y = 0; y < size; ++y)
                for (int x = 0; x < size; ++x)
                    SetPixel(pixels, size, x, y, premul);
            premultiplied_.emplace(Texture2D::CreateFromPixels(device, size, size, pixels));
        }

        // Naive/non-premultiplied: full RGB intensity regardless of alpha.
        {
            auto pixels = MakePixelBuffer(size, size);
            const Color naive(baseColor.getRProperty(), baseColor.getGProperty(), baseColor.getBProperty(), alpha);
            for (int y = 0; y < size; ++y)
                for (int x = 0; x < size; ++x)
                    SetPixel(pixels, size, x, y, naive);
            naive_.emplace(Texture2D::CreateFromPixels(device, size, size, pixels));
        }
    }

    void UnloadContent() override {
        premultiplied_.reset();
        naive_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Same orange color, alpha=128, built two ways. Correct pairing on the left,");
        lines.push_back("swapped (WRONG) pairing on the right -- both visibly darker/washed out.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const float y = end.Y + 20.0f;

        sb.End();
        sb.Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend, nullptr, nullptr, nullptr);
        sb.Draw(*premultiplied_, Vector2(40.0f, y), Color::White);
        sb.Draw(*naive_, Vector2(500.0f, y), Color::White);
        sb.End();

        sb.Begin(SpriteSortMode::Deferred, BlendState::NonPremultiplied, nullptr, nullptr, nullptr);
        sb.Draw(*naive_, Vector2(150.0f, y), Color::White);
        sb.Draw(*premultiplied_, Vector2(610.0f, y), Color::White);
        sb.End();
        sb.Begin();

        sb.DrawString(font, "premultiplied+AlphaBlend", Vector2(40.0f, y + 74.0f), mul(Color::White, TransitionAlpha()));
        sb.DrawString(font, "naive+NonPremultiplied", Vector2(150.0f, y + 94.0f), mul(Color::White, TransitionAlpha()));
        sb.DrawString(font, "(correct, both)", Vector2(40.0f, y + 114.0f), mul(Color(120, 255, 120, 255), TransitionAlpha()));
        sb.DrawString(font, "premultiplied+NonPremult", Vector2(500.0f, y + 74.0f), mul(Color::White, TransitionAlpha()));
        sb.DrawString(font, "naive+AlphaBlend", Vector2(610.0f, y + 94.0f), mul(Color::White, TransitionAlpha()));
        sb.DrawString(font, "(WRONG pairing, both)", Vector2(500.0f, y + 114.0f), mul(Color(255, 120, 120, 255), TransitionAlpha()));
    }

private:
    std::optional<Texture2D> premultiplied_;
    std::optional<Texture2D> naive_;
};

} // namespace CnaExamples::Demos::Graphics2D::BlendStateDemos
