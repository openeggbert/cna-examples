// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/PackedVector/Alpha8.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Bgr565.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Bgra4444.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Bgra5551.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Byte4.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/HalfSingle.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/HalfVector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/HalfVector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/NormalizedByte2.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/NormalizedByte4.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/NormalizedShort2.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/NormalizedShort4.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Rg32.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Rgba1010102.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Rgba64.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Short2.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Short4.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::ColorDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector4;

// The PackedVector family: 17 compact vertex/texture formats, all of which
// implement the same contract -- PackFromVector4 in, ToVector4 out, with a raw
// PackedValue you can hand to the GPU.
//
// The interesting part is what each one LOSES. A round trip through every type
// with the same input makes that concrete: the error column is the whole story,
// and it is why picking a vertex format is a real decision rather than a
// preference.
//
// Two groups behave very differently and are labelled as such:
//   - Unsigned formats (Bgr565, Rgba64, Byte4, ...) store 0..1 and clamp.
//   - "Normalized" formats store -1..1, which is what surface normals and
//     tangents need; feeding them 0..1 data wastes half the range.
class PackedVectorGalleryScreen : public DemoScreen {
public:
    PackedVectorGalleryScreen() : DemoScreen("Color: PackedVector Gallery") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            page_ = (page_ + 1) % 2;
        }
        if (input.IsMenuUp(ControllingPlayer())) {
            inputIndex_ = (inputIndex_ + 1) % kInputCount;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Vector4 source = kInputs[inputIndex_];

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: next page     Up: change the input vector");
        lines.emplace_back();
        lines.push_back("Input = " + V4(source, 3) + "   (" + kInputNames[inputIndex_] + ")");
        lines.push_back("Each type packs it, then unpacks it again. 'err' is the largest");
        lines.push_back("per-component difference introduced by that round trip.");
        lines.emplace_back();

        std::vector<Row> rows = page_ == 0 ? UnsignedRows(source) : NormalizedRows(source);
        lines.push_back(page_ == 0 ? "Page 1/2 -- unsigned formats (store 0..1)"
                                   : "Page 2/2 -- normalized (-1..1) and float formats");
        lines.emplace_back();
        for (const auto& row : rows) {
            lines.push_back("  " + Pad(row.name, 17) + Pad(row.bits, 8) +
                            "err " + F(row.error, 4) + "   " + row.unpacked);
        }

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    struct Row {
        std::string name;
        std::string bits;
        float error;
        std::string unpacked;
    };

    static constexpr int kInputCount = 3;
    static const Vector4 kInputs[kInputCount];
    static constexpr const char* kInputNames[kInputCount] = {
        "mid-range values", "extremes 0 and 1", "negative components",
    };

    static std::string Pad(const std::string& text, std::size_t width) {
        return text.size() >= width ? text + " " : text + std::string(width - text.size(), ' ');
    }

    // Largest absolute per-component error, which is what decides whether a
    // format is usable for a given job.
    static float MaxError(const Vector4& a, const Vector4& b) {
        return std::max(std::max(std::fabs(a.X - b.X), std::fabs(a.Y - b.Y)),
                        std::max(std::fabs(a.Z - b.Z), std::fabs(a.W - b.W)));
    }

    template <typename T>
    static Row RoundTrip(const char* name, const char* bits, const Vector4& source) {
        T packed;
        packed.PackFromVector4(source);
        const Vector4 back = packed.ToVector4();
        return Row{name, bits, MaxError(source, back), V4(back, 3)};
    }

    static std::vector<Row> UnsignedRows(const Vector4& source) {
        using namespace Microsoft::Xna::Framework::Graphics::PackedVector;
        return {
            RoundTrip<Alpha8>("Alpha8", "8", source),
            RoundTrip<Bgr565>("Bgr565", "16", source),
            RoundTrip<Bgra4444>("Bgra4444", "16", source),
            RoundTrip<Bgra5551>("Bgra5551", "16", source),
            RoundTrip<Byte4>("Byte4", "32", source),
            RoundTrip<Rg32>("Rg32", "32", source),
            RoundTrip<Rgba1010102>("Rgba1010102", "32", source),
            RoundTrip<Rgba64>("Rgba64", "64", source),
        };
    }

    static std::vector<Row> NormalizedRows(const Vector4& source) {
        using namespace Microsoft::Xna::Framework::Graphics::PackedVector;
        return {
            RoundTrip<NormalizedByte2>("NormalizedByte2", "16", source),
            RoundTrip<NormalizedByte4>("NormalizedByte4", "32", source),
            RoundTrip<NormalizedShort2>("NormalizedShort2", "32", source),
            RoundTrip<NormalizedShort4>("NormalizedShort4", "64", source),
            RoundTrip<Short2>("Short2", "32", source),
            RoundTrip<Short4>("Short4", "64", source),
            RoundTrip<HalfSingle>("HalfSingle", "16", source),
            RoundTrip<HalfVector2>("HalfVector2", "32", source),
            RoundTrip<HalfVector4>("HalfVector4", "64", source),
        };
    }

    int page_ = 0;
    int inputIndex_ = 0;
};

inline const Vector4 PackedVectorGalleryScreen::kInputs[PackedVectorGalleryScreen::kInputCount] = {
    Vector4(0.75f, 0.35f, 0.60f, 0.90f),
    Vector4(1.0f, 0.0f, 1.0f, 0.0f),
    Vector4(-0.8f, 0.4f, -0.2f, 1.0f),
};

} // namespace CnaExamples::Demos::Math::ColorDemos
