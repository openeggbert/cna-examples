// SPDX-License-Identifier: MIT
#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "CNA/Graphics/PbrMaterial.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PbrEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::PbrDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Graphics::BufferUsage;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::IndexBuffer;
using Microsoft::Xna::Framework::Graphics::IndexElementSize;
using Microsoft::Xna::Framework::Graphics::PbrEffect;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::VertexBuffer;
using Microsoft::Xna::Framework::Graphics::VertexPositionNormalTangentTexture;
using Microsoft::Xna::Framework::Graphics::Viewport;
using CNA::Graphics::PbrMaterial;

// PbrEffect is CNA's metallic-roughness BRDF -- the glTF/industry-standard
// material model, and a genuinely different one from BasicEffect's
// diffuse/specular Blinn-Phong. Two scalars describe almost every opaque
// material:
//
//   METALLIC   0 = dielectric (plastic, wood, stone): a white-ish specular
//                  highlight sits on top of the base colour.
//              1 = metal: there is no diffuse term at all, and the reflection
//                  is TINTED BY THE BASE COLOUR. This is why a metal sphere
//                  goes dark where it reflects nothing -- it has no
//                  diffuse floor to fall back to.
//
//   ROUGHNESS  0 = mirror-smooth, a tight bright highlight.
//              1 = fully diffuse, the highlight spread across the surface.
//
// The grid below is the standard way to read them: metallic increases down the
// rows, roughness increases across the columns, everything else held constant.
// Spheres, not cubes -- a flat face shows almost none of this, because the
// interesting behaviour is the highlight sweeping across a curved surface.
//
// A VertexBuffer is REQUIRED here, and that is worth stating plainly because
// getting it wrong fails silently. PbrEffect needs tangents, so the vertex type
// is VertexPositionNormalTangentTexture -- and GraphicsDevice only provides
// typed DrawUserIndexedPrimitives overloads for four legacy vertex types
// (VertexPositionColor, VertexPositionColorTexture, VertexPositionTexture,
// VertexPositionNormalTexture). A tangent vertex array therefore binds to the
// untyped `const void*` overload, which carries NO vertex declaration, so the
// GPU reinterprets the data under whatever layout was last bound. It does
// not throw or warn -- it draws garbage geometry (or nothing at all) rather
// than failing, which is exactly how this screen was first written and what
// pixel measurement caught.
//
// THE ACTUAL BUG this screen was reverted over (found on a later pass, see
// docs/wip-pbr/README.md and NEXT.md's D2 entry for the full investigation):
// VertexPositionNormalTangentTexture inherits IVertexType, which declares a
// virtual destructor -- so the struct carries a hidden 8-byte vtable pointer,
// and its REAL sizeof() is 56, not the naive 48 = 12+12+16+8 this screen
// originally assumed when calling SetDataRaw(..., sizeof(VertexPositionNormalTangentTexture)).
// Uploading the polymorphic object's raw bytes at stride 56 makes
// EasyGLGraphicsBackend::ApplyLayout pick the wrong (skinned-vertex) attribute
// layout, so the position attribute reads bytes starting at the vtable
// pointer as floats -- degenerate/NaN clip-space positions, i.e. nothing
// visibly draws. The fix, mirroring both ../cna's own
// easygl_pbreffect_golden_test.cpp and CNA's own typed VertexBuffer::SetData
// overloads for legacy vertex types (VertexBuffer.cpp): repack into a
// private, NON-polymorphic, tightly-packed POD before upload, and upload
// THAT at its own real (48-byte) stride -- never the polymorphic struct's
// raw bytes directly.
//
// A SEPARATE finding, made while following up on this screen: CNA::Graphics::PbrMaterial
// (a glTF-style texture-slot + factor settings bag, also NOXNA) exists and plan.md's
// original D2 note claiming otherwise was stale. It is the SAME situation as
// RenderPipelineSettingsScreen.hpp, not new demo substance: PbrEffect (below) and CNA's own
// glTF loader (RuntimeGltfModelTests.cpp) both set PbrEffect's OWN properties directly
// (getTextureProperty/getMetallicFactorProperty/etc., exactly as this screen does) --
// PbrMaterial is never constructed by anything outside its own round-trip test
// (../cna/examples/noxna_settings_example.cpp). Verified live below rather than assumed.
class PbrMetallicRoughnessScreen : public DemoScreen {
public:
    PbrMetallicRoughnessScreen() : DemoScreen("PbrEffect: Metallic & Roughness") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        SphereTangentMesh sphere = BuildSphereTangentMesh(0.42f, 24, 16);

