// SPDX-License-Identifier: MIT
//
// Asserts the mathematical facts the Math area's demo screens state on screen,
// so that prose is checked rather than trusted.
//
// This is not decoration. Writing it caught three claims that were confidently
// wrong before anyone would have seen them:
//
//   1. "A sheared matrix cannot be decomposed" -- false. CNA's Matrix::Decompose
//      returns TRUE for a shear and hands back parts that do not rebuild the
//      original. It only returns false when a scale axis is ~zero.
//   2. "0.1f + 0.2f != 0.3f" -- false in SINGLE precision. That famous example
//      is a double-precision result; in float the rounding coincides.
//   3. "WithinEpsilon rescues the comparison" -- not for 0.3f + 0.6f vs 0.9f,
//      where the gap is exactly MachineEpsilonFloat and the test is a strict <.
//
// Build and run (needs the app's own build tree for includes and libraries):
//
//   FLAGS=$(grep -m1 CXX_INCLUDES build/CMakeFiles/cna_examples.dir/flags.make | sed 's/^CXX_INCLUDES = //')
//   DEFS=$(grep -m1 CXX_DEFINES  build/CMakeFiles/cna_examples.dir/flags.make | sed 's/^CXX_DEFINES = //')
//   LINK=$(sed 's|^/usr/bin/c++ .*-o cna_examples||' build/CMakeFiles/cna_examples.dir/link.txt)
//   (cd build && g++ -std=c++23 $DEFS $FLAGS ../tools/checks/math_claims.cpp -o /tmp/math_claims $LINK)
//   /tmp/math_claims
#include <cmath>
#include <cstdio>
#include <vector>
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/ContainmentType.hpp"
#include "Microsoft/Xna/Framework/Curve.hpp"
#include "Microsoft/Xna/Framework/CurveKey.hpp"
#include "Microsoft/Xna/Framework/CurveKeyCollection.hpp"
#include "Microsoft/Xna/Framework/CurveTangent.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Quaternion.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
using namespace Microsoft::Xna::Framework;
static int fails = 0;
static void check(const char* what, bool ok) {
    std::printf("  %-58s %s\n", what, ok ? "ok" : "FAIL");
    if (!ok) fails++;
}
int main() {
    // Vector3OperationsScreen: right-handed cross product.
    const Vector3 h = Vector3::Cross(Vector3::UnitX, Vector3::UnitY);
    check("Cross(UnitX, UnitY) == +UnitZ  (right-handed)", h.Z > 0.99f);

    // TransformAndDistanceScreen: TransformNormal ignores translation.
    const Matrix world = Matrix::CreateTranslation(100.0f, 50.0f, 0.0f);
    const Vector3 pos = Vector3::Transform(Vector3(1,0,0), world);
    const Vector3 nrm = Vector3::TransformNormal(Vector3(1,0,0), world);
    check("Transform applies translation", std::fabs(pos.X - 101.0f) < 0.001f);
    check("TransformNormal ignores translation", std::fabs(nrm.X - 1.0f) < 0.001f);

    // MatrixCompositionScreen: multiplication does not commute.
    const Matrix r = Matrix::CreateRotationZ(1.0f);
    const Matrix t = Matrix::CreateTranslation(2,0,0);
    const Vector3 rt = Vector3::Transform(Vector3::Zero, r * t);
    const Vector3 tr = Vector3::Transform(Vector3::Zero, t * r);
    check("R*T != T*R", Vector3::Distance(rt, tr) > 0.5f);

    // DecomposeAndInvertScreen: a shear cannot be decomposed.
    Matrix sheared = Matrix::getIdentityProperty();
    sheared.M21 = 0.7f;
    Vector3 s, tr2; Quaternion q(0,0,0,1);
    // Real rule: Decompose fails only on a ~zero scale axis, NOT on shear.
    check("Decompose(shear) returns true (shear is not detected)",
          sheared.Decompose(s, q, tr2) == true);
    {   // ...and the parts it returns do not rebuild the sheared matrix.
        const Matrix rebuilt = Matrix::CreateScale(s) * Matrix::CreateFromQuaternion(q) *
                               Matrix::CreateTranslation(tr2);
        check("shear: rebuilt-from-parts != original (best fit, not decomposition)",
              std::fabs(rebuilt.M21 - sheared.M21) > 1e-3f);
    }
    {   Vector3 s2, t2; Quaternion q2(0,0,0,1);
        check("Decompose(zero-scale axis) returns false",
              Matrix::CreateScale(1.0f, 0.0f, 1.0f).Decompose(s2, q2, t2) == false);
    }
    check("Determinant(Scale(1,0,1)) == 0",
          std::fabs(Matrix::CreateScale(1.0f,0.0f,1.0f).Determinant()) < 1e-6f);

    // QuaternionRotationScreen: unit length, and Conjugate == Inverse.
    const Quaternion rot = Quaternion::CreateFromAxisAngle(Vector3::Normalize(Vector3(0.3f,1,0.2f)), 1.1f);
    check("rotation quaternion is unit length", std::fabs(rot.Length() - 1.0f) < 1e-4f);
    const Quaternion conj = Quaternion::Conjugate(rot), inv = Quaternion::Inverse(rot);
    check("Conjugate == Inverse for a unit quaternion",
          std::fabs(std::fabs(Quaternion::Dot(conj, inv)) - 1.0f) < 1e-4f);

    // SlerpVsLerpScreen: Slerp has constant angular velocity, Lerp does not.
    const Quaternion a = Quaternion::Identity;
    const Quaternion b = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, MathHelper::ToRadians(150.0f));
    auto angleOf = [](Quaternion q){ return 2.0f * std::acos(MathHelper::Clamp(std::fabs(q.W), -1.0f, 1.0f)); };
    const float s1 = angleOf(Quaternion::Slerp(a,b,0.2f)) - angleOf(Quaternion::Slerp(a,b,0.0f));
    const float s2 = angleOf(Quaternion::Slerp(a,b,0.6f)) - angleOf(Quaternion::Slerp(a,b,0.4f));
    check("Slerp angular step is constant", std::fabs(s1 - s2) < 0.02f);
    auto nlerp = [&](float x){ Quaternion q = Quaternion::Lerp(a,b,x); q.Normalize(); return q; };
    const float l1 = angleOf(nlerp(0.2f)) - angleOf(nlerp(0.0f));
    const float l2 = angleOf(nlerp(0.6f)) - angleOf(nlerp(0.4f));
    check("Lerp angular step is NOT constant", std::fabs(l1 - l2) > 0.02f);

    // BoundingVolumesScreen: all three ContainmentType states are reachable.
    const BoundingBox box(Vector3(-1.5f,-1.5f,-1.5f), Vector3(1.5f,1.5f,1.5f));
    check("Contains == Contains (sphere fully inside)",
          box.Contains(BoundingSphere(Vector3::Zero, 0.8f)) == ContainmentType::Contains);
    check("Contains == Intersects (straddling a face)",
          box.Contains(BoundingSphere(Vector3(1.4f,0,0), 0.8f)) == ContainmentType::Intersects);
    check("Contains == Disjoint (far away)",
          box.Contains(BoundingSphere(Vector3(4.0f,0,0), 0.8f)) == ContainmentType::Disjoint);

    // MathHelperScreen: the float-equality claim.
    check("0.1f + 0.2f == 0.3f in SINGLE precision (folklore is about double)",
          (0.1f + 0.2f) == 0.3f);
    check("0.3f + 0.6f != 0.9f in single precision", !((0.3f + 0.6f) == 0.9f));
    check("WithinEpsilon(0.3f+0.6f, 0.9f) is FALSE (gap == tolerance, strict <)",
          !MathHelper::WithinEpsilon(0.3f + 0.6f, 0.9f));
    check("WithinEpsilon accepts an identical pair",
          MathHelper::WithinEpsilon(0.9f, 0.9f));

    // CurveTangentsScreen: Flat tangents really are zero; Smooth's are not.
    Curve flat, smooth;
    for (auto* c : {&flat, &smooth}) {
        c->getKeysProperty().Add(CurveKey(0,0)); c->getKeysProperty().Add(CurveKey(1,2));
        c->getKeysProperty().Add(CurveKey(2,1)); c->getKeysProperty().Add(CurveKey(3,3));
    }
    flat.ComputeTangents(CurveTangent::Flat);
    smooth.ComputeTangents(CurveTangent::Smooth);
    check("Flat tangent on key 1 is zero",
          std::fabs(flat.getKeysProperty()[1].getTangentInProperty()) < 1e-6f);
    check("Smooth tangent on key 1 is non-zero",
          std::fabs(smooth.getKeysProperty()[1].getTangentInProperty()) > 1e-6f);

    std::printf("\n%s (%d failure(s))\n", fails ? "FAILURES" : "all math claims hold", fails);
    return fails ? 1 : 0;
}
