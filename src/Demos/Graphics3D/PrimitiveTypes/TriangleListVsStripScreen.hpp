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

// Demonstrates PrimitiveType::TriangleList vs. TriangleStrip drawing the
// exact same 4-triangle zigzag band shape: TriangleList needs 12 explicit
// vertices (3 per triangle, no sharing), TriangleStrip needs only 6 (each
// new vertex plus the previous 2 forms the next triangle) -- same visual
// result, different vertex economy. Both drawn side by side so the
// equivalence is provable, not just asserted.
class TriangleListVsStripScreen : public DemoScreen {
public:
    TriangleListVsStripScreen() : DemoScreen("PrimitiveType: TriangleList vs. TriangleStrip") {}

    void LoadContent() override {
        effect_.emplace(GetScreenManager()->getGraphicsDeviceProperty());
        effect_->VertexColorEnabled = true;

        // A 6-vertex zigzag band: alternating top/bottom Y, forming 4
        // triangles when read as a TriangleStrip.
        const Color colors[6] = {
            Color(220, 60, 60, 255), Color(60, 220, 90, 255), Color(60, 110, 220, 255),
            Color(220, 200, 60, 255), Color(200, 60, 220, 255), Color(60, 220, 210, 255),
        };
        for (int i = 0; i < 6; ++i) {
            const float x = -1.0f + (float)i * 0.4f;
            const float y = (i % 2 == 0) ? 0.6f : -0.6f;
            stripVerts_[i] = VertexPositionColor(Vector3(x, y, 0.0f), colors[i]);
        }
        // The same shape expanded into 4 independent triangles (12 verts) --
        // triangle t uses stripVerts_[t], stripVerts_[t+1], stripVerts_[t+2],
        // matching exactly how a TriangleStrip itself reads consecutive
        // vertices, just materialized explicitly per triangle.
        int w = 0;
        for (int t = 0; t < 4; ++t) {
            listVerts_[w++] = stripVerts_[t];
            listVerts_[w++] = stripVerts_[t + 1];
            listVerts_[w++] = stripVerts_[t + 2];
        }
    }

    void UnloadContent() override {
        effect_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Same 4-triangle zigzag band drawn 2 ways: TriangleList needs 12 explicit");
        lines.push_back("vertices; TriangleStrip needs only 6 (each vertex reuses the previous 2).");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 170, original.getWidthProperty(), 400);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        const Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);
        effect_->View = view;
        effect_->Projection = projection;

        effect_->World = Matrix::CreateTranslation(-1.1f, 0.6f, 0.0f);
        effect_->Apply();
        device.DrawUserPrimitives(PrimitiveType::TriangleList, listVerts_, 0, 4);

        effect_->World = Matrix::CreateTranslation(1.1f, -0.6f, 0.0f);
        effect_->Apply();
        device.DrawUserPrimitives(PrimitiveType::TriangleStrip, stripVerts_, 0, 4);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();

        sb.DrawString(font, "TriangleList (12 verts)", Vector2(40.0f, 190.0f), mul(Color::White, TransitionAlpha()));
        sb.DrawString(font, "TriangleStrip (6 verts)", Vector2(560.0f, 490.0f), mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<BasicEffect> effect_;
    VertexPositionColor stripVerts_[6];
    VertexPositionColor listVerts_[12];
};

} // namespace CnaExamples::Demos::Graphics3D::PrimitiveTypesDemos
