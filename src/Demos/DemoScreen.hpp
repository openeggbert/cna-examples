// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <cmath>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "GameStateManagement/ScreenManager.hpp"

namespace CnaExamples::Demos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Rectangle;

// Shared base for every leaf demo screen (the 4th navigation level pushed by
// a CategoryScreen entry). Provides consistent chrome -- a centered title,
// a clickable/tappable/Esc/gamepad-B "Back" hint -- and a DrawLines() helper
// for the common "stack of label: value readout lines" layout most Input
// demos use.
// Concrete demos override OnDemoInput()/OnDemoDraw() instead of HandleInput/
// Draw so Back navigation stays uniform across every demo.
class DemoScreen : public GameScreen {
public:
    explicit DemoScreen(std::string title) : title_(std::move(title)) {
        setTransitionOnTime(TimeSpan::FromSeconds(0.3));
        setTransitionOffTime(TimeSpan::FromSeconds(0.3));
    }

    // Both are supplied from outside rather than by the screen itself: a demo
    // knows what it demonstrates, but not the catalog path it was reached
    // through, and duplicating the path in each screen would let the two drift.
    // MakeDemo<>() sets the APIs; the CategoryScreen that pushes the screen (or
    // the headless driver) sets the breadcrumb.
    void SetApis(std::vector<std::string> apis) { apis_ = std::move(apis); }
    void SetBreadcrumb(std::string breadcrumb) { breadcrumb_ = std::move(breadcrumb); }

    // Shortens `text` with a trailing ellipsis until it measures no wider than
    // `maxWidth`. Measured with the real font rather than assuming a character
    // width -- the menu font is proportional, so a character count would cut
    // "IIII" and "MMMM" at the same place.
    //
    // Public because the navigation screens (SearchScreen) need the same
    // clipping rule as the demo screens; a second copy would drift.
    static std::string Ellipsize(SpriteFont& font, const std::string& text, float maxWidth) {
        if (maxWidth <= 0.0f || font.MeasureString(text).X <= maxWidth) return text;

        std::string result = text;
        while (!result.empty() && font.MeasureString(result + "...").X > maxWidth) {
            result.pop_back();
        }
        return result + "...";
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
        if (IsActive()) {
            OnDemoUpdate(gameTime);
        }
    }

    void HandleInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuCancel(ControllingPlayer(), playerIndex)) {
            ExitScreen();
            return;
        }

        Vector2 point;
        if ((input.IsNewTap(point) || input.IsNewClick(point)) &&
            BackHitBox().Contains((int)point.X, (int)point.Y)) {
            ExitScreen();
            return;
        }

        OnDemoInput(input);
    }

    void Draw(const GameTime& gameTime) override {
        ScreenManager& sm = *GetScreenManager();
        SpriteBatch& sb = sm.getSpriteBatch();
        SpriteFont& font = sm.getFont();
        auto& viewport = sm.getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();

        sb.Begin();

        // The breadcrumb replaces the bare title rather than adding a row above
        // it: its last element already is this demo's catalog name, and the band
        // between the title and the first content line is fully spoken for --
        // every existing demo starts drawing at y=82..90.
        const std::string heading =
            breadcrumb_.empty()
                ? title_
                : Ellipsize(font, breadcrumb_, (float)viewport.getWidthProperty() - 32.0f);
        const Vector2 headingSize = font.MeasureString(heading);
        sb.DrawString(font, heading,
                      Vector2(((float)viewport.getWidthProperty() - headingSize.X) / 2.0f, 20.0f),
                      mul(Color(192, 192, 192), alpha));

        OnDemoDraw(gameTime, sb, font);

        DrawApiFooter(sb, font, alpha, viewport.getWidthProperty(), viewport.getHeightProperty());

        sb.DrawString(font, kBackText, Vector2(16.0f, (float)viewport.getHeightProperty() - 40.0f),
                      mul(Color(150, 150, 150), alpha));

        sb.End();
    }

