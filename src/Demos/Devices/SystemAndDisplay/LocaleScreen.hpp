// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/Devices/Locale.hpp"
#include "CNA/Devices/LocaleInfo.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::SystemAndDisplayDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::Locale;
using CNA::Devices::LocaleInfo;

// Demonstrates CNA::Devices::Locale -- the user's OS-level preferred
// language(s)/region(s), most-preferred first. No XNA/WP7 equivalent.
// Genuinely desktop-testable (reads real OS locale settings).
class LocaleScreen : public DemoScreen {
public:
    LocaleScreen() : DemoScreen("System & Display: Locale") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const std::vector<LocaleInfo> locales = Locale::getPreferredLocalesProperty();

        std::vector<std::string> lines;
        lines.push_back("Preferred locales, most-preferred first: " + std::to_string(locales.size()));
        for (const auto& locale : locales) {
            lines.push_back("  " + locale.Language +
                            (locale.Country.empty() ? "" : ("-" + locale.Country)));
        }
        if (locales.empty()) lines.push_back("  (none reported)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }
};

} // namespace CnaExamples::Demos::Devices::SystemAndDisplayDemos
