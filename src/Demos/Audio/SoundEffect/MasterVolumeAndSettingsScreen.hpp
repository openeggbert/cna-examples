// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/AudioChannels.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "System/TimeSpan.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::SoundEffectDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Audio::AudioChannels;
using Microsoft::Xna::Framework::Audio::SoundEffect;

// Demonstrates SoundEffect's static, process-wide settings (MasterVolume,
// DistanceScale, DopplerScale, SpeedOfSound -- the latter three only take
// effect via Apply3D, see the 3D Audio category) plus the static
// byte-count/duration math helpers (GetSampleDuration/GetSampleSizeInBytes),
// restoring the original MasterVolume on exit so leaving this screen
// doesn't silently change every other screen's audio level.
class MasterVolumeAndSettingsScreen : public DemoScreen {
public:
    MasterVolumeAndSettingsScreen() : DemoScreen("SoundEffect: Static Settings") {}

    void LoadContent() override {
        originalMasterVolume_ = SoundEffect::getMasterVolumeProperty();
    }

    void UnloadContent() override {
        SoundEffect::setMasterVolumeProperty(originalMasterVolume_);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            float v = SoundEffect::getMasterVolumeProperty() - 0.25f;
            if (v < 0.0f) v = 1.0f;
            SoundEffect::setMasterVolumeProperty(v);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap steps MasterVolume down by 0.25, wrapping at 0.");
        lines.emplace_back();
        lines.push_back("MasterVolume:  " + std::to_string(SoundEffect::getMasterVolumeProperty()));
        lines.push_back("DistanceScale: " + std::to_string(SoundEffect::getDistanceScaleProperty()));
        lines.push_back("DopplerScale:  " + std::to_string(SoundEffect::getDopplerScaleProperty()));
        lines.push_back("SpeedOfSound:  " + std::to_string(SoundEffect::getSpeedOfSoundProperty()));
        lines.emplace_back();

        const auto duration = System::TimeSpan::FromSeconds(1.0);
        const int bytes = SoundEffect::GetSampleSizeInBytes(duration, 44100, AudioChannels::Mono);
        const auto roundTrip = SoundEffect::GetSampleDuration(bytes, 44100, AudioChannels::Mono);
        lines.push_back("GetSampleSizeInBytes(1s, 44100Hz, Mono) = " + std::to_string(bytes) + " bytes");
        lines.push_back("GetSampleDuration(that many bytes) = " +
                        std::to_string(roundTrip.getTotalSecondsProperty()) + "s (round-trips)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    float originalMasterVolume_ = 1.0f;
};

} // namespace CnaExamples::Demos::Audio::SoundEffectDemos
