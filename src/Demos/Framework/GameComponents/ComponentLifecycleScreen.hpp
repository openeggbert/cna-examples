// SPDX-License-Identifier: MIT
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Framework/GameComponents/DemoComponents.hpp"

namespace CnaExamples::Demos::Framework::GameComponentsDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// Adding a component to a running Game is not just a list insertion: the Game
// calls Initialize() on it immediately, because the Game itself has already
// been initialised. A component added before Game.Run() would instead be
// initialised as part of the Game's own startup.
//
// This screen adds and removes a real GameComponent live and shows the
// Initialize/Update call counts, so "when does Initialize actually run" is
// answered by the counter rather than by documentation.
//
// The component is owned by this screen and removed in UnloadContent(). Leaving
// it registered would keep it updating for the rest of the session, and would
// leave the Game holding a pointer to a destroyed object.
class ComponentLifecycleScreen : public DemoScreen {
public:
    ComponentLifecycleScreen() : DemoScreen("Components: Lifecycle") {}

    void OnDemoLoad() override {
        component_ = std::make_unique<CountingComponent>(GameRef(), "counter");
    }

    void OnDemoUnload() override {
        RemoveIfPresent();
        component_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (!input.IsMenuSelect(ControllingPlayer(), playerIndex)) return;

        if (registered_) {
            RemoveIfPresent();
        } else {
            GameRef().getComponentsProperty().Add(component_.get());
            registered_ = true;
            addCount_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        auto& components = GameRef().getComponentsProperty();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: add or remove the component from Game::Components");
        lines.emplace_back();
        lines.push_back(std::string("Registered: ") + (registered_ ? "yes" : "no"));
        lines.push_back("Times added:   " + std::to_string(addCount_));
        lines.push_back("Times removed: " + std::to_string(removeCount_) +
                        (removeCount_ > 0
                             ? "   (last Remove() reported " +
                                   std::string(lastRemoveReported_ ? "true" : "false") + ")"
                             : ""));
        lines.emplace_back();
        lines.push_back("Initialize() calls: " + std::to_string(component_->InitializeCalls()) +
                        "   (once per add -- the Game is already initialised)");
        lines.push_back("Update() calls:     " + std::to_string(component_->UpdateCalls()) +
                        "   (frozen while unregistered)");
        lines.emplace_back();
        lines.push_back("Game::Components::Count: " + std::to_string((int)components.getCountProperty()));
        lines.push_back("Contains(this):          " +
                        std::string(components.Contains(component_.get()) ? "true" : "false"));
        lines.push_back("IndexOf(this):           " +
                        std::to_string((int)components.IndexOf(component_.get())) +
                        "   (-1 when not registered)");
        lines.emplace_back();
        lines.push_back("The app's own GamerServicesComponent and ScreenManager are in this");
        lines.push_back("collection too -- that is why Count never drops to zero.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    Game& GameRef() const { return GetScreenManager()->getGameProperty(); }

    void RemoveIfPresent() {
        if (!registered_) return;
        // Remove() reports whether the component was actually present; a
        // false here would mean this screen's bookkeeping had drifted.
        const bool wasPresent = GameRef().getComponentsProperty().Remove(component_.get());
        lastRemoveReported_ = wasPresent;
        registered_ = false;
        removeCount_++;
    }

    std::unique_ptr<CountingComponent> component_;
    bool registered_ = false;
    bool lastRemoveReported_ = false;
    int addCount_ = 0;
    int removeCount_ = 0;
};

} // namespace CnaExamples::Demos::Framework::GameComponentsDemos
