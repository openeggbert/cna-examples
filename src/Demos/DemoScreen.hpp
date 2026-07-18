// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Rectangle.hpp"
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

        const Vector2 titleSize = font.MeasureString(title_);
        sb.DrawString(font, title_,
                      Vector2(((float)viewport.getWidthProperty() - titleSize.X) / 2.0f, 20.0f),
                      mul(Color(192, 192, 192), alpha));

        OnDemoDraw(gameTime, sb, font);

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

    // Draws a vertical stack of lines starting at `origin`, one per string.
    static Vector2 DrawLines(SpriteBatch& spriteBatch, SpriteFont& font, Vector2 origin,
                             const std::vector<std::string>& lines, Color color,
                             float lineGap = 6.0f) {
        Vector2 pos = origin;
        for (const auto& line : lines) {
            spriteBatch.DrawString(font, line, pos, color);
            pos.Y += (float)font.getLineSpacingProperty() + lineGap;
        }
        return pos;
    }

private:
    // Rough tappable zone around the drawn "Back" hint, bottom-left corner.
    Rectangle BackHitBox() const {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        return Rectangle(0, viewport.getHeightProperty() - 60, 260, 60);
    }

    static constexpr const char* kBackText = "< Back (Esc / B / tap here)";

    std::string title_;
};

} // namespace CnaExamples::Demos
