// SPDX-License-Identifier: MIT
//
// Asserts what the Audio area's XACT screens state on screen.
//
// Both XACT screens catch their own exceptions and print them, so a total
// failure to open the engine still renders a clean-looking screenshot and
// passes a sweep -- the same trap that hid two real bugs in the XNB screen.
// This checks the claims directly instead: that the banks are really generated,
// really opened in the documented order, and that the cue state machine behaves
// the way XactCueScreen draws it.
//
// Build and run (needs the app's own build tree for includes and libraries):
//
//   FLAGS=$(grep -m1 CXX_INCLUDES build/CMakeFiles/cna_examples.dir/flags.make | sed 's/^CXX_INCLUDES = //')
//   DEFS=$(grep -m1 CXX_DEFINES  build/CMakeFiles/cna_examples.dir/flags.make | sed 's/^CXX_DEFINES = //')
//   LINK=$(sed 's|^/usr/bin/c++ .*-o cna_examples||' build/CMakeFiles/cna_examples.dir/link.txt)
//   (cd build && g++ -std=c++23 $DEFS $FLAGS ../tools/checks/xact_claims.cpp -o /tmp/xact_claims $LINK)
//   (cd build && /tmp/xact_claims)

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Audio/AudioCategory.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioEngine.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioStopOptions.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundBank.hpp"
#include "Microsoft/Xna/Framework/Audio/WaveBank.hpp"

#include "Demos/Audio/Xact/XactBanks.hpp"

using Microsoft::Xna::Framework::Audio::AudioCategory;
using Microsoft::Xna::Framework::Audio::AudioEngine;
using Microsoft::Xna::Framework::Audio::AudioStopOptions;
using Microsoft::Xna::Framework::Audio::Cue;
using Microsoft::Xna::Framework::Audio::SoundBank;
using Microsoft::Xna::Framework::Audio::WaveBank;

namespace Xact = CnaExamples::Demos::Audio::XactDemos;

namespace {

int fails = 0;

void check(const char* what, bool ok) {
    std::printf("  %-62s %s\n", what, ok ? "ok" : "FAIL");
    if (!ok) fails++;
}

template <typename Fn>
bool Throws(Fn&& fn) {
    try { fn(); return false; } catch (...) { return true; }
}

std::uintmax_t SizeOf(const std::string& path) {
    std::error_code ec;
    const auto size = std::filesystem::file_size(path, ec);
    return ec ? 0 : size;
}

} // namespace

