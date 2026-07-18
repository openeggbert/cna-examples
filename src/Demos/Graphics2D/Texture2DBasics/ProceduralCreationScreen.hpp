// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates the two ways to build a Texture2D with no bundled PNG asset:
// the NOXNA Texture2D::CreateFromPixels() one-liner (real XNA has no such
// constructor at all -- this is CNA's raw-pixel-buffer escape hatch) versus
// the XNA-idiomatic Texture2D(device, w, h) + SetData(Color*, count)
// pattern. Both build the exact same gradient; only the construction path
// differs.
class ProceduralCreationScreen : public DemoScreen {
public:
    static constexpr int kSize = 64;

    ProceduralCreationScreen() : DemoScreen("Texture2D: Procedural Creation") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        // (a) NOXNA CreateFromPixels() -- fed a hand-built RGBA byte buffer.
        viaPixels_.emplace(CreateGradientTexture(device, kSize, kSize,
                                                  Color(255, 80, 80, 255), Color(80, 80, 255, 255)));

        // (b) XNA-idiomatic: Texture2D(device, w, h) then SetData(Color*, count).
        viaSetData_.emplace(device, kSize, kSize);
        std::vector<Color> colors((std::size_t)kSize * (std::size_t)kSize, Color::White);
        for (int y = 0; y < kSize; ++y) {
            for (int x = 0; x < kSize; ++x) {
                const float t = (float)x / (float)(kSize - 1);
                colors[(std::size_t)y * kSize + x] = Color(
                    (int)(255 + t * (80 - 255)), 80, (int)(80 + t * (255 - 80)));
            }
        }
        viaSetData_->SetData(colors.data(), (int)colors.size());
    }

    void UnloadContent() override {
        viaPixels_.reset();
        viaSetData_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Both textures below are built procedurally -- no PNG asset is bundled.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const Color tint = mul(Color::White, TransitionAlpha());

        sb.Draw(*viaPixels_, Vector2(40.0f, end.Y + 20.0f), tint);
        sb.DrawString(font, "CreateFromPixels (NOXNA)", Vector2(40.0f, end.Y + 20.0f + kSize + 6.0f), tint);

        const float row2Y = end.Y + 20.0f + kSize + 40.0f;
        sb.Draw(*viaSetData_, Vector2(40.0f, row2Y), tint);
        sb.DrawString(font, "Texture2D(w,h) + SetData (XNA-idiomatic)",
                      Vector2(40.0f, row2Y + kSize + 6.0f), tint);
    }

private:
    std::optional<Texture2D> viaPixels_;
    std::optional<Texture2D> viaSetData_;
};

} // namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos
