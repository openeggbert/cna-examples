// SPDX-License-Identifier: MIT
#pragma once

#include <memory>

#include "GameStateManagement/MenuScreen.hpp"
#include "Navigation/AreaCatalog.hpp"
#include "Navigation/CategoryScreen.hpp"
#include "Navigation/GroupScreen.hpp"

namespace CnaExamples::Navigation {

using namespace CnaExamples::GameStateManagement;

// Second-level menu: lists either the Groups or the Categories within one
// Area, whichever AreaEntry actually populated (see GroupEntry's own doc
// comment for which Areas use which) -- e.g. Input -> {Keyboard, Mouse,
// Gamepad, Touch, Other} (categories, no group level), or
// 2D Graphics -> {SpriteBatch, Textures & Fonts, ...} (groups).
class AreaScreen : public MenuScreen {
public:
    explicit AreaScreen(AreaEntry area)
        : MenuScreen(area.title), area_(std::move(area)) {
        if (area_.groups.empty() && area_.categories.empty()) {
            // Non-interactive placeholder -- see CategoryScreen.hpp for the
            // same pattern. This Area has no categories/groups registered yet.
            MenuEntries().push_back(std::make_shared<MenuEntry>("(coming soon)"));
        }
        for (auto& group : area_.groups) {
            auto entry = std::make_shared<MenuEntry>(group.title);
            GroupEntry groupCopy = group;
            entry->Selected = [this, groupCopy](PlayerIndex p) {
                GetScreenManager()->AddScreen(
                    std::make_shared<GroupScreen>(groupCopy), p);
            };
            MenuEntries().push_back(entry);
        }
        for (auto& category : area_.categories) {
            auto entry = std::make_shared<MenuEntry>(category.title);
            CategoryEntry categoryCopy = category;
            entry->Selected = [this, categoryCopy](PlayerIndex p) {
                GetScreenManager()->AddScreen(
                    std::make_shared<CategoryScreen>(categoryCopy), p);
            };
            MenuEntries().push_back(entry);
        }

        auto back = std::make_shared<MenuEntry>("Back");
        back->Selected = [this](PlayerIndex p) { OnCancel(p); };
        MenuEntries().push_back(back);
    }

private:
    AreaEntry area_;
};

} // namespace CnaExamples::Navigation
