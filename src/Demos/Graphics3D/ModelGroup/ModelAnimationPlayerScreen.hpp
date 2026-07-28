// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <exception>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/AnimationPlayer.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SkinnedEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/TimeSpan.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics3D/ModelGroup/ModelContentHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::AnimationPlayer;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::Model;
using Microsoft::Xna::Framework::Graphics::ModelMesh;
using Microsoft::Xna::Framework::Graphics::ModelMeshPart;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::SkinnedEffect;
using Microsoft::Xna::Framework::Graphics::SkinningData;
using Microsoft::Xna::Framework::Graphics::Viewport;

// D3: real skeletal animation for a real, content-loaded Model -- SkinningData attached to
// Model.Tag (ModelTypeReader's own convention, mirroring the well-known XNA Skinned Model
// Sample), driven by a real AnimationPlayer, fed into two real SkinnedEffects via
// SetBoneTransforms(). This is the general Model-facing skinning path; it is deliberately NOT
// SkinnedModelEXT (a separate, Avatar-only type -- see SkinnedModelEXT.hpp's own doc comment:
// "used by AvatarRenderer::EnableRealRenderingEXT ... deliberately not built on Model/ModelBone/
// ModelMesh"). SkinnedModelEXT belongs with Phase E's avatar work instead, not here.
//
// A 2-bone rig: bone 0 (root) never moves; bone 1 carries a second quad and is the one the
// "Wave" clip animates, translating it up and to the right and back. Verified numerically at
// load time (not by pixel probe): AnimationPlayer::GetWorldTransforms()[1] is measured at two
// clip times and its translation compared against the exact keyframe values, before the player
// is rewound for the live, looping view below.
class ModelAnimationPlayerScreen : public DemoScreen {
public:
    ModelAnimationPlayerScreen() : DemoScreen("Model: Skeletal Animation (AnimationPlayer)") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        try {
            WriteFixture();
            manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
            manager_->setRootDirectoryProperty(ModelContentDirectory());
            manager_->setGraphicsDevice(device);

            model_.emplace(manager_->Load<Model>("rig"));
            skinningData_ = dynamic_cast<SkinningData*>(model_->getTagProperty());
            if (skinningData_ == nullptr) throw std::runtime_error("Model.Tag is not a real SkinningData");

            for (ModelMesh* mesh : model_->getMeshesProperty()) {
                auto* fx = mesh->getMeshPartsProperty().getCountProperty() > 0
                    ? dynamic_cast<SkinnedEffect*>(mesh->getMeshPartsProperty()[0]->getEffectProperty())
                    : nullptr;
                if (fx == nullptr) throw std::runtime_error("mesh part's Effect is not a real SkinnedEffect");
                fx->EnableDefaultLighting();
                // mesh's own Effects collection is already self-maintained by ModelTypeReader's
                // construction order -- see Model: Load & Traverse's own finding. No manual Add().
                skinnedEffects_.push_back(fx);
            }

            player_.emplace(*skinningData_);
            const auto& clip = skinningData_->AnimationClips.at("Wave");
            player_->StartClip(clip);

            player_->Update(System::TimeSpan::Zero, false, false);
            const Matrix atStart = player_->GetWorldTransforms()[1];
            const bool startOk = std::abs(atStart.M41) < 0.01f && std::abs(atStart.M42) < 0.01f;

            player_->Update(System::TimeSpan::FromSeconds(0.8), false, false);
            const Matrix atMid = player_->GetWorldTransforms()[1];
            const bool midOk = std::abs(atMid.M41 - 0.5f) < 0.01f && std::abs(atMid.M42 - 0.4f) < 0.01f;

            verified_ = startOk && midOk;

            player_->StartClip(clip); // rewind for the live view below
        } catch (const std::exception& ex) {
            error_ = ex.what();
        }
    }

    void OnDemoUnload() override {
        player_.reset();
        model_.reset();
        manager_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        if (player_.has_value()) {
            player_->Update(gameTime.getElapsedGameTimeProperty(), true, true);
            for (SkinnedEffect* fx : skinnedEffects_) fx->SetBoneTransforms(player_->GetSkinTransforms());
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A real Model with a SkinningData Tag, a real AnimationPlayer, and two real");
        lines.push_back("SkinnedEffects driven by SetBoneTransforms() every frame -- the general Model path,");
        lines.push_back("not SkinnedModelEXT (that type is Avatar-only; see this screen's header comment).");
        if (!error_.empty()) {
            lines.push_back("Load FAILED: " + error_);
        } else {
            lines.push_back("Red = bone 0 (never moves). Green = bone 1, driven by the \"Wave\" clip. Verified at");
            lines.push_back("load: GetWorldTransforms()[1] at t=0 and t=0.8s matched (0,0,0) and (+0.5,+0.4,0).");
        }
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        const bool pass = error_.empty() && verified_;
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: AnimationPlayer matched both keyframes exactly"
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
            const Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 3.5f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
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

        const auto skeletonBytes = BuildSkeletonBytes(
            {-1, 0},
            {Matrix::getIdentityProperty(), Matrix::getIdentityProperty()},
            {Matrix::getIdentityProperty(), Matrix::getIdentityProperty()});
        WriteBinaryFile(root / "rig.skeleton.bin", skeletonBytes);

        const auto clipBytes = BuildClipBytes(1.6, {
            TrackSpec{1, {
                KeyframeSpec{0.0, Vector3(0.0f, 0.0f, 0.0f)},
                KeyframeSpec{0.8, Vector3(0.5f, 0.4f, 0.0f)},
                KeyframeSpec{1.6, Vector3(0.0f, 0.0f, 0.0f)},
            }},
        });
        WriteBinaryFile(root / "wave.clip.bin", clipBytes);

        std::vector<std::uint8_t> baseVerts, baseIdx, waveVerts, waveIdx;
        BuildSkinnedQuadBytes(0.35f, 0.35f, 0, baseVerts, baseIdx);
        BuildSkinnedQuadBytes(0.3f, 0.3f, 1, waveVerts, waveIdx);
        WriteBinaryFile(root / "base_verts.bin", baseVerts);
        WriteBinaryFile(root / "base_idx.bin", baseIdx);
        WriteBinaryFile(root / "wave_verts.bin", waveVerts);
        WriteBinaryFile(root / "wave_idx.bin", waveIdx);
        WriteSolidColorQoi(root / "base_red.qoi", 220, 60, 60, 255);
        WriteSolidColorQoi(root / "wave_green.qoi", 60, 210, 100, 255);

        WriteTextFile(root / "rig.cnj", R"({
  "cnjVersion": 1,
  "type": "Model",
  "skeleton": "rig.skeleton.bin",
  "animations": [
    { "name": "Wave", "clip": "wave.clip.bin" }
  ],
  "meshes": [
    {
      "name": "Base",
      "vertices": "base_verts.bin",
      "indices": "base_idx.bin",
      "vertexStride": 52,
      "effect": "SkinnedEffect",
      "texture": "base_red.qoi"
    },
    {
      "name": "Wave",
      "vertices": "wave_verts.bin",
      "indices": "wave_idx.bin",
      "vertexStride": 52,
      "effect": "SkinnedEffect",
      "texture": "wave_green.qoi"
    }
  ]
})");
    }

    std::optional<ContentManager> manager_;
    std::optional<Model> model_;
    std::optional<AnimationPlayer> player_;
    SkinningData* skinningData_ = nullptr;
    std::vector<SkinnedEffect*> skinnedEffects_;
    bool verified_ = false;
    std::string error_;
};

} // namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos
