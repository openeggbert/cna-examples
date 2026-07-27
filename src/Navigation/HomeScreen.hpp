// SPDX-License-Identifier: MIT
#pragma once

#include <memory>

#include "GameStateManagement/MenuScreen.hpp"
#include "Navigation/AreaCatalog.hpp"
#include "Navigation/AreaScreen.hpp"
#include "Navigation/SearchScreen.hpp"

namespace CnaExamples::Navigation {

using namespace CnaExamples::GameStateManagement;

// Top-level menu: one entry per CNA Area (Input, Audio, Devices, Net, Media,
// 2D Graphics, 3D Graphics, ...). Selecting one pushes an AreaScreen.
// Cancelling here exits the application (there is nothing above Home).
class HomeScreen : public MenuScreen {
public:
    HomeScreen() : MenuScreen("CNA Examples") {
        // First entry, above the areas: with ~174 demos five levels deep,
        // search is the fastest route to a named demo and should not be buried.
        auto search = std::make_shared<MenuEntry>("Search...");
        search->Selected = [this](PlayerIndex p) {
            GetScreenManager()->AddScreen(std::make_shared<SearchScreen>(), p);
        };
        MenuEntries().push_back(search);

        for (auto& area : BuildAreaCatalog()) {
            auto entry = std::make_shared<MenuEntry>(WithCount(area.title, CountDemos(area)));
            AreaEntry areaCopy = area;
            entry->Selected = [this, areaCopy](PlayerIndex p) {
                GetScreenManager()->AddScreen(
                    std::make_shared<AreaScreen>(areaCopy), p);
            };
            MenuEntries().push_back(entry);
        }
    }

protected:
    void OnCancel(PlayerIndex playerIndex) override {
        (void)playerIndex;
        GetScreenManager()->getGameProperty().Exit();
    }
};

} // namespace CnaExamples::Navigation
