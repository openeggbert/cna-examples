// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <functional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseCursor.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Mouse;
using Microsoft::Xna::Framework::Input::MouseCursor;

// Demonstrates the NOXNA/EXT MouseCursor stock-shape gallery (a MonoGame-
// derived extension, not part of XNA 4.0/FNA): Space/Enter/A/tap cycles
// Mouse::SetCursor() through every stock cursor so the change is visible by
// moving the mouse over the window.
class CursorShapesScreen : public DemoScreen {
public:
    CursorShapesScreen() : DemoScreen("Mouse: Cursor Shapes (EXT)") {}

    void UnloadContent() override {
        // Leave the system in a normal state for whatever screen is next.
        Mouse::SetCursor(MouseCursor::getArrowProperty());
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            index_ = (index_ + 1) % (int)kCursors.size();
            Mouse::SetCursor(kCursors[index_].get());
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap cycles the cursor shape.");
        lines.push_back("Move the mouse over this window to see it.");
        lines.emplace_back();
        lines.push_back(std::string("Current: ") + kCursors[index_].name);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    struct NamedCursor {
        const char* name;
        std::function<MouseCursor&()> get;
    };

    static inline const std::vector<NamedCursor> kCursors = {
        {"Arrow", [] () -> MouseCursor& { return MouseCursor::getArrowProperty(); }},
        {"Crosshair", [] () -> MouseCursor& { return MouseCursor::getCrosshairProperty(); }},
        {"Hand", [] () -> MouseCursor& { return MouseCursor::getHandProperty(); }},
        {"IBeam", [] () -> MouseCursor& { return MouseCursor::getIBeamProperty(); }},
        {"No", [] () -> MouseCursor& { return MouseCursor::getNoProperty(); }},
        {"SizeAll", [] () -> MouseCursor& { return MouseCursor::getSizeAllProperty(); }},
        {"SizeNESW", [] () -> MouseCursor& { return MouseCursor::getSizeNESWProperty(); }},
        {"SizeNS", [] () -> MouseCursor& { return MouseCursor::getSizeNSProperty(); }},
        {"SizeNWSE", [] () -> MouseCursor& { return MouseCursor::getSizeNWSEProperty(); }},
        {"SizeWE", [] () -> MouseCursor& { return MouseCursor::getSizeWEProperty(); }},
        {"Wait", [] () -> MouseCursor& { return MouseCursor::getWaitProperty(); }},
        {"WaitArrow", [] () -> MouseCursor& { return MouseCursor::getWaitArrowProperty(); }},
    };

    int index_ = 0;
};

} // namespace CnaExamples::Demos::Input::MouseDemos
