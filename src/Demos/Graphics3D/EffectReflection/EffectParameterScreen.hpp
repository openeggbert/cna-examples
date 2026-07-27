// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterClass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterType.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics3D::EffectReflectionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::EffectParameter;
using Microsoft::Xna::Framework::Graphics::EffectParameterClass;
using Microsoft::Xna::Framework::Graphics::EffectParameterCollection;
using Microsoft::Xna::Framework::Graphics::EffectParameterType;

// The other half of Effect reflection: an EffectParameter is a name, a type
// description, and a value -- and in CNA it is entirely self-contained. No
// GraphicsDevice, no compiled shader, no backend. It is plain typed storage,
// which is why this screen can build a parameter collection from nothing and
// exercise every accessor.
//
// Class and Type are two different axes and are easy to conflate:
//   * ParameterClass is the SHAPE  -- Scalar, Vector, Matrix, Object, Struct.
//   * ParameterType  is the ELEMENT -- Single, Int32, Bool, String, Texture...
// A Vector3 of floats is class Vector, type Single, 1 row and 3 columns. The
// rows/columns are what distinguish it from a Matrix of the same element type.
//
// The one genuine trap is SetValue vs SetValueTranspose. Both store a Matrix,
// but transposed relative to each other, and both read back through their own
// getter. Round-tripping a matrix through mismatched calls returns the
// transpose silently -- no error, just a wrong matrix. Shown below with a
// deliberately asymmetric matrix, since a symmetric one would hide it.
class EffectParameterScreen : public DemoScreen {
public:
    EffectParameterScreen() : DemoScreen("EffectParameter: Classes, Types & Values") {}

    void OnDemoLoad() override { RunChecks(); }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        lines.push_back("A collection built by hand -- no device, no shader, no backend:");
        lines.push_back("  name          class    type     rows  cols");
        for (const std::string& row : rows_) lines.push_back("  " + row);
        lines.push_back("Class is the SHAPE, Type the ELEMENT: a Vector3 of floats is class");
        lines.push_back("Vector, type Single, 1 row x 3 columns.");
        lines.push_back("SetValue/SetValueTranspose store transposed relative to each other, so a");
        lines.push_back("mismatched read silently returns the transpose: round trip " +
                        YesNo(matrixRoundTrip_) + ", crossed transposed " +
                        YesNo(crossIsTransposed_));
        lines.push_back("Lookup by name returns a pointer, null when absent (" +
                        std::string(missingIsNull_ ? "confirmed" : "NOT confirmed") +
                        ") -- it never throws.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        DrawVerdict(sb, font, end.Y + 6.0f,
                    mul(allHold_ ? Color(40, 200, 90, 255) : Color(220, 60, 60, 255),
                        TransitionAlpha()),
                    tint,
                    allHold_ ? "All " + std::to_string(checks_) + " value round trips verified."
                             : "A round trip FAILED: " + failure_);
    }

