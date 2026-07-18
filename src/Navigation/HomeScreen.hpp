// SPDX-License-Identifier: MIT
#pragma once

#include <memory>

#include "GameStateManagement/MenuScreen.hpp"
#include "Navigation/AreaCatalog.hpp"
#include "Navigation/AreaScreen.hpp"

namespace CnaExamples::Navigation {

using namespace CnaExamples::GameStateManagement;

// Top-level menu: one entry per CNA Area (Input, Audio, Devices, Net, Media,
// 2D Graphics, 3D Graphics, ...). Selecting one pushes an AreaScreen.
// Cancelling here exits the application (there is nothing above Home).
class HomeScreen : public MenuScreen {
public:
    HomeScreen() : MenuScreen("CNA Examples") {
        for (auto& area : BuildAreaCatalog()) {
            auto entry = std::make_shared<MenuEntry>(area.title);
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