        std::vector<GpuVertex> flat;
        flat.reserve(sphere.indices.size());
        for (std::uint16_t index : sphere.indices) {
            const VertexPositionNormalTangentTexture& v = sphere.vertices[index];
            flat.push_back(GpuVertex{
                v.Position.X, v.Position.Y, v.Position.Z,
                v.Normal.X, v.Normal.Y, v.Normal.Z,
                v.Tangent.X, v.Tangent.Y, v.Tangent.Z, v.Tangent.W,
                v.TextureCoordinate.X, v.TextureCoordinate.Y,
            });
        }
        triangleCount_ = (int)flat.size() / 3;
        vb_.emplace(device, (int)flat.size());
        vb_->SetDataRaw(flat.data(), (int)flat.size(), (int)sizeof(GpuVertex));

        // A base-colour texture is not optional in practice: the PBR shader
        // samples albedo unconditionally, so leaving it null renders nothing.
        baseColor_.emplace(CnaExamples::Demos::Graphics2D::CreateCheckerboardTexture(
            device, 8, 8, 8, Color(230, 190, 110, 255), Color(230, 190, 110, 255)));
        effect_.emplace(device);
        effect_->EnableDefaultLighting();
        effect_->setTextureProperty(&*baseColor_);
        effect_->setDiffuseColorProperty(Vector3(0.85f, 0.68f, 0.30f));

        // PbrMaterial round trip -- the only thing about this disconnected type that is
        // testable (see the class comment above). Non-default values, read back exactly.
        PbrMaterial mat;
        const bool defaultsMatch = mat.getAlbedoTexture() == nullptr &&
                                    mat.getMetallicFactor() == 0.0f &&
                                    mat.getRoughnessFactor() == 0.5f;
        mat.setMetallicFactor(0.9f);
        mat.setRoughnessFactor(0.15f);
        mat.setAlbedoTexture(&*baseColor_);
        pbrMaterialHonest_ = defaultsMatch &&
                              mat.getMetallicFactor() == 0.9f &&
                              mat.getRoughnessFactor() == 0.15f &&
                              mat.getAlbedoTexture() == &*baseColor_;

