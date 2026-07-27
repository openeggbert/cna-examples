// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/DisplayMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/DisplayModeCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsAdapter.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Diagnostics::AdapterDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Graphics::DisplayMode;
using Microsoft::Xna::Framework::Graphics::GraphicsAdapter;

// GraphicsAdapter is the display hardware as the framework sees it, and
// SupportedDisplayModes is the list a full-screen resolution picker is built
// from. PresentationParameters is what the device is actually running with
// right now -- the two together answer "what could I ask for" and "what did I
// get".
//
// Under a virtual display much of this is synthetic (one adapter, one mode).
// The screen says so rather than presenting a headless environment's answers as
// though they described real hardware.
class GraphicsAdapterScreen : public DemoScreen {
public:
    GraphicsAdapterScreen() : DemoScreen("Diagnostics: Adapter & Display Modes") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) page_++;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        GraphicsAdapter& adapter = GraphicsAdapter::getDefaultAdapterProperty();
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const auto& presentation = device.getPresentationParametersProperty();

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: page through the supported display modes");
        lines.emplace_back();
        lines.push_back("DefaultAdapter:");
        lines.push_back("  Description:      " + adapter.getDescriptionProperty());
        lines.push_back("  DeviceName:       " + adapter.getDeviceNameProperty());
        lines.push_back("  IsDefaultAdapter: " +
                        std::string(adapter.getIsDefaultAdapterProperty() ? "true" : "false") +
                        "    IsWideScreen: " +
                        std::string(adapter.getIsWideScreenProperty() ? "true" : "false"));
        lines.push_back("  VendorId " + std::to_string((int)adapter.getVendorIdProperty()) +
                        "   DeviceId " + std::to_string((int)adapter.getDeviceIdProperty()) +
                        "   Revision " + std::to_string((int)adapter.getRevisionProperty()));
        lines.emplace_back();
        lines.push_back("CurrentDisplayMode: " + Describe(adapter.getCurrentDisplayModeProperty()));
        lines.emplace_back();

        const auto& modes = adapter.getSupportedDisplayModesProperty();
        const int count = (int)modes.getCountProperty();
        lines.push_back("SupportedDisplayModes: " + std::to_string(count) +
                        "   (what a full-screen resolution picker offers)");
        if (count == 0) {
            lines.push_back("  (none reported)");
        } else {
            constexpr int kPerPage = 5;
            const int pages = (count + kPerPage - 1) / kPerPage;
            const int page = page_ % pages;
            for (int i = page * kPerPage; i < count && i < (page + 1) * kPerPage; ++i) {
                lines.push_back("  " + Describe(modes[i]));
            }
            lines.push_back("  page " + std::to_string(page + 1) + "/" + std::to_string(pages));
        }

        lines.emplace_back();
        lines.push_back("PresentationParameters -- what the device is really running:");
        lines.push_back("  BackBuffer:       " +
                        std::to_string((int)presentation.getBackBufferWidthProperty()) + " x " +
                        std::to_string((int)presentation.getBackBufferHeightProperty()));
        lines.push_back("  MultiSampleCount: " +
                        std::to_string((int)presentation.getMultiSampleCountProperty()) +
                        "    IsFullScreen: " +
                        std::string(presentation.getIsFullScreenProperty() ? "true" : "false"));
        lines.emplace_back();
        lines.push_back("On a virtual display much of this is synthetic -- one adapter, one mode,");
        lines.push_back("placeholder vendor ids. Those are the environment's honest answers, not");
        lines.push_back("a description of real hardware.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static std::string Describe(const DisplayMode& mode) {
        return std::to_string((int)mode.getWidthProperty()) + " x " +
               std::to_string((int)mode.getHeightProperty()) +
               "   aspect " + Aspect(mode);
    }

    static std::string Aspect(const DisplayMode& mode) {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%.2f", (double)mode.getAspectRatioProperty());
        return buf;
    }

    int page_ = 0;
};

} // namespace CnaExamples::Demos::Diagnostics::AdapterDemos
