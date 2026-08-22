// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Graphics/RenderPipelineSettings.hpp"
#include "CNA/Graphics/RenderQuality.hpp"
#include "CNA/Graphics/ShadowQuality.hpp"
#include "CNA/Graphics/TonemappingMode.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics3D::PbrDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Graphics::RenderPipelineSettings;
using CNA::Graphics::RenderQuality;
using CNA::Graphics::ShadowQuality;
using CNA::Graphics::TonemappingMode;

// RenderPipelineSettings was the other half of plan.md's original D2 idea,
// alongside PbrEffect (see PbrMetallicRoughnessScreen.hpp). It requires the
// CNA_CNAEXT CMake option -- OFF by default in CNA itself, same reasoning as
// CNA_DEVICES for the Devices area -- which this project now enables
// alongside CNA_DEVICES in its own top-level CMakeLists.txt.
//
// This screen verifies the value contract directly: defaults and every
// mutated property round-trip exactly. cnanext's RenderPipeline and its
// post-processing passes now consume these settings, but this catalog screen
// deliberately does not create or attach a RenderPipeline to its
// GraphicsDevice, so it demonstrates the configuration object rather than a
// second rendered pipeline.
class RenderPipelineSettingsScreen : public DemoScreen {
public:
    RenderPipelineSettingsScreen() : DemoScreen("RenderPipelineSettings: Extended Configuration") {}

    void OnDemoLoad() override {
        RenderPipelineSettings settings;

        // Defaults, read back exactly as the header documents them.
        defaultsMatch_ =
            !settings.isHDREnabled() &&
            settings.getExposure() == 1.0f &&
            settings.getGamma() == 2.2f &&
            settings.getTonemappingMode() == TonemappingMode::None &&
            !settings.isBloomEnabled() &&
            settings.getBloomIntensity() == 1.0f &&
            !settings.isSSAOEnabled() &&
            settings.getRenderQuality() == RenderQuality::Medium &&
            settings.getShadowQuality() == ShadowQuality::Disabled &&
            !settings.isShadowsEnabled();

        // Set every property to a deliberately non-default value, then read
        // every one back. This proves the configuration object's value
        // contract independently of any particular render pipeline.
        settings.setHDREnabled(true);
        settings.setExposure(2.5f);
        settings.setGamma(1.8f);
        settings.setTonemappingMode(TonemappingMode::Filmic);
        settings.setBloomEnabled(true);
        settings.setBloomIntensity(0.6f);
        settings.setSSAOEnabled(true);
        settings.setRenderQuality(RenderQuality::Ultra);
        settings.setShadowQuality(ShadowQuality::High);
        settings.setShadowsEnabled(true);

        roundTripExact_ =
            settings.isHDREnabled() &&
            settings.getExposure() == 2.5f &&
            settings.getGamma() == 1.8f &&
            settings.getTonemappingMode() == TonemappingMode::Filmic &&
            settings.isBloomEnabled() &&
            settings.getBloomIntensity() == 0.6f &&
            settings.isSSAOEnabled() &&
            settings.getRenderQuality() == RenderQuality::Ultra &&
            settings.getShadowQuality() == ShadowQuality::High &&
            settings.isShadowsEnabled();
    }

    void OnDemoUnload() override {}

protected:
    void OnDemoUpdate(GameTime&) override {}

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;
        lines.push_back("CNA::Graphics::RenderPipelineSettings -- HDR/exposure/gamma, tonemapping,");
        lines.push_back("bloom, SSAO, render & shadow quality. Defaults and every value set above");
        lines.push_back("round-trip exactly, confirming this configuration object's value contract.");
        lines.emplace_back();
        lines.push_back("cnanext's RenderPipeline and post-processing passes consume these settings.");
        lines.push_back("This catalog screen tests the object itself; it does not create or attach a");
        lines.push_back("RenderPipeline to this GraphicsDevice, so changing it here has no visual effect.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        const bool storeIsHonest = defaultsMatch_ && roundTripExact_;
        DrawVerdict(sb, font, end.Y + 6.0f,
                    mul(storeIsHonest ? Color(70, 200, 100, 255) : Color(220, 60, 60, 255), TransitionAlpha()),
                    tint,
                    storeIsHonest
                        ? "Verified live: defaults and all configured values round-trip exactly."
                        : "Round trip mismatch -- the store is not faithful (see NEXT.md).");
    }

private:
    bool defaultsMatch_ = false;
    bool roundTripExact_ = false;
};

} // namespace CnaExamples::Demos::Graphics3D::PbrDemos
