// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates a real SetData()/GetData() round trip: an 8x8 checkerboard
// of Color values is uploaded via SetData(), then read back via GetData()
// into a fresh buffer and compared pixel-by-pixel against what was written,
// proving the GPU upload/readback path is faithful, not just "doesn't
// crash."
class SetDataGetDataRoundTripScreen : public DemoScreen {
public:
    static constexpr int kSize = 8;

    SetDataGetDataRoundTripScreen() : DemoScreen("Texture2D: SetData/GetData Round-Trip") {}

    void LoadContent() override {
        texture_.emplace(GetScreenManager()->getGraphicsDeviceProperty(), kSize, kSize);
        written_.resize((std::size_t)kSize * (std::size_t)kSize, Color::Black);
        for (int y = 0; y < kSize; ++y) {
            for (int x = 0; x < kSize; ++x) {
                const bool isA = (x + y) % 2 == 0;
                written_[(std::size_t)y * kSize + x] = isA ? Color(255, 200, 60, 255) : Color(40, 40, 80, 255);
            }
        }
        texture_->SetData(written_.data(), (int)written_.size());
    }

    void UnloadContent() override {
        texture_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            attempts_++;
            std::vector<Color> readBack((std::size_t)kSize * (std::size_t)kSize, Color::Black);
            texture_->GetData(readBack.data(), (int)readBack.size());

            mismatchIndex_ = -1;
            for (std::size_t i = 0; i < written_.size(); ++i) {
                if (!(written_[i] == readBack[i])) {
                    mismatchIndex_ = (int)i;
                    break;
                }
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: SetData() a checkerboard, GetData() it back, compare.");
        lines.push_back("Attempts: " + std::to_string(attempts_));
        if (attempts_ > 0) {
            lines.push_back(mismatchIndex_ < 0
                                 ? "Round-trip: MATCH (all " + std::to_string(written_.size()) + " pixels)"
                                 : "Round-trip: MISMATCH at pixel " + std::to_string(mismatchIndex_));
        }
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.Draw(*texture_, Vector2(40.0f, end.Y + 20.0f), mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<Texture2D> texture_;
    std::vector<Color> written_;
    int attempts_ = 0;
    int mismatchIndex_ = -1;
};

} // namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos
