// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPresence.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPrivileges.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerProfile.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Net::GamerServicesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::GamerServices::Gamer;
using Microsoft::Xna::Framework::GamerServices::GamerPresenceMode;
using Microsoft::Xna::Framework::GamerServices::GamerProfile;
using Microsoft::Xna::Framework::GamerServices::SignedInGamer;

// Demonstrates SignedInGamer::GetProfile()/Presence/Privileges on the first stub signed-in
// gamer -- all populated with plausible-but-synthetic defaults (there is no real profile
// backend), but the getter/setter surface itself is real and functional.
class ProfilePresencePrivilegesScreen : public DemoScreen {
public:
    ProfilePresencePrivilegesScreen() : DemoScreen("GamerServices: Profile/Presence/Privileges") {}

    void LoadContent() override {
        gamer_ = (*Gamer::getSignedInGamersProperty())[PlayerIndex::One];
    }

    void UnloadContent() override {
        if (profile_ != nullptr) {
            profile_->Dispose();
            delete profile_;
            profile_ = nullptr;
        }
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            if (profile_ != nullptr) { profile_->Dispose(); delete profile_; }
            profile_ = gamer_->GetProfile();
            fetchCount_++;
        } else if (input.IsNewKeyPress(Keys::P, ControllingPlayer(), playerIndex)) {
            auto& presence = gamer_->getPresenceProperty();
            const int next = (static_cast<int>(presence.getPresenceModeProperty()) + 1) % 60;
            presence.setPresenceModeProperty(static_cast<GamerPresenceMode>(next));
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: GetProfile()   P: cycle Presence.PresenceMode");
        lines.emplace_back();

        if (profile_ != nullptr) {
            lines.push_back("GamerScore: " + std::to_string(profile_->getGamerScoreProperty()) +
                             "  Reputation: " + std::to_string(profile_->getReputationProperty()));
            lines.push_back("Motto: \"" + profile_->getMottoProperty() + "\"");
            lines.push_back("Region: " + profile_->getRegionProperty().getEnglishNameProperty());
            lines.push_back("TitlesPlayed: " + std::to_string(profile_->getTitlesPlayedProperty()) +
                             "  TotalAchievements: " + std::to_string(profile_->getTotalAchievementsProperty()));
        } else {
            lines.push_back("(no profile fetched yet)");
        }
        lines.push_back("GetProfile() calls: " + std::to_string(fetchCount_));
        lines.emplace_back();

        const auto& presence = gamer_->getPresenceProperty();
        lines.push_back("Presence.PresenceMode: " + std::to_string((int)presence.getPresenceModeProperty()));
        lines.push_back("Presence.PresenceValue: " + std::to_string(presence.getPresenceValueProperty()));
        lines.emplace_back();

        const auto& priv = gamer_->getPrivilegesProperty();
        lines.push_back("AllowOnlineSessions: " + std::string(priv.getAllowOnlineSessionsProperty() ? "true" : "false") +
                         "  AllowPurchaseContent: " + std::string(priv.getAllowPurchaseContentProperty() ? "true" : "false"));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    SignedInGamer* gamer_ = nullptr;
    GamerProfile* profile_ = nullptr;
    int fetchCount_ = 0;
};

} // namespace CnaExamples::Demos::Net::GamerServicesDemos
