// SPDX-License-Identifier: MIT
#pragma once

#include <memory>

#include "GameStateManagement/MenuScreen.hpp"
#include "Navigation/AreaCatalog.hpp"
#include "Navigation/CategoryScreen.hpp"

namespace CnaExamples::Navigation {

using namespace CnaExamples::GameStateManagement;

// Optional extra navigation level between AreaScreen and CategoryScreen,
// used only by Areas with enough categories to need it (see GroupEntry's
// own doc comment). Otherwise identical in structure to AreaScreen/
// CategoryScreen -- lists GroupEntry::categories, navigating into a
// CategoryScreen on selection.
class GroupScreen : public MenuScreen {
public:
    explicit GroupScreen(GroupEntry group)
        : MenuScreen(group.title), group_(std::move(group)) {
        if (group_.categories.empty()) {
            MenuEntries().push_back(std::make_shared<MenuEntry>("(coming soon)"));
        }
        for (auto& category : group_.categories) {
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
    GroupEntry group_;
};

} // namespace CnaExamples::Navigation
