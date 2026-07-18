// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::CustomShaderDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::ShaderEffect;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates ShaderEffect (NOXNA): a hand-authored GLSL vertex+fragment
// shader pair, compiled live at runtime from source strings (not a file
// path), applied to a rotating VertexPositionColor cube. The vertex
// attribute layout (location 0 = vec3 position, location 1 = vec4 color)
// matches exactly what EasyGL's ApplyLayout() binds for the typed
// VertexPositionColor draw path's 16-byte packed stride (confirmed by
// reading EasyGLGraphicsBackend.cpp's stride-16 case directly) -- vertex
// attribute bindings live on the VAO, independent of whichever shader
// program is currently applied, so this custom shader reads the same
// position/color data BasicEffect would. World/View/Projection are
// forwarded automatically as uniforms (ShaderEffect implements
// IEffectMatrices) -- only the fragment shader's own custom uniform
// (a live time value, mixed into the output color for a pulsing tint) is
// set manually via SetUniformFloat() every frame.
class CustomGlslShaderScreen : public DemoScreen {
public:
    CustomGlslShaderScreen() : DemoScreen("ShaderEffect: Custom GLSL Shader") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeColorMesh(1.0f, {
            Color(220, 60, 60, 255), Color(60, 220, 90, 255), Color(60, 110, 220, 255),
            Color(220, 200, 60, 255), Color(200, 60, 220, 255), Color(60, 220, 210, 255),
        });

        static const char* kVertSrc = R"(#version 300 es
precision highp float;
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
out vec4 vColor;
uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;
void main() {
    gl_Position = Projection * View * World * vec4(aPosition, 1.0);
    vColor = aColor;
}
)";
        static const char* kFragSrc = R"(#version 300 es
precision highp float;
in vec4 vColor;
out vec4 FragColor;
uniform float uTime;
void main() {
    float pulse = 0.5 + 0.5 * sin(uTime * 2.0);
    FragColor = vec4(mix(vColor.rgb, vec3(1.0, 1.0, 1.0), pulse * 0.4), vColor.a);
}
)";
        effect_.emplace(device, kVertSrc, kFragSrc);
    }

    void UnloadContent() override {
        effect_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        time_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("ShaderEffect: hand-authored GLSL, compiled live from source strings.");
        lines.push_back("A custom uTime uniform pulses the cube's color toward white every frame.");
        lines.push_back("Shader compiled: " + std::string(effect_->IsEffectValid() ? "true" : "false"));
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        effect_->setWorldProperty(Matrix::CreateRotationY(spin_));
        effect_->setViewProperty(Matrix::CreateLookAt(Vector3(0.0f, 1.2f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f)));
        effect_->setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f));
        effect_->SetUniformFloat("uTime", time_);

        if (effect_->IsEffectValid()) {
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
    std::optional<ShaderEffect> effect_;
    float spin_ = 0.0f;
    float time_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::CustomShaderDemos
