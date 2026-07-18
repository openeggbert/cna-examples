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

// Demonstrates a real PNG round trip -- SaveAsPng() is confirmed real and
// round-trip-verified upstream (not a stub): a procedurally generated
// texture is written to a real file, then reloaded via the NOXNA
// file-path constructor (which decodes it back through the same PNG
// decoder any real asset would go through), and both are drawn side by
// side.
class SaveAsPngRoundTripScreen : public DemoScreen {
public:
    static constexpr int kSize = 64;
    static constexpr const char* kPath = "/tmp/cna_examples_2d_saveas_test.png";

    SaveAsPngRoundTripScreen() : DemoScreen("Texture2D: SaveAsPng Round-Trip") {}

    void LoadContent() override {
        original_.emplace(CreateGradientTexture(GetScreenManager()->getGraphicsDeviceProperty(), kSize, kSize,
                                                  Color(255, 80, 80, 255), Color(80, 255, 120, 255)));
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
            original_->SaveAsPng(kPath);
            std::error_code ec;
            savedBytes_ = (long long)std::filesystem::file_size(kPath, ec);
            reloaded_.emplace(std::string(kPath), GetScreenManager()->getGraphicsDeviceProperty());
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: SaveAsPng() to a real file, reload it, draw both.");
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
