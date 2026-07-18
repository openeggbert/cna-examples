// SPDX-License-Identifier: MIT
//
// Adapted from the official XNA "Game State Management" sample (see
// GameScreen.hpp for provenance), extended with touch tap-to-select and
// mouse click-to-select so the same menu works on Android and touch
// devices, and with just a mouse on desktop, without requiring a keyboard
// or gamepad.
#pragma once

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "ScreenManager.hpp"
#include "MenuEntry.hpp"

namespace CnaExamples::GameStateManagement {

// Base class for screens that are a vertical list of selectable options.
// The user can move up/down and confirm/cancel via keyboard or gamepad, tap
// an entry directly on a touch screen, or click it with a mouse.
class MenuScreen : public GameScreen {
public:
    explicit MenuScreen(const std::string& menuTitle) : menuTitle_(menuTitle) {
        setTransitionOnTime(TimeSpan::FromSeconds(0.5));
        setTransitionOffTime(TimeSpan::FromSeconds(0.5));
    }

    void HandleInput(InputState& input) override {
        if (!menuEntries_.empty()) {
            if (input.IsMenuUp(ControllingPlayer())) {
                selectedEntry_--;
                if (selectedEntry_ < 0)
                    selectedEntry_ = (int)menuEntries_.size() - 1;
            }
            if (input.IsMenuDown(ControllingPlayer())) {
                selectedEntry_++;
                if (selectedEntry_ >= (int)menuEntries_.size())
                    selectedEntry_ = 0;
            }
        }

        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            if (!menuEntries_.empty())
                OnSelectEntry(selectedEntry_, playerIndex);
        } else if (input.IsMenuCancel(ControllingPlayer(), playerIndex)) {
            OnCancel(playerIndex);
        }

        Vector2 point;
        if (input.IsNewTap(point) || input.IsNewClick(point)) {
            for (size_t i = 0; i < menuEntries_.size(); i++) {
                if (menuEntries_[i]->ContainsPoint(*this, point)) {
                    selectedEntry_ = (int)i;
                    OnSelectEntry((int)i, PlayerIndex::One);
                    break;
                }
            }
        }
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus,
                bool coveredByOtherScreen) override {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
        UpdateMenuEntryLocations();
        for (size_t i = 0; i < menuEntries_.size(); i++) {
            bool isSelected = IsActive() && ((int)i == selectedEntry_);
            menuEntries_[i]->Update(*this, isSelected, gameTime);
        }
    }

    void Draw(const GameTime& gameTime) override {
        auto& graphics = screenManager_->getGraphicsDeviceProperty();
        SpriteBatch& spriteBatch = screenManager_->getSpriteBatch();
        SpriteFont& font = screenManager_->getFont();

        spriteBatch.Begin();

        // Entries below the title/above the screen bottom scroll with
        // scrollOffset_ (see UpdateMenuEntryLocations); skip drawing ones
        // currently scrolled out of view instead of letting them overlap
        // the title or run off the bottom edge.
        const float top = kListTop - 4.0f;
        const float bottom = (float)graphics.getViewportProperty().getHeightProperty();
        for (size_t i = 0; i < menuEntries_.size(); i++) {
            const float y = menuEntries_[i]->Position().Y;
            const float h = (float)menuEntries_[i]->GetHeight(*this);
            if (y + h < top || y > bottom) continue;
            bool isSelected = IsActive() && ((int)i == selectedEntry_);
            menuEntries_[i]->Draw(*this, isSelected, gameTime);
        }

        float transitionOffset = (float)std::pow(TransitionPosition(), 2);

        Vector2 titlePosition((float)(graphics.getViewportProperty().getWidthProperty() / 2), 80.0f);
        Vector2 ms = font.MeasureString(menuTitle_);
        Vector2 titleOrigin(ms.X * 0.5f, ms.Y * 0.5f);
        Color titleColor = mul(Color(192, 192, 192), TransitionAlpha());
        float titleScale = 1.25f;

        titlePosition.Y -= transitionOffset * 100;

        spriteBatch.DrawString(font, menuTitle_, titlePosition, titleColor, 0.0f,
                               titleOrigin, titleScale, SpriteEffects::None, 0.0f);

        spriteBatch.End();
    }

protected:
    std::vector<std::shared_ptr<MenuEntry>>& MenuEntries() { return menuEntries_; }

