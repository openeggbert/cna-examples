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
// CNA_NOXNA CMake option -- OFF by default in CNA itself, same reasoning as
// CNA_DEVICES for the Devices area -- which this project now enables
// alongside CNA_DEVICES in its own top-level CMakeLists.txt.
//
// What this screen actually proves, verified live rather than assumed:
// RenderPipelineSettings is a real, faithful settings store -- every
// property round-trips exactly what was set. What it also proves, by
// grepping the ENTIRE ../cna source tree (not just this header): nothing
// outside RenderPipelineSettings' own .cpp reads it. No GraphicsDevice, no
// backend, no effect consults it. Its own doc comment says "Construct via
// GraphicsDevice::GetRenderPipelineSettings()" -- that method does not
// exist anywhere in CNA. This is therefore an honest "real API, zero
// rendering effect" screen, in the same spirit as D8 Net's amber verdict
// or D5's SurfaceFormat findings: the swatch is not a pass/fail on
// correctness, it is a plain statement of what is and is not wired up.
class RenderPipelineSettingsScreen : public DemoScreen {
public:
    RenderPipelineSettingsScreen() : DemoScreen("RenderPipelineSettings: A Store With No Reader") {}

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
        // every one back. This is the only thing about this type that is
        // testable: is it a faithful store, or does something clamp/ignore
        // a value silently? Verified live, not assumed.
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
        lines.push_back("bloom, SSAO, render & shadow quality. A real, faithful settings bag: every");
        lines.push_back("property set above was read back exactly, defaults match the header too.");
        lines.emplace_back();
        lines.push_back("But grep the whole of ../cna: nothing outside this type's own .cpp reads");
        lines.push_back("it. No GraphicsDevice, no backend, no effect consults these values -- the");
        lines.push_back("header's own \"construct via GetRenderPipelineSettings()\" names a method");
        lines.push_back("that does not exist anywhere in CNA. Setting these has zero visible effect.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        const bool storeIsHonest = defaultsMatch_ && roundTripExact_;
        DrawVerdict(sb, font, end.Y + 6.0f,
                    mul(storeIsHonest ? Color(230, 170, 40, 255) : Color(220, 60, 60, 255), TransitionAlpha()),
                    tint,
                    storeIsHonest
                        ? "Verified live: a real, faithful store -- and, by source grep, an unread one."
                        : "Round trip mismatch -- the store is not faithful (see NEXT.md).");
    }

private:
    bool defaultsMatch_ = false;
    bool roundTripExact_ = false;
};

} // namespace CnaExamples::Demos::Graphics3D::PbrDemos
