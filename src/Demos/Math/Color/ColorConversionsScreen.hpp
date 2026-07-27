// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::ColorDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;

// XNA's Color is premultiplied-alpha by convention, and that single fact
// explains most of the surprises people hit with it:
//
//   - Color(Vector4) and the byte constructor do NOT premultiply. They store
//     exactly what you give them.
//   - FromNonPremultiplied DOES premultiply -- it is the conversion you need
//     when your source data is straight alpha (which almost all authored art
//     and most colour pickers are).
//
// Both are computed on the same input below, so the difference is a pair of
// numbers rather than a warning. At alpha 1.0 they agree exactly, which is
// precisely why this bug hides until something becomes transparent.
class ColorConversionsScreen : public DemoScreen {
public:
    ColorConversionsScreen() : DemoScreen("Color: Conversions") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        alpha_ -= (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.25f;
        if (alpha_ < 0.0f) alpha_ = 1.0f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Vector4 source(1.0f, 0.5f, 0.25f, alpha_);

        const Color direct(source);
        const Color nonPremultiplied = Color::FromNonPremultiplied(source);
        const Color fromBytes(255, 128, 64, (int)(alpha_ * 255.0f));

        std::vector<std::string> lines;
        lines.push_back("Source Vector4 = " + V4(source) + "   (alpha animates 1 -> 0)");
        lines.emplace_back();
        lines.push_back("Color(Vector4)              = " + C(direct));
        lines.push_back("Color::FromNonPremultiplied = " + C(nonPremultiplied));
        lines.push_back("Color(255,128,64,a) bytes   = " + C(fromBytes));
        lines.emplace_back();
        lines.push_back(alpha_ > 0.99f
            ? "At alpha 1.0 the first two agree exactly -- which is why mixing them up"
            : "As alpha drops, FromNonPremultiplied scales RGB by alpha and the other does not.");
        if (alpha_ > 0.99f) {
            lines.push_back("stays invisible until something in the scene becomes transparent.");
        }
        lines.emplace_back();
        lines.push_back("Back to floats:");
        lines.push_back("  direct.ToVector4()  = " + V4(direct.ToVector4(), 3));
        lines.push_back("  direct.ToVector3()  = " + V3(direct.ToVector3(), 3) + "   (alpha dropped)");
        lines.push_back("  PackedValue         = 0x" + Hex(direct.getPackedValueProperty()));
        lines.emplace_back();
        lines.push_back("Named colours are plain constants, not a palette lookup:");
        lines.push_back("  Color::CornflowerBlue = " + C(Color::CornflowerBlue) +
                        "   (XNA's famous default clear colour)");
        lines.push_back("  Color::Transparent    = " + C(Color::Transparent) +
                        "   (all zero -- premultiplied, so RGB is zero too)");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawSwatches(sb, font, direct, nonPremultiplied);
    }

private:
    static std::string C(const Color& c) {
        return "(" + std::to_string((int)c.getRProperty()) + ", " +
               std::to_string((int)c.getGProperty()) + ", " +
               std::to_string((int)c.getBProperty()) + ", " +
               std::to_string((int)c.getAProperty()) + ")";
    }

    static std::string Hex(unsigned int value) {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%08X", value);
        return buf;
    }

    void DrawSwatches(SpriteBatch& sb, SpriteFont& font, const Color& direct,
                      const Color& nonPremultiplied) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float screenAlpha = TransitionAlpha();
        const int left = viewport.getWidthProperty() - 250;
        const int top = 110;

        // Drawn over a mid-grey backdrop: a premultiplied swatch on black looks
        // identical to a darkened one, and the difference only shows against a
        // non-black background.
        FillRect(sb, Rectangle(left - 10, top - 10, 220, 190), mul(Color(90, 90, 95), screenAlpha));

        sb.DrawString(font, "Color(Vector4)", Vector2((float)left, (float)top),
                      mul(Color::White, screenAlpha));
        FillRect(sb, Rectangle(left, top + 34, 200, 46), mul(direct, screenAlpha));

        sb.DrawString(font, "FromNonPremult.", Vector2((float)left, (float)(top + 92)),
                      mul(Color::White, screenAlpha));
        FillRect(sb, Rectangle(left, top + 126, 200, 46), mul(nonPremultiplied, screenAlpha));
    }

    float alpha_ = 1.0f;
};

} // namespace CnaExamples::Demos::Math::ColorDemos
