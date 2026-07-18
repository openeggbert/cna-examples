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

// Demonstrates Song's NOXNA direct-from-file constructor (real XNA's Song has
// no public constructor at all -- only obtainable via MediaLibrary, which is
// a stub on this platform, see the MediaLibrary category) and MediaPlayer's
// transport controls, on a procedurally-generated WAV tone (ffmpeg lavfi
// sine source -- no licensed audio asset needed).
class LoadAndPlayScreen : public DemoScreen {
public:
    LoadAndPlayScreen() : DemoScreen("Song: Load & Play") {}

    void LoadContent() override {
        song_.emplace(kToneAWav, "Tone A (440 Hz)");
    }

    void UnloadContent() override {
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
