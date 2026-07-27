// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/MathHelper.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::VectorsDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Vector2;

// MathHelper's scalar utilities. (GetMachineEpsilonFloat exists but is private
// in CNA, so WithinEpsilon below is the public way to ask the same question.)
//
// Two are worth more than the rest:
//
//   WrapAngle folds any angle into (-Pi, Pi]. Without it, an angle accumulated
//   every frame grows without bound and eventually loses precision outright.
//   The running angle below is deliberately NOT wrapped, so the raw value and
//   the wrapped one can be compared as it climbs.
//
//   WithinEpsilon is the float-comparison helper. The famous "0.1 + 0.2 != 0.3"
//   example is a DOUBLE-precision result and does not reproduce in float: in
//   single precision 0.1f + 0.2f == 0.3f exactly, because the rounding happens
//   to land on the same representable value. Both cases are computed live below
//   so the difference is demonstrated instead of repeated from folklore.
class MathHelperScreen : public DemoScreen {
public:
    MathHelperScreen() : DemoScreen("Vectors: MathHelper") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        // Left unwrapped on purpose -- the point is that it keeps growing.
        rawAngle_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 2.0f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        // Two cases, because the well-known one does not behave as advertised in
        // single precision. Both are computed, not asserted.
        const float famousSum = 0.1f + 0.2f;      // == 0.3f in float
        const float realSum = 0.3f + 0.6f;        // != 0.9f in float

        std::vector<std::string> lines;
        lines.push_back("Constants:");
        lines.push_back("  Pi " + F(MathHelper::Pi, 5) + "   TwoPi " + F(MathHelper::TwoPi, 5) +
                        "   PiOver2 " + F(MathHelper::PiOver2, 5));
        lines.push_back("  E " + F(MathHelper::E, 5) +
                        "   Log10E " + F(MathHelper::Log10E, 5) +
                        "   Log2E " + F(MathHelper::Log2E, 5));
        lines.emplace_back();
        lines.push_back("Angles:");
        lines.push_back("  running angle (never wrapped): " + F(rawAngle_, 3) +
                        "   -- grows without bound");
        lines.push_back("  WrapAngle(...)               : " + F(MathHelper::WrapAngle(rawAngle_), 3) +
                        "   -- folded into (-Pi, Pi]");
        lines.push_back("  ToDegrees(Pi) = " + F(MathHelper::ToDegrees(MathHelper::Pi), 1) +
                        "     ToRadians(180) = " + F(MathHelper::ToRadians(180.0f), 5));
        lines.emplace_back();
        lines.push_back("Clamping and comparison:");
        lines.push_back("  Clamp(7.5, 0, 5) = " + F(MathHelper::Clamp(7.5f, 0.0f, 5.0f)) +
                        "     Clamp(-3, 0, 5) = " + F(MathHelper::Clamp(-3.0f, 0.0f, 5.0f)));
        lines.push_back("  Min(3, 7) = " + F(MathHelper::Min(3.0f, 7.0f)) +
                        "        Max(3, 7) = " + F(MathHelper::Max(3.0f, 7.0f)));
        lines.emplace_back();
        lines.push_back("Float equality, computed live:");
        lines.push_back("  0.1f + 0.2f = " + F(famousSum, 10) + "   == 0.3f ? " +
                        std::string(famousSum == 0.3f ? "TRUE" : "false"));
        lines.push_back("    The famous \"0.1 + 0.2 != 0.3\" is a DOUBLE-precision result. In");
        lines.push_back("    float the rounding lands on the same value, so it does NOT reproduce.");
        lines.push_back("  0.3f + 0.6f = " + F(realSum, 10) + "   == 0.9f ? " +
                        std::string(realSum == 0.9f ? "TRUE" : "false") +
                        "   <- this one really differs");
        lines.push_back("    difference = " + F(std::fabs(realSum - 0.9f), 10) + "  (one ULP here)");
        lines.push_back("  WithinEpsilon(0.3f + 0.6f, 0.9f): " +
                        std::string(MathHelper::WithinEpsilon(realSum, 0.9f) ? "true" : "false") +
                        "  <- and it does NOT rescue it");
        lines.emplace_back();
        lines.push_back("WithinEpsilon is |a-b| < MachineEpsilonFloat: a FIXED absolute tolerance");
        lines.push_back("of about 5.96e-8. Here the gap is exactly that, and the test is a strict");
        lines.push_back("<, so it fails by a hair. An absolute epsilon is sized for values near");
        lines.push_back("1.0 -- comparing much larger or smaller magnitudes needs a relative one.");
        lines.emplace_back();
        lines.push_back("  ClosestMSAAPower(5) = " + std::to_string(MathHelper::ClosestMSAAPower(5)) +
                        "   (NOXNA: rounds a requested sample count to a legal one)");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    float rawAngle_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::VectorsDemos
