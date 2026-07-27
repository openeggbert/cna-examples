// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Quaternion.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::MatrixQuaternionDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Quaternion;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;

// A quaternion is four numbers that encode a rotation, and the useful facts
// about it are all checkable rather than memorisable:
//
//   - A rotation quaternion is always unit length. If Length() drifts from 1
//     after repeated multiplication, that is accumulated error, and Normalize
//     is the fix.
//   - q and -q are the SAME rotation. That is not a rounding artifact; it is
//     why naive interpolation between two quaternions can take the long way
//     round (see the Slerp demo).
//   - Conjugate == Inverse for a unit quaternion, and is far cheaper.
//
// All three are computed live here, including the round trip through a matrix
// and back.
class QuaternionRotationScreen : public DemoScreen {
public:
    QuaternionRotationScreen() : DemoScreen("Quaternion: Rotation Basics") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        angle_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.5f;
        angle_ = MathHelper::WrapAngle(angle_);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Vector3 axis = Vector3::Normalize(Vector3(0.3f, 1.0f, 0.2f));
        const Quaternion fromAxis = Quaternion::CreateFromAxisAngle(axis, angle_);
        const Quaternion fromEuler =
            Quaternion::CreateFromYawPitchRoll(angle_, angle_ * 0.5f, 0.0f);

        // Round trip: quaternion -> matrix -> quaternion.
        const Matrix asMatrix = Matrix::CreateFromQuaternion(fromAxis);
        const Quaternion backAgain = Quaternion::CreateFromRotationMatrix(asMatrix);

        const Quaternion conjugate = Quaternion::Conjugate(fromAxis);
        const Quaternion inverse = Quaternion::Inverse(fromAxis);

        // A rotation applied and then undone must return the original vector.
        const Vector3 probe(1.0f, 0.0f, 0.0f);
        const Vector3 rotated = Vector3::Transform(probe, fromAxis);
        const Vector3 restored = Vector3::Transform(rotated, conjugate);

        std::vector<std::string> lines;
        lines.push_back("angle = " + F(angle_, 3) + " rad about axis " + V3(axis));
        lines.emplace_back();
        lines.push_back("CreateFromAxisAngle       = " + Q(fromAxis));
        lines.push_back("  Length = " + F(fromAxis.Length(), 5) +
                        "   (a rotation quaternion is always unit length)");
        lines.push_back("CreateFromYawPitchRoll    = " + Q(fromEuler));
        lines.emplace_back();
        lines.push_back("Round trip through a matrix:");
        lines.push_back("  quaternion -> matrix -> quaternion = " + Q(backAgain));
        lines.push_back("  same rotation as the original: " +
                        std::string(SameRotation(fromAxis, backAgain) ? "yes" : "no"));
        lines.emplace_back();
        lines.push_back("Conjugate = " + Q(conjugate));
        lines.push_back("Inverse   = " + Q(inverse));
        lines.push_back("  equal (they are, for a unit quaternion -- and Conjugate is cheaper): " +
                        std::string(SameRotation(conjugate, inverse) ? "yes" : "no"));
        lines.emplace_back();
        lines.push_back("Apply then undo, on " + V3(probe) + ":");
        lines.push_back("  rotated  = " + V3(rotated));
        lines.push_back("  restored = " + V3(restored) + "   back to the original: " +
                        std::string(Vector3::Distance(probe, restored) < 0.001f ? "yes" : "no"));
        lines.emplace_back();
        lines.push_back("q and -q encode the SAME rotation -- which is what makes naive");
        lines.push_back("interpolation able to take the long way round. See the Slerp demo.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static std::string Q(const Quaternion& q) {
        return "(" + F(q.X) + ", " + F(q.Y) + ", " + F(q.Z) + ", " + F(q.W) + ")";
    }

    // Compares rotations, not components: q and -q are the same rotation, so a
    // component-wise comparison would report a false mismatch half the time.
    static bool SameRotation(const Quaternion& a, const Quaternion& b) {
        const float dot = Quaternion::Dot(a, b);
        return std::fabs(std::fabs(dot) - 1.0f) < 0.001f;
    }

    float angle_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::MatrixQuaternionDemos
