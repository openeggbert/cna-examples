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
using Microsoft::Xna::Framework::Media::Song;

// Demonstrates a real, honest gap: CNA vendors SDL3_mixer with Opus and
// WavPack decoding fully compiled out (no built-in fallback decoder exists
// for either, unlike MP3/OGG/FLAC, which fall back to bundled dr_mp3/
// stb_vorbis/dr_flac decoders even with the external codec libraries
// disabled). Song's constructor only checks that the file exists -- it has
// no format-sniffing of its own -- so constructing a Song from a real,
// validly-encoded .opus file succeeds. MediaPlayer::Play() then silently
// stays Stopped: SDL3_mixer's MIX_LoadAudio returns null for a format it
// can't decode, and CNA's MediaPlayer treats that as a no-op rather than
// throwing.
class UnsupportedFormatScreen : public DemoScreen {
public:
    UnsupportedFormatScreen() : DemoScreen("Song: Unsupported Format") {}

    void LoadContent() override {
        song_.emplace(kUnsupportedOpus, "Unsupported (real Opus file)");
    }

    void UnloadContent() override {
        MediaPlayer::Stop();
        song_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            MediaPlayer::Play(&*song_);
            playAttempts_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: MediaPlayer::Play() a real, valid .opus file");
        lines.emplace_back();
        lines.push_back("Song construction succeeded (only checks the file exists).");
        lines.push_back("Play() attempts: " + std::to_string(playAttempts_));
        lines.push_back("State: " + std::string(MediaStateName(MediaPlayer::getStateProperty())));
        lines.push_back("(stays Stopped -- Opus decoding is compiled out, not a crash/throw)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Song> song_;
    int playAttempts_ = 0;
};

} // namespace CnaExamples::Demos::Media::SongDemos
