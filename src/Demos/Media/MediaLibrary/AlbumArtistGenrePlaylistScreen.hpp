// SPDX-License-Identifier: MIT
#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/MediaLibrary.hpp"
#include "Microsoft/Xna/Framework/Media/MediaSourceType.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Media::MediaLibraryDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Media::MediaLibrary;
using Microsoft::Xna::Framework::Media::MediaSourceType;

// Album/Artist/Genre/Playlist/Picture/PictureAlbum (+ every *Collection
// wrapping them) are all unreachable on this platform, not just empty: every
// one of them has a private constructor, and MediaLibrary -- their only
// possible producer -- throws on every accessor that could ever hand one
// back (see MediaLibraryScreen). There is no live instance of any of these
// types to demonstrate, so this screen narrates that fact plus the one
// remaining live, callable, throw-worth-showing entry point:
// MediaLibrary(MediaSource*) -- which needs a real MediaSource* to call at
// all, and MediaSource itself is equally unreachable (private constructor,
// its only producer -- GetAvailableMediaSources() -- always returns empty)
// -- so this screen calls it with nullptr instead, still a real call into
// real code that really throws, not a fabricated demonstration.
class AlbumArtistGenrePlaylistScreen : public DemoScreen {
public:
    AlbumArtistGenrePlaylistScreen() : DemoScreen("MediaLibrary: Album/Artist/Genre/Playlist") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            attempts_++;
            try {
                MediaLibrary lib(nullptr);
                (void)lib;
                lastResult_ = "MediaLibrary(nullptr) returned without throwing (unexpected)";
            } catch (const std::runtime_error& ex) {
                lastResult_ = std::string("MediaLibrary(nullptr) threw: ") + ex.what();
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Album/Artist/Genre/Playlist/Picture: private constructors, and their only");
        lines.push_back("producer (MediaLibrary) throws on every accessor -- no live instance of");
        lines.push_back("any of these types is reachable on this platform.");
        lines.emplace_back();
        lines.push_back("MediaSourceType::LocalDevice = " + std::to_string((int)MediaSourceType::LocalDevice));
        lines.push_back("MediaSourceType::WindowsMediaConnect = " +
                         std::to_string((int)MediaSourceType::WindowsMediaConnect));
        lines.emplace_back();
        lines.push_back("Space/Enter/A/tap: MediaLibrary(nullptr) -- a real call, real throw");
        lines.push_back("Attempts: " + std::to_string(attempts_));
        if (!lastResult_.empty()) lines.push_back(lastResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int attempts_ = 0;
    std::string lastResult_;
};

} // namespace CnaExamples::Demos::Media::MediaLibraryDemos
