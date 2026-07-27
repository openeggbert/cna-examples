// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Demos/DemoScreen.hpp"
#include "Demos/Framework/DeviceManager/GdmAccess.hpp"

namespace CnaExamples::Demos::Framework::DeviceManagerDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// GraphicsDeviceManager's "Preferred*" properties are requests, not commands.
// Setting one changes nothing until ApplyChanges() is called, and even then the
// device may come back with something else -- the driver has the final say.
//
// That two-step is the whole lesson here, and this screen shows it directly:
// the preferred values and the device's actual back-buffer size are printed
// side by side, so the gap between "asked for" and "got" is visible both before
// and after ApplyChanges().
//
// Everything is restored on the way out. A demo that left the window at another
// resolution -- or full-screen -- would hand the next demo a different world.
class ResolutionAndFullScreenScreen : public DemoScreen {
public:
    ResolutionAndFullScreenScreen() : DemoScreen("Device Manager: Resolution & FullScreen") {}

    void LoadContent() override {
        auto* gdm = Manager();
        if (gdm == nullptr) return;
        originalWidth_ = (int)gdm->getPreferredBackBufferWidthProperty();
        originalHeight_ = (int)gdm->getPreferredBackBufferHeightProperty();
        originalFullScreen_ = gdm->getIsFullScreenProperty();
    }

    void UnloadContent() override {
        auto* gdm = Manager();
        if (gdm == nullptr) return;
        if (!dirty_) return;
        gdm->setPreferredBackBufferWidthProperty(originalWidth_);
        gdm->setPreferredBackBufferHeightProperty(originalHeight_);
        gdm->setIsFullScreenProperty(originalFullScreen_);
        gdm->ApplyChanges();
    }

protected:
    void OnDemoInput(InputState& input) override {
        auto* gdm = Manager();
        if (gdm == nullptr) return;

        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            // Change the request only. Nothing happens to the window yet --
            // that is the point of the next binding.
            sizeIndex_ = (sizeIndex_ + 1) % kSizeCount;
            gdm->setPreferredBackBufferWidthProperty(kSizes[sizeIndex_][0]);
            gdm->setPreferredBackBufferHeightProperty(kSizes[sizeIndex_][1]);
            pendingSinceApply_ = true;
        }
        if (input.IsMenuUp(ControllingPlayer())) {
            gdm->ApplyChanges();
            applyCount_++;
            pendingSinceApply_ = false;
            dirty_ = true;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        auto* gdm = Manager();

        std::vector<std::string> lines;
        if (gdm == nullptr) {
            lines.push_back("No GraphicsDeviceManager is registered in Game::Services.");
            lines.push_back("A Game is not required to have one; this screen needs it.");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        const auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();

        lines.push_back("Space/Enter/A/tap: change the REQUEST     Up: ApplyChanges()");
        lines.emplace_back();
        lines.push_back("Requested (PreferredBackBuffer): " +
                        std::to_string((int)gdm->getPreferredBackBufferWidthProperty()) + " x " +
                        std::to_string((int)gdm->getPreferredBackBufferHeightProperty()));
        lines.push_back("Actual (device viewport):        " +
                        std::to_string(viewport.getWidthProperty()) + " x " +
                        std::to_string(viewport.getHeightProperty()));
        lines.push_back("They match: " + std::string(Matches(gdm, viewport) ? "yes" : "NO -- a request is pending or was overridden"));
        lines.emplace_back();
        lines.push_back("Change pending since last ApplyChanges(): " +
                        std::string(pendingSinceApply_ ? "yes" : "no"));
        lines.push_back("ApplyChanges() calls: " + std::to_string(applyCount_));
        lines.emplace_back();
        lines.push_back("IsFullScreen: " + std::string(gdm->getIsFullScreenProperty() ? "true" : "false"));
        lines.push_back("Back-buffer format: " + FormatName(gdm->getPreferredBackBufferFormatProperty()));
        lines.emplace_back();
        lines.push_back("Preferred* properties are a request. Nothing reaches the device until");
        lines.push_back("ApplyChanges(), and the driver may still return something different --");
        lines.push_back("which is why real code reads the viewport back instead of assuming.");
        lines.emplace_back();
        lines.push_back("Everything here is restored when you leave this screen.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr int kSizeCount = 3;
    static constexpr int kSizes[kSizeCount][2] = {{960, 640}, {800, 600}, {1024, 720}};

    GraphicsDeviceManager* Manager() const {
        return FindDeviceManager(GetScreenManager()->getGameProperty());
    }

    static bool Matches(GraphicsDeviceManager* gdm,
                        const Microsoft::Xna::Framework::Graphics::Viewport& viewport) {
        return (int)gdm->getPreferredBackBufferWidthProperty() == viewport.getWidthProperty() &&
               (int)gdm->getPreferredBackBufferHeightProperty() == viewport.getHeightProperty();
    }

    static std::string FormatName(Microsoft::Xna::Framework::Graphics::SurfaceFormat format) {
        using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
        switch (format) {
            case SurfaceFormat::Color: return "Color";
            case SurfaceFormat::Bgr565: return "Bgr565";
            case SurfaceFormat::Bgra5551: return "Bgra5551";
            case SurfaceFormat::Bgra4444: return "Bgra4444";
            default: break;
        }
        return "(" + std::to_string((int)format) + ")";
    }

    int originalWidth_ = 0;
    int originalHeight_ = 0;
    bool originalFullScreen_ = false;
    bool dirty_ = false;
    bool pendingSinceApply_ = false;
    int sizeIndex_ = 0;
    int applyCount_ = 0;
};

} // namespace CnaExamples::Demos::Framework::DeviceManagerDemos
