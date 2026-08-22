// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/GraphicsRendererType.hpp"
#include "CNA/GraphicsCapability.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Diagnostics::CapabilitiesDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// CNA picks its graphics backend at COMPILE time (CNA_GRAPHICS_RENDERER), and
// the backends genuinely differ in what they can do. SDL_RENDERER, DX3 and
// CANVAS are 2D-only by design: every 3D call throws. Others lack MSAA, or
// multiple render targets, or occlusion queries.
//
// GraphicsDevice::SupportsCapability is how an application asks instead of
// finding out by exception. Each capability value maps to a real,
// already-documented gap somewhere in CNA -- this is not a speculative list.
//
// This screen queries every capability live. It is also the screen that makes the
// second-backend work meaningful: a build against SDL_RENDERER shows ThreeD
// as unsupported here, and a catalog that gates on that reports honestly
// instead of crashing.
class GraphicsCapabilityScreen : public DemoScreen {
public:
    GraphicsCapabilityScreen() : DemoScreen("Diagnostics: Graphics Capabilities") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();

        std::vector<std::string> lines;
        lines.push_back("Renderer chosen at compile time: " +
                        std::string(CNA::getCurrentGraphicsRendererName()));
        lines.push_back("(the CNA_GRAPHICS_RENDERER CMake option -- not a runtime switch)");
        lines.emplace_back();
        lines.push_back("GraphicsDevice::SupportsCapability, queried live:");
        lines.emplace_back();

        int supported = 0;
        for (const auto& entry : kCapabilities) {
            const bool ok = device.SupportsCapability(entry.value);
            if (ok) supported++;
            lines.push_back("  " + Pad(entry.name, 26) + (ok ? "yes" : "NO ") + "   " + entry.note);
        }

        lines.emplace_back();
        lines.push_back(std::to_string(supported) + " of " +
                        std::to_string((int)std::size(kCapabilities)) + " supported here.");
        lines.emplace_back();
        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    struct Entry {
        CNA::GraphicsCapability value;
        const char* name;
        const char* note;
    };

    // Ordered so the most consequential one is first: without ThreeD, an entire
    // half of this catalog cannot run at all.
    static constexpr Entry kCapabilities[] = {
        {CNA::GraphicsCapability::ThreeD,                  "ThreeD",                  "vertex/index buffers, 3D draws, depth"},
        {CNA::GraphicsCapability::DepthStencilBuffer,      "DepthStencilBuffer",      "a real depth/stencil on the target"},
        {CNA::GraphicsCapability::MultiSampleAntiAliasing, "MultiSampleAntiAliasing", "any sample count above 1"},
        {CNA::GraphicsCapability::MultipleRenderTargets,   "MultipleRenderTargets",   "more than one target at once"},
        {CNA::GraphicsCapability::AnisotropicFiltering,    "AnisotropicFiltering",    "device/driver dependent"},
        {CNA::GraphicsCapability::WireFrame,               "WireFrame",               "FillMode::WireFrame"},
        {CNA::GraphicsCapability::OcclusionQuery,          "OcclusionQuery",          "real GPU pixel counts"},
        {CNA::GraphicsCapability::CustomEffects,           "CustomEffects",           "a non-stock Effect in SpriteBatch"},
        {CNA::GraphicsCapability::Texture3D,                "Texture3D",               "real volume texture storage"},
        {CNA::GraphicsCapability::MultiStreamVertexInput,  "MultiStreamVertexInput",  "several vertex-buffer input streams"},
        {CNA::GraphicsCapability::Instancing,               "Instancing",              "DrawInstancedPrimitives"},
        {CNA::GraphicsCapability::StencilBuffer,            "StencilBuffer",           "an independent stencil plane"},
        {CNA::GraphicsCapability::AdditiveBlending,         "AdditiveBlending",        "genuine BlendState::Additive"},
        {CNA::GraphicsCapability::CompiledEffects,          "CompiledEffects",         "XNA/FNA compiled effect bytecode"},
        {CNA::GraphicsCapability::FloatRenderTargets,       "FloatRenderTargets",      "32-bit float render targets"},
        {CNA::GraphicsCapability::HalfFloatRenderTargets,   "HalfFloatRenderTargets",  "16-bit float render targets"},
        {CNA::GraphicsCapability::HalfFloatTextureLinearFiltering,
                                                        "HalfFloatTextureFiltering", "linear filtering of half-float textures"},
        {CNA::GraphicsCapability::ComputeShaders,           "ComputeShaders",          "compute shaders and storage buffers"},
        {CNA::GraphicsCapability::IndirectDraw,             "IndirectDraw",            "GPU-buffer-driven draw arguments"},
    };

    static std::string Pad(const std::string& text, std::size_t width) {
        return text.size() >= width ? text + " " : text + std::string(width - text.size(), ' ');
    }
};

} // namespace CnaExamples::Demos::Diagnostics::CapabilitiesDemos