private:
    static std::string YesNo(bool value) { return value ? "yes" : "no"; }

    static const char* ClassName(EffectParameterClass value) {
        switch (value) {
            case EffectParameterClass::Scalar: return "Scalar";
            case EffectParameterClass::Vector: return "Vector";
            case EffectParameterClass::Matrix: return "Matrix";
            case EffectParameterClass::Object: return "Object";
            default:                           return "Struct";
        }
    }

    static const char* TypeName(EffectParameterType value) {
        switch (value) {
            case EffectParameterType::Void:        return "Void";
            case EffectParameterType::Bool:        return "Bool";
            case EffectParameterType::Int32:       return "Int32";
            case EffectParameterType::Single:      return "Single";
            case EffectParameterType::String:      return "String";
            case EffectParameterType::Texture:     return "Texture";
            case EffectParameterType::Texture1D:   return "Texture1D";
            case EffectParameterType::Texture2D:   return "Texture2D";
            case EffectParameterType::Texture3D:   return "Texture3D";
            default:                               return "TextureCube";
        }
    }

    void Note(bool ok, const std::string& what) {
        ++checks_;
        if (!ok && allHold_) {
            allHold_ = false;
            failure_ = what;
        }
    }

    void RunChecks() {
        EffectParameterCollection parameters;
        parameters.Add(EffectParameter("Opacity", "OPACITY", 1, 1,
                                       EffectParameterClass::Scalar, EffectParameterType::Single));
        parameters.Add(EffectParameter("Steps", "", 1, 1,
                                       EffectParameterClass::Scalar, EffectParameterType::Int32));
        parameters.Add(EffectParameter("Enabled", "", 1, 1,
                                       EffectParameterClass::Scalar, EffectParameterType::Bool));
        parameters.Add(EffectParameter("Tint", "COLOR", 1, 3,
                                       EffectParameterClass::Vector, EffectParameterType::Single));
        parameters.Add(EffectParameter("WorldViewProj", "WORLDVIEWPROJ", 4, 4,
                                       EffectParameterClass::Matrix, EffectParameterType::Single));

        for (int i = 0; i < parameters.getCountProperty(); ++i) {
            const EffectParameter& p = parameters[i];
            std::string row = p.getNameProperty();
            row.resize(14, ' ');
            std::string cls = ClassName(p.getParameterClassProperty());
            cls.resize(9, ' ');
            std::string type = TypeName(p.getParameterTypeProperty());
            type.resize(9, ' ');
            rows_.push_back(row + cls + type + "  " + std::to_string(p.getRowCountProperty()) +
                            "     " + std::to_string(p.getColumnCountProperty()));
        }

        // Scalar round trips, one per element type.
        if (auto* opacity = parameters["Opacity"]) {
            opacity->SetValue(0.75f);
            Note(std::fabs(opacity->GetValueSingle() - 0.75f) < 1e-6f, "Single");
        }
        if (auto* steps = parameters["Steps"]) {
            steps->SetValue(7);
            Note(steps->GetValueInt32() == 7, "Int32");
        }
        if (auto* enabled = parameters["Enabled"]) {
            enabled->SetValue(true);
            Note(enabled->GetValueBoolean(), "Bool");
        }
        if (auto* tint = parameters["Tint"]) {
            tint->SetValue(Vector3(0.2f, 0.4f, 0.6f));
            const Vector3 got = tint->GetValueVector3();
            Note(std::fabs(got.X - 0.2f) < 1e-6f && std::fabs(got.Y - 0.4f) < 1e-6f &&
                     std::fabs(got.Z - 0.6f) < 1e-6f,
                 "Vector3");
        }

        // The transpose trap. M12 != M21 deliberately, so a transposed result
        // is distinguishable -- a symmetric matrix would hide the bug.
        Matrix asymmetric = Matrix::getIdentityProperty();
        asymmetric.M12 = 2.0f;
        asymmetric.M21 = 5.0f;
        asymmetric.M41 = 9.0f;

        if (auto* wvp = parameters["WorldViewProj"]) {
            wvp->SetValue(asymmetric);
            const Matrix same = wvp->GetValueMatrix();
            matrixRoundTrip_ = std::fabs(same.M12 - 2.0f) < 1e-6f &&
                               std::fabs(same.M21 - 5.0f) < 1e-6f &&
                               std::fabs(same.M41 - 9.0f) < 1e-6f;
            Note(matrixRoundTrip_, "Matrix");

            const Matrix crossed = wvp->GetValueMatrixTranspose();
            crossIsTransposed_ = std::fabs(crossed.M12 - 5.0f) < 1e-6f &&
                                 std::fabs(crossed.M21 - 2.0f) < 1e-6f &&
                                 std::fabs(crossed.M14 - 9.0f) < 1e-6f;
            Note(crossIsTransposed_, "Matrix/Transpose mismatch");

            // And the matching pair round trips cleanly.
            wvp->SetValueTranspose(asymmetric);
            const Matrix back = wvp->GetValueMatrixTranspose();
            Note(std::fabs(back.M12 - 2.0f) < 1e-6f && std::fabs(back.M21 - 5.0f) < 1e-6f,
                 "Transpose/Transpose");
        }

        foundByName_ = parameters["Tint"] != nullptr;
        missingIsNull_ = parameters["NoSuch"] == nullptr;
        Note(foundByName_, "lookup by name");
        Note(missingIsNull_, "missing name returns nullptr");
    }

    std::vector<std::string> rows_;
    std::string failure_;
    int checks_ = 0;
    bool allHold_ = true;
    bool matrixRoundTrip_ = false;
    bool crossIsTransposed_ = false;
    bool foundByName_ = false;
    bool missingIsNull_ = false;
};

} // namespace CnaExamples::Demos::Graphics3D::EffectReflectionDemos
