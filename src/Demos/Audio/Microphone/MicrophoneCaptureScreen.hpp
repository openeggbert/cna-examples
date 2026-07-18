// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Audio/Microphone.hpp"
#include "Microsoft/Xna/Framework/Audio/MicrophoneState.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/EventArgs.hpp"
#include "System/Object.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Audio::MicrophoneDemos {

using namespace CnaExamples::GameStateManagement;
using System::EventArgs;
using System::Object;
using Microsoft::Xna::Framework::Audio::Microphone;
using Microsoft::Xna::Framework::Audio::MicrophoneState;
using SharpRuntime::bytecs;

// Demonstrates Start()/Stop()/GetData() on the default microphone, plus
// the BufferReady event (raised once enough captured data accumulates,
// per BufferDuration -- see Microphone::setBufferDurationProperty()).
// Requires a real capture device to show any activity; with none present
// this screen says so rather than fabricating readings.
class MicrophoneCaptureScreen : public DemoScreen {
public:
    MicrophoneCaptureScreen() : DemoScreen("Microphone: Capture") {}

    void LoadContent() override {
        mic_ = Microphone::getDefaultProperty();
        if (mic_) {
            bufferReadyToken_ = mic_->BufferReady.Add(
                [this](Object*, const EventArgs&) { bufferReadyCount_++; });
        }
    }

    void UnloadContent() override {
        if (mic_) {
            if (mic_->getStateProperty() == MicrophoneState::Started) mic_->Stop();
            mic_->BufferReady.Remove(bufferReadyToken_);
        }
    }

protected:
    void OnDemoInput(InputState& input) override {
        if (!mic_) return;
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            if (mic_->getStateProperty() == MicrophoneState::Started) mic_->Stop();
            else mic_->Start();
        }
    }

    void OnDemoUpdate(GameTime&) override {
        if (!mic_ || mic_->getStateProperty() != MicrophoneState::Started) return;
        scratch_.resize(4096);
        const int read = mic_->GetData(scratch_);
        totalBytesRead_ += read;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        if (!mic_) {
            lines.push_back("No default microphone -- connect a capture device to try this.");
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Space/Enter/A/tap starts/stops capture on: " + mic_->Name);
        lines.emplace_back();
        lines.push_back(std::string("State: ") +
                        (mic_->getStateProperty() == MicrophoneState::Started ? "Started" : "Stopped"));
        lines.push_back("Total bytes read via GetData(): " + std::to_string(totalBytesRead_));
        lines.push_back("BufferReady fired: " + std::to_string(bufferReadyCount_) + " time(s)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    Microphone* mic_ = nullptr;
    std::size_t bufferReadyToken_ = 0;
    std::vector<bytecs> scratch_;
    int totalBytesRead_ = 0;
    int bufferReadyCount_ = 0;
};

} // namespace CnaExamples::Demos::Audio::MicrophoneDemos
