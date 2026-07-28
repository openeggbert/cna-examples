// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/AvatarExpression.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRenderer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRendererState.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/ArgumentException.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Avatars::RendererDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::GamerServices::AvatarExpression;
using Microsoft::Xna::Framework::GamerServices::AvatarRenderer;
using Microsoft::Xna::Framework::GamerServices::AvatarRendererState;

// Phase E: the faithful (real XNA 4.0, non-EXT) half of AvatarRenderer's API surface, verified
// live from AvatarRenderer.cpp/AvatarRenderer.hpp rather than assumed. The real off-Xbox XNA
// implementation never actually renders an avatar -- State forces itself to Unavailable on every
// read (getStateProperty()'s own doc comment), and Draw() is a genuine no-op after argument
// validation (confirmed by reading the .cpp directly: it does nothing but check bones.size() ==
// BoneCount). getBindPoseProperty() throws InvalidOperationException because it checks the raw
// state field against Ready, which nothing ever sets -- even though bindPoseArray_ itself IS
// correctly sized to 71 at construction, its content (all-identity, since nothing ever assigns
// real matrices into it) is simply unreachable through the public getter.
//
// CORRECTED FINDING (this screen's own first draft assumed the opposite and a live screenshot
// caught it -- exactly the "verify by measurement" trap this project's notes warn about
// repeatedly): unlike BindPose, ParentBones is NOT empty. The constructor initializes
// parentBoneIds_ from a real, hardcoded 71-entry kParentBoneIds table
// (AvatarRenderer.cpp's own anonymous namespace), so getParentBonesProperty() genuinely returns
// all 71 real parent-bone indices with no throw and no lazy gate -- the one piece of real skeletal
// data this "never renders anything" class exposes unconditionally. No GraphicsDevice, no 3D
// pipeline and no borrowed asset needed for any of this -- it is real behavior even on a 2D-only
// backend.
class AvatarFaithfulDrawScreen : public DemoScreen {
public:
    AvatarFaithfulDrawScreen() : DemoScreen("AvatarRenderer: The Faithful (No-Op) XNA Surface") {}

    void OnDemoLoad() override {
        AvatarRenderer renderer(nullptr);

        stateBefore_ = renderer.getStateProperty();

        std::vector<Matrix> bones71(71, Matrix::getIdentityProperty());
        AvatarExpression expression;
        try {
            renderer.Draw(bones71, expression);
            drawWithSeventyOneThrew_ = false;
        } catch (const std::exception&) {
            drawWithSeventyOneThrew_ = true;
        }

        std::vector<Matrix> bones70(70, Matrix::getIdentityProperty());
        try {
            renderer.Draw(bones70, expression);
            drawWithSeventyThrew_ = false;
        } catch (const System::ArgumentException&) {
            drawWithSeventyThrew_ = true;
        } catch (const std::exception&) {
            drawWithSeventyThrew_ = true;
        }

        stateAfter_ = renderer.getStateProperty();
        parentBoneCount_ = (int)renderer.getParentBonesProperty().getCountProperty();

        try {
            (void)renderer.getBindPoseProperty();
            bindPoseThrew_ = false;
        } catch (const std::exception&) {
            bindPoseThrew_ = true;
        }
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("The real (non-EXT) AvatarRenderer never renders anything off-Xbox -- verified live:");
        lines.emplace_back();
        lines.push_back("State before any Draw():  " + StateName(stateBefore_) + " (expected Unavailable)");
        lines.push_back("Draw(71 bones, expr):     did NOT throw = " +
                        std::string(!drawWithSeventyOneThrew_ ? "true" : "false") + " (genuinely a no-op)");
        lines.push_back("Draw(70 bones, expr):     threw ArgumentException = " +
                        std::string(drawWithSeventyThrew_ ? "true" : "false") + " (wrong count still validated)");
        lines.push_back("State after Draw():       " + StateName(stateAfter_) + " (expected Unavailable, unchanged)");
        lines.push_back("ParentBones.Count:        " + std::to_string(parentBoneCount_) +
                        " (expected 71 -- a real table, unlike BindPose, populated unconditionally)");
        lines.push_back("BindPose access threw InvalidOperationException = " +
                        std::string(bindPoseThrew_ ? "true" : "false") + " (State is never Ready)");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        const bool pass = stateBefore_ == AvatarRendererState::Unavailable &&
                          !drawWithSeventyOneThrew_ && drawWithSeventyThrew_ &&
                          stateAfter_ == AvatarRendererState::Unavailable &&
                          parentBoneCount_ == 71 && bindPoseThrew_;
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: faithful Draw() confirmed a genuine no-op"
                        : "FAIL: behavior did not match the XNA-faithful contract");
    }

private:
    static std::string StateName(AvatarRendererState state) {
        switch (state) {
            case AvatarRendererState::Loading: return "Loading";
            case AvatarRendererState::Ready: return "Ready";
            case AvatarRendererState::Unavailable: return "Unavailable";
        }
        return "(unknown)";
    }

    AvatarRendererState stateBefore_ = AvatarRendererState::Ready;
    AvatarRendererState stateAfter_ = AvatarRendererState::Ready;
    bool drawWithSeventyOneThrew_ = true;
    bool drawWithSeventyThrew_ = false;
    int parentBoneCount_ = -1;
    bool bindPoseThrew_ = false;
};

} // namespace CnaExamples::Demos::Avatars::RendererDemos
