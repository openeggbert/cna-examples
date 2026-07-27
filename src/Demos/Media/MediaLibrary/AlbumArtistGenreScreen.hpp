// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/Album.hpp"
#include "Microsoft/Xna/Framework/Media/AlbumCollection.hpp"
#include "Microsoft/Xna/Framework/Media/Artist.hpp"
#include "Microsoft/Xna/Framework/Media/ArtistCollection.hpp"
#include "Microsoft/Xna/Framework/Media/Genre.hpp"
#include "Microsoft/Xna/Framework/Media/GenreCollection.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "Microsoft/Xna/Framework/Media/SongCollection.hpp"

#include "Demos/Media/LibraryDemoScreen.hpp"

namespace CnaExamples::Demos::Media::MediaLibraryDemos {

using Microsoft::Xna::Framework::Media::Album;
using Microsoft::Xna::Framework::Media::Artist;
using Microsoft::Xna::Framework::Media::Genre;

// Album, Artist and Genre are grouping views the library derives from the songs'
// own tags -- it never reads a "grouping" from disk. This screen shows all three
// side by side over the same song set, so the derivation is visible: the same
// five tagged songs produce 3 albums, 2 artists and 2 genres, and the untagged
// sixth song appears in none of them.
//
// Album::HasArt is a real per-album difference here, not a constant: the bundled
// library gives exactly one album a cover.png and leaves the others bare.
class AlbumArtistGenreScreen : public LibraryDemoScreen {
public:
    AlbumArtistGenreScreen() : LibraryDemoScreen("MediaLibrary: Album / Artist / Genre") {}

protected:
    void OnDemoInput(InputState& input) override {
        LibraryDemoScreen::OnDemoInput(input);
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            view_ = (view_ + 1) % 3;
        }
    }

    int ItemCount() const override {
        if (Library() == nullptr) return 0;
        switch (view_) {
            case 0: return (int)Library()->getAlbumsProperty()->getCountProperty();
            case 1: return (int)Library()->getArtistsProperty()->getCountProperty();
            default: return (int)Library()->getGenresProperty()->getCountProperty();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!LoadError().empty()) {
            lines.push_back("MediaLibrary failed to build: " + LoadError());
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        static const char* kViewNames[] = {"Albums", "Artists", "Genres"};
        lines.push_back("Space/Enter/A/tap: switch view    Up/Down: select");
        lines.push_back(std::string("View: ") + kViewNames[view_] +
                        "   (" + std::to_string(ItemCount()) + " in library)");
        lines.emplace_back();

        switch (view_) {
            case 0: DrawAlbums(lines);  break;
            case 1: DrawArtists(lines); break;
            default: DrawGenres(lines); break;
        }

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr int kVisible = 6;

    void DrawAlbums(std::vector<std::string>& lines) {
        auto* albums = Library()->getAlbumsProperty();
        const int count = (int)albums->getCountProperty();
        const int start = WindowStart(kVisible);
        for (int i = start; i < count && i < start + kVisible; ++i) {
            Album* a = (*albums)[i];
            lines.push_back(RowPrefix(i, SelectedIndex()) + a->getNameProperty() +
                            "  [" + std::to_string((int)a->getSongsProperty()->getCountProperty()) + " songs]");
        }
        if (count == 0) return;

        Album* a = (*albums)[SelectedIndex()];
        lines.emplace_back();
        lines.push_back("Name:     " + a->getNameProperty());
        lines.push_back("Artist:   " + (a->getArtistProperty() ? a->getArtistProperty()->getNameProperty()
                                                                : std::string("(null)")));
        lines.push_back("Genre:    " + (a->getGenreProperty() ? a->getGenreProperty()->getNameProperty()
                                                              : std::string("(null)")));
        lines.push_back("Duration: " + FormatDuration(a->getDurationProperty()) + "  (sum of member songs)");
        lines.push_back("HasArt:   " + std::string(a->getHasArtProperty() ? "true  (cover.png next to the tracks)"
                                                                          : "false (no folder image, no embedded art)"));
        AppendSongs(lines, a->getSongsProperty());
    }

    void DrawArtists(std::vector<std::string>& lines) {
        auto* artists = Library()->getArtistsProperty();
        const int count = (int)artists->getCountProperty();
        const int start = WindowStart(kVisible);
        for (int i = start; i < count && i < start + kVisible; ++i) {
            Artist* a = (*artists)[i];
            lines.push_back(RowPrefix(i, SelectedIndex()) + a->getNameProperty() +
                            "  [" + std::to_string((int)a->getAlbumsProperty()->getCountProperty()) + " albums, " +
                            std::to_string((int)a->getSongsProperty()->getCountProperty()) + " songs]");
        }
        if (count == 0) return;

        Artist* a = (*artists)[SelectedIndex()];
        lines.emplace_back();
        lines.push_back("Name: " + a->getNameProperty());
        lines.push_back("Albums:");
        auto* albums = a->getAlbumsProperty();
        for (int i = 0; i < (int)albums->getCountProperty(); ++i) {
            lines.push_back("  " + (*albums)[i]->getNameProperty());
        }
        AppendSongs(lines, a->getSongsProperty());
    }

    void DrawGenres(std::vector<std::string>& lines) {
        auto* genres = Library()->getGenresProperty();
        const int count = (int)genres->getCountProperty();
        const int start = WindowStart(kVisible);
        for (int i = start; i < count && i < start + kVisible; ++i) {
            Genre* g = (*genres)[i];
            lines.push_back(RowPrefix(i, SelectedIndex()) + g->getNameProperty() +
                            "  [" + std::to_string((int)g->getAlbumsProperty()->getCountProperty()) + " albums, " +
                            std::to_string((int)g->getSongsProperty()->getCountProperty()) + " songs]");
        }
        if (count == 0) return;

        Genre* g = (*genres)[SelectedIndex()];
        lines.emplace_back();
        lines.push_back("Name: " + g->getNameProperty());
        AppendSongs(lines, g->getSongsProperty());
    }

    static void AppendSongs(std::vector<std::string>& lines,
                            Microsoft::Xna::Framework::Media::SongCollection* songs) {
        lines.push_back("Songs:");
        const int count = (int)songs->getCountProperty();
        for (int i = 0; i < count && i < 5; ++i) {
            lines.push_back("  " + (*songs)[i]->getNameProperty() +
                            "  " + FormatDuration((*songs)[i]->getDurationProperty()));
        }
        if (count > 5) lines.push_back("  ... and " + std::to_string(count - 5) + " more");
    }

    int view_ = 0;
};

} // namespace CnaExamples::Demos::Media::MediaLibraryDemos
