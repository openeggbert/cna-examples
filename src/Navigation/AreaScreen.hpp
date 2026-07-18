// SPDX-License-Identifier: MIT
#pragma once

#include <memory>

#include "GameStateManagement/MenuScreen.hpp"
#include "Navigation/AreaCatalog.hpp"
#include "Navigation/CategoryScreen.hpp"

namespace CnaExamples::Navigation {

using namespace CnaExamples::GameStateManagement;

// Second-level menu: lists the categories within one Area (e.g. Input ->
// {Keyboard, Mouse, Gamepad, Touch, Other}).
class AreaScreen : public MenuScreen {
public:
    explicit AreaScreen(AreaEntry area)
        : MenuScreen(area.title), area_(std::move(area)) {
        if (area_.categories.empty()) {
            // Non-interactive placeholder -- see CategoryScreen.hpp for the
            // same pattern. This Area has no categories registered yet.
            MenuEntries().push_back(std::make_shared<MenuEntry>("(coming soon)"));
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
