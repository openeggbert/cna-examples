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

// Demonstrates Draw(texture, Rectangle destinationRectangle, Color) --
// unlike the position overloads, the destination rectangle stretches (or
// shrinks) the whole texture to exactly fill it, ignoring the texture's own
// pixel dimensions.
class DestinationRectangleScreen : public DemoScreen {
public:
    DestinationRectangleScreen() : DemoScreen("SpriteBatch: Destination Rectangle") {}

    void LoadContent() override {
        icon_.emplace(CreateCheckerboardTexture(
            GetScreenManager()->getGraphicsDeviceProperty(), 16, 16, 4,
            Color(255, 200, 60, 255), Color(60, 90, 255, 255)));
    }

    void UnloadContent() override {
        icon_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Draw(texture, Rectangle destinationRectangle, Color)");
        lines.push_back("The same 16x16 checkerboard, stretched to 3 different rectangle sizes:");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        const Color tint = mul(Color::White, TransitionAlpha());
        sb.Draw(*icon_, Rectangle(40, (int)end.Y + 20, 40, 40), tint);
        sb.Draw(*icon_, Rectangle(120, (int)end.Y + 20, 120, 80), tint);
        sb.Draw(*icon_, Rectangle(280, (int)end.Y + 20, 60, 160), tint);
    }

private:
    std::optional<Texture2D> icon_;
};

} // namespace CnaExamples::Demos::Graphics2D::DrawingBasicsDemos
