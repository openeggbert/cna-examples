// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::SpriteFontDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using SharpRuntime::charcs;

// Demonstrates SpriteFont's public NOXNA constructor -- in real XNA this
// constructor is internal, only ever invoked by the content pipeline's
// SpriteFontReader; CNA has no XNB pipeline, so it's exposed directly for
// applications (or content readers) that build the glyph atlas themselves.
// This screen builds a complete, working SpriteFont entirely from scratch
// -- NOT the shared "menufont" every other screen uses -- covering only
// the 10 digit characters, each glyph a distinct flat color (a stand-in
// for real glyph art, purely to prove the constructor/rendering pipeline
// works end to end).
class HandBuiltFontScreen : public DemoScreen {
public:
    static constexpr int kCellSize = 24;

    HandBuiltFontScreen() : DemoScreen("SpriteFont: Hand-Built From Scratch") {}

    void LoadContent() override {
        std::vector<Color> palette = {
            Color(255, 80, 80, 255), Color(255, 160, 60, 255), Color(255, 220, 60, 255), Color(160, 255, 60, 255),
            Color(60, 255, 120, 255), Color(60, 255, 220, 255), Color(60, 160, 255, 255), Color(120, 60, 255, 255),
            Color(220, 60, 255, 255), Color(255, 60, 160, 255),
        };
        Texture2D atlas = CreateGridTexture(GetScreenManager()->getGraphicsDeviceProperty(), 10, 1, kCellSize, palette);

        std::vector<Rectangle> glyphBounds;
        std::vector<Rectangle> cropping;
        std::vector<charcs> characters;
        std::vector<Vector3> kerning;
        for (int i = 0; i < 10; ++i) {
            const Rectangle cell(i * kCellSize, 0, kCellSize, kCellSize);
            glyphBounds.push_back(cell);
            // Cropping is a per-glyph rendering offset relative to the pen, NOT
            // the glyph's position within the atlas -- reusing `cell` here (i.e.
            // its atlas-space X) would double up with the pen's own advance and
            // corrupt left-to-right glyph order for any non-monotonic string.
            cropping.push_back(Rectangle(0, 0, kCellSize, kCellSize));
            characters.push_back((charcs)('0' + i));
            kerning.push_back(Vector3(0.0f, (float)kCellSize, 0.0f));
        }

        font_.emplace(atlas, glyphBounds, cropping, characters, kCellSize + 4, 2.0f, kerning, charcs('0'));
    }

    void UnloadContent() override {
        font_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A brand-new SpriteFont built from scratch (not the shared menufont) --");
        lines.push_back("10 digit glyphs, each a flat color standing in for real glyph art.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const Color tint = mul(Color::White, TransitionAlpha());

        sb.DrawString(*font_, "0123456789", Vector2(40.0f, end.Y + 20.0f), tint);
        sb.DrawString(*font_, "42", Vector2(40.0f, end.Y + 70.0f), tint);
    }

private:
    std::optional<SpriteFont> font_;
};

} // namespace CnaExamples::Demos::Graphics2D::SpriteFontDemos
