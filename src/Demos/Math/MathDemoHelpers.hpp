// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <cstdio>
#include <string>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace CnaExamples::Demos::Math {

using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;

// Math demos are mostly numbers, and unformatted floats are unreadable at a
// glance -- "0.707107" and "-0.000000" both need fixing. Two decimals is enough
// to see what an operation did, and a shared formatter keeps every screen in
// this Area lined up.

inline std::string F(float value, int decimals = 2) {
    char buf[48];
    std::snprintf(buf, sizeof(buf), "%.*f", decimals, (double)value);
    // Collapse "-0.00" to "0.00": a signed zero is a distraction, not a fact
    // about the operation being demonstrated.
    std::string text = buf;
    if (text.find_first_not_of("-0.") == std::string::npos && text.front() == '-') {
        text.erase(text.begin());
    }
    return text;
}

inline std::string V2(const Vector2& v, int decimals = 2) {
    return "(" + F(v.X, decimals) + ", " + F(v.Y, decimals) + ")";
}

inline std::string V3(const Vector3& v, int decimals = 2) {
    return "(" + F(v.X, decimals) + ", " + F(v.Y, decimals) + ", " + F(v.Z, decimals) + ")";
}

inline std::string V4(const Vector4& v, int decimals = 2) {
    return "(" + F(v.X, decimals) + ", " + F(v.Y, decimals) + ", " +
           F(v.Z, decimals) + ", " + F(v.W, decimals) + ")";
}

// Maps math-space (y up, origin centred) onto screen-space (y down) for the
// 2D diagrams. Every vector demo that draws needs exactly this, and getting the
// Y flip wrong is the classic way to produce a diagram that silently disagrees
// with the numbers printed beside it.
struct PlotSpace {
    Vector2 origin;        // screen position of math-space (0,0)
    float unitsToPixels;   // scale

    [[nodiscard]] Vector2 ToScreen(const Vector2& mathPoint) const {
        return Vector2(origin.X + mathPoint.X * unitsToPixels,
                       origin.Y - mathPoint.Y * unitsToPixels);
    }

    [[nodiscard]] Vector2 ToMath(const Vector2& screenPoint) const {
        return Vector2((screenPoint.X - origin.X) / unitsToPixels,
                       (origin.Y - screenPoint.Y) / unitsToPixels);
    }
};

} // namespace CnaExamples::Demos::Math
