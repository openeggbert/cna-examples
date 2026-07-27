// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"

namespace CnaExamples::Demos::Framework::GameComponentsDemos {

using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameComponent;
using Microsoft::Xna::Framework::GameTime;

// Real components for the Game Components category to add, remove, reorder,
// enable and hide. They are deliberately minimal: all they do is record that
// their virtuals were called, because "was Update actually called, and in what
// order" is exactly the question these demos exist to answer.
//
// They are NOT registered with the Game at construction. Each demo owns its
// components and adds/removes them from Game::Components itself -- that is the
// behaviour under test.

// A counting GameComponent. Update runs only while Enabled; the component
// itself does not check the flag, the Game does, which is the point.
class CountingComponent : public GameComponent {
public:
    CountingComponent(Game& game, std::string label)
        : GameComponent(game), label_(std::move(label)) {}

    void Initialize() override {
        GameComponent::Initialize();
        initializeCalls_++;
    }

    void Update(GameTime& gameTime) override {
        GameComponent::Update(gameTime);
        updateCalls_++;
    }

    [[nodiscard]] const std::string& Label() const { return label_; }
    [[nodiscard]] int InitializeCalls() const { return initializeCalls_; }
    [[nodiscard]] int UpdateCalls() const { return updateCalls_; }
    void ResetCounts() { updateCalls_ = 0; }

    [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CnaExamples.CountingComponent";
        return name;
    }

private:
    std::string label_;
    int initializeCalls_ = 0;
    int updateCalls_ = 0;
};

// A DrawableGameComponent that appends its label to a shared log every time it
// draws. Reading that log back is how the DrawOrder demo shows the actual
// ordering rather than the requested one.
class OrderLoggingComponent : public DrawableGameComponent {
public:
    OrderLoggingComponent(Game& game, std::string label, std::vector<std::string>* drawLog)
        : DrawableGameComponent(game), label_(std::move(label)), drawLog_(drawLog) {}

    void Update(GameTime& gameTime) override {
        DrawableGameComponent::Update(gameTime);
        updateCalls_++;
    }

    void Draw(const GameTime& gameTime) override {
        DrawableGameComponent::Draw(gameTime);
        drawCalls_++;
        if (drawLog_ != nullptr) drawLog_->push_back(label_);
    }

    [[nodiscard]] const std::string& Label() const { return label_; }
    [[nodiscard]] int UpdateCalls() const { return updateCalls_; }
    [[nodiscard]] int DrawCalls() const { return drawCalls_; }

    [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CnaExamples.OrderLoggingComponent";
        return name;
    }

private:
    std::string label_;
    std::vector<std::string>* drawLog_ = nullptr;
    int updateCalls_ = 0;
    int drawCalls_ = 0;
};

} // namespace CnaExamples::Demos::Framework::GameComponentsDemos
