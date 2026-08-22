// SPDX-License-Identifier: MIT
//
// Adapted from the official XNA "Game State Management" sample's InputState
// (see GameScreen.hpp for provenance), extended with a touch "tap" query
// (IsNewTap) and a mouse "click" query (IsNewClick) since cna-examples
// targets both Android and desktop as "now" platforms and its menus must
// be usable by touch or mouse, not just keyboard/gamepad.
#pragma once

#include <array>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace CnaExamples::GameStateManagement {

using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::GamePadState;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::Keys;
using Microsoft::Xna::Framework::Input::Buttons;
using Microsoft::Xna::Framework::Input::Mouse;
using Microsoft::Xna::Framework::Input::MouseState;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;
using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;

// Reads input from keyboard, gamepad, mouse and touch, tracking current/
// previous state and exposing high-level "menu up/down/select/cancel",
// "tap" and "click" queries so a single MenuScreen implementation can
// drive all four.
class InputState {
public:
    static const int MaxInputs = 4;

    std::array<KeyboardState, MaxInputs> CurrentKeyboardStates;
    std::array<GamePadState,  MaxInputs> CurrentGamePadStates;

    std::array<KeyboardState, MaxInputs> LastKeyboardStates;
    std::array<GamePadState,  MaxInputs> LastGamePadStates;

    std::array<bool, MaxInputs> GamePadWasConnected{};

    // A menu action injected by the headless driver (see Harness/CommandLine.hpp)
    // instead of coming from a real device. Only these four exist: they are
    // exactly the vocabulary MenuScreen and DemoScreen navigate with, so a
    // scripted run can reach any screen in the catalog without an X server, a
    // window manager or synthetic X11 key events.
    enum class ScriptedAction { None, Up, Down, Left, Right, Select, Cancel };

    InputState() = default;

    // Queues an action to be reported by exactly one future Update(). Queued
    // rather than applied immediately so each action lands on its own frame,
    // matching how a real key press is seen (down this frame, up the next) --
    // menus reject a repeat within the same frame otherwise.
    void QueueScriptedAction(ScriptedAction action) { scripted_.push_back(action); }

    [[nodiscard]] bool HasQueuedScriptedActions() const { return !scripted_.empty(); }

    // Headless runs stop reading real devices altogether. A verification sweep
    // must depend only on what it scripted: a stray X event reaching the window
    // (seen once in practice under Xvfb -- a phantom "select" that silently
    // toggled a demo's mode before the screenshot) otherwise turns a sweep into
    // a flaky one, and the resulting screenshot looks plausible while showing
    // the wrong state.
    void SetScriptedOnly(bool scriptedOnly) { scriptedOnly_ = scriptedOnly; }

    // Drives the pointer directly, for scripted runs. Drag-to-scroll is a
    // gesture, not a keystroke, so it is unreachable from QueueScriptedAction --
    // and an unverified gesture in the one code path that exists purely for
    // touch users is exactly the kind of thing that quietly rots.
    void SetScriptedPointer(bool down, Vector2 position) {
        scriptedPointerDown_ = down;
        scriptedPointerPosition_ = position;
    }

