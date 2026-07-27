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

// GameWindow is the application's view of the real OS window: its title, its
// client area, which display it is on.
//
// ClientBounds is worth showing next to the GraphicsDevice viewport, because
// the two answer different questions and are routinely confused. ClientBounds is
// the window's drawable area in desktop terms; the viewport is the region the
// device currently renders into. They usually agree, and a render target or a
// custom viewport makes them disagree.
class TitleAndClientBoundsScreen : public DemoScreen {
public:
    TitleAndClientBoundsScreen() : DemoScreen("Window: Title & ClientBounds") {}

    void OnDemoLoad() override {
        originalTitle_ = Window().getTitleProperty();
    }

    void OnDemoUnload() override {
        // The title belongs to the whole application; leaving this demo's text
        // in the task bar would outlive the demo by the rest of the session.
        Window().setTitleProperty(originalTitle_);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            titleIndex_ = (titleIndex_ + 1) % kTitleCount;
            Window().setTitleProperty(kTitles[titleIndex_]);
            titleChanges_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        GameWindow& window = Window();
        const auto bounds = window.getClientBoundsProperty();
        const auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: change the real OS window title");
        lines.emplace_back();
        lines.push_back("Title now:        \"" + window.getTitleProperty() + "\"");
        lines.push_back("Title on entry:   \"" + originalTitle_ + "\"   (restored on Back)");
        lines.push_back("Changes made:     " + std::to_string(titleChanges_));
        lines.emplace_back();
        lines.push_back("ClientBounds:     x " + std::to_string(bounds.X) +
                        "  y " + std::to_string(bounds.Y) +
                        "  w " + std::to_string(bounds.Width) +
                        "  h " + std::to_string(bounds.Height));
        lines.push_back("Viewport:         x " + std::to_string(viewport.getXProperty()) +
                        "  y " + std::to_string(viewport.getYProperty()) +
                        "  w " + std::to_string(viewport.getWidthProperty()) +
                        "  h " + std::to_string(viewport.getHeightProperty()));
        lines.push_back("Same size:        " +
                        std::string(bounds.Width == viewport.getWidthProperty() &&
                                            bounds.Height == viewport.getHeightProperty()
                                        ? "yes" : "no"));
        lines.emplace_back();
        lines.push_back("ScreenDeviceName: " + window.getScreenDeviceNameProperty());
        lines.push_back("AllowUserResizing: " +
                        std::string(window.getAllowUserResizingProperty() ? "true" : "false"));
        lines.push_back("IsBorderlessEXT:   " +
                        std::string(window.getIsBorderlessEXTProperty() ? "true" : "false") +
                        "   (a CNA extension, not stock XNA)");
        lines.emplace_back();
        lines.push_back("ClientBounds is the window's drawable area; the viewport is what the");
        lines.push_back("device renders into right now. A render target makes them differ.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr int kTitleCount = 3;
    static constexpr const char* kTitles[kTitleCount] = {
        "CNA Examples -- title changed at runtime",
        "CNA Examples -- GameWindow::Title",
        "CNA Examples",
    };

    GameWindow& Window() const { return GetScreenManager()->getGameProperty().getWindowProperty(); }

    std::string originalTitle_;
    int titleIndex_ = 0;
    int titleChanges_ = 0;
};

} // namespace CnaExamples::Demos::Framework::WindowDemos
