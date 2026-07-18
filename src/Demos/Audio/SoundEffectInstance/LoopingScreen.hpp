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

// Demonstrates IsLooped: a short (0.4s) tone that either plays once and
// stops (State -> Stopped) or repeats indefinitely (State stays Playing),
// toggled live -- IsLooped must be set before Play() to take effect for
// that playback (matching FNA), so toggling it restarts the instance.
class LoopingScreen : public DemoScreen {
public:
    LoopingScreen() : DemoScreen("SoundEffectInstance: Looping") {}

    void LoadContent() override {
        try {
            effect_.emplace(GenerateSineWavePcm16(523.0f, 0.4f), 44100, AudioChannels::Mono);
            instance_ = effect_->CreateInstance();
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
            looped_ = !looped_;
            instance_->Stop(true);
            instance_->setIsLoopedProperty(looped_);
            instance_->Play();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        if (unavailable_) {
            lines.push_back("NoAudioHardwareException: no audio device on this machine.");
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Space/Enter/A/tap toggles IsLooped and restarts the tone.");
        lines.emplace_back();
        lines.push_back(std::string("IsLooped: ") + (looped_ ? "true" : "false"));
        lines.push_back(std::string("State: ") +
                        (instance_->getStateProperty() == SoundState::Playing ? "Playing" :
                         instance_->getStateProperty() == SoundState::Paused ? "Paused" : "Stopped"));
        lines.push_back(looped_ ? "(will repeat until you toggle again)"
                                : "(plays once, then State becomes Stopped on its own)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<SoundEffect> effect_;
    std::optional<SoundEffectInstance> instance_;
    bool looped_ = false;
    bool unavailable_ = false;
};

} // namespace CnaExamples::Demos::Audio::SoundEffectInstanceDemos
