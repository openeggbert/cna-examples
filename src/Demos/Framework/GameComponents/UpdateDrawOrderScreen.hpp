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

// DrawOrder decides the order components are drawn in, independently of the
// order they were added. Lower draws first, so higher values end up on top.
//
// The proof here is a real draw log: each component appends its own label every
// time its Draw runs, and the screen prints the resulting sequence. Swapping the
// DrawOrder values flips that sequence -- the claim is read back out of the
// framework rather than asserted.
//
// UpdateOrder works the same way for Update; it is shown as a value here rather
// than as a second log, because two logs of the same shape teaches nothing extra.
class UpdateDrawOrderScreen : public DemoScreen {
public:
    UpdateDrawOrderScreen() : DemoScreen("Components: Update/Draw Order") {}

    void OnDemoLoad() override {
        auto& game = GameRef();
        first_ = std::make_unique<OrderLoggingComponent>(game, "A", &drawLog_);
        second_ = std::make_unique<OrderLoggingComponent>(game, "B", &drawLog_);
        ApplyOrder();

        game.getComponentsProperty().Add(first_.get());
        game.getComponentsProperty().Add(second_.get());
    }

    void OnDemoUnload() override {
        auto& components = GameRef().getComponentsProperty();
        (void)components.Remove(first_.get());
        (void)components.Remove(second_.get());
        first_.reset();
        second_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            swapped_ = !swapped_;
            ApplyOrder();
        }
    }

    void OnDemoUpdate(GameTime&) override {
        // Keep only the most recent draws: the log is a window onto the current
        // ordering, not a growing history.
        if (drawLog_.size() > 8) {
            drawLog_.erase(drawLog_.begin(), drawLog_.end() - 8);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: swap the two components' DrawOrder/UpdateOrder");
        lines.emplace_back();
        lines.push_back("Component A: DrawOrder " + std::to_string((int)first_->getDrawOrderProperty()) +
                        "   UpdateOrder " + std::to_string((int)first_->getUpdateOrderProperty()) +
                        "   draws " + std::to_string(first_->DrawCalls()));
        lines.push_back("Component B: DrawOrder " + std::to_string((int)second_->getDrawOrderProperty()) +
                        "   UpdateOrder " + std::to_string((int)second_->getUpdateOrderProperty()) +
                        "   draws " + std::to_string(second_->DrawCalls()));
        lines.emplace_back();
        lines.push_back("Both were added in the order A, B. Draw order is decided by DrawOrder,");
        lines.push_back("not by insertion order -- lower draws first, so higher ends up on top.");
        lines.emplace_back();

        std::string sequence;
        for (const auto& label : drawLog_) {
            if (!sequence.empty()) sequence += " -> ";
            sequence += label;
        }
        lines.push_back("Actual recent draw sequence, logged by the components themselves:");
        lines.push_back("  " + (sequence.empty() ? std::string("(none yet)") : sequence));

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    Game& GameRef() const { return GetScreenManager()->getGameProperty(); }

    void ApplyOrder() {
        // Well below the ScreenManager's own draw order so these never paint
        // over the demo's text.
        first_->setDrawOrderProperty(swapped_ ? -20 : -30);
        second_->setDrawOrderProperty(swapped_ ? -30 : -20);
        first_->setUpdateOrderProperty(swapped_ ? -20 : -30);
        second_->setUpdateOrderProperty(swapped_ ? -30 : -20);
        drawLog_.clear();
    }

    std::unique_ptr<OrderLoggingComponent> first_;
    std::unique_ptr<OrderLoggingComponent> second_;
    std::vector<std::string> drawLog_;
    bool swapped_ = false;
};

} // namespace CnaExamples::Demos::Framework::GameComponentsDemos
