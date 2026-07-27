// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture3D.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics3D::TexturesAndQueriesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Texture3D;

// A Texture3D is a volume: width x height x DEPTH, addressed by three
// coordinates. SpriteBatch cannot draw one -- it only knows how to sample a 2D
// texture -- so this screen does what a volume texture is actually for at the
// data level, and then shows the result by reading each Z slice back out and
// blitting it into an ordinary Texture2D.
//
// That readback is the point rather than a workaround. The screen makes a
// claim, so it verifies the claim: every voxel written is read back, compared
// against what it should be, and the result drives the swatch at the bottom.
// A screen that merely rendered without throwing would prove nothing.
//
// Two writes are shown, because they are different APIs:
//   * SetData(data, count)                     -- the whole volume at once
//   * SetData(level, l,t,r,b,front,back, ...)  -- one axis-aligned sub-box
// The sub-box overwrite is why the middle slices have a red core and the outer
// ones do not.
class Texture3DVolumeScreen : public DemoScreen {
public:
    Texture3DVolumeScreen() : DemoScreen("Texture3D: Volume & Sub-Box") {}

    void OnDemoLoad() override {
        try {
            BuildVolume();
        } catch (const std::exception& ex) {
            error_ = ex.what();
        }
    }

    void OnDemoUnload() override {
        slices_.clear();
        volume_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        if (!error_.empty()) {
            lines.push_back("Texture3D is not available on this backend:");
            lines.push_back("  " + error_);
            lines.emplace_back();
            lines.push_back("A volume texture needs a 3D-capable device. The 2D-only backends");
            lines.push_back("(SDL_RENDERER, DX3, CANVAS) refuse it rather than pretending.");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);
            return;
        }

        lines.push_back("Texture3D " + std::to_string(kWidth) + "x" + std::to_string(kHeight) +
                        "x" + std::to_string(kDepth) + ", SurfaceFormat::Color -- " +
                        std::to_string(kWidth * kHeight * kDepth) + " voxels.");
        lines.emplace_back();
        lines.push_back("Written twice: the whole volume as an RGB gradient (red = X, green = Y,");
        lines.push_back("blue = Z), then an axis-aligned sub-box overwritten solid red with");
        lines.push_back("SetData(level, left, top, right, bottom, front, back, ...).");
        lines.push_back("The box spans Z " + std::to_string(kBoxFront) + ".." +
                        std::to_string(kBoxBack - 1) + ", so only those slices carry a red core.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        // One Texture2D per Z slice, read back out of the volume.
        const float top = end.Y + 18.0f;
        for (std::size_t i = 0; i < slices_.size(); ++i) {
            const float x = 40.0f + (float)i * (kSliceDraw + 8.0f);
            sb.Draw(*slices_[i], Rectangle((int)x, (int)top, (int)kSliceDraw, (int)kSliceDraw), tint);
            sb.DrawString(font, "z=" + std::to_string(i), Vector2(x + 4.0f, top + kSliceDraw + 4.0f), tint);
        }

        // The verification result, as a swatch a pixel probe can assert on.
        const float swatchY = top + kSliceDraw + 40.0f;
        FillRect(sb, Rectangle(40, (int)swatchY, 24, 24),
                 mul(roundTripOk_ ? Color(40, 200, 90, 255) : Color(220, 60, 60, 255),
                     TransitionAlpha()));
        sb.DrawString(font,
                      roundTripOk_
                          ? "GetData round trip verified: all " + std::to_string(checked_) +
                                " voxels read back exactly as written."
                          : "GetData round trip FAILED at voxel " + failureDetail_,
                      Vector2(76.0f, swatchY), tint);
    }

private:
    static constexpr int kWidth = 32;
    static constexpr int kHeight = 32;
    static constexpr int kDepth = 8;
    static constexpr int kBoxFront = 3;
    static constexpr int kBoxBack = 6;   // exclusive
    static constexpr int kBoxLeft = 8;
    static constexpr int kBoxTop = 8;
    static constexpr int kBoxRight = 24; // exclusive
    static constexpr int kBoxBottom = 24;
    static constexpr float kSliceDraw = 96.0f;

