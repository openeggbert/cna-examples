// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::DrawingBasicsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates float vs. Vector2 scale (uniform vs. non-uniform), plus
// SpriteEffects::FlipHorizontally/FlipVertically -- the marker texture has
// 4 differently-colored quadrants specifically so a flip is visually
// unambiguous (colors swap corners, not just a symmetric mirror).
class ScaleAndEffectsScreen : public DemoScreen {
public:
    ScaleAndEffectsScreen() : DemoScreen("SpriteBatch: Scale & SpriteEffects") {}

    void LoadContent() override {
        std::vector<Color> palette = {
            Color(255, 80, 80, 255), Color(80, 255, 80, 255),
            Color(80, 80, 255, 255), Color(255, 255, 80, 255),
        };
        marker_.emplace(CreateGridTexture(GetScreenManager()->getGraphicsDeviceProperty(), 2, 2, 16, palette));
    }

    void UnloadContent() override {
        marker_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Top-left=red, top-right=green, bottom-left=blue, bottom-right=yellow.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const Color tint = mul(Color::White, TransitionAlpha());
        const float y = end.Y + 20.0f;

        const float labelY = y + 60.0f;

        sb.Draw(*marker_, Vector2(40.0f, y), std::nullopt, tint, 0.0f, Vector2::Zero,
                1.0f, SpriteEffects::None, 0.0f);
        sb.DrawString(font, "normal", Vector2(40.0f, labelY), tint);

        sb.Draw(*marker_, Vector2(140.0f, y), std::nullopt, tint, 0.0f, Vector2::Zero,
                2.5f, SpriteEffects::None, 0.0f);
        sb.DrawString(font, "float scale=2.5", Vector2(140.0f, labelY), tint);

        sb.Draw(*marker_, Vector2(320.0f, y), std::nullopt, tint, 0.0f, Vector2::Zero,
                Vector2(1.0f, 3.0f), SpriteEffects::None, 0.0f);
        sb.DrawString(font, "Vector2 scale(1,3)", Vector2(320.0f, labelY), tint);

        sb.Draw(*marker_, Vector2(500.0f, y), std::nullopt, tint, 0.0f, Vector2::Zero,
                1.0f, SpriteEffects::FlipHorizontally, 0.0f);
        sb.DrawString(font, "FlipHorizontally", Vector2(500.0f, labelY), tint);

        sb.Draw(*marker_, Vector2(620.0f, y), std::nullopt, tint, 0.0f, Vector2::Zero,
                1.0f, SpriteEffects::FlipVertically, 0.0f);
        sb.DrawString(font, "FlipVertically", Vector2(620.0f, labelY), tint);
    }

private:
    std::optional<Texture2D> marker_;
};

} // namespace CnaExamples::Demos::Graphics2D::DrawingBasicsDemos
