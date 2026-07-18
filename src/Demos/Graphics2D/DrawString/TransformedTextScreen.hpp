// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::DrawStringDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;

// Demonstrates DrawString's rotation/origin/scale overload -- text spinning
// live around its own measured center, animated via GameTime.
class TransformedTextScreen : public DemoScreen {
public:
    TransformedTextScreen() : DemoScreen("SpriteBatch: DrawString Transform") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        angle_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 1.0f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("DrawString(font, text, position, color, rotation, origin, scale, effects, depth)");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        const std::string sample = "Spinning!";
        const Vector2 measured = font.MeasureString(sample);
        const Vector2 origin = measured / 2.0f;
        const Vector2 position(200.0f, end.Y + 120.0f);

        sb.DrawString(font, sample, position, mul(Color::White, TransitionAlpha()), angle_, origin, 1.5f,
                      SpriteEffects::None, 0.0f);
    }

private:
    float angle_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics2D::DrawStringDemos