    // What voxel (x,y,z) should hold after both writes.
    static Color Expected(int x, int y, int z) {
        const bool inBox = x >= kBoxLeft && x < kBoxRight && y >= kBoxTop && y < kBoxBottom &&
                           z >= kBoxFront && z < kBoxBack;
        if (inBox) return Color(255, 0, 0, 255);
        return Gradient(x, y, z);
    }

    static Color Gradient(int x, int y, int z) {
        return Color((SharpRuntime::bytecs)(x * 8), (SharpRuntime::bytecs)(y * 8),
                     (SharpRuntime::bytecs)(z * 32), (SharpRuntime::bytecs)255);
    }

    void BuildVolume() {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        volume_.emplace(device, kWidth, kHeight, kDepth, false, SurfaceFormat::Color);

        // 1. The whole volume at once. Data is Z-major: all of slice 0, then slice 1...
        std::vector<Color> voxels((std::size_t)kWidth * kHeight * kDepth, Color::Black);
        for (int z = 0; z < kDepth; ++z) {
            for (int y = 0; y < kHeight; ++y) {
                for (int x = 0; x < kWidth; ++x) {
                    voxels[Index(x, y, z)] = Gradient(x, y, z);
                }
            }
        }
        volume_->SetData(voxels.data(), (int)voxels.size());

        // 2. One axis-aligned sub-box, solid red. Its data is tightly packed for
        //    the box's own extents, NOT for the volume's -- a common trap.
        const int boxW = kBoxRight - kBoxLeft;
        const int boxH = kBoxBottom - kBoxTop;
        const int boxD = kBoxBack - kBoxFront;
        std::vector<Color> box((std::size_t)boxW * boxH * boxD, Color(255, 0, 0, 255));
        volume_->SetData(0, kBoxLeft, kBoxTop, kBoxRight, kBoxBottom, kBoxFront, kBoxBack,
                         box.data(), 0, (int)box.size());

        // 3. Read the whole volume back and check every voxel.
        std::vector<Color> readBack((std::size_t)kWidth * kHeight * kDepth, Color::Black);
        volume_->GetData(readBack.data(), (int)readBack.size());

        roundTripOk_ = true;
        checked_ = 0;
        for (int z = 0; z < kDepth && roundTripOk_; ++z) {
            for (int y = 0; y < kHeight && roundTripOk_; ++y) {
                for (int x = 0; x < kWidth; ++x) {
                    const Color got = readBack[Index(x, y, z)];
                    const Color want = Expected(x, y, z);
                    ++checked_;
                    if (got.getRProperty() != want.getRProperty() ||
                        got.getGProperty() != want.getGProperty() ||
                        got.getBProperty() != want.getBProperty()) {
                        roundTripOk_ = false;
                        failureDetail_ = "(" + std::to_string(x) + "," + std::to_string(y) + "," +
                                         std::to_string(z) + ")";
                        break;
                    }
                }
            }
        }

        // 4. Each Z slice as an ordinary Texture2D, so SpriteBatch can show it.
        for (int z = 0; z < kDepth; ++z) {
            std::vector<Color> slice((std::size_t)kWidth * kHeight, Color::Black);
            for (int y = 0; y < kHeight; ++y) {
                for (int x = 0; x < kWidth; ++x) slice[(std::size_t)y * kWidth + x] = readBack[Index(x, y, z)];
            }
            auto texture = std::make_unique<Texture2D>(device, kWidth, kHeight);
            texture->SetData(slice.data(), (int)slice.size());
            slices_.push_back(std::move(texture));
        }
    }

    static std::size_t Index(int x, int y, int z) {
        return ((std::size_t)z * kHeight + y) * kWidth + x;
    }

    std::optional<Texture3D> volume_;
    std::vector<std::unique_ptr<Texture2D>> slices_;
    std::string error_;
    std::string failureDetail_;
    bool roundTripOk_ = false;
    int checked_ = 0;
};

} // namespace CnaExamples::Demos::Graphics3D::TexturesAndQueriesDemos
