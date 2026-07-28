// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <string>
#include <vector>

#include "SharpRuntime/SharpRuntimeHelper.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Storage/StorageDemoHelpers.hpp"

namespace CnaExamples::Demos::Storage::ContainerDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// Directory operations, and the one thing they do NOT do: recurse.
//
// CreateDirectory/GetDirectoryNames/GetFileNames all look like a small
// filesystem API, so it is easy to assume GetFileNames() would find a file
// nested inside a subdirectory created with CreateDirectory(). It does not --
// both listing methods only ever look at this container's OWN root, never
// descending into subdirectories. A file written to "notes/todo.txt" is real
// (CreateFile happily accepts a nested relative path) and is genuinely
// invisible to GetFileNames() at the root.
//
// DeleteDirectory is also stricter than it looks: it refuses a non-empty
// directory by throwing, unlike StorageDevice::DeleteContainer (see the
// Container Lifetime screen) which removes an entire tree at once.
//
// Runs its checks once at load and reports a verdict, the same as any other
// screen in this app that makes a checkable claim -- state left over from a
// prior run would otherwise make this misleading rather than just wrong.
class DirectoriesAndFilesScreen : public DemoScreen {
public:
    DirectoriesAndFilesScreen() : DemoScreen("Storage: Directories & Files") {}

    void OnDemoLoad() override { RunChecks(); }

    void OnDemoUnload() override { container_.reset(); device_.reset(); }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        if (!container_) {
            lines.push_back("No StorageContainer could be opened.");
            for (const auto& line : log_) lines.push_back("  " + line);
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);
            return;
        }

        lines.push_back("notes/ + empty_dir/ created; readme.txt (root), notes/todo.txt, notes/idea.txt written.");
        lines.emplace_back();
        lines.push_back("GetDirectoryNames()          -> " + Join(allDirs_));
        lines.push_back("GetDirectoryNames(\"empty*\") -> " + Join(globDirs_));
        lines.push_back("GetFileNames() at ROOT       -> " + Join(rootFiles_));
        lines.push_back("GetFileNames(\"*.txt\") ROOT   -> " + Join(globFiles_));
        lines.push_back("^ neither lists notes/todo.txt or notes/idea.txt: listing does NOT recurse.");
        lines.emplace_back();
        lines.push_back("DeleteDirectory(\"notes\") while non-empty: " + deleteNonEmptyResult_);
        lines.push_back("Deleting both files first, then DeleteDirectory(\"notes\") again: succeeds.");
        for (const auto& line : log_)
            if (line != "Checks complete.") lines.push_back("  " + line);
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        DrawVerdict(sb, font, end.Y + 6.0f,
                    mul(allHold_ ? Color(40, 200, 90, 255) : Color(220, 60, 60, 255),
                        TransitionAlpha()),
                    tint,
                    allHold_ ? std::to_string(checks_) + " claims verified live."
                             : "A claim did NOT hold: " + failure_);
    }

