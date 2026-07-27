// SPDX-License-Identifier: MIT
#pragma once

#include <chrono>
#include <exception>
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

// ContentManager caches loaded assets by name, so loading the same name twice
// does not re-read the file.
//
// An important honesty note about how that is demonstrated here. In real XNA
// Load<T> returns a reference to the cached instance, so two loads can be shown
// to be the same object by comparing them. CNA's Load<T> returns **by value**
// (see the Texture2D/SoundEffect/TextureCube specialisations in
// ContentManager.hpp), so consumer code cannot compare identity at all -- any
// "same object" check written here would really be testing a cache this screen
// had built itself, and would prove nothing about ContentManager.
//
// What IS observable from outside is the cost: the first load touches the disk
// and the second does not. That is what this screen measures, and Unload()
// makes the next load expensive again -- which is the cache being emptied.
class LoadAndCacheScreen : public DemoScreen {
public:
    LoadAndCacheScreen() : DemoScreen("Content: Load & Cache") {}

    void UnloadContent() override {
        // A private ContentManager, so nothing this screen unloads can disturb
        // the assets the running application still needs.
        privateManager_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) MeasureLoads();
        if (input.IsMenuUp(ControllingPlayer())) UnloadAll();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: load \"blank\" three times, timing each");
        lines.push_back("Up: Unload() -- empties the cache, so the next load is slow again");
        lines.emplace_back();
        lines.push_back("This screen owns a private ContentManager. RootDirectory = \"Content\".");
        lines.emplace_back();
        lines.push_back("Measurements taken: " + std::to_string(rounds_) +
                        "   Unload() calls: " + std::to_string(unloads_));
        lines.emplace_back();
        if (result_.empty()) {
            lines.push_back("(nothing measured yet)");
        } else {
            for (const auto& line : result_) lines.push_back(line);
        }
        lines.emplace_back();
        lines.push_back("Why timing rather than an identity check: CNA's Load<T> returns BY");
        lines.push_back("VALUE, unlike XNA's, so two loads are two objects even on a cache hit.");
        lines.push_back("Comparing addresses here would test nothing about ContentManager.");
        lines.emplace_back();
        lines.push_back("Unload() releases everything this manager owns at once -- there is no");
        lines.push_back("per-asset release. That is why a game keeps several managers: a");
        lines.push_back("long-lived one for menus, a per-level one it can discard wholesale.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void EnsureManager() {
        if (privateManager_) return;
        // ContentManager takes an IServiceProvider*; GameServiceContainer is one.
        privateManager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
        privateManager_->setRootDirectoryProperty("Content");
        // The service provider alone is not enough: CNA's ContentManager needs
        // its GraphicsDevice set explicitly before any texture load, exactly as
        // CNA's own .xnb tests do. Without it every Load<Texture2D> fails.
        privateManager_->setGraphicsDevice(GetScreenManager()->getGraphicsDeviceProperty());
    }

    void MeasureLoads() {
        EnsureManager();
        result_.clear();
        try {
            long long first = 0, second = 0, third = 0;
            {
                const auto start = std::chrono::steady_clock::now();
                Texture2D texture = privateManager_->Load<Texture2D>("blank");
                first = Micros(start);
                size_ = std::to_string(texture.getWidthProperty()) + "x" +
                        std::to_string(texture.getHeightProperty());
            }
            {
                const auto start = std::chrono::steady_clock::now();
                Texture2D texture = privateManager_->Load<Texture2D>("blank");
                second = Micros(start);
                (void)texture;
            }
            {
                const auto start = std::chrono::steady_clock::now();
                Texture2D texture = privateManager_->Load<Texture2D>("blank");
                third = Micros(start);
                (void)texture;
            }

            rounds_++;
            result_.push_back("Load<Texture2D>(\"blank\") -> " + size_ + " texture");
            result_.push_back("  1st load: " + std::to_string(first) + " us" +
                              (rounds_ == 1 || unloadedSinceLastRound_
                                   ? "   <- reads the file"
                                   : "   <- already cached from a previous round"));
            result_.push_back("  2nd load: " + std::to_string(second) + " us   <- cache hit");
            result_.push_back("  3rd load: " + std::to_string(third) + " us   <- cache hit");
            unloadedSinceLastRound_ = false;
        } catch (const std::exception& ex) {
            result_.push_back(std::string("Load threw: ") + ex.what());
        }
    }

    void UnloadAll() {
        if (!privateManager_) return;
        privateManager_->Unload();
        unloads_++;
        unloadedSinceLastRound_ = true;
        result_.clear();
        result_.push_back("Unload() called. Every asset this manager owned is released;");
        result_.push_back("the next load has to read the file again. Measure once more to see it.");
    }

    static long long Micros(std::chrono::steady_clock::time_point from) {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now() - from)
            .count();
    }

    std::optional<Microsoft::Xna::Framework::Content::ContentManager> privateManager_;
    std::vector<std::string> result_;
    std::string size_;
    int rounds_ = 0;
    int unloads_ = 0;
    bool unloadedSinceLastRound_ = true;
};

} // namespace CnaExamples::Demos::Content::BasicsDemos
