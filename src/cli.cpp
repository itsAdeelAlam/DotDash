#include "cli.hpp"

#include <iostream>

namespace dotdash::cli {

namespace {

// Adds an unavailable feature only if it has not already been recorded.
void addUnavailableFeature(Options& options, const std::string& feature) {
    for (const std::string& existing : options.unavailableFeatures) {
        if (existing == feature) {
            return;
        }
    }

    options.unavailableFeatures.push_back(feature);
}

} // namespace

bool parseArguments(int argc, char* argv[],
                    Options& options,
                    std::string& errorMessage) {
    bool treatArgumentsAsPaths = false;

    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];

        if (!treatArgumentsAsPaths && argument == "--") {
            treatArgumentsAsPaths = true;
            continue;
        }

        if (!treatArgumentsAsPaths && argument == "--help") {
            options.showHelp = true;
        } else if (!treatArgumentsAsPaths && argument == "-h") {
            options.showHelp = true;
        } else if (!treatArgumentsAsPaths && argument == "--version") {
            options.showVersion = true;
        } else if (!treatArgumentsAsPaths && argument == "-v") {
            options.showVersion = true;
        } else if (!treatArgumentsAsPaths &&
                   (argument == "--tokens" || argument == "--lexer")) {
            options.showTokens = true;
        } else if (!treatArgumentsAsPaths && argument == "--verbose") {
            options.verbose = true;
        } else if (!treatArgumentsAsPaths && argument == "--parse") {
            addUnavailableFeature(options, "--parse");
        } else if (!treatArgumentsAsPaths && argument == "--ast") {
            addUnavailableFeature(options, "--ast");
        } else if (!treatArgumentsAsPaths && argument == "--check") {
            addUnavailableFeature(options, "--check");
        } else if (!treatArgumentsAsPaths && argument == "--interpret") {
            addUnavailableFeature(options, "--interpret");
        } else if (!treatArgumentsAsPaths &&
                   !argument.empty() && argument.front() == '-') {
            errorMessage = "unknown option '" + argument + "'";
            return false;
        } else if (options.filePath.empty()) {
            options.filePath = argument;
        } else {
            errorMessage = "multiple source files were provided";
            return false;
        }
    }

    return true;
}

void printHelp() {
    std::cout
        << APPLICATION_NAME << " " << DOTDASH_VERSION << "\n"
        << "DotDash programming language tool\n\n"
        << "Usage:\n"
        << "  dotdash [options] <file.dd>\n\n"
        << "General options:\n"
        << "  -h, --help       Display this help message\n"
        << "  -v, --version    Display version information\n"
        << "      --verbose    Display additional diagnostic information\n\n"
        << "Lexer options:\n"
        << "      --tokens     Tokenize the source and display its tokens\n"
        << "      --lexer      Alias for --tokens\n\n"
        << "Planned features (not implemented):\n"
        << "      --parse      Parse the source into a syntax tree\n"
        << "      --ast        Display the abstract syntax tree\n"
        << "      --check      Check program semantics\n"
        << "      --interpret  Execute the program\n\n"
        << "Argument rules:\n"
        << "  Only one source file is accepted per invocation.\n"
        << "  Use -- before a filename that begins with a hyphen.\n\n"
        << "Examples:\n"
        << "  dotdash --tokens hello.dd\n"
        << "  dotdash --verbose --tokens hello.dd\n"
        << "  dotdash -- -example.dd\n\n"
        << "Exit status:\n"
        << "  0  Success\n"
        << "  1  Runtime or lexical error, or unavailable feature\n"
        << "  2  Invalid command-line arguments\n";
}

void printVersion() {
    std::cout << APPLICATION_NAME << " " << DOTDASH_VERSION << '\n';
}

int reportArgumentError(const std::string& message) {
    std::cerr << APPLICATION_NAME << ": error: " << message << '\n';
    std::cerr << "Use 'dotdash --help' for usage information.\n";
    return EXIT_ARGUMENT_ERROR;
}

int reportUnavailableFeature(const std::string& feature) {
    std::cerr
        << APPLICATION_NAME << ": error: "
        << feature << " is not implemented yet.\n";
    return EXIT_RUNTIME_ERROR;
}

} // namespace dotdash::cli