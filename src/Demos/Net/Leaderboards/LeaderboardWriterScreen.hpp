// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardEntry.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardIdentity.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardKey.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardWriter.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "System/NotSupportedException.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Net::LeaderboardsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::GamerServices::Gamer;
using Microsoft::Xna::Framework::GamerServices::LeaderboardEntry;
using Microsoft::Xna::Framework::GamerServices::LeaderboardIdentity;
using Microsoft::Xna::Framework::GamerServices::LeaderboardKey;
using Microsoft::Xna::Framework::GamerServices::SignedInGamer;

// Demonstrates LeaderboardEntry (Columns/Gamer/Rating/RankingEXT -- a real, working value
// type once constructed) alongside Gamer::LeaderboardWriter::GetLeaderboard(), which always
// throws NotSupportedException on this platform (same stub-API-surface reasoning as
// LeaderboardReaderScreen -- there is no leaderboard backend to write to).
class LeaderboardWriterScreen : public DemoScreen {
public:
    LeaderboardWriterScreen() : DemoScreen("Leaderboards: LeaderboardWriter") {}

    void LoadContent() override {
        gamer_ = (*Gamer::getSignedInGamersProperty())[PlayerIndex::One];
        entry_ = LeaderboardEntry::CreateInternal(gamer_, 9001, 1);
        entry_.getColumnsProperty().SetValue("wave", 12);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            writeAttempts_++;
            try {
                LeaderboardIdentity identity = LeaderboardIdentity::Create(LeaderboardKey::BestScoreLifeTime);
                LeaderboardEntry* unused = gamer_->getLeaderboardWriterProperty().GetLeaderboard(identity);
                (void)unused;
                lastResult_ = "GetLeaderboard() returned without throwing (unexpected)";
            } catch (const System::NotSupportedException& ex) {
                lastResult_ = std::string("GetLeaderboard() threw NotSupportedException: ") + ex.getMessageProperty();
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: Gamer::LeaderboardWriter.GetLeaderboard()");
        lines.emplace_back();
        lines.push_back("A locally-constructed LeaderboardEntry (LeaderboardEntry::CreateInternal):");
        lines.push_back("  Gamer: " + entry_.getGamerProperty()->getGamertagProperty());
        lines.push_back("  Rating: " + std::to_string(entry_.getRatingProperty()));
        lines.push_back("  RankingEXT: " + std::to_string(entry_.getRankingEXTProperty()));
        lines.push_back("  Columns[\"wave\"]: " + std::to_string(entry_.getColumnsProperty().GetValueInt32("wave")));
        lines.emplace_back();
        lines.push_back("GetLeaderboard() attempts: " + std::to_string(writeAttempts_));
        if (!lastResult_.empty()) lines.push_back(lastResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    SignedInGamer* gamer_ = nullptr;
    LeaderboardEntry entry_ = LeaderboardEntry::CreateInternal(nullptr, 0, 0);
    int writeAttempts_ = 0;
    std::string lastResult_;
};

} // namespace CnaExamples::Demos::Net::LeaderboardsDemos
