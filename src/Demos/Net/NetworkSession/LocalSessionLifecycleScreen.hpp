// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Net/GameEndedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/GameStartedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndedEventArgs.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Net/NetDemoHelpers.hpp"

namespace CnaExamples::Demos::Net::NetworkSessionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Net::GameEndedEventArgs;
using Microsoft::Xna::Framework::Net::GameStartedEventArgs;
using Microsoft::Xna::Framework::Net::LocalNetworkGamer;
using Microsoft::Xna::Framework::Net::NetworkSessionEndedEventArgs;

// Demonstrates NetworkSession::StartGame()/EndGame()/RemoveGamer() and the resulting
// NetworkSessionState transitions -- NetworkSessionType::Local, so entirely single-process
// with no real networking involved (Local sessions route their event queue purely in-memory).
// Session events are queued internally and only raised on Update(), not synchronously --
// OnDemoUpdate() drives that every active frame.
class LocalSessionLifecycleScreen : public DemoScreen {
public:
    LocalSessionLifecycleScreen() : DemoScreen("NetworkSession: Local Lifecycle") {}

    void LoadContent() override {
        session_ = NetworkSession::Create(NetworkSessionType::Local, 1, 4);
        session_->GameStarted += [this](System::Object*, const GameStartedEventArgs&) { gameStartedCount_++; };
        session_->GameEnded += [this](System::Object*, const GameEndedEventArgs&) { gameEndedCount_++; };
        session_->SessionEnded += [this](System::Object*, const NetworkSessionEndedEventArgs& e) {
            sessionEndedCount_++;
            lastEndReason_ = EndReasonName(e.getEndReasonProperty());
        };
        localGamer_ = session_->getLocalGamersProperty()[0];
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
        if (input.IsNewKeyPress(Keys::S, ControllingPlayer(), playerIndex)) {
            session_->StartGame();
        } else if (input.IsNewKeyPress(Keys::E, ControllingPlayer(), playerIndex)) {
            session_->EndGame();
        } else if (input.IsNewKeyPress(Keys::R, ControllingPlayer(), playerIndex) && localGamer_ != nullptr) {
            session_->RemoveGamer(localGamer_, NetworkSessionEndReason::HostEndedSession);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("S: StartGame()   E: EndGame()   R: RemoveGamer(local, leave)");
        lines.emplace_back();
        lines.push_back(std::string("SessionType: ") + SessionTypeName(session_->getSessionTypeProperty()));
        lines.push_back(std::string("SessionState: ") + SessionStateName(session_->getSessionStateProperty()));
        lines.push_back("IsHost: " + std::string(session_->getIsHostProperty() ? "true" : "false"));
        lines.emplace_back();
        lines.push_back("GameStarted fired: " + std::to_string(gameStartedCount_));
        lines.push_back("GameEnded fired: " + std::to_string(gameEndedCount_));
        lines.push_back("SessionEnded fired: " + std::to_string(sessionEndedCount_) +
                         (lastEndReason_.empty() ? "" : (" (" + lastEndReason_ + ")")));
        if (localGamer_ != nullptr) {
            lines.emplace_back();
            lines.push_back("Local gamer HasLeftSession: " +
                             std::string(localGamer_->getHasLeftSessionProperty() ? "true" : "false"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    NetworkSession* session_ = nullptr;
    LocalNetworkGamer* localGamer_ = nullptr;
    int gameStartedCount_ = 0;
    int gameEndedCount_ = 0;
    int sessionEndedCount_ = 0;
    std::string lastEndReason_;
};

} // namespace CnaExamples::Demos::Net::NetworkSessionDemos
