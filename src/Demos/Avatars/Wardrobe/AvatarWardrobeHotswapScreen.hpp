// SPDX-License-Identifier: MIT
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarAppearanceEXT.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarBodyType.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarBodyTypeNamesEXT.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRenderer.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SkinnedModelEXT.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/TimeSpan.hpp"

#include "Demos/Avatars/AvatarContentHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Avatars::WardrobeDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::SkinnedModelEXT;
using Microsoft::Xna::Framework::Graphics::Viewport;
using Microsoft::Xna::Framework::GamerServices::AvatarAppearanceEXT;
using Microsoft::Xna::Framework::GamerServices::AvatarBodyType;
using Microsoft::Xna::Framework::GamerServices::AvatarBodyTypeToContentNameEXT;
using Microsoft::Xna::Framework::GamerServices::AvatarRenderer;

// Phase E: SkinnedModelEXT::AttachPartEXT/RemovePartEXT, following
// ../cna/examples/demo_avatar_wardrobe_hotswap/src/HotswapDemo.cpp's exact proven cycle: baked-in
// hair -> Cap -> Ponytail -> (fresh reload) baked-in. AttachPartEXT's replace-by-name semantics
// (its own header comment, Task 11.4) remove any existing same-named part via RemovePartEXT before
// attaching the incoming one, so re-attaching never leaves two hairstyles rendered at once and
// never leaks the old part's GPU resources -- restoring the ORIGINAL baked-in hair needs a genuine
// fresh Load(), since RemovePartEXT already freed it the moment it was first replaced (there is no
// "undo").
//
// Verified live via a real measurement no other screen in this repo has access to:
// SkinnedModelEXT::GetOwnedPartCountForTesting() (a NOXNA testing accessor on the type itself).
// The base avatar owns exactly 5 parts (Body/Hair/Pants/Shirt/Shoes); after every swap in the
// cycle this stays exactly 5, proving replace-by-name neither leaks a duplicate nor drops a part,
// rather than trusting that AttachPartEXT's doc comment describes what actually happens.
class AvatarWardrobeHotswapScreen : public DemoScreen {
public:
    AvatarWardrobeHotswapScreen() : DemoScreen("Wardrobe: Hot-Swap (AttachPartEXT/RemovePartEXT)") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        try {
            manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
            manager_->setRootDirectoryProperty(AvatarContentRootDirectory());
            manager_->setGraphicsDevice(device);

            ApplyState(0);
            renderer_ = std::make_unique<AvatarRenderer>(nullptr);
            ConfigureRenderer(device);
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
        if (!error_.empty()) return;
        cycleSeconds_ += gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        if (cycleSeconds_ >= 2.5) {
            cycleSeconds_ = 0.0;
            state_ = (state_ + 1) % 3;
            try {
                ApplyState(state_);
                ConfigureRenderer(GetScreenManager()->getGraphicsDeviceProperty());
                const int count = (int)model_->GetOwnedPartCountForTesting();
                if (count != 5) allCountsCorrect_ = false;
                swapsObserved_++;
            } catch (const std::exception& ex) {
                error_ = ex.what();
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("AttachPartEXT/RemovePartEXT hot-swap cycle: baked-in hair -> Cap -> Ponytail -> reload, every 2.5s.");
        if (!error_.empty()) {
            lines.push_back("FAILED: " + error_);
        } else {
            lines.push_back("Current: " + StateName(state_));
            lines.push_back("GetOwnedPartCountForTesting(): " +
                            (model_ ? std::to_string(model_->GetOwnedPartCountForTesting()) : std::string("?")) +
                            " (expected 5 -- Body/Hair/Pants/Shirt/Shoes, unchanged across every swap)");
            lines.push_back("Swaps observed so far: " + std::to_string(swapsObserved_) +
                            ", all stayed at exactly 5 parts: " + std::string(allCountsCorrect_ ? "true" : "false"));
        }
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        if (error_.empty()) {
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
                Matrix::CreateLookAt(Vector3(0.0f, 1.3f, 1.6f), Vector3(0.0f, 1.5f, 0.0f), Vector3::Up));
            renderer_->setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(kPiOver4, aspect, 0.1f, 100.0f));
            renderer_->DrawRealEXT("Stand0", System::TimeSpan::Zero, /*loop=*/true);

            device.setViewportProperty(original);
            device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
            device.setDepthStencilStateProperty(DepthStencilState::None);
            sb.Begin();
        }

        const bool pass = error_.empty() && swapsObserved_ >= 2 && allCountsCorrect_;
        const bool pending = error_.empty() && swapsObserved_ < 2;
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255)
                                                   : pending ? Color(210, 170, 40, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: part count stayed exactly 5 across every swap"
                        : pending ? "Cycling through hair states..."
                                  : (error_.empty() ? "FAIL: part count deviated from 5 on at least one swap"
                                                    : "UNAVAILABLE: see message above"));
    }

private:
    static std::string StateName(int state) {
        switch (state) {
            case 0: return "Baked-in";
            case 1: return "Cap";
            default: return "Ponytail";
        }
    }

    void ApplyState(int state) {
        auto& content = *manager_;
        if (state == 0) {
            // RemovePartEXT already freed the original baked-in hair's GPU resources the moment
            // it was first replaced -- restoring it needs a genuine fresh reload, not an "undo".
            // Unload() clears ContentManager's own cache map only; this screen's model_/renderer_
            // hold independent references, so nothing dangles (same as HotswapDemo.cpp).
            content.Unload();
            model_ = content.Load<std::shared_ptr<SkinnedModelEXT>>(
                AvatarBodyTypeToContentNameEXT(AvatarBodyType::Male));
        } else {
            const char* style = (state == 1) ? "Cap" : "Ponytail";
            auto wardrobePiece = content.Load<std::shared_ptr<SkinnedModelEXT>>(
                std::string("wardrobe/hair_") + style + "/avatar");
            model_->AttachPartEXT(std::move(*wardrobePiece));
        }
    }

    void ConfigureRenderer(GraphicsDevice& device) {
        renderer_->EnableRealRenderingEXT(device, model_);
        AvatarAppearanceEXT appearance;
        renderer_->SetAppearanceEXT(appearance);
        ConfigureStandardAvatarLightingEXT(*renderer_);
    }

    std::optional<ContentManager> manager_;
    std::shared_ptr<SkinnedModelEXT> model_;
    std::unique_ptr<AvatarRenderer> renderer_;
    std::string error_;
    int state_ = 0;
    double cycleSeconds_ = 0.0;
    int swapsObserved_ = 0;
    bool allCountsCorrect_ = true;
};

} // namespace CnaExamples::Demos::Avatars::WardrobeDemos