protected:
    // Override to poll live device state once per active frame (most Input
    // demos call the real Microsoft::Xna::Framework::Input static APIs
    // directly here, not through the app's own menu-navigation InputState).
    virtual void OnDemoUpdate(GameTime& gameTime) { (void)gameTime; }

    // Override to react to input beyond the built-in Back handling.
    virtual void OnDemoInput(InputState& input) { (void)input; }

    // Override to draw the demo's own content. Called between the title and
    // the Back hint, inside the same SpriteBatch Begin/End pair as Draw().
    virtual void OnDemoDraw(const GameTime& gameTime, SpriteBatch& spriteBatch, SpriteFont& font) {
        (void)gameTime; (void)spriteBatch; (void)font;
    }

    // How far a drawn line's ink can actually extend below its draw origin.
    //
    // This is NOT font.LineSpacing, and NOT MeasureString(...).Y either --
    // MeasureString computes its height from LineSpacing, so both report the
    // same number. The menu font declares a line spacing of 29, but its glyph
    // descriptors place ink as far as 52px below the origin (a crop offset of
    // up to 46 plus a source height of up to 24). Laying out against
    // LineSpacing therefore under-reserves vertical space by roughly 23px, and
    // three 3D demos drew their scene labels straight through the Back hint
    // because of it.
    //
    // Doubling LineSpacing is a deliberately conservative bound (58 >= 52) that
    // stays correct if the font is regenerated at a different size, which a
    // hard-coded 52 would not.
    static float GlyphExtent(SpriteFont& font) {
        return (float)font.getLineSpacingProperty() * 2.0f;
    }

    // The lowest Y at which a demo may start drawing a line of text without it
    // colliding with the "< Back" hint.
    float LabelBaselineLimit(SpriteFont& font) const {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        return (float)viewport.getHeightProperty() - 44.0f - GlyphExtent(font);
    }

    // Fills a solid rectangle using the ScreenManager's 1x1 blank texture.
    // Demos that need a bar, meter or panel use this instead of each building
    // its own single-pixel Texture2D.
    void FillRect(SpriteBatch& spriteBatch, const Rectangle& rect, Color color) const {
        spriteBatch.Draw(GetScreenManager()->getBlankTexture(), rect, color);
    }

    // Draws a straight line between two points by stretching and rotating the
    // 1x1 blank texture. There is no line primitive in SpriteBatch, and the
    // alternative -- a chain of small FillRect calls -- aliases badly on
    // shallow diagonals, which is exactly what a vector diagram is made of.
    void DrawLine(SpriteBatch& spriteBatch, Vector2 from, Vector2 to, Color color,
                  float thickness = 2.0f) const {
        const Vector2 delta(to.X - from.X, to.Y - from.Y);
        const float length = std::sqrt(delta.X * delta.X + delta.Y * delta.Y);
        if (length < 0.01f) return;

        spriteBatch.Draw(GetScreenManager()->getBlankTexture(),
                         Rectangle((int)from.X, (int)from.Y, (int)length, (int)thickness),
                         std::nullopt, color,
                         std::atan2(delta.Y, delta.X),
                         Vector2(0.0f, thickness * 0.5f),
                         SpriteEffects::None, 0.0f);
    }

    // A line with a solid head at `to`, for showing direction as well as extent.
    void DrawArrow(SpriteBatch& spriteBatch, Vector2 from, Vector2 to, Color color,
                   float thickness = 2.0f, float headLength = 12.0f) const {
        DrawLine(spriteBatch, from, to, color, thickness);

        const Vector2 delta(to.X - from.X, to.Y - from.Y);
        const float length = std::sqrt(delta.X * delta.X + delta.Y * delta.Y);
        if (length < headLength) return;

        const Vector2 unit(delta.X / length, delta.Y / length);
        const Vector2 back(to.X - unit.X * headLength, to.Y - unit.Y * headLength);
        const Vector2 side(-unit.Y * headLength * 0.4f, unit.X * headLength * 0.4f);
        DrawLine(spriteBatch, to, Vector2(back.X + side.X, back.Y + side.Y), color, thickness);
        DrawLine(spriteBatch, to, Vector2(back.X - side.X, back.Y - side.Y), color, thickness);
    }

    // Draws a vertical stack of lines starting at `origin`, one per string.
    // Lines are clipped to the viewport width: demos print real filesystem
    // paths, exception messages and API names, none of which have a bounded
    // length, and a line running off the right edge silently loses the part
    // that usually matters most.
    Vector2 DrawLines(SpriteBatch& spriteBatch, SpriteFont& font, Vector2 origin,
                      const std::vector<std::string>& lines, Color color,
                      float lineGap = 6.0f) const {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float maxWidth = (float)viewport.getWidthProperty() - origin.X - 16.0f;
        const float step = (float)font.getLineSpacingProperty() + lineGap;
        // Stop above the Back hint rather than drawing underneath it. Bounded by
        // the real glyph extent, not by `step` -- see GlyphExtent().
        const float maxY = LabelBaselineLimit(font);

        Vector2 pos = origin;
        for (std::size_t i = 0; i < lines.size(); ++i) {
            const std::size_t remaining = lines.size() - i;
            if (pos.Y > maxY) {
                // Say what was dropped. A demo whose readout silently runs off
                // the bottom looks complete while hiding the part the user came
                // for; an explicit marker turns that into a visible layout bug.
                spriteBatch.DrawString(font,
                                       "... (" + std::to_string(remaining) +
                                           (remaining == 1 ? " more line)" : " more lines)"),
                                       pos, color);
                pos.Y += step;
                break;
            }
            spriteBatch.DrawString(font, Ellipsize(font, lines[i], maxWidth), pos, color);
            pos.Y += step;
        }
        return pos;
    }

private:
    // Right-aligned on the Back hint's row, so it costs no vertical space at
    // all. Demos already use every pixel between the title and the hint; a
    // footer that pushed content up would have broken the layout of all ~174
    // existing screens at once.
    void DrawApiFooter(SpriteBatch& spriteBatch, SpriteFont& font, float alpha,
                       int viewportWidth, int viewportHeight) const {
        if (apis_.empty()) return;

        std::string text;
        for (const auto& api : apis_) {
            if (!text.empty()) text += "  ";
            text += api;
        }

        // The Back hint owns the left ~340px of this row; never overlap it.
        constexpr float kBackHintWidth = 350.0f;
        const float available = (float)viewportWidth - kBackHintWidth - 16.0f;
        text = Ellipsize(font, text, available);

        const float x = (float)viewportWidth - font.MeasureString(text).X - 16.0f;
        spriteBatch.DrawString(font, text, Vector2(std::max(kBackHintWidth, x),
                                                   (float)viewportHeight - 40.0f),
                               mul(Color(95, 110, 95), alpha));
    }

    // Rough tappable zone around the drawn "Back" hint, bottom-left corner.
    Rectangle BackHitBox() const {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        return Rectangle(0, viewport.getHeightProperty() - 60, 260, 60);
    }

    static constexpr const char* kBackText = "< Back (Esc / B / tap here)";

    std::string title_;
    std::string breadcrumb_;
    std::vector<std::string> apis_;
};

} // namespace CnaExamples::Demos
