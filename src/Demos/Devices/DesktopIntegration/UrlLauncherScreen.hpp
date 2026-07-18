// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "CNA/Devices/UrlLauncher.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::UrlLauncher;

// Demonstrates CNA::Devices::UrlLauncher::Open() -- hands a URL to the
// system's default external application (a browser, for an http(s) link),
// no XNA/WP7 equivalent. A true result only means something was launched
// to handle it, not that it succeeded; this app's own automated
// verification does not press the trigger, since a real browser/handler
// launching is an external side effect outside this app's control -- try
// it interactively.
class UrlLauncherScreen : public DemoScreen {
public:
    UrlLauncherScreen() : DemoScreen("Desktop: UrlLauncher") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            lastResult_ = UrlLauncher::Open("https://github.com/openeggbert/cna");
            openCount_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap opens https://github.com/openeggbert/cna in the");
        lines.push_back("system's default browser/handler.");
        lines.emplace_back();
        lines.push_back("Opens triggered: " + std::to_string(openCount_));
        if (lastResult_.has_value()) {
            lines.push_back("Last Open() result: " + std::string(*lastResult_ ? "true" : "false"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int openCount_ = 0;
    std::optional<bool> lastResult_;
};

} // namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos
