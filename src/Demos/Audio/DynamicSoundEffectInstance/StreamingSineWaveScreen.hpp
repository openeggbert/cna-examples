// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/AudioChannels.hpp"
#include "Microsoft/Xna/Framework/Audio/DynamicSoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/Audio/NoAudioHardwareException.hpp"
#include "System/EventArgs.hpp"
#include "System/Object.hpp"

#include "Demos/Audio/AudioDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::DynamicSoundEffectInstanceDemos {

using namespace CnaExamples::GameStateManagement;
using System::EventArgs;
using System::Object;
using Microsoft::Xna::Framework::Audio::AudioChannels;
using Microsoft::Xna::Framework::Audio::DynamicSoundEffectInstance;
using Microsoft::Xna::Framework::Audio::NoAudioHardwareException;

// Demonstrates DynamicSoundEffectInstance: unlike SoundEffect/
// SoundEffectInstance (a fixed, pre-generated buffer), this instance has no
// audio of its own -- the app supplies short PCM chunks on demand via
// SubmitBuffer() every time the BufferNeeded event fires, i.e. real-time
// audio synthesis. Left/Right retunes the sine wave's frequency live, heard
// immediately in the next submitted chunk.
class StreamingSineWaveScreen : public DemoScreen {
public:
    StreamingSineWaveScreen() : DemoScreen("DynamicSoundEffectInstance: Streaming") {}

    void LoadContent() override {
        try {
            instance_.emplace(44100, AudioChannels::Mono);
            bufferNeededToken_ = instance_->BufferNeeded.Add(
                [this](Object*, const EventArgs&) { SubmitNextChunk(); });
            SubmitNextChunk();
            SubmitNextChunk();
            instance_->Play();
        } catch (const NoAudioHardwareException&) {
            unavailable_ = true;
        }
    }

    void UnloadContent() override {
        if (instance_.has_value()) {
            instance_->BufferNeeded.Remove(bufferNeededToken_);
            instance_->Stop(true);
        }
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsNewKeyPress(Keys::Left, ControllingPlayer(), playerIndex) ||
            input.IsNewButtonPress(Buttons::DPadLeft, ControllingPlayer(), playerIndex)) {
            frequencyHz_ = frequencyHz_ > 60.0f ? frequencyHz_ - 40.0f : frequencyHz_;
        }
        if (input.IsNewKeyPress(Keys::Right, ControllingPlayer(), playerIndex) ||
            input.IsNewButtonPress(Buttons::DPadRight, ControllingPlayer(), playerIndex)) {
            frequencyHz_ += 40.0f;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        if (unavailable_) {
            lines.push_back("NoAudioHardwareException: no audio device on this machine.");
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Left/Right retunes the frequency live -- each new chunk uses it.");
        lines.emplace_back();
        lines.push_back("Frequency: " + std::to_string((int)frequencyHz_) + " Hz");
        lines.push_back("Chunks submitted: " + std::to_string(chunksSubmitted_));
        lines.push_back("PendingBufferCount: " + std::to_string(instance_->getPendingBufferCountProperty()));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void SubmitNextChunk() {
        instance_->SubmitBuffer(GenerateSineWavePcm16(frequencyHz_, kChunkSeconds));
        chunksSubmitted_++;
    }

    static constexpr float kChunkSeconds = 0.1f;

    std::optional<DynamicSoundEffectInstance> instance_;
    std::size_t bufferNeededToken_ = 0;
    float frequencyHz_ = 440.0f;
    int chunksSubmitted_ = 0;
    bool unavailable_ = false;
};

} // namespace CnaExamples::Demos::Audio::DynamicSoundEffectInstanceDemos
