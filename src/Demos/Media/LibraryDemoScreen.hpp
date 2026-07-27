// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Media/MediaLibrary.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Media/MediaDemoHelpers.hpp"

namespace CnaExamples::Demos::Media {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Media::MediaLibrary;

// Shared base for every demo that browses a real MediaLibrary. Handles the two
// things all of them need identically:
//
//   1. Building the library against the bundled synthetic roots (see
//      ScopedDemoMediaRoots in MediaDemoHelpers.hpp) and tearing it down in the
//      right order, so the process-global root override never outlives the
//      library that read it.
//   2. Up/Down selection over a list of N items, with a windowed view so a list
//      longer than the screen still scrolls -- the same behaviour MenuScreen
//      gives the navigation menus, but for a demo's own content list.
//
// Subclasses implement ItemCount()/OnDemoDraw() and read SelectedIndex().
class LibraryDemoScreen : public DemoScreen {
public:
    explicit LibraryDemoScreen(std::string title) : DemoScreen(std::move(title)) {}

    void OnDemoLoad() override {
        const auto roots = Roots();
        roots_ = std::make_unique<ScopedDemoMediaRoots>(roots.first, roots.second);
        try {
            library_ = std::make_unique<MediaLibrary>();
        } catch (const std::exception& ex) {
            loadError_ = ex.what();
        }
        OnLibraryLoaded();
    }

    void OnDemoUnload() override {
        library_.reset();   // must go first: it holds paths resolved from the override
        roots_.reset();
    }

protected:
    // (music root, picture root) the library is built against. Defaults to the
    // read-only bundled demo library; overridden by the one demo that writes.
    virtual std::pair<std::string, std::string> Roots() const {
        return {DemoMusicRoot(), DemoPictureRoot()};
    }

    // Called once, after library_ is built (or after loadError_ is set).
    virtual void OnLibraryLoaded() {}

    // Rebuilds the library in place, picking up anything that changed on disk
    // since it was last indexed (MediaLibrary snapshots at construction).
    void RebuildLibrary() {
        library_.reset();
        loadError_.clear();
        try {
            library_ = std::make_unique<MediaLibrary>();
        } catch (const std::exception& ex) {
            loadError_ = ex.what();
        }
    }

    // Number of selectable items; 0 disables selection entirely.
    virtual int ItemCount() const { return 0; }

    void OnDemoInput(InputState& input) override {
        const int count = ItemCount();
        if (count <= 0) return;

        int sel = SelectedIndex();
        if (input.IsMenuUp(ControllingPlayer()))   sel = (sel + count - 1) % count;
        if (input.IsMenuDown(ControllingPlayer())) sel = (sel + 1) % count;
        selected_ = sel;
    }

    MediaLibrary* Library() const { return library_.get(); }
    const std::string& LoadError() const { return loadError_; }

    // Clamped on read rather than only on write: a screen that switches between
    // lists of different lengths (Albums/Artists/Genres) would otherwise carry a
    // now-out-of-range index into the shorter list.
    int SelectedIndex() const {
        const int count = ItemCount();
        if (count <= 0) return 0;
        return selected_ < count ? selected_ : count - 1;
    }

    void ResetSelection() { selected_ = 0; }

    // First index of the visible window for a list showing `visible` rows at a
    // time, kept so the selection is always inside it.
    int WindowStart(int visible) const {
        const int count = ItemCount();
        if (count <= visible) return 0;
        int start = SelectedIndex() - visible / 2;
        if (start < 0) start = 0;
        if (start > count - visible) start = count - visible;
        return start;
    }

    // "> " on the selected row, two spaces otherwise, so the caret costs no
    // extra layout and stays readable in the monospaced-ish menu font.
    static std::string RowPrefix(int index, int selected) {
        return index == selected ? "> " : "  ";
    }

private:
    std::unique_ptr<ScopedDemoMediaRoots> roots_;
    std::unique_ptr<MediaLibrary> library_;
    std::string loadError_;
    int selected_ = 0;
};

} // namespace CnaExamples::Demos::Media
