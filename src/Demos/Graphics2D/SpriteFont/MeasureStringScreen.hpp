// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::SpriteFontDemos {

using namespace CnaExamples::GameStateManagement;

// Demonstrates SpriteFont::MeasureString() on a few different strings --
// short, long, and multi-line -- drawing each sample alongside its real
// measured Vector2 width/height so the numbers can be checked by eye
// against the rendered text.
class MeasureStringScreen : public DemoScreen {
public:
    MeasureStringScreen() : DemoScreen("SpriteFont: MeasureString") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("MeasureString(text) -- the same font used for this app's own menus.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const Color tint = mul(Color::White, TransitionAlpha());

        float y = end.Y + 20.0f;
        const std::vector<std::string> samples = {"Hi", "A longer sample string", "Two\nlines"};
        for (const auto& sample : samples) {
            sb.DrawString(font, sample, Vector2(40.0f, y), tint);
            const Vector2 measured = font.MeasureString(sample);
            sb.DrawString(font, "-> " + std::to_string((int)measured.X) + "x" + std::to_string((int)measured.Y),
                          Vector2(320.0f, y), mul(Color(150, 220, 255, 255), TransitionAlpha()));
            y += measured.Y + 16.0f;
        }
    }
};

} // namespace CnaExamples::Demos::Graphics2D::SpriteFontDemos
