
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

constexpr const char* DOTDASH_VERSION = "0.1.0";

struct CliOptions {
    std::string filePath;
    std::vector<std::string> unavailableFeatures;

    bool showHelp = false;
    bool showVersion = false;
    bool showTokens = false;
    bool verbose = false;
};

void printHelp() {
    std::cout
        << "DotDash " << DOTDASH_VERSION << "\n"
        << "Usage: dotdash [options] <file.dd>\n\n"
        << "Options:\n"
        << "  -h, --help       Display this help message\n"
        << "  -v, --version    Display version information\n"
        << "  --tokens         Display lexical tokens (not implemented)\n"
        << "  --lexer          Alias for --tokens\n"
        << "  --parse          Parse the source file (not implemented)\n"
        << "  --ast            Display the abstract syntax tree (not implemented)\n"
        << "  --check          Check the source file (not implemented)\n"
        << "  --interpret      Interpret the source file (not implemented)\n"
        << "  --verbose        Enable verbose diagnostics (not implemented)\n";
}

void printVersion() {
    std::cout << "DotDash " << DOTDASH_VERSION << '\n';
}

int reportError(const std::string& message) {
    std::cerr << "Error: " << message << '\n';
    return 2;
}

int reportUnavailable(const std::string& feature) {
    std::cerr
        << "Sorry! The " << feature << " feature is not available yet.\n"
        << "It is planned for a future version of DotDash.\n";
    return 1;
}

bool addUnavailableFeature(
    CliOptions& options,
    const std::string& feature
) {
    for (const std::string& existing : options.unavailableFeatures) {
        if (existing == feature) {
            return true;
        }
    }

    options.unavailableFeatures.push_back(feature);
    return true;
}

bool parseArguments(int argc, char* argv[], CliOptions& options) {
    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];

        if (argument == "--help" || argument == "-h") {
            options.showHelp = true;
        } else if (argument == "--version" || argument == "-v") {
            options.showVersion = true;
        } else if (argument == "--tokens" || argument == "--lexer") {
            options.showTokens = true;
        } else if (argument == "--parse") {
            addUnavailableFeature(options, "parser");
        } else if (argument == "--ast") {
            addUnavailableFeature(options, "AST inspection");
        } else if (argument == "--check") {
            addUnavailableFeature(options, "source checking");
        } else if (argument == "--interpret") {
            addUnavailableFeature(options, "interpreter");
        } else if (argument == "--verbose") {
            options.verbose = true;
            addUnavailableFeature(options, "verbose diagnostics");
        } else if (!argument.empty() && argument[0] == '-') {
            return false;
        } else if (options.filePath.empty()) {
            options.filePath = argument;
        } else {
            return false;
        }
    }

    return true;
}

bool readSourceFile(
    const std::string& filePath,
    std::string& source
) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    source.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    return !file.bad();
}

int main(int argc, char* argv[]) {
    CliOptions options;

    if (!parseArguments(argc, argv, options)) {
        return reportError(
            "Invalid command-line arguments. Run 'dotdash --help' "
            "for usage information."
        );
    }

    if (options.showHelp) {
        printHelp();
        return 0;
    }

    if (options.showVersion) {
        printVersion();
        return 0;
    }

    if (options.filePath.empty()) {
        return reportError("No source file provided.");
    }

    std::string source;

    if (!readSourceFile(options.filePath, source)) {
        return reportError(
            "Could not read source file '" + options.filePath + "'."
        );
    }

    if (!options.unavailableFeatures.empty()) {
        if (options.unavailableFeatures.size() > 1) {
            return reportError(
                "Multiple unavailable features were requested together."
            );
        }

        return reportUnavailable(options.unavailableFeatures.front());
    }

    if (options.showTokens) {
        return reportUnavailable("lexer integration");
    }

    return reportUnavailable("program execution");
}
