
#include "lexer.hpp"
#include "token.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

// Application configuration.
constexpr const char* APPLICATION_NAME = "DotDash";
constexpr const char* DOTDASH_VERSION = "0.1.0";

// Process exit codes.
constexpr int EXIT_SUCCESS_CODE = 0;
constexpr int EXIT_RUNTIME_ERROR = 1;
constexpr int EXIT_ARGUMENT_ERROR = 2;

// Command-line options supported by the current application.
struct CliOptions {
    std::string filePath;

    bool showHelp = false;
    bool showVersion = false;
    bool showTokens = false;
    bool verbose = false;

    // Features recognized by the CLI but not implemented yet.
    std::vector<std::string> unavailableFeatures;
};

// Converts a token type into a readable name for terminal output.
const char* tokenTypeToString(dotdash::TokenType type) {
    using dotdash::TokenType;

    switch (type) {
        case TokenType::EndOfFile:          return "EndOfFile";
        case TokenType::Invalid:             return "Invalid";

        case TokenType::IntegerLiteral:      return "IntegerLiteral";
        case TokenType::DoubleLiteral:       return "DoubleLiteral";
        case TokenType::StringLiteral:       return "StringLiteral";
        case TokenType::BooleanTrue:         return "BooleanTrue";
        case TokenType::BooleanFalse:        return "BooleanFalse";

        case TokenType::Identifier:          return "Identifier";

        case TokenType::Plus:                return "Plus";
        case TokenType::Minus:               return "Minus";
        case TokenType::Multiply:             return "Multiply";
        case TokenType::Divide:               return "Divide";
        case TokenType::Modulo:               return "Modulo";
        case TokenType::Power:                return "Power";

        case TokenType::Equal:                return "Equal";
        case TokenType::NotEqual:             return "NotEqual";
        case TokenType::LessThan:             return "LessThan";
        case TokenType::GreaterThan:          return "GreaterThan";
        case TokenType::LessThanOrEqual:      return "LessThanOrEqual";
        case TokenType::GreaterThanOrEqual:   return "GreaterThanOrEqual";

        case TokenType::LogicalAnd:           return "LogicalAnd";
        case TokenType::LogicalOr:            return "LogicalOr";
        case TokenType::LogicalNot:           return "LogicalNot";

        case TokenType::Assignment:            return "Assignment";

        case TokenType::Let:                  return "Let";
        case TokenType::Const:                return "Const";
        case TokenType::If:                   return "If";
        case TokenType::Elif:                 return "Elif";
        case TokenType::Else:                 return "Else";
        case TokenType::While:                return "While";
        case TokenType::Break:                return "Break";
        case TokenType::Continue:             return "Continue";

        case TokenType::Input:                return "Input";
        case TokenType::Output:               return "Output";

        case TokenType::Newline:              return "Newline";
        case TokenType::Indent:               return "Indent";
        case TokenType::Dedent:               return "Dedent";
    }

    return "Unknown";
}

// Escapes special characters so each lexeme can be displayed on one line.
std::string escapeLexeme(const std::string& lexeme) {
    std::string escaped;
    escaped.reserve(lexeme.size());

    for (char character : lexeme) {
        switch (character) {
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            case '"':
                escaped += "\\\"";
                break;
            default:
                escaped += character;
                break;
        }
    }

    return escaped;
}

// Prints the command-line usage and available options.
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

// Prints version information.
void printVersion() {
    std::cout << APPLICATION_NAME << " " << DOTDASH_VERSION << '\n';
}

// Reports a command-line error and returns its exit code.
int reportArgumentError(const std::string& message) {
    std::cerr << APPLICATION_NAME << ": error: " << message << '\n';
    std::cerr << "Use 'dotdash --help' for usage information.\n";

    return EXIT_ARGUMENT_ERROR;
}

// Reports an unavailable feature.
int reportUnavailableFeature(const std::string& feature) {
    std::cerr
        << APPLICATION_NAME << ": error: "
        << feature << " is not implemented yet.\n";

    return EXIT_RUNTIME_ERROR;
}

