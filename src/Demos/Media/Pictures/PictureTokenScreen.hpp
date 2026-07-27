// SPDX-License-Identifier: MIT
#pragma once

#include <chrono>
#include <ctime>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Media/Picture.hpp"
#include "Microsoft/Xna/Framework/Media/PictureCollection.hpp"

#include "Demos/Media/LibraryDemoScreen.hpp"

namespace CnaExamples::Demos::Media::PictureDemos {

using namespace CnaExamples::Demos::Media;
using Microsoft::Xna::Framework::Media::Picture;

// A "token" is XNA's opaque handle for a picture that outlives a MediaLibrary
// instance: you keep the string, and later ask a library to hand the Picture
// back. In CNA the token is the file's path (exposed as the NOXNA getTokenEXT()),
// but the round trip below only ever uses it as an opaque string, which is how
// consumer code should treat it.
//
// The identity members are worth showing together because they do not all mean
// the same thing: GetPictureFromToken returns the *same object* the collection
// holds (pointer equality), while Equals compares by value and would also match
// a different object describing the same picture.
class PictureTokenScreen : public LibraryDemoScreen {
public:
    PictureTokenScreen() : LibraryDemoScreen("Pictures: Tokens & Identity") {}

protected:
    int ItemCount() const override {
        return Library() != nullptr ? (int)Library()->getPicturesProperty()->getCountProperty() : 0;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;

        if (!LoadError().empty()) {
            lines.push_back("MediaLibrary failed to build: " + LoadError());
            DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        const int count = ItemCount();
        lines.push_back("Up/Down: select a picture   (" + std::to_string(count) + " indexed)");
        lines.emplace_back();

        if (count == 0) {
            lines.push_back("(no pictures found)");
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        auto* pictures = Library()->getPicturesProperty();
        const int visible = 5;
        const int start = WindowStart(visible);
        for (int i = start; i < count && i < start + visible; ++i) {
            lines.push_back(RowPrefix(i, SelectedIndex()) + (*pictures)[i]->getNameProperty());
        }

        Picture* picture = (*pictures)[SelectedIndex()];
        const std::string token = picture->getTokenEXT();
        Picture* roundTripped = Library()->GetPictureFromToken(token);

        lines.emplace_back();
        lines.push_back("Token:            " + token);
        lines.push_back("GetPictureFromToken(token):");
        lines.push_back("  returned:       " + (roundTripped ? roundTripped->getNameProperty()
                                                             : std::string("(nullptr)")));
        lines.push_back("  same object as Pictures[i]: " +
                        std::string(roundTripped == picture ? "yes (pointer equality)" : "no"));
        lines.push_back("  Equals(Pictures[i]):        " +
                        std::string(roundTripped && roundTripped->Equals(picture) ? "true" : "false"));
        lines.emplace_back();
        lines.push_back("GetPictureFromToken(\"no/such/picture\"): " +
                        std::string(Library()->GetPictureFromToken("no/such/picture") == nullptr
                                        ? "nullptr (no throw)" : "non-null (unexpected)"));
        lines.emplace_back();
        lines.push_back("Name:        " + picture->getNameProperty());
        lines.push_back("Date:        " + FormatDate(picture->getDateProperty()) +
                        "  (the file's own timestamp)");
        lines.push_back("GetHashCode: " + std::to_string(picture->GetHashCode()));
        lines.push_back("ToString:    " + picture->ToString());
        lines.push_back("IsDisposed:  " + std::string(picture->getIsDisposedProperty() ? "true" : "false"));
        lines.emplace_back();
        lines.push_back("Equals against the next picture in the collection: " +
                        std::string(count > 1 && picture->Equals((*pictures)[(SelectedIndex() + 1) % count])
                                        ? "true (unexpected)" : "false"));

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static std::string FormatDate(std::chrono::system_clock::time_point tp) {
        const std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm{};
#if defined(_WIN32)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
        return buf;
    }
};

} // namespace CnaExamples::Demos::Media::PictureDemos
