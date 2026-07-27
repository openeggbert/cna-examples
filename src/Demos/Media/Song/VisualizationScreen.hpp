// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <cmath>
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

// MediaPlayer::IsVisualizationEnabled / GetVisualizationData() are real on this
// platform: enabling visualization installs a post-mix tap on the audio mixer,
// and GetVisualizationData() fills both arrays -- 256 raw samples and 256 FFT
// frequency bins -- from whatever is actually being mixed. (This used to be a
// stub whose buffers stayed zero; that changed when CNA's feature/media branch
// merged on 2026-07-18.)
//
// The screen plays a single 440 Hz sine on loop and draws both arrays live, so
// the data is visibly real: a pure tone produces one dominant frequency bin and
// a clean periodic waveform, not noise.
//
// Two honest details it also shows: the flag only reports true once the tap is
// genuinely installed (a failed install leaves it false rather than lying), and
// with visualization switched off the arrays go back to being all zero.
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
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            MediaPlayer::setIsVisualizationEnabledProperty(
                !MediaPlayer::getIsVisualizationEnabledProperty());
        }
    }

    void OnDemoUpdate(GameTime&) override {
        MediaPlayer::GetVisualizationData(data_);

        freqSum_ = 0.0f;
        sampSum_ = 0.0f;
        peakBin_ = 0;
        peakValue_ = 0.0f;

        const auto& freq = data_.getFrequenciesProperty();
        const auto& samp = data_.getSamplesProperty();
        for (int i = 0; i < (int)VisualizationData::Size; ++i) {
            freqSum_ += std::fabs(freq[(std::size_t)i]);
            sampSum_ += std::fabs(samp[(std::size_t)i]);
            if (freq[(std::size_t)i] > peakValue_) {
                peakValue_ = freq[(std::size_t)i];
                peakBin_ = i;
            }
        }
        frames_++;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const bool enabled = MediaPlayer::getIsVisualizationEnabledProperty();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: toggle IsVisualizationEnabled");
        lines.emplace_back();
        lines.push_back("IsVisualizationEnabled " + std::string(enabled ? "true" : "false") +
                        "    MediaPlayer " + MediaStateName(MediaPlayer::getStateProperty()) +
                        "    " + std::to_string((int)VisualizationData::Size) + " floats/array");
        lines.push_back("Frames sampled " + std::to_string(frames_) +
                        "    Sum |freq| " + std::to_string(freqSum_) +
                        "    Sum |samp| " + std::to_string(sampSum_));
        lines.push_back("Peak bin " + std::to_string(peakBin_) +
                        " (value " + std::to_string(peakValue_) + ")");
        lines.emplace_back();
        lines.push_back(enabled
            ? "A 440 Hz sine loops: one bin dominates, the waveform is periodic."
            : "Off: the tap is uninstalled and both arrays read back as zero.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        // Fixed geometry rather than "wherever the text ended": the graphs are
        // the point of this screen, so they get a reserved band that a longer
        // readout can never push off the bottom of the window.
        DrawSpectrum(sb, font);
    }

private:
    void DrawSpectrum(SpriteBatch& sb, SpriteFont& font) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const int left = 40;
        const int barCount = (int)VisualizationData::Size;
        const int barWidth = std::max(1, (viewport.getWidthProperty() - 2 * left) / barCount);
        const int plotWidth = barCount * barWidth;
        const float alpha = TransitionAlpha();

        // Both graphs sit in the band between the text block and the Back hint.
        const int bottom = viewport.getHeightProperty() - 56;
        const int graphHeight = 56;
        const int waveCentre = bottom - graphHeight / 2;
        const int freqBaseline = waveCentre - graphHeight / 2 - 34;
        const int freqTop = freqBaseline - graphHeight;

        const auto& freq = data_.getFrequenciesProperty();
        const auto& samp = data_.getSamplesProperty();

        // Frequency bins: normalised against this frame's own peak so a quiet
        // signal is still readable; the raw peak value is printed above.
        const float scale = peakValue_ > 0.0001f ? 1.0f / peakValue_ : 0.0f;
        sb.DrawString(font, "Frequencies (FFT bins, normalised to this frame's peak)",
                      Vector2((float)left, (float)freqTop - 34.0f), mul(Color(150, 150, 150), alpha));
        FillRect(sb, Rectangle(left, freqBaseline, plotWidth, 1), mul(Color(70, 70, 70), alpha));
        for (int i = 0; i < barCount; ++i) {
            const int h = (int)(std::clamp(freq[(std::size_t)i] * scale, 0.0f, 1.0f) * graphHeight);
            if (h <= 0) continue;
            FillRect(sb, Rectangle(left + i * barWidth, freqBaseline - h, barWidth, h),
                     mul(Color(90, 200, 120), alpha));
        }

        // Raw samples: signed, so they are drawn around a centre line.
        sb.DrawString(font, "Samples (waveform)",
                      Vector2((float)left, (float)(waveCentre - graphHeight / 2) - 26.0f),
                      mul(Color(150, 150, 150), alpha));
        FillRect(sb, Rectangle(left, waveCentre, plotWidth, 1), mul(Color(70, 70, 70), alpha));
        for (int i = 0; i < barCount; ++i) {
            const int h = (int)(std::clamp(samp[(std::size_t)i], -1.0f, 1.0f) * (graphHeight / 2));
            if (h == 0) continue;
            const int y = h > 0 ? waveCentre - h : waveCentre;
            FillRect(sb, Rectangle(left + i * barWidth, y, barWidth, std::abs(h)),
                     mul(Color(120, 160, 220), alpha));
        }
    }

    std::optional<Song> song_;
    VisualizationData data_;
    float freqSum_ = 0.0f;
    float sampSum_ = 0.0f;
    float peakValue_ = 0.0f;
    int peakBin_ = 0;
    int frames_ = 0;
};

} // namespace CnaExamples::Demos::Media::SongDemos
