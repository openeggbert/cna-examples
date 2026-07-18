// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/Video/Video.hpp"
#include "Microsoft/Xna/Framework/Media/Video/VideoPlayer.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Media/MediaDemoHelpers.hpp"

namespace CnaExamples::Demos::Media::VideoDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Media::Video;
using Microsoft::Xna::Framework::Media::VideoPlayer;

// Demonstrates Video::SetAudioTrackEXT()/SetVideoTrackEXT() and VideoPlayer's
// matching pair -- CNA extensions with no stock XNA equivalent (real XNA's
// Video/VideoPlayer have no multi-track concept at all). This app's own
// synthetic test clip only has 1 video + 1 audio stream, so selecting track
// 0 is a real, no-throw call into the real API path rather than an
// observable change -- honestly noted here rather than staged to look more
// dramatic than it is.
class MultiTrackEXTScreen : public DemoScreen {
public:
    MultiTrackEXTScreen() : DemoScreen("Video: Multi-Track (EXT)") {}

    void LoadContent() override {
        video_.emplace(kTestClipMp4, &GetScreenManager()->getGraphicsDeviceProperty());
        player_.Play(&*video_);
    }

    void UnloadContent() override {
        player_.Dispose();
        video_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsNewKeyPress(Keys::A, ControllingPlayer(), playerIndex)) {
            video_->SetAudioTrackEXT(0);
            player_.SetAudioTrackEXT(0);
            audioTrackCalls_++;
        } else if (input.IsNewKeyPress(Keys::V, ControllingPlayer(), playerIndex)) {
            video_->SetVideoTrackEXT(0);
            player_.SetVideoTrackEXT(0);
            videoTrackCalls_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A: Video::SetAudioTrackEXT(0) + VideoPlayer::SetAudioTrackEXT(0)");
        lines.push_back("V: Video::SetVideoTrackEXT(0) + VideoPlayer::SetVideoTrackEXT(0)");
        lines.emplace_back();
        lines.push_back("SetAudioTrackEXT calls: " + std::to_string(audioTrackCalls_));
        lines.push_back("SetVideoTrackEXT calls: " + std::to_string(videoTrackCalls_));
        lines.emplace_back();
        lines.push_back("This clip has only 1 video + 1 audio stream -- selecting track 0");
        lines.push_back("exercises the real API path with no throw, but no visible change.");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Video> video_;
    VideoPlayer player_;
    int audioTrackCalls_ = 0;
    int videoTrackCalls_ = 0;
};

} // namespace CnaExamples::Demos::Media::VideoDemos
