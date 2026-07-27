// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::VectorsDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Vector3;

// Cross is the operation Vector3 has and Vector2 does not, and it is the one
// with a handedness convention attached -- get it backwards and your surface
// normals point into the wall.
//
// XNA (and CNA) are right-handed: Cross(UnitX, UnitY) == +UnitZ. That identity
// is computed live below rather than asserted, because it is the single fastest
// way to check which convention a framework uses.
//
// Reflect is shown alongside it because it is the other operation whose result
// is easy to state and easy to get subtly wrong: it needs a *unit* normal, and
// feeding it an unnormalised one silently scales the result.
class Vector3OperationsScreen : public DemoScreen {
public:
    Vector3OperationsScreen() : DemoScreen("Vectors: Vector3 Cross, Dot & Reflect") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        angle_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.5f;
        angle_ = MathHelper::WrapAngle(angle_);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Vector3 a(std::cos(angle_), std::sin(angle_), 0.35f);
        const Vector3 b(0.0f, 1.0f, 0.0f);

        const Vector3 cross = Vector3::Cross(a, b);
        const float dot = Vector3::Dot(a, b);
        const Vector3 unitNormal = Vector3::Normalize(Vector3(0.0f, 1.0f, 0.0f));
        const Vector3 reflected = Vector3::Reflect(a, unitNormal);

        // The handedness check, computed rather than claimed.
        const Vector3 handedness = Vector3::Cross(Vector3::UnitX, Vector3::UnitY);

        std::vector<std::string> lines;
        lines.push_back("a rotates in the XY plane with a fixed Z; b is world up.");
        lines.emplace_back();
        lines.push_back("a = " + V3(a) + "   Length " + F(a.Length()));
        lines.push_back("b = " + V3(b));
        lines.emplace_back();
        lines.push_back("Cross(a, b) = " + V3(cross));
        lines.push_back("  perpendicular to both -- Dot(cross,a) = " + F(Vector3::Dot(cross, a)) +
                        ", Dot(cross,b) = " + F(Vector3::Dot(cross, b)));
        lines.push_back("  |Cross| = " + F(cross.Length()) +
                        "  (= |a||b|sin, so it collapses to 0 when a and b are parallel)");
        lines.emplace_back();
        lines.push_back("Dot(a, b)   = " + F(dot));
        lines.push_back("Reflect(a, UnitY) = " + V3(reflected) +
                        "   (Y is negated; X and Z pass through)");
        lines.emplace_back();
        lines.push_back("Handedness check, computed live:");
        lines.push_back("  Cross(UnitX, UnitY) = " + V3(handedness) +
                        (handedness.Z > 0.5f ? "   -> +Z, i.e. RIGHT-handed"
                                             : "   -> -Z, i.e. left-handed"));
        lines.emplace_back();
        lines.push_back("Reflect needs a UNIT normal. Passing an unnormalised one does not");
        lines.push_back("error -- it silently scales the result, which is why it is easy to miss.");
        lines.push_back("  Reflect(a, b*2) = " + V3(Vector3::Reflect(a, Vector3(0.0f, 2.0f, 0.0f))));

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    float angle_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::VectorsDemos
