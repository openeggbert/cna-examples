// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
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
using Microsoft::Xna::Framework::Media::Song;

// Demonstrates MediaPlayer's live-adjustable playback settings -- Volume,
// IsMuted, IsRepeating, IsShuffled -- on a looping tone. MediaPlayer is a
// static/global singleton (like TouchPanel::EnabledGestures in the Input
// area), so every setting touched here is reset to its default on exit --
// otherwise it would silently leak into whichever Song demo the user visits
// next.
class VolumeMuteRepeatShuffleScreen : public DemoScreen {
public:
    VolumeMuteRepeatShuffleScreen() : DemoScreen("Song: Volume/Mute/Repeat/Shuffle") {}

    void LoadContent() override {
        song_.emplace(kToneAWav, "Tone A (440 Hz)");
        MediaPlayer::setIsRepeatingProperty(true);
        MediaPlayer::Play(&*song_);
    }

    void UnloadContent() override {
        MediaPlayer::Stop();
        MediaPlayer::setVolumeProperty(1.0f);
        MediaPlayer::setIsMutedProperty(false);
        MediaPlayer::setIsRepeatingProperty(false);
        MediaPlayer::setIsShuffledProperty(false);
        song_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuUp(ControllingPlayer())) {
            MediaPlayer::setVolumeProperty(std::min(1.0f, MediaPlayer::getVolumeProperty() + 0.1f));
        } else if (input.IsMenuDown(ControllingPlayer())) {
            MediaPlayer::setVolumeProperty(std::max(0.0f, MediaPlayer::getVolumeProperty() - 0.1f));
        } else if (input.IsNewKeyPress(Keys::M, ControllingPlayer(), playerIndex)) {
            MediaPlayer::setIsMutedProperty(!MediaPlayer::getIsMutedProperty());
        } else if (input.IsNewKeyPress(Keys::R, ControllingPlayer(), playerIndex)) {
            MediaPlayer::setIsRepeatingProperty(!MediaPlayer::getIsRepeatingProperty());
        } else if (input.IsNewKeyPress(Keys::S, ControllingPlayer(), playerIndex)) {
            MediaPlayer::setIsShuffledProperty(!MediaPlayer::getIsShuffledProperty());
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Up/Down: Volume +-0.1   M: toggle Muted");
        lines.push_back("R: toggle Repeating   S: toggle Shuffled");
        lines.emplace_back();
        lines.push_back("Volume: " + std::to_string(MediaPlayer::getVolumeProperty()));
        lines.push_back("IsMuted: " + std::string(MediaPlayer::getIsMutedProperty() ? "true" : "false"));
        lines.push_back("IsRepeating: " + std::string(MediaPlayer::getIsRepeatingProperty() ? "true" : "false"));
        lines.push_back("IsShuffled: " + std::string(MediaPlayer::getIsShuffledProperty() ? "true" : "false"));
        lines.push_back("State: " + std::string(MediaStateName(MediaPlayer::getStateProperty())));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Song> song_;
};

} // namespace CnaExamples::Demos::Media::SongDemos
