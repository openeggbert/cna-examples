// SPDX-License-Identifier: MIT
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GameServiceContainer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IGraphicsDeviceService.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Framework::ServicesDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Graphics::IGraphicsDeviceService;

// A service any part of the app could ask the Game for. Deliberately trivial:
// the point of the demo is the container, not the service.
class ExampleClock {
public:
    void Tick() { ticks_++; }
    [[nodiscard]] int Ticks() const { return ticks_; }
private:
    int ticks_ = 0;
};

// Game::Services is XNA's dependency lookup: a type-keyed bag that components
// use to find things they were not handed directly. The one service the
// framework always puts there itself is IGraphicsDeviceService -- which is how
// a DrawableGameComponent finds the GraphicsDevice without a back-reference to
// the Game.
//
// This screen registers and removes its own service live and queries both, so
// the "registered / not registered" transition is visible rather than described.
class GameServiceContainerScreen : public DemoScreen {
public:
    GameServiceContainerScreen() : DemoScreen("Services: GameServiceContainer") {}

    void OnDemoLoad() override {
        clock_ = std::make_unique<ExampleClock>();
    }

    void OnDemoUnload() override {
        // The container outlives this screen, so a service pointing at a
        // destroyed object would be a dangling entry for the rest of the session.
        if (registered_) Services().RemoveService<ExampleClock>();
        clock_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (!input.IsMenuSelect(ControllingPlayer(), playerIndex)) return;

        if (registered_) {
            Services().RemoveService<ExampleClock>();
            registered_ = false;
        } else {
            Services().AddService<ExampleClock>(clock_.get());
            registered_ = true;
        }
    }

    void OnDemoUpdate(GameTime&) override {
        // Reached through the container, exactly as a decoupled component would.
        if (auto* found = Services().GetService<ExampleClock>()) found->Tick();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        auto* clock = Services().GetService<ExampleClock>();
        auto* graphicsService = Services().GetService<IGraphicsDeviceService>();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: add or remove ExampleClock from Game::Services");
        lines.emplace_back();
        lines.push_back("AddService<ExampleClock>() called: " + std::string(registered_ ? "yes" : "no"));
        lines.push_back("GetService<ExampleClock>():        " +
                        std::string(clock != nullptr ? "found" : "nullptr"));
        lines.push_back("Ticks (only advances while registered): " +
                        std::to_string(clock_ ? clock_->Ticks() : 0));
        lines.emplace_back();
        lines.push_back("The service the framework registers for you:");
        lines.push_back("  GetService<IGraphicsDeviceService>(): " +
                        std::string(graphicsService != nullptr ? "found" : "nullptr"));
        lines.push_back("  Its GraphicsDevice is the same object as this screen's: " +
                        std::string(SameDevice(graphicsService) ? "yes" : "no"));
        lines.emplace_back();
        lines.push_back("That service is how a DrawableGameComponent reaches the GraphicsDevice");
        lines.push_back("without holding a reference to the Game -- the container is the seam.");
        lines.emplace_back();
        lines.push_back("Lookup is by type, so there is exactly one ExampleClock slot; a second");
        lines.push_back("AddService for the same type would replace it, not queue behind it.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    Microsoft::Xna::Framework::GameServiceContainer& Services() const {
        return GetScreenManager()->getGameProperty().getServicesProperty();
    }

    bool SameDevice(IGraphicsDeviceService* service) const {
        if (service == nullptr) return false;
        return service->getGraphicsDeviceProperty() ==
               &GetScreenManager()->getGraphicsDeviceProperty();
    }

    std::unique_ptr<ExampleClock> clock_;
    bool registered_ = false;
};

} // namespace CnaExamples::Demos::Framework::ServicesDemos
