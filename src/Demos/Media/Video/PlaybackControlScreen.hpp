// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Media/MediaState.hpp"
#include "Microsoft/Xna/Framework/Media/Video/Video.hpp"
#include "Microsoft/Xna/Framework/Media/Video/VideoPlayer.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Media/MediaDemoHelpers.hpp"

namespace CnaExamples::Demos::Media::VideoDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Media::MediaState;
using Microsoft::Xna::Framework::Media::Video;
using Microsoft::Xna::Framework::Media::VideoPlayer;

// Demonstrates VideoPlayer's transport controls (Play/Pause/Resume/Stop),
// IsLooped, and live-adjustable Volume/IsMuted -- all real, driving the same
// FFmpeg decoder + SDL3 audio stream LoadAndPlayScreen exercises.
class PlaybackControlScreen : public DemoScreen {
public:
    PlaybackControlScreen() : DemoScreen("Video: Playback Control") {}

    void LoadContent() override {
        video_.emplace(kTestClipMp4, &GetScreenManager()->getGraphicsDeviceProperty());
        player_.setIsLoopedProperty(true);
        player_.Play(&*video_);
    }

    void UnloadContent() override {
        player_.Dispose();
        video_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            switch (player_.getStateProperty()) {
                case MediaState::Stopped: player_.Play(&*video_); break;
                case MediaState::Playing: player_.Pause(); break;
                case MediaState::Paused:  player_.Resume(); break;
            }
        } else if (input.IsNewKeyPress(Keys::X, ControllingPlayer(), playerIndex)) {
            player_.Stop();
        } else if (input.IsNewKeyPress(Keys::L, ControllingPlayer(), playerIndex)) {
            player_.setIsLoopedProperty(!player_.getIsLoopedProperty());
        } else if (input.IsNewKeyPress(Keys::M, ControllingPlayer(), playerIndex)) {
            player_.setIsMutedProperty(!player_.getIsMutedProperty());
        } else if (input.IsMenuUp(ControllingPlayer())) {
            player_.setVolumeProperty(std::min(1.0f, player_.getVolumeProperty() + 0.1f));
        } else if (input.IsMenuDown(ControllingPlayer())) {
            player_.setVolumeProperty(std::max(0.0f, player_.getVolumeProperty() - 0.1f));
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: Stopped->Play, Playing->Pause, Paused->Resume.");
        lines.push_back("X: Stop   L: toggle Loop   M: toggle Mute   Up/Down: Volume +-0.1");
        lines.emplace_back();
        lines.push_back("State: " + std::string(MediaStateName(player_.getStateProperty())));
        lines.push_back("IsLooped: " + std::string(player_.getIsLoopedProperty() ? "true" : "false"));
        lines.push_back("IsMuted: " + std::string(player_.getIsMutedProperty() ? "true" : "false"));
        lines.push_back("Volume: " + std::to_string(player_.getVolumeProperty()));

        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        Texture2D* frame = player_.GetTexture();
        if (frame != nullptr) {
            sb.Draw(*frame, Vector2(40.0f, end.Y + 10.0f), mul(Color::White, TransitionAlpha()));
        }
    }

private:
    std::optional<Video> video_;
    VideoPlayer player_;
};

} // namespace CnaExamples::Demos::Media::VideoDemos
