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

// Demonstrates SoundEffectInstance::Apply3D(listener, emitter): a fixed
// AudioListener at the origin and a movable AudioEmitter (Left/Right, or
// DPad, moves it along X) -- Apply3D recomputes the instance's Volume/Pan
// from the emitter's position relative to the listener each frame, heard
// as the tone panning and fading with distance as you move it.
class Apply3DScreen : public DemoScreen {
public:
    Apply3DScreen() : DemoScreen("3D Audio: Apply3D") {}

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
        if (input.IsNewKeyPress(Keys::Left, ControllingPlayer(), playerIndex) ||
            input.IsNewButtonPress(Buttons::DPadLeft, ControllingPlayer(), playerIndex)) {
            emitterX_ -= 2.0f;
        }
        if (input.IsNewKeyPress(Keys::Right, ControllingPlayer(), playerIndex) ||
            input.IsNewButtonPress(Buttons::DPadRight, ControllingPlayer(), playerIndex)) {
            emitterX_ += 2.0f;
        }
    }

    void OnDemoUpdate(GameTime&) override {
        if (!instance_.has_value()) return;
        AudioListener listener; // default: at origin, facing -Z, matching FNA's default.
        AudioEmitter emitter;
        emitter.setPositionProperty(Vector3(emitterX_, 0.0f, 0.0f));
        instance_->Apply3D(listener, emitter);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        if (unavailable_) {
            lines.push_back("NoAudioHardwareException: no audio device on this machine.");
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Left/Right (or DPad) moves the emitter along X; the listener stays at");
        lines.push_back("the origin. Apply3D() recomputes Volume/Pan every frame from that.");
        lines.emplace_back();
        lines.push_back("Emitter X: " + std::to_string(emitterX_));
        lines.push_back("Resulting Volume: " + std::to_string(instance_->getVolumeProperty()));
        lines.push_back("Resulting Pan:    " + std::to_string(instance_->getPanProperty()));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<SoundEffect> effect_;
    std::optional<SoundEffectInstance> instance_;
    float emitterX_ = 0.0f;
    bool unavailable_ = false;
};

} // namespace CnaExamples::Demos::Audio::Audio3DDemos