    virtual void OnSelectEntry(int entryIndex, PlayerIndex playerIndex) {
        menuEntries_[entryIndex]->OnSelectEntry(playerIndex);
    }

    virtual void OnCancel(PlayerIndex playerIndex) {
        (void)playerIndex;
        ExitScreen();
    }

    // Positions entries in a vertical list, centered horizontally. Rows are
    // spaced generously (GetHeight() + kRowPadding) so touch targets stay
    // comfortably tappable on a phone screen, not just readable on desktop.
    // When the list is taller than the viewport (e.g. Keyboard's 10 demos +
    // Back), scrollOffset_ shifts everything up just enough to keep the
    // selected entry on screen -- see AutoScrollToSelection().
    virtual void UpdateMenuEntryLocations() {
        constexpr float kRowPadding = 18.0f;
        float transitionOffset = (float)std::pow(TransitionPosition(), 2);

        Vector2 position(0.0f, kListTop - scrollOffset_);

        for (size_t i = 0; i < menuEntries_.size(); i++) {
            auto& menuEntry = menuEntries_[i];

            position.X = (float)(screenManager_->getGraphicsDeviceProperty()
                                     .getViewportProperty().getWidthProperty() / 2)
                         - menuEntry->GetWidth(*this) / 2.0f;

            if (GetScreenState() == ScreenState::TransitionOn)
                position.X -= transitionOffset * 256;
            else
                position.X += transitionOffset * 512;

            menuEntry->setPosition(position);
            position.Y += menuEntry->GetHeight(*this) + kRowPadding;
        }

        AutoScrollToSelection();
    }

private:
    static constexpr float kListTop = 175.0f;
    static constexpr float kListBottomMargin = 24.0f;

    // Keeps the currently-selected entry within the visible viewport band
    // by adjusting scrollOffset_ -- entries are repositioned on the *next*
    // UpdateMenuEntryLocations() call using the new offset (one frame of
    // lag, imperceptible at 60fps). Only keyboard/gamepad Up/Down and
    // ContainsPoint (touch) drive selectedEntry_, so this alone keeps every
    // reachable entry visible; a long list still needs a swipe/drag gesture
    // to browse entries beyond the selection, which is future work.
    void AutoScrollToSelection() {
        if (menuEntries_.empty() || selectedEntry_ < 0 ||
            selectedEntry_ >= (int)menuEntries_.size()) {
            return;
        }
        auto& viewport = screenManager_->getGraphicsDeviceProperty().getViewportProperty();
        const float bottom = (float)viewport.getHeightProperty() - kListBottomMargin;

        auto& selected = menuEntries_[selectedEntry_];
        const float y = selected->Position().Y;
        const float h = (float)selected->GetHeight(*this);

        if (y < kListTop) {
            scrollOffset_ -= (kListTop - y);
        } else if (y + h > bottom) {
            scrollOffset_ += (y + h - bottom);
        }
        if (scrollOffset_ < 0.0f) scrollOffset_ = 0.0f;
    }

    std::vector<std::shared_ptr<MenuEntry>> menuEntries_;
    int selectedEntry_ = 0;
    float scrollOffset_ = 0.0f;
    std::string menuTitle_;
};

// ---- MenuEntry methods that depend on MenuScreen / ScreenManager ----

inline void MenuEntry::Draw(MenuScreen& screen, bool isSelected, const GameTime& gameTime) {
    Color color = isSelected ? Color::Yellow : Color::White;

    double time = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();
    float pulsate = (float)std::sin(time * 6) + 1;
    float scale = 1 + pulsate * 0.05f * selectionFade_;

    color = mul(color, screen.TransitionAlpha());

    ScreenManager* sm = screen.GetScreenManager();
    SpriteBatch& spriteBatch = sm->getSpriteBatch();
    SpriteFont& font = sm->getFont();

    Vector2 origin(0.0f, font.getLineSpacingProperty() / 2.0f);

    spriteBatch.DrawString(font, text_, position_, color, 0.0f, origin, scale,
                           SpriteEffects::None, 0.0f);
}

inline int MenuEntry::GetHeight(MenuScreen& screen) {
    return screen.GetScreenManager()->getFont().getLineSpacingProperty();
}

inline int MenuEntry::GetWidth(MenuScreen& screen) {
    return (int)screen.GetScreenManager()->getFont().MeasureString(text_).X;
}

} // namespace CnaExamples::GameStateManagement
