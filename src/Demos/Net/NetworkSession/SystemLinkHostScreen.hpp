// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Net/GamerJoinedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/GamerLeftEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Net/NetDemoHelpers.hpp"

namespace CnaExamples::Demos::Net::NetworkSessionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Net::GamerJoinedEventArgs;
using Microsoft::Xna::Framework::Net::GamerLeftEventArgs;

// Demonstrates hosting a real NetworkSessionType::SystemLink session --
// unlike Local, this is genuinely backed by an ENet UDP socket bound to a
// real port and discoverable over LAN/loopback (see the DiscoverAndJoin
// screen). This screen keeps the session open for as long as it stays on
// screen; run a second cna_examples instance and pick "Discover & Join"
// there to see this session actually found and joined.
class SystemLinkHostScreen : public DemoScreen {
public:
    SystemLinkHostScreen() : DemoScreen("NetworkSession: SystemLink Host") {}

    void LoadContent() override {
        // The 8 requested here is never actually applied -- EndCreate() unconditionally
        // hardcodes MaxGamers to 69 for every Create() overload, a real, faithfully-preserved
        // upstream FNA quirk (its own reference source does the same), not a CNA bug or a typo
        // in this demo. See MaxGamers below, which genuinely reads 69.
        session_ = NetworkSession::Create(NetworkSessionType::SystemLink, 1, 8);
        session_->GamerJoined += [this](System::Object*, const GamerJoinedEventArgs&) { gamerJoinedCount_++; };
        session_->GamerLeft += [this](System::Object*, const GamerLeftEventArgs&) { gamerLeftCount_++; };
        // Replays for gamers already present at subscription time (this session's own
        // initial local gamer) only once Update() actually runs -- see NetworkSession::
        // GamerJoined's own doc comment for why this call is required here.
        session_->Update();
    }

    void UnloadContent() override {
        EndSession(session_);
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        session_->Update();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Hosting a real SystemLink session -- run a 2nd instance of this app and");
        lines.push_back("open Discover & Join to find and join it over a real UDP socket.");
        lines.emplace_back();
        lines.push_back(std::string("SessionType: ") + SessionTypeName(session_->getSessionTypeProperty()));
        lines.push_back(std::string("SessionState: ") + SessionStateName(session_->getSessionStateProperty()));
        lines.push_back("IsHost: " + std::string(session_->getIsHostProperty() ? "true" : "false"));
        lines.push_back("MaxGamers: " + std::to_string(session_->getMaxGamersProperty()) +
                         " (always 69 -- real upstream FNA quirk, not a bug)");
        lines.push_back("AllGamers.Count: " + std::to_string(session_->getAllGamersProperty().getCountProperty()));
        lines.push_back("Host gamertag: " +
                         (session_->getHostProperty() != nullptr
                              ? session_->getHostProperty()->getGamertagProperty()
                              : std::string("(none)")));
        lines.emplace_back();
        lines.push_back("GamerJoined fired: " + std::to_string(gamerJoinedCount_));
        lines.push_back("GamerLeft fired: " + std::to_string(gamerLeftCount_));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    NetworkSession* session_ = nullptr;
    int gamerJoinedCount_ = 0;
    int gamerLeftCount_ = 0;
};

} // namespace CnaExamples::Demos::Net::NetworkSessionDemos
