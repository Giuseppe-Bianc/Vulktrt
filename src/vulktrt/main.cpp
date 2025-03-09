// NOLINTBEGIN(*-include-cleaner)
#include <CLI/CLI.hpp>
#include <Vulktrt/vulktrt.hpp>

// This file will be generated automatically when cur_you run the CMake
// configuration step. It creates a namespace called `Vulktrt`. You can modify
// the source template at `configured_files/config.hpp.in`.
#include <internal_use_only/config.hpp>

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, const char **argv) {
    try {
        CLI::App app{FORMAT("{} version {}", Vulktrt::cmake::project_name, Vulktrt::cmake::project_version)};

        bool show_version = false;
        app.add_flag("--version", show_version, "Show version information");

        CLI11_PARSE(app, argc, argv);

        if(show_version) {
            LINFO("{}\n", Vulktrt::cmake::project_version);
            return EXIT_SUCCESS;
        }

    } catch(const std::exception &e) { spdlog::error("Unhandled exception in main: {}", e.what()); }
}

// NOLINTEND(*-include-cleaner)