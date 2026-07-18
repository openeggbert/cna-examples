// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::CameraAndProjectionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates Matrix::CreatePerspectiveFieldOfView vs. CreateOrthographic
// on the EXACT same scene (3 same-size cubes at different depths),
// rendered side by side in the same frame via 2 separate sub-viewports:
// left half uses perspective (farther cubes appear smaller -- real
// depth-based foreshortening), right half uses orthographic (all 3 cubes
// appear the SAME apparent size regardless of depth -- no foreshortening
// at all). Both halves share the same View matrix and cube positions;
// only the Projection matrix differs.
class PerspectiveVsOrthographicScreen : public DemoScreen {
public:
    PerspectiveVsOrthographicScreen() : DemoScreen("Camera: Perspective vs. Orthographic") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeColorMesh(0.5f, {
            Color(220, 60, 60, 255), Color(60, 220, 90, 255), Color(60, 110, 220, 255),
            Color(220, 200, 60, 255), Color(200, 60, 220, 255), Color(60, 220, 210, 255),
        });
        effect_.emplace(device);
        effect_->VertexColorEnabled = true;
    }

    void UnloadContent() override {
        effect_.reset();
    }

protected:
    void DrawScene(GraphicsDevice& device, const Matrix& view, const Matrix& projection) {
        effect_->View = view;
        effect_->Projection = projection;
        const float depths[3] = {0.0f, -2.0f, -4.0f};
        for (int i = 0; i < 3; ++i) {
            effect_->World = Matrix::CreateTranslation(0.0f, 0.0f, depths[i]);
            effect_->Apply();
            device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                             (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                             (int)mesh_.indices.size() / 3);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Same 3 cubes (same size, at Z=0/-2/-4), same View matrix, split-screen:");
        lines.push_back("Perspective shrinks farther cubes; Orthographic keeps them the same size.");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        const int sceneY = 170;
        const int sceneH = 380;
        const int halfW = original.getWidthProperty() / 2;
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        const Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 0.8f, 3.0f), Vector3(0.0f, 0.0f, -2.0f),
                                                  Vector3(0.0f, 1.0f, 0.0f));

        Viewport left(0, sceneY, halfW, sceneH);
        device.setViewportProperty(left);
        DrawScene(device, view,
                  Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4, (float)halfW / (float)sceneH, 0.1f, 100.0f));

        Viewport right(halfW, sceneY, halfW, sceneH);
        device.setViewportProperty(right);
        DrawScene(device, view, Matrix::CreateOrthographic(6.0f, 6.0f * (float)sceneH / (float)halfW, 0.1f, 100.0f));

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();

        const Color tint = mul(Color::White, TransitionAlpha());
        sb.DrawString(font, "Perspective (shrinks with depth)", Vector2(30.0f, (float)(sceneY + sceneH + 10)), tint);
        sb.DrawString(font, "Orthographic (constant size)", Vector2((float)(halfW + 30), (float)(sceneY + sceneH + 10)), tint);
    }

private:
    CubeColorMesh mesh_;
    std::optional<BasicEffect> effect_;
};

} // namespace CnaExamples::Demos::Graphics3D::CameraAndProjectionDemos
