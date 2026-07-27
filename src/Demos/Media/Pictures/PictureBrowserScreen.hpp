// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Media/Picture.hpp"
#include "Microsoft/Xna/Framework/Media/PictureAlbum.hpp"
#include "Microsoft/Xna/Framework/Media/PictureCollection.hpp"
#include "System/IO/FileStream.hpp"

#include "Demos/Media/LibraryDemoScreen.hpp"

namespace CnaExamples::Demos::Media::PictureDemos {

using namespace CnaExamples::Demos::Media;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Media::Picture;

// MediaLibrary::Pictures is the flattened list of every image found anywhere
// under the Pictures root -- .png/.jpg/.jpeg/.bmp, at any depth. Each Picture
// carries the metadata the indexer read from the file itself (real pixel
// Width/Height, the filesystem timestamp as Date) plus a back-reference to the
// PictureAlbum -- i.e. the directory -- it was found in.
//
// XNA's Picture has no way to hand back pixels; the image is opened separately.
// This screen does that with the picture's own NOXNA token (its path) and
// Texture2D::FromStream, and draws the result, so the metadata on the left is
// visibly the metadata of the image on the right.
class PictureBrowserScreen : public LibraryDemoScreen {
public:
    PictureBrowserScreen() : LibraryDemoScreen("Pictures: Browse") {}

    void UnloadContent() override {
        texture_.reset();
        LibraryDemoScreen::UnloadContent();
    }

protected:
    void OnLibraryLoaded() override { LoadSelectedTexture(); }

    int ItemCount() const override {
        return Library() != nullptr ? (int)Library()->getPicturesProperty()->getCountProperty() : 0;
    }

    void OnDemoInput(InputState& input) override {
        const int before = SelectedIndex();
        LibraryDemoScreen::OnDemoInput(input);
        if (SelectedIndex() != before) LoadSelectedTexture();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!LoadError().empty()) {
            lines.push_back("MediaLibrary failed to build: " + LoadError());
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        const int count = ItemCount();
        lines.push_back("Up/Down: select a picture   (" + std::to_string(count) + " indexed)");
        lines.emplace_back();

        auto* pictures = Library()->getPicturesProperty();
        const int visible = 6;
        const int start = WindowStart(visible);
        for (int i = start; i < count && i < start + visible; ++i) {
            lines.push_back(RowPrefix(i, SelectedIndex()) + (*pictures)[i]->getNameProperty());
        }

        if (count > 0) {
            Picture* p = (*pictures)[SelectedIndex()];
            lines.emplace_back();
            lines.push_back("Name:   " + p->getNameProperty());
            lines.push_back("Size:   " + std::to_string((int)p->getWidthProperty()) + " x " +
                            std::to_string((int)p->getHeightProperty()) + " px (read from the file itself)");
            lines.push_back("Album:  " + (p->getAlbumProperty() ? p->getAlbumProperty()->getNameProperty()
                                                                : std::string("(null)")));
            lines.push_back("Token:  " + CompactPath(p->getTokenEXT()));
            lines.push_back(std::string("Image:  ") +
                            (texture_.has_value() ? "decoded via Texture2D::FromStream, drawn on the right"
                                                  : ("could not be opened: " + textureError_)));
        }

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        if (texture_.has_value()) DrawPreview(sb);
    }

private:
    void DrawPreview(SpriteBatch& sb) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const int boxSize = 200;
        const int boxX = viewport.getWidthProperty() - boxSize - 40;
        const int boxY = 110;

        // Letterbox into a fixed square so a 320x240 and a 200x200 image both
        // stay undistorted and in the same place on screen.
        const float tw = (float)texture_->getWidthProperty();
        const float th = (float)texture_->getHeightProperty();
        const float scale = (tw > th) ? (float)boxSize / tw : (float)boxSize / th;
        const int dw = (int)(tw * scale);
        const int dh = (int)(th * scale);

        const Color tint = mul(Color::White, TransitionAlpha());
        FillRect(sb, Rectangle(boxX - 2, boxY - 2, boxSize + 4, boxSize + 4),
                 mul(Color(60, 60, 60), TransitionAlpha()));
        sb.Draw(*texture_,
                Rectangle(boxX + (boxSize - dw) / 2, boxY + (boxSize - dh) / 2, dw, dh),
                tint);
    }

    void LoadSelectedTexture() {
        texture_.reset();
        textureError_.clear();
        if (Library() == nullptr || ItemCount() == 0) return;

        Picture* p = (*Library()->getPicturesProperty())[SelectedIndex()];
        try {
            System::IO::FileStream stream(p->getTokenEXT());
            texture_.emplace(Texture2D::FromStream(GetScreenManager()->getGraphicsDeviceProperty(), stream));
        } catch (const std::exception& ex) {
            textureError_ = ex.what();
        }
    }

    std::optional<Texture2D> texture_;
    std::string textureError_;
};

} // namespace CnaExamples::Demos::Media::PictureDemos
