// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <exception>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/CubeMapFace.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EnvironmentMapEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTargetCube.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::TexturesAndQueriesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::CubeMapFace;
using Microsoft::Xna::Framework::Graphics::DepthFormat;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::EnvironmentMapEffect;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::RenderTargetCube;
using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
using Microsoft::Xna::Framework::Graphics::Viewport;

// A RenderTargetCube is a TextureCube the GPU draws INTO, one face at a time.
// That "one face at a time" is the whole API difference: SetRenderTarget takes
// a CubeMapFace as its second argument, and binding the cube binds ONE surface,
// not six. Six passes, six bindings.
//
// This is how a runtime environment map is made -- point a 90-degree camera
// down each axis and render the scene six times. Here each face is simply
// cleared to its own colour, which is enough to prove the six faces are
// independent surfaces, and the cube is then fed straight into
// EnvironmentMapEffect as the reflection source for the spinning object below.
// Different faces reflect different colours as it turns; a demo that had
// written one face six times would reflect a single flat colour.
//
// SECOND, AND THE REASON THIS SCREEN CHECKS ITSELF: reading a RenderTargetCube
// back with GetData does not work here, and does not say so.
// ITextureCubeBackend::GetData is declared a no-op by default, and EasyGL's
// render-target cube backend overrides only SetData. TextureCube::GetData
// still runs its full validation, allocates a ZERO-INITIALISED staging buffer,
// calls the backend (which does nothing), and copies that buffer out -- so the
// caller gets 32x32 transparent-black texels and no exception. Plausible-
// looking data, silently wrong. Nothing about the call site reveals it.
//
// The screen therefore does not assume either outcome: it fills its buffer
// with a magenta sentinel, calls GetData, and reports which of the three
// things actually happened.
class RenderTargetCubeScreen : public DemoScreen {
public:
    RenderTargetCubeScreen() : DemoScreen("RenderTargetCube: Rendered Faces") {}

    void OnDemoLoad() override {
        try {
            Build();
        } catch (const std::exception& ex) {
            error_ = ex.what();
        }
    }

    void OnDemoUnload() override {
        effect_.reset();
        cube_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        if (!error_.empty()) {
            lines.push_back("RenderTargetCube is not available on this backend:");
            lines.push_back("  " + error_);
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);
            return;
        }

        lines.push_back("Six faces rendered one at a time, then used as a reflection source:");
        lines.push_back("  device.SetRenderTarget(&cube, CubeMapFace::PositiveX);  // ONE surface");
        lines.push_back("Binding the cube does not bind six surfaces. Six passes, six bindings.");
        lines.push_back("The spinning cube below reflects them via EnvironmentMapEffect -- the");
        lines.push_back("colour changes as it turns, so the faces really do differ.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        // The readback finding, as a swatch a pixel probe can assert on. Amber
        // rather than red: this demo is not broken -- the API is unsupported
        // here, and reporting that accurately is the point.
        FillRect(sb, Rectangle(40, (int)end.Y + 6, 24, 24),
                 mul(readback_ == Readback::RealPixels ? Color(40, 200, 90, 255)
                                                       : Color(230, 170, 40, 255),
                     TransitionAlpha()));
        sb.DrawString(font, ReadbackHeadline(), Vector2(76.0f, end.Y + 6.0f), tint);
        sb.DrawString(font, ReadbackDetail(), Vector2(76.0f, end.Y + 30.0f), tint);

        sb.End();
        DrawReflection();
        sb.Begin();
    }

private:
    enum class Readback { RealPixels, SilentlyZeroed, Untouched, Threw, Other };

    const char* ReadbackHeadline() const {
        switch (readback_) {
            case Readback::RealPixels:     return "GetData returned the real rendered pixels on this backend.";
            case Readback::SilentlyZeroed: return "GetData returned all-zero texels -- and threw nothing.";
            case Readback::Untouched:      return "GetData left the caller's buffer completely untouched.";
            case Readback::Threw:          return "GetData threw -- unsupported, but at least it said so.";
            default:                       return "GetData returned something unexpected.";
        }
    }

