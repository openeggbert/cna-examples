// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::DrawStringDemos {

using namespace CnaExamples::GameStateManagement;

// Demonstrates DrawString(font, text, position, color) -- the simplest
// overload -- alongside MeasureString(), drawing a real rectangle sized to
// the measured string so the reported width/height can be checked by eye
// against the text it wraps.
class BasicTextScreen : public DemoScreen {
public:
    BasicTextScreen() : DemoScreen("SpriteBatch: DrawString Basics") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("DrawString(font, text, position, color)");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        const std::string sample = "Hello, CNA!";
        const Vector2 pos(60.0f, end.Y + 30.0f);
        const Vector2 measured = font.MeasureString(sample);

        sb.DrawString(font, sample, pos, mul(Color::White, TransitionAlpha()));
        sb.DrawString(font, "MeasureString(): " + std::to_string((int)measured.X) + "x" + std::to_string((int)measured.Y),
                      Vector2(60.0f, end.Y + 70.0f), mul(Color(150, 220, 255, 255), TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Graphics2D::DrawStringDemos
