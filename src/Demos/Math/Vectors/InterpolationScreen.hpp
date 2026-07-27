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

// XNA's four interpolators, plotted as curves of output against t. Printed
// values alone do not distinguish them -- they all start at 0 and end at 1 --
// so the shape between those endpoints is the entire content of this screen.
//
//   Lerp        straight line: constant rate, hard corners at both ends.
//   SmoothStep  eases in and out; the rate is zero at t=0 and t=1.
//   CatmullRom  a spline through 4 control points; the segment drawn is between
//               the middle two, and the outer two set the incoming/outgoing
//               slope. It can overshoot outside [0,1] -- visible here.
//   Hermite     defined by two points plus an explicit tangent at each.
//
// A dot rides each curve at the current t so the pacing difference (not just
// the shape difference) is visible.
class InterpolationScreen : public DemoScreen {
public:
    InterpolationScreen() : DemoScreen("Vectors: Interpolation") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        t_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.35f;
        if (t_ > 1.0f) t_ -= 1.0f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Output plotted against t. All four run 0 -> 1; the shape between differs.");
        lines.emplace_back();
        lines.push_back("t = " + F(t_, 3));
        lines.push_back("  Lerp(0,1,t)       = " + F(MathHelper::Lerp(0.0f, 1.0f, t_), 3));
        lines.push_back("  SmoothStep(0,1,t) = " + F(MathHelper::SmoothStep(0.0f, 1.0f, t_), 3));
        lines.push_back("  CatmullRom(...)   = " + F(CatmullRomAt(t_), 3));
        lines.push_back("  Hermite(...)      = " + F(HermiteAt(t_), 3));
        lines.emplace_back();
        lines.push_back("Barycentric(a,b,c,u,v) weights three values; with u=v=0 it returns a.");
        lines.push_back("  Barycentric(0,1,2, " + F(t_) + ", 0) = " +
                        F(MathHelper::Barycentric(0.0f, 1.0f, 2.0f, t_, 0.0f), 3));
        lines.emplace_back();
        lines.push_back("CatmullRom can leave [0,1] between its endpoints -- the outer control");
        lines.push_back("points set the slope, and overshoot is a normal spline behaviour, not a bug.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawPlots(sb, font);
    }

private:
    // Fixed control points, chosen so CatmullRom visibly overshoots.
    static float CatmullRomAt(float t) {
        return MathHelper::CatmullRom(-0.6f, 0.0f, 1.0f, 1.4f, t);
    }
    static float HermiteAt(float t) {
        return MathHelper::Hermite(0.0f, 3.0f, 1.0f, 0.0f, t);
    }

    void DrawPlots(SpriteBatch& sb, SpriteFont& font) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();

        const int left = viewport.getWidthProperty() - 330;
        const int top = 96;
        const int size = 220;

        // Frame plus the 0 and 1 guide lines, so overshoot is measurable by eye.
        FillRect(sb, Rectangle(left, top, size, 1), mul(Color(70, 70, 70), alpha));
        FillRect(sb, Rectangle(left, top + size, size, 1), mul(Color(70, 70, 70), alpha));
        FillRect(sb, Rectangle(left, top, 1, size), mul(Color(70, 70, 70), alpha));

        struct Curve { const char* name; float (*fn)(float); Color color; };
        const Curve curves[] = {
            {"Lerp",       [](float t) { return MathHelper::Lerp(0.0f, 1.0f, t); },       Color(230, 190, 70)},
            {"SmoothStep", [](float t) { return MathHelper::SmoothStep(0.0f, 1.0f, t); }, Color(90, 200, 120)},
            {"CatmullRom", CatmullRomAt,                                                  Color(120, 160, 220)},
            {"Hermite",    HermiteAt,                                                     Color(210, 110, 190)},
        };

        int labelY = top + size + 12;
        for (const auto& curve : curves) {
            Vector2 previous;
            bool havePrevious = false;
            for (int step = 0; step <= 60; ++step) {
                const float t = (float)step / 60.0f;
                const float value = curve.fn(t);
                const Vector2 point((float)left + t * (float)size,
                                    (float)(top + size) - value * (float)size);
                if (havePrevious) DrawLine(sb, previous, point, mul(curve.color, alpha), 2.0f);
                previous = point;
                havePrevious = true;
            }

            // The moving dot: pacing, not just shape.
            const float value = curve.fn(t_);
            FillRect(sb, Rectangle(left + (int)(t_ * (float)size) - 3,
                                   top + size - (int)(value * (float)size) - 3, 7, 7),
                     mul(curve.color, alpha));

            sb.DrawString(font, curve.name, Vector2((float)left, (float)labelY),
                          mul(curve.color, alpha));
            labelY += 26;
        }
    }

    float t_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::VectorsDemos
