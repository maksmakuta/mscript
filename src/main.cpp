#include <expected>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include "mscript/frontend/Lexer.hpp"

enum class ErrorCode {
    FileNotFound,
    IOError,
    EmptyFile
};

std::expected<std::string, ErrorCode> readFile(const std::string& path) {
    if (std::filesystem::exists(path)) {
        if (std::ifstream file(path); file.is_open()) {
            std::stringstream ss;
            ss << file.rdbuf();
            auto content = ss.str();
            if (content.empty()) {
                return std::unexpected{ ErrorCode::EmptyFile };
            }
            return content;
        }
        return std::unexpected{ ErrorCode::IOError };
    }
    return std::unexpected{ ErrorCode::FileNotFound };
}

void interpret(const std::string_view& code) {
    auto lexer = ms::Lexer(code);
    for (const auto& token : lexer.tokenize()) {
        std::cout << to_string(token) << std::endl;
    }
}

int main(const int argc, const char** argv) {
    if (argc < 2) {
        std::string code;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, code);
            if (code == "exit" || code == "quit" || code == "/q" || code == "/e") {
                break;
            }
            interpret(code);
        }
    }else {
        const auto path = argv[1];
        if (const auto code = readFile(path)) {
            interpret(*code);
        }else {
            switch (code.error()) {
                case ErrorCode::FileNotFound:
                    std::cout << "File not found: " << path << std::endl;
                    break;
                case ErrorCode::IOError:
                    std::cout << "Cannot open file: " << path << std::endl;
                    break;
                case ErrorCode::EmptyFile:
                    std::cout << "File is empty: " << path << std::endl;
                    break;
                default:
                    std::cout << "Unknown error: " << static_cast<int>(code.error()) << std::endl;
                    break;
            }
        }
    }
    return 0;
}
