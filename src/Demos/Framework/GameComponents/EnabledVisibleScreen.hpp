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

// Enabled and Visible are separate switches for separate halves of a component:
// Enabled gates Update, Visible gates Draw. A component can update without
// drawing (physics that is off-screen) or draw without updating (a frozen
// display), and this screen shows all four combinations by watching the two
// call counters diverge.
//
// Both properties raise a change event, and this screen subscribes to them so
// the log is written by the framework's notification rather than by the code
// that flipped the flag.
class EnabledVisibleScreen : public DemoScreen {
public:
    EnabledVisibleScreen() : DemoScreen("Components: Enabled & Visible") {}

    void LoadContent() override {
        auto& game = GameRef();
        component_ = std::make_unique<OrderLoggingComponent>(game, "toggled", nullptr);
        component_->setDrawOrderProperty(-30);

        enabledToken_ = component_->EnabledChanged.Add(
            [this](System::Object*, const System::EventArgs&) { Log("EnabledChanged"); });
        visibleToken_ = component_->VisibleChanged.Add(
            [this](System::Object*, const System::EventArgs&) { Log("VisibleChanged"); });

        game.getComponentsProperty().Add(component_.get());
    }

    void UnloadContent() override {
        component_->EnabledChanged.Remove(enabledToken_);
        component_->VisibleChanged.Remove(visibleToken_);
        (void)GameRef().getComponentsProperty().Remove(component_.get());
        component_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            component_->setEnabledProperty(!component_->getEnabledProperty());
        }
        if (input.IsMenuUp(ControllingPlayer())) {
            component_->setVisibleProperty(!component_->getVisibleProperty());
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: toggle Enabled     Up: toggle Visible");
        lines.emplace_back();
        lines.push_back("Enabled: " + std::string(component_->getEnabledProperty() ? "true " : "false") +
                        "   -> Update calls: " + std::to_string(component_->UpdateCalls()));
        lines.push_back("Visible: " + std::string(component_->getVisibleProperty() ? "true " : "false") +
                        "   -> Draw calls:   " + std::to_string(component_->DrawCalls()));
        lines.emplace_back();
        lines.push_back("The two counters are independent: whichever flag is off, that counter");
        lines.push_back("stops while the other keeps rising. Enabled gates Update, Visible gates Draw.");
        lines.emplace_back();
        lines.push_back("Change events, logged by the handlers:");
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
        if (log_.size() > 6) log_.erase(log_.begin());
    }

    std::unique_ptr<OrderLoggingComponent> component_;
    std::vector<std::string> log_;
    int sequence_ = 0;
    System::EventHandler<System::EventArgs>::Token enabledToken_{};
    System::EventHandler<System::EventArgs>::Token visibleToken_{};
};

} // namespace CnaExamples::Demos::Framework::GameComponentsDemos