// Adds an unavailable feature only if it has not already been recorded.
void addUnavailableFeature(
    CliOptions& options,
    const std::string& feature
) {
    for (const std::string& existing : options.unavailableFeatures) {
        if (existing == feature) {
            return;
        }
    }

    options.unavailableFeatures.push_back(feature);
}

// Parses command-line arguments into CliOptions.
//
// Returns false if an argument is invalid or multiple source files
// are supplied. The errorMessage parameter receives the explanation.
bool parseArguments(
    int argc,
    char* argv[],
    CliOptions& options,
    std::string& errorMessage
) {
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

// Reads a source file into a string.
//
// Returns false if the file cannot be opened or a read error occurs.
bool readSourceFile(
    const std::string& filePath,
    std::string& source,
    std::string& errorMessage
) {
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        errorMessage = "cannot open source file '" + filePath + "'";
        return false;
    }

    source.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    if (file.bad()) {
        errorMessage = "failed to read source file '" + filePath + "'";
        return false;
    }

    return true;
}

// Prints a diagnostic for a token that the lexer could not recognize.
void reportLexicalError(
    const std::string& filePath,
    const dotdash::Token& token
) {
    std::cerr
        << filePath << ':'
        << token.line << ':'
        << token.column
        << ": error: invalid token \""
        << escapeLexeme(token.lexeme)
        << "\"\n";
}

// Runs the lexer and displays the resulting tokens.
//
// Returns EXIT_RUNTIME_ERROR if any Invalid tokens are produced.
int runLexer(
    const std::string& filePath,
    const std::string& source,
    bool verbose
) {
    dotdash::Lexer lexer(source);
    const std::vector<dotdash::Token> tokens = lexer.tokenize();

    std::size_t invalidTokenCount = 0;

    if (verbose) {
        std::cerr
            << "DotDash: tokenizing '" << filePath << "'...\n";
    }

    std::cout
        << std::left
        << std::setw(8)  << "LINE"
        << std::setw(8)  << "COLUMN"
        << std::setw(24) << "TYPE"
        << "LEXEME\n";

    std::cout << std::string(64, '-') << '\n';

    for (const dotdash::Token& token : tokens) {
        std::cout
            << std::left
            << std::setw(8)  << token.line
            << std::setw(8)  << token.column
            << std::setw(24) << tokenTypeToString(token.type)
            << '"' << escapeLexeme(token.lexeme) << '"'
            << '\n';

        if (token.type == dotdash::TokenType::Invalid) {
            ++invalidTokenCount;
            reportLexicalError(filePath, token);
        }
    }

    if (verbose) {
        std::cerr
            << "DotDash: processed " << tokens.size()
            << " tokens; found " << invalidTokenCount
            << " invalid token(s).\n";
    }

    return invalidTokenCount == 0
        ? EXIT_SUCCESS_CODE
        : EXIT_RUNTIME_ERROR;
}

} // namespace

// Coordinates command-line processing and the selected application action.
int main(int argc, char* argv[]) {
    CliOptions options;
    std::string errorMessage;

    if (!parseArguments(argc, argv, options, errorMessage)) {
        return reportArgumentError(errorMessage);
    }

    if (options.showHelp) {
        printHelp();
        return EXIT_SUCCESS_CODE;
    }

    if (options.showVersion) {
        printVersion();
        return EXIT_SUCCESS_CODE;
    }

    if (!options.unavailableFeatures.empty()) {
        return reportUnavailableFeature(
            options.unavailableFeatures.front()
        );
    }

    if (options.filePath.empty()) {
        return reportArgumentError("no source file was provided");
    }

    if (!options.showTokens) {
        return reportUnavailableFeature(
            "program execution (try --tokens to inspect the lexer output)"
        );
    }

    std::string source;

    if (!readSourceFile(options.filePath, source, errorMessage)) {
        std::cerr << APPLICATION_NAME << ": error: "
                  << errorMessage << '\n';
        return EXIT_RUNTIME_ERROR;
    }

    return runLexer(options.filePath, source, options.verbose);
}
