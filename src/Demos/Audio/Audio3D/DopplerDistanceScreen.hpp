// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/AudioChannels.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioEmitter.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioListener.hpp"
#include "Microsoft/Xna/Framework/Audio/NoAudioHardwareException.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/Audio/AudioDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::Audio3DDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Audio::AudioChannels;
using Microsoft::Xna::Framework::Audio::AudioEmitter;
using Microsoft::Xna::Framework::Audio::AudioListener;
using Microsoft::Xna::Framework::Audio::NoAudioHardwareException;
using Microsoft::Xna::Framework::Audio::SoundEffect;
using Microsoft::Xna::Framework::Audio::SoundEffectInstance;

// Demonstrates SoundEffect::DopplerScale/SpeedOfSound: Apply3D computes a
// real closed-form Doppler pitch shift from the emitter's Velocity (not
// just its Position, see the plain Apply3DScreen) relative to the
// listener. Space/Enter/A/tap cycles the emitter's velocity through
// approaching / stationary / receding, heard as the tone's pitch bending.
class DopplerDistanceScreen : public DemoScreen {
public:
    DopplerDistanceScreen() : DemoScreen("3D Audio: Doppler & Distance Scale") {}

    void LoadContent() override {
        try {
            effect_.emplace(GenerateSineWavePcm16(392.0f, 6.0f), 44100, AudioChannels::Mono);
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
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            velocityIndex_ = (velocityIndex_ + 1) % 3;
        }
    }

    void OnDemoUpdate(GameTime&) override {
        if (!instance_.has_value()) return;
        AudioListener listener; // at origin, stationary.
        AudioEmitter emitter;
        emitter.setPositionProperty(Vector3(5.0f, 0.0f, 0.0f));
        // -Z: approaching the listener along X toward the origin. Units are
        // "per second" in whatever world scale SpeedOfSound is expressed in.
        static constexpr float kVelocities[3] = {-20.0f, 0.0f, 20.0f};
        emitter.setVelocityProperty(Vector3(kVelocities[velocityIndex_], 0.0f, 0.0f));
        instance_->Apply3D(listener, emitter);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        if (unavailable_) {
            lines.push_back("NoAudioHardwareException: no audio device on this machine.");
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        static constexpr const char* kNames[3] = {"Approaching (-X)", "Stationary", "Receding (+X)"};

        lines.push_back("Space/Enter/A/tap cycles the emitter's velocity: approaching/still/receding.");
        lines.emplace_back();
        lines.push_back(std::string("Velocity: ") + kNames[velocityIndex_]);
        lines.push_back("SoundEffect::DopplerScale: " + std::to_string(SoundEffect::getDopplerScaleProperty()));
        lines.push_back("SoundEffect::SpeedOfSound: " + std::to_string(SoundEffect::getSpeedOfSoundProperty()));
        lines.push_back("Resulting Pitch (from Doppler): " + std::to_string(instance_->getPitchProperty()));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<SoundEffect> effect_;
    std::optional<SoundEffectInstance> instance_;
    int velocityIndex_ = 1;
    bool unavailable_ = false;
};

} // namespace CnaExamples::Demos::Audio::Audio3DDemos
