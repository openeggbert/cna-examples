// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// XNA declares nineteen SurfaceFormats. A backend does not have to support all
// of them, and the honest question -- "which of these actually work here?" --
// has no API that answers it: there is no SupportsFormat() query. The only way
// to find out is to construct a Texture2D and see whether it throws.
//
// So that is what this screen does, once, for all nineteen, and reports the
// result as a table. Nothing here is asserted from documentation; every row is
// the outcome of a real construction attempt on the backend you are running.
//
// The second column separates two things that are easy to conflate: a format
// can construct successfully and still not store pixels the way its name
// implies, because several backends keep everything as RGBA8 internally. So
// each surviving format also gets a Color written and read back, and the table
// says whether the round trip was exact. A format that constructs but mangles
// data is invisible if you only check for an exception.
//
// Measured on EASYGL when this screen was written: exactly ONE of the nineteen
// -- Color -- constructs at all; the other eighteen throw. That is worth
// knowing before designing around Dxt5 or a float format, and it is the
// opposite of the "everything is silently RGBA8" behaviour the second column
// was built to catch: here the unsupported formats are refused outright rather
// than quietly coerced. Other backends will fill this table differently, which
// is exactly why it is probed live instead of hard-coded.
class SurfaceFormatMatrixScreen : public DemoScreen {
public:
    SurfaceFormatMatrixScreen() : DemoScreen("Texture2D: SurfaceFormat Matrix") {}

    void OnDemoLoad() override { Probe(); }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        lines.push_back("Every SurfaceFormat, constructed for real on this backend. There is no");
        lines.push_back("SupportsFormat() query -- attempting it is the only way to know.");
        lines.emplace_back();

        // Two columns of the table, so all nineteen fit above the Back hint.
        const int half = (kFormatCount + 1) / 2;
        for (int i = 0; i < half; ++i) {
            std::string row = Row(i);
            if (i + half < kFormatCount) {
                row.resize(38, ' ');
                row += Row(i + half);
            }
            lines.push_back(row);
        }
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        DrawVerdict(sb, font, end.Y + 4.0f, mul(Color(40, 200, 90, 255), TransitionAlpha()), tint,
                    std::to_string(created_) + "/" + std::to_string(kFormatCount) +
                        " construct, " + std::to_string(roundTripped_) +
                        " keep Color data through SetData/GetData.");
    }

private:
    static constexpr int kFormatCount = 19;

    static constexpr std::array<SurfaceFormat, kFormatCount> kFormats = {
        SurfaceFormat::Color,       SurfaceFormat::Bgr565,      SurfaceFormat::Bgra5551,
        SurfaceFormat::Bgra4444,    SurfaceFormat::Dxt1,        SurfaceFormat::Dxt3,
        SurfaceFormat::Dxt5,        SurfaceFormat::NormalizedByte2, SurfaceFormat::NormalizedByte4,
        SurfaceFormat::Rgba1010102, SurfaceFormat::Rg32,        SurfaceFormat::Rgba64,
        SurfaceFormat::Alpha8,      SurfaceFormat::Single,      SurfaceFormat::Vector2,
        SurfaceFormat::Vector4,     SurfaceFormat::HalfSingle,  SurfaceFormat::HalfVector2,
        SurfaceFormat::HalfVector4,
    };

    static constexpr std::array<const char*, kFormatCount> kNames = {
        "Color", "Bgr565", "Bgra5551", "Bgra4444", "Dxt1", "Dxt3", "Dxt5",
        "NormByte2", "NormByte4", "Rgba1010102", "Rg32", "Rgba64", "Alpha8",
        "Single", "Vector2", "Vector4", "HalfSingle", "HalfVec2", "HalfVec4",
    };

    std::string Row(int i) const {
        std::string name = kNames[i];
        name.resize(12, ' ');
        return "  " + name + status_[i];
    }

    void Probe() {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const Color probe(200, 120, 60, 255);

        for (int i = 0; i < kFormatCount; ++i) {
            std::unique_ptr<Texture2D> texture;
            try {
                texture = std::make_unique<Texture2D>(device, 4, 4, false, kFormats[i]);
            } catch (const std::exception&) {
                status_[i] = "no";
                continue;
            }
            ++created_;

            // Constructed is not the same as "stores what you gave it". Only the
            // Color-shaped SetData/GetData pair is meaningful for every format,
            // so a throw here is a legitimate answer too, not a failure.
            try {
                std::vector<Color> in(16, probe);
                texture->SetData(in.data(), (int)in.size());
                std::vector<Color> out(16, Color::Black);
                texture->GetData(out.data(), (int)out.size());
                const bool exact = out[0].getRProperty() == probe.getRProperty() &&
                                   out[0].getGProperty() == probe.getGProperty() &&
                                   out[0].getBProperty() == probe.getBProperty();
                if (exact) {
                    ++roundTripped_;
                    status_[i] = "yes, exact";
                } else {
                    status_[i] = "yes, lossy";
                }
            } catch (const std::exception&) {
                status_[i] = "yes, no RGBA";
            }
        }
    }

    std::array<std::string, kFormatCount> status_{};
    int created_ = 0;
    int roundTripped_ = 0;
};

} // namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos
