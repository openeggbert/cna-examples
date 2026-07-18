// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::FogDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates BasicEffect distance fog: 5 cubes at increasing distance
// from the camera (4/8/12/16/20 units), FogEnabled=true, FogStart=3/
// FogEnd=16 tuned to that range so the nearest cube is essentially
// unfogged and the farthest is fully faded to FogColor.
//
// CONFIRMED REAL FRAMEWORK LIMITATION (found via Xvfb screenshot showing
// zero fog fade across any cube, then traced into EasyGLGraphicsBackend
// .cpp's own fog shader source and its own code comment): CNA's fog
// calculation on this backend uses the raw OBJECT-SPACE vertex Z attribute
// directly as a stand-in for camera distance --
// `vFogFactor = clamp((aPos.z + FogEnd) / (FogEnd - FogStart), 0, 1)` --
// it does NOT transform the vertex through World/View first. The
// backend's own comment admits this "matches FNA's ComputeFogFactor...
// since World=View=Identity in every CNA fog test/scene" -- i.e. fog only
// ever worked correctly in upstream tests because those tests happen to
// use identity World/View, where object-space Z coincidentally equals
// camera-space depth. Placing an object away from the origin via a
// non-identity World matrix (as almost any real scene would) leaves fog
// computed from the wrong Z entirely and it silently never fades.
//
// Real screenshots proved this: with cubes positioned only via World-matrix
// translation (object-space Z always in the tiny +-0.8 cube-local range),
// EVERY cube rendered fully unfogged regardless of its actual on-screen
// distance. The only way to demonstrate real fog given this constraint is
// to bake each cube's intended "distance" directly into its own vertex
// data (so object-space Z really does equal -distance), and use World only
// for the lateral (X) placement and a FIXED (non-distance-dependent) Z
// translation that repositions the whole baked-Z range relative to the
// camera. This is an honest workaround for a confirmed real limitation,
// not a fabricated demo -- see plan.md section 5.7 for the full writeup.
class DistanceFogScreen : public DemoScreen {
public:
    static constexpr float kCameraWorldZ = 2.0f;

    DistanceFogScreen() : DemoScreen("BasicEffect: Distance Fog") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const std::array<Color, 6> faceColors = {
            Color(220, 200, 60, 255), Color(220, 200, 60, 255), Color(220, 200, 60, 255),
            Color(220, 200, 60, 255), Color(220, 200, 60, 255), Color(220, 200, 60, 255),
        };
        for (std::size_t i = 0; i < kDistancesFromEye.size(); ++i) {
            CubeColorMesh mesh = BuildCubeColorMesh(0.8f, faceColors);
            // Bake -distance directly into each vertex's own Z so the
            // (object-space-Z-only) fog calculation reacts to it -- see the
            // class comment above for why this bypass is necessary.
            for (auto& v : mesh.vertices) {
                v.Position.Z -= kDistancesFromEye[i];
            }
            meshes_[i] = std::move(mesh);
        }
        effect_.emplace(device);
        effect_->VertexColorEnabled = true;
        effect_->setFogEnabledProperty(true);
        effect_->setFogColorProperty(Vector3(0.5f, 0.5f, 0.55f));
        effect_->setFogStartProperty(3.0f);
        effect_->setFogEndProperty(16.0f);
    }

    void UnloadContent() override {
        effect_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("5 cubes at object-space distance 4/8/12/16/20 (baked into their own");
        lines.push_back("vertex Z -- see this file's class comment for why). FogStart=3, FogEnd=16:");
        lines.push_back("nearest is unfogged, farthest fully faded to FogColor (mid-grey).");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 1.0f, kCameraWorldZ), Vector3(0.0f, 1.0f, -20.0f),
                                             Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        // World only shifts X (lateral spread, so the progressively-smaller
        // farther cubes don't sit directly behind the nearest one -- a
        // separate real bug also found via screenshot and fixed) and a
        // FIXED +kCameraWorldZ, which re-centers each mesh's own baked
        // -distance range back onto the camera's actual world position
        // (kCameraWorldZ - distance == the on-screen depth the cube should
        // appear at) WITHOUT touching the object-space Z the fog shader
        // itself reads.
        static const std::array<float, 5> kLateralOffsets = {0.0f, 1.6f, 3.2f, 4.8f, 6.4f};
        for (std::size_t i = 0; i < kDistancesFromEye.size(); ++i) {
            effect_->World = Matrix::CreateTranslation(kLateralOffsets[i], 1.0f, kCameraWorldZ);
            effect_->Apply();
            device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, meshes_[i].vertices.data(), 0,
                                             (int)meshes_[i].vertices.size(), meshes_[i].indices.data(), 0,
                                             (int)meshes_[i].indices.size() / 3);
        }

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    static const std::array<float, 5> kDistancesFromEye;
    std::array<CubeColorMesh, 5> meshes_;
    std::optional<BasicEffect> effect_;
};

inline const std::array<float, 5> DistanceFogScreen::kDistancesFromEye = {4.0f, 8.0f, 12.0f, 16.0f, 20.0f};

} // namespace CnaExamples::Demos::Graphics3D::FogDemos
