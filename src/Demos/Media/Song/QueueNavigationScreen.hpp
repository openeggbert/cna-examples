// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "Microsoft/Xna/Framework/Media/SongCollection.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Media/MediaDemoHelpers.hpp"

namespace CnaExamples::Demos::Media::SongDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Media::MediaPlayer;
using Microsoft::Xna::Framework::Media::Song;
using Microsoft::Xna::Framework::Media::SongCollection;

// Demonstrates MediaPlayer::Play(SongCollection, index) + MoveNext()/
// MovePrevious() over 3 procedurally-generated tones at different
// frequencies, and MediaPlayer::Queue's ActiveSong/ActiveSongIndex.
class QueueNavigationScreen : public DemoScreen {
public:
    QueueNavigationScreen() : DemoScreen("Song: Queue Navigation") {}

    void LoadContent() override {
        songA_.emplace(kToneAWav, "Tone A (440 Hz)");
        songB_.emplace(kToneBWav, "Tone B (554 Hz)");
        songC_.emplace(kToneCWav, "Tone C (659 Hz)");
        SongCollection collection({&*songA_, &*songB_, &*songC_});
        MediaPlayer::Play(collection, 0);
    }

    void UnloadContent() override {
        MediaPlayer::Stop();
        songA_.reset();
        songB_.reset();
        songC_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsNewKeyPress(Keys::N, ControllingPlayer(), playerIndex)) {
            MediaPlayer::MoveNext();
        } else if (input.IsNewKeyPress(Keys::P, ControllingPlayer(), playerIndex)) {
            MediaPlayer::MovePrevious();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("N: MoveNext()   P: MovePrevious()");
        lines.emplace_back();
        const auto& queue = MediaPlayer::getQueueProperty();
        lines.push_back("Queue.Count: " + std::to_string(queue.getCountProperty()));
        lines.push_back("Queue.ActiveSongIndex: " + std::to_string(queue.getActiveSongIndexProperty()));
        Song* active = queue.getActiveSongProperty();
        lines.push_back("Queue.ActiveSong: " + (active != nullptr ? active->getNameProperty() : std::string("(none)")));
        lines.push_back("State: " + std::string(MediaStateName(MediaPlayer::getStateProperty())));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Song> songA_;
    std::optional<Song> songB_;
    std::optional<Song> songC_;
};

} // namespace CnaExamples::Demos::Media::SongDemos
