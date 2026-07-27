// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Media/MediaDemoHelpers.hpp"

namespace CnaExamples::Demos::Media::SongDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Media::MediaPlayer;
using Microsoft::Xna::Framework::Media::MediaState;
using Microsoft::Xna::Framework::Media::Song;

// Demonstrates Song's NOXNA direct-from-file constructor and MediaPlayer's
// transport controls, on a procedurally-generated WAV tone (ffmpeg lavfi sine
// source -- no licensed audio asset needed).
//
// Real XNA's Song has no public constructor at all: the only way to get one is
// through MediaLibrary, which indexes the user's own music. CNA keeps that route
// working (see the MediaLibrary category) and adds this direct-from-file
// constructor as a NOXNA extension, so a game can ship and load its own audio
// without going through the user's library.
class LoadAndPlayScreen : public DemoScreen {
public:
    LoadAndPlayScreen() : DemoScreen("Song: Load & Play") {}

    void OnDemoLoad() override {
        song_.emplace(kToneAWav, "Tone A (440 Hz)");
    }

    void OnDemoUnload() override {
        MediaPlayer::Stop();
        song_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            switch (MediaPlayer::getStateProperty()) {
                case MediaState::Stopped: MediaPlayer::Play(&*song_); break;
                case MediaState::Playing: MediaPlayer::Pause(); break;
                case MediaState::Paused:  MediaPlayer::Resume(); break;
            }
        } else if (input.IsNewKeyPress(Keys::X, ControllingPlayer(), playerIndex)) {
            MediaPlayer::Stop();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: Stopped->Play, Playing->Pause, Paused->Resume.");
        lines.push_back("X: Stop.");
        lines.emplace_back();
        lines.push_back("Song.Name: \"" + song_->getNameProperty() + "\"");
        lines.push_back("State: " + std::string(MediaStateName(MediaPlayer::getStateProperty())));
        lines.push_back("PlayPosition: " +
                         std::to_string(MediaPlayer::getPlayPositionProperty().getTotalSecondsProperty()) + "s");
        lines.push_back("Duration: " + std::to_string(song_->getDurationProperty().getTotalSecondsProperty()) + "s");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Song> song_;
};

} // namespace CnaExamples::Demos::Media::SongDemos
