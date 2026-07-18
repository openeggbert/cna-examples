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
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
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
using Microsoft::Xna::Framework::Graphics::BufferUsage;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::IndexBuffer;
using Microsoft::Xna::Framework::Graphics::IndexElementSize;
using Microsoft::Xna::Framework::Graphics::Model;
using Microsoft::Xna::Framework::Graphics::ModelBone;
using Microsoft::Xna::Framework::Graphics::ModelMesh;
using Microsoft::Xna::Framework::Graphics::ModelMeshPart;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::VertexBuffer;
using Microsoft::Xna::Framework::Graphics::Viewport;

// Demonstrates a real Model/ModelMesh/ModelMeshPart/ModelBone object graph
// built entirely BY HAND -- no XNB, no content pipeline, matching this
// app's "everything procedural" discipline. 2 ModelMeshParts (2 separate
// small cubes, each its own VertexBuffer+IndexBuffer+BasicEffect) are
// combined into ONE ModelMesh, one root ModelBone, one Model -- then
// model_->Draw(world, view, projection) is called every frame, proving the
// real Model API works end to end without any asset.
//
// Non-obvious real API detail (confirmed by reading Model.cpp directly,
// not assumed): Model::Draw() updates World/View/Projection on each entry
// of ModelMesh::getEffectsProperty() (a separate ModelEffectCollection),
// NOT on each ModelMeshPart's own individually-assigned Effect. Setting a
// part's Effect via setEffectProperty() alone is not enough for
// Model::Draw() to ever touch its matrices -- the SAME Effect pointers
// must ALSO be added via mesh->getEffectsPropertyMutable().Add(effect) or
// Model::Draw() silently leaves them at their construction-time identity
// matrices forever. Both effects_[0]/[1] below are added to both places.
//
// Object-lifetime note: Model/ModelMesh/ModelMeshPart/ModelBone all store
// raw, non-owning pointers to each other (matching FNA's own C# reference
// semantics) -- every owned object (buffers, effects, mesh parts, the
// mesh, the bone) is kept alive as a plain member field for as long as
// this screen exists, never a temporary.
class ProceduralModelScreen : public DemoScreen {
public:
    ProceduralModelScreen() : DemoScreen("Model: Procedural Construction") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        // Model::Draw(world,view,projection) applies a SINGLE shared world
        // matrix to every ModelMeshPart -- there is no per-part transform
        // without a real multi-bone hierarchy (deliberately out of scope for
        // this screen, which uses just 1 root bone). Both cubes were
        // originally built around their own local origin with no offset,
        // so they rendered exactly on top of each other (confirmed via
        // Xvfb screenshot: only the second-drawn blue cube was visible at
        // all). Fixed by baking a fixed lateral offset directly into each
        // mesh's own vertex data, the same technique already used to work
        // around BasicEffect's object-space-only fog calculation in
        // Fog/DistanceFogScreen.hpp.
        CubeColorMesh cubeA = BuildCubeColorMesh(0.5f, {
            Color(220, 60, 60, 255), Color(220, 60, 60, 255), Color(220, 60, 60, 255),
            Color(220, 60, 60, 255), Color(220, 60, 60, 255), Color(220, 60, 60, 255),
        });
        CubeColorMesh cubeB = BuildCubeColorMesh(0.5f, {
            Color(60, 110, 220, 255), Color(60, 110, 220, 255), Color(60, 110, 220, 255),
            Color(60, 110, 220, 255), Color(60, 110, 220, 255), Color(60, 110, 220, 255),
        });
        for (auto& v : cubeA.vertices) v.Position.X -= 0.7f;
        for (auto& v : cubeB.vertices) v.Position.X += 0.7f;

