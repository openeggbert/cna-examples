// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::BeginEndStateDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates the full 7-argument Begin() overload's transform Matrix
// parameter -- a single combined scale+rotation+translation matrix applied
// to every sprite in the batch, the same mechanism a real XNA 2D camera
// uses. Every Draw() call below uses the SAME fixed local position; only
// the Begin() matrix (animated via GameTime) moves them around the screen.
class TransformMatrixScreen : public DemoScreen {
public:
    TransformMatrixScreen() : DemoScreen("SpriteBatch: Begin() Transform Matrix") {}

    void LoadContent() override {
        marker_.emplace(CreateCheckerboardTexture(GetScreenManager()->getGraphicsDeviceProperty(), 24, 24, 24,
                                                    Color(255, 200, 60, 255), Color(255, 200, 60, 255)));
    }

    void UnloadContent() override {
        marker_.reset();
    }

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        elapsed_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("The 7-arg Begin(..., effect, transformMatrix) overload:");
        lines.push_back("All 3 markers below are drawn at the SAME local (0,0)/(40,0)/(80,0) -- only");
        lines.push_back("the Begin() transform matrix (an orbiting camera) moves them.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        const Matrix camera = Matrix::CreateTranslation(-40.0f, 0.0f, 0.0f) *
                               Matrix::CreateRotationZ(elapsed_) *
                               Matrix::CreateTranslation(240.0f, end.Y + 150.0f, 0.0f);

        sb.End();
        sb.Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend, nullptr, nullptr, nullptr, nullptr, camera);
        const Color tint = mul(Color::White, TransitionAlpha());
        sb.Draw(*marker_, Vector2(0.0f, 0.0f), tint);
        sb.Draw(*marker_, Vector2(40.0f, 0.0f), tint);
        sb.Draw(*marker_, Vector2(80.0f, 0.0f), tint);
        sb.End();
        sb.Begin();
    }

private:
    std::optional<Texture2D> marker_;
    float elapsed_ = 0.0f;
};

} // namespace CnaExamples::Demos::Graphics2D::BeginEndStateDemos
