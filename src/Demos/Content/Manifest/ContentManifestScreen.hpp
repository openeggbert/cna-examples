// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManifestEntry.hpp"

#include "Demos/Content/ContentDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Content::ManifestDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// GetContentManifest() (a CNA extension, not stock XNA) scans the content root
// once and reports every asset name it can see, along with which formats back
// it. It answers a question XNA gave applications no way to ask: "what content
// is actually here?"
//
// That is genuinely useful beyond curiosity -- a build check can compare the
// manifest against what the game intends to load and catch a missing asset
// before a player does, instead of at the Load<T> call that throws.
//
// The manifest is built lazily on first access and cached; RefreshContentManifest()
// rebuilds it, which is what a hot-reload workflow needs after files change on
// disk. Both are exercised here.
class ContentManifestScreen : public DemoScreen {
public:
    ContentManifestScreen() : DemoScreen("Content: Manifest") {}

    void LoadContent() override {
        manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
        manager_->setRootDirectoryProperty("Content");
        // The service provider alone is not enough: CNA's ContentManager needs
        // its GraphicsDevice set explicitly before any texture load, exactly as
        // CNA's own .xnb tests do. Without it every Load<Texture2D> fails.
        manager_->setGraphicsDevice(GetScreenManager()->getGraphicsDeviceProperty());
    }

    void UnloadContent() override { manager_.reset(); }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            page_++;
            refreshed_ = false;
        }
        if (input.IsMenuUp(ControllingPlayer())) {
            manager_->RefreshContentManifest();
            refreshCount_++;
            refreshed_ = true;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        if (!manager_) {
            DrawLines(sb, font, Vector2(40.0f, 82.0f), {"(no ContentManager)"},
                      mul(Color::White, TransitionAlpha()));
            return;
        }

        const auto& manifest = manager_->GetContentManifest();

        lines.push_back("Space/Enter/A/tap: next page     Up: RefreshContentManifest()");
        lines.emplace_back();
        lines.push_back("GetContentManifest() found " + std::to_string((int)manifest.size()) +
                        " asset name(s) under \"Content\".");
        lines.push_back("RefreshContentManifest() calls: " + std::to_string(refreshCount_) +
                        (refreshed_ ? "   (just rebuilt)" : ""));
        lines.emplace_back();

        if (manifest.empty()) {
            lines.push_back("(nothing found -- is the Content directory next to the executable?)");
        } else {
            constexpr int kPerPage = 9;
            const int pages = ((int)manifest.size() + kPerPage - 1) / kPerPage;
            const int page = pages > 0 ? page_ % pages : 0;
            lines.push_back("Page " + std::to_string(page + 1) + "/" + std::to_string(pages) +
                            "    name   [xnb][cnj] native extensions");
            for (int i = page * kPerPage;
                 i < (int)manifest.size() && i < (page + 1) * kPerPage; ++i) {
                lines.push_back("  " + Describe(manifest[(std::size_t)i]));
            }
        }

        lines.emplace_back();
        const auto usage = manager_->GetXnbReaderUsageSummary();
        lines.push_back("GetXnbReaderUsageSummary(): " + std::to_string((int)usage.size()) +
                        " distinct .xnb reader(s) referenced by the content on disk");
        for (int i = 0; i < (int)usage.size() && i < 3; ++i) {
            lines.push_back("  " + ShortReaderName(usage[(std::size_t)i].readerName) + "   " +
                            (usage[(std::size_t)i].isRegistered ? "registered"
                                                                : "NOT registered -- would throw"));
        }
        if (usage.size() > 3) lines.push_back("  ... and " + std::to_string((int)usage.size() - 3) + " more");
        lines.emplace_back();
        lines.push_back("The reader summary is the useful half: it says up front which .xnb");
        lines.push_back("types this build could actually load, instead of finding out at Load<T>.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    using Entry = Microsoft::Xna::Framework::Content::ContentManifestEntry;

    static std::string Describe(const Entry& entry) {
        std::string text = entry.relativePath;
        if (text.size() > 34) text = "..." + text.substr(text.size() - 31);
        text += std::string(entry.hasXnb ? "  [xnb]" : "  [   ]");
        text += std::string(entry.hasCnj ? "[cnj]" : "[   ]");
        for (const auto& extension : entry.nativeExtensions) text += " " + extension;
        return text;
    }

    // Reader names are assembly-qualified .NET type names and run to hundreds of
    // characters; the leading type name is the part that identifies them.
    static std::string ShortReaderName(const std::string& name) {
        const std::size_t comma = name.find(',');
        std::string shortened = comma == std::string::npos ? name : name.substr(0, comma);
        const std::size_t lastDot = shortened.find_last_of('.');
        return lastDot == std::string::npos ? shortened : shortened.substr(lastDot + 1);
    }

    std::optional<Microsoft::Xna::Framework::Content::ContentManager> manager_;
    int page_ = 0;
    int refreshCount_ = 0;
    bool refreshed_ = false;
};

} // namespace CnaExamples::Demos::Content::ManifestDemos
