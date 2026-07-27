// SPDX-License-Identifier: MIT
#pragma once

#include <filesystem>
#include <string>

namespace CnaExamples::Demos::Content {

// The .xnb fixtures this area loads are NOT part of this repository. They are
// real MonoGame-produced files under ../cna/tests/assets/xnb, copied into the
// build output by cmake/ExamplesHelpers.cmake at build time.
//
// Two reasons they are borrowed rather than committed:
//   - They are Ms-PL; cna-examples is MIT and states that it ships no ported
//     Microsoft content.
//   - CNA consumes .xnb and never writes it, so unlike every other asset here
//     they cannot be generated locally.
//
// FontCalibri14.xnb is excluded outright even from the copy: it embeds a
// rasterised Calibri glyph atlas, a proprietary Microsoft typeface.
//
// A checkout without ../cna therefore has no fixtures, and every demo in this
// category must say so on screen rather than throw.

inline constexpr const char* kXnbRoot = "Content/ContentDemo/xnb/monogame/windows";

// Asset names are given to ContentManager relative to its RootDirectory
// ("Content"), so they deliberately omit the leading "Content/".
inline constexpr const char* kAssetRoot = "ContentDemo/xnb/monogame/windows";

inline bool XnbFixturesAvailable() {
    std::error_code ec;
    return std::filesystem::exists(kXnbRoot, ec);
}

inline std::string MissingFixturesMessage() {
    return std::string("No .xnb fixtures found under ") + kXnbRoot + ".";
}

// Human-readable byte count for the file listings these demos print.
inline std::string FormatBytes(std::uintmax_t bytes) {
    if (bytes < 1024) return std::to_string((unsigned long long)bytes) + " B";
    if (bytes < 1024 * 1024) {
        return std::to_string((unsigned long long)(bytes / 1024)) + " KB";
    }
    return std::to_string((unsigned long long)(bytes / (1024 * 1024))) + " MB";
}

} // namespace CnaExamples::Demos::Content
