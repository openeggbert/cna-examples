// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
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
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::ShaderEffect;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates that CNA surfaces a genuine GLSL compile failure through
// ShaderEffect::IsEffectValid() rather than crashing or silently ignoring
// it: the ShaderEffect constructor does not throw on a bad shader (checked
// directly against ShaderEffect.cpp -- no throw anywhere in it), it simply
// leaves the effect in a "compiled=false" state. This screen constructs
// one with deliberately broken GLSL (a fragment shader referencing an
// undeclared variable), confirms IsEffectValid() is false, and -- per
// this API's own contract -- never calls Apply() or issues a draw call
// with it (drawing through an unlinked/invalid shader program is
// undefined). A separate, genuinely valid BasicEffect cube is drawn
// alongside so the screen still shows real 3D content, not a blank
// viewport.
class InvalidShaderScreen : public DemoScreen {
public:
    InvalidShaderScreen() : DemoScreen("ShaderEffect: Invalid Shader Handling") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        mesh_ = BuildCubeColorMesh(1.0f, {
            Color(220, 60, 60, 255), Color(60, 220, 90, 255), Color(60, 110, 220, 255),
            Color(220, 200, 60, 255), Color(200, 60, 220, 255), Color(60, 220, 210, 255),
        });
        goodEffect_.emplace(device);
        goodEffect_->VertexColorEnabled = true;

        static const char* kVertSrc = R"(#version 300 es
precision highp float;
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;
void main() {
    gl_Position = Projection * View * World * vec4(aPosition, 1.0);
}
)";
        // Deliberately broken: references an undeclared variable
        // (`undeclaredValue`) and is missing a statement terminator --
        // this must fail to compile/link, not silently succeed.
        static const char* kBrokenFragSrc = R"(#version 300 es
precision highp float;
out vec4 FragColor;
void main() {
    FragColor = vec4(undeclaredValue, 0.0, 0.0, 1.0)
}
)";
        badEffect_.emplace(device, kVertSrc, kBrokenFragSrc);
    }

    void UnloadContent() override {
        badEffect_.reset();
        goodEffect_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A ShaderEffect built from deliberately broken GLSL (undeclared variable +");
        lines.push_back("missing semicolon). The constructor does NOT throw; it surfaces the real");
        lines.push_back("compile failure through IsEffectValid() instead -- never Apply()'d/drawn.");
        lines.push_back("badEffect_.IsEffectValid(): " + std::string(badEffect_->IsEffectValid() ? "true" : "false"));
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 210, original.getWidthProperty(), 360);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        goodEffect_->World = Matrix::CreateRotationY(spin_);
        goodEffect_->View = Matrix::CreateLookAt(Vector3(0.0f, 1.2f, 4.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        goodEffect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        // Only the GOOD effect is ever applied/drawn with -- badEffect_ is
        // constructed purely to prove IsEffectValid() reports the failure.
        goodEffect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh_.vertices.data(), 0,
                                         (int)mesh_.vertices.size(), mesh_.indices.data(), 0,
                                         (int)mesh_.indices.size() / 3);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    CubeColorMesh mesh_;
    std::optional<BasicEffect> goodEffect_;
    std::optional<ShaderEffect> badEffect_;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::CustomShaderDemos
