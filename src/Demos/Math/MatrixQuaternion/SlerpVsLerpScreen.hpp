// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Quaternion.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::MatrixQuaternionDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Quaternion;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;

// Slerp and Lerp both get from rotation A to rotation B, and both are correct at
// t=0 and t=1. They differ everywhere in between, and the difference is angular
// speed, not path:
//
//   Slerp -- spherical, constant angular velocity. The rotation sweeps evenly.
//   Lerp  -- straight line through 4D component space, then renormalised. The
//            rotation speeds up in the middle and slows at the ends. Cheaper.
//
// The gap widens with the angle between the two rotations, so this screen uses
// a deliberately large one (150 degrees) where it is unmistakable, and prints
// the per-step angular delta for both so the "constant velocity" claim is a
// measurement rather than an assertion.
class SlerpVsLerpScreen : public DemoScreen {
public:
    SlerpVsLerpScreen() : DemoScreen("Quaternion: Slerp vs Lerp") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        t_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.3f;
        if (t_ > 1.0f) t_ -= 1.0f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Quaternion from = Quaternion::Identity;
        const Quaternion to = Quaternion::CreateFromAxisAngle(Vector3::UnitZ,
                                                              MathHelper::ToRadians(150.0f));

        const Quaternion slerped = Quaternion::Slerp(from, to, t_);
        Quaternion lerped = Quaternion::Lerp(from, to, t_);
        lerped.Normalize();   // Lerp does not preserve unit length on its own

        const Vector3 probe(1.0f, 0.0f, 0.0f);
        const Vector3 slerpVec = Vector3::Transform(probe, slerped);
        const Vector3 lerpVec = Vector3::Transform(probe, lerped);

        std::vector<std::string> lines;
        lines.push_back("Rotating 150 degrees about Z. Both are exact at t=0 and t=1.");
        lines.emplace_back();
        lines.push_back("t = " + F(t_, 3));
        lines.push_back("  Slerp angle from start: " + F(MathHelper::ToDegrees(AngleOf(slerped)), 1) + " deg");
        lines.push_back("  Lerp  angle from start: " + F(MathHelper::ToDegrees(AngleOf(lerped)), 1) + " deg");
        lines.push_back("  Difference: " +
                        F(MathHelper::ToDegrees(std::fabs(AngleOf(slerped) - AngleOf(lerped))), 1) +
                        " deg");
        lines.emplace_back();
        lines.push_back("Angle swept per 0.1 of t -- constant for Slerp, not for Lerp:");
        lines.push_back("  t      Slerp    Lerp");
        for (int step = 0; step < 5; ++step) {
            const float a = (float)step * 0.2f;
            const float b = a + 0.2f;
            const float slerpDelta = MathHelper::ToDegrees(
                AngleOf(Quaternion::Slerp(from, to, b)) - AngleOf(Quaternion::Slerp(from, to, a)));
            const float lerpDelta = MathHelper::ToDegrees(
                AngleOf(NormalizedLerp(from, to, b)) - AngleOf(NormalizedLerp(from, to, a)));
            lines.push_back("  " + F(a, 1) + "-" + F(b, 1) + "  " + F(slerpDelta, 1) +
                            "     " + F(lerpDelta, 1));
        }
        lines.emplace_back();
        lines.push_back("Lerp does not preserve unit length -- it must be normalised afterwards,");
        lines.push_back("which this screen does. Slerp stays on the unit sphere by construction.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawDial(sb, font, slerpVec, lerpVec);
    }

private:
    static Quaternion NormalizedLerp(const Quaternion& a, const Quaternion& b, float t) {
        Quaternion result = Quaternion::Lerp(a, b, t);
        result.Normalize();
        return result;
    }

    // Rotation angle encoded by a unit quaternion: 2 * acos(W).
    static float AngleOf(const Quaternion& q) {
        return 2.0f * std::acos(MathHelper::Clamp(std::fabs(q.W), -1.0f, 1.0f));
    }

    void DrawDial(SpriteBatch& sb, SpriteFont& font, const Vector3& slerp, const Vector3& lerp) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();
        const PlotSpace plot{Vector2((float)viewport.getWidthProperty() - 150.0f, 250.0f), 60.0f};
        const Vector2 centre = plot.ToScreen(Vector2::Zero);

        DrawLine(sb, plot.ToScreen(Vector2(-1.4f, 0.0f)), plot.ToScreen(Vector2(1.4f, 0.0f)),
                 mul(Color(60, 60, 60), alpha), 1.0f);
        DrawLine(sb, plot.ToScreen(Vector2(0.0f, -1.4f)), plot.ToScreen(Vector2(0.0f, 1.4f)),
                 mul(Color(60, 60, 60), alpha), 1.0f);

        DrawArrow(sb, centre, plot.ToScreen(Vector2(slerp.X, slerp.Y)),
                  mul(Color(90, 200, 120), alpha), 3.0f);
        DrawArrow(sb, centre, plot.ToScreen(Vector2(lerp.X, lerp.Y)),
                  mul(Color(230, 190, 70), alpha), 3.0f);

        sb.DrawString(font, "Slerp", Vector2(centre.X - 60.0f, centre.Y + 80.0f),
                      mul(Color(90, 200, 120), alpha));
        sb.DrawString(font, "Lerp", Vector2(centre.X - 60.0f, centre.Y + 108.0f),
                      mul(Color(230, 190, 70), alpha));
    }

    float t_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::MatrixQuaternionDemos
