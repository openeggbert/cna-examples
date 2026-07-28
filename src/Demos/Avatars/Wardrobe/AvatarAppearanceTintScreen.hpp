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
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "System/TimeSpan.hpp"

#include "Demos/Avatars/AvatarContentHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Avatars::WardrobeDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Rectangle;
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

// Phase E: AvatarAppearanceEXT's five tint slots (skin/hair/shirt/pants/shoes) applied via
// AvatarRenderer::SetAppearanceEXT, resolved per SkinnedModelEXT part at draw time by
// AvatarRenderer::PartTintEXT's substring match against each part's name (e.g. "CNAAvatarShirt"
// contains "Shirt"). Verified live, not assumed: the screen alternates between two starkly
// different appearances every 2s and probes a torso-height pixel each time DrawRealEXT runs;
// once both appearances have been sampled at least once, it confirms the probed pixel actually
// changed -- proof the tint reaches the rendered pixels, not just the AvatarAppearanceEXT struct.
class AvatarAppearanceTintScreen : public DemoScreen {
public:
    AvatarAppearanceTintScreen() : DemoScreen("Wardrobe: Per-Slot Tinting (AvatarAppearanceEXT)") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        try {
            manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
            manager_->setRootDirectoryProperty(AvatarContentRootDirectory());
            manager_->setGraphicsDevice(device);

            model_ = manager_->Load<std::shared_ptr<SkinnedModelEXT>>(
                AvatarBodyTypeToContentNameEXT(AvatarBodyType::Male));

            renderer_ = std::make_unique<AvatarRenderer>(nullptr);
            renderer_->EnableRealRenderingEXT(device, model_);
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
        cycleSeconds_ += gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        if (cycleSeconds_ >= 2.0) {
            cycleSeconds_ = 0.0;
            appearanceIndex_ = 1 - appearanceIndex_;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("AvatarAppearanceEXT: shirt/pants/hair/skin tint, alternating every 2s, torso pixel probed each time.");
        if (!error_.empty()) {
            lines.push_back("Load FAILED: " + error_);
        } else {
            lines.push_back(std::string("Current appearance: ") + (appearanceIndex_ == 0 ? "Default (Navajo/SaddleBrown)"
                                                                                          : "Alternate (bright red/yellow/blue)"));
            lines.push_back("Sample 0 captured: " + std::string(sample0Captured_ ? "yes" : "not yet") +
                            "   Sample 1 captured: " + std::string(sample1Captured_ ? "yes" : "not yet"));
            if (sample0Captured_ && sample1Captured_) {
                lines.push_back("Torso pixel changed between appearances: " +
                                std::string(changed_ ? "true" : "false"));
            }
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

            AvatarAppearanceEXT appearance;
            if (appearanceIndex_ == 1) {
                appearance.setShirtColorProperty(Color(230, 30, 30, 255));
                appearance.setPantsColorProperty(Color(230, 210, 30, 255));
                appearance.setHairColorProperty(Color(30, 60, 230, 255));
            }
            renderer_->SetAppearanceEXT(appearance);

            constexpr float kPiOver4 = 0.78539816339744830962f;
            const float aspect = (scene.getHeightProperty() > 0)
                                      ? (float)scene.getWidthProperty() / (float)scene.getHeightProperty()
                                      : 1.0f;
            renderer_->setWorldProperty(Matrix::getIdentityProperty());
            renderer_->setViewProperty(
                Matrix::CreateLookAt(Vector3(0.0f, 1.0f, 2.4f), Vector3(0.0f, 1.0f, 0.0f), Vector3::Up));
            renderer_->setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(kPiOver4, aspect, 0.1f, 100.0f));
            renderer_->DrawRealEXT("Stand0", System::TimeSpan::Zero, /*loop=*/true);

            Rectangle torso(scene.getWidthProperty() / 2, sceneTop + (int)(scene.getHeightProperty() * 0.45f), 1, 1);
            Color sample(0, 0, 0, 0);
            device.GetBackBufferData(&torso, &sample, 0, 1);
            if (appearanceIndex_ == 0 && !sample0Captured_) {
                sample0_ = sample;
                sample0Captured_ = true;
            } else if (appearanceIndex_ == 1 && !sample1Captured_) {
                sample1_ = sample;
                sample1Captured_ = true;
            }
            if (sample0Captured_ && sample1Captured_) {
                const int dr = (int)sample0_.getRProperty() - (int)sample1_.getRProperty();
                const int dg = (int)sample0_.getGProperty() - (int)sample1_.getGProperty();
                const int db = (int)sample0_.getBProperty() - (int)sample1_.getBProperty();
                changed_ = (dr * dr + dg * dg + db * db) > 400;
            }

            device.setViewportProperty(original);
            device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
            device.setDepthStencilStateProperty(DepthStencilState::None);
            sb.Begin();
        }

        const bool pass = error_.empty() && sample0Captured_ && sample1Captured_ && changed_;
        const bool pending = error_.empty() && (!sample0Captured_ || !sample1Captured_);
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255)
                                                   : pending ? Color(210, 170, 40, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: torso pixel measurably changed"
                        : pending ? "Sampling both appearances..."
                                  : (error_.empty() ? "FAIL: torso pixel did not change" : "UNAVAILABLE: see message above"));
    }

private:
    std::optional<ContentManager> manager_;
    std::shared_ptr<SkinnedModelEXT> model_;
    std::unique_ptr<AvatarRenderer> renderer_;
    std::string error_;
    int appearanceIndex_ = 0;
    double cycleSeconds_ = 0.0;
    bool sample0Captured_ = false;
    bool sample1Captured_ = false;
    Color sample0_{0, 0, 0, 0};
    Color sample1_{0, 0, 0, 0};
    bool changed_ = false;
};

} // namespace CnaExamples::Demos::Avatars::WardrobeDemos
