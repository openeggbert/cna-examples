// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::MouseDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::Mouse;
using Microsoft::Xna::Framework::Input::MouseState;

// Demonstrates Mouse::GetState(): live cursor position and all five button
// states (Left/Right/Middle/XButton1/XButton2). Also draws a small marker at
// the cursor position, doubling as the manual hardware-validation tool
// (see plan.md section 5.1) for "does the mouse move/click actually work".
class LivePositionButtonsScreen : public DemoScreen {
public:
    LivePositionButtonsScreen() : DemoScreen("Mouse: Live Position & Buttons") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const MouseState state = Mouse::GetState();

        std::vector<std::string> lines;
        lines.push_back("Move the mouse and click buttons; a marker follows the cursor.");
        lines.emplace_back();
        lines.push_back("Position: (" + std::to_string(state.getXProperty()) + ", " +
                        std::to_string(state.getYProperty()) + ")");
        lines.emplace_back();
        AppendButton(lines, "Left", state.getLeftButtonProperty());
        AppendButton(lines, "Right", state.getRightButtonProperty());
        AppendButton(lines, "Middle", state.getMiddleButtonProperty());
        AppendButton(lines, "XButton1", state.getXButton1Property());
        AppendButton(lines, "XButton2", state.getXButton2Property());

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        // Cursor-following crosshair marker, drawn as short text glyphs since
        // this app has no primitive-line helper (SpriteFont-only rendering).
        const Vector2 pos((float)state.getXProperty(), (float)state.getYProperty());
        sb.DrawString(font, "+", Vector2(pos.X - 6.0f, pos.Y - 14.0f),
                      mul(Color::Yellow, TransitionAlpha()));
    }

private:
    static void AppendButton(std::vector<std::string>& lines, const char* label, ButtonState state) {
        lines.push_back(std::string(label) + ": " +
                        (state == ButtonState::Pressed ? "Pressed" : "Released"));
    }
};

} // namespace CnaExamples::Demos::Input::MouseDemos
