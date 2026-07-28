// SPDX-License-Identifier: MIT
#pragma once

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
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Viewport;

// D3: a real Model loaded through ContentManager (not hand-built in C++ the way
// ModelGroup/ProceduralModelScreen.hpp is), then traversed via the real
// Bones/Meshes/MeshParts collections. The .cnj fixture (two textured quad
// meshes) is synthesized at OnDemoLoad into a temp dir -- see
// ModelContentHelpers.hpp's own header comment for why that is possible here
// and is not the XNB-borrowing pattern Content/Xnb uses.
//
// FINDING, corrected after an empirical check caught a wrong first assumption (a standalone
// debug binary against ../cna's own libCNA, not just reading source): ModelMeshPart::
// setEffectProperty() (ModelMeshPart.cpp) keeps its parent ModelMesh's own Effects collection
// self-maintained -- Add() on set, and Remove() on change ONLY if no other part in the mesh still
// uses the old effect (real ref-counted sharing, not a blind remove). This fires correctly for a
// ContentManager-loaded model because ModelTypeReader constructs the ModelMesh (which sets each
// part's parent_) BEFORE calling setEffectProperty() on its parts. mesh.Effects.Count == 1 with no
// extra code, confirmed live below, and the rotating view further down receives live camera
// matrices with no workaround needed.
//
// The catch: setEffectProperty()'s sync only fires when parent_ is already set, and nothing
// enforces the ordering. ModelGroup/ProceduralModelScreen.hpp calls setEffectProperty() on parts
// BEFORE constructing their owning ModelMesh (parent_ still null at that point) -- so its sync
// silently does nothing, which is why that screen's own OnDemoLoad() needs an explicit
// mesh_->getEffectsPropertyMutable().Add() afterward. Both screens are correct; the API is just
// silently order-dependent, with no error either way.
class ContentModelLoadScreen : public DemoScreen {
public:
    ContentModelLoadScreen() : DemoScreen("Model: Load & Traverse") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        try {
            WriteFixture();

            manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
            manager_->setRootDirectoryProperty(ModelContentDirectory());
            manager_->setGraphicsDevice(device);

            model_.emplace(manager_->Load<Model>("panels"));

            meshCount_ = model_->getMeshesProperty().getCountProperty();
            for (ModelMesh* mesh : model_->getMeshesProperty()) {
                const auto& parts = mesh->getMeshPartsProperty();
                std::string line = mesh->getNameProperty() + ": " +
                                   std::to_string(parts.getCountProperty()) + " part(s)";
                if (parts.getCountProperty() > 0) {
                    auto* effect = parts[0]->getEffectProperty();
                    auto* basic = dynamic_cast<BasicEffect*>(effect);
                    line += basic != nullptr ? ", effect = real BasicEffect" : ", effect = (other/null)";
                    if (basic == nullptr) allEffectsAreBasicEffect_ = false;
                } else {
                    allEffectsAreBasicEffect_ = false;
                }
                meshLines_.push_back(line);
            }

            // Confirmed live, not assumed: the Effects collection is already self-maintained by
            // the time Load<Model>() returns -- see this class's own header comment.
            effectsSelfMaintained_ = true;
            for (ModelMesh* mesh : model_->getMeshesProperty()) {
                const bool oneEffect = mesh->getEffectsProperty().getCountProperty() == 1;
                const bool containsIt = mesh->getMeshPartsProperty().getCountProperty() > 0 &&
                    mesh->getEffectsProperty().Contains(mesh->getMeshPartsProperty()[0]->getEffectProperty());
                if (!oneEffect || !containsIt) effectsSelfMaintained_ = false;
            }
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
        spin_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("A real Model, loaded via ContentManager.Load<Model>() from a synthesized .cnj fixture.");
        if (!error_.empty()) {
            lines.push_back("Load FAILED: " + error_);
        } else {
            lines.push_back("Meshes: " + std::to_string(meshCount_) + ", each its own child bone + a real BasicEffect.");
            lines.emplace_back();
            lines.push_back("FINDING: setEffectProperty() self-maintains its mesh's Effects collection, but only");
            lines.push_back("once the part already has a parent (ModelTypeReader builds the mesh first;");
            lines.push_back("ProceduralModelScreen sets Effect first, so IT needs a manual Add). Confirmed: " +
                            std::string(effectsSelfMaintained_ ? "yes" : "NO -- did not reproduce"));
        }
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        const bool pass = error_.empty() && meshCount_ == 2 && allEffectsAreBasicEffect_ && effectsSelfMaintained_;
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: meshes loaded; Effects self-maintenance verified"
                        : "FAIL: load or traversal did not match expectations");

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
            const Matrix view = Matrix::CreateLookAt(Vector3(0.0f, 0.8f, 3.5f), Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f));
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

        std::vector<std::uint8_t> vertsA, vertsB, indices;
        BuildQuadStride32Bytes(0.7f, 0.7f, vertsA, indices, -0.6f);
        BuildQuadStride32Bytes(0.7f, 0.7f, vertsB, indices, 0.6f);
        WriteBinaryFile(root / "panel_a_verts.bin", vertsA);
        WriteBinaryFile(root / "panel_b_verts.bin", vertsB);
        WriteBinaryFile(root / "panel_idx.bin", indices);
        WriteSolidColorQoi(root / "panel_a.qoi", 220, 60, 60, 255);
        WriteSolidColorQoi(root / "panel_b.qoi", 60, 110, 220, 255);

        WriteTextFile(root / "panels.cnj", R"({
  "cnjVersion": 1,
  "type": "Model",
  "meshes": [
    {
      "name": "PanelA",
      "vertices": "panel_a_verts.bin",
      "indices": "panel_idx.bin",
      "vertexStride": 32,
      "effect": "BasicEffect",
      "texture": "panel_a.qoi"
    },
    {
      "name": "PanelB",
      "vertices": "panel_b_verts.bin",
      "indices": "panel_idx.bin",
      "vertexStride": 32,
      "effect": "BasicEffect",
      "texture": "panel_b.qoi"
    }
  ]
})");
    }

    std::optional<ContentManager> manager_;
    std::optional<Model> model_;
    std::string error_;
    std::vector<std::string> meshLines_;
    int meshCount_ = 0;
    bool allEffectsAreBasicEffect_ = true;
    bool effectsSelfMaintained_ = false;
    float spin_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos
