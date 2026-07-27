// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/AudioCategory.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioEngine.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioStopOptions.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundBank.hpp"
#include "Microsoft/Xna/Framework/Audio/WaveBank.hpp"

#include "Demos/Audio/Xact/XactBanks.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::XactDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Audio::AudioCategory;
using Microsoft::Xna::Framework::Audio::AudioEngine;
using Microsoft::Xna::Framework::Audio::AudioStopOptions;
using Microsoft::Xna::Framework::Audio::Cue;
using Microsoft::Xna::Framework::Audio::SoundBank;
using Microsoft::Xna::Framework::Audio::WaveBank;

// Cues and categories -- the half of XACT a game actually talks to.
//
// A Cue is a named, stateful playback instance with a genuinely detailed state
// machine: Created, Preparing, Prepared, Playing, Paused, Stopping, Stopped.
// All seven are shown live, because "is it playing" is rarely the only question
// and the intermediate states are where XACT differs from SoundEffectInstance.
//
// Categories are the reason to use XACT at all. Every cue belongs to one, named
// in the authored data, and a category can be volume-controlled, paused or
// stopped as a group without the game knowing which cues are in it. The banks
// here define Music (Tone1, Tone2) and SFX (Tone3, Tone4), so a category action
// visibly affects half the cues.
class XactCueScreen : public DemoScreen {
public:
    XactCueScreen() : DemoScreen("XACT: Cues & Categories") {}

    void OnDemoLoad() override {
        generationError_ = GenerateXactBanks();
        if (!generationError_.empty()) return;
        try {
            engine_ = std::make_unique<AudioEngine>(SettingsPath());
            waveBank_ = std::make_unique<WaveBank>(engine_.get(), WaveBankPath());
            soundBank_ = std::make_unique<SoundBank>(engine_.get(), SoundBankPath());
        } catch (const std::exception& ex) {
            openError_ = ex.what();
        }
    }

    void OnDemoUnload() override {
        cue_.reset();
        soundBank_.reset();
        waveBank_.reset();
        engine_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        if (!soundBank_) return;
        PlayerIndex playerIndex;

        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            // A Cue is single-use: once stopped it cannot be replayed, so each
            // press asks the bank for a fresh one. That is XACT's design, not a
            // limitation of this demo.
            try {
                cue_.reset(soundBank_->GetCue(kCueNames[cueIndex_]));
                if (cue_) cue_->Play();
                lastAction_ = std::string("GetCue(\"") + kCueNames[cueIndex_] + "\") + Play()";
            } catch (const std::exception& ex) {
                lastAction_ = std::string("threw: ") + ex.what();
            }
        }
        if (input.IsMenuUp(ControllingPlayer())) {
            cueIndex_ = (cueIndex_ + 1) % 4;
            lastAction_ = std::string("selected cue \"") + kCueNames[cueIndex_] + "\"";
        }
        if (input.IsMenuDown(ControllingPlayer())) {
            ToggleCategory();
        }
        if (!cue_) return;
        if (input.IsNewKeyPress(Keys::Left, ControllingPlayer(), playerIndex)) {
            // Pause/Resume, so the IsPaused-plus-IsPlaying pair below can be seen.
            if (cue_->getIsPausedProperty()) {
                cue_->Resume();
                lastAction_ = "Resume()";
            } else {
                cue_->Pause();
                lastAction_ = "Pause()";
            }
        }
        if (input.IsNewKeyPress(Keys::Right, ControllingPlayer(), playerIndex)) {
            cue_->Stop(AudioStopOptions::Immediate);
            lastAction_ = "Stop(AudioStopOptions::Immediate)";
        }
    }

    void OnDemoUpdate(GameTime&) override {
        if (engine_) engine_->Update();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!kXactGeneratorAvailable) {
            lines.push_back("XACT bank generation is not available in this build.");
            lines.push_back("See the Engine & Banks screen for why.");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }
        if (!soundBank_) {
            lines.push_back(generationError_.empty() ? ("Could not open the banks: " + openError_)
                                                     : ("Generation failed: " + generationError_));
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Space/Enter/A/tap: play the selected cue    Up: next cue");
        lines.push_back("Left: pause/resume    Right: stop    Down: mute/unmute the Music category");
        lines.emplace_back();
        lines.push_back("Cues in the bank (category is authored data, not code):");
        for (int i = 0; i < 4; ++i) {
            lines.push_back(std::string(i == cueIndex_ ? "  > " : "    ") + kCueNames[i] +
                            "   " + kCueCategoryNames[i] +
                            "   " + std::to_string((int)kCueFrequencies[i]) + " Hz");
        }
        lines.emplace_back();
        lines.push_back("Last action: " + (lastAction_.empty() ? std::string("(none)") : lastAction_));
        lines.emplace_back();

        if (!cue_) {
            lines.push_back("No cue instance yet -- press Space to get one and play it.");
        } else {
            lines.push_back("Cue \"" + cue_->getNameProperty() + "\" state, all seven flags:");
            lines.push_back("  IsCreated   " + Yn(cue_->getIsCreatedProperty()) +
                            "   IsPreparing " + Yn(cue_->getIsPreparingProperty()) +
                            "   IsPrepared " + Yn(cue_->getIsPreparedProperty()));
            lines.push_back("  IsPlaying   " + Yn(cue_->getIsPlayingProperty()) +
                            "   IsStopping  " + Yn(cue_->getIsStoppingProperty()) +
                            "   IsStopped  " + Yn(cue_->getIsStoppedProperty()));
            lines.push_back("  IsPaused    " + Yn(cue_->getIsPausedProperty()) +
                            "   <- the odd one out, see below");
        }
        lines.emplace_back();
        lines.push_back("Those seven are not seven independent booleans. Six of them are one");
        lines.push_back("mutually-exclusive state value -- exactly one is ever true -- so a cue");
        lines.push_back("straight out of GetCue() reads IsPrepared, never IsCreated.");
        lines.push_back("IsPaused is the exception: real FACT only sets and clears a PAUSED bit and");
        lines.push_back("never touches PLAYING, so a paused cue stays IsPlaying=yes as well.");
        lines.emplace_back();
        lines.push_back("Music category volume: " + std::string(musicMuted_ ? "0.0 (muted)" : "1.0"));
        lines.push_back("A category acts on every cue in it at once, and the game never needs to");
        lines.push_back("know which cues those are -- that is the whole reason to use XACT.");
        lines.emplace_back();
        lines.push_back("A Cue is single-use: once stopped it cannot be replayed, so each press");
        lines.push_back("asks the SoundBank for a fresh instance. That is XACT's design.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static std::string Yn(bool value) { return value ? "yes" : "no "; }

    void ToggleCategory() {
        try {
            AudioCategory music = engine_->GetCategory("Music");
            musicMuted_ = !musicMuted_;
            music.SetVolume(musicMuted_ ? 0.0f : 1.0f);
            lastAction_ = std::string("Music category SetVolume(") +
                          (musicMuted_ ? "0.0" : "1.0") + ")";
        } catch (const std::exception& ex) {
            lastAction_ = std::string("GetCategory threw: ") + ex.what();
        }
    }

    std::unique_ptr<AudioEngine> engine_;
    std::unique_ptr<WaveBank> waveBank_;
    std::unique_ptr<SoundBank> soundBank_;
    std::unique_ptr<Cue> cue_;
    std::string generationError_;
    std::string openError_;
    std::string lastAction_;
    int cueIndex_ = 0;
    bool musicMuted_ = false;
};

} // namespace CnaExamples::Demos::Audio::XactDemos
