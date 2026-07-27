// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/CubeMapFace.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCube.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics3D::TexturesAndQueriesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::CubeMapFace;
using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::TextureCube;

// A TextureCube is six square faces sharing one size and format, addressed by
// CubeMapFace rather than by index. Every SetData/GetData overload takes the
// face as its first argument -- there is no "whole cube at once" call, which is
// the practical difference from Texture2D and the reason this screen exists.
//
// The six faces are laid out below as the conventional unfolded cross, so the
// +X/-X, +Y/-Y, +Z/-Z pairings are visible rather than merely named. Each face
// gets its own colour AND a distinct corner marker, so a demo that silently
// wrote the same face six times would look obviously wrong instead of merely
// colourful.
//
// As with the volume screen, every face is read back with GetData and compared
// against what was written; the swatch reports the result.
class TextureCubeFacesScreen : public DemoScreen {
public:
    TextureCubeFacesScreen() : DemoScreen("TextureCube: Six Faces") {}

    void OnDemoLoad() override {
        try {
            BuildCube();
        } catch (const std::exception& ex) {
            error_ = ex.what();
        }
    }

    void OnDemoUnload() override {
        faceTextures_.clear();
        cube_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        if (!error_.empty()) {
            lines.push_back("TextureCube is not available on this backend:");
            lines.push_back("  " + error_);
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);
            return;
        }

        lines.push_back("TextureCube, " + std::to_string(kSize) + "x" + std::to_string(kSize) +
                        " per face, SurfaceFormat::Color. Six faces, one size, one format.");
        lines.push_back("Every SetData/GetData overload names a CubeMapFace -- there is no call");
        lines.push_back("that writes the whole cube at once.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        // The conventional unfolded cross:  the middle row is +X -Z -X +Z read
        // left to right, with +Y above and -Y below the -Z cell.
        const float originX = 300.0f;
        const float originY = end.Y + 16.0f;
        DrawFace(sb, font, CubeMapFace::PositiveY, originX + kCell, originY, tint);
        DrawFace(sb, font, CubeMapFace::PositiveX, originX, originY + kCell, tint);
        DrawFace(sb, font, CubeMapFace::NegativeZ, originX + kCell, originY + kCell, tint);
        DrawFace(sb, font, CubeMapFace::NegativeX, originX + kCell * 2.0f, originY + kCell, tint);
        DrawFace(sb, font, CubeMapFace::PositiveZ, originX + kCell * 3.0f, originY + kCell, tint);
        DrawFace(sb, font, CubeMapFace::NegativeY, originX + kCell, originY + kCell * 2.0f, tint);

        const float swatchY = originY + kCell * 3.0f + 16.0f;
        FillRect(sb, Rectangle(40, (int)swatchY, 24, 24),
                 mul(roundTripOk_ ? Color(40, 200, 90, 255) : Color(220, 60, 60, 255),
                     TransitionAlpha()));
        sb.DrawString(font,
                      roundTripOk_ ? "GetData round trip verified on all six faces."
                                   : "GetData round trip FAILED on " + failureDetail_,
                      Vector2(76.0f, swatchY), tint);
    }

private:
    static constexpr int kSize = 32;
    static constexpr float kCell = 72.0f;

    static constexpr std::array<CubeMapFace, 6> kFaces = {
        CubeMapFace::PositiveX, CubeMapFace::NegativeX, CubeMapFace::PositiveY,
        CubeMapFace::NegativeY, CubeMapFace::PositiveZ, CubeMapFace::NegativeZ,
    };

    static const char* FaceName(CubeMapFace face) {
        switch (face) {
            case CubeMapFace::PositiveX: return "+X";
            case CubeMapFace::NegativeX: return "-X";
            case CubeMapFace::PositiveY: return "+Y";
            case CubeMapFace::NegativeY: return "-Y";
            case CubeMapFace::PositiveZ: return "+Z";
            default:                     return "-Z";
        }
    }

    static Color FaceColor(CubeMapFace face) {
        switch (face) {
            case CubeMapFace::PositiveX: return Color(220, 70, 70, 255);
            case CubeMapFace::NegativeX: return Color(120, 30, 30, 255);
            case CubeMapFace::PositiveY: return Color(70, 220, 90, 255);
            case CubeMapFace::NegativeY: return Color(30, 110, 45, 255);
            case CubeMapFace::PositiveZ: return Color(80, 120, 240, 255);
            default:                     return Color(35, 55, 130, 255);
        }
    }

    // Face colour everywhere, except an N-pixel corner block whose size differs
    // per face -- so two faces written with the same colour by mistake would
    // still be distinguishable.
    static Color ExpectedTexel(CubeMapFace face, int x, int y) {
        const int marker = 4 + (int)face * 2;
        if (x < marker && y < marker) return Color(250, 250, 250, 255);
        return FaceColor(face);
    }

    void DrawFace(SpriteBatch& sb, SpriteFont& font, CubeMapFace face, float x, float y,
                  const Color& tint) const {
        const auto& texture = faceTextures_[(std::size_t)face];
        sb.Draw(*texture, Rectangle((int)x, (int)y, (int)kCell - 4, (int)kCell - 4), tint);
        sb.DrawString(font, FaceName(face), Vector2(x + 6.0f, y + 4.0f), tint);
    }

    void BuildCube() {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        cube_.emplace(device, kSize, false, SurfaceFormat::Color);

        roundTripOk_ = true;
        faceTextures_.resize(6);

        for (CubeMapFace face : kFaces) {
            std::vector<Color> texels((std::size_t)kSize * kSize, Color::Black);
            for (int y = 0; y < kSize; ++y) {
                for (int x = 0; x < kSize; ++x) {
                    texels[(std::size_t)y * kSize + x] = ExpectedTexel(face, x, y);
                }
            }
            cube_->SetData(face, texels.data(), (int)texels.size());

            std::vector<Color> readBack((std::size_t)kSize * kSize, Color::Black);
            cube_->GetData(face, readBack.data(), (int)readBack.size());
            for (int y = 0; y < kSize && roundTripOk_; ++y) {
                for (int x = 0; x < kSize; ++x) {
                    const Color got = readBack[(std::size_t)y * kSize + x];
                    const Color want = ExpectedTexel(face, x, y);
                    if (got.getRProperty() != want.getRProperty() ||
                        got.getGProperty() != want.getGProperty() ||
                        got.getBProperty() != want.getBProperty()) {
                        roundTripOk_ = false;
                        failureDetail_ = std::string(FaceName(face)) + " at (" +
                                         std::to_string(x) + "," + std::to_string(y) + ")";
                        break;
                    }
                }
            }

            // Blit what came back into a 2D texture so SpriteBatch can show it:
            // SpriteBatch cannot sample a cube face directly.
            auto texture = std::make_unique<Texture2D>(device, kSize, kSize);
            texture->SetData(readBack.data(), (int)readBack.size());
            faceTextures_[(std::size_t)face] = std::move(texture);
        }
    }

    std::optional<TextureCube> cube_;
    std::vector<std::unique_ptr<Texture2D>> faceTextures_;
    std::string error_;
    std::string failureDetail_;
    bool roundTripOk_ = false;
};

} // namespace CnaExamples::Demos::Graphics3D::TexturesAndQueriesDemos
