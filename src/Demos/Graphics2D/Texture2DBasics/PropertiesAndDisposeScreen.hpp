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

// Demonstrates Texture2D's inspectable properties (Width/Height/Bounds) and
// its disposal lifecycle -- IsDisposed (inherited from GraphicsResource) and
// the NOXNA HasBackend() live/disposed check.
class PropertiesAndDisposeScreen : public DemoScreen {
public:
    PropertiesAndDisposeScreen() : DemoScreen("Texture2D: Properties & Dispose") {}

    void LoadContent() override {
        texture_.emplace(CreateCheckerboardTexture(GetScreenManager()->getGraphicsDeviceProperty(),
                                                     48, 32, 8, Color(255, 200, 60, 255), Color(60, 90, 255, 255)));
    }

    void UnloadContent() override {
        texture_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsNewKeyPress(Keys::X, ControllingPlayer(), playerIndex)) {
            if (!texture_->getIsDisposedProperty()) {
                texture_->Dispose();
                disposeAttempts_++;
            }
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("X: Dispose() this texture.");
        lines.emplace_back();
        lines.push_back("Width: " + std::to_string(texture_->getWidthProperty()) +
                         "  Height: " + std::to_string(texture_->getHeightProperty()));
        const Rectangle bounds = texture_->getBoundsProperty();
        lines.push_back("Bounds: (" + std::to_string(bounds.X) + "," + std::to_string(bounds.Y) + "," +
                         std::to_string(bounds.Width) + "," + std::to_string(bounds.Height) + ")");
        lines.push_back("IsDisposed: " + std::string(texture_->getIsDisposedProperty() ? "true" : "false"));
        lines.push_back("HasBackend(): " + std::string(texture_->HasBackend() ? "true" : "false"));
        lines.push_back("Dispose() calls: " + std::to_string(disposeAttempts_));
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        if (!texture_->getIsDisposedProperty()) {
            sb.Draw(*texture_, Vector2(40.0f, end.Y + 20.0f), mul(Color::White, TransitionAlpha()));
        } else {
            sb.DrawString(font, "(not drawn -- disposed)", Vector2(40.0f, end.Y + 20.0f),
                          mul(Color(180, 100, 100, 255), TransitionAlpha()));
        }
    }

private:
    std::optional<Texture2D> texture_;
    int disposeAttempts_ = 0;
};

} // namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos
