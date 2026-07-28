// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/OcclusionQuery.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::TexturesAndQueriesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::OcclusionQuery;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// OcclusionQuery counts how many pixels a draw call actually wrote -- after
// the depth test, which is the whole point. It is how a game decides not to
// bother with an expensive object that turned out to be behind a wall.
//
// A big red occluder sits between the camera and a small blue cube. Space
// moves the occluder out of the way. The pixel count for the blue cube is
// measured either way, so the two numbers can be compared directly.
//
// Two things about the API are easy to get wrong and are shown honestly:
//
//  * The result is ASYNCHRONOUS. End() submits the query; the answer is not
//    there yet. Polling getIsCompleteProperty() before reading
//    getPixelCountProperty() is mandatory -- reading early gives a stale
//    number, not a fresh one. This screen keeps exactly one query in flight
//    and shows how many frames it waited.
//  * On OpenGL ES 3.0 the count is not a count. The spec only guarantees
//    "any samples passed", so CNA reports 0 or 1 there. The screen prints the
//    raw value rather than pretending it is always a pixel total.
class OcclusionQueryScreen : public DemoScreen {
public:
    OcclusionQueryScreen() : DemoScreen("OcclusionQuery: Occluded vs Visible") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        target_ = BuildCubeColorMesh(0.6f, std::array<Color, 6>{
            Color(70, 130, 240, 255), Color(60, 110, 210, 255), Color(90, 150, 250, 255),
            Color(50, 95, 190, 255), Color(80, 140, 245, 255), Color(55, 105, 200, 255)});
        occluder_ = BuildCubeColorMesh(1.4f, std::array<Color, 6>{
            Color(210, 70, 60, 255), Color(180, 55, 48, 255), Color(225, 85, 70, 255),
            Color(165, 48, 42, 255), Color(215, 75, 62, 255), Color(175, 52, 45, 255)});
        effect_.emplace(device);
        effect_->VertexColorEnabled = true;
        query_.emplace(device);
    }

    void OnDemoUnload() override {
        query_.reset();
        effect_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            occluderEnabled_ = !occluderEnabled_;
        }
    }

    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.6f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A: occluder is " +
                        std::string(occluderEnabled_ ? "IN THE WAY" : "moved aside"));
        lines.emplace_back();
        lines.push_back("Pixels of the blue cube that survived the depth test:");
        lines.push_back("  occluder in the way : " + Report(occludedResult_));
        lines.push_back("  occluder aside      : " + Report(visibleResult_));
        lines.emplace_back();
        lines.push_back("The result is asynchronous: End() submits the query, it is not the");
        lines.push_back("answer. getIsCompleteProperty() must be polled before reading the");
        lines.push_back("count -- this one waited " + std::to_string(lastWaitFrames_) + " frame(s).");
        lines.push_back("On OpenGL ES 3.0 the value is 0 or 1, not a pixel total -- the spec only");
        lines.push_back("guarantees \"any samples passed\", so the raw number is shown as-is.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        // The screen claims occlusion reduces the count, so it checks it rather
        // than leaving two numbers on screen for the reader to trust.
        const bool measuredBoth = occludedResult_ >= 0 && visibleResult_ >= 0;
        const bool claimHolds = measuredBoth && occludedResult_ < visibleResult_;
        FillRect(sb, Rectangle(40, (int)end.Y + 6, 24, 24),
                 mul(!measuredBoth ? Color(150, 150, 150, 255)
                                   : claimHolds ? Color(40, 200, 90, 255)
                                                : Color(220, 60, 60, 255),
                     TransitionAlpha()));
        sb.DrawString(font,
                      !measuredBoth
                          ? "Press Space to measure the other case."
                          : claimHolds ? "Verified: the occluded count is lower."
                                       : "Occlusion did NOT reduce the count on this backend.",
                      Vector2(76.0f, end.Y + 6.0f), tint);

        sb.End();
        DrawScene();
        sb.Begin();
    }

private:
    static std::string Report(int value) {
        return value < 0 ? "(not measured yet)" : std::to_string(value);
    }

    void DrawScene() {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 340, original.getWidthProperty(), 240);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.Clear(Microsoft::Xna::Framework::Graphics::ClearOptions::DepthBuffer,
                     Color::Black, 1.0f, 0);

        effect_->View = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 6.0f), Vector3::Zero,
                                             Vector3(0.0f, 1.0f, 0.0f));
        effect_->Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        // The occluder goes down FIRST, so its depth is already in the buffer
        // when the query runs. Query the other order and the count is always
        // "fully visible" -- the depth test has nothing to reject against yet.
        if (occluderEnabled_) {
            effect_->World = Matrix::CreateTranslation(Vector3(0.0f, 0.0f, 2.2f));
            DrawMesh(device, occluder_);
        }

        // Exactly one query in flight: Begin/End on a query still awaiting its
        // result would discard the pending one.
        const bool starting = !pending_;
        if (starting) {
            queriedWithOccluder_ = occluderEnabled_;
            waitFrames_ = 0;
            query_->Begin();
        }

        effect_->World = Matrix::CreateRotationY(spin_) * Matrix::CreateRotationX(spin_ * 0.4f);
        DrawMesh(device, target_);

        if (starting) {
            query_->End();
            pending_ = true;
        } else {
            ++waitFrames_;
            if (query_->getIsCompleteProperty()) {
                const int count = query_->getPixelCountProperty();
                (queriedWithOccluder_ ? occludedResult_ : visibleResult_) = count;
                lastWaitFrames_ = waitFrames_;
                pending_ = false;
            }
        }

        device.setViewportProperty(original);
        device.setDepthStencilStateProperty(DepthStencilState::None);
    }

    void DrawMesh(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
              const CubeColorMesh& mesh) {
        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, mesh.vertices.data(), 0,
                                         (int)mesh.vertices.size(), mesh.indices.data(), 0,
                                         (int)mesh.indices.size() / 3);
    }

    CubeColorMesh target_;
    CubeColorMesh occluder_;
    std::optional<BasicEffect> effect_;
    std::optional<OcclusionQuery> query_;
    float spin_ = 0.0f;
    bool occluderEnabled_ = true;
    bool pending_ = false;
    bool queriedWithOccluder_ = true;
    int waitFrames_ = 0;
    int lastWaitFrames_ = 0;
    int occludedResult_ = -1;
    int visibleResult_ = -1;
};

} // namespace CnaExamples::Demos::Graphics3D::TexturesAndQueriesDemos
