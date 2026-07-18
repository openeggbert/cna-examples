// SPDX-License-Identifier: MIT
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::SaveAsReloadDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Same real round trip as SaveAsPngRoundTripScreen, but through
// SaveAsJpeg() -- JPEG is a lossy format, so this deliberately does NOT
// claim a pixel-exact match, only that a real, valid JPEG file was written
// and successfully redecoded.
class SaveAsJpegRoundTripScreen : public DemoScreen {
public:
    static constexpr int kSize = 64;
    static constexpr const char* kPath = "/tmp/cna_examples_2d_saveas_test.jpg";

    SaveAsJpegRoundTripScreen() : DemoScreen("Texture2D: SaveAsJpeg Round-Trip") {}

    void LoadContent() override {
        original_.emplace(CreateGradientTexture(GetScreenManager()->getGraphicsDeviceProperty(), kSize, kSize,
                                                  Color(80, 160, 255, 255), Color(255, 220, 80, 255)));
    }

    void UnloadContent() override {
        original_.reset();
        reloaded_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            attempts_++;
            original_->SaveAsJpeg(kPath);
            std::error_code ec;
            savedBytes_ = (long long)std::filesystem::file_size(kPath, ec);
            reloaded_.emplace(std::string(kPath), GetScreenManager()->getGraphicsDeviceProperty());
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: SaveAsJpeg() to a real file, reload it, draw both.");
        lines.push_back("JPEG is lossy -- expect small color differences, not an exact match.");
        lines.push_back("Attempts: " + std::to_string(attempts_));
        if (savedBytes_ >= 0) lines.push_back("Saved file size: " + std::to_string(savedBytes_) + " bytes");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const Color tint = mul(Color::White, TransitionAlpha());

        sb.Draw(*original_, Vector2(40.0f, end.Y + 20.0f), tint);
        sb.DrawString(font, "original", Vector2(40.0f, end.Y + 20.0f + kSize + 6.0f), tint);

        if (reloaded_.has_value()) {
            sb.Draw(*reloaded_, Vector2(200.0f, end.Y + 20.0f), tint);
            sb.DrawString(font, "reloaded from disk", Vector2(200.0f, end.Y + 20.0f + kSize + 6.0f), tint);
        }
    }

private:
    std::optional<Texture2D> original_;
    std::optional<Texture2D> reloaded_;
    int attempts_ = 0;
    long long savedBytes_ = -1;
};

} // namespace CnaExamples::Demos::Graphics2D::SaveAsReloadDemos
