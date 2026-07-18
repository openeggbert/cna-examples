// SPDX-License-Identifier: MIT
#pragma once

#include <memory>

#include "GameStateManagement/MenuScreen.hpp"
#include "Navigation/AreaCatalog.hpp"

namespace CnaExamples::Navigation {

using namespace CnaExamples::GameStateManagement;

// Third-level menu: lists the demos within one Category (e.g. Input ->
// Keyboard -> {list of keyboard demos}). Empty until real demo screens are
// registered in AreaCatalog.hpp -- see plan.md section 8.
class CategoryScreen : public MenuScreen {
public:
    explicit CategoryScreen(CategoryEntry category)
        : MenuScreen(category.title), category_(std::move(category)) {
        if (category_.demos.empty()) {
            // Non-interactive placeholder -- no Selected handler, so
            // pressing/tapping it is a harmless no-op. Real demos replace
            // this entry as they are registered in AreaCatalog.hpp.
            MenuEntries().push_back(std::make_shared<MenuEntry>("(coming soon)"));
        }
        for (auto& demo : category_.demos) {
            auto entry = std::make_shared<MenuEntry>(demo.title);
            const auto& factory = demo.create;
            entry->Selected = [this, factory](PlayerIndex p) {
                if (factory)
                    GetScreenManager()->AddScreen(factory(), p);
            };
            MenuEntries().push_back(entry);
        }

        auto back = std::make_shared<MenuEntry>("Back");
        back->Selected = [this](PlayerIndex p) { OnCancel(p); };
        MenuEntries().push_back(back);
    }

private:
    CategoryEntry category_;
};

} // namespace CnaExamples::Navigation
