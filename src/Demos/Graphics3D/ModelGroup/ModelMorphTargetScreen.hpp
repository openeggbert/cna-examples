// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <cstring>
#include <exception>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/MorphTargetEXT.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/ModelGroup/ModelContentHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::Model;
using Microsoft::Xna::Framework::Graphics::ModelMesh;
using Microsoft::Xna::Framework::Graphics::ModelMeshPart;
using Microsoft::Xna::Framework::Graphics::MorphTargetDataEXT;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// D3: MorphTargetEXT -- CPU-side blend-shape morphing, glTF's "weights" animation channel.
// Deliberately independent of AnimationPlayer's own bone-track timeline (see MorphTargetEXT.hpp's
// own doc comment: a morph weight track targets a mesh-instance node directly, not a skeleton
// joint), so nothing drives it automatically -- a consumer calls EvaluateMorphWeightsEXT() then
// SetMorphWeightsEXT() itself every frame, exactly as this screen does.
//
// One quad, one morph target that translates every vertex +0.5 on Y (a "jump" blend shape), with
// a LINEAR weight track: 0 -> 1 -> 0 over 2 seconds. Verified two ways: BlendMorphTargetsEXT() is
// checked directly against its own documented formula (finalPosition = base + sum(weight*delta))
// at weight 0 and weight 1, by decoding the raw blended bytes -- no GPU involved, no risk; then
// the same blend is exercised live every frame via SetMorphWeightsEXT(), which re-uploads the
// mesh part's real vertex buffer.
class ModelMorphTargetScreen : public DemoScreen {
public:
    ModelMorphTargetScreen() : DemoScreen("Model: Morph Targets (MorphTargetEXT)") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        try {
            WriteFixture();
            manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
            manager_->setRootDirectoryProperty(ModelContentDirectory());
            manager_->setGraphicsDevice(device);

            model_.emplace(manager_->Load<Model>("blob"));
            mesh_ = model_->getMeshesProperty().getCountProperty() > 0
                ? model_->getMeshesProperty()[0] : nullptr;
            part_ = (mesh_ != nullptr && mesh_->getMeshPartsProperty().getCountProperty() > 0)
                ? mesh_->getMeshPartsProperty()[0] : nullptr;
            morph_ = part_ != nullptr ? dynamic_cast<MorphTargetDataEXT*>(part_->getTagProperty()) : nullptr;
            if (morph_ == nullptr) throw std::runtime_error("mesh part has no real MorphTargetDataEXT Tag");

            // mesh_'s Effects collection is already self-maintained (see Model: Load & Traverse's
            // own finding), so the live view below gets real camera matrices with no extra code.
            basicEffect_ = part_ != nullptr ? dynamic_cast<BasicEffect*>(part_->getEffectProperty()) : nullptr;

            // Direct check of BlendMorphTargetsEXT()'s own documented formula, no GPU involved:
            // decode vertex 0's Y (stride-32 VertexPositionNormalTexture, Y at byte offset 4).
            const auto atZero = BlendMorphTargetsEXT(*morph_, {0.0f});
            const auto atOne = BlendMorphTargetsEXT(*morph_, {1.0f});
            float yAtZero = 0.0f, yAtOne = 0.0f;
            std::memcpy(&yAtZero, atZero.data() + 4, sizeof(float));
            std::memcpy(&yAtOne, atOne.data() + 4, sizeof(float));
            // Vertex 0 sits at the quad's bottom edge (Y = -0.35 in the bind pose, not 0 --
            // BuildQuadNormalTextureMesh's own vertex order), so the correct check is the DELTA
            // matching the authored +0.5 target exactly, not an assumed absolute Y.
            blendVerified_ = std::abs((yAtOne - yAtZero) - 0.5f) < 0.001f;
        } catch (const std::exception& ex) {
            error_ = ex.what();
        }
    }

    void OnDemoUnload() override {
        model_.reset();
        manager_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        elapsed_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        if (morph_ == nullptr || part_ == nullptr) return;
        const double t = std::fmod((double)elapsed_, 2.0);
        currentWeights_ = EvaluateMorphWeightsEXT(morph_->WeightTrack, t);
        SetMorphWeightsEXT(*part_, currentWeights_);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A real MorphTargetDataEXT on a loaded ModelMeshPart's Tag. One \"jump\" target (+0.5 Y);");
        lines.push_back("a LINEAR weight track (0->1->0 over 2s) is evaluated and applied by hand every frame --");
        lines.push_back("nothing drives a morph track automatically, unlike bone animation.");
        if (!error_.empty()) {
            lines.push_back("Load FAILED: " + error_);
        } else {
            lines.push_back("BlendMorphTargetsEXT() checked directly: vertex 0's Y shifted by exactly +0.5");
            lines.push_back("between weight 0 and weight 1, matching the authored target's own delta.");
            lines.push_back("Current live weight[0] = " +
                            std::to_string(currentWeights_.empty() ? 0.0f : currentWeights_[0]));
        }
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        const bool pass = error_.empty() && blendVerified_;
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: BlendMorphTargetsEXT matched the authored delta"
                        : "FAIL: see above");

        sb.End();

        if (error_.empty()) {
            auto& device = GetScreenManager()->getGraphicsDeviceProperty();
            const Viewport original = device.getViewportProperty();
            const int sceneTop = (int)end.Y + 50;
            Viewport scene(0, sceneTop, original.getWidthProperty(), original.getHeightProperty() - sceneTop - 60);
            device.setViewportProperty(scene);
            device.setDepthStencilStateProperty(DepthStencilState::Default);
            device.setRasterizerStateProperty(RasterizerState::CullNone);

            const Matrix world = Matrix::getIdentityProperty();
            const Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 0.2f, 3.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
            const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
                MathHelper::PiOver4, (float)scene.getWidthProperty() / (float)scene.getHeightProperty(), 0.1f, 100.0f);
            model_->Draw(world, view, projection);

            device.setViewportProperty(original);
            device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
            device.setDepthStencilStateProperty(DepthStencilState::None);
        }

        sb.Begin();
    }

