// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/AudioChannels.hpp"
#include "Microsoft/Xna/Framework/Audio/NoAudioHardwareException.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"

#include "Demos/Audio/AudioDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::SoundEffectDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Audio::AudioChannels;
using Microsoft::Xna::Framework::Audio::NoAudioHardwareException;
using Microsoft::Xna::Framework::Audio::SoundEffect;

// Demonstrates the simplest SoundEffect path: a raw 16-bit PCM buffer (a
// generated sine wave -- this app ships no WAV asset, see
// AudioDemoHelpers.hpp) wrapped in a SoundEffect and played with Play().
// Space/Enter/A/tap plays a fresh one-shot tone each time; SoundEffect
// itself has no "is this playing" query (that's SoundEffectInstance's
// job -- see the next category), so this demo is fire-and-forget by design,
// matching the real API shape.
class PlayToneScreen : public DemoScreen {
public:
    PlayToneScreen() : DemoScreen("SoundEffect: Play a Tone") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            PlayTone();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap plays a fresh 440Hz tone (a generated sine wave,");
        lines.push_back("no WAV asset needed -- see AudioDemoHelpers.hpp).");
        lines.emplace_back();
        lines.push_back("Plays triggered this screen: " + std::to_string(playCount_));
        if (!lastResult_.empty()) lines.push_back(lastResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void PlayTone() {
        try {
            // A SoundEffect must stay alive for as long as it might still be playing (a real
            // game keeps loaded SoundEffects around as long-lived ContentManager-owned assets,
            // not locals) -- kept here as a member, overwritten on the next Play, rather than
            // let it go out of scope immediately after Play() returns.
            lastTone_.emplace(GenerateSineWavePcm16(440.0f, 0.5f), 44100, AudioChannels::Mono);
            const bool started = lastTone_->Play();
            playCount_++;
            lastResult_ = "Last Play() result: " + std::string(started ? "true" : "false") +
                         "; Duration: " +
                         std::to_string(lastTone_->getDurationProperty().getTotalSecondsProperty()) + "s";
        } catch (const NoAudioHardwareException&) {
            lastResult_ = "NoAudioHardwareException: no audio device on this machine.";
        }
    }

    int playCount_ = 0;
    std::string lastResult_;
    std::optional<SoundEffect> lastTone_;
};

} // namespace CnaExamples::Demos::Audio::SoundEffectDemos
