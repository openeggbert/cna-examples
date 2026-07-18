// SPDX-License-Identifier: MIT
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "GameStateManagement/GameScreen.hpp"

#include "Demos/Input/Keyboard/LiveKeyStateScreen.hpp"
#include "Demos/Input/Keyboard/PressedKeysListScreen.hpp"
#include "Demos/Input/Keyboard/KeyTransitionLogScreen.hpp"
#include "Demos/Input/Keyboard/ModifiersAndLocksScreen.hpp"
#include "Demos/Input/Keyboard/TextInputScreen.hpp"
#include "Demos/Input/Keyboard/ScancodeKeycodeNamesScreen.hpp"
#include "Demos/Input/Keyboard/StateEqualityHashScreen.hpp"
#include "Demos/Input/Keyboard/PlayerIndexSlotsScreen.hpp"
#include "Demos/Input/Keyboard/SingleKeyQueryScreen.hpp"
#include "Demos/Input/Keyboard/KeyHoldDurationScreen.hpp"

namespace CnaExamples::Navigation {

using CnaExamples::GameStateManagement::GameScreen;

// A single runnable demonstration within a Category. `create` is a factory
// so building the catalog never constructs a screen (and its content) until
// the user actually navigates to it.
struct DemoEntry {
    std::string title;
    std::string description;
    std::function<std::shared_ptr<GameScreen>()> create;
};

// A group of related demos within an Area (e.g. Input's "Keyboard" category).
struct CategoryEntry {
    std::string title;
    std::vector<DemoEntry> demos;
};

// A top-level CNA subsystem shown on the Home screen (e.g. "Input").
struct AreaEntry {
    std::string title;
    std::vector<CategoryEntry> categories;
};

// DemoEntry helper: builds an entry whose factory default-constructs T.
template <typename T>
DemoEntry MakeDemo(std::string title, std::string description) {
    return DemoEntry{std::move(title), std::move(description),
                     [] { return std::make_shared<T>(); }};
}

inline std::vector<DemoEntry> BuildKeyboardDemos() {
    using namespace CnaExamples::Demos::Input::KeyboardDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LiveKeyStateScreen>(
        "Live Key State", "IsKeyDown() on a curated keyboard-shaped grid"));
    demos.push_back(MakeDemo<PressedKeysListScreen>(
        "Pressed Keys List", "GetPressedKeys() live listing"));
    demos.push_back(MakeDemo<KeyTransitionLogScreen>(
        "Press/Release Log", "Edge-detected down/up events over time"));
    demos.push_back(MakeDemo<ModifiersAndLocksScreen>(
        "Modifiers & Locks", "GetModStateEXT() -- Shift/Ctrl/Alt/Caps/Num/Scroll"));
    demos.push_back(MakeDemo<TextInputScreen>(
        "Text Input", "TextInputEXT -- composed, IME-aware character events"));
    demos.push_back(MakeDemo<ScancodeKeycodeNamesScreen>(
        "Scancode vs. Keycode", "Physical vs. layout-dependent key naming (EXT)"));
    demos.push_back(MakeDemo<StateEqualityHashScreen>(
        "State Equals()/GetHashCode()", "Snapshot comparison correctness"));
    demos.push_back(MakeDemo<PlayerIndexSlotsScreen>(
        "GetState(PlayerIndex)", "The 4-slot local-multiplayer keyboard API"));
    demos.push_back(MakeDemo<SingleKeyQueryScreen>(
        "IsKeyDown / operator[]", "The single-key query surface, side by side"));
    demos.push_back(MakeDemo<KeyHoldDurationScreen>(
        "Hold Duration & Repeat", "Building typematic repeat on IsKeyDown + GameTime"));
    return demos;
}

// Builds the full Home -> Area -> Category -> Demo data set. This is the
// single place new areas/categories/demos get registered as they are
// implemented; see plan.md section 8 for what is intentionally still empty.
inline std::vector<AreaEntry> BuildAreaCatalog() {
    return {
        AreaEntry{"Input", {
            CategoryEntry{"Keyboard", BuildKeyboardDemos()},
            CategoryEntry{"Mouse", {}},
            CategoryEntry{"Gamepad", {}},
            CategoryEntry{"Touch", {}},
            CategoryEntry{"Other", {}},
        }},
        AreaEntry{"Audio", {}},
        AreaEntry{"Devices", {}},
        AreaEntry{"Net", {}},
        AreaEntry{"Media", {}},
        AreaEntry{"2D Graphics", {}},
        AreaEntry{"3D Graphics", {}},
    };
}

} // namespace CnaExamples::Navigation
