// SPDX-License-Identifier: MIT
//
// Adapted from the official XNA "Game State Management" sample (see
// GameScreen.hpp for provenance).
#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"

#include "GameScreen.hpp"

namespace CnaExamples::GameStateManagement {

using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;

class MenuScreen;

// A single selectable entry in a MenuScreen. Draws its text and raises
// Selected when activated (keyboard/gamepad confirm, or a touch tap that
// landed inside its bounds).
class MenuEntry {
public:
    explicit MenuEntry(const std::string& text) : text_(text) {}
    virtual ~MenuEntry() = default;

    const std::string& Text() const { return text_; }
    void setText(const std::string& value) { text_ = value; }

    Vector2 Position() const { return position_; }
    void setPosition(Vector2 value) { position_ = value; }

    // Event raised when the menu entry is selected.
    std::function<void(PlayerIndex)> Selected;

    void OnSelectEntry(PlayerIndex playerIndex) {
        if (Selected)
            Selected(playerIndex);
    }

    // Updates the fading selection-highlight effect.
    virtual void Update(MenuScreen& screen, bool isSelected, GameTime& gameTime) {
        (void)screen;
        float fadeSpeed = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 4;
        if (isSelected)
            selectionFade_ = std::min(selectionFade_ + fadeSpeed, 1.0f);
        else
            selectionFade_ = std::max(selectionFade_ - fadeSpeed, 0.0f);
    }

    // Defined in MenuScreen.hpp (needs ScreenManager internals).
    virtual void Draw(MenuScreen& screen, bool isSelected, const GameTime& gameTime);
    virtual int GetHeight(MenuScreen& screen);
    virtual int GetWidth(MenuScreen& screen);

    // Touch hit-test in the entry's own local layout space (before the
    // caller offsets by Position()); large enough for a comfortable tap
    // target on a phone screen, not just the tight text bounds.
    bool ContainsPoint(MenuScreen& screen, Vector2 point) {
        const float w = (float)GetWidth(screen);
        const float h = (float)GetHeight(screen);
        constexpr float kTouchPaddingY = 12.0f;
        return point.X >= position_.X && point.X <= position_.X + w &&
               point.Y >= position_.Y - kTouchPaddingY &&
               point.Y <= position_.Y + h + kTouchPaddingY;
    }

private:
    std::string text_;
    float selectionFade_ = 0.0f;
    Vector2 position_;
};

} // namespace CnaExamples::GameStateManagement
