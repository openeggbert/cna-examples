// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "System/Text/StringBuilder.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::DrawStringDemos {

using namespace CnaExamples::GameStateManagement;
using System::Text::StringBuilder;

// Demonstrates the StringBuilder overloads of DrawString()/MeasureString()
// -- the same layout surface as the std::string overloads, but fed an
// incrementally-built StringBuilder instead, useful for avoiding per-frame
// std::string allocation in a real game's HUD code.
class StringBuilderScreen : public DemoScreen {
public:
    StringBuilderScreen() : DemoScreen("SpriteBatch: DrawString(StringBuilder)") {}

protected:
    void OnDemoUpdate(GameTime&) override {
        frameCount_++;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("DrawString(font, StringBuilder, position, color) -- rebuilt every frame below.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        StringBuilder builder;
        builder.Append("Frame #").Append(frameCount_).Append(" via StringBuilder");

        sb.DrawString(font, builder, Vector2(60.0f, end.Y + 30.0f), mul(Color::White, TransitionAlpha()));

        const Vector2 measured = font.MeasureString(builder);
        sb.DrawString(font, "MeasureString(StringBuilder): " + std::to_string((int)measured.X) + "x" +
                             std::to_string((int)measured.Y),
                      Vector2(60.0f, end.Y + 60.0f), mul(Color(150, 220, 255, 255), TransitionAlpha()));
    }

private:
    int frameCount_ = 0;
};

} // namespace CnaExamples::Demos::Graphics2D::DrawStringDemos
