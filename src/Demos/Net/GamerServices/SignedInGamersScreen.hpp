// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "System/EventHandler.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Net::GamerServicesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::GamerServices::Gamer;
using Microsoft::Xna::Framework::GamerServices::SignedInGamer;

// Demonstrates Gamer::SignedInGamers -- populated with 4 "Stub Gamer" entries the instant
// GamerServicesDispatcher::Initialize() runs (this app's own GamerServicesComponent does that
// at startup; see CnaExamplesGame.hpp), matching FNA's own permanent stub-sign-in behavior
// rather than any real platform sign-in flow. Also subscribes the static SignedIn/SignedOut
// events, which are genuine public XNA 4.0 API but never actually raised in this platform's
// implementation -- expect both counters to stay at 0.
class SignedInGamersScreen : public DemoScreen {
public:
    SignedInGamersScreen() : DemoScreen("GamerServices: Signed-In Gamers") {}

    void LoadContent() override {
        signedInToken_ = SignedInGamer::SignedIn.Add(
            [this](System::Object*, const auto&) { signedInCount_++; });
        signedOutToken_ = SignedInGamer::SignedOut.Add(
            [this](System::Object*, const auto&) { signedOutCount_++; });
    }

    void UnloadContent() override {
        SignedInGamer::SignedIn.Remove(signedInToken_);
        SignedInGamer::SignedOut.Remove(signedOutToken_);
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        auto* gamers = Gamer::getSignedInGamersProperty();
        lines.push_back("SignedInGamers.Count: " + std::to_string(gamers->getCountProperty()));
        lines.emplace_back();

        for (SignedInGamer* g : *gamers) {
            lines.push_back(g->getGamertagProperty() + " (" + g->getDisplayNameProperty() + ")" +
                             "  Player" + std::to_string((int)g->getPlayerIndexProperty() + 1) +
                             "  Live=" + (g->getIsSignedInToLiveProperty() ? "true" : "false") +
                             "  Guest=" + (g->getIsGuestProperty() ? "true" : "false"));
        }
        lines.emplace_back();
        lines.push_back("SignedIn fired: " + std::to_string(signedInCount_) +
                         "   SignedOut fired: " + std::to_string(signedOutCount_));
        lines.push_back("(real public XNA events, never raised on this platform)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    System::EventHandler<Microsoft::Xna::Framework::GamerServices::SignedInEventArgs>::Token signedInToken_{};
    System::EventHandler<Microsoft::Xna::Framework::GamerServices::SignedOutEventArgs>::Token signedOutToken_{};
    int signedInCount_ = 0;
    int signedOutCount_ = 0;
};

} // namespace CnaExamples::Demos::Net::GamerServicesDemos
