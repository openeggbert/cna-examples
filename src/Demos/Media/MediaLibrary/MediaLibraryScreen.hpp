// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/MediaLibrary.hpp"
#include "Microsoft/Xna/Framework/Media/MediaSource.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Media::MediaLibraryDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Media::MediaLibrary;
using Microsoft::Xna::Framework::Media::MediaSource;

// Demonstrates MediaLibrary -- a hard stub on this platform, not a
// gracefully-empty one: only the default (parameterless) constructor
// succeeds, and every single accessor below (Songs/Albums/Artists/Genres/
// Playlists) throws std::runtime_error("not implemented"). There is no code
// path on this platform that enumerates the OS media catalog. By contrast,
// MediaSource::GetAvailableMediaSources() is real and callable -- it just
// always reports zero sources, without throwing.
class MediaLibraryScreen : public DemoScreen {
public:
    MediaLibraryScreen() : DemoScreen("MediaLibrary: Catalog Access") {}

    void LoadContent() override {
        library_.emplace();
    }

    void UnloadContent() override {
        // Deliberately not calling library_->Dispose() here: it throws too (see
        // OnDemoDraw's note) -- UnloadContent() must not throw, so this just lets
        // the optional's destructor run instead, which needs no real cleanup since
        // a default-constructed MediaLibrary holds no resources.
        library_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            results_.clear();
            TryAccessor("Songs", [this] { return library_->getSongsProperty(); });
            TryAccessor("Albums", [this] { return library_->getAlbumsProperty(); });
            TryAccessor("Artists", [this] { return library_->getArtistsProperty(); });
            TryAccessor("Genres", [this] { return library_->getGenresProperty(); });
            TryAccessor("Playlists", [this] { return library_->getPlaylistsProperty(); });
            TryAccessor("Dispose", [this] { library_->Dispose(); return nullptr; });
            attempts_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: call every MediaLibrary accessor and catch the throw");
        lines.emplace_back();
        lines.push_back("MediaLibrary() default ctor: succeeded (only ctor that doesn't throw)");
        lines.push_back("Attempts: " + std::to_string(attempts_));
        for (const auto& r : results_) lines.push_back("  " + r);
        lines.emplace_back();
        lines.push_back("MediaSource::GetAvailableMediaSources().size(): " +
                         std::to_string(MediaSource::GetAvailableMediaSources().size()) + " (real, never throws)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    template <typename Fn>
    void TryAccessor(const char* name, Fn&& fn) {
        try {
            (void)fn();
            results_.push_back(std::string(name) + ": returned without throwing (unexpected)");
        } catch (const std::runtime_error& ex) {
            results_.push_back(std::string(name) + ": threw " + ex.what());
        }
    }

    std::optional<MediaLibrary> library_;
    std::vector<std::string> results_;
    int attempts_ = 0;
};

} // namespace CnaExamples::Demos::Media::MediaLibraryDemos
