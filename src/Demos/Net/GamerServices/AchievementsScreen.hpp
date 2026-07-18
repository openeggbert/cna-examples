// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/Achievement.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AchievementCollection.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "System/DateTime.hpp"
#include "System/NotImplementedException.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Net::GamerServicesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::GamerServices::Achievement;
using Microsoft::Xna::Framework::GamerServices::Gamer;
using Microsoft::Xna::Framework::GamerServices::SignedInGamer;

// Demonstrates SignedInGamer::AwardAchievement()/GetAchievements() -- both real, no-op-but-
// functional stubs (no persistence backend, so GetAchievements() always returns an empty
// collection even right after awarding one), plus Achievement::GetPicture(), which always
// throws NotImplementedException (a genuine platform-unavailability limitation carried over
// from FNA, not something this port could implement -- real Xbox achievement art was streamed
// from Xbox Live).
class AchievementsScreen : public DemoScreen {
public:
    AchievementsScreen() : DemoScreen("GamerServices: Achievements") {}

    void LoadContent() override {
        gamer_ = (*Gamer::getSignedInGamersProperty())[PlayerIndex::One];
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            gamer_->AwardAchievement("demo-achievement");
            awardCount_++;
            auto achievements = gamer_->GetAchievements();
            achievementsCount_ = achievements.getCountProperty();
            achievements.Dispose();
        } else if (input.IsNewKeyPress(Keys::G, ControllingPlayer(), playerIndex)) {
            Achievement demo = Achievement::CreateInternal(
                "demo-achievement", "Demo Achievement", "Awarded from this screen",
                true, true, System::DateTime::getNowProperty());
            try {
                demo.GetPicture();
                lastPictureResult_ = "GetPicture() returned without throwing (unexpected)";
            } catch (const System::NotImplementedException& ex) {
                lastPictureResult_ = std::string("GetPicture() threw NotImplementedException: ") + ex.getMessageProperty();
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: AwardAchievement() + GetAchievements()");
        lines.push_back("G: construct a demo Achievement and call GetPicture()");
        lines.emplace_back();
        lines.push_back("AwardAchievement() calls: " + std::to_string(awardCount_));
        lines.push_back("GetAchievements().Count (always empty, no persistence backend): " +
                         std::to_string(achievementsCount_));
        lines.emplace_back();
        if (!lastPictureResult_.empty()) lines.push_back(lastPictureResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    SignedInGamer* gamer_ = nullptr;
    int awardCount_ = 0;
    int achievementsCount_ = 0;
    std::string lastPictureResult_;
};

} // namespace CnaExamples::Demos::Net::GamerServicesDemos
