// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/Album.hpp"
#include "Microsoft/Xna/Framework/Media/Artist.hpp"
#include "Microsoft/Xna/Framework/Media/Genre.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "Microsoft/Xna/Framework/Media/SongCollection.hpp"

#include "Demos/Media/LibraryDemoScreen.hpp"

namespace CnaExamples::Demos::Media::MediaLibraryDemos {

using Microsoft::Xna::Framework::Media::Song;

// Every per-Song property the library populates from the file's own tags, for
// one selected song at a time. XNA's Song exposes Album/Artist/Genre; FNA omits
// them, which is why they were missing from CNA for a long time -- they are real
// back-references into the same Album/Artist/Genre objects the library's own
// collections hand out, not lookalike copies (the screen proves that by
// comparing pointers).
//
// The bundled library deliberately contains one untagged .wav next to five
// fully-tagged .ogg files, so the "no tags -> Name falls back to the filename
// stem, and Album/Artist/Genre stay null" path is visible rather than described.
class SongMetadataScreen : public LibraryDemoScreen {
public:
    SongMetadataScreen() : LibraryDemoScreen("MediaLibrary: Song Metadata") {}

protected:
    int ItemCount() const override {
        return Library() != nullptr ? (int)Library()->getSongsProperty()->getCountProperty() : 0;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!LoadError().empty()) {
            lines.push_back("MediaLibrary failed to build: " + LoadError());
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        const int count = ItemCount();
        lines.push_back("Up/Down: select a song   (" + std::to_string(count) + " indexed)");
        lines.emplace_back();

        auto* songs = Library()->getSongsProperty();
        const int visible = 6;
        const int start = WindowStart(visible);
        for (int i = start; i < count && i < start + visible; ++i) {
            Song* song = (*songs)[i];
            lines.push_back(RowPrefix(i, SelectedIndex()) + song->getNameProperty());
        }

        lines.emplace_back();
        if (count > 0) {
            Song* song = (*songs)[SelectedIndex()];
            auto* album  = song->getAlbumProperty();
            auto* artist = song->getArtistProperty();
            auto* genre  = song->getGenreProperty();

            const std::string untagged = "(null -- untagged)";
            lines.push_back("Artist: " + (artist ? artist->getNameProperty() : untagged));
            lines.push_back("Album:  " + (album ? album->getNameProperty() : untagged) +
                            "     Genre: " + (genre ? genre->getNameProperty() : untagged));
            lines.push_back("Duration " + FormatDuration(song->getDurationProperty()) +
                            " (container probe, no decode)   Track " +
                            std::to_string((int)song->getTrackNumberProperty()));
            lines.push_back("Rating " + std::to_string((int)song->getRatingProperty()) +
                            "  IsRated " + (song->getIsRatedProperty() ? "true" : "false") +
                            "   PlayCount " + std::to_string((int)song->getPlayCountProperty()) +
                            "  IsProtected " + (song->getIsProtectedProperty() ? "true" : "false"));
            lines.push_back("Handle: " + ShortenPath(song->getHandle()));
            lines.push_back(std::string("Song.Album is the same object as Albums[...]: ") +
                            (AlbumIsShared(album) ? "yes" : (album ? "no" : "n/a (null)")));
        }

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    // Proves Song::Album points into the library's own Album objects rather than
    // at a private duplicate -- a real distinction, since MediaPlayer::Play()
    // does operate on a duplicated Song.
    bool AlbumIsShared(const Microsoft::Xna::Framework::Media::Album* album) const {
        if (album == nullptr) return false;
        auto* albums = Library()->getAlbumsProperty();
        for (int i = 0; i < (int)albums->getCountProperty(); ++i) {
            if ((*albums)[i] == album) return true;
        }
        return false;
    }

    // Absolute paths are long enough to run off the screen; the last two
    // components are what actually identifies the file to a reader.
    static std::string ShortenPath(const std::string& path) {
        const std::size_t last = path.find_last_of("/\\");
        if (last == std::string::npos || last == 0) return path;
        const std::size_t prev = path.find_last_of("/\\", last - 1);
        return prev == std::string::npos ? path : "..." + path.substr(prev);
    }
};

} // namespace CnaExamples::Demos::Media::MediaLibraryDemos
