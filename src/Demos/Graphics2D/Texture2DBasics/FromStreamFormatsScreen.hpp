// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <vector>

#include "System/IO/FileStream.hpp"
#include "System/IO/MemoryStream.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Texture2D::FromStream decodes an encoded image -- PNG, BMP, and whatever else
// the build's image decoder was compiled with -- from any Stream. Two things
// about it are worth a screen.
//
// First, WHICH formats decode is a property of the build, not of XNA, and there
// is no query for it. As with SurfaceFormat, the only honest way to answer is
// to hand it bytes and see. This screen does that with a real PNG off disk and
// with a BMP synthesised in memory here -- BMP's header is simple enough to
// write by hand, which keeps the demo honest without vendoring a second asset
// just to prove a decoder exists.
//
// Second, and more useful: HOW IT FAILS. An image loader is fed untrusted bytes
// more often than almost anything else in a game, so "what happens on garbage"
// is not an edge case, it is the common case. Empty streams, truncated files
// and random noise are all tried below and the outcome reported. What must not
// happen is a silent success returning a garbage texture; every failure path is
// checked for that too.
class FromStreamFormatsScreen : public DemoScreen {
public:
    FromStreamFormatsScreen() : DemoScreen("Texture2D: FromStream Formats & Failures") {}

    void OnDemoLoad() override { Probe(); }

