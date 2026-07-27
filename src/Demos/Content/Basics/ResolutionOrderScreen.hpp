// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/Content/ContentDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Content::BasicsDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// An asset name is not a filename. ContentManager takes a name with no
// extension, resolves it under RootDirectory, and picks a file according to a
// defined priority order -- CNA supports three content formats side by side:
//
//   1. <name>.xnb  -- the binary XNA pipeline format
//   2. <name>      -- the literal path as given (a loose .png, .wav, ...)
//   3. <name>.cnj  -- CNA's own JSON descriptor format
//
// This screen resolves several real names against the running app's own
// Content directory and shows which candidate files actually exist for each,
// so the ordering is demonstrated against the filesystem rather than asserted.
class ResolutionOrderScreen : public DemoScreen {
public:
    ResolutionOrderScreen() : DemoScreen("Content: Asset Name Resolution") {}

    void UnloadContent() override { privateManager_.reset(); }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            nameIndex_ = (nameIndex_ + 1) % kNameCount;
            loadResult_.clear();
        }
        if (input.IsMenuUp(ControllingPlayer())) TryLoad();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const std::string name = kNames[nameIndex_];

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: next asset name     Up: try Load<Texture2D> on it");
        lines.emplace_back();
        lines.push_back("RootDirectory = \"Content\"");
        lines.push_back("Asset name    = \"" + name + "\"");
        lines.emplace_back();
        lines.push_back("Candidates, in the order ContentManager considers them:");
        lines.push_back("  1. " + Candidate(name + ".xnb"));
        lines.push_back("  2. " + Candidate(name) + "        (the literal path)");
        lines.push_back("  3. " + Candidate(name + ".cnj"));
        lines.emplace_back();
        lines.push_back("Load result:");
        if (loadResult_.empty()) {
            lines.push_back("  (press Up to try)");
        } else {
            for (const auto& line : loadResult_) lines.push_back("  " + line);
        }
        lines.emplace_back();
        lines.push_back("The name carries no extension on purpose: the same game code loads a");
        lines.push_back("loose .png during development and a packed .xnb in a shipping build,");
        lines.push_back("with nothing at the call site to change.");
        lines.emplace_back();
        lines.push_back(".cnj is CNA's own JSON descriptor format -- see the CNJ Format category.");
        lines.push_back("It is what this app's own menu font is stored as.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr int kNameCount = 4;
    static constexpr const char* kNames[kNameCount] = {
        "blank",                 // a loose .png next to the executable
        "menufont",              // stored as .cnj
        "menufont_atlas",        // the .png the .cnj descriptor points at
        "no_such_asset",         // resolves to nothing at all
    };

    // Reports whether a candidate file is actually on disk, so the priority
    // list is checked against reality rather than recited.
    static std::string Candidate(const std::string& relative) {
        std::error_code ec;
        const std::filesystem::path path = std::filesystem::path("Content") / relative;
        const bool exists = std::filesystem::exists(path, ec) && !ec;
        return relative + (exists ? "   [exists]" : "   [absent]");
    }

    void TryLoad() {
        if (!privateManager_) {
            privateManager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
            privateManager_->setRootDirectoryProperty("Content");
            // The service provider alone is not enough: CNA's ContentManager
            // needs its GraphicsDevice set explicitly before any texture load,
            // exactly as CNA's own .xnb tests do. Without it every
            // Load<Texture2D> fails.
            privateManager_->setGraphicsDevice(GetScreenManager()->getGraphicsDeviceProperty());
        }
        loadResult_.clear();
        try {
            Texture2D texture = privateManager_->Load<Texture2D>(kNames[nameIndex_]);
            loadResult_.push_back("loaded a " + std::to_string(texture.getWidthProperty()) + "x" +
                                  std::to_string(texture.getHeightProperty()) + " Texture2D");
        } catch (const std::exception& ex) {
            loadResult_.push_back("threw: " + std::string(ex.what()));
            loadResult_.push_back("(expected for a name with no matching file, and for one whose");
            loadResult_.push_back(" file is not a texture -- see the Errors category)");
        }
    }

    std::optional<Microsoft::Xna::Framework::Content::ContentManager> privateManager_;
    std::vector<std::string> loadResult_;
    int nameIndex_ = 0;
};

} // namespace CnaExamples::Demos::Content::BasicsDemos
