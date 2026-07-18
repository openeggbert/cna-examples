// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/AudioChannels.hpp"
#include "Microsoft/Xna/Framework/Audio/NoAudioHardwareException.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"

#include "Demos/Audio/AudioDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::SoundEffectInstanceDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Audio::AudioChannels;
using Microsoft::Xna::Framework::Audio::NoAudioHardwareException;
using Microsoft::Xna::Framework::Audio::SoundEffect;
using Microsoft::Xna::Framework::Audio::SoundEffectInstance;

// Demonstrates SoundEffectInstance's Volume/Pitch/Pan properties on a
// continuously looping tone: Up/Down adjusts Volume, Left/Right adjusts
// Pan, gamepad shoulders adjust Pitch -- all audible live while the
// instance keeps playing, unlike SoundEffect::Play(volume, pitch, pan)
// (the SoundEffect category) which only takes a one-shot snapshot at the
// moment it starts.
class VolumePitchPanScreen : public DemoScreen {
public:
    VolumePitchPanScreen() : DemoScreen("SoundEffectInstance: Volume/Pitch/Pan") {}

    void LoadContent() override {
        try {
            effect_.emplace(GenerateSineWavePcm16(220.0f, 4.0f), 44100, AudioChannels::Mono);
            instance_ = effect_->CreateInstance();
            instance_->setIsLoopedProperty(true);
            instance_->Play();
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
        if (input.IsNewKeyPress(Keys::Up, ControllingPlayer(), playerIndex) ||
            input.IsNewButtonPress(Buttons::DPadUp, ControllingPlayer(), playerIndex)) {
            instance_->setVolumeProperty(Clamp01(instance_->getVolumeProperty() + 0.1f));
        }
        if (input.IsNewKeyPress(Keys::Down, ControllingPlayer(), playerIndex) ||
            input.IsNewButtonPress(Buttons::DPadDown, ControllingPlayer(), playerIndex)) {
            instance_->setVolumeProperty(Clamp01(instance_->getVolumeProperty() - 0.1f));
        }
        if (input.IsNewKeyPress(Keys::Left, ControllingPlayer(), playerIndex) ||
            input.IsNewButtonPress(Buttons::DPadLeft, ControllingPlayer(), playerIndex)) {
            instance_->setPanProperty(ClampPm1(instance_->getPanProperty() - 0.1f));
        }
        if (input.IsNewKeyPress(Keys::Right, ControllingPlayer(), playerIndex) ||
            input.IsNewButtonPress(Buttons::DPadRight, ControllingPlayer(), playerIndex)) {
            instance_->setPanProperty(ClampPm1(instance_->getPanProperty() + 0.1f));
        }
        if (input.IsNewButtonPress(Buttons::LeftShoulder, ControllingPlayer(), playerIndex)) {
            instance_->setPitchProperty(ClampPm1(instance_->getPitchProperty() - 0.1f));
        }
        if (input.IsNewButtonPress(Buttons::RightShoulder, ControllingPlayer(), playerIndex)) {
            instance_->setPitchProperty(ClampPm1(instance_->getPitchProperty() + 0.1f));
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        if (unavailable_) {
            lines.push_back("NoAudioHardwareException: no audio device on this machine.");
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Up/Down (or DPad): Volume.  Left/Right (or DPad): Pan.");
        lines.push_back("Gamepad shoulders: Pitch. A looping tone plays continuously.");
        lines.emplace_back();
        lines.push_back("Volume: " + std::to_string(instance_->getVolumeProperty()));
        lines.push_back("Pan:    " + std::to_string(instance_->getPanProperty()));
        lines.push_back("Pitch:  " + std::to_string(instance_->getPitchProperty()));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static float Clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }
    static float ClampPm1(float v) { return v < -1.0f ? -1.0f : (v > 1.0f ? 1.0f : v); }

    std::optional<SoundEffect> effect_;
    std::optional<SoundEffectInstance> instance_;
    bool unavailable_ = false;
};

} // namespace CnaExamples::Demos::Audio::SoundEffectInstanceDemos
