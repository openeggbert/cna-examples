// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::KeyboardDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::Keys;

// Demonstrates the four scancode/keycode NOXNA/EXT helpers on the most
// recently pressed key: GetKeyNameEXT (layout-dependent "virtual key" name)
// vs. GetScancodeNameEXT (physical, layout-independent position name), and
// their inverses/round-trips (GetKeyFromNameEXT, GetScancodeFromNameEXT,
// GetKeyFromScancodeEXT). On a non-QWERTY layout the two names can differ
// for the same physical key -- that distinction is the whole point of the
// scancode/keycode split.
class ScancodeKeycodeNamesScreen : public DemoScreen {
public:
    ScancodeKeycodeNamesScreen()
        : DemoScreen("Keyboard: Scancode vs. Keycode (EXT)"), previousPressed_(Keyboard::GetState().GetPressedKeys()) {}

protected:
    void OnDemoUpdate(GameTime&) override {
        const std::vector<Keys> current = Keyboard::GetState().GetPressedKeys();
        for (Keys key : current) {
            if (std::find(previousPressed_.begin(), previousPressed_.end(), key) == previousPressed_.end()) {
                lastKey_ = key;
                haveKey_ = true;
            }
        }
        previousPressed_ = current;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Press any key. Its scancode/keycode round-trip is shown below.");
        lines.emplace_back();

        if (!haveKey_) {
            lines.push_back("(no key pressed yet)");
        } else {
            const std::string keyName = Keyboard::GetKeyNameEXT(lastKey_);
            const std::string scancodeName = Keyboard::GetScancodeNameEXT(lastKey_);
            const Keys keyFromScancode = Keyboard::GetKeyFromScancodeEXT(lastKey_);
            const Keys keyRoundTrip = Keyboard::GetKeyFromNameEXT(keyName);
            const Keys scancodeRoundTrip = Keyboard::GetScancodeFromNameEXT(scancodeName);

            lines.push_back("GetKeyNameEXT (layout-dependent):      \"" + keyName + "\"");
            lines.push_back("GetScancodeNameEXT (physical position): \"" + scancodeName + "\"");
            lines.emplace_back();
            lines.push_back("GetKeyFromScancodeEXT(lastKey as scancode): " +
                            Keyboard::GetKeyNameEXT(keyFromScancode));
            lines.push_back(std::string("GetKeyFromNameEXT(keyName) round-trips: ") +
                            (keyRoundTrip == lastKey_ ? "yes" : "no"));
            lines.push_back(std::string("GetScancodeFromNameEXT(scancodeName) round-trips: ") +
                            (scancodeRoundTrip == lastKey_ ? "yes" : "no"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::vector<Keys> previousPressed_;
    Keys lastKey_ = Keys::None;
    bool haveKey_ = false;
};

} // namespace CnaExamples::Demos::Input::KeyboardDemos
