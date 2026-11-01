#pragma once

#include <string>
#include <vector>

namespace dotdash::cli {

// Application configuration.
inline constexpr const char* APPLICATION_NAME = "DotDash";
inline constexpr const char* DOTDASH_VERSION  = "0.1.0";

// Process exit codes.
inline constexpr int EXIT_SUCCESS_CODE   = 0;
inline constexpr int EXIT_RUNTIME_ERROR  = 1;
inline constexpr int EXIT_ARGUMENT_ERROR = 2;

// Command-line options supported by the current application.
struct Options {
    std::string filePath;

    bool showHelp    = false;
    bool showVersion = false;
    bool showTokens  = false;
    bool verbose     = false;

    // Features recognized by the CLI but not implemented yet.
    std::vector<std::string> unavailableFeatures;
};

// Parses command-line arguments into Options.
//
// Returns false if an argument is invalid or multiple source files
// are supplied. On failure, errorMessage receives the explanation.
bool parseArguments(int argc, char* argv[],
                    Options& options,
                    std::string& errorMessage);

// Prints the command-line usage and available options.
void printHelp();

// Prints version information.
void printVersion();

// Reports a command-line error and returns its exit code.
int reportArgumentError(const std::string& message);

// Reports an unavailable feature and returns its exit code.
int reportUnavailableFeature(const std::string& feature);

} // namespace dotdash::cli