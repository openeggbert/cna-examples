// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::BuffersDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::BufferUsage;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::IndexBuffer;
using Microsoft::Xna::Framework::Graphics::IndexElementSize;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::VertexBuffer;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates the same cube geometry drawn 2 ways: immediate mode
// (GraphicsDevice::DrawUserIndexedPrimitives, no VertexBuffer/IndexBuffer
// objects -- the vertex/index arrays are uploaded to a transient buffer
// every single call) vs. buffered mode (an explicit VertexBuffer +
// IndexBuffer created once in LoadContent, bound via SetVertexBuffer/
// SetIndexBuffer, drawn via DrawIndexedPrimitives every frame) -- visually
// identical, different GPU upload cost per frame.
class ImmediateVsBufferedScreen : public DemoScreen {
public:
    ImmediateVsBufferedScreen() : DemoScreen("Buffers: Immediate vs. Buffered Draw") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeColorMesh(0.8f, {
            Color(220, 60, 60, 255), Color(60, 220, 90, 255), Color(60, 110, 220, 255),
            Color(220, 200, 60, 255), Color(200, 60, 220, 255), Color(60, 220, 210, 255),
        });

        vb_.emplace(device, VertexPositionColor::getVertexDeclarationStatic(),
                    (int)mesh_.vertices.size(), BufferUsage::None);
        vb_->SetData(mesh_.vertices.data(), (int)mesh_.vertices.size());
        ib_.emplace(device, IndexElementSize::SixteenBits, (int)mesh_.indices.size(), BufferUsage::None);
        ib_->SetData(mesh_.indices.data(), (int)mesh_.indices.size());

        effect_.emplace(device);
        effect_->VertexColorEnabled = true;
    }

    void UnloadContent() override {
        effect_.reset();
        ib_.reset();
        vb_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("The same cube, drawn 2 ways:");
        lines.push_back("Immediate: DrawUserIndexedPrimitives, no buffer objects.");
        lines.push_back("Buffered: VertexBuffer+IndexBuffer, bound + drawn every frame.");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 1.2f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);
        const Matrix spin = Matrix::CreateRotationY(spin_);

        effect_->World = Matrix::CreateTranslation(-1.4f, 0.0f, 0.0f) * spin;
        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                         (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                         (int)mesh_.indices.size() / 3);

        device.SetVertexBuffer(&*vb_);
        device.SetIndexBuffer(&*ib_);
        effect_->World = Matrix::CreateTranslation(1.4f, 0.0f, 0.0f) * spin;
        effect_->Apply();
        device.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0,
                                     (int)mesh_.vertices.size(), 0, (int)mesh_.indices.size() / 3);
        device.SetVertexBuffer(nullptr);
        device.SetIndexBuffer(nullptr);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();

        const Color tint = mul(Color::White, TransitionAlpha());
        sb.DrawString(font, "Immediate", Vector2(180.0f, 560.0f), tint);
        sb.DrawString(font, "Buffered", Vector2(620.0f, 560.0f), tint);
    }

private:
    CubeColorMesh mesh_;
    std::optional<VertexBuffer> vb_;
    std::optional<IndexBuffer> ib_;
    std::optional<BasicEffect> effect_;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::BuffersDemos
