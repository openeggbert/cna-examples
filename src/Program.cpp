// SPDX-License-Identifier: MIT
#include <cstdio>

#include "CNA/Entrypoint.hpp"
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
        for (const auto& row : CnaExamples::Harness::FlattenCatalog(
                 CnaExamples::Navigation::BuildAreaCatalog())) {
            std::printf("%s\n", row.path.c_str());
        }
        return 0;
    }

    CnaExamples::CnaExamplesGame game(options);
    if (game.StartupFailed()) return 2;
    game.Run();
    return 0;
}