    void OnDemoUnload() override {
        png_.reset();
        bmp_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        lines.push_back("FromStream decodes encoded images from any Stream. Which formats work is");
        lines.push_back("a property of this build's decoder -- there is no query, so this probes.");
        lines.emplace_back();
        for (const std::string& row : rows_) lines.push_back("  " + row);
        lines.emplace_back();
        lines.push_back("How it FAILS matters more than which formats it takes: an image loader is");
        lines.push_back("fed untrusted bytes constantly. Every bad input above must throw, not");
        lines.push_back("quietly return a garbage texture.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        // Whatever decoded, drawn -- proof the bytes became real pixels.
        float x = 620.0f;
        if (png_.has_value()) {
            sb.Draw(*png_, Rectangle((int)x, 96, 120, 90), tint);
            sb.DrawString(font, "PNG", Vector2(x, 190.0f), tint);
            x += 140.0f;
        }
        if (bmp_.has_value()) {
            sb.Draw(*bmp_, Rectangle((int)x, 96, 90, 90), tint);
            sb.DrawString(font, "BMP", Vector2(x, 190.0f), tint);
        }

        DrawVerdict(sb, font, end.Y + 4.0f,
                    mul(badInputsAllThrew_ ? Color(40, 200, 90, 255) : Color(220, 60, 60, 255),
                        TransitionAlpha()),
                    tint,
                    badInputsAllThrew_
                        ? "All " + std::to_string(badInputs_) + " malformed inputs threw; none returned a texture."
                        : "A malformed input did NOT throw -- it returned something.");
    }

private:
    // A 2x2 24-bit BMP, uncompressed: 14-byte file header + 40-byte DIB header
    // + 4 pixels padded to a 4-byte row stride. Written by hand so the demo can
    // prove a second decoder without another file in the repository.
    static std::vector<SharpRuntime::bytecs> MakeTinyBmp() {
        auto u16 = [](std::vector<SharpRuntime::bytecs>& out, std::uint16_t v) {
            out.push_back((SharpRuntime::bytecs)(v & 0xFF));
            out.push_back((SharpRuntime::bytecs)(v >> 8));
        };
        auto u32 = [](std::vector<SharpRuntime::bytecs>& out, std::uint32_t v) {
            for (int i = 0; i < 4; ++i) out.push_back((SharpRuntime::bytecs)((v >> (8 * i)) & 0xFF));
        };

        // Each row is 2 pixels * 3 bytes = 6, padded up to 8.
        const std::uint32_t pixelBytes = 16;
        const std::uint32_t offset = 54;

        std::vector<SharpRuntime::bytecs> bmp;
        bmp.push_back('B');
        bmp.push_back('M');
        u32(bmp, offset + pixelBytes);   // file size
        u16(bmp, 0);
        u16(bmp, 0);
        u32(bmp, offset);                // pixel data offset

        u32(bmp, 40);                    // DIB header size
        u32(bmp, 2);                     // width
        u32(bmp, 2);                     // height
        u16(bmp, 1);                     // planes
        u16(bmp, 24);                    // bits per pixel
        u32(bmp, 0);                     // BI_RGB, no compression
        u32(bmp, pixelBytes);
        u32(bmp, 2835);                  // ~72 DPI
        u32(bmp, 2835);
        u32(bmp, 0);
        u32(bmp, 0);

        // BGR, bottom-up. Two rows of two pixels, each padded to 8 bytes.
        const SharpRuntime::bytecs rows[2][8] = {
            {40, 200, 90, 60, 110, 255, 0, 0},    // green, blue
            {60, 120, 230, 255, 255, 255, 0, 0},  // orange, white
        };
        for (const auto& row : rows) {
            for (SharpRuntime::bytecs b : row) bmp.push_back(b);
        }
        return bmp;
    }

    void Try(const std::string& label, const std::vector<SharpRuntime::bytecs>& bytes,
             std::optional<Texture2D>* into, bool expectFailure) {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        if (expectFailure) ++badInputs_;
        try {
            System::IO::MemoryStream stream(bytes.data(), (SharpRuntime::intcs)bytes.size());
            Texture2D decoded = Texture2D::FromStream(device, stream);
            const std::string size = std::to_string(decoded.getWidthProperty()) + "x" +
                                     std::to_string(decoded.getHeightProperty());
            if (expectFailure) {
                badInputsAllThrew_ = false;
                rows_.push_back(label + "decoded " + size + "  <-- SHOULD HAVE THROWN");
            } else {
                rows_.push_back(label + "decoded " + size);
                if (into != nullptr) into->emplace(std::move(decoded));
            }
        } catch (const std::exception& ex) {
            rows_.push_back(label + (expectFailure ? "threw (correct)" : std::string("threw: ") + Short(ex.what())));
        }
    }

    static std::string Short(const std::string& text) {
        return text.size() <= 34 ? text : text.substr(0, 31) + "...";
    }

    static std::string Label(const char* what) {
        std::string label = what;
        label.resize(22, ' ');
        return label;
    }

    void Probe() {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        // A real PNG off disk, through a FileStream rather than a memory buffer,
        // because that is how a game actually loads one.
        const std::string path = "Content/MediaLibraryDemo/Pictures/Patterns/smptebars.png";
        try {
            System::IO::FileStream stream(path);
            png_.emplace(Texture2D::FromStream(device, stream));
            rows_.push_back(Label("PNG from disk") + "decoded " +
                            std::to_string(png_->getWidthProperty()) + "x" +
                            std::to_string(png_->getHeightProperty()));
        } catch (const std::exception& ex) {
            rows_.push_back(Label("PNG from disk") + "threw: " + Short(ex.what()));
        }

        Try(Label("BMP built in memory"), MakeTinyBmp(), &bmp_, false);

        // The failure paths.
        Try(Label("empty stream"), {}, nullptr, true);
        Try(Label("random noise"), {0x7F, 0x21, 0x03, 0xAA, 0x5C, 0x11, 0x90, 0xFE}, nullptr, true);

        // A truncated PNG: a valid 8-byte signature and nothing after it. This
        // is the nastiest case, because the magic number says "trust me".
        Try(Label("truncated PNG"), {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A}, nullptr, true);
    }

    std::optional<Texture2D> png_;
    std::optional<Texture2D> bmp_;
    std::vector<std::string> rows_;
    int badInputs_ = 0;
    bool badInputsAllThrew_ = true;
};

} // namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos
