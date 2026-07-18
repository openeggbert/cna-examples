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

namespace CnaExamples::Demos::Graphics3D::DepthAndCullingDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates DepthStencilState::Default vs. None on 2 overlapping cubes.
// A red cube sits FARTHER from the camera; a blue cube sits NEARER, and is
// drawn FIRST (deliberately the "wrong" submission order -- the interesting
// case). Space/Enter/A toggles the state: under Default (depth test+write
// ON), the nearer blue cube correctly occludes the farther red one
// regardless of draw order (real depth-buffer resolution). Under None
// (both off), whichever cube is drawn LAST simply paints over the other --
// since red is drawn second, red would appear fully on top despite being
// farther away, a real, visible "wrong" result proving depth testing was
// doing real work a moment ago.
class DepthStencilComparisonScreen : public DemoScreen {
public:
    DepthStencilComparisonScreen() : DemoScreen("DepthStencilState: Default vs. None") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        nearMesh_ = BuildCubeColorMesh(0.8f, MakeFaceColors(Color(60, 110, 220, 255)));
        farMesh_ = BuildCubeColorMesh(0.8f, MakeFaceColors(Color(220, 70, 70, 255)));
        effect_.emplace(device);
        effect_->VertexColorEnabled = true;
    }

    void UnloadContent() override {
        effect_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            depthOn_ = !depthOn_;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Blue cube is NEARER, drawn FIRST (deliberately wrong submission order).");
        lines.push_back("Space/Enter/A: DepthStencilState = " +
                         std::string(depthOn_ ? "Default (real occlusion)" : "None (paint order wins)"));
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 170, original.getWidthProperty(), 400);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(depthOn_ ? DepthStencilState::Default : DepthStencilState::None);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 0.6f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        // Blue (near, Z=0) drawn FIRST.
        effect_->World = Matrix::CreateTranslation(-0.3f, 0.0f, 0.0f);
        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, nearMesh_.vertices.data(), 0,
                                         (int)nearMesh_.vertices.size(), nearMesh_.indices.data(), 0,
                                         (int)nearMesh_.indices.size() / 3);

        // Red (far, Z=-2) drawn SECOND, despite being behind the blue cube.
        effect_->World = Matrix::CreateTranslation(0.3f, 0.0f, -2.0f);
        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, farMesh_.vertices.data(), 0,
                                         (int)farMesh_.vertices.size(), farMesh_.indices.data(), 0,
                                         (int)farMesh_.indices.size() / 3);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    static std::array<Color, 6> MakeFaceColors(Color c) {
        return {c, c, c, c, c, c};
    }

    CubeColorMesh nearMesh_;
    CubeColorMesh farMesh_;
    std::optional<BasicEffect> effect_;
    bool depthOn_ = true;
};

} // namespace CnaExamples::Demos::Graphics3D::DepthAndCullingDemos
