// SPDX-License-Identifier: MIT
#pragma once

#include <cstdlib>
#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "System/IO/FileAccess.hpp"
#include "System/IO/FileMode.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/IO/Stream.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Storage/StorageDemoHelpers.hpp"

namespace CnaExamples::Demos::Storage::ContainerDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// The complete save-game round trip: open a device, open a container, write a
// file, read it back, and see it survive.
//
// A StorageContainer is a sandboxed directory. Every path handed to it is
// relative and confined -- which is what lets a game write "save1.dat" without
// knowing or caring where that lands on the host platform.
//
// The save counter genuinely persists across runs: leaving this demo and
// returning re-reads the file, and so does restarting the application. That is
// the only way to demonstrate persistence honestly -- an in-memory counter
// would look identical on screen and prove nothing.
class SaveGameRoundTripScreen : public DemoScreen {
public:
    SaveGameRoundTripScreen() : DemoScreen("Storage: Save Game Round Trip") {}

    void LoadContent() override {
        Open();
        Read();
    }

    void UnloadContent() override {
        // The container holds an open handle to the save directory; drop it
        // before the device it borrows from.
        container_.reset();
        device_.reset();
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) Write();
        if (input.IsMenuUp(ControllingPlayer())) Delete();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: write a save     Up: delete it");
        lines.emplace_back();

        if (!container_) {
            lines.push_back("No StorageContainer could be opened.");
            for (const auto& line : log_) lines.push_back("  " + line);
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
            return;
        }

        lines.push_back("Container:   " + container_->getDisplayNameProperty());
        lines.push_back("File exists: " +
                        std::string(container_->FileExists(kSaveFile) ? "yes" : "no"));
        // ResolvePath is private, so the container will not tell an application
        // where a file actually landed. The storage root is the closest public
        // answer -- deliberately, since the sandbox is the point.
        lines.push_back("Storage root: " +
                        Shorten(Microsoft::Xna::Framework::Storage::StorageDevice::GetStorageRootEXT()));
        lines.emplace_back();
        lines.push_back("Contents read back from disk:");
        if (contents_.empty()) {
            lines.push_back("  (no save file yet -- press Space to write one)");
        } else {
            lines.push_back("  \"" + contents_ + "\"");
        }
        lines.emplace_back();
        lines.push_back("Files in this container: " + std::to_string(fileCount_));
        for (const auto& line : log_) lines.push_back("  " + line);
        lines.emplace_back();
        lines.push_back("This really persists. Leave and come back, or restart the app entirely --");
        lines.push_back("the counter carries on from where it was. An in-memory value would look");
        lines.push_back("exactly the same on screen and would demonstrate nothing.");
        lines.emplace_back();
        lines.push_back("Paths inside a container are relative and sandboxed, which is how a game");
        lines.push_back("writes \"save1.dat\" without knowing where the platform puts it.");
        lines.push_back("StorageContainer::ResolvePath is private for exactly that reason: an");
        lines.push_back("application is not meant to reason about the absolute path at all.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr const char* kSaveFile = "save1.dat";

    void Open() {
        log_.clear();
        device_ = OpenDevice(PlayerIndex::One);
        if (!device_) {
            log_.emplace_back("EndShowSelector returned nullptr");
            return;
        }
        container_ = OpenContainer(*device_, kContainerName);
        if (!container_) log_.emplace_back("EndOpenContainer returned nullptr");
    }

    void Write() {
        if (!container_) return;
        log_.clear();
        try {
            const int next = saveCounter_ + 1;
            const std::string payload =
                "cna-examples save #" + std::to_string(next);

            // CreateFile truncates an existing file, which is what a save wants.
            auto stream = container_->CreateFile(kSaveFile);
            if (!stream) {
                log_.emplace_back("CreateFile returned nullptr");
                return;
            }
            stream->Write(reinterpret_cast<const SharpRuntime::bytecs*>(payload.data()), 0,
                          (SharpRuntime::intcs)payload.size());
            stream->Flush();
            stream.reset();   // close before reading back

            saveCounter_ = next;
            log_.emplace_back("CreateFile + Write + Flush + close");
            Read();
        } catch (const std::exception& ex) {
            log_.emplace_back(std::string("write threw: ") + ex.what());
        }
    }

    void Read() {
        contents_.clear();
        fileCount_ = 0;
        if (!container_) return;
        try {
            fileCount_ = (int)container_->GetFileNames().size();
            if (!container_->FileExists(kSaveFile)) return;

            auto stream = container_->OpenFile(kSaveFile, System::IO::FileMode::Open,
                                               System::IO::FileAccess::Read);
            if (!stream) return;

            std::string text;
            SharpRuntime::bytecs buffer[256];
            for (;;) {
                const auto read = stream->Read(buffer, 0, (SharpRuntime::intcs)sizeof(buffer));
                if (read <= 0) break;
                text.append(reinterpret_cast<const char*>(buffer), (std::size_t)read);
            }
            contents_ = text;

            // Recover the counter from the file, so a restart continues rather
            // than restarting at 1 -- the persistence claim depends on this.
            const std::size_t hash = text.rfind('#');
            if (hash != std::string::npos) {
                try { saveCounter_ = std::stoi(text.substr(hash + 1)); } catch (...) {}
            }
        } catch (const std::exception& ex) {
            log_.emplace_back(std::string("read threw: ") + ex.what());
        }
    }

    void Delete() {
        if (!container_) return;
        log_.clear();
        try {
            if (container_->FileExists(kSaveFile)) {
                container_->DeleteFile(kSaveFile);
                log_.emplace_back("DeleteFile(\"" + std::string(kSaveFile) + "\")");
                saveCounter_ = 0;
            } else {
                log_.emplace_back("nothing to delete");
            }
            Read();
        } catch (const std::exception& ex) {
            log_.emplace_back(std::string("delete threw: ") + ex.what());
        }
    }

    static std::string Shorten(const std::string& path) {
        if (const char* home = std::getenv("HOME")) {
            const std::string h = home;
            if (!h.empty() && path.rfind(h, 0) == 0) return "~" + path.substr(h.size());
        }
        return path;
    }

    std::unique_ptr<Microsoft::Xna::Framework::Storage::StorageDevice> device_;
    std::unique_ptr<Microsoft::Xna::Framework::Storage::StorageContainer> container_;
    std::vector<std::string> log_;
    std::string contents_;
    int fileCount_ = 0;
    int saveCounter_ = 0;
};

} // namespace CnaExamples::Demos::Storage::ContainerDemos
