// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "SharpRuntime/SharpRuntimeHelper.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::SpriteFontDemos {

using namespace CnaExamples::GameStateManagement;
using SharpRuntime::charcs;

// Demonstrates SpriteFont::DefaultCharacter -- when DrawString()/
// MeasureString() encounter a character the font has no glyph for, they
// fall back to DefaultCharacter if one is set, or throw
// std::invalid_argument("Text contains characters that cannot be resolved
// by this SpriteFont.") if it's unset (confirmed by reading
// SpriteBatch.cpp's internal layout code directly). This screen mutates
// the shared "menufont" instance's DefaultCharacter -- the same
// screen-scoped-global-state discipline as TouchPanel::EnabledGestures in
// the Input area -- so LoadContent()/UnloadContent() capture and restore
// its original value.
class DefaultCharacterFallbackScreen : public DemoScreen {
public:
    DefaultCharacterFallbackScreen() : DemoScreen("SpriteFont: DefaultCharacter Fallback") {}

    void LoadContent() override {
        SpriteFont& font = GetScreenManager()->getFont();
        originalDefault_ = font.getDefaultCharacterProperty();

        // Find a codepoint this font's atlas genuinely does not cover, rather
        // than assuming one -- robust regardless of exactly which characters
        // tools/gen_menu_font.py baked in.
        const auto& covered = font.getCharactersProperty();
        for (charcs candidate = 0x00A1; candidate < 0x00A1 + 200; ++candidate) {
            if (std::find(covered.begin(), covered.end(), candidate) == covered.end()) {
                missingChar_ = candidate;
                break;
            }
        }
    }

    void UnloadContent() override {
        GetScreenManager()->getFont().setDefaultCharacterProperty(originalDefault_);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            withFallbackAttempts_++;
            SpriteFont& font = GetScreenManager()->getFont();
            font.setDefaultCharacterProperty(charcs('?'));
            try {
                (void)font.MeasureString(BuildMissingCharString());
                withFallbackResult_ = "MeasureString() succeeded -- fell back to '?'";
            } catch (const std::invalid_argument& ex) {
                withFallbackResult_ = std::string("threw unexpectedly: ") + ex.what();
            }
        } else if (input.IsNewKeyPress(Keys::X, ControllingPlayer(), playerIndex)) {
            noFallbackAttempts_++;
            SpriteFont& font = GetScreenManager()->getFont();
            font.setDefaultCharacterProperty(std::nullopt);
            try {
                (void)font.MeasureString(BuildMissingCharString());
                noFallbackResult_ = "MeasureString() returned without throwing (unexpected)";
            } catch (const std::invalid_argument& ex) {
                noFallbackResult_ = std::string("threw: ") + ex.what();
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: DefaultCharacter='?' -- measure missing char");
        lines.push_back("X: DefaultCharacter unset -- measure missing char (throws)");
        lines.emplace_back();
        lines.push_back("Missing codepoint used: U+" + std::to_string((int)missingChar_));
        lines.push_back("With fallback attempts: " + std::to_string(withFallbackAttempts_));
        if (!withFallbackResult_.empty()) lines.push_back(withFallbackResult_);
        lines.push_back("No fallback attempts: " + std::to_string(noFallbackAttempts_));
        if (!noFallbackResult_.empty()) lines.push_back(noFallbackResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::string BuildMissingCharString() const {
        // UTF-8 encode the missing BMP codepoint (all candidates are in the
        // 0x00A1-0x0169 range, which UTF-8-encodes as exactly 2 bytes).
        std::string s;
        s.push_back((char)(0xC0 | (missingChar_ >> 6)));
        s.push_back((char)(0x80 | (missingChar_ & 0x3F)));
        return s;
    }

    std::optional<charcs> originalDefault_;
    charcs missingChar_ = 0x00A1;
    int withFallbackAttempts_ = 0;
    std::string withFallbackResult_;
    int noFallbackAttempts_ = 0;
    std::string noFallbackResult_;
};

} // namespace CnaExamples::Demos::Graphics2D::SpriteFontDemos
