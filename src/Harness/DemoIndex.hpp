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

    // The same "Area > Group > Category > Demo" heading CategoryScreen stamps
    // onto a screen it pushes. Built from the same parts as `path`, which uses
    // "/" because it is what the user types on the command line.
    [[nodiscard]] std::string Breadcrumb() const {
        return area + (group.empty() ? "" : " > " + group) + " > " + category + " > " + title;
    }
};

// IndexedDemo::demo points INTO `areas`, so the caller must keep the catalog
// alive for as long as the index is used. Binding to a temporary
// (`FlattenCatalog(BuildAreaCatalog())`) leaves every entry dangling the moment
// the full expression ends, so that spelling is deleted below rather than left
// as a runtime trap -- it was one, and it crashed every --demo launch with
// std::bad_array_new_length as soon as MakeDemo's factory started capturing
// state.
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

std::vector<IndexedDemo> FlattenCatalog(std::vector<AreaEntry>&&) = delete;

// ---------------------------------------------------------------------------
// Query matching
// ---------------------------------------------------------------------------
// Shared by SearchScreen and by `--list-demos --search`, so what the UI shows
// and what a script can assert are the same function rather than two
// implementations that agree until one of them is changed.

inline std::string LowerAscii(std::string s) {
    for (char& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

// Everything a demo can be found by: its catalog path, its description, and the
// APIs it declares it exercises.
inline std::string HaystackFor(const IndexedDemo& row) {
    std::string haystack = row.path + " " + row.demo->description;
    for (const auto& api : row.demo->apis) haystack += " " + api;
    return LowerAscii(haystack);
}

// Whitespace-separated terms, all of which must match (AND). Typing more words
// therefore narrows the result set, which is what people expect from a filter.
inline std::vector<std::string> ParseQueryTerms(const std::string& query) {
    std::vector<std::string> terms;
    const std::string lowered = LowerAscii(query);
    std::size_t pos = 0;
    while (pos < lowered.size()) {
        const std::size_t space = lowered.find(' ', pos);
        const std::string term =
            lowered.substr(pos, space == std::string::npos ? std::string::npos : space - pos);
        if (!term.empty()) terms.push_back(term);
        if (space == std::string::npos) break;
        pos = space + 1;
    }
    return terms;
}

inline bool MatchesTerms(const IndexedDemo& row, const std::vector<std::string>& terms) {
    if (terms.empty()) return true;
    const std::string haystack = HaystackFor(row);
    for (const auto& term : terms) {
        if (haystack.find(term) == std::string::npos) return false;
    }
    return true;
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
