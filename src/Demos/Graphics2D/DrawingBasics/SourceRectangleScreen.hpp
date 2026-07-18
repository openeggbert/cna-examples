// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::DrawingBasicsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates the source-rectangle Draw overloads (cropping a single cell
// out of a larger texture) -- the classic sprite-sheet pattern. The atlas
// here is a procedurally generated 4x4 grid of distinctly colored cells;
// N/P step through which cell is cropped and drawn, both at native size and
// blown up via a destination rectangle.
class SourceRectangleScreen : public DemoScreen {
public:
    static constexpr int kCols = 4;
    static constexpr int kRows = 4;
    static constexpr int kCellSize = 24;

    SourceRectangleScreen() : DemoScreen("SpriteBatch: Source Rectangle") {}

    void LoadContent() override {
        std::vector<Color> palette = {
            Color(255, 80, 80, 255),  Color(80, 255, 80, 255),  Color(80, 80, 255, 255),  Color(255, 255, 80, 255),
            Color(255, 80, 255, 255), Color(80, 255, 255, 255), Color(255, 160, 60, 255), Color(160, 60, 255, 255),
            Color(60, 160, 255, 255), Color(255, 60, 160, 255), Color(160, 255, 60, 255), Color(60, 255, 160, 255),
            Color(200, 200, 200, 255), Color(120, 120, 120, 255), Color(255, 255, 255, 255), Color(0, 0, 0, 255),
        };
        atlas_.emplace(CreateGridTexture(GetScreenManager()->getGraphicsDeviceProperty(), kCols, kRows, kCellSize, palette));
    }

    void UnloadContent() override {
        atlas_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsNewKeyPress(Keys::N, ControllingPlayer(), playerIndex)) {
            cellIndex_ = (cellIndex_ + 1) % (kCols * kRows);
        } else if (input.IsNewKeyPress(Keys::P, ControllingPlayer(), playerIndex)) {
            cellIndex_ = (cellIndex_ + kCols * kRows - 1) % (kCols * kRows);
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("N/P: step through the 4x4 atlas -- Draw(texture, position, sourceRect, color)");
        lines.push_back("Cell " + std::to_string(cellIndex_) + " of " + std::to_string(kCols * kRows));
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        const int col = cellIndex_ % kCols;
        const int row = cellIndex_ / kCols;
        const Rectangle src(col * kCellSize, row * kCellSize, kCellSize, kCellSize);
        const Color tint = mul(Color::White, TransitionAlpha());

        sb.Draw(*atlas_, Vector2(40.0f, end.Y + 20.0f), src, tint);
        sb.DrawString(font, "native size", Vector2(40.0f, end.Y + 50.0f), tint);

        sb.Draw(*atlas_, Rectangle(160, (int)end.Y + 20, 96, 96), std::optional<Rectangle>(src), tint);
        sb.DrawString(font, "blown up via dest rect", Vector2(160.0f, end.Y + 125.0f), tint);

        const float atlasY = end.Y + 170.0f;
        sb.Draw(*atlas_, Vector2(40.0f, atlasY), std::nullopt, tint);
        sb.DrawString(font, "whole atlas (no source rect)", Vector2(40.0f, atlasY + kCols * kCellSize + 6.0f), tint);
    }

private:
    std::optional<Texture2D> atlas_;
    int cellIndex_ = 0;
};

} // namespace CnaExamples::Demos::Graphics2D::DrawingBasicsDemos
