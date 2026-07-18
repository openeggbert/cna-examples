// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::RenderToTextureBasicsDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::RenderTarget2D;

// Mirrors this codebase's own easygl_rt_roundtrip_test.cpp: switches the
// active render target back and forth across TWO separate RenderTarget2D
// instances (not just one), proving each keeps its own distinct content
// correctly rather than only proving a single one-shot round trip. RT1 is
// filled solid green, RT2 solid blue, both in LoadContent(); both are then
// drawn side by side every frame as ordinary textures.
class MultiTargetRoundTripScreen : public DemoScreen {
public:
    MultiTargetRoundTripScreen() : DemoScreen("RenderTarget2D: Multi-Target Round Trip") {}

    void LoadContent() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        rt1_.emplace(device, 120, 120);
        rt2_.emplace(device, 120, 120);

        device.SetRenderTarget(&*rt1_);
        device.Clear(Color(40, 220, 90, 255));
        device.SetRenderTarget(nullptr);

        device.SetRenderTarget(&*rt2_);
        device.Clear(Color(60, 110, 255, 255));
        device.SetRenderTarget(nullptr);

        // Switch back to RT1 a second time (re-clearing to the same color)
        // to prove the device can revisit a render target it already left,
        // not just visit each one once -- RT1's content survives the round
        // trip through RT2 and the backbuffer in between.
        device.SetRenderTarget(&*rt1_);
        device.Clear(Color(40, 220, 90, 255));
        device.SetRenderTarget(nullptr);
    }

    void UnloadContent() override {
        rt1_.reset();
        rt2_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("2 separate RenderTarget2D instances, switched to and from multiple times.");
        lines.push_back("RT1 (green) and RT2 (blue) each keep their own distinct content.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
        const Color tint = mul(Color::White, TransitionAlpha());

        sb.Draw(*rt1_, Vector2(60.0f, end.Y + 20.0f), tint);
        sb.DrawString(font, "RT1", Vector2(60.0f, end.Y + 145.0f), tint);

        sb.Draw(*rt2_, Vector2(240.0f, end.Y + 20.0f), tint);
        sb.DrawString(font, "RT2", Vector2(240.0f, end.Y + 145.0f), tint);
    }

private:
    std::optional<RenderTarget2D> rt1_;
    std::optional<RenderTarget2D> rt2_;
};

} // namespace CnaExamples::Demos::Graphics2D::RenderToTextureBasicsDemos
