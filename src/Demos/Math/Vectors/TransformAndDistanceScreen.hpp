// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::VectorsDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;

// Transform vs TransformNormal is one of the most consequential distinctions in
// the whole math API, and the two calls look identical at the call site.
//
//   Transform       treats the input as a POSITION: rotation, scale AND
//                   translation all apply.
//   TransformNormal treats it as a DIRECTION: translation is ignored.
//
// Getting it wrong is not a crash, it is lighting that breaks only when the
// object moves away from the origin -- because a normal that was translated is
// still a unit vector when the object sits at (0,0,0). This screen shows both
// results side by side under a matrix that has a large translation, so the gap
// is unmissable.
//
// DistanceSquared is included for the related reason that it is the one people
// forget: comparing squared distances avoids a square root per comparison, and
// the ordering is identical.
class TransformAndDistanceScreen : public DemoScreen {
public:
    TransformAndDistanceScreen() : DemoScreen("Vectors: Transform & Distance") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            translated_ = !translated_;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Vector3 translation = translated_ ? Vector3(100.0f, 50.0f, 0.0f) : Vector3::Zero;
        const Matrix world = Matrix::CreateRotationZ(0.7853982f) *   // 45 degrees
                             Matrix::CreateScale(2.0f) *
                             Matrix::CreateTranslation(translation);

        const Vector3 point(1.0f, 0.0f, 0.0f);
        const Vector3 direction(1.0f, 0.0f, 0.0f);

        const Vector3 asPosition = Vector3::Transform(point, world);
        const Vector3 asNormal = Vector3::TransformNormal(direction, world);

        const Vector2 a(3.0f, 4.0f);
        const Vector2 b(0.0f, 0.0f);

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: toggle the matrix's translation on and off");
        lines.emplace_back();
        lines.push_back("World = RotationZ(45) * Scale(2) * Translation" + V3(translation, 0));
        lines.emplace_back();
        lines.push_back("Same input vector (1, 0, 0), two different calls:");
        lines.push_back("  Transform       -> " + V3(asPosition) + "   (a POSITION)");
        lines.push_back("  TransformNormal -> " + V3(asNormal) + "   (a DIRECTION)");
        lines.push_back("  Difference: " + V3(Vector3::Subtract(asPosition, asNormal)));
        lines.emplace_back();
        lines.push_back(translated_
            ? "With a translation the two disagree by exactly that translation."
            : "With NO translation the two agree -- which is why this bug hides until an");
        if (!translated_) {
            lines.push_back("object moves away from the origin. Toggle the translation on.");
        }
        lines.emplace_back();
        lines.push_back("Distance, on Vector2 a=" + V2(a) + " b=" + V2(b) + ":");
        lines.push_back("  Distance        = " + F(Vector2::Distance(a, b)));
        lines.push_back("  DistanceSquared = " + F(Vector2::DistanceSquared(a, b)) +
                        "   (no square root; same ordering, so use it to compare)");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    bool translated_ = true;
};

} // namespace CnaExamples::Demos::Math::VectorsDemos