    const char* ReadbackDetail() const {
        switch (readback_) {
            case Readback::RealPixels:
                return "";
            case Readback::SilentlyZeroed:
                return "Zeroed staging buffer + no-op backend = plausible, silently wrong data.";
            case Readback::Untouched:
                return "A caller trusting it would read whatever its own buffer already held.";
            case Readback::Threw:
                return "";
            default:
                return "Neither the sentinel, nor zeros, nor the face colour.";
        }
    }

    static constexpr int kSize = 32;

    static constexpr std::array<CubeMapFace, 6> kFaces = {
        CubeMapFace::PositiveX, CubeMapFace::NegativeX, CubeMapFace::PositiveY,
        CubeMapFace::NegativeY, CubeMapFace::PositiveZ, CubeMapFace::NegativeZ,
    };

    static Color FaceColor(CubeMapFace face) {
        switch (face) {
            case CubeMapFace::PositiveX: return Color(235, 80, 60, 255);
            case CubeMapFace::NegativeX: return Color(120, 40, 30, 255);
            case CubeMapFace::PositiveY: return Color(70, 225, 110, 255);
            case CubeMapFace::NegativeY: return Color(30, 110, 55, 255);
            case CubeMapFace::PositiveZ: return Color(80, 130, 250, 255);
            default:                     return Color(35, 55, 130, 255);
        }
    }

    void Build() {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeNormalTextureMesh(1.0f);
        cube_.emplace(device, kSize, false, SurfaceFormat::Color, DepthFormat::None);

        for (CubeMapFace face : kFaces) {
            device.SetRenderTarget(&*cube_, face);
            device.Clear(FaceColor(face));
            device.SetRenderTarget(nullptr);
        }

        // Prove the readback behaviour rather than assuming it. The buffer goes
        // in filled with a colour no face was cleared to, so "untouched",
        // "zeroed" and "real pixels" are all distinguishable afterwards.
        const Color sentinel(255, 0, 255, 255);
        std::vector<Color> probe((std::size_t)kSize * kSize, sentinel);
        try {
            cube_->GetData(CubeMapFace::PositiveX, probe.data(), (int)probe.size());
        } catch (const std::exception&) {
            readback_ = Readback::Threw;
            return;
        }

        const Color want = FaceColor(CubeMapFace::PositiveX);
        bool allSentinel = true, allZero = true, matchesFace = true;
        for (const Color& c : probe) {
            const int r = (int)c.getRProperty(), g = (int)c.getGProperty(), b = (int)c.getBProperty();
            if (r != 255 || g != 0 || b != 255) allSentinel = false;
            if (r != 0 || g != 0 || b != 0) allZero = false;
            if (r != (int)want.getRProperty() || g != (int)want.getGProperty() ||
                b != (int)want.getBProperty()) matchesFace = false;
        }
        readback_ = matchesFace  ? Readback::RealPixels
                  : allZero      ? Readback::SilentlyZeroed
                  : allSentinel  ? Readback::Untouched
                                 : Readback::Other;

        effect_.emplace(device);
        effect_->setEnvironmentMapProperty(&*cube_);
        effect_->setEnvironmentMapAmountProperty(1.0f);
        effect_->setDiffuseColorProperty(Vector3(0.3f, 0.3f, 0.3f));
    }

    void DrawReflection() {
        if (!effect_) return;
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 300, original.getWidthProperty(), 270);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->setWorldProperty(Matrix::CreateRotationY(spin_) *
                                  Matrix::CreateRotationX(spin_ * 0.7f));
        effect_->setViewProperty(Matrix::CreateLookAt(Vector3(0.0f, 1.2f, 4.0f), Vector3::Zero,
                                                      Vector3(0.0f, 1.0f, 0.0f)));
        effect_->setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f));

        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                         (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                         (int)mesh_.indices.size() / 3);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
    }

    CubeNormalTextureMesh mesh_;
    std::optional<RenderTargetCube> cube_;
    std::optional<EnvironmentMapEffect> effect_;
    std::string error_;
    float spin_ = 0.0f;
    Readback readback_ = Readback::Other;
};

} // namespace CnaExamples::Demos::Graphics3D::TexturesAndQueriesDemos
