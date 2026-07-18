// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Net/AvailableNetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSessionCollection.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionProperties.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Net/NetDemoHelpers.hpp"

namespace CnaExamples::Demos::Net::NetworkSessionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Net::AvailableNetworkSession;
using Microsoft::Xna::Framework::Net::NetworkSessionProperties;

// Demonstrates NetworkSession::Find()/Join() -- a real, blocking (~150ms) UDP LAN
// broadcast/unicast discovery, not a stub (FNA's own Find() never discovers anything real;
// this is one of CNA's own extensions on top). Requires a second cna_examples instance
// running the SystemLink Host screen to actually find anything on this desktop; with none
// running, Find() still genuinely executes real discovery I/O and correctly returns zero
// results.
class DiscoverAndJoinScreen : public DemoScreen {
public:
    DiscoverAndJoinScreen() : DemoScreen("NetworkSession: Discover & Join") {}

    void UnloadContent() override {
        EndSession(joinedSession_);
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        if (joinedSession_ != nullptr) {
            joinedSession_->Update();
        }
    }

    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsNewKeyPress(Keys::F, ControllingPlayer(), playerIndex)) {
            auto found = NetworkSession::Find(NetworkSessionType::SystemLink, 1, NetworkSessionProperties{});
            discovered_.assign(found.begin(), found.end());
            selectedIndex_ = 0;
            searchCount_++;
        } else if (input.IsMenuUp(ControllingPlayer()) && !discovered_.empty()) {
            selectedIndex_ = (selectedIndex_ + (int)discovered_.size() - 1) % (int)discovered_.size();
        } else if (input.IsMenuDown(ControllingPlayer()) && !discovered_.empty()) {
            selectedIndex_ = (selectedIndex_ + 1) % (int)discovered_.size();
        } else if (input.IsMenuSelect(ControllingPlayer(), playerIndex) &&
                   selectedIndex_ >= 0 && selectedIndex_ < (int)discovered_.size()) {
            EndSession(joinedSession_);
            joinedSession_ = NetworkSession::Join(&discovered_[(std::size_t)selectedIndex_]);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("F: Find()   Up/Down: select   Space/Enter/A/tap: Join() selected");
        lines.push_back("Searches: " + std::to_string(searchCount_) +
                         "   Found: " + std::to_string(discovered_.size()));
        lines.emplace_back();

        for (std::size_t i = 0; i < discovered_.size() && i < 5; ++i) {
            const AvailableNetworkSession& s = discovered_[i];
            const std::string marker = ((int)i == selectedIndex_) ? "> " : "  ";
            lines.push_back(marker + s.getHostGamertagProperty() + " @ " + s.GetConnectAddress() + ":" +
                             std::to_string(s.GetConnectPort()) +
                             "  gamers=" + std::to_string(s.getCurrentGamerCountProperty()) +
                             "  RTT=" + std::to_string((int)s.getQualityOfServiceProperty()
                                                            .getAverageRoundtripTimeProperty()
                                                            .getTotalMillisecondsProperty()) + "ms");
        }

        if (joinedSession_ != nullptr) {
            lines.emplace_back();
            lines.push_back("Joined! IsHost: " + std::string(joinedSession_->getIsHostProperty() ? "true" : "false") +
                             "   SessionState: " + SessionStateName(joinedSession_->getSessionStateProperty()));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::vector<AvailableNetworkSession> discovered_;
    int selectedIndex_ = -1;
    int searchCount_ = 0;
    NetworkSession* joinedSession_ = nullptr;
};

} // namespace CnaExamples::Demos::Net::NetworkSessionDemos
