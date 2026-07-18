// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/Microphone.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::MicrophoneDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Audio::Microphone;

// Demonstrates Microphone::getAllProperty()/getDefaultProperty() and the
// per-device static properties (SampleRate, IsHeadset, BufferDuration).
// Microphone::Default is nullptr when no capture device exists, which this
// screen shows honestly rather than assuming one is always present.
class MicrophoneEnumerationScreen : public DemoScreen {
public:
    MicrophoneEnumerationScreen() : DemoScreen("Microphone: Enumeration") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const auto& all = Microphone::getAllProperty();
        Microphone* defaultMic = Microphone::getDefaultProperty();

        std::vector<std::string> lines;
        lines.push_back("Microphone::All count: " + std::to_string(all.size()));
        for (const Microphone* mic : all) {
            lines.push_back("  " + mic->Name + " -- " + std::to_string(mic->getSampleRateProperty()) +
                            "Hz, headset=" + (mic->getIsHeadsetProperty() ? "yes" : "no"));
        }
        lines.emplace_back();
        lines.push_back(std::string("Default: ") +
                        (defaultMic ? defaultMic->Name : "(none -- no capture device present)"));

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Audio::MicrophoneDemos
