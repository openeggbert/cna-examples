// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/PreparingDeviceSettingsEventArgs.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Framework/DeviceManager/GdmAccess.hpp"

namespace CnaExamples::Demos::Framework::DeviceManagerDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::PreparingDeviceSettingsEventArgs;

// The GraphicsDeviceManager's five events are the device's whole life story:
//
//   PreparingDeviceSettings -- fires BEFORE the device is created or reset, and
//       hands you the settings that are about to be used. It is the only place
//       an application can change them; every other event is a notification
//       after the fact.
//   DeviceCreated    -- the device now exists.
//   DeviceResetting  -- it is about to be reset; release anything device-bound.
//   DeviceReset      -- it is back; recreate what you released.
//   DeviceDisposing  -- it is going away for good.
//
// DeviceCreated has already happened by the time any screen can subscribe (the
// device is created during Game startup), so it is listed but will not appear in
// the log. The reset pair is reachable: ApplyChanges() with a changed setting
// resets the device, and this screen does exactly that on demand.
class DeviceEventsScreen : public DemoScreen {
public:
    DeviceEventsScreen() : DemoScreen("Device Manager: Device Events") {}

    void LoadContent() override {
        auto* gdm = Manager();
        if (gdm == nullptr) return;

        originalWidth_ = (int)gdm->getPreferredBackBufferWidthProperty();
        originalHeight_ = (int)gdm->getPreferredBackBufferHeightProperty();

        preparingToken_ = gdm->PreparingDeviceSettings.Add(
            [this](System::Object*, const PreparingDeviceSettingsEventArgs&) {
                Log("PreparingDeviceSettings");
            });
        createdToken_ = gdm->DeviceCreated.Add(
            [this](System::Object*, const System::EventArgs&) { Log("DeviceCreated"); });
        resettingToken_ = gdm->DeviceResetting.Add(
            [this](System::Object*, const System::EventArgs&) { Log("DeviceResetting"); });
        resetToken_ = gdm->DeviceReset.Add(
            [this](System::Object*, const System::EventArgs&) { Log("DeviceReset"); });
        disposingToken_ = gdm->DeviceDisposing.Add(
            [this](System::Object*, const System::EventArgs&) { Log("DeviceDisposing"); });
    }

    void UnloadContent() override {
        auto* gdm = Manager();
        if (gdm == nullptr) return;

        // These handlers capture `this`; the manager outlives the screen.
        gdm->PreparingDeviceSettings.Remove(preparingToken_);
        gdm->DeviceCreated.Remove(createdToken_);
        gdm->DeviceResetting.Remove(resettingToken_);
        gdm->DeviceReset.Remove(resetToken_);
        gdm->DeviceDisposing.Remove(disposingToken_);

        if (dirty_) {
            gdm->setPreferredBackBufferWidthProperty(originalWidth_);
            gdm->setPreferredBackBufferHeightProperty(originalHeight_);
            gdm->ApplyChanges();
        }
    }

protected:
    void OnDemoInput(InputState& input) override {
        auto* gdm = Manager();
        if (gdm == nullptr) return;

        PlayerIndex playerIndex;
        if (!input.IsMenuSelect(ControllingPlayer(), playerIndex)) return;

        // ApplyChanges only resets the device if something actually changed, so
        // nudge the size back and forth to force a real reset each time.
        toggled_ = !toggled_;
        gdm->setPreferredBackBufferWidthProperty(toggled_ ? originalWidth_ - 64 : originalWidth_);
        gdm->setPreferredBackBufferHeightProperty(originalHeight_);
        gdm->ApplyChanges();
        dirty_ = true;
        applyCount_++;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        if (Manager() == nullptr) {
            lines.push_back("No GraphicsDeviceManager is registered in Game::Services.");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Space/Enter/A/tap: change the back-buffer size and ApplyChanges(),");
        lines.push_back("forcing a real device reset.");
        lines.emplace_back();
        lines.push_back("ApplyChanges() calls: " + std::to_string(applyCount_));
        lines.emplace_back();
        lines.push_back("Event log (written by the handlers, newest last):");
        if (log_.empty()) {
            lines.push_back("  (nothing yet)");
        } else {
            for (const auto& entry : log_) lines.push_back("  " + entry);
        }
        lines.emplace_back();
        lines.push_back("DeviceCreated is subscribed to but will not appear: the device is created");
        lines.push_back("during Game startup, long before any screen exists to listen for it.");
        lines.emplace_back();
        lines.push_back("PreparingDeviceSettings is the only one of the five that can still change");
        lines.push_back("the outcome -- it runs before the device is made. The rest are reports.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    GraphicsDeviceManager* Manager() const {
        return FindDeviceManager(GetScreenManager()->getGameProperty());
    }

    void Log(std::string message) {
        log_.push_back("#" + std::to_string(++sequence_) + "  " + std::move(message));
        if (log_.size() > 8) log_.erase(log_.begin());
    }

    std::vector<std::string> log_;
    int sequence_ = 0;
    int applyCount_ = 0;
    int originalWidth_ = 0;
    int originalHeight_ = 0;
    bool toggled_ = false;
    bool dirty_ = false;
    System::EventHandler<PreparingDeviceSettingsEventArgs>::Token preparingToken_{};
    System::EventHandler<System::EventArgs>::Token createdToken_{};
    System::EventHandler<System::EventArgs>::Token resettingToken_{};
    System::EventHandler<System::EventArgs>::Token resetToken_{};
    System::EventHandler<System::EventArgs>::Token disposingToken_{};
};

} // namespace CnaExamples::Demos::Framework::DeviceManagerDemos