        // Reserve upfront so no later emplace_back() ever reallocates -- every raw pointer
        // taken into these vectors below (for ModelMeshPart/ModelMesh/Model's own raw-pointer
        // constructors) stays valid for the vector's lifetime. BasicEffect itself has a private
        // copy constructor (used only by Clone()) and no declared move constructor, so
        // std::vector<BasicEffect> cannot compile reserve()/reallocation at all (confirmed by a
        // real compile error) -- effectA_/effectB_ are plain std::optional<BasicEffect> members
        // instead, matching the single-effect pattern used everywhere else in this Area.
        vertexBuffers_.reserve(2);
        indexBuffers_.reserve(2);
        meshParts_.reserve(2);

        vertexBuffers_.emplace_back(device, VertexPositionColor::getVertexDeclarationStatic(),
                                     (int)cubeA.vertices.size(), BufferUsage::None);
        vertexBuffers_.back().SetData(cubeA.vertices.data(), (int)cubeA.vertices.size());
        vertexBuffers_.emplace_back(device, VertexPositionColor::getVertexDeclarationStatic(),
                                     (int)cubeB.vertices.size(), BufferUsage::None);
        vertexBuffers_.back().SetData(cubeB.vertices.data(), (int)cubeB.vertices.size());

        indexBuffers_.emplace_back(device, IndexElementSize::SixteenBits, (int)cubeA.indices.size(), BufferUsage::None);
        indexBuffers_.back().SetData(cubeA.indices.data(), (int)cubeA.indices.size());
        indexBuffers_.emplace_back(device, IndexElementSize::SixteenBits, (int)cubeB.indices.size(), BufferUsage::None);
        indexBuffers_.back().SetData(cubeB.indices.data(), (int)cubeB.indices.size());

        effectA_.emplace(device);
        effectA_->VertexColorEnabled = true;
        effectB_.emplace(device);
        effectB_->VertexColorEnabled = true;

        meshParts_.emplace_back(&vertexBuffers_[0], &indexBuffers_[0], (int)cubeA.vertices.size(),
                                (int)cubeA.indices.size() / 3, 0, 0);
        meshParts_[0].setEffectProperty(&*effectA_);
        meshParts_.emplace_back(&vertexBuffers_[1], &indexBuffers_[1], (int)cubeB.vertices.size(),
                                (int)cubeB.indices.size() / 3, 0, 0);
        meshParts_[1].setEffectProperty(&*effectB_);

        mesh_.emplace(&device, std::vector<ModelMeshPart*>{&meshParts_[0], &meshParts_[1]});
        mesh_->getEffectsPropertyMutable().Add(&*effectA_);
        mesh_->getEffectsPropertyMutable().Add(&*effectB_);

        bone_.emplace(0, "Root");

        model_.emplace(&device, std::vector<ModelBone*>{&*bone_}, std::vector<ModelMesh*>{&*mesh_});
    }

    void UnloadContent() override {
        model_.reset();
        bone_.reset();
        mesh_.reset();
        meshParts_.clear();
        effectB_.reset();
        effectA_.reset();
        indexBuffers_.clear();
        vertexBuffers_.clear();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A real Model, built by hand: 1 ModelMesh, 2 ModelMeshParts (red+blue");
        lines.push_back("cubes, each its own VertexBuffer+IndexBuffer+Effect), 1 root ModelBone.");
        lines.push_back("No content pipeline, no asset -- model_->Draw(world,view,proj) each frame.");
        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Viewport original = device.getViewportProperty();
        Viewport scene(0, 190, original.getWidthProperty(), 380);
        device.setViewportProperty(scene);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        const Matrix world = Matrix::CreateRotationY(spin_);
        const Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 1.0f, 3.5f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);

        model_->Draw(world, view, projection);

        device.setViewportProperty(original);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        sb.Begin();
    }

private:
    std::vector<VertexBuffer> vertexBuffers_;
    std::vector<IndexBuffer> indexBuffers_;
    std::optional<BasicEffect> effectA_;
    std::optional<BasicEffect> effectB_;
    std::vector<ModelMeshPart> meshParts_;
    std::optional<ModelMesh> mesh_;
    std::optional<ModelBone> bone_;
    std::optional<Model> model_;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos
