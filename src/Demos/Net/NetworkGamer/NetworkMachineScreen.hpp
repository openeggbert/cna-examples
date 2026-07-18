// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkMachine.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "System/NotImplementedException.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Net/NetDemoHelpers.hpp"

namespace CnaExamples::Demos::Net::NetworkGamerDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Net::LocalNetworkGamer;
using Microsoft::Xna::Framework::Net::NetworkMachine;

// Demonstrates NetworkGamer::Machine (the NetworkMachine grouping gamers that share a
// physical host) and NetworkMachine::RemoveFromSession() -- which always throws
// NotImplementedException, matching FNA's own stub, not a CNA gap.
class NetworkMachineScreen : public DemoScreen {
public:
    NetworkMachineScreen() : DemoScreen("NetworkGamer: NetworkMachine") {}

    void LoadContent() override {
        session_ = NetworkSession::Create(NetworkSessionType::Local, 1, 4);
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
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            attemptCount_++;
            try {
                NetworkMachine machine = localGamer_->getMachineProperty();
                machine.RemoveFromSession();
                lastResult_ = "RemoveFromSession() returned without throwing (unexpected)";
            } catch (const System::NotImplementedException& ex) {
                lastResult_ = std::string("RemoveFromSession() threw NotImplementedException: ") + ex.getMessageProperty();
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: call NetworkMachine::RemoveFromSession()");
        lines.emplace_back();
        const NetworkMachine& machine = localGamer_->getMachineProperty();
        lines.push_back("Machine.Gamers.Count: " + std::to_string(machine.getGamersProperty().getCountProperty()));
        lines.emplace_back();
        lines.push_back("Attempts: " + std::to_string(attemptCount_));
        if (!lastResult_.empty()) lines.push_back(lastResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    NetworkSession* session_ = nullptr;
    LocalNetworkGamer* localGamer_ = nullptr;
    int attemptCount_ = 0;
    std::string lastResult_;
};

} // namespace CnaExamples::Demos::Net::NetworkGamerDemos
