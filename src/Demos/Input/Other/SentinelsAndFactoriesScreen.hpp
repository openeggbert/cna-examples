// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadButtons.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::Buttons;
using Microsoft::Xna::Framework::Input::GamePadButtons;
using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
using Microsoft::Xna::Framework::Input::Touch::TouchLocation;
using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

// Three small Input APIs that are easy to use incorrectly, and that no other
// screen in this area touches. All of them work on data built here, so none of
// this needs a controller or a touchscreen -- which is exactly why they are
// worth a screen: they are the parts you cannot check by waving hardware at it.
//
//  * TouchCollection::FindById returns BOOL and writes a SENTINEL location when
//    it fails. It does not throw and does not leave the out-parameter alone, so
//    code that ignores the return value gets a real TouchLocation object whose
//    contents mean "not found". Its state is Invalid -- that is the tell.
//
//  * TouchPanel::NO_FINGER is the "no such finger" id (-1). It is a real
//    constant rather than a magic number precisely so this comparison is
//    greppable; FNA declares it internal, CNA exposes it as NOXNA.
//
//  * GamePadButtons::FromButtonArray builds a button set from a list, which is
//    the only sane way to construct one for a test or a replay -- the normal
//    constructor takes a single packed flag value.
//
// Everything below is computed live and checked, so the swatch reports whether
// the claims still hold rather than asking you to trust the text.
class SentinelsAndFactoriesScreen : public DemoScreen {
public:
    SentinelsAndFactoriesScreen() : DemoScreen("Sentinels & Factories") {}

    void OnDemoLoad() override { RunChecks(); }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        lines.push_back("TouchCollection::FindById -- returns bool, WRITES A SENTINEL on failure:");
        lines.push_back("  FindById(existing id)  returned " + YesNo(foundExisting_) +
                        ", state " + stateOfFound_);
        lines.push_back("  FindById(absent id)    returned " + YesNo(foundAbsent_) +
                        ", state " + stateOfMissing_);
        lines.push_back("Ignore the return value and you still get a TouchLocation -- one whose");
        lines.push_back("state is Invalid. That is the tell; there is no exception.");
        lines.emplace_back();
        lines.push_back("TouchPanel::NO_FINGER = " + std::to_string((int)TouchPanel::NO_FINGER) +
                        "  -- a named constant, not a magic -1, so it greps.");
        lines.emplace_back();
        lines.push_back("GamePadButtons::FromButtonArray({A, X, LeftShoulder}):");
        lines.push_back("  A " + Pressed(aPressed_) + "   X " + Pressed(xPressed_) +
                        "   LeftShoulder " + Pressed(lbPressed_) + "   B " + Pressed(bPressed_));
        lines.push_back("The normal constructor takes one packed flag value, so this factory is");
        lines.push_back("the readable way to build a set for a test or an input replay.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        DrawVerdict(sb, font, end.Y + 6.0f,
                    mul(allHold_ ? Color(40, 200, 90, 255) : Color(220, 60, 60, 255),
                        TransitionAlpha()),
                    tint,
                    allHold_ ? "All " + std::to_string(checks_) + " claims verified live."
                             : "A claim did NOT hold: " + failure_);
    }

private:
    static std::string YesNo(bool value) { return value ? "true" : "false"; }
    static std::string Pressed(bool value) { return value ? "down" : "up  "; }

    static std::string StateName(TouchLocationState state) {
        switch (state) {
            case TouchLocationState::Invalid:  return "Invalid";
            case TouchLocationState::Moved:    return "Moved";
            case TouchLocationState::Pressed:  return "Pressed";
            default:                           return "Released";
        }
    }

    void Note(bool ok, const std::string& what) {
        ++checks_;
        if (!ok && allHold_) {
            allHold_ = false;
            failure_ = what;
        }
    }

    void RunChecks() {
        // A collection built here, so this needs no touchscreen.
        TouchCollection touches;
        touches.Add(TouchLocation(7, TouchLocationState::Pressed, Vector2(120.0f, 80.0f)));
        touches.Add(TouchLocation(9, TouchLocationState::Moved, Vector2(300.0f, 220.0f)));

        TouchLocation found(0, TouchLocationState::Invalid, Vector2(0.0f, 0.0f));
        foundExisting_ = touches.FindById(7, found);
        stateOfFound_ = StateName(found.getStateProperty());
        Note(foundExisting_, "FindById did not find id 7");
        Note(found.getStateProperty() == TouchLocationState::Pressed, "found location's state");

        // The failure path: the out-parameter IS written, with a sentinel.
        TouchLocation missing(42, TouchLocationState::Moved, Vector2(1.0f, 1.0f));
        foundAbsent_ = touches.FindById(1234, missing);
        stateOfMissing_ = StateName(missing.getStateProperty());
        Note(!foundAbsent_, "FindById claimed to find an absent id");
        Note(missing.getStateProperty() == TouchLocationState::Invalid,
             "the sentinel location was not Invalid");

        Note((int)TouchPanel::NO_FINGER == -1, "NO_FINGER is not -1");

        const GamePadButtons set =
            GamePadButtons::FromButtonArray({Buttons::A, Buttons::X, Buttons::LeftShoulder});
        aPressed_ = set.getAProperty() == ButtonState::Pressed;
        xPressed_ = set.getXProperty() == ButtonState::Pressed;
        lbPressed_ = set.getLeftShoulderProperty() == ButtonState::Pressed;
        bPressed_ = set.getBProperty() == ButtonState::Pressed;
        Note(aPressed_ && xPressed_ && lbPressed_, "a listed button was not pressed");
        Note(!bPressed_, "an unlisted button came back pressed");
    }

    std::string stateOfFound_ = "?";
    std::string stateOfMissing_ = "?";
    std::string failure_;
    int checks_ = 0;
    bool allHold_ = true;
    bool foundExisting_ = false;
    bool foundAbsent_ = false;
    bool aPressed_ = false;
    bool xPressed_ = false;
    bool lbPressed_ = false;
    bool bPressed_ = false;
};

} // namespace CnaExamples::Demos::Input::OtherDemos
