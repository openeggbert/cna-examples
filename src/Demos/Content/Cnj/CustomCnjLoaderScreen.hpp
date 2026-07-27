// SPDX-License-Identifier: MIT
#pragma once

#include <exception>
#include <optional>
#include <string>
#include <vector>

#include "CNA/Internal/Json.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"

#include "Demos/Content/ContentDemoHelpers.hpp"
#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Content::CnjDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;

// A game-defined asset type with no built-in reader anywhere in CNA. This is
// ordinary application data -- exactly the case RegisterCnjLoader exists for.
struct GameplayRecord {
    std::string documentType;
    std::string displayName;
    int hitPoints = 0;
    float speed = 0.0f;
    std::vector<std::string> tags;
};

// RegisterCnjLoader<T> lets an application teach ContentManager to load its own
// data types from `.cnj`, with no C++ reader class to write and no build step.
//
// The part that distinguishes it from RegisterTypeReader<T> is dispatch by the
// DOCUMENT's "type" field rather than by T alone: several differently-named
// `.cnj` types can each register their own factory and all produce the same C++
// type. This screen loads two -- "EnemyDefinition" and "LootTable" -- into one
// GameplayRecord struct, which is the whole feature in one screen.
//
// Its registration rules fail fast rather than silently, and all three are
// exercised live below: an empty type name is rejected, an empty factory is
// rejected, and re-registering the same (T, typeName) pair throws instead of
// quietly replacing the earlier factory. Two *different* names for one T remain
// supported -- only an exact repeat is an error.
class CustomCnjLoaderScreen : public DemoScreen {
public:
    CustomCnjLoaderScreen() : DemoScreen("Content: Custom CNJ Loaders") {}

    void OnDemoLoad() override {
        manager_.emplace(&GetScreenManager()->getGameProperty().getServicesProperty());
        manager_->setRootDirectoryProperty("Content");
        manager_->setGraphicsDevice(GetScreenManager()->getGraphicsDeviceProperty());

        RegisterLoaders();
        LoadBoth();
        ExerciseGuards();
    }

    void OnDemoUnload() override { manager_.reset(); }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Two .cnj documents with DIFFERENT \"type\" values, one C++ struct.");
        lines.emplace_back();
        lines.push_back("Registered loaders for GameplayRecord:");
        lines.push_back("  \"EnemyDefinition\"  and  \"LootTable\"");
        lines.emplace_back();
        for (const auto& line : loaded_) lines.push_back(line);
        lines.emplace_back();
        lines.push_back("Registration guards, each called for real:");
        for (const auto& line : guards_) lines.push_back("  " + line);
        lines.emplace_back();
        lines.push_back("RegisterTypeReader<T> binds one reader to one C++ type. RegisterCnjLoader");
        lines.push_back("dispatches on the document's own \"type\" instead, so a game can add new");
        lines.push_back("data kinds without a new C++ type or a content-pipeline build step.");
        lines.emplace_back();
        lines.push_back("It only applies to a T with no reader already registered -- registering");
        lines.push_back("one for a type that has a reader throws, since it could never be reached.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    using Manager = Microsoft::Xna::Framework::Content::ContentManager;

    // Both named types deserialise into the same struct; only the "type" field
    // and a couple of defaults differ, which is the point being made.
    static GameplayRecord ParseRecord(const std::string& json, const std::string& documentType) {
        GameplayRecord record;
        record.documentType = documentType;

        const auto document = CNA::Internal::ParseJson(json);
        if (const auto* name = document.FindMember("displayName")) record.displayName = name->stringValue;
        if (const auto* hp = document.FindMember("hitPoints")) record.hitPoints = (int)hp->numberValue;
        if (const auto* speed = document.FindMember("speed")) record.speed = (float)speed->numberValue;
        if (const auto* tags = document.FindMember("tags")) {
            for (const auto& tag : tags->arrayValue) record.tags.push_back(tag.stringValue);
        }
        return record;
    }

    void RegisterLoaders() {
        manager_->RegisterCnjLoader<GameplayRecord>(
            "EnemyDefinition",
            [](const std::string& json, Manager&) { return ParseRecord(json, "EnemyDefinition"); });
        manager_->RegisterCnjLoader<GameplayRecord>(
            "LootTable",
            [](const std::string& json, Manager&) { return ParseRecord(json, "LootTable"); });
    }

    void LoadBoth() {
        Load("ContentDemo/cnj/goblin");
        Load("ContentDemo/cnj/cave_loot");
    }

    void Load(const std::string& assetName) {
        try {
            const GameplayRecord record = manager_->Load<GameplayRecord>(assetName);
            std::string tags;
            for (const auto& tag : record.tags) {
                if (!tags.empty()) tags += ", ";
                tags += tag;
            }
            loaded_.push_back(assetName + "  ->  type \"" + record.documentType + "\"");
            loaded_.push_back("    " + record.displayName + "   hp " +
                              std::to_string(record.hitPoints) + "   speed " +
                              std::to_string(record.speed).substr(0, 4));
            loaded_.push_back("    tags: " + (tags.empty() ? std::string("(none)") : tags));
        } catch (const std::exception& ex) {
            loaded_.push_back(assetName + "  ->  threw: " + std::string(ex.what()));
        }
    }

    void ExerciseGuards() {
        guards_.push_back(Guard("empty type name", [this] {
            manager_->RegisterCnjLoader<GameplayRecord>(
                "", [](const std::string&, Manager&) { return GameplayRecord{}; });
        }));
        guards_.push_back(Guard("empty factory", [this] {
            manager_->RegisterCnjLoader<GameplayRecord>(
                "SomethingElse", Manager::CnjLoaderFn<GameplayRecord>{});
        }));
        guards_.push_back(Guard("duplicate (T, \"EnemyDefinition\")", [this] {
            manager_->RegisterCnjLoader<GameplayRecord>(
                "EnemyDefinition",
                [](const std::string&, Manager&) { return GameplayRecord{}; });
        }));
    }

    template <typename Fn>
    static std::string Guard(const std::string& label, Fn&& attempt) {
        try {
            attempt();
            return label + ": accepted (unexpected -- it should have thrown)";
        } catch (const std::exception& ex) {
            std::string what = ex.what();
            if (what.size() > 44) what = what.substr(0, 44) + "...";
            return label + ": rejected -- " + what;
        }
    }

    std::optional<Manager> manager_;
    std::vector<std::string> loaded_;
    std::vector<std::string> guards_;
};

} // namespace CnaExamples::Demos::Content::CnjDemos