        rendered_ = false;
        probedOnce_ = false;
    }

    void OnDemoUnload() override {
        effect_.reset();
        baseColor_.reset();
        vb_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.35f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;
        lines.push_back("The metallic-roughness BRDF -- glTF's material model, a different one from");
        lines.push_back("BasicEffect's Blinn-Phong. Roughness 0->1 across; metallic 0->1 down.");
        lines.push_back("A metal has NO diffuse term, so the bottom row darkens where it reflects nothing.");
        lines.push_back("VertexPositionNormalTangentTexture is polymorphic (hidden vtable ptr inflates its");
        lines.push_back("size past the naive 48 bytes) -- fix: repack into a private, packed POD first.");
        lines.push_back("PbrMaterial (a related NOXNA type) round-trips faithfully but is never read by");
        lines.push_back("PbrEffect or CNA's glTF loader -- both set PbrEffect's own properties directly.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        // rendered_ reflects the PREVIOUS frame's probe (the probe itself can only run after
        // this frame's scene is drawn, below) -- stable from frame 2 onward, same
        // establish-then-report order OcclusionQueryScreen uses for its own async result.
        // pbrMaterialHonest_ is the PbrMaterial round trip from OnDemoLoad -- folded into the
        // same verdict so a future regression in that store (unlikely, but real) would show.
        const bool allGood = rendered_ && pbrMaterialHonest_;
        DrawVerdict(sb, font, end.Y + 6.0f,
                    mul(allGood ? Color(40, 200, 90, 255) : Color(220, 60, 60, 255),
                        TransitionAlpha()),
                    tint,
                    !rendered_
                        ? "Probe found no geometry -- the centre sphere is not rendering."
                        : !pbrMaterialHonest_
                              ? "Sphere renders, but PbrMaterial's round trip is no longer faithful."
                              : "Verified live: sphere renders, PbrMaterial round-trips faithfully (unread).");

        sb.End();
        DrawGrid();
        sb.Begin();
    }

private:
    static constexpr int kCols = 5;
    static constexpr int kRows = 3;

    // Stride-48 GPU-compact PBR vertex, matching ApplyLayout's stride==48
    // case (Position+Normal+Tangent+TextureCoordinate) and
    // ../cna/examples/easygl_pbreffect_golden_test.cpp's own PbrGpuVertex
    // layout exactly -- deliberately NOT derived from IVertexType, so it has
    // no vtable pointer and its sizeof() is the real, honest 48 bytes.
    struct GpuVertex {
        float px, py, pz;
        float nx, ny, nz;
        float tx, ty, tz, tw;
        float u, v;
    };
    static_assert(sizeof(GpuVertex) == 48, "PBR vertex must be 48 bytes");

    void DrawGrid() {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 340, original.getWidthProperty(), 260);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);
        device.Clear(Microsoft::Xna::Framework::Graphics::ClearOptions::DepthBuffer,
                     Color::Black, 1.0f, 0);

        const float aspect = (float)scene.getWidthProperty() / (float)scene.getHeightProperty();
        effect_->setViewProperty(Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 5.4f), Vector3::Zero,
                                                      Vector3(0.0f, 1.0f, 0.0f)));
        effect_->setProjectionProperty(
            Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4, aspect, 0.1f, 100.0f));

        device.SetVertexBuffer(&*vb_);
        for (int row = 0; row < kRows; ++row) {
            for (int col = 0; col < kCols; ++col) {
                const float metallic = (float)row / (float)(kRows - 1);
                const float roughness = 0.05f + 0.95f * (float)col / (float)(kCols - 1);
                effect_->setMetallicFactorProperty(metallic);
                effect_->setRoughnessFactorProperty(roughness);

                const float x = ((float)col - (kCols - 1) * 0.5f) * 1.05f;
                const float y = ((kRows - 1) * 0.5f - (float)row) * 1.05f;
                effect_->setWorldProperty(Matrix::CreateRotationY(spin_) *
                                          Matrix::CreateTranslation(Vector3(x, y, 0.0f)));
                effect_->Apply();
                device.DrawPrimitives(PrimitiveType::TriangleList, 0, triangleCount_);
            }
        }
        device.SetVertexBuffer(nullptr);

        // Verified live, not assumed: the centre grid cell (row=1, col=2) sits
        // exactly at world origin with this camera, so it projects to the
        // exact centre of the scene viewport. A point near the viewport's own
        // top edge sits above the whole grid (the top row's sphere extends to
        // world Y ~1.47, well short of the ~2.24 half-height this camera
        // shows at this distance), so it stays background regardless of
        // whether the grid rendered. Same GetBackBufferData route
        // tools/headless.sh's own --screenshot uses (NEXT.md discovery #1).
        if (!probedOnce_) {
            const int centerX = scene.getXProperty() + scene.getWidthProperty() / 2;
            const int centerY = scene.getYProperty() + scene.getHeightProperty() / 2;
            const int bgY = scene.getYProperty() + 6;
            Rectangle bgRect(centerX, bgY, 1, 1);
            Color bg(0, 0, 0, 0);
            device.GetBackBufferData(&bgRect, &bg, 0, 1);
            Rectangle probeRect(centerX, centerY, 1, 1);
            Color probe(0, 0, 0, 0);
            device.GetBackBufferData(&probeRect, &probe, 0, 1);
            const int dr = (int)probe.getRProperty() - (int)bg.getRProperty();
            const int dg = (int)probe.getGProperty() - (int)bg.getGProperty();
            const int db = (int)probe.getBProperty() - (int)bg.getBProperty();
            rendered_ = (dr * dr + dg * dg + db * db) > 400; // ~20 per channel, well above dither noise
            probedOnce_ = true;
        }

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
    }

    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> baseColor_;
    std::optional<VertexBuffer> vb_;
    std::optional<PbrEffect> effect_;
    int triangleCount_ = 0;
    float spin_ = 0.0f;
    bool pbrMaterialHonest_ = false;
    bool rendered_ = false;
    bool probedOnce_ = false;
};

} // namespace CnaExamples::Demos::Graphics3D::PbrDemos
