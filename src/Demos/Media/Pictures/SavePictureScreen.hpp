// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <exception>
#include <filesystem>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/Picture.hpp"
#include "Microsoft/Xna/Framework/Media/PictureAlbum.hpp"
#include "Microsoft/Xna/Framework/Media/PictureCollection.hpp"

#include "Demos/Media/LibraryDemoScreen.hpp"

namespace CnaExamples::Demos::Media::PictureDemos {

using namespace CnaExamples::Demos::Media;
using Microsoft::Xna::Framework::Media::Picture;

// The one writing member of the whole Media area: SavePicture(name, bytes)
// drops a real file into a "Saved Pictures" folder under the Pictures root and
// hands back a live Picture describing it.
//
// Two behaviours worth watching here, both real:
//   - "Saved Pictures" is created lazily. Merely constructing (or browsing) a
//     MediaLibrary never creates it; the first SavePicture() call does.
//   - MediaLibrary snapshots the filesystem at construction. A picture saved
//     through *this* library instance appears in SavedPictures immediately, but
//     the flat Pictures collection is the snapshot from construction time --
//     rebuilding the library is what makes the new file show up there.
//
// It writes to a temp directory rather than the bundled Content/ folder, so
// running the demo never leaves new files in the checked-out repository.
class SavePictureScreen : public LibraryDemoScreen {
public:
    SavePictureScreen() : LibraryDemoScreen("Pictures: SavePicture") {}

public:
    void LoadContent() override {
        // The scratch root has to exist before the library indexes it, not after:
        // MediaLibrary snapshots the filesystem in its constructor, so creating
        // the directory later would leave this screen's first run looking at a
        // root that did not exist yet.
        std::error_code ec;
        std::filesystem::create_directories(ScratchPictureRoot(), ec);
        savedDirExistsAtStart_ =
            std::filesystem::exists(std::filesystem::path(ScratchPictureRoot()) / "Saved Pictures", ec);

        LibraryDemoScreen::LoadContent();
    }

protected:
    std::pair<std::string, std::string> Roots() const override {
        return {DemoMusicRoot(), ScratchPictureRoot()};
    }

    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (!input.IsMenuSelect(ControllingPlayer(), playerIndex) || Library() == nullptr) return;

        const std::string name = "demo_" + std::to_string(saveCount_ + 1);
        try {
            Picture* saved = Library()->SavePicture(name, MakeBmp(64, 64, saveCount_));
            saveCount_++;
            if (saved != nullptr) {
                lastResult_ = saved->getNameProperty() + "  " +
                              std::to_string((int)saved->getWidthProperty()) + "x" +
                              std::to_string((int)saved->getHeightProperty()) +
                              "  album=" + (saved->getAlbumProperty()
                                                ? saved->getAlbumProperty()->getNameProperty()
                                                : std::string("(null)"));
                lastToken_ = saved->getTokenEXT();
            } else {
                lastResult_ = "SavePicture returned nullptr";
            }
        } catch (const std::exception& ex) {
            lastResult_ = std::string("threw: ") + ex.what();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!LoadError().empty()) {
            lines.push_back("MediaLibrary failed to build: " + LoadError());
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        std::error_code ec;
        const std::filesystem::path savedDir = std::filesystem::path(ScratchPictureRoot()) / "Saved Pictures";
        const bool savedDirExistsNow = std::filesystem::exists(savedDir, ec);

        lines.push_back("Space/Enter/A/tap: save a freshly generated 64x64 BMP into the library");
        lines.emplace_back();
        lines.push_back("Pictures root: " + ScratchPictureRoot());
        lines.push_back("\"Saved Pictures\" folder existed before any save: " +
                        std::string(savedDirExistsAtStart_ ? "yes (left over from a previous run)" : "no"));
        lines.push_back("\"Saved Pictures\" folder exists now: " +
                        std::string(savedDirExistsNow ? "yes" : "no -- created lazily on first save"));
        lines.emplace_back();
        lines.push_back("SavePicture calls this session: " + std::to_string(saveCount_));
        lines.push_back("Last result: " + (lastResult_.empty() ? std::string("(none yet)") : lastResult_));
        if (!lastToken_.empty()) lines.push_back("Last token: " + lastToken_);
        lines.emplace_back();
        lines.push_back("MediaLibrary.SavedPictures: " +
                        std::to_string((int)Library()->getSavedPicturesProperty()->getCountProperty()) +
                        "   (snapshot taken when this library was built)");
        lines.push_back("MediaLibrary.Pictures:      " +
                        std::to_string((int)Library()->getPicturesProperty()->getCountProperty()));
        lines.push_back("Files actually on disk in Saved Pictures: " + std::to_string(CountFiles(savedDir)));
        lines.emplace_back();
        lines.push_back("The counts above differ on purpose: a MediaLibrary indexes the filesystem");
        lines.push_back("once, at construction. Re-enter this demo to see a freshly built one.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static int CountFiles(const std::filesystem::path& dir) {
        std::error_code ec;
        if (!std::filesystem::exists(dir, ec)) return 0;
        int n = 0;
        for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
            if (ec) break;
            if (entry.is_regular_file(ec) && !ec) ++n;
        }
        return n;
    }

    // A real, minimal 24-bit BMP built by hand -- no image library and no bundled
    // asset, matching how the Audio area generates its tones instead of shipping
    // a WAV. Each call produces a visibly different image so successive saves are
    // distinguishable.
    static std::vector<std::uint8_t> MakeBmp(int width, int height, int variant) {
        const int rowStride = ((width * 3 + 3) / 4) * 4;
        const int pixelBytes = rowStride * height;
        const int headerBytes = 54;
        const int fileBytes = headerBytes + pixelBytes;

        std::vector<std::uint8_t> bmp(fileBytes, 0);
        auto put16 = [&bmp](int at, int v) {
            bmp[at]     = (std::uint8_t)(v & 0xFF);
            bmp[at + 1] = (std::uint8_t)((v >> 8) & 0xFF);
        };
        auto put32 = [&bmp](int at, int v) {
            bmp[at]     = (std::uint8_t)(v & 0xFF);
            bmp[at + 1] = (std::uint8_t)((v >> 8) & 0xFF);
            bmp[at + 2] = (std::uint8_t)((v >> 16) & 0xFF);
            bmp[at + 3] = (std::uint8_t)((v >> 24) & 0xFF);
        };

        bmp[0] = 'B'; bmp[1] = 'M';
        put32(2, fileBytes);
        put32(10, headerBytes);
        put32(14, 40);              // BITMAPINFOHEADER size
        put32(18, width);
        put32(22, height);
        put16(26, 1);               // planes
        put16(28, 24);              // bits per pixel
        put32(34, pixelBytes);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const int at = headerBytes + y * rowStride + x * 3;
                bmp[at]     = (std::uint8_t)(x * 4 + variant * 40);   // blue
                bmp[at + 1] = (std::uint8_t)(y * 4);                  // green
                bmp[at + 2] = (std::uint8_t)(255 - variant * 40);     // red
            }
        }
        return bmp;
    }

    bool savedDirExistsAtStart_ = false;
    int saveCount_ = 0;
    std::string lastResult_, lastToken_;
};

} // namespace CnaExamples::Demos::Media::PictureDemos
