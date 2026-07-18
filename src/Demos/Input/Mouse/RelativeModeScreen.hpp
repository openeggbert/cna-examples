// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Mouse;
using Microsoft::Xna::Framework::Input::MouseState;

// Demonstrates the NOXNA/EXT IsRelativeMouseModeEXT toggle: normally
// MouseState.X/Y report an absolute, window-relative position; in relative
// mode (typical for a first-person camera) the same fields instead report
// per-frame motion deltas, and the OS cursor is hidden/confined. Space/
// Enter/A/tap toggles it. Always restored to false on exit (UnloadContent)
// so leaving this screen doesn't leave the whole app's cursor behavior
// altered.
class RelativeModeScreen : public DemoScreen {
public:
    RelativeModeScreen() : DemoScreen("Mouse: Relative Mode (EXT)") {}

    void UnloadContent() override {
        Mouse::setIsRelativeMouseModeEXTProperty(false);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            Mouse::setIsRelativeMouseModeEXTProperty(!Mouse::getIsRelativeMouseModeEXTProperty());
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const bool relative = Mouse::getIsRelativeMouseModeEXTProperty();
        const MouseState state = Mouse::GetState();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap toggles relative mouse mode.");
        lines.push_back("Esc still works to go Back even while relative mode is on.");
        lines.emplace_back();
        lines.push_back(std::string("IsRelativeMouseModeEXT: ") + (relative ? "ON" : "off"));
        lines.push_back(std::string("MouseState now reports: ") +
                        (relative ? "per-frame motion DELTA" : "absolute position"));
        lines.push_back("X, Y: (" + std::to_string(state.getXProperty()) + ", " +
                        std::to_string(state.getYProperty()) + ")");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Input::MouseDemos
