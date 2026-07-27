// SPDX-License-Identifier: MIT
//
// Asserts what the Content area's CNJ screens state on screen.
//
// Those screens catch their own exceptions and print them, so a total failure
// to load still renders a clean-looking screenshot and passes a sweep -- the
// same trap that hid two real bugs in the XNB screen. This checks the claims
// directly instead.
//
// Build and run (needs the app's own build tree for includes and libraries):
//
//   FLAGS=$(grep -m1 CXX_INCLUDES build/CMakeFiles/cna_examples.dir/flags.make | sed 's/^CXX_INCLUDES = //')
//   DEFS=$(grep -m1 CXX_DEFINES  build/CMakeFiles/cna_examples.dir/flags.make | sed 's/^CXX_DEFINES = //')
//   LINK=$(sed 's|^/usr/bin/c++ .*-o cna_examples||' build/CMakeFiles/cna_examples.dir/link.txt)
//   (cd build && g++ -std=c++23 $DEFS $FLAGS ../tools/checks/cnj_claims.cpp -o /tmp/cnj_claims $LINK)
//   (cd build && /tmp/cnj_claims)      # must run where Content/ lives

#include <cstdio>
#include <string>
#include <vector>

#include "CNA/Internal/CnjEnvelope.hpp"
#include "CNA/Internal/Json.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"

using Microsoft::Xna::Framework::Content::ContentManager;

namespace {

int fails = 0;

void check(const char* what, bool ok) {
    std::printf("  %-58s %s\n", what, ok ? "ok" : "FAIL");
    if (!ok) fails++;
}

// The same game-defined type the Custom Loaders screen uses.
struct GameplayRecord {
    std::string documentType;
    std::string displayName;
    int hitPoints = 0;
    std::vector<std::string> tags;
};

GameplayRecord ParseRecord(const std::string& json, const std::string& documentType) {
    GameplayRecord record;
    record.documentType = documentType;
    const auto document = CNA::Internal::ParseJson(json);
    if (const auto* name = document.FindMember("displayName")) record.displayName = name->stringValue;
    if (const auto* hp = document.FindMember("hitPoints")) record.hitPoints = (int)hp->numberValue;
    if (const auto* tags = document.FindMember("tags")) {
        for (const auto& tag : tags->arrayValue) record.tags.push_back(tag.stringValue);
    }
    return record;
}

template <typename Fn>
bool Throws(Fn&& fn) {
    try { fn(); return false; } catch (...) { return true; }
}

} // namespace

int main() {
    // No service provider and no GraphicsDevice: this asset type needs neither,
    // which is itself part of the claim -- .cnj game data is not a GPU resource.
    ContentManager cm(nullptr, "Content");

    cm.RegisterCnjLoader<GameplayRecord>(
        "EnemyDefinition",
        [](const std::string& json, ContentManager&) { return ParseRecord(json, "EnemyDefinition"); });
    cm.RegisterCnjLoader<GameplayRecord>(
        "LootTable",
        [](const std::string& json, ContentManager&) { return ParseRecord(json, "LootTable"); });

    // The feature itself: two differently-named .cnj types, one C++ type.
    bool loadedEnemy = false, loadedLoot = false;
    GameplayRecord enemy, loot;
    try { enemy = cm.Load<GameplayRecord>("ContentDemo/cnj/goblin"); loadedEnemy = true; }
    catch (const std::exception& e) { std::printf("  goblin load threw: %s\n", e.what()); }
    try { loot = cm.Load<GameplayRecord>("ContentDemo/cnj/cave_loot"); loadedLoot = true; }
    catch (const std::exception& e) { std::printf("  cave_loot load threw: %s\n", e.what()); }

    check("goblin.cnj loads through the EnemyDefinition loader", loadedEnemy);
    check("cave_loot.cnj loads through the LootTable loader", loadedLoot);
    check("the two documents dispatched to DIFFERENT loaders",
          loadedEnemy && loadedLoot && enemy.documentType == "EnemyDefinition" &&
              loot.documentType == "LootTable");
    check("field data really came out of the JSON",
          loadedEnemy && enemy.displayName == "Goblin Scout" && enemy.hitPoints == 24 &&
              enemy.tags.size() == 2);

    // Fail-fast registration rules the screen also claims.
    check("empty type name is rejected", Throws([&] {
        cm.RegisterCnjLoader<GameplayRecord>(
            "", [](const std::string&, ContentManager&) { return GameplayRecord{}; });
    }));
    check("empty factory is rejected", Throws([&] {
        cm.RegisterCnjLoader<GameplayRecord>(
            "Unused", ContentManager::CnjLoaderFn<GameplayRecord>{});
    }));
    check("re-registering the same (T, typeName) is rejected", Throws([&] {
        cm.RegisterCnjLoader<GameplayRecord>(
            "EnemyDefinition",
            [](const std::string&, ContentManager&) { return GameplayRecord{}; });
    }));

    // The envelope claims from the Envelope screen.
    const auto good = CNA::Internal::ParseCnjEnvelope(
        R"({"cnjVersion":1,"type":"EnemyDefinition"})");
    check("ParseCnjEnvelope reads cnjVersion and type",
          good.cnjVersion == 1 && good.type == "EnemyDefinition");

    const auto future = CNA::Internal::ParseCnjEnvelope(
        R"({"cnjVersion":99,"type":"EnemyDefinition"})");
    check("a non-1 cnjVersion is reported, not normalised", future.cnjVersion == 99);

    check("a cnjVersion 99 document is refused by Load<T>", Throws([&] {
        (void)cm.Load<GameplayRecord>("ContentDemo/cnj/bad_version");
    }));

    std::printf("\n%s (%d failure(s))\n", fails ? "FAILURES" : "all CNJ claims hold", fails);
    return fails ? 1 : 0;
}
