// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "CNA/Devices/SystemTray.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::SystemTray;

// Demonstrates CNA::Devices::SystemTray -- a real notification-area icon
// with a flat menu, no XNA/WP7 equivalent, and genuinely desktop-only (no
// Android/iOS/Web backend exists at all, unlike most of this app's other
// Devices demos which at least try on mobile). Creates one real tray icon
// with a checkable "Enabled" entry on entry, destroys it on exit -- check
// the system tray/notification area while this screen is open.
class SystemTrayScreen : public DemoScreen {
public:
    SystemTrayScreen() : DemoScreen("Desktop: SystemTray") {}

    void LoadContent() override {
        supported_ = SystemTray::getIsSupportedProperty();
        if (!supported_) return;
        tray_.emplace("cna-examples: SystemTray demo");
        entryIndex_ = tray_->AddEntry("Enabled", true, true, true,
                                      [this]() { OnEntryClicked(); });
    }

    void UnloadContent() override {
        tray_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back(std::string("IsSupported: ") + (supported_ ? "true" : "false"));

        if (!supported_) {
            lines.push_back("(not supported on this platform -- desktop only)");
        } else {
            lines.push_back("A real tray icon is showing right now -- check your notification area.");
            lines.push_back("It has one checkable \"Enabled\" menu entry.");
            lines.push_back("Entry checked: " + std::string(tray_->GetEntryChecked(entryIndex_) ? "true" : "false"));
            lines.push_back("Menu clicks: " + std::to_string(clickCount_));
        }

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void OnEntryClicked() {
        clickCount_++;
        tray_->SetEntryChecked(entryIndex_, !tray_->GetEntryChecked(entryIndex_));
    }

    bool supported_ = false;
    std::optional<SystemTray> tray_;
    std::size_t entryIndex_ = 0;
    int clickCount_ = 0;
};

} // namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos
