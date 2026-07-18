// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::DrawStringDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;

// Demonstrates SpriteEffects on DrawString -- flipping mirrors the WHOLE
// rendered glyph sequence as a block (the entire string reads backwards,
// origin-shifted by the measured width), not each glyph individually in
// place. Also demonstrates a real, non-obvious MeasureString() quirk: a
// trailing newline adds a FULL second empty line's height (Y = 2x
// LineSpacing), not a fraction of one -- shown here by comparing measured
// height with vs. without a trailing "\n".
class FlippedTextScreen : public DemoScreen {
public:
    FlippedTextScreen() : DemoScreen("SpriteBatch: DrawString SpriteEffects") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("FlipHorizontally mirrors the WHOLE string as a block, not per-glyph.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const Color tint = mul(Color::White, TransitionAlpha());

        const std::string sample = "ABC 123";
        sb.DrawString(font, sample, Vector2(60.0f, end.Y + 20.0f), tint, 0.0f, Vector2::Zero, 1.0f,
                      SpriteEffects::None, 0.0f);
        sb.DrawString(font, "normal", Vector2(60.0f, end.Y + 50.0f), tint);

        sb.DrawString(font, sample, Vector2(280.0f, end.Y + 20.0f), tint, 0.0f, Vector2::Zero, 1.0f,
                      SpriteEffects::FlipHorizontally, 0.0f);
        sb.DrawString(font, "FlipHorizontally", Vector2(280.0f, end.Y + 50.0f), tint);

        const Vector2 withoutNewline = font.MeasureString("one line");
        const Vector2 withNewline = font.MeasureString("one line\n");
        sb.DrawString(font, "MeasureString(\"one line\").Y = " + std::to_string((int)withoutNewline.Y),
                      Vector2(60.0f, end.Y + 100.0f), tint);
        sb.DrawString(font, "MeasureString(\"one line\\n\").Y = " + std::to_string((int)withNewline.Y) +
                             " (a full extra empty line, not a partial one)",
                      Vector2(60.0f, end.Y + 125.0f), tint);
    }
};

} // namespace CnaExamples::Demos::Graphics2D::DrawStringDemos
