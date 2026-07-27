// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/GraphicsBackendType.hpp"
#include "CNA/Platform.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Diagnostics::PlatformDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// What this particular binary is: which platform it was compiled for, which
// graphics backend was selected, and which optional subsystems were compiled
// in at all.
//
// The last part is the useful one. Several CNA features are compile-time
// options that vanish entirely when off -- CNA_DEVICES is OFF by default in
// CNA itself, and this app forces it ON precisely so the Devices area can
// exist. A screen reporting "not available" is very different from one
// reporting "compiled out", and only the build knows which.
//
// Everything here is constexpr or a preprocessor check: there is nothing to
// query at runtime, because these decisions were all made before the binary
// existed.
class PlatformInfoScreen : public DemoScreen {
public:
    PlatformInfoScreen() : DemoScreen("Diagnostics: Platform & Build") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Everything below was decided at COMPILE time. Nothing here is queried");
        lines.push_back("at runtime, because none of it can change while the app is running.");
        lines.emplace_back();
        lines.push_back("CNA::getCurrentPlatform()          = " + PlatformName());
        lines.push_back("CNA::getCurrentGraphicsBackendName = " +
                        std::string(CNA::getCurrentGraphicsBackendName()));
        lines.emplace_back();
        lines.push_back("Compiler:");
        lines.push_back("  " + CompilerName());
        lines.push_back("  __cplusplus = " + std::to_string((long)__cplusplus));
        lines.push_back("  built " __DATE__ " " __TIME__);
        lines.emplace_back();
        lines.push_back("Optional CNA subsystems, as compiled into THIS binary:");
        for (const auto& line : Features()) lines.push_back("  " + line);
        lines.emplace_back();
        lines.push_back("CNA_DEVICES defaults to OFF in CNA itself -- these are CNA-specific");
        lines.push_back("extensions beyond XNA 4.0, not something every consumer wants. This app");
        lines.push_back("forces it ON in its own CMakeLists so the Devices area can exist at all.");
        lines.emplace_back();
        lines.push_back("\"Not available\" and \"compiled out\" are different answers, and only");
        lines.push_back("the build knows which one applies.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static std::string PlatformName() {
        switch (CNA::getCurrentPlatform()) {
            case CNA::Platform::Desktop: return "Desktop";
            case CNA::Platform::Android: return "Android";
            case CNA::Platform::iOS:     return "iOS";
            case CNA::Platform::Web:     return "Web (Emscripten)";
        }
        return "(unknown)";
    }

    static std::string CompilerName() {
#if defined(__clang__)
        return "Clang " __clang_version__;
#elif defined(__GNUC__)
        return "GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__);
#elif defined(_MSC_VER)
        return "MSVC " + std::to_string(_MSC_VER);
#else
        return "(unrecognised compiler)";
#endif
    }

    // Reported by testing the macros the build actually defines, so this cannot
    // drift from the real configuration the way a hand-maintained list would.
    static std::vector<std::string> Features() {
        std::vector<std::string> features;
        features.push_back(std::string("CNA_DEVICES      ") +
#if defined(CNA_DEVICES)
                           "compiled in    (Devices area: sensors, camera, clipboard, ...)");
#else
                           "compiled OUT   (the Devices area would not exist)");
#endif
        features.push_back(std::string("SOUND_ENABLED    ") +
#if defined(SOUND_ENABLED)
                           "compiled in    (Audio and Media playback are real)");
#else
                           "compiled OUT   (audio calls become no-ops)");
#endif
        features.push_back(std::string("NOXNA purity     ") +
#if defined(CNA_NOXNA)
                           "ON             (non-XNA declarations are deprecation warnings)");
#else
                           "off            (CNA's NOXNA extensions are freely usable)");
#endif
        features.push_back(std::string("Build type       ") +
#if defined(NDEBUG)
                           "Release-style  (NDEBUG defined, assertions compiled out)");
#else
                           "Debug-style    (NDEBUG not defined, assertions live)");
#endif
        return features;
    }
};

} // namespace CnaExamples::Demos::Diagnostics::PlatformDemos