private:
    static constexpr const char* kDir = "notes";
    static constexpr const char* kEmptyDir = "empty_dir";
    static constexpr const char* kRootFile = "readme.txt";
    static constexpr const char* kNestedA = "notes/todo.txt";
    static constexpr const char* kNestedB = "notes/idea.txt";

    void Note(bool ok, const std::string& what) {
        ++checks_;
        if (!ok && allHold_) { allHold_ = false; failure_ = what; }
    }

    static void WriteMarker(Microsoft::Xna::Framework::Storage::StorageContainer& c,
                            const std::string& path) {
        auto stream = c.CreateFile(path);
        const std::string payload = "x";
        stream->Write(reinterpret_cast<const SharpRuntime::bytecs*>(payload.data()), 0,
                     (SharpRuntime::intcs)payload.size());
        stream->Flush();
    }

    static std::string Join(const std::vector<std::string>& names) {
        if (names.empty()) return "(none)";
        std::string out;
        for (std::size_t i = 0; i < names.size(); ++i) {
            if (i) out += ", ";
            out += names[i];
        }
        return out;
    }

    void RunChecks() {
        log_.clear();
        device_ = OpenDevice(PlayerIndex::One);
        if (!device_) { log_.emplace_back("EndShowSelector returned nullptr"); return; }
        container_ = OpenContainer(*device_, kDirsContainerName);
        if (!container_) { log_.emplace_back("EndOpenContainer returned nullptr"); return; }

        try {
            // Leftover from a prior run (a previous crash mid-demo, say) would
            // otherwise make DeleteDirectory's "throws on non-empty" check
            // below misleading -- start from a known-empty state.
            CleanUp();

            container_->CreateDirectory(kDir);
            container_->CreateDirectory(kDir);   // idempotent: no throw the 2nd time
            container_->CreateDirectory(kEmptyDir);
            Note(container_->DirectoryExists(kDir), "notes should exist after CreateDirectory");
            Note(container_->DirectoryExists(kEmptyDir), "empty_dir should exist");

            WriteMarker(*container_, kRootFile);
            WriteMarker(*container_, kNestedA);
            WriteMarker(*container_, kNestedB);
            Note(container_->FileExists(kNestedA), "notes/todo.txt should really exist");
            Note(container_->FileExists(kNestedB), "notes/idea.txt should really exist");

            allDirs_ = container_->GetDirectoryNames();
            globDirs_ = container_->GetDirectoryNames("empty*");
            rootFiles_ = container_->GetFileNames();
            globFiles_ = container_->GetFileNames("*.txt");

            Note(Contains(allDirs_, kDir) && Contains(allDirs_, kEmptyDir),
                "GetDirectoryNames() should list both directories");
            Note(globDirs_.size() == 1 && Contains(globDirs_, kEmptyDir),
                "GetDirectoryNames(\"empty*\") should match only empty_dir");
            Note(Contains(rootFiles_, kRootFile), "readme.txt should be listed at root");
            Note(!Contains(rootFiles_, "todo.txt") && !Contains(rootFiles_, "idea.txt"),
                "root listing should NOT contain nested files -- GetFileNames does not recurse");
            Note(globFiles_.size() == rootFiles_.size(), "*.txt should match every root file here");

            try {
                container_->DeleteDirectory(kDir);
                deleteNonEmptyResult_ = "did NOT throw (unexpected)";
                Note(false, "DeleteDirectory on a non-empty directory should throw");
            } catch (const std::exception&) {
                deleteNonEmptyResult_ = "threw, as it should -- not empty yet";
                Note(true, "");
            }

            container_->DeleteFile(kNestedA);
            container_->DeleteFile(kNestedB);
            container_->DeleteDirectory(kDir);   // now empty: succeeds
            Note(!container_->DirectoryExists(kDir), "notes should be gone after cleanup");

            CleanUp();
            log_.emplace_back("Checks complete.");
        } catch (const std::exception& ex) {
            log_.emplace_back(std::string("threw: ") + ex.what());
            Note(false, ex.what());
        }
    }

    void CleanUp() {
        if (!container_) return;
        if (container_->FileExists(kRootFile)) container_->DeleteFile(kRootFile);
        if (container_->FileExists(kNestedA)) container_->DeleteFile(kNestedA);
        if (container_->FileExists(kNestedB)) container_->DeleteFile(kNestedB);
        if (container_->DirectoryExists(kDir)) container_->DeleteDirectory(kDir);
        if (container_->DirectoryExists(kEmptyDir)) container_->DeleteDirectory(kEmptyDir);
    }

    static bool Contains(const std::vector<std::string>& v, const std::string& s) {
        for (const auto& e : v) if (e == s) return true;
        return false;
    }

    std::unique_ptr<Microsoft::Xna::Framework::Storage::StorageDevice> device_;
    std::unique_ptr<Microsoft::Xna::Framework::Storage::StorageContainer> container_;
    std::vector<std::string> log_;
    std::vector<std::string> allDirs_, globDirs_, rootFiles_, globFiles_;
    std::string deleteNonEmptyResult_ = "?";
    std::string failure_;
    int checks_ = 0;
    bool allHold_ = true;
};

} // namespace CnaExamples::Demos::Storage::ContainerDemos