private:
    void WriteFixture() {
        namespace fs = std::filesystem;
        const fs::path root = ModelContentDirectory();
        fs::create_directories(root);

        std::vector<std::uint8_t> verts, indices;
        BuildQuadStride32Bytes(0.7f, 0.7f, verts, indices);
        WriteBinaryFile(root / "blob_verts.bin", verts);
        WriteBinaryFile(root / "blob_idx.bin", indices);
        WriteSolidColorQoi(root / "blob.qoi", 220, 170, 60, 255);

        const auto morphBytes = BuildMorphBytes({
            MorphTargetSpec{{Vector3(0.0f, 0.5f, 0.0f), Vector3(0.0f, 0.5f, 0.0f),
                            Vector3(0.0f, 0.5f, 0.0f), Vector3(0.0f, 0.5f, 0.0f)}},
        });
        WriteBinaryFile(root / "blob_morph.bin", morphBytes);

        WriteTextFile(root / "blob.cnj", R"({
  "cnjVersion": 1,
  "type": "Model",
  "meshes": [
    {
      "name": "Blob",
      "vertices": "blob_verts.bin",
      "indices": "blob_idx.bin",
      "vertexStride": 32,
      "effect": "BasicEffect",
      "texture": "blob.qoi",
      "morphTargets": "blob_morph.bin",
      "morphWeights": [0.0],
      "morphWeightTrack": {
        "stepInterpolation": false,
        "cubicSpline": false,
        "keys": [
          {"time": 0.0, "weights": [0.0]},
          {"time": 1.0, "weights": [1.0]},
          {"time": 2.0, "weights": [0.0]}
        ]
      }
    }
  ]
})");
    }

    std::optional<ContentManager> manager_;
    std::optional<Model> model_;
    ModelMesh* mesh_ = nullptr;
    ModelMeshPart* part_ = nullptr;
    MorphTargetDataEXT* morph_ = nullptr;
    BasicEffect* basicEffect_ = nullptr;
    std::vector<float> currentWeights_;
    bool blendVerified_ = false;
    std::string error_;
    float elapsed_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos
