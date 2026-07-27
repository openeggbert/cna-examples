// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Input/TextInputEXT.hpp"
#include "System/MulticastAction.hpp"

#include "Demos/DemoScreen.hpp"
#include "GameStateManagement/ScreenManager.hpp"
#include "Harness/DemoIndex.hpp"
#include "Navigation/AreaCatalog.hpp"

namespace CnaExamples::Navigation {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Input::TextInputEXT;
using SharpRuntime::charcs;

// Free-text search across every demo in the catalog -- the equivalent of
// javafx-ensemble8's SearchPopover, and the difference between a 174-screen
// catalog being browsable and being navigable.
//
// Matching runs over the demo's whole catalog path, its description and its
// `apis` list, so "FromStream", "occlusion" and "Media/Pictures" all find
// something. Every term must match (AND), which is what makes narrowing by
// typing more words behave the way people expect.
//
// This is not a MenuScreen: MenuScreen owns Up/Down/Select over a fixed entry
// list built at construction, whereas the result list here is rebuilt on every
// keystroke. Reusing it would have meant rebuilding MenuEntry objects per frame.
class SearchScreen : public GameScreen {
public:
    // `initialQuery` lets the search be deep-linked into (the headless
    // `--search` flag). It is also the only way to exercise the filter without
    // a keyboard, since typing arrives through TextInputEXT's SDL event rather
    // than through anything the scripted-input path can reach.
    explicit SearchScreen(std::string initialQuery = {})
        // The index holds pointers into `catalog_`, so the catalog is a member
        // and outlives it. Binding it to a temporary is a deleted overload --
        // see Harness::FlattenCatalog.
        : catalog_(BuildAreaCatalog()), index_(Harness::FlattenCatalog(catalog_)),
          query_(std::move(initialQuery)) {
        setTransitionOnTime(TimeSpan::FromSeconds(0.2));
        setTransitionOffTime(TimeSpan::FromSeconds(0.2));
        Refilter();
    }

    void LoadContent() override {
        TextInputEXT::StartTextInput();
        subscription_ = TextInputEXT::TextInput.Add([this](charcs c) { OnChar(c); });
    }

    void UnloadContent() override {
        TextInputEXT::TextInput.Remove(subscription_);
        TextInputEXT::StopTextInput();
    }

    void HandleInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuCancel(ControllingPlayer(), playerIndex)) {
            ExitScreen();
            return;
        }

        if (!matches_.empty()) {
            if (input.IsMenuUp(ControllingPlayer())) {
                selected_ = (selected_ + (int)matches_.size() - 1) % (int)matches_.size();
            }
            if (input.IsMenuDown(ControllingPlayer())) {
                selected_ = (selected_ + 1) % (int)matches_.size();
            }
        }

        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            Launch(playerIndex);
            return;
        }

        // Tap/click a result row to run it.
        Vector2 point;
        if (input.IsNewTap(point) || input.IsNewClick(point)) {
            const int row = RowAtPoint(point);
            if (row >= 0) {
                selected_ = row;
                Launch(PlayerIndex::One);
            }
        }
    }

    void Draw(const GameTime&) override {
        ScreenManager& sm = *GetScreenManager();
        SpriteBatch& sb = sm.getSpriteBatch();
        SpriteFont& font = sm.getFont();
        auto& viewport = sm.getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();

        sb.Begin();

        sb.DrawString(font, "Search", Vector2(16.0f, 20.0f), mul(Color(192, 192, 192), alpha));

        // The caret makes it obvious the field has focus even before anything
        // is typed -- there is no other visual cue on a screen this sparse.
        const std::string prompt = "> " + query_ + "_";
        sb.DrawString(font, prompt, Vector2(16.0f, 64.0f), mul(Color::White, alpha));

        const std::string summary =
            query_.empty()
                ? std::to_string((int)index_.size()) + " demos -- type to filter"
                : std::to_string((int)matches_.size()) + " of " +
                      std::to_string((int)index_.size()) + " match";
        sb.DrawString(font, summary, Vector2(16.0f, 106.0f), mul(Color(140, 140, 140), alpha));

        const float rowStep = (float)font.getLineSpacingProperty() + 8.0f;
        const int visible = VisibleRows(viewport.getHeightProperty(), rowStep);
        const int start = WindowStart(visible);

        for (int i = start; i < (int)matches_.size() && i < start + visible; ++i) {
            const auto& row = *matches_[(std::size_t)i];
            const bool isSelected = (i == selected_);
            const std::string text = (isSelected ? "> " : "  ") + row.path;
            sb.DrawString(font,
                          Demos::DemoScreen::Ellipsize(font, text,
                                                       (float)viewport.getWidthProperty() - 32.0f),
                          Vector2(16.0f, kResultsTop + (float)(i - start) * rowStep),
                          mul(isSelected ? Color::Yellow : Color::White, alpha));
        }

        if (matches_.empty() && !query_.empty()) {
            sb.DrawString(font, "(nothing matches)", Vector2(16.0f, kResultsTop),
                          mul(Color(140, 140, 140), alpha));
        }

        sb.DrawString(font, "Up/Down select   Enter run   Esc back",
                      Vector2(16.0f, (float)viewport.getHeightProperty() - 40.0f),
                      mul(Color(150, 150, 150), alpha));

        sb.End();
    }

