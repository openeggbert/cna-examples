// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarAnimationPreset.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarAnimationPresetNamesEXT.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarAppearanceEXT.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarBodyType.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarBodyTypeNamesEXT.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRenderer.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SkinnedModelEXT.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/TimeSpan.hpp"

#include "Demos/Avatars/AvatarContentHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Avatars::RendererDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::SkinnedModelEXT;
using Microsoft::Xna::Framework::Graphics::Viewport;
using Microsoft::Xna::Framework::GamerServices::AvatarAnimationPreset;
using Microsoft::Xna::Framework::GamerServices::AvatarAnimationPresetToClipNameEXT;
using Microsoft::Xna::Framework::GamerServices::AvatarAppearanceEXT;
using Microsoft::Xna::Framework::GamerServices::AvatarBodyType;
using Microsoft::Xna::Framework::GamerServices::AvatarBodyTypeToContentNameEXT;
using Microsoft::Xna::Framework::GamerServices::AvatarRenderer;

// Phase E, finding: plan.md's original row assumed "all 31 animation presets auto-cycled" on a
// single avatar. That is NOT what the real content supports -- AvatarAnimationPreset has 31
// enumerators (11 gender-neutral + 10 Female* + 10 Male*, confirmed on the AvatarNameTablesScreen),
// but a single loaded SkinnedModelEXT only ever bakes its OWN gender's 10 plus the 11 neutral ones
// (21 total) into its Clips map -- the other gender's 10 preset names are simply absent, so
// SkinnedModelEXT::ComputeBoneTransformsEXT (which DrawRealEXT calls internally) throws
// ArgumentException("Unknown animation clip: ...") for them. Verified here directly against
// ComputeBoneTransformsEXT for all 31 presets BEFORE ever attempting a draw, rather than
// discovering the throw live and treating it as a bug -- it is real, correct, documented behavior.
// The screen then auto-cycles only the 21 presets that actually exist on the loaded (Male) body.
//
// UNRESOLVED, noted rather than chased down: at least one clip in the cycle (observed on "Stand2")
// renders with the avatar's head invisible/out of frame, while "Stand0" (identical code path,
// same camera) frames correctly -- confirmed by pixel-scanning a screenshot column, not by eye,
// and confirmed NOT a camera-distance problem (pulling the camera back from 3.0 to 4.4 world units
// did not bring it back). Since the torso/arms/legs of the very same CNAAvatarBody part render
// fine, this points at a per-clip head/neck bone transform issue in that clip's data or in
// ComputeBoneTransformsEXT's per-bone hierarchy math, not a whole-part or rendering-pipeline
// failure. Not root-caused further -- it does not affect this screen's own verified claim (the
// valid/invalid preset split and DrawRealEXT succeeding without throwing), so it is recorded here
// and in NEXT.md rather than blocking the rest of Phase E.
class AvatarAnimationPresetCycleScreen : public DemoScreen {
public:
    AvatarAnimationPresetCycleScreen() : DemoScreen("AvatarRenderer: Animation Preset Cycling") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        try {
            manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
            manager_->setRootDirectoryProperty(AvatarContentRootDirectory());
            manager_->setGraphicsDevice(device);

            model_ = manager_->Load<std::shared_ptr<SkinnedModelEXT>>(
                AvatarBodyTypeToContentNameEXT(AvatarBodyType::Male));

            for (int i = 0; i <= (int)AvatarAnimationPreset::MaleYawn; ++i) {
                const std::string clipName = AvatarAnimationPresetToClipNameEXT((AvatarAnimationPreset)i);
                std::vector<Matrix> dummy;
                try {
                    model_->ComputeBoneTransformsEXT(clipName, System::TimeSpan::Zero, false, dummy);
                    validClipNames_.push_back(clipName);
                } catch (const std::exception&) {
                    ++invalidCount_;
                }
            }

            renderer_ = std::make_unique<AvatarRenderer>(nullptr);
            renderer_->EnableRealRenderingEXT(device, model_);
            AvatarAppearanceEXT appearance;
            renderer_->SetAppearanceEXT(appearance);
            ConfigureStandardAvatarLightingEXT(*renderer_);
        } catch (const std::exception& ex) {
            error_ = ex.what();
        }
    }

    void OnDemoUnload() override {
        renderer_.reset();
        model_.reset();
        manager_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        if (validClipNames_.empty()) return;
        const double dt = gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        clipSeconds_ += dt;
        cycleSeconds_ += dt;
        if (cycleSeconds_ >= 1.5) {
            cycleSeconds_ = 0.0;
            currentIndex_ = (currentIndex_ + 1) % validClipNames_.size();
            clipSeconds_ = 0.0;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("All 31 AvatarAnimationPreset values probed against ComputeBoneTransformsEXT before any draw:");
        if (!error_.empty()) {
            lines.push_back("Load FAILED: " + error_);
        } else {
            lines.push_back("Valid on this (Male) body: " + std::to_string(validClipNames_.size()) +
                            " (expected 21 = 11 neutral + 10 Male*)");
            lines.push_back("Threw ArgumentException:   " + std::to_string(invalidCount_) +
                            " (expected 10 -- the Female* presets, absent from this body's Clips)");
            lines.push_back("Auto-cycling the 21 valid presets, 1.5s each -- current: " +
                            (validClipNames_.empty() ? std::string("(none)") : validClipNames_[currentIndex_]));
        }
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        bool painted = false;
        if (error_.empty() && !validClipNames_.empty()) {
            sb.End();
            auto& device = GetScreenManager()->getGraphicsDeviceProperty();
            const Viewport original = device.getViewportProperty();
            const int sceneTop = (int)end.Y + 20;
            Viewport scene(0, sceneTop, original.getWidthProperty(), original.getHeightProperty() - sceneTop - 60);
            device.setViewportProperty(scene);
            device.setDepthStencilStateProperty(DepthStencilState::Default);
            device.setRasterizerStateProperty(RasterizerState::CullNone);

            constexpr float kPiOver4 = 0.78539816339744830962f;
            const float aspect = (scene.getHeightProperty() > 0)
                                      ? (float)scene.getWidthProperty() / (float)scene.getHeightProperty()
                                      : 1.0f;
            renderer_->setWorldProperty(Matrix::getIdentityProperty());
            renderer_->setViewProperty(
                Matrix::CreateLookAt(Vector3(0.0f, 1.1f, 4.4f), Vector3(0.0f, 0.9f, 0.0f), Vector3::Up));
            renderer_->setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(kPiOver4, aspect, 0.1f, 100.0f));
            renderer_->DrawRealEXT(validClipNames_[currentIndex_], System::TimeSpan::FromSeconds(clipSeconds_),
                                   /*loop=*/true);
            painted = true; // DrawRealEXT not throwing here is itself part of the claim.

            device.setViewportProperty(original);
            device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
            device.setDepthStencilStateProperty(DepthStencilState::None);
            sb.Begin();
        }

        const bool pass = error_.empty() && validClipNames_.size() == 21 && invalidCount_ == 10 && painted;
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: 21 valid / 10 invalid presets, as expected"
                        : (error_.empty() ? "FAIL: preset validity counts did not match"
                                          : "UNAVAILABLE: see message above"));
    }

private:
    std::optional<ContentManager> manager_;
    std::shared_ptr<SkinnedModelEXT> model_;
    std::unique_ptr<AvatarRenderer> renderer_;
    std::vector<std::string> validClipNames_;
    int invalidCount_ = 0;
    std::size_t currentIndex_ = 0;
    double clipSeconds_ = 0.0;
    double cycleSeconds_ = 0.0;
    std::string error_;
};

} // namespace CnaExamples::Demos::Avatars::RendererDemos
