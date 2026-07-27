// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameWindow.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Framework::WindowDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::GameWindow;

// GameWindow::ClientSizeChanged is how a game learns the user resized the
// window, which is the cue to rebuild anything sized against the back buffer --
// render targets, projection matrices, UI layout.
//
// AllowUserResizing is off by default in XNA and in this app, so this screen
// turns it on while it is open (and back off on the way out): a resize event
// that cannot be produced is not much of a demonstration. Grab the window edge
// and the log below fills in.
//
// Under a headless run there is no window manager to drag with, so the log
// stays empty and the screen says so rather than looking broken.
class ClientSizeChangedScreen : public DemoScreen {
public:
    ClientSizeChangedScreen() : DemoScreen("Window: ClientSizeChanged") {}

    void OnDemoLoad() override {
        GameWindow& window = Window();
        originalAllowResizing_ = window.getAllowUserResizingProperty();
        window.setAllowUserResizingProperty(true);

        lastBounds_ = window.getClientBoundsProperty();
        token_ = window.ClientSizeChanged.Add(
            [this](System::Object*, const System::EventArgs&) { OnResized(); });
    }

    void OnDemoUnload() override {
        GameWindow& window = Window();
        window.ClientSizeChanged.Remove(token_);
        window.setAllowUserResizingProperty(originalAllowResizing_);
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        GameWindow& window = Window();
        const auto bounds = window.getClientBoundsProperty();

        std::vector<std::string> lines;
        lines.push_back("Drag the window's edge to resize it. AllowUserResizing is switched on");
        lines.push_back("while this screen is open and restored when you leave.");
        lines.emplace_back();
        lines.push_back("AllowUserResizing now:      " +
                        std::string(window.getAllowUserResizingProperty() ? "true" : "false"));
        lines.push_back("AllowUserResizing on entry: " +
                        std::string(originalAllowResizing_ ? "true" : "false"));
        lines.emplace_back();
        lines.push_back("ClientBounds: " + std::to_string(bounds.Width) + " x " +
                        std::to_string(bounds.Height));
        lines.push_back("ClientSizeChanged raised: " + std::to_string(eventCount_) + " time(s)");
        lines.emplace_back();
        lines.push_back("Event log (written by the handler):");
        if (log_.empty()) {
            lines.push_back("  (no resize yet -- and none is possible in a headless run,");
            lines.push_back("   which has no window manager to drag with)");
        } else {
            for (const auto& entry : log_) lines.push_back("  " + entry);
        }
        lines.emplace_back();
        lines.push_back("This event is the cue to rebuild anything sized against the back");
        lines.push_back("buffer: render targets, projection matrices, UI layout.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    GameWindow& Window() const { return GetScreenManager()->getGameProperty().getWindowProperty(); }

    void OnResized() {
        const auto bounds = Window().getClientBoundsProperty();
        eventCount_++;
        log_.push_back("#" + std::to_string(eventCount_) + "  " +
                       std::to_string(lastBounds_.Width) + "x" + std::to_string(lastBounds_.Height) +
                       "  ->  " +
                       std::to_string(bounds.Width) + "x" + std::to_string(bounds.Height));
        if (log_.size() > 6) log_.erase(log_.begin());
        lastBounds_ = bounds;
    }

    Microsoft::Xna::Framework::Rectangle lastBounds_;
    std::vector<std::string> log_;
    bool originalAllowResizing_ = false;
    int eventCount_ = 0;
    System::EventHandler<System::EventArgs>::Token token_{};
};

} // namespace CnaExamples::Demos::Framework::WindowDemos
