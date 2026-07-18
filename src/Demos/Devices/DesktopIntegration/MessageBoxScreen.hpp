// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Devices/MessageBox.hpp"
#include "CNA/Devices/MessageBoxType.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::MessageBox;
using CNA::Devices::MessageBoxType;

// Demonstrates CNA::Devices::MessageBox -- native, modal OS dialogs, no
// XNA/WP7 equivalent. Space/Enter/A/tap shows a simple one-button dialog;
// gamepad X shows a two-button "Yes/No" dialog and records which one was
// clicked. Genuinely desktop-testable -- but note both calls BLOCK this
// screen's whole thread until the dialog is dismissed (SDL3's own documented
// behavior for SDL_ShowMessageBox()), so nothing else in the app updates
// while it is open. Not triggered by this app's own automated verification
// for exactly that reason; try it interactively.
class MessageBoxScreen : public DemoScreen {
public:
    MessageBoxScreen() : DemoScreen("Desktop: MessageBox") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            MessageBox::ShowSimple(MessageBoxType::Information, "cna-examples",
                                   "This is a native, modal message box.");
            shownCount_++;
        } else if (input.IsNewButtonPress(Buttons::X, ControllingPlayer(), playerIndex)) {
            lastChoice_ = MessageBox::Show(MessageBoxType::Warning, "cna-examples",
                                           "Pick a button.", {"Yes", "No"});
            shownCount_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: ShowSimple(). Gamepad X: Show() with Yes/No.");
        lines.push_back("Both BLOCK this screen until dismissed -- that's real SDL3 behavior.");
        lines.emplace_back();
        lines.push_back(std::string("IsSupported: ") + (MessageBox::getIsSupportedProperty() ? "true" : "false"));
        lines.push_back("Dialogs shown: " + std::to_string(shownCount_));
        if (lastChoice_ >= 0) {
            lines.push_back("Last Show() choice index: " + std::to_string(lastChoice_) +
                            (lastChoice_ == 0 ? " (Yes)" : " (No)"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int shownCount_ = 0;
    int lastChoice_ = -1;
};

} // namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos
