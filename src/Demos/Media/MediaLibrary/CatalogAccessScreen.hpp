// SPDX-License-Identifier: MIT
#pragma once

#include <cstdio>
#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/AlbumCollection.hpp"
#include "Microsoft/Xna/Framework/Media/ArtistCollection.hpp"
#include "Microsoft/Xna/Framework/Media/GenreCollection.hpp"
#include "Microsoft/Xna/Framework/Media/MediaLibrary.hpp"
#include "Microsoft/Xna/Framework/Media/MediaSource.hpp"
#include "Microsoft/Xna/Framework/Media/PictureAlbum.hpp"
#include "Microsoft/Xna/Framework/Media/PictureCollection.hpp"
#include "Microsoft/Xna/Framework/Media/PlaylistCollection.hpp"
#include "Microsoft/Xna/Framework/Media/SongCollection.hpp"

#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Media/MediaDemoHelpers.hpp"

namespace CnaExamples::Demos::Media::MediaLibraryDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Media::MediaLibrary;
using Microsoft::Xna::Framework::Media::MediaSource;
using Microsoft::Xna::Framework::Media::MediaSourceType;

// MediaLibrary is a real, working catalog on this platform: it walks the
// Music/Pictures roots, probes every audio file's real duration, parses its
// tags, groups the results into Albums/Artists/Genres, parses .m3u playlists
// and builds a PictureAlbum tree. (It used to be a hard stub that threw
// "not implemented" from every accessor -- that changed when CNA's
// feature/media branch merged on 2026-07-18.)
//
// This screen builds the library twice on demand: once against the bundled
// synthetic library under Content/MediaLibraryDemo/ (deterministic, the mode
// every other MediaLibrary demo uses), and once against the real OS Music and
// Pictures folders, so what the unredirected API actually reports on this
// machine is visible too.
class CatalogAccessScreen : public DemoScreen {
public:
    CatalogAccessScreen() : DemoScreen("MediaLibrary: Catalog Access") {}

    void OnDemoLoad() override { Rebuild(); }

    void OnDemoUnload() override {
        library_.reset();
        roots_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            useDemoRoots_ = !useDemoRoots_;
            Rebuild();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: switch to the " +
                        std::string(useDemoRoots_ ? "real OS folders" : "bundled demo library"));
        lines.emplace_back();
        lines.push_back(std::string("Indexing: ") +
                        (useDemoRoots_ ? "bundled demo library (NOXNA root override)"
                                       : "the real OS Music/Pictures folders"));
        lines.push_back("  Music:    " + CompactPath(musicRoot_));
        lines.push_back("  Pictures: " + CompactPath(pictureRoot_));
        lines.emplace_back();

        if (!buildError_.empty()) {
            lines.push_back("MediaLibrary construction threw: " + buildError_);
        } else {
            lines.push_back("Songs " + std::to_string(counts_.songs) +
                            "   Albums " + std::to_string(counts_.albums) +
                            "   Artists " + std::to_string(counts_.artists) +
                            "   Genres " + std::to_string(counts_.genres) +
                            "   Playlists " + std::to_string(counts_.playlists));
            lines.push_back("Pictures " + std::to_string(counts_.pictures) +
                            "   SavedPictures " + std::to_string(counts_.savedPictures) +
                            "   root album: " + rootAlbumName_);
            lines.push_back("MediaSource: " + sourceName_ + " (" + sourceType_ + ")   IsDisposed: " +
                            (library_->getIsDisposedProperty() ? "true" : "false"));
        }

        lines.emplace_back();
        lines.push_back("Constructor guards, called live on every rebuild:");
        for (const auto& g : guardResults_) lines.push_back("  " + g);
        lines.emplace_back();
        lines.push_back("MediaSource::GetAvailableMediaSources(): " +
                        std::to_string(MediaSource::GetAvailableMediaSources().size()) + " source(s)");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    struct Counts {
        int songs = 0, albums = 0, artists = 0, genres = 0, playlists = 0;
        int pictures = 0, savedPictures = 0;
    };

