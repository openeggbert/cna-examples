// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "Microsoft/Xna/Framework/Media/VisualizationData.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Media/MediaDemoHelpers.hpp"

namespace CnaExamples::Demos::Media::SongDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Media::MediaPlayer;
using Microsoft::Xna::Framework::Media::Song;
using Microsoft::Xna::Framework::Media::VisualizationData;

// Demonstrates MediaPlayer::IsVisualizationEnabled/GetVisualizationData() --
// a confirmed, permanent stub on this platform (SDL3_mixer exposes no
// frequency/sample visualization data at all): the setter is a real no-op
// (the getter always reports false regardless), and GetVisualizationData()
// never writes into the buffer passed to it. This screen calls it every
// frame during real playback and sums both arrays live to prove they stay
// exactly zero, rather than just asserting it in a comment.
class VisualizationScreen : public DemoScreen {
public:
    VisualizationScreen() : DemoScreen("Song: Visualization") {}

    void LoadContent() override {
        song_.emplace(kToneAWav, "Tone A (440 Hz)");
        MediaPlayer::setIsRepeatingProperty(true);
        MediaPlayer::Play(&*song_);
        MediaPlayer::setIsVisualizationEnabledProperty(true);
    }

    void UnloadContent() override {
        MediaPlayer::Stop();
        MediaPlayer::setIsRepeatingProperty(false);
        MediaPlayer::setIsVisualizationEnabledProperty(false);
        song_.reset();
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        VisualizationData data;
        MediaPlayer::GetVisualizationData(data);
        float sum = 0.0f;
        for (float f : data.getFrequenciesProperty()) sum += f;
        for (float s : data.getSamplesProperty()) sum += s;
        lastSum_ = sum;
        sampleCount_++;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Real playback is running; GetVisualizationData() is polled every frame.");
        lines.emplace_back();
        lines.push_back("setIsVisualizationEnabledProperty(true) was called on entry.");
        lines.push_back("IsVisualizationEnabled: " +
                         std::string(MediaPlayer::getIsVisualizationEnabledProperty() ? "true" : "false") +
                         " (setter is a real no-op -- always false here)");
        lines.push_back("VisualizationData::Size: " + std::to_string(VisualizationData::Size) + " floats/array");
        lines.push_back("Frames sampled: " + std::to_string(sampleCount_));
        lines.push_back("Sum of freq[]+samp[] this frame: " + std::to_string(lastSum_) + " (always 0)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Song> song_;
    float lastSum_ = 0.0f;
    int sampleCount_ = 0;
};

} // namespace CnaExamples::Demos::Media::SongDemos
