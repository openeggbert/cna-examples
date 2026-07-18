// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Net/NetDemoHelpers.hpp"

namespace CnaExamples::Demos::Net::NetworkGamerDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Net::NetworkGamer;

// Demonstrates NetworkSession::AllGamers/LocalGamers/RemoteGamers/PreviousGamers and the
// per-gamer NetworkGamer properties (Id/IsHost/IsLocal/RoundtripTime), on a
// NetworkSessionType::Local session created with maxLocalGamers=2. In practice this yields
// only 1 local gamer, not 2: NetworkSession's implicit-local-gamer loop skips any
// SignedInGamer with IsGuest true, and 3 of the 4 stub SignedInGamers GamerServicesComponent
// populates (all but SignedInGamers[0]) are guests -- a real, verified consequence of the
// stub data, not a bug in this screen.
class GamerRosterScreen : public DemoScreen {
public:
    GamerRosterScreen() : DemoScreen("NetworkGamer: Roster") {}

    void LoadContent() override {
        session_ = NetworkSession::Create(NetworkSessionType::Local, 2, 4);
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
        lines.push_back("AllGamers: " + std::to_string(session_->getAllGamersProperty().getCountProperty()) +
                         "   LocalGamers: " + std::to_string(session_->getLocalGamersProperty().getCountProperty()) +
                         "   RemoteGamers: " + std::to_string(session_->getRemoteGamersProperty().getCountProperty()) +
                         "   PreviousGamers: " + std::to_string(session_->getPreviousGamersProperty().getCountProperty()));
        lines.emplace_back();

        for (NetworkGamer* g : session_->getAllGamersProperty()) {
            lines.push_back(g->getGamertagProperty() + "  Id=" + std::to_string(g->getIdProperty()) +
                             "  IsHost=" + (g->getIsHostProperty() ? "true" : "false") +
                             "  IsLocal=" + (g->getIsLocalProperty() ? "true" : "false") +
                             "  RTT=" + std::to_string((int)g->getRoundtripTimeProperty().getTotalMillisecondsProperty()) + "ms");
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    NetworkSession* session_ = nullptr;
};

} // namespace CnaExamples::Demos::Net::NetworkGamerDemos