    void Rebuild() {
        // Order matters: the override has to be installed before the constructor
        // runs, because MediaLibrary reads the roots once, in its constructor.
        library_.reset();
        roots_.reset();
        roots_ = std::make_unique<ScopedDemoMediaRoots>(useDemoRoots_);

        musicRoot_   = CNA::Internal::Media::MediaLibraryPaths::GetMusicRoot();
        pictureRoot_ = CNA::Internal::Media::MediaLibraryPaths::GetPictureRoot();

        buildError_.clear();
        counts_ = Counts{};
        rootAlbumName_ = "(none)";
        sourceName_.clear();
        sourceType_.clear();

        try {
            library_ = std::make_unique<MediaLibrary>();
            counts_.songs         = (int)library_->getSongsProperty()->getCountProperty();
            counts_.albums        = (int)library_->getAlbumsProperty()->getCountProperty();
            counts_.artists       = (int)library_->getArtistsProperty()->getCountProperty();
            counts_.genres        = (int)library_->getGenresProperty()->getCountProperty();
            counts_.playlists     = (int)library_->getPlaylistsProperty()->getCountProperty();
            counts_.pictures      = (int)library_->getPicturesProperty()->getCountProperty();
            counts_.savedPictures = (int)library_->getSavedPicturesProperty()->getCountProperty();

            if (auto* root = library_->getRootPictureAlbumProperty()) {
                rootAlbumName_ = root->getNameProperty();
            }
            if (auto* source = library_->getMediaSourceProperty()) {
                sourceName_ = source->getNameProperty();
                sourceType_ = MediaSourceTypeName(source->getMediaSourceTypeProperty());
            }
        } catch (const std::exception& ex) {
            buildError_ = ex.what();
        }

        RunConstructorGuards();
    }

    // MediaLibrary's source-taking constructor has two guards; only one of them
    // is reachable from consumer code at all. Both live calls below are real --
    // the messages come out of CNA itself, not from this screen.
    void RunConstructorGuards() {
        guardResults_.clear();

        try {
            MediaLibrary rejected(nullptr);
            guardResults_.emplace_back("MediaLibrary(nullptr): returned without throwing (unexpected)");
        } catch (const std::exception& ex) {
            guardResults_.emplace_back(std::string("MediaLibrary(nullptr): ") + ex.what());
        }

        // Round-tripping this library's own MediaSource back into the source-taking
        // constructor is the only way to reach that overload successfully: the second
        // guard (non-LocalDevice -> NotSupportedException) cannot be triggered from
        // outside CNA at all, because MediaSource's constructor is private with
        // `friend class MediaLibrary` and GetAvailableMediaSources() only ever reports
        // LocalDevice. That branch is real code, just unreachable from here -- noted
        // rather than faked with a staged failure.
        if (library_ != nullptr) {
            try {
                MediaLibrary copy(library_->getMediaSourceProperty());
                guardResults_.emplace_back(
                    "MediaLibrary(<this library's own LocalDevice source>): built, " +
                    std::to_string((int)copy.getSongsProperty()->getCountProperty()) + " song(s)");
            } catch (const std::exception& ex) {
                guardResults_.emplace_back(std::string("MediaLibrary(LocalDevice source): ") + ex.what());
            }
        }
        guardResults_.emplace_back(
            "MediaSourceType::WindowsMediaConnect -> NotSupportedException: unreachable "
            "(MediaSource's ctor is private)");
    }

    bool useDemoRoots_ = true;
    std::unique_ptr<ScopedDemoMediaRoots> roots_;
    std::unique_ptr<MediaLibrary> library_;
    std::string musicRoot_, pictureRoot_, buildError_, rootAlbumName_, sourceName_, sourceType_;
    std::vector<std::string> guardResults_;
    Counts counts_;
};

} // namespace CnaExamples::Demos::Media::MediaLibraryDemos
