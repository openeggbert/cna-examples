// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::MatrixQuaternionDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;

// Matrix multiplication is not commutative, and this is the demo of that fact.
//
// Rotate-then-translate moves the object along the WORLD axes after turning it.
// Translate-then-rotate moves it first, then swings the whole thing around the
// origin -- so the object orbits instead of spinning in place.
//
// Both orders are drawn simultaneously from the same two component matrices, so
// the divergence is a property of the multiplication and nothing else.
//
// In XNA's row-vector convention, `A * B` means "apply A, then B" -- the
// opposite reading order from the column-vector convention most linear-algebra
// texts use. That is the part people import a bug from.
class MatrixCompositionScreen : public DemoScreen {
public:
    MatrixCompositionScreen() : DemoScreen("Matrix: Composition Order") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        angle_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.7f;
        angle_ = MathHelper::WrapAngle(angle_);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Matrix rotation = Matrix::CreateRotationZ(angle_);
        const Matrix translation = Matrix::CreateTranslation(2.0f, 0.0f, 0.0f);

        const Matrix rotateThenTranslate = rotation * translation;
        const Matrix translateThenRotate = translation * rotation;

        const Vector3 origin = Vector3::Zero;
        const Vector3 aResult = Vector3::Transform(origin, rotateThenTranslate);
        const Vector3 bResult = Vector3::Transform(origin, translateThenRotate);

        std::vector<std::string> lines;
        lines.push_back("The same two matrices, multiplied both ways, applied to the origin.");
        lines.emplace_back();
        lines.push_back("angle = " + F(angle_, 2) + " rad");
        lines.emplace_back();
        lines.push_back("rotation * translation  -> " + V3(aResult));
        lines.push_back("  spins in place, then steps 2 along world X: a fixed offset from centre");
        lines.emplace_back();
        lines.push_back("translation * rotation  -> " + V3(bResult));
        lines.push_back("  steps out first, then the whole thing rotates: it orbits the origin");
        lines.emplace_back();
        lines.push_back("Same operands, different results -- matrix multiply does not commute.");
        lines.push_back("Distance between the two results: " +
                        F(Vector3::Distance(aResult, bResult)));
        lines.emplace_back();
        lines.push_back("XNA uses row vectors, so A * B reads \"apply A, then B\" -- the reverse");
        lines.push_back("of the column-vector convention in most textbooks. Importing the habit");
        lines.push_back("without importing the convention is where the bug usually comes from.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawDiagram(sb, font, aResult, bResult);
    }

private:
    void DrawDiagram(SpriteBatch& sb, SpriteFont& font, const Vector3& a, const Vector3& b) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();
        const PlotSpace plot{Vector2((float)viewport.getWidthProperty() - 170.0f, 280.0f), 34.0f};

        DrawLine(sb, plot.ToScreen(Vector2(-3.5f, 0.0f)), plot.ToScreen(Vector2(3.5f, 0.0f)),
                 mul(Color(70, 70, 70), alpha), 1.0f);
        DrawLine(sb, plot.ToScreen(Vector2(0.0f, -3.5f)), plot.ToScreen(Vector2(0.0f, 3.5f)),
                 mul(Color(70, 70, 70), alpha), 1.0f);

        const Vector2 aScreen = plot.ToScreen(Vector2(a.X, a.Y));
        const Vector2 bScreen = plot.ToScreen(Vector2(b.X, b.Y));

        FillRect(sb, Rectangle((int)aScreen.X - 6, (int)aScreen.Y - 6, 13, 13),
                 mul(Color(230, 190, 70), alpha));
        FillRect(sb, Rectangle((int)bScreen.X - 6, (int)bScreen.Y - 6, 13, 13),
                 mul(Color(90, 200, 120), alpha));

        sb.DrawString(font, "R*T", Vector2(aScreen.X + 10.0f, aScreen.Y - 10.0f),
                      mul(Color(230, 190, 70), alpha));
        sb.DrawString(font, "T*R", Vector2(bScreen.X + 10.0f, bScreen.Y - 10.0f),
                      mul(Color(90, 200, 120), alpha));
    }

    float angle_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::MatrixQuaternionDemos