private:
    static constexpr float kResultsTop = 152.0f;

    // Matching itself lives in Harness/DemoIndex.hpp so that `--list-demos
    // --search <q>` filters through exactly this code path. A second copy here
    // would let the UI and the scriptable form drift apart silently.
    void Refilter() {
        matches_.clear();
        const auto terms = Harness::ParseQueryTerms(query_);
        for (const auto& row : index_) {
            if (Harness::MatchesTerms(row, terms)) matches_.push_back(&row);
        }
        if (selected_ >= (int)matches_.size()) selected_ = 0;
    }

    void OnChar(charcs c) {
        if (c == u'\b') {
            if (!query_.empty()) query_.pop_back();
            Refilter();
            return;
        }
        if (c == u'\r' || c == u'\n') return;   // Enter runs the selection; handled in HandleInput
        if (c >= 0x20 && c < 0x7F) {
            query_ += (char)c;
            Refilter();
        }
    }

    void Launch(PlayerIndex playerIndex) {
        if (matches_.empty()) return;
        const auto& row = *matches_[(std::size_t)selected_];
        if (!row.demo->create) return;

        auto screen = row.demo->create();
        if (auto* demoScreen = dynamic_cast<Demos::DemoScreen*>(screen.get())) {
            demoScreen->SetBreadcrumb(row.Breadcrumb());
        }
        GetScreenManager()->AddScreen(std::move(screen), playerIndex);
    }

    int VisibleRows(int viewportHeight, float rowStep) const {
        const float room = (float)viewportHeight - kResultsTop - 56.0f;
        return std::max(1, (int)(room / rowStep));
    }

    int WindowStart(int visible) const {
        const int count = (int)matches_.size();
        if (count <= visible) return 0;
        int start = selected_ - visible / 2;
        start = std::max(0, start);
        return std::min(start, count - visible);
    }

    int RowAtPoint(const Vector2& point) const {
        auto& sm = *GetScreenManager();
        auto& viewport = sm.getGraphicsDeviceProperty().getViewportProperty();
        const float rowStep = (float)sm.getFont().getLineSpacingProperty() + 8.0f;
        const int visible = VisibleRows(viewport.getHeightProperty(), rowStep);
        const int start = WindowStart(visible);

        if (point.Y < kResultsTop) return -1;
        const int offset = (int)((point.Y - kResultsTop) / rowStep);
        if (offset < 0 || offset >= visible) return -1;
        const int row = start + offset;
        return row < (int)matches_.size() ? row : -1;
    }

    // Declaration order matters: these are initialised in this order by the
    // constructor's member-init list (catalog_ before index_, which points into
    // it; query_ before Refilter() reads it).
    std::vector<AreaEntry> catalog_;
    std::vector<Harness::IndexedDemo> index_;
    std::string query_;
    std::vector<const Harness::IndexedDemo*> matches_;
    int selected_ = 0;
    System::MulticastAction<charcs>::Token subscription_ =
        System::MulticastAction<charcs>::InvalidToken;
};

} // namespace CnaExamples::Navigation
