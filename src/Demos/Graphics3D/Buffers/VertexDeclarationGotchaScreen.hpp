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
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::BuffersDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates a REAL, confirmed silent-corruption trap (traced end to end
// through GraphicsDevice::DrawUserIndexedPrimitives and EasyGL's
// ApplyLayout() stride-dispatch switch before writing this screen): the
// generic void*+VertexDeclaration overload uploads raw bytes at whatever
// stride the passed VertexDeclaration reports -- but
// VertexPositionColor::getVertexDeclarationStatic() reports
// sizeof(VertexPositionColor) as that stride, which in this C++ build is
// 40 bytes (Color gained virtual bases -- IPackedVector + IEquatable --
// long after XNA's tightly-packed 16-byte layout), NOT one of EasyGL's
// backend-recognized strides (16/20/24/32/48/52/56/68). An unrecognized
// stride falls into ApplyLayout()'s default case (EasyGLGraphicsBackend
// .cpp's ApplyLayout(), "unknown stride" branch), which enables ONLY the
// Position attribute -- Color is never enabled at all, so the shader reads
// OpenGL's default generic-attribute value for it, which renders as solid
// black. Verified live: the "unsafe" cube on the right renders completely
// invisible against this screen's black background -- not just
// "wrong-looking", genuinely vanished, no exception, no error log above
// debug level. The fix is simple: never pair a typed vertex struct's own
// declaration with the raw void*+VertexDeclaration overload -- use the
// dedicated typed overload instead (which re-packs to a compact,
// recognized GPU layout internally).
class VertexDeclarationGotchaScreen : public DemoScreen {
public:
    VertexDeclarationGotchaScreen() : DemoScreen("Buffers: VertexDeclaration Stride Gotcha") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeColorMesh(0.8f, {
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
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Left: safe typed DrawUserIndexedPrimitives(VertexPositionColor*, ...).");
        lines.push_back("Right: the SAME data via the raw void*+VertexDeclaration overload, using");
        lines.push_back("VertexPositionColor's OWN declaration -- its stride (40, C++-bloated) is");
        lines.push_back("unrecognized, so Color is never enabled -- the cube silently VANISHES.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, (int)end.Y + 20, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 1.2f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);
        const Matrix spin = Matrix::CreateRotationY(spin_);

        // Safe: the typed overload re-packs to a compact, recognized layout.
        effect_->World = Matrix::CreateTranslation(-1.4f, 0.0f, 0.0f) * spin;
        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                         (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                         (int)mesh_.indices.size() / 3);

        // Unsafe: raw bytes at VertexPositionColor's OWN (bloated, unrecognized) stride.
        effect_->World = Matrix::CreateTranslation(1.4f, 0.0f, 0.0f) * spin;
        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                         (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                         (int)mesh_.indices.size() / 3,
                                         VertexPositionColor::getVertexDeclarationStatic());

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();

        const Color tint = mul(Color::White, TransitionAlpha());
        sb.DrawString(font, "Typed overload (correct)", Vector2(40.0f, (float)original.getHeightProperty() - 60.0f), tint);
        sb.DrawString(font, "Raw+VertexDecl (vanished!)",
                      Vector2(540.0f, (float)original.getHeightProperty() - 60.0f), tint);
    }

private:
    CubeColorMesh mesh_;
    std::optional<BasicEffect> effect_;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::BuffersDemos
