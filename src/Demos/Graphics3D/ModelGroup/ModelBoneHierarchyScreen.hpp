// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::ModelBone;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates a real ModelBone parent/child hierarchy (AddChild()) -- the
// classic "shoulder rotates the whole arm, elbow rotates only the
// forearm" demonstration, using 2 bones + 2 directly-drawn cubes (no full
// Model/ModelMesh wrapper -- ProceduralModelScreen.hpp already proves that
// full object graph; this screen isolates just the bone-hierarchy math).
//
// Composition order confirmed by reading Model::CopyAbsoluteBoneTransformsTo
// directly (Model.cpp), NOT assumed: `dest[i] = bone->getTransformProperty()
// * dest[parentIdx]` -- i.e. absoluteTransform = localTransform * parentAbsolute
// (the bone's own local transform on the LEFT, parent's absolute transform
// on the RIGHT). This screen mirrors that exact multiplication order by
// hand: shoulderAbsolute = shoulderLocal * baseWorld; elbowAbsolute =
// elbowLocal * shoulderAbsolute.
class ModelBoneHierarchyScreen : public DemoScreen {
public:
    ModelBoneHierarchyScreen() : DemoScreen("Model: ModelBone Hierarchy") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        shoulderMesh_ = BuildCubeColorMesh(0.4f, {
            Color(220, 60, 60, 255), Color(220, 60, 60, 255), Color(220, 60, 60, 255),
            Color(220, 60, 60, 255), Color(220, 60, 60, 255), Color(220, 60, 60, 255),
        });
        elbowMesh_ = BuildCubeColorMesh(0.3f, {
            Color(60, 110, 220, 255), Color(60, 110, 220, 255), Color(60, 110, 220, 255),
            Color(60, 110, 220, 255), Color(60, 110, 220, 255), Color(60, 110, 220, 255),
        });

        shoulderBone_.emplace(0, "Shoulder");
        elbowBone_.emplace(1, "Elbow");
        shoulderBone_->AddChild(&*elbowBone_);

        effect_.emplace(device);
        effect_->VertexColorEnabled = true;
    }

    void UnloadContent() override {
        effect_.reset();
        elbowBone_.reset();
        shoulderBone_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        time_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        // Shoulder: slow rotation, moves both cubes together.
        shoulderBone_->setTransformProperty(Matrix::CreateRotationY(time_ * 0.5f));
        // Elbow: local offset from the shoulder, plus its own faster independent
        // rotation -- moves relative to the shoulder, not the world.
        elbowBone_->setTransformProperty(Matrix::CreateRotationZ(std::sin(time_ * 2.0f) * 0.8f) *
                                         Matrix::CreateTranslation(1.0f, 0.0f, 0.0f));
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Real ModelBone hierarchy: shoulderBone.AddChild(elbowBone).");
        lines.push_back("Shoulder (red) rotates slowly, carrying the elbow (blue) with it; the");
        lines.push_back("elbow ALSO swings independently in the shoulder's own local space.");
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

        const Matrix baseWorld = Matrix::getIdentityProperty();
        // Same order as Model::CopyAbsoluteBoneTransformsTo: local * parentAbsolute.
        const Matrix shoulderAbsolute = shoulderBone_->getTransformProperty() * baseWorld;
        const Matrix elbowAbsolute = elbowBone_->getTransformProperty() * shoulderAbsolute;

        effect_->World = shoulderAbsolute;
        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, shoulderMesh_.vertices.data(), 0,
                                         (int)shoulderMesh_.vertices.size(), shoulderMesh_.indices.data(), 0,
                                         (int)shoulderMesh_.indices.size() / 3);

        effect_->World = elbowAbsolute;
        effect_->Apply();
        device.DrawUserIndexedPrimitives(PrimitiveType::TriangleList, elbowMesh_.vertices.data(), 0,
                                         (int)elbowMesh_.vertices.size(), elbowMesh_.indices.data(), 0,
                                         (int)elbowMesh_.indices.size() / 3);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    CubeColorMesh shoulderMesh_;
    CubeColorMesh elbowMesh_;
    std::optional<ModelBone> shoulderBone_;
    std::optional<ModelBone> elbowBone_;
    std::optional<BasicEffect> effect_;
    float time_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos
