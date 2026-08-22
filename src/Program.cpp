// SPDX-License-Identifier: MIT
#include <cstdio>

#include "CNA/Platform/Entrypoint.hpp"
#include "CnaExamplesGame.hpp"
#include "Harness/CommandLine.hpp"
#include "Harness/DemoIndex.hpp"

int main(int argc, char** argv) {
    const CnaExamples::Harness::Options options =
        CnaExamples::Harness::ParseCommandLine(argc, argv);

    if (!options.error.empty()) {
        std::fprintf(stderr, "%s\n\n%s", options.error.c_str(), CnaExamples::Harness::kUsage);
        return 2;
    }
    if (options.showHelp) {
        std::fputs(CnaExamples::Harness::kUsage, stdout);
        return 0;
    }

    // Listing needs the catalog but no graphics device, so it runs before any
    // Game is constructed -- which is what makes it usable from a build script.
    if (options.listDemos) {
        const auto catalog = CnaExamples::Navigation::BuildAreaCatalog();
        // Combining --list-demos with --search prints exactly what the search
        // screen would show for that query. That is what makes the filter
        // assertable from a shell instead of only inspectable in a screenshot.
        const auto terms = CnaExamples::Harness::ParseQueryTerms(options.searchQuery);
        for (const auto& row : CnaExamples::Harness::FlattenCatalog(catalog)) {
            if (CnaExamples::Harness::MatchesTerms(row, terms)) {
                std::printf("%s\n", row.path.c_str());
            }
        }
        return 0;
    }

#if defined(__EMSCRIPTEN__)
    // Game::Run() registers Emscripten's asynchronous main loop by unwinding
    // this stack frame. The game therefore must outlive main(), rather than
    // being a local whose destructor cancels the loop before the first Draw.
    auto* game = new CnaExamples::CnaExamplesGame(options);
    if (game->StartupFailed()) {
        delete game;
        return 2;
    }
    game->Run();
#else
    CnaExamples::CnaExamplesGame game(options);
    if (game.StartupFailed()) return 2;
    game.Run();
#endif
    return 0;
}
