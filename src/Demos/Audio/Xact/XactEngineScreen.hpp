// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/AudioEngine.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundBank.hpp"
#include "Microsoft/Xna/Framework/Audio/WaveBank.hpp"

#include "Demos/Audio/Xact/XactBanks.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::XactDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Audio::AudioEngine;
using Microsoft::Xna::Framework::Audio::SoundBank;
using Microsoft::Xna::Framework::Audio::WaveBank;

// XACT is XNA's data-driven audio system, and it is a genuinely different model
// from SoundEffect: the game does not load or own sounds at all. It loads three
// authored files -- global settings (.xgs), a wave bank (.xwb) and a sound bank
// (.xsb) -- and then asks for cues BY NAME. Which wave a cue plays, how loud,
// in which category, is authored data rather than code.
//
// The ordering below is not incidental and is the main thing this screen shows:
// AudioEngine first, then WaveBank and SoundBank against it. The banks register
// themselves with the engine, and the engine must outlive them.
//
// AudioEngine::Update() must also be pumped every frame -- it is what advances
// cue state and applies category changes. Unlike FrameworkDispatcher, Game does
// NOT call it for you.
class XactEngineScreen : public DemoScreen {
public:
    XactEngineScreen() : DemoScreen("XACT: Engine & Banks") {}

    void OnDemoLoad() override {
        generationError_ = GenerateXactBanks();
        if (!generationError_.empty()) return;
        OpenEngine();
    }

    void OnDemoUnload() override {
        // Strict reverse order: the banks hold a pointer to the engine and
        // unregister themselves from it on destruction.
        soundBank_.reset();
        waveBank_.reset();
        engine_.reset();
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        // Nothing else pumps this. Without it cue state never advances.
        if (engine_) {
            engine_->Update();
            updates_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!kXactGeneratorAvailable) {
            lines.push_back("XACT bank generation is not available in this build.");
            lines.emplace_back();
            lines.push_back("XACT needs three real binary files (.xgs / .xsb / .xwb). CNA reads");
            lines.push_back("that format and never writes it, and there is no XACT authoring tool");
            lines.push_back("on Linux -- so unlike every other asset here they cannot be generated");
            lines.push_back("locally.");
            lines.emplace_back();
            lines.push_back("../cnanext already solved this for its own demo_xact program. That header");
            lines.push_back("is put on the include path when ../cnanext is present, but never copied");
            lines.push_back("into this repository: it is Ms-PL and this repo is MIT.");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Three authored files, generated at runtime into a temp directory:");
        lines.push_back("  " + XactDirectory());
        lines.emplace_back();

        if (!generationError_.empty()) {
            lines.push_back("Generation failed: " + generationError_);
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }
        if (!openError_.empty()) {
            lines.push_back("Opening the engine or banks failed:");
            lines.push_back("  " + openError_);
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("AudioEngine   Demo.xgs    disposed: " +
                        std::string(engine_->getIsDisposedProperty() ? "true" : "false"));
        lines.push_back("WaveBank      Waves.xwb   prepared: " +
                        std::string(waveBank_->getIsPreparedProperty() ? "true" : "false") +
                        "   in use: " +
                        std::string(waveBank_->getIsInUseProperty() ? "true" : "false"));
        lines.push_back("SoundBank     Sounds.xsb  in use:   " +
                        std::string(soundBank_->getIsInUseProperty() ? "true" : "false"));
        lines.emplace_back();
        lines.push_back("AudioEngine::ContentVersion = " +
                        std::to_string((int)AudioEngine::ContentVersion) +
                        "   (the XACT tool version these files must match)");
        lines.push_back("Renderer details reported: " +
                        std::to_string((int)engine_->getRendererDetailsProperty().size()));
        lines.emplace_back();
        lines.push_back("Global variables live in the .xgs, not in code:");
        lines.push_back("  SpeedOfSound = " + std::to_string(engine_->GetGlobalVariable("SpeedOfSound")));
        lines.emplace_back();
        lines.push_back("AudioEngine::Update() calls this session: " + std::to_string(updates_));
        lines.push_back("Nothing pumps this for you -- unlike FrameworkDispatcher, Game::Update");
        lines.push_back("does not call it. Without it, cue state never advances.");
        lines.emplace_back();
        lines.push_back("Construction order matters: engine first, then the banks against it.");
        lines.push_back("The banks register with the engine and must not outlive it.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void OpenEngine() {
        try {
            engine_ = std::make_unique<AudioEngine>(SettingsPath());
            waveBank_ = std::make_unique<WaveBank>(engine_.get(), WaveBankPath());
            soundBank_ = std::make_unique<SoundBank>(engine_.get(), SoundBankPath());
        } catch (const std::exception& ex) {
            openError_ = ex.what();
            soundBank_.reset();
            waveBank_.reset();
            engine_.reset();
        }
    }

    std::unique_ptr<AudioEngine> engine_;
    std::unique_ptr<WaveBank> waveBank_;
    std::unique_ptr<SoundBank> soundBank_;
    std::string generationError_;
    std::string openError_;
    int updates_ = 0;
};

} // namespace CnaExamples::Demos::Audio::XactDemos
