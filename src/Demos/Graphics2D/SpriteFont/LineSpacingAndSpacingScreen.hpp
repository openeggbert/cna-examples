// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::SpriteFontDemos {

using namespace CnaExamples::GameStateManagement;

// Demonstrates SpriteFont::LineSpacing (vertical distance between lines)
// and Spacing (extra horizontal gap between characters), both
// live-adjustable. Mutates the shared "menufont" instance -- captured and
// restored in LoadContent()/UnloadContent(), same discipline as
// DefaultCharacterFallbackScreen.
class LineSpacingAndSpacingScreen : public DemoScreen {
public:
    LineSpacingAndSpacingScreen() : DemoScreen("SpriteFont: LineSpacing & Spacing") {}

    void LoadContent() override {
        SpriteFont& font = GetScreenManager()->getFont();
        originalLineSpacing_ = font.getLineSpacingProperty();
        originalSpacing_ = font.getSpacingProperty();
    }

    void UnloadContent() override {
        SpriteFont& font = GetScreenManager()->getFont();
        font.setLineSpacingProperty(originalLineSpacing_);
        font.setSpacingProperty(originalSpacing_);
    }

protected:
    void OnDemoInput(InputState& input) override {
        SpriteFont& font = GetScreenManager()->getFont();
        if (input.IsMenuUp(ControllingPlayer())) {
            font.setLineSpacingProperty(font.getLineSpacingProperty() + 2);
        } else if (input.IsMenuDown(ControllingPlayer())) {
            font.setLineSpacingProperty(std::max(0, font.getLineSpacingProperty() - 2));
        }
        PlayerIndex playerIndex;
        if (input.IsNewKeyPress(Keys::Right, ControllingPlayer(), playerIndex)) {
            font.setSpacingProperty(font.getSpacingProperty() + 1.0f);
        } else if (input.IsNewKeyPress(Keys::Left, ControllingPlayer(), playerIndex)) {
            font.setSpacingProperty(font.getSpacingProperty() - 1.0f);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Up/Down: LineSpacing +-2   Left/Right: Spacing +-1");
        lines.push_back("LineSpacing: " + std::to_string(font.getLineSpacingProperty()) +
                         "   Spacing: " + std::to_string(font.getSpacingProperty()));
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const Color tint = mul(Color::White, TransitionAlpha());

        sb.DrawString(font, "Two\nlines", Vector2(40.0f, end.Y + 20.0f), tint);
        sb.DrawString(font, "S p a c e d   o u t", Vector2(40.0f, end.Y + 100.0f), tint);
    }

private:
    int originalLineSpacing_ = 0;
    float originalSpacing_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics2D::SpriteFontDemos
