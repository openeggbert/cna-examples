// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Devices/FileDialog.hpp"
#include "CNA/Devices/FileDialogFilter.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::FileDialog;
using CNA::Devices::FileDialogFilter;

// Demonstrates CNA::Devices::FileDialog -- native open/save/open-folder
// dialogs, no XNA/WP7 equivalent. Unlike MessageBox, every call here is
// asynchronous (returns immediately; the result arrives later via a
// callback) -- but the real backend still launches a genuine native dialog
// process (e.g. zenity on Linux) that waits indefinitely for a human, so
// this app's own automated verification does not press the trigger either;
// try it interactively. Space/Enter/A/tap: ShowOpenFile(). Gamepad X:
// ShowOpenFolder().
class FileDialogScreen : public DemoScreen {
public:
    FileDialogScreen() : DemoScreen("Desktop: FileDialog") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            waitingForResult_ = true;
            FileDialog::ShowOpenFile(
                [this](const std::vector<std::string>& files) { OnResult(files); },
                {FileDialogFilter{"Text files", "txt"}, FileDialogFilter{"All files", "*"}});
        } else if (input.IsNewButtonPress(Buttons::X, ControllingPlayer(), playerIndex)) {
            waitingForResult_ = true;
            FileDialog::ShowOpenFolder(
                [this](const std::vector<std::string>& files) { OnResult(files); }, "", false);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: ShowOpenFile(). Gamepad X: ShowOpenFolder().");
        lines.push_back("Both are async -- the result callback fires whenever the dialog closes.");
        lines.emplace_back();
        lines.push_back(std::string("IsSupported: ") + (FileDialog::getIsSupportedProperty() ? "true" : "false"));
        lines.push_back(std::string("Waiting for a result: ") + (waitingForResult_ ? "yes" : "no"));
        lines.push_back("Results received: " + std::to_string(resultCount_));
        for (const auto& f : lastFiles_) lines.push_back("  " + f);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void OnResult(const std::vector<std::string>& files) {
        waitingForResult_ = false;
        resultCount_++;
        lastFiles_ = files;
    }

    bool waitingForResult_ = false;
    int resultCount_ = 0;
    std::vector<std::string> lastFiles_;
};

} // namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos
