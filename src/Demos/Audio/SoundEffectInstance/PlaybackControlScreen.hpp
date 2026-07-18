// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/AudioChannels.hpp"
#include "Microsoft/Xna/Framework/Audio/NoAudioHardwareException.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundState.hpp"

#include "Demos/Audio/AudioDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::SoundEffectInstanceDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Audio::AudioChannels;
using Microsoft::Xna::Framework::Audio::NoAudioHardwareException;
using Microsoft::Xna::Framework::Audio::SoundEffect;
using Microsoft::Xna::Framework::Audio::SoundEffectInstance;
using Microsoft::Xna::Framework::Audio::SoundState;

// Demonstrates SoundEffectInstance's transport controls (Play/Pause/Resume/
// Stop) and its State property -- unlike SoundEffect::Play() (fire-and-
// forget, see the SoundEffect category), an instance is a long-lived object
// a game holds onto specifically so it CAN query/control an in-progress
// sound. Built on a 4-second looping tone so there's time to try each
// control before it would finish on its own.
class PlaybackControlScreen : public DemoScreen {
public:
    PlaybackControlScreen() : DemoScreen("SoundEffectInstance: Playback Control") {}

    void LoadContent() override {
        try {
            effect_.emplace(GenerateSineWavePcm16(330.0f, 4.0f), 44100, AudioChannels::Mono);
            instance_ = effect_->CreateInstance();
            instance_->setIsLoopedProperty(true);
        } catch (const NoAudioHardwareException&) {
            unavailable_ = true;
        }
    }

    void UnloadContent() override {
        if (instance_.has_value()) instance_->Stop(true);
    }

protected:
    void OnDemoInput(InputState& input) override {
        if (!instance_.has_value()) return;
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            switch (instance_->getStateProperty()) {
                case SoundState::Stopped: instance_->Play(); break;
                case SoundState::Playing: instance_->Pause(); break;
                case SoundState::Paused: instance_->Resume(); break;
            }
        } else if (input.IsNewButtonPress(Buttons::X, ControllingPlayer(), playerIndex)) {
            instance_->Stop();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        if (unavailable_) {
            lines.push_back("NoAudioHardwareException: no audio device on this machine.");
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Space/Enter/A/tap: Stopped->Play, Playing->Pause, Paused->Resume.");
        lines.push_back("Gamepad X: Stop.");
        lines.emplace_back();
        lines.push_back(std::string("State: ") + StateName(instance_->getStateProperty()));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static const char* StateName(SoundState state) {
        switch (state) {
            case SoundState::Playing: return "Playing";
            case SoundState::Paused: return "Paused";
            default: return "Stopped";
        }
    }

    std::optional<SoundEffect> effect_;
    std::optional<SoundEffectInstance> instance_;
    bool unavailable_ = false;
};

} // namespace CnaExamples::Demos::Audio::SoundEffectInstanceDemos
