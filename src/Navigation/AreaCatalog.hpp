// SPDX-License-Identifier: MIT
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "GameStateManagement/GameScreen.hpp"

namespace CnaExamples::Navigation {

using CnaExamples::GameStateManagement::GameScreen;

// A single runnable demonstration within a Category. `create` is a factory
// so building the catalog never constructs a screen (and its content) until
// the user actually navigates to it.
struct DemoEntry {
    std::string title;
    std::string description;
    std::function<std::shared_ptr<GameScreen>()> create;
};

// A group of related demos within an Area (e.g. Input's "Keyboard" category).
struct CategoryEntry {
    std::string title;
    std::vector<DemoEntry> demos;
};

// A top-level CNA subsystem shown on the Home screen (e.g. "Input").
struct AreaEntry {
    std::string title;
    std::vector<CategoryEntry> categories;
};

// Builds the full Home -> Area -> Category -> Demo data set. This is the
// single place new areas/categories/demos get registered as they are
// implemented; see plan.md section 8 for what is intentionally still empty.
inline std::vector<AreaEntry> BuildAreaCatalog() {
    return {
        AreaEntry{"Input", {
            CategoryEntry{"Keyboard", {}},
            CategoryEntry{"Mouse", {}},
            CategoryEntry{"Gamepad", {}},
            CategoryEntry{"Touch", {}},
            CategoryEntry{"Other", {}},
        }},
        AreaEntry{"Audio", {}},
        AreaEntry{"Devices", {}},
        AreaEntry{"Net", {}},
        AreaEntry{"Media", {}},
        AreaEntry{"2D Graphics", {}},
        AreaEntry{"3D Graphics", {}},
    };
}

} // namespace CnaExamples::Navigation
