// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics3D::BuffersDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::BufferUsage;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer;
using Microsoft::Xna::Framework::Graphics::IndexBuffer;
using Microsoft::Xna::Framework::Graphics::IndexElementSize;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::SetDataOptions;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates DynamicVertexBuffer: a 12x12 grid of VertexPositionColor
// whose Y positions are recomputed into a sine ripple every frame and
// re-uploaded via SetData(..., SetDataOptions::Discard) -- a real,
// continuously-live CPU->GPU upload every frame, not a static mesh.
//
// Real C++ gotcha worth documenting: DynamicVertexBuffer declares its own
// 4-argument SetData(data, startIndex, count, SetDataOptions) without a
// `using VertexBuffer::SetData;` -- this HIDES the base VertexBuffer's
// 2-argument SetData(data, count) overload for DynamicVertexBuffer objects
// specifically (plain name hiding, not overload resolution -- the base
// overload isn't even a candidate). The 4-argument form is mandatory here.
class DynamicVertexBufferScreen : public DemoScreen {
public:
    static constexpr int kGridSize = 12;

    DynamicVertexBufferScreen() : DemoScreen("Buffers: DynamicVertexBuffer Ripple") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const int vertCount = kGridSize * kGridSize;
        vertices_.resize((std::size_t)vertCount, VertexPositionColor(Vector3::Zero, Color::White));

        std::vector<std::uint16_t> indices;
        for (int z = 0; z < kGridSize - 1; ++z) {
            for (int x = 0; x < kGridSize - 1; ++x) {
                const std::uint16_t i0 = (std::uint16_t)(z * kGridSize + x);
                const std::uint16_t i1 = (std::uint16_t)(z * kGridSize + x + 1);
                const std::uint16_t i2 = (std::uint16_t)((z + 1) * kGridSize + x);
                const std::uint16_t i3 = (std::uint16_t)((z + 1) * kGridSize + x + 1);
                indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
                indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
            }
        }
        indexCount_ = (int)indices.size();
        ib_.emplace(device, IndexElementSize::SixteenBits, indexCount_, BufferUsage::None);
        ib_->SetData(indices.data(), indexCount_);

        dvb_.emplace(device, VertexPositionColor::getVertexDeclarationStatic(), vertCount, BufferUsage::None);

        effect_.emplace(device);
        effect_->VertexColorEnabled = true;
    }

    void UnloadContent() override {
        effect_.reset();
        dvb_.reset();
        ib_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        phase_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        const float spacing = 2.4f / (float)(kGridSize - 1);
        for (int z = 0; z < kGridSize; ++z) {
            for (int x = 0; x < kGridSize; ++x) {
                const float worldX = -1.2f + (float)x * spacing;
                const float worldZ = -1.2f + (float)z * spacing;
                const float dist = std::sqrt(worldX * worldX + worldZ * worldZ);
                const float y = std::sin(dist * 4.0f - phase_ * 2.5f) * 0.2f;
                const int g = (int)(140 + y / 0.2f * 100.0f);
                vertices_[(std::size_t)(z * kGridSize + x)] =
                    VertexPositionColor(Vector3(worldX, y, worldZ), Color(60, g, 220, 255));
            }
        }
        dvb_->SetData(vertices_.data(), 0, (int)vertices_.size(), SetDataOptions::Discard);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A 12x12 DynamicVertexBuffer grid, Y positions recomputed into a sine");
        lines.push_back("ripple every frame and re-uploaded via SetData(..., Discard).");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 170, original.getWidthProperty(), 400);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->World = Matrix::getIdentityProperty();
        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 2.2f, 3.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        device.SetVertexBuffer(&*dvb_);
        device.SetIndexBuffer(&*ib_);
        effect_->Apply();
        device.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0, (int)vertices_.size(), 0, indexCount_ / 3);
        device.SetVertexBuffer(nullptr);
        device.SetIndexBuffer(nullptr);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    std::vector<VertexPositionColor> vertices_;
    std::optional<DynamicVertexBuffer> dvb_;
    std::optional<IndexBuffer> ib_;
    std::optional<BasicEffect> effect_;
    int indexCount_ = 0;
    float phase_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::BuffersDemos
