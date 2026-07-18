// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/CubeMapFace.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EnvironmentMapEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCube.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::EnvironmentMapEffectDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::CubeMapFace;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::EnvironmentMapEffect;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
using Microsoft::Xna::Framework::Graphics::TextureCube;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Builds a 6-distinct-flat-color procedural TextureCube -- no photographic
// skybox asset is bundled (this app never bundles image assets), but 6
// solid colors are enough to prove real cube-map reflection: as the cube
// rotates, each face's reflected color changes because the reflection
// vector (derived from the real surface normal + view direction) samples a
// different cube face, not a fixed 2D texture.
//
// Fills `out` in place via emplace() rather than returning a TextureCube by
// value -- TextureCube.hpp defaults its move constructor inline against a
// forward-declared backend type (ITextureCubeBackend), which only compiles
// where the complete backend type is visible (inside the cna library's own
// translation units); a by-value return from application code requires
// that same defaulted move constructor and fails to compile here with an
// "invalid application of sizeof to incomplete type" error. Constructing
// in place with emplace() (and calling SetData directly on the reference)
// never needs to move/copy a TextureCube at all.
inline void CreateSixColorEnvironmentMap(GraphicsDevice& device, int size, std::optional<TextureCube>& out) {
    out.emplace(device, size, false, SurfaceFormat::Color);
    const Color faceColors[6] = {
        Color(255, 60, 60, 255),  Color(60, 255, 90, 255),  Color(60, 110, 255, 255),
        Color(255, 220, 60, 255), Color(220, 60, 255, 255), Color(60, 230, 220, 255),
    };
    const CubeMapFace faces[6] = {
        CubeMapFace::PositiveX, CubeMapFace::NegativeX, CubeMapFace::PositiveY,
        CubeMapFace::NegativeY, CubeMapFace::PositiveZ, CubeMapFace::NegativeZ,
    };
    std::vector<Color> facePixels((std::size_t)size * (std::size_t)size, Color::White);
    for (int f = 0; f < 6; ++f) {
        std::fill(facePixels.begin(), facePixels.end(), faceColors[f]);
        out->SetData(faces[f], facePixels.data(), (int)facePixels.size());
    }
}

// Demonstrates EnvironmentMapEffect: a rotating cube fully reflecting
// (EnvironmentMapAmount=1.0) the 6-distinct-color procedural cube map
// above. Different faces show different reflected colors as the cube
// spins, proving the reflection genuinely follows the cube's own
// orientation/surface normals rather than being a fixed decal.
class ReflectiveCubeScreen : public DemoScreen {
public:
    ReflectiveCubeScreen() : DemoScreen("EnvironmentMapEffect: Reflective Cube") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeNormalTextureMesh(1.0f);
        CreateSixColorEnvironmentMap(device, 8, envMap_);
        effect_.emplace(device);
        effect_->setEnvironmentMapProperty(&*envMap_);
        effect_->setEnvironmentMapAmountProperty(1.0f);
        effect_->setDiffuseColorProperty(Vector3(0.3f, 0.3f, 0.3f));
    }

    void UnloadContent() override {
        effect_.reset();
        envMap_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("EnvironmentMapEffect.EnvironmentMap: a 6-distinct-color procedural");
        lines.push_back("TextureCube, fully reflective (EnvironmentMapAmount=1.0). Different");
        lines.push_back("faces show different reflected colors as the cube spins.");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->setWorldProperty(Matrix::CreateRotationY(spin_) * Matrix::CreateRotationX(spin_ * 0.7f));
        effect_->setViewProperty(Matrix::CreateLookAt(Vector3(0.0f, 1.2f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f)));
        effect_->setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f));

        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                         (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                         (int)mesh_.indices.size() / 3);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    CubeNormalTextureMesh mesh_;
    std::optional<TextureCube> envMap_;
    std::optional<EnvironmentMapEffect> effect_;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::EnvironmentMapEffectDemos
