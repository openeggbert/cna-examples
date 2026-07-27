// SPDX-License-Identifier: MIT
#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "GameStateManagement/InputState.hpp"

namespace CnaExamples::Harness {

using GameStateManagement::InputState;

// Options for the headless driver. Everything here exists to make one screen
// reachable and capturable without a window manager: the app normally has to be
// navigated by hand, which is fine for a human and useless for a verification
// sweep over ~290 screens.
struct Options {
    bool listDemos = false;                 // --list-demos
    std::string demoPath;                   // --demo <path>
    std::string searchQuery;                // --search <text>
    bool openSearch = false;                // set when --search was given
    std::string screenshotPath;             // --screenshot <file.png>
    int frames = 0;                         // --frames <n>; 0 = run until closed
    int keyInterval = 12;                   // --key-interval <n>
    std::vector<InputState::ScriptedAction> keys;   // --keys up,down,select,cancel
    // --pointer x,y1,y2 : press at (x,y1), drag to (x,y2) over kPointerFrames
    // frames, then release. Empty means "no scripted pointer".
    bool pointerScript = false;
    float pointerX = 0.0f, pointerFromY = 0.0f, pointerToY = 0.0f;
    bool showHelp = false;                  // --help
    std::string error;                      // set when parsing failed
};

inline const char* kUsage =
    "cna_examples -- browsable catalog of live CNA demonstrations\n"
    "\n"
    "Run with no arguments for the normal interactive app.\n"
    "\n"
    "Headless / verification options:\n"
    "  --list-demos            print every demo's path, one per line, and exit\n"
    "  --demo <path>           open straight into this demo, skipping the menus.\n"
    "                          Accepts a full \"Area/Category/Demo\" path or any\n"
    "                          unambiguous substring of one.\n"
    "  --search <text>         open the search screen with this query already typed\n"
    "  --keys <a,b,c>          scripted menu actions, one per --key-interval frames.\n"
    "                          Each is up, down, select or cancel.\n"
    "  --key-interval <n>      frames between scripted actions (default 12)\n"
    "  --frames <n>            exit after n drawn frames (default: run until closed,\n"
    "                          or 90 when --screenshot is given)\n"
    "  --pointer <x,y1,y2>     press at (x,y1), drag to (x,y2), release -- drives\n"
    "                          touch/mouse gestures such as drag-to-scroll\n"
    "  --screenshot <file>     save the back buffer as PNG on the final frame\n"
    "  --help                  show this text\n";

inline bool ParseScriptedAction(const std::string& name, InputState::ScriptedAction& out) {
    if (name == "up")     { out = InputState::ScriptedAction::Up;     return true; }
    if (name == "down")   { out = InputState::ScriptedAction::Down;   return true; }
    if (name == "select") { out = InputState::ScriptedAction::Select; return true; }
    if (name == "cancel") { out = InputState::ScriptedAction::Cancel; return true; }
    return false;
}

inline Options ParseCommandLine(int argc, char** argv) {
    Options options;

    auto needsValue = [&](int& i, const char* flag) -> const char* {
        if (i + 1 >= argc) {
            options.error = std::string(flag) + " needs a value";
            return nullptr;
        }
        return argv[++i];
    };

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--list-demos") {
            options.listDemos = true;
        } else if (arg == "--demo") {
            if (const char* v = needsValue(i, "--demo")) options.demoPath = v; else break;
        } else if (arg == "--search") {
            if (const char* v = needsValue(i, "--search")) {
                options.searchQuery = v;
                options.openSearch = true;
            } else break;
        } else if (arg == "--pointer") {
            const char* v = needsValue(i, "--pointer");
            if (v == nullptr) break;
            float x = 0.0f, y1 = 0.0f, y2 = 0.0f;
            if (std::sscanf(v, "%f,%f,%f", &x, &y1, &y2) != 3) {
                options.error = "--pointer expects x,y1,y2 (e.g. 480,500,220)";
                return options;
            }
            options.pointerScript = true;
            options.pointerX = x;
            options.pointerFromY = y1;
            options.pointerToY = y2;
        } else if (arg == "--screenshot") {
            if (const char* v = needsValue(i, "--screenshot")) options.screenshotPath = v; else break;
        } else if (arg == "--frames") {
            if (const char* v = needsValue(i, "--frames")) options.frames = std::atoi(v); else break;
        } else if (arg == "--key-interval") {
            if (const char* v = needsValue(i, "--key-interval")) options.keyInterval = std::atoi(v); else break;
        } else if (arg == "--keys") {
            const char* v = needsValue(i, "--keys");
            if (v == nullptr) break;
            std::string list = v;
            std::size_t start = 0;
            while (start <= list.size()) {
                const std::size_t comma = list.find(',', start);
                const std::string name = list.substr(start, comma == std::string::npos
                                                                ? std::string::npos
                                                                : comma - start);
                if (!name.empty()) {
                    InputState::ScriptedAction action;
                    if (!ParseScriptedAction(name, action)) {
                        options.error = "unknown key action '" + name +
                                        "' (expected up, down, select or cancel)";
                        return options;
                    }
                    options.keys.push_back(action);
                }
                if (comma == std::string::npos) break;
                start = comma + 1;
            }
        } else {
            options.error = "unknown option '" + arg + "'";
            return options;
        }
    }

    // A screenshot run has to end by itself or it never writes the file.
    if (!options.screenshotPath.empty() && options.frames <= 0) options.frames = 90;
    if (options.keyInterval <= 0) options.keyInterval = 1;
    return options;
}

} // namespace CnaExamples::Harness