    // Reads the latest state of the keyboard, gamepad and touch panel.
    void Update() {
        // Consume at most one scripted action per frame. It is combined with,
        // not substituted for, real device state, so a scripted run still works
        // if a real key happens to be held.
        currentScripted_ = ScriptedAction::None;
        if (!scripted_.empty()) {
            currentScripted_ = scripted_.front();
            scripted_.erase(scripted_.begin());
        }

        if (scriptedOnly_) {
            // Still clear the per-frame edge state, so a stale tap/click from
            // before the switch can't be reported forever.
            newTapPosition_.reset();
            newClickPosition_.reset();

            const bool wasScriptedDown = pointerDown_;
            pointerDown_ = scriptedPointerDown_;
            if (pointerDown_) pointerPosition_ = scriptedPointerPosition_;
            pointerReleased_ = wasScriptedDown && !pointerDown_;
            return;
        }

        // GamePad::GetState starts CNA's controller subsystem on first use.
        // ScreenManager updates before its first Draw, so defer that first call
        // by one frame and let the initial menu be presented immediately.
        const bool pollGamePad = gamePadPollingStarted_;
        for (int i = 0; i < MaxInputs; i++) {
            LastKeyboardStates[i] = CurrentKeyboardStates[i];
            CurrentKeyboardStates[i] = Keyboard::GetState(static_cast<PlayerIndex>(i));

            if (pollGamePad) {
                LastGamePadStates[i]  = CurrentGamePadStates[i];
                CurrentGamePadStates[i]  = GamePad::GetState(static_cast<PlayerIndex>(i));

                if (CurrentGamePadStates[i].getIsConnectedProperty())
                    GamePadWasConnected[i] = true;
            }
        }
        gamePadPollingStarted_ = true;

        newTapPosition_.reset();
        std::optional<Vector2> heldTouch;
        for (const auto& touch : TouchPanel::GetState()) {
            if (touch.getStateProperty() == TouchLocationState::Pressed) {
                newTapPosition_ = touch.getPositionProperty();
            }
            // Pressed and Moved both mean "still on the glass"; Released does not.
            if (touch.getStateProperty() == TouchLocationState::Pressed ||
                touch.getStateProperty() == TouchLocationState::Moved) {
                heldTouch = touch.getPositionProperty();
            }
        }

        const MouseState mouse = Mouse::GetState();
        newClickPosition_.reset();
        if (mouse.getLeftButtonProperty() == ButtonState::Pressed &&
            previousMouseLeftButton_ == ButtonState::Released) {
            newClickPosition_ = Vector2((float)mouse.getXProperty(), (float)mouse.getYProperty());
        }
        previousMouseLeftButton_ = mouse.getLeftButtonProperty();

        // A single "pointer" abstraction over touch and mouse, tracking the held
        // state rather than just the press edge. Drag-to-scroll needs to know
        // where the finger is *now* and when it lifted; IsNewTap/IsNewClick only
        // ever report the frame contact began.
        const bool wasDown = pointerDown_;
        if (heldTouch.has_value()) {
            pointerDown_ = true;
            pointerPosition_ = heldTouch.value();
        } else if (mouse.getLeftButtonProperty() == ButtonState::Pressed) {
            pointerDown_ = true;
            pointerPosition_ = Vector2((float)mouse.getXProperty(), (float)mouse.getYProperty());
        } else {
            pointerDown_ = false;
        }
        pointerReleased_ = wasDown && !pointerDown_;
    }

    // True while a finger or the left mouse button is held; `position` is where.
    bool IsPointerDown(Vector2& position) const {
        if (!pointerDown_) return false;
        position = pointerPosition_;
        return true;
    }

    // True on the single frame contact ended; `position` is the last known
    // location, which is what a tap-to-select wants.
    bool IsPointerReleased(Vector2& position) const {
        if (!pointerReleased_) return false;
        position = pointerPosition_;
        return true;
    }

    // Helper for checking if a key was newly pressed during this update.
    //
    // A scripted action reports itself here as the key it stands for, so a demo
    // that binds raw keys rather than the menu verbs (Left/Right especially,
    // which have no menu equivalent) is still reachable from --keys.
    bool IsNewKeyPress(Keys key, std::optional<PlayerIndex> controllingPlayer,
                       PlayerIndex& playerIndex) {
        if (currentScripted_ != ScriptedAction::None && key == ScriptedKey(currentScripted_)) {
            playerIndex = controllingPlayer.value_or(PlayerIndex::One);
            return true;
        }
        if (controllingPlayer.has_value()) {
            playerIndex = controllingPlayer.value();
            int i = static_cast<int>(playerIndex);
            return CurrentKeyboardStates[i].IsKeyDown(key) &&
                   LastKeyboardStates[i].IsKeyUp(key);
        }
        return IsNewKeyPress(key, PlayerIndex::One, playerIndex) ||
               IsNewKeyPress(key, PlayerIndex::Two, playerIndex) ||
               IsNewKeyPress(key, PlayerIndex::Three, playerIndex) ||
               IsNewKeyPress(key, PlayerIndex::Four, playerIndex);
    }

