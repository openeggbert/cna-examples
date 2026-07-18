// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/LeaderboardIdentity.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardKey.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardReader.hpp"
#include "System/NotSupportedException.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Net::LeaderboardsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::GamerServices::LeaderboardIdentity;
using Microsoft::Xna::Framework::GamerServices::LeaderboardKey;
using Microsoft::Xna::Framework::GamerServices::LeaderboardReader;

// Demonstrates LeaderboardIdentity (a pure value type, always constructible/inspectable) and
// LeaderboardReader::Read()/BeginRead() -- every real read entry point unconditionally throws
// NotSupportedException on this platform (leaderboards were never implemented upstream in
// FNA either; this is a stub API surface, not a CNA gap), so this screen demonstrates the
// honest failure path rather than a real leaderboard page.
class LeaderboardReaderScreen : public DemoScreen {
public:
    LeaderboardReaderScreen() : DemoScreen("Leaderboards: LeaderboardReader") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            readAttempts_++;
            try {
                LeaderboardIdentity identity = LeaderboardIdentity::Create(LeaderboardKey::BestScoreLifeTime);
                LeaderboardReader reader = LeaderboardReader::Read(identity, 0, 10);
                reader.Dispose();
                lastResult_ = "Read() returned without throwing (unexpected)";
            } catch (const System::NotSupportedException& ex) {
                lastResult_ = std::string("Read() threw NotSupportedException: ") + ex.getMessageProperty();
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: LeaderboardReader::Read(BestScoreLifeTime, 0, 10)");
        lines.emplace_back();
        LeaderboardIdentity identity = LeaderboardIdentity::Create(LeaderboardKey::BestScoreLifeTime, 2);
        lines.push_back("LeaderboardIdentity.Key: \"" + identity.getKeyProperty() + "\"");
        lines.push_back("LeaderboardIdentity.GameMode: " + std::to_string(identity.getGameModeProperty()));
        lines.emplace_back();
        lines.push_back("Read attempts: " + std::to_string(readAttempts_));
        if (!lastResult_.empty()) lines.push_back(lastResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int readAttempts_ = 0;
    std::string lastResult_;
};

} // namespace CnaExamples::Demos::Net::LeaderboardsDemos
