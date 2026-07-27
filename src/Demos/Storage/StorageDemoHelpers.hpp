// SPDX-License-Identifier: MIT
#pragma once

#include <cstdio>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageContainer.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"

namespace CnaExamples::Demos::Storage {

using Microsoft::Xna::Framework::Storage::StorageContainer;
using Microsoft::Xna::Framework::Storage::StorageDevice;

// Every Storage demo writes to a container named after this app, under the
// per-OS save-game root that StorageDevice resolves. Nothing is written into
// the repository or next to the executable -- the whole point of the Storage
// API is that the platform decides where saves live.
//
// SetAppNameEXT is a CNA extension (real XNA derived the folder from the
// assembly's title). Setting it explicitly keeps this app's saves out of
// whatever default the runtime would otherwise pick, and makes the location
// predictable enough to display on screen.
inline constexpr const char* kAppName = "cna-examples";
inline constexpr const char* kContainerName = "StorageDemo";

// Applied once, on first use, and never reverted: unlike the timestep or the
// window title, an app name is not something another demo could be surprised by.
inline void EnsureAppName() {
    static bool applied = false;
    if (applied) return;
    StorageDevice::SetAppNameEXT(kAppName);
    applied = true;
}

// XNA's storage API is "fake async": BeginXxx completes synchronously and the
// paired EndXxx extracts the result. It looked asynchronous because the Xbox 360
// could show a device-selection UI, but nothing here ever blocks or defers.
//
// Returns nullptr if no device could be opened, which callers must handle --
// on a console a player really could decline the selector.
inline std::unique_ptr<StorageDevice> OpenDevice(
    Microsoft::Xna::Framework::PlayerIndex player) {
    EnsureAppName();
    auto asyncResult = StorageDevice::BeginShowSelector(player, nullptr, nullptr);
    if (!asyncResult) return nullptr;
    return StorageDevice::EndShowSelector(asyncResult.get());
}

inline std::unique_ptr<StorageContainer> OpenContainer(StorageDevice& device,
                                                       const std::string& name) {
    auto asyncResult = device.BeginOpenContainer(name, nullptr, nullptr);
    if (!asyncResult) return nullptr;
    return device.EndOpenContainer(asyncResult.get());
}

inline std::string FormatBytes(long long bytes) {
    if (bytes < 0) return "(unknown)";
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double value = (double)bytes;
    int unit = 0;
    while (value >= 1024.0 && unit < 4) {
        value /= 1024.0;
        unit++;
    }
    char buf[48];
    std::snprintf(buf, sizeof(buf), "%.1f %s", value, units[unit]);
    return buf;
}

} // namespace CnaExamples::Demos::Storage
