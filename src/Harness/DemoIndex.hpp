// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Navigation/AreaCatalog.hpp"

namespace CnaExamples::Harness {

using Navigation::AreaEntry;
using Navigation::DemoEntry;

// One flat, addressable row per demo in the catalog, so a demo can be named on
// the command line ("Media/MediaLibrary/Song Metadata") instead of reached by
// counting menu keystrokes. The same flattening is what a search feature needs,
// so it lives here rather than inside the driver.
struct IndexedDemo {
    std::string path;      // "Area/Category/Demo" or "Area/Group/Category/Demo"
    std::string area;
    std::string group;     // empty when the Area has no Group level
    std::string category;
    std::string title;
    const DemoEntry* demo = nullptr;
};

inline std::vector<IndexedDemo> FlattenCatalog(const std::vector<AreaEntry>& areas) {
    std::vector<IndexedDemo> out;

    auto addCategory = [&out](const AreaEntry& area, const std::string& groupTitle,
                              const Navigation::CategoryEntry& category) {
        for (const auto& demo : category.demos) {
            IndexedDemo row;
            row.area     = area.title;
            row.group    = groupTitle;
            row.category = category.title;
            row.title    = demo.title;
            row.path     = area.title + "/" +
                           (groupTitle.empty() ? "" : groupTitle + "/") +
                           category.title + "/" + demo.title;
            row.demo     = &demo;
            out.push_back(std::move(row));
        }
    };

    for (const auto& area : areas) {
        for (const auto& category : area.categories) {
            addCategory(area, "", category);
        }
        for (const auto& group : area.groups) {
            for (const auto& category : group.categories) {
                addCategory(area, group.title, category);
            }
        }
    }
    return out;
}

// Case-insensitive exact match on the full path first, then a unique
// case-insensitive substring match, so "Song Metadata" resolves without having
// to type the whole path. Returns nullptr when nothing matches or when a
// substring is ambiguous -- never a silent first-match guess.
inline const IndexedDemo* FindDemo(const std::vector<IndexedDemo>& index,
                                   const std::string& query,
                                   std::string& errorOut) {
    auto lower = [](std::string s) {
        for (char& c : s) c = (char)std::tolower((unsigned char)c);
        return s;
    };
    const std::string q = lower(query);

    for (const auto& row : index) {
        if (lower(row.path) == q) return &row;
    }

    const IndexedDemo* match = nullptr;
    int matches = 0;
    for (const auto& row : index) {
        if (lower(row.path).find(q) != std::string::npos) {
            match = &row;
            matches++;
        }
    }
    if (matches == 1) return match;
    if (matches == 0) {
        errorOut = "no demo matches '" + query + "'";
    } else {
        errorOut = std::to_string(matches) + " demos match '" + query + "' -- be more specific";
    }
    return nullptr;
}

} // namespace CnaExamples::Harness
