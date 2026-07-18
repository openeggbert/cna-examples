// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Graphics2D/TextureDemoHelpers.hpp"

namespace CnaExamples::Demos::Graphics2D::ViewportScissorDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::RasterizerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates real scissor clipping. A real, confirmed limitation from
// this app's own research: SpriteBatch::Begin()'s rasterizerState
// parameter is accepted but silently NOT forwarded to the backend, so
// enabling scissor testing through Begin() has no effect on sprite draws.
// This screen instead sets RasterizerState.ScissorTestEnable=true directly
// on GraphicsDevice.RasterizerState (which SpriteBatch::Begin() does NOT
// overwrite) before drawing -- a 200x200 texture is drawn at a fixed
// position, but GraphicsDevice.ScissorRectangle restricts visible output to
// only its left half. Device state is restored to defaults on exit.
class ScissorClippingScreen : public DemoScreen {
public:
    ScissorClippingScreen() : DemoScreen("Viewport: Scissor Clipping") {}

    void LoadContent() override {
        texture_.emplace(CreateGradientTexture(GetScreenManager()->getGraphicsDeviceProperty(), 200, 200,
                                                 Color(255, 80, 80, 255), Color(80, 80, 255, 255)));
    }

    void UnloadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        device.setScissorRectangleProperty(device.getViewportProperty().getBoundsProperty());
        device.setRasterizerStateProperty(RasterizerState());
        texture_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("GraphicsDevice.ScissorRectangle clips drawing to a sub-rect -- only the");
        lines.push_back("left half of this 200x200 gradient is actually visible below.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        // sb.End() MUST happen before the scissor rectangle changes: SpriteBatch
        // defers actual rasterization to End() (SpriteSortMode::Deferred), so the
        // title/description text queued by DrawLines() above is only physically
        // drawn at this End() call -- changing ScissorRectangle first would clip
        // that already-queued text too, not just draws issued after the change.
        sb.End();

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        RasterizerState scissorOn;
        scissorOn.setScissorTestEnableProperty(true);
        device.setRasterizerStateProperty(scissorOn);
        device.setScissorRectangleProperty(Rectangle(40, (int)end.Y + 20, 100, 200));

        sb.Begin();
        sb.Draw(*texture_, Vector2(40.0f, end.Y + 20.0f), mul(Color::White, TransitionAlpha()));
        sb.End();

        device.setRasterizerStateProperty(RasterizerState());
        device.setScissorRectangleProperty(device.getViewportProperty().getBoundsProperty());
        sb.Begin();
    }

private:
    std::optional<Texture2D> texture_;
};

} // namespace CnaExamples::Demos::Graphics2D::ViewportScissorDemos
