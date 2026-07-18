// SPDX-License-Identifier: MIT
//
// Adapted from the official XNA "Game State Management" sample (see
// GameScreen.hpp for provenance).
#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "GameScreen.hpp"

namespace CnaExamples::GameStateManagement {

using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// A component owning a stack of GameScreens: calls their Update/Draw at the
// right time and routes input to the topmost active (non-covered) screen.
// This is the navigation substrate for the whole app -- see plan.md section 4.
class ScreenManager : public DrawableGameComponent {
public:
    explicit ScreenManager(Game& game) : DrawableGameComponent(game) {}

    SpriteBatch& getSpriteBatch() { return *spriteBatch_; }
    SpriteFont&  getFont()        { return *font_; }

    void LoadContent() override {
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        auto& content = getGameProperty().getContentProperty();
        font_.emplace(content.Load<SpriteFont>("menufont"));
        blankTexture_.emplace(content.Load<Texture2D>("blank"));

        for (auto& screen : screens_)
            screen->LoadContent();

        isInitialized_ = true;
    }

    void UnloadContent() override {
        for (auto& screen : screens_)
            screen->UnloadContent();
    }

    void Update(GameTime& gameTime) override {
        input_.Update();

        // Snapshot the screen list: a screen's Update can add/remove others.
        screensToUpdate_.clear();
        for (auto& screen : screens_)
            screensToUpdate_.push_back(screen);

        bool otherScreenHasFocus = !getGameProperty().getIsActiveProperty();
        bool coveredByOtherScreen = false;

        while (!screensToUpdate_.empty()) {
            std::shared_ptr<GameScreen> screen = screensToUpdate_.back();
            screensToUpdate_.pop_back();

            screen->Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

            if (screen->GetScreenState() == ScreenState::TransitionOn ||
                screen->GetScreenState() == ScreenState::Active) {
                if (!otherScreenHasFocus) {
                    screen->HandleInput(input_);
                    otherScreenHasFocus = true;
                }

                if (!screen->IsPopup())
                    coveredByOtherScreen = true;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        for (auto& screen : screens_) {
            if (screen->GetScreenState() == ScreenState::Hidden)
                continue;
            screen->Draw(gameTime);
        }
    }

    // Adds a new screen to the top of the stack.
    void AddScreen(std::shared_ptr<GameScreen> screen,
                   std::optional<PlayerIndex> controllingPlayer) {
        screen->setControllingPlayer(controllingPlayer);
        screen->setScreenManager(this);
        screen->setIsExiting(false);

        if (isInitialized_)
            screen->LoadContent();

        screens_.push_back(std::move(screen));
    }

    // Removes a screen immediately. Prefer GameScreen::ExitScreen() so the
    // screen can transition off first.
    void RemoveScreen(GameScreen* screen) {
        if (isInitialized_)
            screen->UnloadContent();

        eraseByPtr(screens_, screen);
        eraseByPtr(screensToUpdate_, screen);
    }

    std::vector<std::shared_ptr<GameScreen>> GetScreens() const {
        return screens_;
    }

    // Draws a translucent black fullscreen sprite, for fades and popup dimming.
    void FadeBackBufferToBlack(float alpha) {
        auto& viewport = getGraphicsDeviceProperty().getViewportProperty();
        spriteBatch_->Begin();
        spriteBatch_->Draw(*blankTexture_,
                           Rectangle(0, 0, viewport.getWidthProperty(), viewport.getHeightProperty()),
                           mul(Color::Black, alpha));
        spriteBatch_->End();
    }

private:
    static void eraseByPtr(std::vector<std::shared_ptr<GameScreen>>& v, GameScreen* p) {
        v.erase(std::remove_if(v.begin(), v.end(),
                               [p](const std::shared_ptr<GameScreen>& s) { return s.get() == p; }),
                v.end());
    }

    std::vector<std::shared_ptr<GameScreen>> screens_;
    std::vector<std::shared_ptr<GameScreen>> screensToUpdate_;
    InputState input_;
    std::unique_ptr<SpriteBatch> spriteBatch_;
    std::optional<SpriteFont> font_;
    std::optional<Texture2D> blankTexture_;
    bool isInitialized_ = false;
};

// ---- GameScreen methods that depend on ScreenManager (defined here) ----

inline void GameScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                               bool coveredByOtherScreen) {
    otherScreenHasFocus_ = otherScreenHasFocus;

    if (isExiting_) {
        screenState_ = ScreenState::TransitionOff;
        if (!UpdateTransition(gameTime, transitionOffTime_, 1)) {
            screenManager_->RemoveScreen(this);  // last action: 'this' may be freed
        }
    } else if (coveredByOtherScreen) {
        if (UpdateTransition(gameTime, transitionOffTime_, 1))
            screenState_ = ScreenState::TransitionOff;
        else
            screenState_ = ScreenState::Hidden;
    } else {
        if (UpdateTransition(gameTime, transitionOnTime_, -1))
            screenState_ = ScreenState::TransitionOn;
        else
            screenState_ = ScreenState::Active;
    }
}

inline void GameScreen::ExitScreen() {
    if (transitionOffTime_.getTotalMillisecondsProperty() <= 0.0) {
        screenManager_->RemoveScreen(this);
    } else {
        isExiting_ = true;
    }
}

} // namespace CnaExamples::GameStateManagement
