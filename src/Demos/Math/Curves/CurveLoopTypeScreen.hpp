// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Curve.hpp"
#include "Microsoft/Xna/Framework/CurveKey.hpp"
#include "Microsoft/Xna/Framework/CurveKeyCollection.hpp"
#include "Microsoft/Xna/Framework/CurveLoopType.hpp"
#include "Microsoft/Xna/Framework/CurveTangent.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::CurvesDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Curve;
using Microsoft::Xna::Framework::CurveKey;
using Microsoft::Xna::Framework::CurveLoopType;
using Microsoft::Xna::Framework::CurveTangent;
using Microsoft::Xna::Framework::Vector2;

// PreLoop and PostLoop decide what a Curve returns OUTSIDE the range its keys
// cover -- before the first key and after the last. This matters more than it
// sounds: an animation curve is almost always evaluated past its end at some
// point, and the five modes give five different answers.
//
//   Constant     clamp to the nearest end key's value.
//   Linear       continue along the end key's tangent, straight off to infinity.
//   Cycle        repeat the curve, jumping back to the start value.
//   CycleOffset  repeat, but offset so it continues from where it left off --
//                the one that makes a looping animation not snap.
//   Oscillate    ping-pong back and forth.
//
// The plot deliberately spans well beyond the key range so all of that is
// visible at once, and the sampled value is printed for each mode at the same
// out-of-range position.
class CurveLoopTypeScreen : public DemoScreen {
public:
    CurveLoopTypeScreen() : DemoScreen("Curves: Pre/Post Loop") {}

    void OnDemoLoad() override {
        for (int i = 0; i < kModeCount; ++i) {
            Curve& curve = curves_[i];
            curve.getKeysProperty().Add(CurveKey(0.0f, 0.0f));
            curve.getKeysProperty().Add(CurveKey(1.0f, 2.0f));
            curve.getKeysProperty().Add(CurveKey(2.0f, 1.0f));
            curve.ComputeTangents(CurveTangent::Smooth);
            curve.setPreLoopProperty(kModes[i]);
            curve.setPostLoopProperty(kModes[i]);
        }
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        position_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.7f;
        if (position_ > 6.0f) position_ -= 8.0f;   // sweeps -2 .. 6
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const bool outside = position_ < 0.0f || position_ > 2.0f;

        std::vector<std::string> lines;
        lines.push_back("Keys cover positions 0..2 only. Everything outside that is decided");
        lines.push_back("by PreLoop (before 0) and PostLoop (after 2).");
        lines.emplace_back();
        lines.push_back("position = " + F(position_, 2) +
                        (outside ? "   -- OUTSIDE the key range" : "   -- inside the key range"));
        lines.emplace_back();
        for (int i = 0; i < kModeCount; ++i) {
            lines.push_back("  " + std::string(kModeNames[i]) + std::string(12 - std::string(kModeNames[i]).size(), ' ') +
                            F(curves_[i].Evaluate(position_), 3));
        }
        lines.emplace_back();
        lines.push_back("Inside the range all five agree exactly -- the mode only ever affects");
        lines.push_back("evaluation outside it.");
        lines.emplace_back();
        lines.push_back("CycleOffset is the one worth knowing: it repeats the shape but carries");
        lines.push_back("the end value forward, so a looping animation continues instead of");
        lines.push_back("snapping back. Cycle restarts from the first key's value every time.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawPlot(sb, font);
    }

private:
    static constexpr int kModeCount = 5;
    static constexpr CurveLoopType kModes[kModeCount] = {
        CurveLoopType::Constant, CurveLoopType::Cycle, CurveLoopType::CycleOffset,
        CurveLoopType::Oscillate, CurveLoopType::Linear,
    };
    static constexpr const char* kModeNames[kModeCount] = {
        "Constant", "Cycle", "CycleOffset", "Oscillate", "Linear",
    };

    void DrawPlot(SpriteBatch& sb, SpriteFont& font) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();

        const int left = viewport.getWidthProperty() - 320;
        const int top = 96;
        const int width = 270;
        const int height = 210;
        const float from = -2.0f;
        const float to = 6.0f;
        const float valueMin = -3.0f;
        const float valueMax = 5.0f;

        auto toScreen = [&](float position, float value) {
            return Vector2((float)left + (position - from) / (to - from) * (float)width,
                           (float)(top + height) -
                               (value - valueMin) / (valueMax - valueMin) * (float)height);
        };

        // Shade the key range so "inside" vs "outside" is obvious.
        const Vector2 rangeStart = toScreen(0.0f, valueMax);
        const Vector2 rangeEnd = toScreen(2.0f, valueMin);
        FillRect(sb, Rectangle((int)rangeStart.X, (int)rangeStart.Y,
                               (int)(rangeEnd.X - rangeStart.X), (int)(rangeEnd.Y - rangeStart.Y)),
                 mul(Color(38, 42, 50), alpha));

        static const Color kColors[kModeCount] = {
            Color(230, 190, 70), Color(90, 200, 120), Color(120, 160, 220),
            Color(210, 110, 190), Color(200, 200, 200),
        };

        for (int i = 0; i < kModeCount; ++i) {
            Vector2 previous;
            bool havePrevious = false;
            for (int step = 0; step <= 120; ++step) {
                const float p = from + (to - from) * (float)step / 120.0f;
                const Vector2 point = toScreen(p, curves_[i].Evaluate(p));
                // Linear runs far off the plot; clipping keeps it from smearing.
                if (point.Y > (float)top - 40.0f && point.Y < (float)(top + height) + 40.0f) {
                    if (havePrevious) DrawLine(sb, previous, point, mul(kColors[i], alpha), 2.0f);
                    previous = point;
                    havePrevious = true;
                } else {
                    havePrevious = false;
                }
            }
            sb.DrawString(font, kModeNames[i],
                          Vector2((float)left, (float)(top + height + 14 + i * 26)),
                          mul(kColors[i], alpha));
        }

        const Vector2 playhead = toScreen(position_, valueMax);
        FillRect(sb, Rectangle((int)playhead.X, top, 1, height), mul(Color(160, 160, 160), alpha));
    }

    Curve curves_[kModeCount];
    float position_ = -2.0f;
};

} // namespace CnaExamples::Demos::Math::CurvesDemos
