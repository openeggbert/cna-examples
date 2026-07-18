// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Net/GamerJoinedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/GamerLeftEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/HostChangedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionProperties.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Net/NetDemoHelpers.hpp"

namespace CnaExamples::Demos::Net::NetworkSessionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Net::GamerJoinedEventArgs;
using Microsoft::Xna::Framework::Net::GamerLeftEventArgs;
using Microsoft::Xna::Framework::Net::HostChangedEventArgs;
using Microsoft::Xna::Framework::Net::LocalNetworkGamer;
using Microsoft::Xna::Framework::Net::NetworkSessionProperties;

// Demonstrates NetworkSessionProperties (custom int? session-filter columns) plus the
// GamerJoined/GamerLeft/HostChanged event trio. GamerJoined is documented to only replay for
// gamers already present at +=-subscription time once Update() runs once afterward (see
// NetworkSession::GamerJoined's own doc comment) -- LoadContent() below follows that exact
// pattern. HostChanged is subscribed too but is never actually raised on this branch (real
// host migration is a stored-but-inert property here, not implemented).
class SessionPropertiesAndEventsScreen : public DemoScreen {
public:
    SessionPropertiesAndEventsScreen() : DemoScreen("NetworkSession: Properties & Events") {}

    void LoadContent() override {
        NetworkSessionProperties properties;
        properties.Add(7);
        properties.Add(std::nullopt);
        properties.Add(42);

        session_ = NetworkSession::Create(NetworkSessionType::Local, 2, 4, 0, properties);
        session_->GamerJoined += [this](System::Object*, const GamerJoinedEventArgs& e) {
            gamerJoinedCount_++;
            lastGamerJoined_ = e.getGamerProperty()->getGamertagProperty();
        };
        session_->GamerLeft += [this](System::Object*, const GamerLeftEventArgs&) { gamerLeftCount_++; };
        session_->HostChanged += [this](System::Object*, const HostChangedEventArgs&) { hostChangedCount_++; };
        // Required to receive the replayed join events for this session's own 2 initial local
        // gamers -- see the class doc comment above.
        session_->Update();
    }

    void UnloadContent() override {
        EndSession(session_);
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        session_->Update();
    }

    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            session_->ResetReady();
        } else if (input.IsNewKeyPress(Keys::J, ControllingPlayer(), playerIndex)) {
            LocalNetworkGamer* g = session_->getLocalGamersProperty()[0];
            g->setIsReadyProperty(!g->getIsReadyProperty());
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: ResetReady()   J: toggle local gamer[0] IsReady");
        lines.emplace_back();
        lines.push_back("SessionProperties (int?, 3 slots): ");
        std::string props = "  [";
        for (const auto& p : session_->getSessionPropertiesProperty()) {
            props += p.has_value() ? std::to_string(*p) : std::string("null");
            props += " ";
        }
        props += "]";
        lines.push_back(props);
        lines.push_back("IsEveryoneReady: " + std::string(session_->getIsEveryoneReadyProperty() ? "true" : "false"));
        lines.emplace_back();
        lines.push_back("GamerJoined fired: " + std::to_string(gamerJoinedCount_) +
                         (lastGamerJoined_.empty() ? "" : (" (last: " + lastGamerJoined_ + ")")));
        lines.push_back("GamerLeft fired: " + std::to_string(gamerLeftCount_));
        lines.push_back("HostChanged fired: " + std::to_string(hostChangedCount_) +
                         "  (host migration not implemented on this branch -- expect 0)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    NetworkSession* session_ = nullptr;
    int gamerJoinedCount_ = 0;
    std::string lastGamerJoined_;
    int gamerLeftCount_ = 0;
    int hostChangedCount_ = 0;
};

} // namespace CnaExamples::Demos::Net::NetworkSessionDemos
