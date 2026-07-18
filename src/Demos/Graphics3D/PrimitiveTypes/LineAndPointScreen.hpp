// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics3D::PrimitiveTypesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;
using Microsoft::Xna::Framework::Graphics::VertexPositionColor;

// Demonstrates PrimitiveType::LineList, LineStrip, and PointListEXT --
// (real XNA 4.0 has no TriangleFan; CNA's point-primitive member is named
// PointListEXT, not PointList, a confirmed intentional CNA naming choice)
// drawing the same 4-corner square: LineList needs 8 vertices (4 edges,
// no sharing), LineStrip needs 5 (the loop-closing 5th vertex repeats the
// first corner), PointListEXT needs just the 4 corners as isolated points
// (rendered 1 GPU point each -- honestly small/subtle at default point
// size, not enlarged to look more dramatic than it really is).
class LineAndPointScreen : public DemoScreen {
public:
    LineAndPointScreen() : DemoScreen("PrimitiveType: LineList / LineStrip / PointListEXT") {}

    void LoadContent() override {
        effect_.emplace(GetScreenManager()->getGraphicsDeviceProperty());
        effect_->VertexColorEnabled = true;

        const Color c(255, 220, 60, 255);
        const Vector3 corners[4] = {
            Vector3(-0.6f, -0.6f, 0.0f), Vector3(0.6f, -0.6f, 0.0f),
            Vector3(0.6f, 0.6f, 0.0f), Vector3(-0.6f, 0.6f, 0.0f),
        };

        int w = 0;
        for (int e = 0; e < 4; ++e) {
            lineListVerts_[w++] = VertexPositionColor(corners[e], c);
            lineListVerts_[w++] = VertexPositionColor(corners[(e + 1) % 4], c);
        }
        for (int i = 0; i < 4; ++i) lineStripVerts_[i] = VertexPositionColor(corners[i], c);
        lineStripVerts_[4] = VertexPositionColor(corners[0], c);
        for (int i = 0; i < 4; ++i) pointVerts_[i] = VertexPositionColor(corners[i], c);
    }

    void UnloadContent() override {
        effect_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("The same square's 4 corners drawn 3 ways: LineList (8 verts, 4 separate");
        lines.push_back("edges), LineStrip (5 verts, one connected path), PointListEXT (4 verts,");
        lines.push_back("isolated points -- real XNA names this PointList; CNA's is PointListEXT).");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        effect_->World = Matrix::CreateTranslation(-1.6f, 0.0f, 0.0f);
        effect_->Apply();
        device.DrawUserPrimitives(PrimitiveType::LineList, lineListVerts_, 0, 4);

        effect_->World = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
        effect_->Apply();
        device.DrawUserPrimitives(PrimitiveType::LineStrip, lineStripVerts_, 0, 4);

        effect_->World = Matrix::CreateTranslation(1.6f, 0.0f, 0.0f);
        effect_->Apply();
        device.DrawUserPrimitives(PrimitiveType::PointListEXT, pointVerts_, 0, 4);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();

        const Color tint = mul(Color::White, TransitionAlpha());
        sb.DrawString(font, "LineList", Vector2(60.0f, 560.0f), tint);
        sb.DrawString(font, "LineStrip", Vector2(430.0f, 560.0f), tint);
        sb.DrawString(font, "PointListEXT", Vector2(770.0f, 560.0f), tint);
    }

private:
    std::optional<BasicEffect> effect_;
    VertexPositionColor lineListVerts_[8];
    VertexPositionColor lineStripVerts_[5];
    VertexPositionColor pointVerts_[4];
};

} // namespace CnaExamples::Demos::Graphics3D::PrimitiveTypesDemos
