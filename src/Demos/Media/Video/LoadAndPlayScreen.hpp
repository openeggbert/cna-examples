// SPDX-License-Identifier: MIT
#pragma once

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
using Microsoft::Xna::Framework::Media::Video;
using Microsoft::Xna::Framework::Media::VideoPlayer;

// Demonstrates a real, FFmpeg-decoded video: Video's constructor probes the
// file for real Width/Height/FramesPerSecond/Duration, and VideoPlayer::
// GetTexture() decodes real frames on demand, clock-driven, into a live
// Texture2D drawn here every frame. The clip is a fully synthetic ffmpeg
// lavfi testsrc (color bars) + a sine tone, muxed to H.264/AAC MP4 -- no
// licensed video asset is bundled with this app (see plan.md section 5.5).
class LoadAndPlayScreen : public DemoScreen {
public:
    LoadAndPlayScreen() : DemoScreen("Video: Load & Play") {}

    void LoadContent() override {
        video_.emplace(kTestClipMp4, &GetScreenManager()->getGraphicsDeviceProperty());
        player_.Play(&*video_);
    }

    void UnloadContent() override {
        player_.Dispose();
        video_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A real 320x240 H.264/AAC clip, decoded live via FFmpeg.");
        lines.emplace_back();
        lines.push_back("Video: " + std::to_string(video_->getWidthProperty()) + "x" +
                         std::to_string(video_->getHeightProperty()) + " @ " +
                         std::to_string(video_->getFramesPerSecondProperty()) + " fps");
        lines.push_back("Duration: " + std::to_string(video_->getDurationProperty().getTotalSecondsProperty()) + "s");
        lines.push_back("State: " + std::string(MediaStateName(player_.getStateProperty())));
        lines.push_back("PlayPosition: " + std::to_string(player_.getPlayPositionProperty().getTotalSecondsProperty()) + "s");

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
