// SPDX-License-Identifier: MIT
#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "CNA/Internal/CnjEnvelope.hpp"

#include "Demos/Content/ContentDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Content::CnjDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// `.cnj` is CNA's own content format: plain JSON with a three-field envelope
// that every document shares, whatever its type.
//
//   cnjVersion   strictly 1. Anything else is rejected outright rather than
//                best-effort parsed -- a versioned format that silently accepts
//                the wrong version is not versioned.
//   type         which reader should handle the rest of the document.
//   sourceFile   optional. Points at a real file (a .png, a .wav) that this
//                document describes, which is what lets .cnj act as a metadata
//                sidecar rather than only as a replacement format.
//
// Every envelope below is parsed live from a real file on disk by CNA's own
// ParseCnjEnvelope, including a deliberately-invalid one, so the rejection is
// demonstrated rather than described.
class CnjEnvelopeScreen : public DemoScreen {
public:
    CnjEnvelopeScreen() : DemoScreen("Content: CNJ Envelope") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            fileIndex_ = (fileIndex_ + 1) % kFileCount;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const auto& entry = kFiles[fileIndex_];
        const std::string json = ReadAll(entry.path);

        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: next .cnj document");
        lines.emplace_back();
        lines.push_back("File: " + std::string(entry.path));
        lines.push_back("      " + std::string(entry.note));
        lines.emplace_back();

        if (json.empty()) {
            lines.push_back("(file not found next to the executable)");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        const auto envelope = CNA::Internal::ParseCnjEnvelope(json);
        lines.push_back("ParseCnjEnvelope() ->");
        lines.push_back("  cnjVersion: " + std::to_string(envelope.cnjVersion));
        lines.push_back("  type:       \"" + envelope.type + "\"");
        lines.push_back("  sourceFile: " +
                        (envelope.sourceFile.empty() ? std::string("(none)")
                                                     : "\"" + envelope.sourceFile + "\""));
        if (!envelope.parseErrorDetail.empty()) {
            lines.push_back("  parse error: " + envelope.parseErrorDetail);
        }
        lines.emplace_back();
        lines.push_back(envelope.cnjVersion == 1
            ? "cnjVersion is 1, so this document would be accepted."
            : "cnjVersion is not 1 -- ContentManager rejects this outright. A format that");
        if (envelope.cnjVersion != 1) {
            lines.push_back("silently accepted an unknown version would not be versioned at all.");
        }
        lines.emplace_back();
        lines.push_back("Raw document:");
        for (const auto& line : FirstLines(json, 8)) lines.push_back("  " + line);

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    struct FileEntry { const char* path; const char* note; };

    static constexpr int kFileCount = 3;
    static constexpr FileEntry kFiles[kFileCount] = {
        {"Content/menufont.cnj",
         "this app's own menu font -- a SpriteFont described in JSON"},
        {"Content/ContentDemo/cnj/goblin.cnj",
         "a game-defined type with no built-in reader (see Custom Loaders)"},
        {"Content/ContentDemo/cnj/bad_version.cnj",
         "deliberately invalid: cnjVersion 99"},
    };

    static std::string ReadAll(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return {};
        std::ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // The menu font's document is hundreds of glyph entries long; only the head
    // of it is the envelope, which is what this screen is about.
    static std::vector<std::string> FirstLines(const std::string& text, int count) {
        std::vector<std::string> lines;
        std::istringstream stream(text);
        std::string line;
        while ((int)lines.size() < count && std::getline(stream, line)) {
            if (line.size() > 68) line = line.substr(0, 68) + "...";
            lines.push_back(line);
        }
        if (!stream.eof()) lines.push_back("...");
        return lines;
    }

    int fileIndex_ = 0;
};

} // namespace CnaExamples::Demos::Content::CnjDemos
