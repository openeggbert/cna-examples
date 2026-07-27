// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/GraphicsCapability.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Framework/DeviceManager/GdmAccess.hpp"
#include "Demos/Framework/FrameworkDemoHelpers.hpp"

namespace CnaExamples::Demos::Framework::DeviceManagerDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// SynchronizeWithVerticalRetrace is v-sync: it caps presentation to the
// display's refresh rate. Turning it off is measurable rather than visible --
// the frame rate rises above the refresh rate -- so this screen measures.
//
// The measurement is only meaningful with IsFixedTimeStep off. A fixed-timestep
// loop holds its own rate regardless of v-sync, so leaving it on would flatten
// the very difference being demonstrated. The screen forces it off and says so.
//
// PreferMultiSampling is a request for MSAA, and like every other Preferred*
// property it needs ApplyChanges(). Whether it can be honoured at all is a
// backend question, so the answer is read from
// GraphicsDevice::SupportsCapability rather than assumed.
class VSyncAndMultiSamplingScreen : public DemoScreen {
public:
    VSyncAndMultiSamplingScreen() : DemoScreen("Device Manager: VSync & MultiSampling") {}

    void LoadContent() override {
        auto& game = GetScreenManager()->getGameProperty();
        originalIsFixed_ = game.getIsFixedTimeStepProperty();
        // Measuring v-sync under a fixed timestep would measure the timestep.
        game.setIsFixedTimeStepProperty(false);

        if (auto* gdm = Manager()) {
            originalVSync_ = gdm->getSynchronizeWithVerticalRetraceProperty();
            originalMsaa_ = gdm->getPreferMultiSamplingProperty();
        }
    }

    void UnloadContent() override {
        GetScreenManager()->getGameProperty().setIsFixedTimeStepProperty(originalIsFixed_);
        auto* gdm = Manager();
        if (gdm == nullptr || !dirty_) return;
        gdm->setSynchronizeWithVerticalRetraceProperty(originalVSync_);
        gdm->setPreferMultiSamplingProperty(originalMsaa_);
        gdm->ApplyChanges();
    }

protected:
    void OnDemoInput(InputState& input) override {
        auto* gdm = Manager();
        if (gdm == nullptr) return;

        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            gdm->setSynchronizeWithVerticalRetraceProperty(
                !gdm->getSynchronizeWithVerticalRetraceProperty());
            gdm->ApplyChanges();
            dirty_ = true;
            history_.Clear();
        }
        if (input.IsMenuUp(ControllingPlayer())) {
            gdm->setPreferMultiSamplingProperty(!gdm->getPreferMultiSamplingProperty());
            gdm->ApplyChanges();
            dirty_ = true;
        }
    }

    void OnDemoUpdate(GameTime& gameTime) override {
        history_.Add(gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty());
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        auto* gdm = Manager();
        std::vector<std::string> lines;

        if (gdm == nullptr) {
            lines.push_back("No GraphicsDeviceManager is registered in Game::Services.");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        const bool msaaSupported =
            device.SupportsCapability(CNA::GraphicsCapability::MultiSampleAntiAliasing);

        lines.push_back("Space/Enter/A/tap: toggle v-sync     Up: toggle PreferMultiSampling");
        lines.emplace_back();
        lines.push_back("SynchronizeWithVerticalRetrace: " +
                        std::string(gdm->getSynchronizeWithVerticalRetraceProperty() ? "true" : "false"));
        lines.push_back("IsFixedTimeStep: false   (forced off here -- see below)");
        lines.emplace_back();

        char stats[144];
        std::snprintf(stats, sizeof(stats), "mean %.2f ms  (%.0f fps)   min %.2f   max %.2f",
                      history_.Mean(),
                      history_.Mean() > 0.0 ? 1000.0 / history_.Mean() : 0.0,
                      history_.Min(), history_.Max());
        lines.push_back("Measured over the last " + std::to_string((int)history_.Count()) + " frames:");
        lines.push_back("  " + std::string(stats));
        lines.emplace_back();
        lines.push_back("PreferMultiSampling: " +
                        std::string(gdm->getPreferMultiSamplingProperty() ? "true" : "false"));
        lines.push_back("Backend supports MSAA: " +
                        std::string(msaaSupported ? "yes" : "no -- the request cannot be honoured"));
        lines.push_back("Back-buffer MultiSampleCount: " +
                        std::to_string(device.getPresentationParametersProperty()
                                           .getMultiSampleCountProperty()));
        lines.emplace_back();
        lines.push_back("V-sync caps presentation to the display's refresh rate, so its effect is");
        lines.push_back("measurable rather than visible. A fixed timestep would hold the rate on");
        lines.push_back("its own and hide the difference entirely -- hence forcing it off.");
        lines.emplace_back();
        lines.push_back("Under a headless/virtual display there is no real refresh rate to sync");
        lines.push_back("to, so both settings may measure the same. That is the environment, not a bug.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    GraphicsDeviceManager* Manager() const {
        return FindDeviceManager(GetScreenManager()->getGameProperty());
    }

    FrameDeltaHistory history_{120};
    bool originalIsFixed_ = true;
    bool originalVSync_ = true;
    bool originalMsaa_ = false;
    bool dirty_ = false;
};

} // namespace CnaExamples::Demos::Framework::DeviceManagerDemos
