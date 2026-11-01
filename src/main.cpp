#include "cli.hpp"
#include "lexer.hpp"
#include "token.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

using dotdash::cli::APPLICATION_NAME;
using dotdash::cli::EXIT_RUNTIME_ERROR;
using dotdash::cli::EXIT_SUCCESS_CODE;

// Converts a token type into a readable name for terminal output.
const char* tokenTypeToString(dotdash::TokenType type) {
    using dotdash::TokenType;

    switch (type) {
        case TokenType::EndOfFile:            return "EndOfFile";
        case TokenType::Invalid:              return "Invalid";

        case TokenType::IntegerLiteral:       return "IntegerLiteral";
        case TokenType::DoubleLiteral:        return "DoubleLiteral";
        case TokenType::StringLiteral:        return "StringLiteral";
        case TokenType::BooleanTrue:          return "BooleanTrue";
        case TokenType::BooleanFalse:         return "BooleanFalse";

        case TokenType::Identifier:           return "Identifier";

        case TokenType::Plus:                 return "Plus";
        case TokenType::Minus:                return "Minus";
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

        case TokenType::Assignment:           return "Assignment";

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
    using namespace dotdash::cli;

    Options options;
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