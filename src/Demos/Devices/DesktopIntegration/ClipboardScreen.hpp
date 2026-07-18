// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "CNA/Devices/Clipboard.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::Clipboard;

// Demonstrates CNA::Devices::Clipboard -- reads/writes the system
// clipboard's UTF-8 text, no XNA/WP7 equivalent. Space/Enter/A/tap writes a
// fixed marker string; gamepad X reads back whatever the clipboard
// currently holds (so copying something from another app, then pressing X,
// shows it here). Genuinely desktop-testable.
class ClipboardScreen : public DemoScreen {
public:
    ClipboardScreen() : DemoScreen("Desktop: Clipboard") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            writeCount_++;
            lastSetResult_ = Clipboard::setTextProperty(
                "cna-examples clipboard test #" + std::to_string(writeCount_));
        } else if (input.IsNewButtonPress(Buttons::X, ControllingPlayer(), playerIndex)) {
            lastRead_ = Clipboard::getTextProperty();
            hasReadOnce_ = true;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: write a marker string to the clipboard.");
        lines.push_back("Gamepad X: read the clipboard's current text back.");
        lines.emplace_back();
        lines.push_back(std::string("HasText: ") + (Clipboard::getHasTextProperty() ? "true" : "false"));
        if (lastSetResult_.has_value()) {
            lines.push_back("Last setTextProperty() result: " + std::string(*lastSetResult_ ? "true" : "false"));
        }
        if (hasReadOnce_) {
            lines.push_back("Last read: \"" + lastRead_ + "\"");
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int writeCount_ = 0;
    std::optional<bool> lastSetResult_;
    std::string lastRead_;
    bool hasReadOnce_ = false;
};

} // namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos
