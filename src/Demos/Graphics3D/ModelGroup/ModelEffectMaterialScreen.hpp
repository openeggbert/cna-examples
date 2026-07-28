// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectMaterial.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
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
using Microsoft::Xna::Framework::Graphics::Effect;
using Microsoft::Xna::Framework::Graphics::EffectMaterial;
using Microsoft::Xna::Framework::Graphics::Model;
using Microsoft::Xna::Framework::Graphics::ModelMesh;
using Microsoft::Xna::Framework::Graphics::ModelMeshPart;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;

// D3: two related findings about ModelMeshPart's own Effect slot.
//
// First -- EffectMaterial. Its own doc comment says it is "created internally
// by the content pipeline; games do not instantiate it directly." Neither of
// CNA's two Model content readers agrees: the .cnj ModelTypeReader (read
// directly, ContentManager.cpp) always constructs a real stock effect
// (BasicEffect/SkinnedEffect/DualTextureEffect/PbrEffect/SkinnedPbrEffect)
// straight into the part's Effect slot, never an EffectMaterial; the XNB
// ModelContentTypeReaders.cpp path does the same. The only place EffectMaterial
// is actually constructed anywhere in this codebase is its own unit test. It
// IS fully constructible and clonable standalone (proven below, the same way
// the unit test does) -- but reading EffectMaterial.cpp directly shows its
// OnApply() override is an empty function body. Applying one, even
// successfully constructed, would bind no parameters and change no GPU state;
// this is stated from source, not attempted live, since drawing with a
// deliberately no-op shader application is not something to try against a
// real GL context.
//
// Second -- swapping a mesh part's Effect at runtime, live. ModelMeshPart::setEffectProperty()
// (see Model: Load & Traverse's own finding) self-maintains its parent mesh's own Effects
// collection by itself once the part has a parent -- Add() on set, Remove() on change only if no
// other part in the mesh still shares the old effect. SwapEffect() below therefore needs only the
// one setEffectProperty() call; no manual Add()/Remove() bookkeeping, verified live.
class ModelEffectMaterialScreen : public DemoScreen {
public:
    ModelEffectMaterialScreen() : DemoScreen("Model: EffectMaterial & Effect Swapping") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        // Part 1: EffectMaterial, constructed directly (never via content).
        materialSource_.emplace(device);
        material_.emplace(*materialSource_);
        materialTypeNameOk_ = material_->GetTypeName() == "Microsoft.Xna.Framework.Graphics.EffectMaterial";
        materialClone_.reset(material_->Clone());
        auto* clonedMaterial = dynamic_cast<EffectMaterial*>(materialClone_.get());
        materialCloneOk_ = clonedMaterial != nullptr &&
                           static_cast<Effect*>(clonedMaterial) != static_cast<Effect*>(&*material_);

        // Part 2: a real Model, loaded through content, whose one mesh part's
        // Effect gets swapped live between two different real BasicEffects.
        try {
            WriteFixture();
            manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
            manager_->setRootDirectoryProperty(ModelContentDirectory());
            manager_->setGraphicsDevice(device);

            model_.emplace(manager_->Load<Model>("swappable"));
            mesh_ = model_->getMeshesProperty().getCountProperty() > 0
                ? model_->getMeshesProperty()[0] : nullptr;
            part_ = (mesh_ != nullptr && mesh_->getMeshPartsProperty().getCountProperty() > 0)
                ? mesh_->getMeshPartsProperty()[0] : nullptr;

            loadedEffect_ = part_ != nullptr ? dynamic_cast<BasicEffect*>(part_->getEffectProperty()) : nullptr;

            swapTexture_.emplace(manager_->Load<Texture2D>("swap_blue"));
            swapEffect_.emplace(device);
            swapEffect_->setTextureProperty(&*swapTexture_);
            swapEffect_->setTextureEnabledProperty(true);

            const bool startedOnLoaded = part_ != nullptr && part_->getEffectProperty() == loadedEffect_ &&
                                         mesh_->getEffectsProperty().Contains(loadedEffect_) &&
                                         !mesh_->getEffectsProperty().Contains(&*swapEffect_);

            SwapEffect(); // exercised once automatically, so the sweep screenshot verifies it
            const bool swappedToBlue = part_ != nullptr && part_->getEffectProperty() == &*swapEffect_ &&
                                       mesh_->getEffectsProperty().Contains(&*swapEffect_) &&
                                       !mesh_->getEffectsProperty().Contains(loadedEffect_);

            swapVerified_ = startedOnLoaded && swappedToBlue;
        } catch (const std::exception& ex) {
            error_ = ex.what();
        }
    }

    void OnDemoUnload() override {
        model_.reset();
        swapEffect_.reset();
        swapTexture_.reset();
        manager_.reset();
        materialClone_.reset();
        material_.reset();
        materialSource_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) SwapEffect();
    }

    void OnDemoUpdate(GameTime& gameTime) override {
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("EffectMaterial's doc comment: \"created internally by the content pipeline.\" Neither");
        lines.push_back("Model reader agrees -- both always build a real stock effect directly. It IS");
        lines.push_back("constructible/clonable by hand (below), but OnApply() is an empty function body.");
        lines.push_back("  GetTypeName correct: " + std::string(materialTypeNameOk_ ? "yes" : "NO") +
                        "   Clone() independent: " + std::string(materialCloneOk_ ? "yes" : "NO"));
        lines.emplace_back();
        if (!error_.empty()) {
            lines.push_back("Model load FAILED: " + error_);
        } else {
            lines.push_back("Select: swap the mesh part's real Effect between two live BasicEffects. Currently: " +
                            std::string(part_ != nullptr && part_->getEffectProperty() == loadedEffect_
                                        ? "content-loaded (red)" : "swapped-in (blue)"));
        }
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        const bool pass = materialTypeNameOk_ && materialCloneOk_ && error_.empty() && swapVerified_;
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: EffectMaterial verified; live swap confirmed"
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

            const Matrix world = Matrix::CreateRotationY(spin_);
            const Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 3.0f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
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
    void SwapEffect() {
        if (part_ == nullptr) return;
        Effect* current = part_->getEffectProperty();
        Effect* next = (current == static_cast<Effect*>(loadedEffect_)) ? static_cast<Effect*>(&*swapEffect_)
                                                                        : static_cast<Effect*>(loadedEffect_);
        // setEffectProperty() alone keeps mesh_'s own Effects collection in sync -- see this
        // class's own header comment.
        part_->setEffectProperty(next);
    }

    void WriteFixture() {
        namespace fs = std::filesystem;
        const fs::path root = ModelContentDirectory();
        fs::create_directories(root);

        std::vector<std::uint8_t> verts, indices;
        BuildQuadStride32Bytes(1.0f, 1.0f, verts, indices);
        WriteBinaryFile(root / "swap_verts.bin", verts);
        WriteBinaryFile(root / "swap_idx.bin", indices);
        WriteSolidColorQoi(root / "swap_red.qoi", 220, 60, 60, 255);
        WriteSolidColorQoi(root / "swap_blue.qoi", 60, 110, 220, 255);

        WriteTextFile(root / "swappable.cnj", R"({
  "cnjVersion": 1,
  "type": "Model",
  "meshes": [
    {
      "name": "Swappable",
      "vertices": "swap_verts.bin",
      "indices": "swap_idx.bin",
      "vertexStride": 32,
      "effect": "BasicEffect",
      "texture": "swap_red.qoi"
    }
  ]
})");
    }

    std::optional<BasicEffect> materialSource_;
    std::optional<EffectMaterial> material_;
    std::unique_ptr<Effect> materialClone_;
    bool materialTypeNameOk_ = false;
    bool materialCloneOk_ = false;

    std::optional<ContentManager> manager_;
    std::optional<Model> model_;
    std::optional<Texture2D> swapTexture_;
    std::optional<BasicEffect> swapEffect_;
    ModelMesh* mesh_ = nullptr;
    ModelMeshPart* part_ = nullptr;
    BasicEffect* loadedEffect_ = nullptr;
    bool swapVerified_ = false;
    std::string error_;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos
