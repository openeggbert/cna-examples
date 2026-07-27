// SPDX-License-Identifier: MIT
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Demos/DemoScreen.hpp"
#include "Demos/Storage/StorageDemoHelpers.hpp"

namespace CnaExamples::Demos::Storage::DeviceDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// StorageDevice is where a save game goes, and its API is XNA's "fake async"
// pattern: BeginShowSelector / EndShowSelector.
//
// That shape is a historical artifact worth understanding rather than
// imitating. On the Xbox 360 the selector really could show UI and really could
// take time -- the player might have several memory units, or none. Everywhere
// else it completes immediately, which CNA reports honestly through
// IAsyncResult::CompletedSynchronously.
//
// The screen calls it per PlayerIndex, because on a console each player had
// their own storage; on desktop all four resolve to the same place, and showing
// that is more useful than pretending otherwise.
class StorageDeviceScreen : public DemoScreen {
public:
    StorageDeviceScreen() : DemoScreen("Storage: StorageDevice") {}

    void LoadContent() override { Open(); }

    void UnloadContent() override { device_.reset(); }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            playerSlot_ = (playerSlot_ + 1) % 4;
            Open();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: select the device for the next PlayerIndex");
        lines.emplace_back();
        lines.push_back("BeginShowSelector(PlayerIndex::" + PlayerName(playerSlot_) + ", ...)");
        lines.push_back("  IAsyncResult::IsCompleted:            " +
                        std::string(completed_ ? "true" : "false"));
        lines.push_back("  IAsyncResult::CompletedSynchronously: " +
                        std::string(synchronous_ ? "true" : "false") +
                        (synchronous_ ? "   <- it never actually waits" : ""));
        lines.emplace_back();

        if (!device_) {
            lines.push_back("EndShowSelector returned nullptr -- no device available.");
            lines.push_back("A game must handle this: on a console the player can decline.");
        } else {
            lines.push_back("StorageDevice:");
            lines.push_back("  IsConnected: " +
                            std::string(device_->getIsConnectedProperty() ? "true" : "false"));
            lines.push_back("  FreeSpace:   " + FormatBytes(device_->getFreeSpaceProperty()));
            lines.push_back("  TotalSpace:  " + FormatBytes(device_->getTotalSpaceProperty()));
        }
        lines.emplace_back();
        lines.push_back("Where saves actually go (NOXNA extensions):");
        lines.push_back("  SetAppNameEXT(\"" + std::string(kAppName) + "\")");
        lines.push_back("  GetStorageRootEXT() = " +
                        Microsoft::Xna::Framework::Storage::StorageDevice::GetStorageRootEXT());
        lines.emplace_back();
        lines.push_back("Real XNA derived that folder from the assembly title and gave the");
        lines.push_back("application no way to ask where it was. CNA exposes both, which is what");
        lines.push_back("makes a save location debuggable instead of a guess.");
        lines.emplace_back();
        lines.push_back("On desktop every PlayerIndex resolves to the same place. On the 360 they");
        lines.push_back("did not -- each player could pick a different memory unit.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static std::string PlayerName(int slot) {
        static const char* names[] = {"One", "Two", "Three", "Four"};
        return names[slot % 4];
    }

    void Open() {
        device_.reset();
        EnsureAppName();

        const auto player = (PlayerIndex)playerSlot_;
        auto asyncResult =
            Microsoft::Xna::Framework::Storage::StorageDevice::BeginShowSelector(
                player, nullptr, nullptr);
        if (!asyncResult) {
            completed_ = false;
            synchronous_ = false;
            return;
        }
        // Read the flags before End*, which is the only point they are meaningful.
        completed_ = asyncResult->getIsCompletedProperty();
        synchronous_ = asyncResult->getCompletedSynchronouslyProperty();
        device_ = Microsoft::Xna::Framework::Storage::StorageDevice::EndShowSelector(
            asyncResult.get());
    }

    std::unique_ptr<Microsoft::Xna::Framework::Storage::StorageDevice> device_;
    int playerSlot_ = 0;
    bool completed_ = false;
    bool synchronous_ = false;
};

} // namespace CnaExamples::Demos::Storage::DeviceDemos
