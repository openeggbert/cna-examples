// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
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

namespace CnaExamples::Demos::Avatars::RendererDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::MathHelper;
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

// Phase E: the headline demo -- a real, GPU-skinned avatar body (not the faithful no-op XNA
// surface AvatarFaithfulDrawScreen covers) via AvatarRenderer::EnableRealRenderingEXT +
// DrawRealEXT, following ../cnanext/modules/gamer-services/examples/demo_avatar/src/AvatarDemo.cpp's exact proven call
// sequence: ContentManager.Load<shared_ptr<SkinnedModelEXT>>(AvatarBodyTypeToContentNameEXT(...)),
// EnableRealRenderingEXT, SetAppearanceEXT, standard lighting, then per-frame
// DrawRealEXT(clipName, position, loop). Both genders load side by side in the same frame.
// Verified live, not assumed: after drawing, GetBackBufferData probes three points down each
// avatar's own half of the screen and confirms at least one differs measurably from a background
// reference point -- the same route tools/headless.sh's own --screenshot uses (NEXT.md
// discovery #1), just called from inside the demo instead of the harness.
class AvatarRealRenderScreen : public DemoScreen {
public:
    AvatarRealRenderScreen() : DemoScreen("AvatarRenderer: Real Render (Male & Female)") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        try {
            manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
            manager_->setRootDirectoryProperty(AvatarContentRootDirectory());
            manager_->setGraphicsDevice(device);

            maleModel_ = manager_->Load<std::shared_ptr<SkinnedModelEXT>>(
                AvatarBodyTypeToContentNameEXT(AvatarBodyType::Male));
            femaleModel_ = manager_->Load<std::shared_ptr<SkinnedModelEXT>>(
                AvatarBodyTypeToContentNameEXT(AvatarBodyType::Female));

            maleRenderer_ = std::make_unique<AvatarRenderer>(nullptr);
            maleRenderer_->EnableRealRenderingEXT(device, maleModel_);
            femaleRenderer_ = std::make_unique<AvatarRenderer>(nullptr);
            femaleRenderer_->EnableRealRenderingEXT(device, femaleModel_);

            AvatarAppearanceEXT appearance;
            appearance.setSkinColorProperty(Color(210, 170, 130, 255));
            appearance.setHairColorProperty(Color(40, 25, 15, 255));
            maleRenderer_->SetAppearanceEXT(appearance);
            femaleRenderer_->SetAppearanceEXT(appearance);

            ConfigureStandardAvatarLightingEXT(*maleRenderer_);
            ConfigureStandardAvatarLightingEXT(*femaleRenderer_);
        } catch (const std::exception& ex) {
            error_ = ex.what();
        }
    }

    void OnDemoUnload() override {
        femaleRenderer_.reset();
        maleRenderer_.reset();
        femaleModel_.reset();
        maleModel_.reset();
        manager_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        clipSeconds_ += gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Real, GPU-skinned avatars via AvatarRenderer::EnableRealRenderingEXT + DrawRealEXT --");
        lines.push_back("the CNA extension, NOT the faithful (never-renders) XNA API AvatarFaithfulDrawScreen covers.");
        if (!error_.empty()) {
            lines.push_back("Load FAILED: " + error_);
            lines.push_back("(../cnanext checkout missing or avatar content not built -- see cmake/ExamplesHelpers.cmake)");
        }
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        bool malePainted = false, femalePainted = false;
        if (error_.empty()) {
            sb.End();

            auto& device = GetScreenManager()->getGraphicsDeviceProperty();
            const Viewport original = device.getViewportProperty();
            const int sceneTop = (int)end.Y + 20;
            const int sceneHeight = original.getHeightProperty() - sceneTop - 60;
            const int halfWidth = original.getWidthProperty() / 2;
            device.setDepthStencilStateProperty(DepthStencilState::Default);
            device.setRasterizerStateProperty(RasterizerState::CullNone);

            const auto position = System::TimeSpan::FromSeconds(std::fmod(clipSeconds_, 3.0));

            DrawOneAvatar(device, *maleRenderer_, 0, sceneTop, halfWidth, sceneHeight, "Stand0", position);
            DrawOneAvatar(device, *femaleRenderer_, halfWidth, sceneTop, halfWidth, sceneHeight, "Stand0", position);

            malePainted = DifferentFromBackground(device, sceneTop, sceneHeight, 0, halfWidth);
            femalePainted = DifferentFromBackground(device, sceneTop, sceneHeight, halfWidth, halfWidth);

            device.setViewportProperty(original);
            device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
            device.setDepthStencilStateProperty(DepthStencilState::None);

            sb.Begin();
        }

        const bool pass = error_.empty() && malePainted && femalePainted;
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: both avatars painted measurably different pixels"
                        : (error_.empty() ? "FAIL: probe found no painted pixels" : "UNAVAILABLE: see message above"));
    }

private:
    void DrawOneAvatar(GraphicsDevice& device, AvatarRenderer& renderer, int x, int y, int w, int h,
                       const std::string& clipName, System::TimeSpan position) {
        Viewport scene(x, y, w, h);
        device.setViewportProperty(scene);

        constexpr float kPiOver4 = 0.78539816339744830962f;
        const float aspect = (h > 0) ? (float)w / (float)h : 1.0f;
        renderer.setWorldProperty(Matrix::getIdentityProperty());
        renderer.setViewProperty(Matrix::CreateLookAt(Vector3(0.0f, 1.0f, 3.0f), Vector3(0.0f, 0.9f, 0.0f), Vector3::Up));
        renderer.setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(kPiOver4, aspect, 0.1f, 100.0f));
        renderer.DrawRealEXT(clipName, position, /*loop=*/true);
    }

    // Samples three points down the given half of the scene viewport and compares each against a
    // background reference point at the very top of that same half (above the avatar's head,
    // reliably still the clear color) -- self-referential so this needs no hardcoded clear-color
    // constant. Passes if at least one sample differs enough to be real geometry, not noise.
    bool DifferentFromBackground(GraphicsDevice& device, int sceneTop, int sceneHeight, int x, int w) {
        Rectangle bgRect(x + w / 2, sceneTop + 4, 1, 1);
        Color bg(0, 0, 0, 0);
        device.GetBackBufferData(&bgRect, &bg, 0, 1);

        for (float frac : {0.35f, 0.5f, 0.65f}) {
            Rectangle probeRect(x + w / 2, sceneTop + (int)(sceneHeight * frac), 1, 1);
            Color probe(0, 0, 0, 0);
            device.GetBackBufferData(&probeRect, &probe, 0, 1);
            const int dr = (int)probe.getRProperty() - (int)bg.getRProperty();
            const int dg = (int)probe.getGProperty() - (int)bg.getGProperty();
            const int db = (int)probe.getBProperty() - (int)bg.getBProperty();
            if (dr * dr + dg * dg + db * db > 400) return true; // ~20 per channel, well above dither noise
        }
        return false;
    }

    std::optional<ContentManager> manager_;
    std::shared_ptr<SkinnedModelEXT> maleModel_;
    std::shared_ptr<SkinnedModelEXT> femaleModel_;
    std::unique_ptr<AvatarRenderer> maleRenderer_;
    std::unique_ptr<AvatarRenderer> femaleRenderer_;
    std::string error_;
    double clipSeconds_ = 0.0;
};

} // namespace CnaExamples::Demos::Avatars::RendererDemos
