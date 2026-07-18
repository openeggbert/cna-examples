// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
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

namespace CnaExamples::Demos::Graphics3D::CameraAndProjectionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates Matrix::CreateLookAt driving a genuinely moving CAMERA (not
// just a spinning World matrix): 3 static cubes sit at fixed world
// positions while the View matrix orbits around them every frame
// (cameraPos = (sin(t)*radius, height, cos(t)*radius), always looking at
// the origin). Proves the View matrix, not just World, is what's actually
// moving -- the cubes themselves never rotate; only the vantage point does.
class OrbitingCameraScreen : public DemoScreen {
public:
    OrbitingCameraScreen() : DemoScreen("Camera: Orbiting View Matrix") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeColorMesh(0.6f, {
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
    void OnDemoUpdate(GameTime& gameTime) override {
        orbit_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("3 STATIC cubes never rotate -- only the camera (View matrix) orbits");
        lines.push_back("around them every frame via Matrix::CreateLookAt, always facing the origin.");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 170, original.getWidthProperty(), 400);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        const float radius = 4.5f;
        const Vector3 cameraPos(std::sin(orbit_) * radius, 1.5f, std::cos(orbit_) * radius);
        effect_->View = Matrix::CreateLookAt(cameraPos, Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        const Vector3 positions[3] = {Vector3(-1.4f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f),
                                       Vector3(1.4f, 0.0f, 0.0f)};
        for (const Vector3& p : positions) {
            effect_->World = Matrix::CreateTranslation(p);
            effect_->Apply();
            device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                             (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                             (int)mesh_.indices.size() / 3);
        }

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    CubeColorMesh mesh_;
    std::optional<BasicEffect> effect_;
    float orbit_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::CameraAndProjectionDemos
