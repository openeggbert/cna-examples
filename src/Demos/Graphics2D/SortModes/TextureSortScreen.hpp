// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::SortModesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates SpriteSortMode::Texture -- sprites are stable-sorted by raw
// Texture2D* pointer value before flushing, a real, literal pointer-address
// sort (not a smart "group by texture" heuristic). Two overlapping quads
// using two DIFFERENT Texture2D objects are submitted red-then-green; under
// Texture sort mode, which one ends up on top depends on which object's
// address happens to be numerically larger -- something this app's own code
// cannot predict or control. Honestly shown as "may or may not match
// submission order" rather than staged to look deterministic.
class TextureSortScreen : public DemoScreen {
public:
    TextureSortScreen() : DemoScreen("SpriteBatch: SpriteSortMode::Texture") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        red_.emplace(CreateCheckerboardTexture(device, 80, 80, 80, Color(255, 60, 60, 255), Color(255, 60, 60, 255)));
        green_.emplace(CreateCheckerboardTexture(device, 80, 80, 80, Color(60, 255, 60, 255), Color(60, 255, 60, 255)));
    }

    void UnloadContent() override {
        red_.reset();
        green_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Submitted red, then green (2 different Texture2D objects, overlapping).");
        lines.push_back("Sorted by raw Texture2D* pointer -- top one depends on object addresses,");
        lines.push_back("not submission order. Not predictable from this app's own code.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        sb.End();
        sb.Begin(SpriteSortMode::Texture, BlendState::AlphaBlend, nullptr, nullptr, nullptr);
        const Color tint = mul(Color::White, TransitionAlpha());
        sb.Draw(*red_, Vector2(80.0f, end.Y + 20.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.0f);
        sb.Draw(*green_, Vector2(120.0f, end.Y + 50.0f), std::nullopt, tint, 0.0f, Vector2::Zero, 1.0f,
                SpriteEffects::None, 0.0f);
        sb.End();
        sb.Begin();
    }

private:
    std::optional<Texture2D> red_;
    std::optional<Texture2D> green_;
};

} // namespace CnaExamples::Demos::Graphics2D::SortModesDemos
