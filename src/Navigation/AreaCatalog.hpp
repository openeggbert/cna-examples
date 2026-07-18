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

#include "Demos/Input/Mouse/LivePositionButtonsScreen.hpp"
#include "Demos/Input/Mouse/ScrollWheelScreen.hpp"
#include "Demos/Input/Mouse/SetPositionScreen.hpp"
#include "Demos/Input/Mouse/CursorShapesScreen.hpp"
#include "Demos/Input/Mouse/ClickedEventScreen.hpp"
#include "Demos/Input/Mouse/RelativeModeScreen.hpp"
#include "Demos/Input/Mouse/GlobalPositionWarpScreen.hpp"
#include "Demos/Input/Mouse/CaptureScreen.hpp"
#include "Demos/Input/Mouse/StateEqualityHashScreen.hpp"
#include "Demos/Input/Mouse/ButtonTransitionLogScreen.hpp"

#include "Demos/Input/Gamepad/ConnectionAndCapabilitiesScreen.hpp"
#include "Demos/Input/Gamepad/LiveButtonsGridScreen.hpp"
#include "Demos/Input/Gamepad/DPadThumbsticksScreen.hpp"
#include "Demos/Input/Gamepad/TriggersScreen.hpp"
#include "Demos/Input/Gamepad/DeadZoneModesScreen.hpp"
#include "Demos/Input/Gamepad/VibrationScreen.hpp"
#include "Demos/Input/Gamepad/PowerInfoScreen.hpp"
#include "Demos/Input/Gamepad/PlayerIndexEXTScreen.hpp"
#include "Demos/Input/Gamepad/DeviceInfoScreen.hpp"
#include "Demos/Input/Gamepad/StateEqualityHashScreen.hpp"

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

inline std::vector<DemoEntry> BuildMouseDemos() {
    using namespace CnaExamples::Demos::Input::MouseDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LivePositionButtonsScreen>(
        "Live Position & Buttons", "GetState() -- position and all five button states"));
    demos.push_back(MakeDemo<ScrollWheelScreen>(
        "Scroll Wheel", "Cumulative value + derived per-frame delta, vertical and horizontal"));
    demos.push_back(MakeDemo<SetPositionScreen>(
        "SetPosition()", "The write side of the Mouse API -- warping the cursor"));
    demos.push_back(MakeDemo<CursorShapesScreen>(
        "Cursor Shapes", "MouseCursor's stock shape gallery (EXT)"));
    demos.push_back(MakeDemo<ClickedEventScreen>(
        "ClickedEXT Event", "Event-driven clicks vs. polling ButtonState (EXT)"));
    demos.push_back(MakeDemo<RelativeModeScreen>(
        "Relative Mode", "Absolute position vs. per-frame motion delta (EXT)"));
    demos.push_back(MakeDemo<GlobalPositionWarpScreen>(
        "Global Position & Warp", "Desktop-space coordinates, independent of the window (EXT)"));
    demos.push_back(MakeDemo<CaptureScreen>(
        "Capture", "Keep receiving events once the cursor leaves the window (EXT)"));
    demos.push_back(MakeDemo<StateEqualityHashScreen>(
        "State Equals()/GetHashCode()", "Snapshot comparison correctness"));
    demos.push_back(MakeDemo<ButtonTransitionLogScreen>(
        "Button Press/Release Log", "Edge-detected transitions from polled ButtonState"));
    return demos;
}

inline std::vector<DemoEntry> BuildGamepadDemos() {
    using namespace CnaExamples::Demos::Input::GamepadDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ConnectionAndCapabilitiesScreen>(
        "Connection & Capabilities", "IsConnected + GetCapabilities() across all 4 player slots"));
    demos.push_back(MakeDemo<LiveButtonsGridScreen>(
        "Live Buttons Grid", "IsButtonDown() on every digital button"));
    demos.push_back(MakeDemo<DPadThumbsticksScreen>(
        "DPad & Thumbsticks", "The DPad and analog stick positions, live"));
    demos.push_back(MakeDemo<TriggersScreen>(
        "Triggers", "Analog trigger values + GamePad::TriggerThreshold"));
    demos.push_back(MakeDemo<DeadZoneModesScreen>(
        "Dead Zone Modes", "None vs. IndependentAxes vs. Circular, side by side"));
    demos.push_back(MakeDemo<VibrationScreen>(
        "Vibration", "SetVibration() + SetTriggerVibrationEXT()"));
    demos.push_back(MakeDemo<PowerInfoScreen>(
        "Power Info", "Battery/charge state (EXT)"));
    demos.push_back(MakeDemo<PlayerIndexEXTScreen>(
        "Player Index LED", "The controller's own player-number LED (EXT)"));
    demos.push_back(MakeDemo<DeviceInfoScreen>(
        "Device Info", "GUID/path/serial/firmware/Steam handle/connection/touchpads (EXT)"));
    demos.push_back(MakeDemo<StateEqualityHashScreen>(
        "State Equals()/GetHashCode()", "Snapshot comparison correctness"));
    return demos;
}

// Builds the full Home -> Area -> Category -> Demo data set. This is the
// single place new areas/categories/demos get registered as they are
// implemented; see plan.md section 8 for what is intentionally still empty.
inline std::vector<AreaEntry> BuildAreaCatalog() {
    return {
        AreaEntry{"Input", {
            CategoryEntry{"Keyboard", BuildKeyboardDemos()},
            CategoryEntry{"Mouse", BuildMouseDemos()},
            CategoryEntry{"Gamepad", BuildGamepadDemos()},
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
