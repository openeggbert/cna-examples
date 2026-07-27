// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Curve.hpp"
#include "Microsoft/Xna/Framework/CurveKey.hpp"
#include "Microsoft/Xna/Framework/CurveKeyCollection.hpp"
#include "Microsoft/Xna/Framework/CurveTangent.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::CurvesDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Curve;
using Microsoft::Xna::Framework::CurveKey;
using Microsoft::Xna::Framework::CurveTangent;
using Microsoft::Xna::Framework::Vector2;

// A Curve is a set of keys, and the shape BETWEEN keys is decided entirely by
// each key's tangents. ComputeTangents() fills them in from a chosen rule:
//
//   Flat    tangents are zero -- the curve levels off at every key, giving an
//           ease-in/ease-out between them.
//   Linear  tangents point straight at the neighbouring keys, so the result is
//           effectively straight segments.
//   Smooth  Catmull-Rom style: tangents follow the surrounding keys, giving a
//           continuous curve that can overshoot past a key's value.
//
// All three curves here have IDENTICAL keys. Every difference on screen comes
// from the tangent rule alone, which is the point -- and it is why a curve that
// "looks wrong" is usually a tangent problem, not a key problem.
class CurveTangentsScreen : public DemoScreen {
public:
    CurveTangentsScreen() : DemoScreen("Curves: Tangents") {}

    void OnDemoLoad() override {
        BuildCurves();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        position_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.5f;
        if (position_ > 4.0f) position_ -= 4.0f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Three curves with IDENTICAL keys. Only ComputeTangents() differs.");
        lines.emplace_back();
        lines.push_back("Keys: (0, 0)  (1, 2)  (2, 1)  (3, 3)  (4, 0)");
        lines.emplace_back();
        lines.push_back("position = " + F(position_, 2));
        lines.push_back("  Flat   -> " + F(flat_.Evaluate(position_), 3));
        lines.push_back("  Linear -> " + F(linear_.Evaluate(position_), 3));
        lines.push_back("  Smooth -> " + F(smooth_.Evaluate(position_), 3));
        lines.emplace_back();
        lines.push_back("Tangents on key 1 (position 1, value 2):");
        lines.push_back("  Flat   in " + F(flat_.getKeysProperty()[1].getTangentInProperty(), 3) +
                        "  out " + F(flat_.getKeysProperty()[1].getTangentOutProperty(), 3));
        lines.push_back("  Linear in " + F(linear_.getKeysProperty()[1].getTangentInProperty(), 3) +
                        "  out " + F(linear_.getKeysProperty()[1].getTangentOutProperty(), 3));
        lines.push_back("  Smooth in " + F(smooth_.getKeysProperty()[1].getTangentInProperty(), 3) +
                        "  out " + F(smooth_.getKeysProperty()[1].getTangentOutProperty(), 3));
        lines.emplace_back();
        lines.push_back("IsConstant: " + std::string(smooth_.getIsConstantProperty() ? "true" : "false") +
                        "   (true only when a curve has fewer than two keys)");
        lines.emplace_back();
        lines.push_back("Smooth can overshoot past a key's value between keys -- normal spline");
        lines.push_back("behaviour. Flat never does, which is why it suits UI easing.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawPlot(sb, font);
    }

private:
    void BuildCurves() {
        const float keys[5][2] = {{0.0f, 0.0f}, {1.0f, 2.0f}, {2.0f, 1.0f}, {3.0f, 3.0f}, {4.0f, 0.0f}};
        for (Curve* curve : {&flat_, &linear_, &smooth_}) {
            for (const auto& key : keys) {
                curve->getKeysProperty().Add(CurveKey(key[0], key[1]));
            }
        }
        flat_.ComputeTangents(CurveTangent::Flat);
        linear_.ComputeTangents(CurveTangent::Linear);
        smooth_.ComputeTangents(CurveTangent::Smooth);
    }

    void DrawPlot(SpriteBatch& sb, SpriteFont& font) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();

        const int left = viewport.getWidthProperty() - 330;
        const int top = 100;
        const int width = 280;
        const int height = 200;
        // Values run roughly -0.5..3.5; map that band onto the plot.
        const float valueMin = -0.6f;
        const float valueMax = 3.6f;

        FillRect(sb, Rectangle(left, top, width, 1), mul(Color(60, 60, 60), alpha));
        FillRect(sb, Rectangle(left, top + height, width, 1), mul(Color(60, 60, 60), alpha));

        struct Entry { const char* name; const Curve* curve; Color color; };
        const Entry entries[] = {
            {"Flat",   &flat_,   Color(230, 190, 70)},
            {"Linear", &linear_, Color(90, 200, 120)},
            {"Smooth", &smooth_, Color(120, 160, 220)},
        };

        int labelY = top + height + 14;
        for (const auto& entry : entries) {
            Vector2 previous;
            bool havePrevious = false;
            for (int step = 0; step <= 80; ++step) {
                const float p = (float)step / 80.0f * 4.0f;
                const float value = entry.curve->Evaluate(p);
                const float normalized = (value - valueMin) / (valueMax - valueMin);
                const Vector2 point((float)left + p / 4.0f * (float)width,
                                    (float)(top + height) - normalized * (float)height);
                if (havePrevious) DrawLine(sb, previous, point, mul(entry.color, alpha), 2.0f);
                previous = point;
                havePrevious = true;
            }
            sb.DrawString(font, entry.name, Vector2((float)left, (float)labelY),
                          mul(entry.color, alpha));
            labelY += 26;
        }

        // The playhead, so the printed values line up with the plot.
        const int playX = left + (int)(position_ / 4.0f * (float)width);
        FillRect(sb, Rectangle(playX, top, 1, height), mul(Color(150, 150, 150), alpha));
    }

    Curve flat_;
    Curve linear_;
    Curve smooth_;
    float position_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::CurvesDemos
