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

// Demonstrates rotation + origin together, live-animated via GameTime:
// origin = texture center makes it spin in place; origin = (0,0) (a
// corner) makes the same rotation orbit the draw position instead --
// origin is measured in texture-local pixels and affects both rotation
// pivot and where "position" lands on the sprite.
class RotationOriginScreen : public DemoScreen {
public:
    RotationOriginScreen() : DemoScreen("SpriteBatch: Rotation & Origin") {}

    void LoadContent() override {
        std::vector<Color> palette = {
            Color(255, 80, 80, 255), Color(80, 255, 80, 255),
            Color(80, 80, 255, 255), Color(255, 255, 80, 255),
        };
        marker_.emplace(CreateGridTexture(GetScreenManager()->getGraphicsDeviceProperty(), 2, 2, 20, palette));
    }

    void UnloadContent() override {
        marker_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        angle_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 1.5f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Draw(..., rotation, origin, scale, effects, layerDepth) -- animated live.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const Color tint = mul(Color::White, TransitionAlpha());

        const Vector2 centerOrigin(20.0f, 20.0f);
        sb.Draw(*marker_, Vector2(140.0f, end.Y + 100.0f), std::nullopt, tint,
                angle_, centerOrigin, 1.0f, SpriteEffects::None, 0.0f);
        sb.DrawString(font, "origin = center (spins in place)", Vector2(40.0f, end.Y + 170.0f), tint);

        const Vector2 cornerOrigin(0.0f, 0.0f);
        sb.Draw(*marker_, Vector2(340.0f, end.Y + 100.0f), std::nullopt, tint,
                angle_, cornerOrigin, 1.0f, SpriteEffects::None, 0.0f);
        sb.DrawString(font, "origin = (0,0) corner (orbits the position)", Vector2(40.0f, end.Y + 195.0f), tint);
    }

private:
    std::optional<Texture2D> marker_;
    float angle_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics2D::DrawingBasicsDemos
