// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "CNA/Internal/Xnb/XnbBuiltInReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/Content/ContentDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Content::XnbDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Real .xnb files, produced by MonoGame's content pipeline and loaded through
// CNA's own .xnb reader -- the binary format XNA shipped, complete with its
// LZX-compressed variant.
//
// These fixtures are NOT part of this repository. They are copied out of
// ../cna/tests/assets at build time, because they are Ms-PL while this repo is
// MIT, and because CNA consumes .xnb without ever writing it, so unlike every
// other asset here they cannot be generated locally. A checkout without ../cna
// has none, and this screen reports that rather than throwing.
//
// FontCalibri14.xnb is excluded even from the copy: it embeds a rasterised
// Calibri glyph atlas, a proprietary typeface.
//
// One thing this screen exists to make unmissable: .xnb support is NOT on by
// default. CNA does not auto-register its built-in readers, deliberately -- so
// a fresh ContentManager throws "references an unregistered .xnb content type
// reader 'Texture2DReader'" on the very first load. A real game calls
// CNA::Internal::Xnb::RegisterAllBuiltInXnbReaders() once at startup. The
// screen shows the registration state before and after doing exactly that,
// because the error message alone does not tell you what to call.
class XnbFixturesScreen : public DemoScreen {
public:
    XnbFixturesScreen() : DemoScreen("Content: XNB Fixtures") {}

    void LoadContent() override {
        // Recorded BEFORE registering, so the screen can show the transition
        // rather than only the end state.
        readerRegisteredBefore_ =
            Microsoft::Xna::Framework::Content::ContentTypeReaderManager::IsRegistered(kReaderName);

        // The step a real game performs once at startup. Idempotent.
        CNA::Internal::Xnb::RegisterAllBuiltInXnbReaders();

        readerRegisteredAfter_ =
            Microsoft::Xna::Framework::Content::ContentTypeReaderManager::IsRegistered(kReaderName);

        manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
        manager_->setRootDirectoryProperty("Content");
        // The service provider alone is not enough: CNA's ContentManager needs
        // its GraphicsDevice set explicitly before any texture load, exactly as
        // CNA's own .xnb tests do. Without it every Load<Texture2D> fails.
        manager_->setGraphicsDevice(GetScreenManager()->getGraphicsDeviceProperty());
        Load();
    }

    void UnloadContent() override {
        texture_.reset();
        manager_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            fixtureIndex_ = (fixtureIndex_ + 1) % kFixtureCount;
            Load();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!XnbFixturesAvailable()) {
            lines.push_back(MissingFixturesMessage());
            lines.emplace_back();
            lines.push_back("These are real MonoGame-produced .xnb files, copied from");
            lines.push_back("../cna/tests/assets/xnb at build time rather than committed here:");
            lines.push_back("  - they are Ms-PL, and this repository is MIT;");
            lines.push_back("  - CNA reads .xnb but never writes it, so they cannot be generated");
            lines.push_back("    locally the way every other asset in this app is.");
            lines.emplace_back();
            lines.push_back("Build with ../cna checked out alongside this repo and they appear.");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Space/Enter/A/tap: next fixture");
        lines.emplace_back();
        lines.push_back("Texture2DReader registered on entry: " +
                        std::string(readerRegisteredBefore_ ? "yes" : "NO"));
        lines.push_back("...after RegisterAllBuiltInXnbReaders(): " +
                        std::string(readerRegisteredAfter_ ? "yes" : "no"));
        lines.push_back("Without that call every .xnb load throws \"unregistered content type");
        lines.push_back("reader\" -- CNA does not turn .xnb support on by itself.");
        lines.emplace_back();
        lines.push_back("Fixture: " + std::string(kFixtures[fixtureIndex_].label));
        lines.push_back("Asset:   " + std::string(kFixtures[fixtureIndex_].assetName));
        lines.push_back("On disk: " + FileSize(kFixtures[fixtureIndex_].assetName));
        lines.emplace_back();
        for (const auto& line : result_) lines.push_back(line);
        lines.emplace_back();
        lines.push_back("The LZX fixture matters: .xnb content is often block-compressed, and");
        lines.push_back("decompressing it is a whole subsystem rather than a header flag. CNA's");
        lines.push_back("decoder was differentially tested against FNA's own LzxDecoder.cs.");
        lines.emplace_back();
        lines.push_back("Not shipped: FontCalibri14.xnb, the LZX SpriteFont fixture -- it embeds");
        lines.push_back("a rasterised Calibri atlas, which is a proprietary Microsoft typeface.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        if (texture_.has_value()) DrawPreview(sb);
    }

private:
    struct Fixture {
        const char* label;
        const char* assetName;   // relative to RootDirectory, no extension
    };

    static constexpr int kFixtureCount = 3;
    static constexpr Fixture kFixtures[kFixtureCount] = {
        {"uncompressed Texture2D",       "ContentDemo/xnb/monogame/windows/uncompressed/white-1"},
        {"uncompressed DXT1 TextureCube","ContentDemo/xnb/monogame/windows/uncompressed/SampleCube64DXT1Mips"},
        {"LZX-compressed Texture2D",     "ContentDemo/xnb/monogame/windows/lzx/Explosion"},
    };

    static std::string FileSize(const std::string& assetName) {
        std::error_code ec;
        const std::filesystem::path path =
            std::filesystem::path("Content") / (assetName + ".xnb");
        if (!std::filesystem::exists(path, ec) || ec) return "(not found)";
        return FormatBytes(std::filesystem::file_size(path, ec));
    }

    void Load() {
        texture_.reset();
        result_.clear();
        if (!manager_ || !XnbFixturesAvailable()) return;

        try {
            texture_.emplace(manager_->Load<Texture2D>(kFixtures[fixtureIndex_].assetName));
            result_.push_back("Loaded: " + std::to_string(texture_->getWidthProperty()) + " x " +
                              std::to_string(texture_->getHeightProperty()) + " Texture2D");
            result_.push_back("Every byte of that came out of the .xnb container: header parse,");
            result_.push_back("type-reader table, then the Texture2D reader itself.");
        } catch (const std::exception& ex) {
            // A cube map is not a Texture2D; that failure is real and worth showing.
            result_.push_back("Load<Texture2D> threw:");
            result_.push_back("  " + std::string(ex.what()));
            result_.push_back("Not every .xnb holds a Texture2D -- the cube-map fixture is a");
            result_.push_back("TextureCube, and asking for the wrong type is a genuine error.");
        }
    }

    void DrawPreview(SpriteBatch& sb) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const int box = 150;
        const int x = viewport.getWidthProperty() - box - 40;
        const int y = 110;
        const float alpha = TransitionAlpha();

        FillRect(sb, Rectangle(x - 3, y - 3, box + 6, box + 6), mul(Color(70, 70, 70), alpha));
        sb.Draw(*texture_, Rectangle(x, y, box, box), mul(Color::White, alpha));
    }

    std::optional<Microsoft::Xna::Framework::Content::ContentManager> manager_;
    std::optional<Texture2D> texture_;
    std::vector<std::string> result_;
    static constexpr const char* kReaderName = "Microsoft.Xna.Framework.Content.Texture2DReader";

    int fixtureIndex_ = 0;
    bool readerRegisteredBefore_ = false;
    bool readerRegisteredAfter_ = false;
};

} // namespace CnaExamples::Demos::Content::XnbDemos