    // Helper for checking if a button was newly pressed during this update.
    bool IsNewButtonPress(Buttons button, std::optional<PlayerIndex> controllingPlayer,
                          PlayerIndex& playerIndex) {
        if (controllingPlayer.has_value()) {
            playerIndex = controllingPlayer.value();
            int i = static_cast<int>(playerIndex);
            return CurrentGamePadStates[i].IsButtonDown(button) &&
                   LastGamePadStates[i].IsButtonUp(button);
        }
        return IsNewButtonPress(button, PlayerIndex::One, playerIndex) ||
               IsNewButtonPress(button, PlayerIndex::Two, playerIndex) ||
               IsNewButtonPress(button, PlayerIndex::Three, playerIndex) ||
               IsNewButtonPress(button, PlayerIndex::Four, playerIndex);
    }

    // Checks for a "menu select" input action (keyboard/gamepad only; touch
    // taps are queried separately via IsNewTap since they carry a position).
    bool IsMenuSelect(std::optional<PlayerIndex> controllingPlayer, PlayerIndex& playerIndex) {
        if (currentScripted_ == ScriptedAction::Select) { playerIndex = PlayerIndex::One; return true; }
        return IsNewKeyPress(Keys::Space, controllingPlayer, playerIndex) ||
               IsNewKeyPress(Keys::Enter, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::A, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Start, controllingPlayer, playerIndex);
    }

    // Checks for a "menu cancel" input action.
    bool IsMenuCancel(std::optional<PlayerIndex> controllingPlayer, PlayerIndex& playerIndex) {
        if (currentScripted_ == ScriptedAction::Cancel) { playerIndex = PlayerIndex::One; return true; }
        return IsNewKeyPress(Keys::Escape, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::B, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Back, controllingPlayer, playerIndex);
    }

    // Checks for a "menu up" input action.
    bool IsMenuUp(std::optional<PlayerIndex> controllingPlayer) {
        if (currentScripted_ == ScriptedAction::Up) return true;
        PlayerIndex playerIndex;
        return IsNewKeyPress(Keys::Up, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::DPadUp, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::LeftThumbstickUp, controllingPlayer, playerIndex);
    }

    // Checks for a "menu down" input action.
    bool IsMenuDown(std::optional<PlayerIndex> controllingPlayer) {
        if (currentScripted_ == ScriptedAction::Down) return true;
        PlayerIndex playerIndex;
        return IsNewKeyPress(Keys::Down, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::DPadDown, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::LeftThumbstickDown, controllingPlayer, playerIndex);
    }

    // NOXNA-style extension (not part of the original sample): reports a
    // newly-began touch this frame, if any. TouchLocationState::Pressed is
    // only reported on the first frame of a contact, so no extra "new vs.
    // held" bookkeeping is needed beyond re-checking it every Update().
    bool IsNewTap(Vector2& position) const {
        if (!newTapPosition_.has_value())
            return false;
        position = newTapPosition_.value();
        return true;
    }

    // NOXNA-style extension: reports a newly-pressed left mouse button this
    // frame, if any, at the cursor's window-relative position -- the mouse
    // equivalent of IsNewTap(), so desktop users can click a menu entry the
    // same way touch users tap one (see MenuScreen::HandleInput).
    bool IsNewClick(Vector2& position) const {
        if (!newClickPosition_.has_value())
            return false;
        position = newClickPosition_.value();
        return true;
    }

private:
    std::optional<Vector2> newTapPosition_;
    std::optional<Vector2> newClickPosition_;
    ButtonState previousMouseLeftButton_ = ButtonState::Released;
    // The keyboard key each scripted action stands for. Keys::None for actions
    // with no single key equivalent.
    static Keys ScriptedKey(ScriptedAction action) {
        switch (action) {
            case ScriptedAction::Up:     return Keys::Up;
            case ScriptedAction::Down:   return Keys::Down;
            case ScriptedAction::Left:   return Keys::Left;
            case ScriptedAction::Right:  return Keys::Right;
            case ScriptedAction::Select: return Keys::Space;
            case ScriptedAction::Cancel: return Keys::Escape;
            default:                     return Keys::None;
        }
    }

    std::vector<ScriptedAction> scripted_;
    ScriptedAction currentScripted_ = ScriptedAction::None;
    bool scriptedOnly_ = false;
    bool gamePadPollingStarted_ = false;
    bool pointerDown_ = false;
    bool pointerReleased_ = false;
    Vector2 pointerPosition_;
    bool scriptedPointerDown_ = false;
    Vector2 scriptedPointerPosition_;
};

} // namespace CnaExamples::GameStateManagement
