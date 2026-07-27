// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "Microsoft/Xna/Framework/Media/Playlist.hpp"
#include "Microsoft/Xna/Framework/Media/PlaylistCollection.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "Microsoft/Xna/Framework/Media/SongCollection.hpp"

#include "Demos/Media/LibraryDemoScreen.hpp"

namespace CnaExamples::Demos::Media::MediaLibraryDemos {

using Microsoft::Xna::Framework::Media::MediaPlayer;
using Microsoft::Xna::Framework::Media::Playlist;
using Microsoft::Xna::Framework::Media::Song;

// Playlists come from real .m3u/.m3u8 files found anywhere under the music root.
// The parser resolves each entry's (usually relative) path against the playlist's
// own directory and then matches it to an already-indexed Song -- so an entry
// pointing at a file the indexer skipped simply does not appear in the playlist,
// and Playlist::Duration only ever sums the songs that did resolve.
//
// The bundled Favourites.m3u deliberately lists its three tracks out of
// directory order, so the playlist's own ordering being preserved (rather than
// re-sorted into library order) is visible.
class PlaylistScreen : public LibraryDemoScreen {
public:
    PlaylistScreen() : LibraryDemoScreen("MediaLibrary: Playlists") {}

    void UnloadContent() override {
        // MediaPlayer is a process-global singleton; leaving a playlist running
        // after the screen is gone would bleed into the next demo.
        MediaPlayer::Stop();
        LibraryDemoScreen::UnloadContent();
    }

protected:
    int ItemCount() const override {
        return Library() != nullptr ? (int)Library()->getPlaylistsProperty()->getCountProperty() : 0;
    }

    void OnDemoInput(InputState& input) override {
        LibraryDemoScreen::OnDemoInput(input);
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex) && ItemCount() > 0) {
            Playlist* playlist = (*Library()->getPlaylistsProperty())[SelectedIndex()];
            auto* songs = playlist->getSongsProperty();
            if (songs->getCountProperty() > 0) {
                MediaPlayer::Play(*songs, 0);
                played_ = true;
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!LoadError().empty()) {
            lines.push_back("MediaLibrary failed to build: " + LoadError());
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        const int count = ItemCount();
        lines.push_back("Up/Down: select a playlist   Space/Enter/A/tap: play it through MediaPlayer");
        lines.push_back(std::to_string(count) + " playlist(s) parsed from .m3u/.m3u8 files under the music root");
        lines.emplace_back();

        if (count == 0) {
            lines.push_back("(no playlists found)");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        auto* playlists = Library()->getPlaylistsProperty();
        for (int i = 0; i < count; ++i) {
            Playlist* p = (*playlists)[i];
            lines.push_back(RowPrefix(i, SelectedIndex()) + p->getNameProperty() +
                            "  [" + std::to_string((int)p->getSongsProperty()->getCountProperty()) +
                            " songs, " + FormatDuration(p->getDurationProperty()) + "]");
        }

        Playlist* p = (*playlists)[SelectedIndex()];
        lines.emplace_back();
        lines.push_back("Name:     " + p->getNameProperty() + "   (the .m3u filename stem)");
        lines.push_back("Duration: " + FormatDuration(p->getDurationProperty()) +
                        "  (sum of the entries that resolved to an indexed song)");
        lines.push_back("Entries, in the playlist's own order:");
        auto* songs = p->getSongsProperty();
        for (int i = 0; i < (int)songs->getCountProperty(); ++i) {
            lines.push_back("  " + std::to_string(i + 1) + ". " + (*songs)[i]->getNameProperty() +
                            "  " + FormatDuration((*songs)[i]->getDurationProperty()));
        }

        lines.emplace_back();
        lines.push_back("MediaPlayer state: " + std::string(MediaStateName(MediaPlayer::getStateProperty())));
        if (played_) {
            const auto& queue = MediaPlayer::getQueueProperty();
            Song* active = queue.getActiveSongProperty();
            lines.push_back("Queue.ActiveSongIndex: " + std::to_string((int)queue.getActiveSongIndexProperty()));
            lines.push_back("Queue.ActiveSong: " + (active ? active->getNameProperty() : std::string("(none)")));
        }

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    bool played_ = false;
};

} // namespace CnaExamples::Demos::Media::MediaLibraryDemos
