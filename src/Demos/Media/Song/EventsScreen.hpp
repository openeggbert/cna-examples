// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "Microsoft/Xna/Framework/Media/SongCollection.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Media/MediaDemoHelpers.hpp"

namespace CnaExamples::Demos::Media::SongDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Media::MediaPlayer;
using Microsoft::Xna::Framework::Media::Song;
using Microsoft::Xna::Framework::Media::SongCollection;

// Demonstrates MediaPlayer::ActiveSongChanged/MediaStateChanged -- real
// events, deferred one frame through FrameworkDispatcher (Game::Update()
// pumps this automatically every frame, no manual wiring needed here).
// MediaPlayer is a static singleton, so subscriptions are removed in
// UnloadContent() via the returned tokens, same discipline as
// VolumeMuteRepeatShuffleScreen resetting its settings.
class EventsScreen : public DemoScreen {
public:
    EventsScreen() : DemoScreen("Song: MediaPlayer Events") {}

    void LoadContent() override {
        songA_.emplace(kToneAWav, "Tone A (440 Hz)");
        songB_.emplace(kToneBWav, "Tone B (554 Hz)");
        activeSongToken_ = MediaPlayer::ActiveSongChanged.Add(
            [this](System::Object*, const System::EventArgs&) { activeSongChangedCount_++; });
        stateToken_ = MediaPlayer::MediaStateChanged.Add(
            [this](System::Object*, const System::EventArgs&) { mediaStateChangedCount_++; });
    }

    void UnloadContent() override {
        MediaPlayer::ActiveSongChanged.Remove(activeSongToken_);
        MediaPlayer::MediaStateChanged.Remove(stateToken_);
        MediaPlayer::Stop();
        songA_.reset();
        songB_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            SongCollection collection({&*songA_, &*songB_});
            MediaPlayer::Play(collection, 0);
        } else if (input.IsNewKeyPress(Keys::N, ControllingPlayer(), playerIndex)) {
            MediaPlayer::MoveNext();
        } else if (input.IsNewKeyPress(Keys::X, ControllingPlayer(), playerIndex)) {
            MediaPlayer::Stop();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: Play the 2-song queue   N: MoveNext()   X: Stop()");
        lines.emplace_back();
        lines.push_back("State: " + std::string(MediaStateName(MediaPlayer::getStateProperty())));
        lines.push_back("ActiveSongChanged fired: " + std::to_string(activeSongChangedCount_));
        lines.push_back("MediaStateChanged fired: " + std::to_string(mediaStateChangedCount_));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Song> songA_;
    std::optional<Song> songB_;
    System::EventHandler<System::EventArgs>::Token activeSongToken_{};
    System::EventHandler<System::EventArgs>::Token stateToken_{};
    int activeSongChangedCount_ = 0;
    int mediaStateChangedCount_ = 0;
};

} // namespace CnaExamples::Demos::Media::SongDemos
