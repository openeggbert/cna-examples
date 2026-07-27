// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"

#include "Demos/Content/ContentDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Content::ErrorsDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Content loading fails in more than one way, and the failures are worth
// knowing apart, because they point at different mistakes:
//
//   missing asset      -- the name resolves to no file at all
//   wrong type         -- the file exists but does not hold what was asked for
//   corrupt/not-content-- the file exists and is not valid content
//   unsupported reader -- valid .xnb naming a reader CNA does not implement
//
// Every message below is produced by really attempting the load and catching
// what comes back. None of them is transcribed: if CNA's wording changes, this
// screen changes with it.
class ContentLoadExceptionScreen : public DemoScreen {
public:
    ContentLoadExceptionScreen() : DemoScreen("Content: Load Failures") {}

    void OnDemoLoad() override {
        manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
        manager_->setRootDirectoryProperty("Content");
        // The service provider alone is not enough: CNA's ContentManager needs
        // its GraphicsDevice set explicitly before any texture load, exactly as
        // CNA's own .xnb tests do. Without it every Load<Texture2D> fails.
        manager_->setGraphicsDevice(GetScreenManager()->getGraphicsDeviceProperty());
        RunAll();
    }

    void OnDemoUnload() override { manager_.reset(); }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) RunAll();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: run every failing load again");
        lines.emplace_back();
        lines.push_back("Each message below came out of a real attempt, not a transcript.");
        lines.emplace_back();
        for (const auto& line : results_) lines.push_back(line);
        lines.emplace_back();
        lines.push_back("None of these crash the app: ContentLoadException is a normal");
        lines.push_back("exception, and a game is expected to catch it and fall back --");
        lines.push_back("to a placeholder texture, or to a clear message to the player.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void RunAll() {
        results_.clear();
        Attempt("missing asset", "no_such_asset_anywhere");
        Attempt("wrong type (a SpriteFont descriptor asked for as a texture)", "menufont");
        if (XnbFixturesAvailable()) {
            Attempt("wrong type (.xnb holding a TextureCube)",
                    "ContentDemo/xnb/monogame/windows/uncompressed/SampleCube64DXT1Mips");
        } else {
            results_.push_back("wrong type (.xnb): skipped -- " + MissingFixturesMessage());
            results_.emplace_back();
        }
    }

    void Attempt(const std::string& label, const std::string& assetName) {
        results_.push_back(label + ":");
        results_.push_back("  Load<Texture2D>(\"" + assetName + "\")");
        try {
            Texture2D texture = manager_->Load<Texture2D>(assetName);
            results_.push_back("  -> succeeded (" + std::to_string(texture.getWidthProperty()) +
                               "x" + std::to_string(texture.getHeightProperty()) +
                               ") -- not a failure case after all");
        } catch (const std::exception& ex) {
            results_.push_back("  -> " + Wrap(ex.what()));
        }
        results_.emplace_back();
    }

    // Exception messages routinely embed a full path and overflow the line.
    static std::string Wrap(const std::string& message) {
        constexpr std::size_t kMax = 76;
        return message.size() <= kMax ? message : message.substr(0, kMax) + "...";
    }

    std::optional<Microsoft::Xna::Framework::Content::ContentManager> manager_;
    std::vector<std::string> results_;
};

} // namespace CnaExamples::Demos::Content::ErrorsDemos
