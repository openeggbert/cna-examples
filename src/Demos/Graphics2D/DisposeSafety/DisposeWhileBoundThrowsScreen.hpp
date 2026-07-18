// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "System/InvalidOperationException.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics2D::DisposeSafetyDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::RenderTarget2D;

// Demonstrates a real, confirmed gotcha: RenderTarget2D::Dispose() throws
// System::InvalidOperationException("Disposing target that is still
// bound") if called while the render target is still the device's active
// render target -- matching FNA's own "disposing a bound render target is
// a programming error" behavior, not a CNA bug. OnDemoInput can't touch
// the device/SpriteBatch directly, so a pending-attempt flag defers the
// actual bind+Dispose() attempt into OnDemoDraw.
class DisposeWhileBoundThrowsScreen : public DemoScreen {
public:
    DisposeWhileBoundThrowsScreen() : DemoScreen("RenderTarget2D: Dispose While Bound Throws") {}

    void LoadContent() override {
        target_.emplace(GetScreenManager()->getGraphicsDeviceProperty(), 64, 64);
    }

    void UnloadContent() override {
        // Deliberately not calling target_->Dispose() here -- see the class
        // doc comment above: it's confirmed to throw while bound, and this
        // demo's own attempt below may leave the target's disposal state
        // ambiguous either way. UnloadContent() must never throw, so this
        // just lets the optional's destructor run instead, same discipline
        // as MediaLibraryScreen's UnloadContent() in the Media area.
        target_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            pendingAttempt_ = true;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        if (pendingAttempt_) {
            pendingAttempt_ = false;
            attempts_++;
            auto& device = GetScreenManager()->getGraphicsDeviceProperty();
            device.SetRenderTarget(&*target_);
            try {
                target_->Dispose();
                lastResult_ = "Dispose() returned without throwing (unexpected)";
            } catch (const System::InvalidOperationException& ex) {
                lastResult_ = std::string("Dispose() threw: ") + ex.getMessageProperty();
            }
            device.SetRenderTarget(nullptr); // leave the device in a clean state either way
        }

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: SetRenderTarget(this), then Dispose() while still bound");
        lines.emplace_back();
        lines.push_back("Attempts: " + std::to_string(attempts_));
        if (!lastResult_.empty()) lines.push_back(lastResult_);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    std::optional<RenderTarget2D> target_;
    bool pendingAttempt_ = false;
    int attempts_ = 0;
    std::string lastResult_;
};

} // namespace CnaExamples::Demos::Graphics2D::DisposeSafetyDemos