int main() {
    if (!Xact::kXactGeneratorAvailable) {
        std::printf("XactFileGen.hpp was not on the include path -- nothing to check.\n");
        std::printf("(That is the same condition the screens report; not a failure here.)\n");
        return 0;
    }

    // --- The banks are really produced, not merely reported as produced. -----
    const std::string generationError = Xact::GenerateXactBanks();
    check("GenerateXactBanks() succeeds", generationError.empty());
    if (!generationError.empty()) {
        std::printf("  generation error: %s\n", generationError.c_str());
        return 1;
    }
    check("all three bank files exist", Xact::BanksExist());
    check("Demo.xgs is non-empty", SizeOf(Xact::SettingsPath()) > 0);
    check("Sounds.xsb is non-empty", SizeOf(Xact::SoundBankPath()) > 0);
    check("Waves.xwb holds four 0.8s waves (>100 KB)",
          SizeOf(Xact::WaveBankPath()) > 100000);

    // --- Construction order, the main claim of the Engine & Banks screen. ----
    std::unique_ptr<AudioEngine> engine;
    std::unique_ptr<WaveBank> waveBank;
    std::unique_ptr<SoundBank> soundBank;
    bool opened = false;
    try {
        engine = std::make_unique<AudioEngine>(Xact::SettingsPath());
        waveBank = std::make_unique<WaveBank>(engine.get(), Xact::WaveBankPath());
        soundBank = std::make_unique<SoundBank>(engine.get(), Xact::SoundBankPath());
        opened = true;
    } catch (const std::exception& ex) {
        std::printf("  opening threw: %s\n", ex.what());
    }
    check("AudioEngine, WaveBank and SoundBank all open", opened);
    if (!opened) return 1;

    check("AudioEngine::ContentVersion is 46", AudioEngine::ContentVersion == 46);
    check("the wave bank reports itself prepared", waveBank->getIsPreparedProperty());
    check("SpeedOfSound comes out of the .xgs as 343",
          std::fabs(engine->GetGlobalVariable("SpeedOfSound") - 343.0f) < 0.001f);
    check("an unauthored global variable is rejected",
          Throws([&] { (void)engine->GetGlobalVariable("NotAuthored"); }));

    // --- Cues are looked up by authored name. --------------------------------
    bool allFourFound = true;
    for (const char* name : Xact::kCueNames) {
        std::unique_ptr<Cue> cue;
        try { cue.reset(soundBank->GetCue(name)); } catch (...) {}
        if (!cue || cue->getNameProperty() != name) allFourFound = false;
    }
    check("all four authored cues resolve by name", allFourFound);
    check("an unauthored cue name is rejected",
          Throws([&] { (void)soundBank->GetCue("NoSuchCue"); }));

    // --- The seven-flag state machine the cue screen draws. ------------------
    std::unique_ptr<Cue> cue(soundBank->GetCue(Xact::kCueNames[0]));

    // Six of the seven flags are ONE mutually-exclusive state value, not six
    // independent booleans -- exactly one is true at a time. IsPaused is the
    // odd one out: a separate bit layered on top of Playing.
    const auto exclusiveStatesSet = [&] {
        return (int)cue->getIsCreatedProperty() + (int)cue->getIsPreparingProperty() +
               (int)cue->getIsPreparedProperty() + (int)cue->getIsPlayingProperty() +
               (int)cue->getIsStoppingProperty() + (int)cue->getIsStoppedProperty();
    };
    check("exactly one of the six state flags is set on a fresh cue",
          exclusiveStatesSet() == 1);
    check("a cue from GetCue() is Prepared -- NOT Created",
          cue->getIsPreparedProperty() && !cue->getIsCreatedProperty());
    check("a fresh cue is not Playing", !cue->getIsPlayingProperty());

    cue->Play();
    engine->Update();
    check("after Play() the cue is Playing and not Stopped",
          cue->getIsPlayingProperty() && !cue->getIsStoppedProperty());

    // Real FACT only ever sets/clears the PAUSED bit, so a paused cue stays
    // IsPlaying==true. CNA models that bitmask rather than a single-value enum,
    // which is why the screen shows all seven flags instead of one state word.
    cue->Pause();
    check("a paused cue is IsPaused AND still IsPlaying (FACT bitmask semantics)",
          cue->getIsPausedProperty() && cue->getIsPlayingProperty());
    check("pausing does not add a second exclusive state", exclusiveStatesSet() == 1);
    cue->Resume();
    check("Resume() clears IsPaused", !cue->getIsPausedProperty());

    cue->Stop(AudioStopOptions::Immediate);
    engine->Update();
    check("after Stop(Immediate) the cue is Stopped and not Playing",
          cue->getIsStoppedProperty() && !cue->getIsPlayingProperty());

    // The single-use claim: a stopped cue does not go back to Playing, whether
    // CNA refuses the call or silently ignores it.
    try { cue->Play(); } catch (...) {}
    engine->Update();
    check("a stopped cue cannot be replayed (single-use)", !cue->getIsPlayingProperty());

    // --- Categories act on cues the game never names. ------------------------
    bool categoryOk = false;
    try {
        AudioCategory music = engine->GetCategory("Music");
        music.SetVolume(0.0f);
        music.SetVolume(1.0f);
        categoryOk = true;
    } catch (const std::exception& ex) {
        std::printf("  GetCategory(\"Music\") threw: %s\n", ex.what());
    }
    check("the authored Music category exists and takes SetVolume", categoryOk);
    check("the authored SFX category exists",
          !Throws([&] { (void)engine->GetCategory("SFX"); }));
    check("an unauthored category is rejected",
          Throws([&] { (void)engine->GetCategory("NoSuchCategory"); }));

    // Reverse-order teardown, as the screen states.
    cue.reset();
    soundBank.reset();
    waveBank.reset();
    engine.reset();

    std::printf("\n%s (%d failure(s))\n", fails ? "FAILURES" : "all XACT claims hold", fails);
    return fails ? 1 : 0;
}
