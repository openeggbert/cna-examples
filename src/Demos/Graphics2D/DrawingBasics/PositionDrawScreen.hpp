// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::DrawingBasicsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates the two simplest SpriteBatch::Draw() overloads:
// Draw(texture, Vector2 position, Color) and the NOXNA convenience
// Draw(texture, float x, float y) -- both draw the whole texture unscaled,
// unrotated, at a fixed position.
class PositionDrawScreen : public DemoScreen {
public:
    PositionDrawScreen() : DemoScreen("SpriteBatch: Position Draw") {}

    void LoadContent() override {
        icon_.emplace(CreateCheckerboardTexture(
            GetScreenManager()->getGraphicsDeviceProperty(), 48, 48, 12,
            Color(255, 200, 60, 255), Color(60, 90, 255, 255)));
    }

    void UnloadContent() override {
        icon_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Draw(texture, Vector2 position, Color) -- the standard XNA overload.");
        lines.push_back("Draw(texture, float x, float y) -- a NOXNA convenience (no XNA equivalent).");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.Draw(*icon_, Vector2(60.0f, end.Y + 30.0f), mul(Color::White, TransitionAlpha()));
        sb.DrawString(font, "Vector2 position", Vector2(140.0f, end.Y + 45.0f), mul(Color::White, TransitionAlpha()));

        sb.Draw(*icon_, 60.0f, end.Y + 100.0f);
        sb.DrawString(font, "float x, float y (NOXNA)", Vector2(140.0f, end.Y + 115.0f), mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Texture2D> icon_;
};

} // namespace CnaExamples::Demos::Graphics2D::DrawingBasicsDemos
