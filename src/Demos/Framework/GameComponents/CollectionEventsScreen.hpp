// SPDX-License-Identifier: MIT
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollectionEventArgs.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Framework/GameComponents/DemoComponents.hpp"

namespace CnaExamples::Demos::Framework::GameComponentsDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::GameComponentCollectionEventArgs;

// GameComponentCollection raises ComponentAdded/ComponentRemoved. This is how
// the Game itself learns that a component appeared and needs initialising, and
// it is the hook a game would use to keep its own registry in step.
//
// The log below is written by the event handlers, not by the code that does the
// adding -- so what is on screen is genuinely what the collection reported.
class CollectionEventsScreen : public DemoScreen {
public:
    CollectionEventsScreen() : DemoScreen("Components: Collection Events") {}

    void LoadContent() override {
        auto& game = GameRef();
        component_ = std::make_unique<CountingComponent>(game, "watched");

        auto& components = game.getComponentsProperty();
        addedToken_ = components.ComponentAdded.Add(
            [this](System::Object*, const GameComponentCollectionEventArgs&) {
                Log("ComponentAdded raised");
            });
        removedToken_ = components.ComponentRemoved.Add(
            [this](System::Object*, const GameComponentCollectionEventArgs&) {
                Log("ComponentRemoved raised");
            });
    }

    void UnloadContent() override {
        auto& components = GameRef().getComponentsProperty();
        // Unsubscribe before the component goes away: these handlers capture
        // `this`, and the collection outlives this screen by a long way.
        components.ComponentAdded.Remove(addedToken_);
        components.ComponentRemoved.Remove(removedToken_);
        if (registered_) (void)components.Remove(component_.get());
        component_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (!input.IsMenuSelect(ControllingPlayer(), playerIndex)) return;

        auto& components = GameRef().getComponentsProperty();
        if (registered_) {
            (void)components.Remove(component_.get());
            registered_ = false;
        } else {
            components.Add(component_.get());
            registered_ = true;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        auto& components = GameRef().getComponentsProperty();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: add or remove a component and watch the events fire");
        lines.emplace_back();
        lines.push_back("Component registered: " + std::string(registered_ ? "yes" : "no"));
        lines.push_back("Collection Count:     " + std::to_string((int)components.getCountProperty()));
        lines.emplace_back();
        lines.push_back("Event log (written by the handlers themselves):");
        if (log_.empty()) {
            lines.push_back("  (nothing yet)");
        } else {
            for (const auto& entry : log_) lines.push_back("  " + entry);
        }

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    Game& GameRef() const { return GetScreenManager()->getGameProperty(); }

    void Log(std::string message) {
        log_.push_back("#" + std::to_string(++sequence_) + "  " + std::move(message));
        if (log_.size() > 8) log_.erase(log_.begin());
    }

    std::unique_ptr<CountingComponent> component_;
    std::vector<std::string> log_;
    bool registered_ = false;
    int sequence_ = 0;
    System::EventHandler<GameComponentCollectionEventArgs>::Token addedToken_{};
    System::EventHandler<GameComponentCollectionEventArgs>::Token removedToken_{};
};

} // namespace CnaExamples::Demos::Framework::GameComponentsDemos
