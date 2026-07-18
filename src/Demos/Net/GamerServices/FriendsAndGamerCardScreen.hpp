// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/FriendCollection.hpp"
#include "Microsoft/Xna/Framework/GamerServices/FriendGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Net::GamerServicesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::GamerServices::FriendGamer;
using Microsoft::Xna::Framework::GamerServices::Gamer;
using Microsoft::Xna::Framework::GamerServices::Guide;
using Microsoft::Xna::Framework::GamerServices::SignedInGamer;

// Demonstrates SignedInGamer::GetFriends() (always a real, empty FriendCollection -- there is
// no real friends backend) alongside a manually-built demo roster via FriendGamer::
// CreateInternal(), the only way to see FriendGamer's IsOnline/IsPlaying/IsAway/Presence
// fields populated on this platform. Also exercises Guide::ShowFriends()/ShowGamerCard(),
// both real, silent no-ops.
class FriendsAndGamerCardScreen : public DemoScreen {
public:
    FriendsAndGamerCardScreen() : DemoScreen("GamerServices: Friends & GamerCard") {}

    void LoadContent() override {
        gamer_ = (*Gamer::getSignedInGamersProperty())[PlayerIndex::One];
        demoRoster_.push_back(FriendGamer::CreateInternal("AlphaWolf99", "Alpha", true, true, false, false, false, false));
        demoRoster_.push_back(FriendGamer::CreateInternal("NightOwl", "Night Owl", true, false, true, false, false, false));
        demoRoster_.push_back(FriendGamer::CreateInternal("QuietStorm", "Quiet Storm", false, false, false, false, false, false));
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            auto friends = gamer_->GetFriends();
            realFriendsCount_ = friends.getCountProperty();
            friends.Dispose();
        } else if (input.IsNewKeyPress(Keys::F, ControllingPlayer(), playerIndex)) {
            Guide::ShowFriends(PlayerIndex::One);
            showFriendsCount_++;
        } else if (input.IsNewKeyPress(Keys::C, ControllingPlayer(), playerIndex)) {
            Guide::ShowGamerCard(PlayerIndex::One, gamer_);
            showGamerCardCount_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: GetFriends()   F: ShowFriends()   C: ShowGamerCard()");
        lines.push_back("Real GetFriends().Count: " + std::to_string(realFriendsCount_) + " (always empty)");
        lines.push_back("ShowFriends() calls: " + std::to_string(showFriendsCount_) +
                         "   ShowGamerCard() calls: " + std::to_string(showGamerCardCount_));
        lines.push_back("(both are real, silent no-ops)");
        lines.emplace_back();
        lines.push_back("Manually-built demo roster (FriendGamer::CreateInternal, not real friends):");
        for (const auto& f : demoRoster_) {
            lines.push_back("  " + f.getGamertagProperty() + "  Online=" + (f.getIsOnlineProperty() ? "true" : "false") +
                             "  Playing=" + (f.getIsPlayingProperty() ? "true" : "false") +
                             "  Away=" + (f.getIsAwayProperty() ? "true" : "false"));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    SignedInGamer* gamer_ = nullptr;
    std::vector<FriendGamer> demoRoster_;
    int realFriendsCount_ = 0;
    int showFriendsCount_ = 0;
    int showGamerCardCount_ = 0;
};

} // namespace CnaExamples::Demos::Net::GamerServicesDemos
