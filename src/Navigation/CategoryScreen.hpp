// SPDX-License-Identifier: MIT
#pragma once

#include <memory>
#include <string>
#include <utility>

#include "Demos/DemoScreen.hpp"
#include "GameStateManagement/MenuScreen.hpp"
#include "Navigation/AreaCatalog.hpp"

namespace CnaExamples::Navigation {

using namespace CnaExamples::GameStateManagement;

// Third-level menu: lists the demos within one Category (e.g. Input ->
// Keyboard -> {list of keyboard demos}). Empty until real demo screens are
// registered in AreaCatalog.hpp -- see plan.md section 8.
class CategoryScreen : public MenuScreen {
public:
    // `pathPrefix` is the breadcrumb trail leading here ("Media", or
    // "2D Graphics > SpriteBatch"), threaded down from AreaScreen/GroupScreen.
    // This is the level that finally knows a demo's whole path, so it is the
    // level that stamps the breadcrumb onto the screen it pushes.
    explicit CategoryScreen(CategoryEntry category, std::string pathPrefix = {})
        : MenuScreen(category.title), category_(std::move(category)),
          pathPrefix_(std::move(pathPrefix)) {
        if (category_.demos.empty()) {
            // Non-interactive placeholder -- no Selected handler, so
            // pressing/tapping it is a harmless no-op. Real demos replace
            // this entry as they are registered in AreaCatalog.hpp.
            MenuEntries().push_back(std::make_shared<MenuEntry>("(coming soon)"));
        }
        const std::string categoryPath =
            pathPrefix_.empty() ? category_.title : pathPrefix_ + " > " + category_.title;
        for (auto& demo : category_.demos) {
            auto entry = std::make_shared<MenuEntry>(demo.title);
            const auto& factory = demo.create;
            const std::string breadcrumb = categoryPath + " > " + demo.title;
            entry->Selected = [this, factory, breadcrumb](PlayerIndex p) {
                if (!factory) return;
                auto screen = factory();
                if (auto* demoScreen = dynamic_cast<Demos::DemoScreen*>(screen.get())) {
                    demoScreen->SetBreadcrumb(breadcrumb);
                }
                GetScreenManager()->AddScreen(std::move(screen), p);
            };
            MenuEntries().push_back(entry);
        }

        auto back = std::make_shared<MenuEntry>("Back");
        back->Selected = [this](PlayerIndex p) { OnCancel(p); };
        MenuEntries().push_back(back);
    }

private:
    CategoryEntry category_;
    std::string pathPrefix_;
};

} // namespace CnaExamples::Navigation
