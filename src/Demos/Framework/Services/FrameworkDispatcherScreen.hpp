// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/FrameworkDispatcher.hpp"
#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Framework::ServicesDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::FrameworkDispatcher;

// FrameworkDispatcher::Update() is the pump that turns deferred framework work
// into the events and callbacks an application actually observes: MediaPlayer's
// ActiveSongChanged/MediaStateChanged, and refilling DynamicSoundEffectInstance
// buffers.
//
// The reason most XNA code never mentions it is that Game::Update() calls it
// for you, once per frame. Code that does NOT use Game -- a headless tool, a
// custom loop, a unit test -- has to call it itself or those events simply
// never fire.
//
// This screen makes the mechanism visible: the pending-event flags the
// dispatcher consumes are readable (they are NOXNA extensions in CNA), and
// calling Update() by hand here is a real, harmless extra pump.
class FrameworkDispatcherScreen : public DemoScreen {
public:
    FrameworkDispatcherScreen() : DemoScreen("Services: FrameworkDispatcher") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            // Calling it a second time in a frame is safe: it drains whatever is
            // pending and does nothing when nothing is.
            FrameworkDispatcher::Update();
            manualPumps_++;
        }
    }

    void OnDemoUpdate(GameTime&) override {
        frames_++;
        // Sampled before this frame's automatic pump has necessarily happened,
        // so a flag caught here is genuinely one that was still outstanding.
        if (FrameworkDispatcher::ActiveSongChanged) sawActiveSongPending_++;
        if (FrameworkDispatcher::MediaStateChanged) sawMediaStatePending_++;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: call FrameworkDispatcher::Update() by hand");
        lines.emplace_back();
        lines.push_back("Frames on this screen: " + std::to_string(frames_));
        lines.push_back("Manual pumps:          " + std::to_string(manualPumps_));
        lines.emplace_back();
        lines.push_back("Pending flags the dispatcher drains (NOXNA, readable in CNA):");
        lines.push_back("  ActiveSongChanged pending now: " +
                        std::string(FrameworkDispatcher::ActiveSongChanged ? "true" : "false"));
        lines.push_back("  MediaStateChanged pending now: " +
                        std::string(FrameworkDispatcher::MediaStateChanged ? "true" : "false"));
        lines.push_back("  Frames where a pending flag was observed: " +
                        std::to_string(sawActiveSongPending_) + " / " +
                        std::to_string(sawMediaStatePending_));
        lines.push_back("  Registered dynamic audio streams: " +
                        std::to_string((int)FrameworkDispatcher::Streams.size()));
        lines.emplace_back();
        lines.push_back("Both flags normally read false here, and that is the correct result:");
        lines.push_back("Game::Update() already pumped the dispatcher earlier this same frame.");
        lines.push_back("The Media area's 'MediaPlayer Events' demo is where they actually fire.");
        lines.emplace_back();
        lines.push_back("Without a Game -- a headless tool, a custom loop, a test -- nothing calls");
        lines.push_back("Update() for you, and MediaPlayer's events would never be raised at all.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int frames_ = 0;
    int manualPumps_ = 0;
    int sawActiveSongPending_ = 0;
    int sawMediaStatePending_ = 0;
};

} // namespace CnaExamples::Demos::Framework::ServicesDemos
