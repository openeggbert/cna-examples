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

// Decompose pulls a world matrix back apart into scale, rotation and
// translation, and returns a bool saying whether it could.
//
// What that bool actually detects is narrower than people expect, and this
// screen shows the real rule rather than the assumed one. CNA (following FNA)
// derives each scale axis from the length of a basis row and returns false only
// when one of them is ~zero -- i.e. the matrix collapses a dimension. That is
// the "Singular" case below.
//
// A SHEARED matrix returns TRUE. Decompose cannot represent shear in
// scale/rotation/translation, so what comes back is a best-fit that does not
// reconstruct the original -- a silently wrong answer rather than a reported
// failure. The screen multiplies the parts back together and shows the
// mismatch, because that is the only way to notice.
//
// Invert and Determinant are shown together for the same reason: a matrix with
// a zero determinant is singular and has no inverse, and that is the case worth
// being able to recognise.
class DecomposeAndInvertScreen : public DemoScreen {
public:
    DecomposeAndInvertScreen() : DemoScreen("Matrix: Decompose, Invert & Determinant") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            caseIndex_ = (caseIndex_ + 1) % kCaseCount;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Matrix source = BuildCase(caseIndex_);

        Vector3 scale;
        // Quaternion has no default constructor in CNA; seed it with identity.
        Quaternion rotation(0.0f, 0.0f, 0.0f, 1.0f);
        Vector3 translation;
        const bool decomposed = source.Decompose(scale, rotation, translation);

        const float determinant = source.Determinant();
        const Matrix inverse = Matrix::Invert(source);
        const Matrix roundTrip = source * inverse;

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: cycle the source matrix");
        lines.emplace_back();
        lines.push_back(std::string("Case: ") + kCaseNames[caseIndex_]);
        lines.emplace_back();
        lines.push_back("Decompose() returned: " +
                        std::string(decomposed ? "true" : "FALSE -- a scale axis is ~zero"));
        if (decomposed) {
            lines.push_back("  scale       " + V3(scale));
            lines.push_back("  translation " + V3(translation));
            lines.push_back("  rotation    (" + F(rotation.X) + ", " + F(rotation.Y) + ", " +
                            F(rotation.Z) + ", " + F(rotation.W) + ")");

            // Rebuild from the parts. If they do not reproduce the original, the
            // "true" above was a best fit, not a decomposition.
            const Matrix rebuilt = Matrix::CreateScale(scale) *
                                   Matrix::CreateFromQuaternion(rotation) *
                                   Matrix::CreateTranslation(translation);
            lines.push_back("  rebuilt from the parts == original: " +
                            std::string(Approximately(rebuilt, source)
                                            ? "yes -- a true decomposition"
                                            : "NO -- 'true' was a best fit, not a decomposition"));
        } else {
            lines.push_back("  One basis row has ~zero length, so there is no scale to divide by.");
            lines.push_back("  rotation is set to Identity and the bool is the real answer.");
        }
        lines.emplace_back();
        lines.push_back("Determinant: " + F(determinant, 4) +
                        (MathHelper::WithinEpsilon(determinant, 0.0f)
                             ? "   -- zero: singular, no inverse exists"
                             : "   -- non-zero: invertible"));
        lines.emplace_back();
        lines.push_back("M * Invert(M) should be identity. Diagonal of the product:");
        lines.push_back("  " + F(roundTrip.M11, 4) + "  " + F(roundTrip.M22, 4) + "  " +
                        F(roundTrip.M33, 4) + "  " + F(roundTrip.M44, 4));
        lines.push_back("  is identity: " + std::string(IsIdentity(roundTrip) ? "yes" : "no"));
        lines.emplace_back();
        lines.push_back("Transpose(M) is NOT the inverse in general -- only for a pure rotation,");
        lines.push_back("where it is, and is much cheaper. For this matrix:");
        lines.push_back("  Transpose == Invert: " +
                        std::string(Approximately(Matrix::Transpose(source), inverse) ? "yes" : "no"));
        lines.emplace_back();
        lines.push_back("The shear case is the one worth sitting with: Decompose reports success");
        lines.push_back("and hands back parts that do not rebuild the matrix. Only checking the");
        lines.push_back("round trip catches it -- the return value alone will not.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr int kCaseCount = 4;
    static constexpr const char* kCaseNames[kCaseCount] = {
        "Scale(2,3,1) * RotationZ(30) * Translation(5,1,0)",
        "Pure rotation: RotationY(45)",
        "Singular: Scale(1, 0, 1)   -- a zero scale axis",
        "Sheared (M21 = 0.7)        -- decomposes, but WRONGLY",
    };

    static Matrix BuildCase(int index) {
        switch (index) {
            case 0:
                return Matrix::CreateScale(2.0f, 3.0f, 1.0f) *
                       Matrix::CreateRotationZ(MathHelper::ToRadians(30.0f)) *
                       Matrix::CreateTranslation(5.0f, 1.0f, 0.0f);
            case 1:
                return Matrix::CreateRotationY(MathHelper::ToRadians(45.0f));
            case 2:
                return Matrix::CreateScale(1.0f, 0.0f, 1.0f);
            default: {
                Matrix sheared = Matrix::getIdentityProperty();
                sheared.M21 = 0.7f;
                return sheared;
            }
        }
    }

    static bool IsIdentity(const Matrix& m) {
        const Matrix identity = Matrix::getIdentityProperty();
        return Approximately(m, identity);
    }

    // Field by field rather than walking a float* from &M11: that would bake in
    // an assumption about Matrix's storage layout which nothing guarantees.
    static bool Approximately(const Matrix& a, const Matrix& b) {
        return MathHelper::WithinEpsilon(a.M11, b.M11) && MathHelper::WithinEpsilon(a.M12, b.M12) &&
               MathHelper::WithinEpsilon(a.M13, b.M13) && MathHelper::WithinEpsilon(a.M14, b.M14) &&
               MathHelper::WithinEpsilon(a.M21, b.M21) && MathHelper::WithinEpsilon(a.M22, b.M22) &&
               MathHelper::WithinEpsilon(a.M23, b.M23) && MathHelper::WithinEpsilon(a.M24, b.M24) &&
               MathHelper::WithinEpsilon(a.M31, b.M31) && MathHelper::WithinEpsilon(a.M32, b.M32) &&
               MathHelper::WithinEpsilon(a.M33, b.M33) && MathHelper::WithinEpsilon(a.M34, b.M34) &&
               MathHelper::WithinEpsilon(a.M41, b.M41) && MathHelper::WithinEpsilon(a.M42, b.M42) &&
               MathHelper::WithinEpsilon(a.M43, b.M43) && MathHelper::WithinEpsilon(a.M44, b.M44);
    }

    int caseIndex_ = 0;
};

} // namespace CnaExamples::Demos::Math::MatrixQuaternionDemos
