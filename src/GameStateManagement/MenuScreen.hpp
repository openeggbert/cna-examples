// SPDX-License-Identifier: MIT
//
// Adapted from the official XNA "Game State Management" sample (see
// GameScreen.hpp for provenance), extended with touch tap-to-select and
// mouse click-to-select so the same menu works on Android and touch
// devices, and with just a mouse on desktop, without requiring a keyboard
// or gamepad.
#pragma once

#include <algorithm>
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
                // Moving the selection hands scroll control back to auto-scroll.
                userScrolled_ = false;
            }
            if (input.IsMenuDown(ControllingPlayer())) {
                selectedEntry_++;
                if (selectedEntry_ >= (int)menuEntries_.size())
                    selectedEntry_ = 0;
                userScrolled_ = false;
            }
        }

        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            if (!menuEntries_.empty())
                OnSelectEntry(selectedEntry_, playerIndex);
        } else if (input.IsMenuCancel(ControllingPlayer(), playerIndex)) {
            OnCancel(playerIndex);
        }

        HandlePointer(input);
    }

    // Touch/mouse: drag to scroll, release without dragging to select.
    //
    // Selection deliberately happens on RELEASE, not on press. Selecting on
    // press (the previous behaviour) makes drag-to-scroll impossible: the
    // moment a finger lands on an entry to start scrolling, that entry runs.
    void HandlePointer(InputState& input) {
        Vector2 point;

        if (input.IsPointerDown(point)) {
            if (!pointerWasDown_) {
                pointerWasDown_ = true;
                dragging_ = false;
                dragStartY_ = point.Y;
                dragLastY_ = point.Y;
                scrollAtDragStart_ = scrollOffset_;
            } else {
                // A few pixels of slop before it counts as a drag, so a shaky
                // tap still selects rather than nudging the list.
                if (!dragging_ && std::fabs(point.Y - dragStartY_) > kDragThreshold) {
                    dragging_ = true;
                }
                if (dragging_) {
                    // Content follows the finger: dragging up scrolls down.
                    scrollOffset_ = scrollAtDragStart_ - (point.Y - dragStartY_);
                    ClampScroll();
                    // Where the user scrolled to now wins over the selection.
                    // Without this, releasing the finger let AutoScrollToSelection
                    // immediately drag the list back to the selected entry, so a
                    // drag appeared to do nothing at all.
                    userScrolled_ = true;
                }
                dragLastY_ = point.Y;
            }
            return;
        }

        if (input.IsPointerReleased(point)) {
            const bool wasDragging = dragging_;
            pointerWasDown_ = false;
            dragging_ = false;
            if (wasDragging) return;   // a scroll gesture must not also select

            for (size_t i = 0; i < menuEntries_.size(); i++) {
                if (menuEntries_[i]->ContainsPoint(*this, point)) {
                    selectedEntry_ = (int)i;
                    OnSelectEntry((int)i, PlayerIndex::One);
                    break;
                }
            }
            return;
        }

        pointerWasDown_ = false;
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

        DrawScrollbar(spriteBatch);

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

        // Total laid-out height, recomputed here because entry heights come from
        // the font and are not known until a ScreenManager exists.
        contentHeight_ = position.Y - (kListTop - scrollOffset_);

        // A drag owns the scroll position while it is happening, and keeps
        // owning it after release until the selection moves -- see userScrolled_.
        if (!dragging_ && !userScrolled_) AutoScrollToSelection();
        ClampScroll();
    }

    // Drawn by Draw() after the entries. Only appears when the list actually
    // overflows, so short menus stay visually clean.
    void DrawScrollbar(SpriteBatch& spriteBatch) {
        const float maxScroll = MaxScroll();
        if (maxScroll <= 0.0f) return;

        auto& viewport = screenManager_->getGraphicsDeviceProperty().getViewportProperty();
        const float trackTop = kListTop;
        const float trackHeight = (float)viewport.getHeightProperty() - kListBottomMargin - trackTop;
        if (trackHeight <= 0.0f) return;

        const float visibleFraction = trackHeight / contentHeight_;
        const float thumbHeight = std::max(24.0f, trackHeight * visibleFraction);
        const float progress = scrollOffset_ / maxScroll;
        const float thumbY = trackTop + progress * (trackHeight - thumbHeight);

        const int x = viewport.getWidthProperty() - 12;
        const float alpha = TransitionAlpha();
        spriteBatch.Draw(screenManager_->getBlankTexture(),
                         Rectangle(x, (int)trackTop, 4, (int)trackHeight),
                         mul(Color(48, 48, 48), alpha));
        spriteBatch.Draw(screenManager_->getBlankTexture(),
                         Rectangle(x, (int)thumbY, 4, (int)thumbHeight),
                         mul(Color(130, 130, 130), alpha));
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
    }

    // How far the list can scroll before its last entry sits at the bottom.
    float MaxScroll() const {
        auto& viewport = screenManager_->getGraphicsDeviceProperty().getViewportProperty();
        const float visible = (float)viewport.getHeightProperty() - kListBottomMargin - kListTop;
        return std::max(0.0f, contentHeight_ - visible);
    }

    // Applied on every layout pass, not just after a drag: auto-scroll can also
    // push past the end, and an unbounded offset scrolls the list into the void.
    void ClampScroll() {
        scrollOffset_ = std::clamp(scrollOffset_, 0.0f, MaxScroll());
    }

    static constexpr float kDragThreshold = 8.0f;

    std::vector<std::shared_ptr<MenuEntry>> menuEntries_;
    int selectedEntry_ = 0;
    float scrollOffset_ = 0.0f;
    float contentHeight_ = 0.0f;
    bool pointerWasDown_ = false;
    bool dragging_ = false;
    bool userScrolled_ = false;
    float dragStartY_ = 0.0f;
    float dragLastY_ = 0.0f;
    float scrollAtDragStart_ = 0.0f;
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
