// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/Picture.hpp"
#include "Microsoft/Xna/Framework/Media/PictureAlbum.hpp"
#include "Microsoft/Xna/Framework/Media/PictureAlbumCollection.hpp"
#include "Microsoft/Xna/Framework/Media/PictureCollection.hpp"

#include "Demos/Media/LibraryDemoScreen.hpp"

namespace CnaExamples::Demos::Media::PictureDemos {

using namespace CnaExamples::Demos::Media;
using Microsoft::Xna::Framework::Media::PictureAlbum;

// The PictureAlbum tree mirrors the Pictures directory tree one-for-one: one
// album per directory, Parent pointing back up, Albums holding the child
// directories and Pictures holding the images in that directory only (not
// recursively -- MediaLibrary::Pictures is the recursive flat view).
//
// This screen walks the real tree from RootPictureAlbum down, and lets the user
// descend into a child album and back out, so Parent/Albums/Pictures are all
// exercised as navigation rather than printed as counts.
class PictureAlbumTreeScreen : public LibraryDemoScreen {
public:
    PictureAlbumTreeScreen() : LibraryDemoScreen("Pictures: Album Tree") {}

protected:
    void OnLibraryLoaded() override {
        if (Library() != nullptr) current_ = Library()->getRootPictureAlbumProperty();
    }

    int ItemCount() const override {
        return current_ != nullptr ? (int)current_->getAlbumsProperty()->getCountProperty() : 0;
    }

    void OnDemoInput(InputState& input) override {
        LibraryDemoScreen::OnDemoInput(input);
        PlayerIndex playerIndex;
        if (!input.IsMenuSelect(ControllingPlayer(), playerIndex) || current_ == nullptr) return;

        // Descend into the selected child album; if this album has no children,
        // go back up instead, so one button covers the whole tree walk. (Esc
        // still leaves the demo entirely -- DemoScreen handles that.)
        if (ItemCount() > 0) {
            current_ = (*current_->getAlbumsProperty())[SelectedIndex()];
        } else if (current_->getParentProperty() != nullptr) {
            current_ = current_->getParentProperty();
        }
        ResetSelection();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!LoadError().empty()) {
            lines.push_back("MediaLibrary failed to build: " + LoadError());
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }
        if (current_ == nullptr) {
            lines.push_back("RootPictureAlbum is null -- the Pictures root does not exist or is empty.");
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Up/Down: select a child album   Space/Enter/A/tap: descend (or go up at a leaf)");
        lines.push_back("Path: " + PathOf(current_));
        lines.emplace_back();
        lines.push_back("Album:  " + current_->getNameProperty());
        lines.push_back("Parent: " + (current_->getParentProperty()
                                          ? current_->getParentProperty()->getNameProperty()
                                          : std::string("(null -- this is the root)")));
        lines.emplace_back();

        auto* children = current_->getAlbumsProperty();
        const int childCount = (int)children->getCountProperty();
        lines.push_back("Albums (" + std::to_string(childCount) + "):");
        for (int i = 0; i < childCount; ++i) {
            PictureAlbum* child = (*children)[i];
            lines.push_back(RowPrefix(i, SelectedIndex()) + child->getNameProperty() +
                            "  [" + std::to_string((int)child->getAlbumsProperty()->getCountProperty()) +
                            " albums, " + std::to_string((int)child->getPicturesProperty()->getCountProperty()) +
                            " pictures]");
        }
        if (childCount == 0) lines.push_back("  (none -- leaf album)");

        auto* pictures = current_->getPicturesProperty();
        lines.emplace_back();
        lines.push_back("Pictures in this album only (" +
                        std::to_string((int)pictures->getCountProperty()) + "):");
        for (int i = 0; i < (int)pictures->getCountProperty(); ++i) {
            lines.push_back("  " + (*pictures)[i]->getNameProperty() + "  " +
                            std::to_string((int)(*pictures)[i]->getWidthProperty()) + "x" +
                            std::to_string((int)(*pictures)[i]->getHeightProperty()));
        }
        if (pictures->getCountProperty() == 0) lines.push_back("  (none)");

        lines.emplace_back();
        lines.push_back("MediaLibrary.Pictures (recursive, whole tree): " +
                        std::to_string((int)Library()->getPicturesProperty()->getCountProperty()));

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    // Rebuilt from Parent each frame rather than tracked in a stack, so it can
    // never disagree with where the tree actually says we are.
    static std::string PathOf(const PictureAlbum* album) {
        std::vector<std::string> parts;
        for (const PictureAlbum* a = album; a != nullptr; a = a->getParentProperty()) {
            parts.push_back(a->getNameProperty());
        }
        std::string path;
        for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
            if (!path.empty()) path += " / ";
            path += *it;
        }
        return path;
    }

    PictureAlbum* current_ = nullptr;
};

} // namespace CnaExamples::Demos::Media::PictureDemos
