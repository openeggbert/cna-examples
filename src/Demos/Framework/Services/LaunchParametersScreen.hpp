// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/LaunchParameters.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Framework::ServicesDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::LaunchParameters;

// Game::LaunchParameters is the parsed command line, as a string->string map.
// XNA populated it from the process arguments; CNA does the same.
//
// This app is unusually good at demonstrating it, because it has a real command
// line of its own: run it as
//
//     cna_examples --demo "Framework/Services/LaunchParameters"
//
// and both of those show up in the table below, parsed into a key and a value.
// The screen also parses a fixed synthetic argument list through the NOXNA
// explicit constructor, so the parsing rules (leading dashes stripped, "=" or a
// following token as the value, bare flags getting an empty value) are visible
// even when the app was started with no arguments at all.
class LaunchParametersScreen : public DemoScreen {
public:
    LaunchParametersScreen() : DemoScreen("Services: LaunchParameters") {}

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const LaunchParameters& live = GetScreenManager()->getGameProperty()
                                           .getLaunchParametersProperty();

        std::vector<std::string> lines;
        lines.push_back("Game::LaunchParameters -- this process's own command line, parsed.");
        lines.emplace_back();
        lines.push_back("Entries: " + std::to_string((int)live.size()));
        if (live.empty()) {
            lines.push_back("  (started with no arguments)");
        } else {
            int shown = 0;
            for (const auto& [key, value] : live) {
                if (shown++ >= 6) { lines.push_back("  ..."); break; }
                lines.push_back("  \"" + key + "\" = \"" + value + "\"");
            }
        }
        lines.push_back("ContainsKey(\"demo\"): " +
                        std::string(live.ContainsKey("demo") ? "true" : "false"));
        lines.emplace_back();
        lines.push_back("Parsed from a fixed synthetic argv (NOXNA explicit constructor),");
        lines.push_back("so the rules are visible even with no real arguments:");
        for (const auto& [key, value] : Synthetic()) {
            lines.push_back("  \"" + key + "\" = \"" + value + "\"");
        }
        lines.emplace_back();
        lines.push_back("Input was: --width=800  -fullscreen  --level  forest");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    // Built once and cached: LaunchParameters is an unordered_map, so rebuilding
    // it every frame would reshuffle the display order for no reason.
    static const LaunchParameters& Synthetic() {
        static const LaunchParameters parsed(std::vector<std::string>{
            "--width=800", "-fullscreen", "--level", "forest"});
        return parsed;
    }
};

} // namespace CnaExamples::Demos::Framework::ServicesDemos
