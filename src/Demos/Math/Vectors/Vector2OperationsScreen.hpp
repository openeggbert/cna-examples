// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::VectorsDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Vector2;

// The core Vector2 operations, drawn as well as printed. A slowly rotating
// vector `a` keeps the numbers moving, so the relationship between the picture
// and the values is continuously visible rather than a single frozen case.
//
// The two worth watching are Dot and the Normalize/Length pair:
//   - Dot goes positive when the vectors point the same way, through zero when
//     they are perpendicular, and negative when they oppose. The sign is the
//     useful part, and it changes on screen as `a` sweeps round.
//   - Normalize scales to unit length, so Length becomes 1.00 while the
//     direction is untouched.
class Vector2OperationsScreen : public DemoScreen {
public:
    Vector2OperationsScreen() : DemoScreen("Vectors: Vector2 Operations") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        angle_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.6f;
        angle_ = MathHelper::WrapAngle(angle_);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Vector2 a(std::cos(angle_) * 2.0f, std::sin(angle_) * 2.0f);
        const Vector2 b(2.5f, 1.0f);

        const Vector2 sum = Vector2::Add(a, b);
        const Vector2 difference = Vector2::Subtract(a, b);
        const float dot = Vector2::Dot(a, b);
        const Vector2 normalizedA = Vector2::Normalize(a);
        const Vector2 reflected = Vector2::Reflect(a, Vector2::Normalize(b));

        std::vector<std::string> lines;
        lines.push_back("a rotates; b is fixed. Every number below is recomputed each frame.");
        lines.emplace_back();
        lines.push_back("a = " + V2(a) + "   Length " + F(a.Length()) +
                        "   LengthSquared " + F(a.LengthSquared()));
        lines.push_back("b = " + V2(b) + "   Length " + F(b.Length()));
        lines.emplace_back();
        lines.push_back("Add(a, b)       = " + V2(sum));
        lines.push_back("Subtract(a, b)  = " + V2(difference));
        lines.push_back("Dot(a, b)       = " + F(dot) + "   " + DotMeaning(dot));
        lines.push_back("Normalize(a)    = " + V2(normalizedA) +
                        "   Length " + F(normalizedA.Length()));
        lines.push_back("Reflect(a, n(b))= " + V2(reflected) +
                        "   (a bounced off the plane whose normal is b)");
        lines.push_back("Distance(a, b)  = " + F(Vector2::Distance(a, b)) +
                        "   DistanceSquared " + F(Vector2::DistanceSquared(a, b)));

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawDiagram(sb, font, a, b, sum);
    }

private:
    static std::string DotMeaning(float dot) {
        if (dot > 0.15f)  return "positive: pointing broadly the same way";
        if (dot < -0.15f) return "negative: pointing broadly opposite";
        return "~zero: roughly perpendicular";
    }

    void DrawDiagram(SpriteBatch& sb, SpriteFont& font, const Vector2& a, const Vector2& b,
                     const Vector2& sum) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();

        const PlotSpace plot{Vector2((float)viewport.getWidthProperty() - 190.0f, 300.0f), 26.0f};

        // Axes first, so the vectors sit on top of them.
        DrawLine(sb, plot.ToScreen(Vector2(-5.0f, 0.0f)), plot.ToScreen(Vector2(5.0f, 0.0f)),
                 mul(Color(70, 70, 70), alpha), 1.0f);
        DrawLine(sb, plot.ToScreen(Vector2(0.0f, -5.0f)), plot.ToScreen(Vector2(0.0f, 5.0f)),
                 mul(Color(70, 70, 70), alpha), 1.0f);

        const Vector2 centre = plot.ToScreen(Vector2::Zero);
        DrawArrow(sb, centre, plot.ToScreen(sum), mul(Color(90, 90, 140), alpha), 2.0f);
        DrawArrow(sb, centre, plot.ToScreen(b), mul(Color(90, 200, 120), alpha), 3.0f);
        DrawArrow(sb, centre, plot.ToScreen(a), mul(Color(230, 190, 70), alpha), 3.0f);

        sb.DrawString(font, "a", plot.ToScreen(a), mul(Color(230, 190, 70), alpha));
        sb.DrawString(font, "b", plot.ToScreen(b), mul(Color(90, 200, 120), alpha));
        sb.DrawString(font, "a+b", plot.ToScreen(sum), mul(Color(140, 140, 190), alpha));
    }

    float angle_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::VectorsDemos
