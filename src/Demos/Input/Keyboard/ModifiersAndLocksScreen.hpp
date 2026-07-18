// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Input/KeyModifiers.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::KeyboardDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using CNA::Input::KeyModifiersEXT;
using Microsoft::Xna::Framework::Input::Keyboard;

// Demonstrates Keyboard::GetModStateEXT(), the NOXNA/EXT extension that
// reports held modifier keys (Shift/Ctrl/Alt/Gui) and toggled lock keys
// (Caps/Num/Scroll) as a single bitflag snapshot, independent of tracking
// individual Keys via IsKeyDown.
class ModifiersAndLocksScreen : public DemoScreen {
public:
    ModifiersAndLocksScreen() : DemoScreen("Keyboard: Modifiers & Locks (EXT)") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const KeyModifiersEXT mods = Keyboard::GetModStateEXT();

        std::vector<std::string> lines;
        lines.push_back("Keyboard::GetModStateEXT() -- hold Shift/Ctrl/Alt, or toggle a lock key:");
        lines.emplace_back();

        AppendFlag(lines, "Shift", mods, KeyModifiersEXT::Shift);
        AppendFlag(lines, "Ctrl", mods, KeyModifiersEXT::Ctrl);
        AppendFlag(lines, "Alt", mods, KeyModifiersEXT::Alt);
        AppendFlag(lines, "Gui (Win/Cmd)", mods, KeyModifiersEXT::Gui);
        AppendFlag(lines, "Mode (AltGr)", mods, KeyModifiersEXT::Mode);
        lines.emplace_back();
        AppendFlag(lines, "Caps Lock", mods, KeyModifiersEXT::Caps);
        AppendFlag(lines, "Num Lock", mods, KeyModifiersEXT::Num);
        AppendFlag(lines, "Scroll Lock", mods, KeyModifiersEXT::Scroll);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static void AppendFlag(std::vector<std::string>& lines, const char* label,
                           KeyModifiersEXT mods, KeyModifiersEXT flag) {
        const bool active = (mods & flag) != KeyModifiersEXT::None;
        lines.push_back(std::string(label) + ": " + (active ? "ON" : "off"));
    }
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
