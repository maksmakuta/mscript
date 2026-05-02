#include <expected>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>

#include "mscript/backend/Interpreter.hpp"
#include "mscript/frontend/Lexer.hpp"
#include "mscript/frontend/Parser.hpp"

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

    static void indent(const int depth) {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
    }

    void printExpression(const ms::Expression& expr, int depth);

    void printStatement(const ms::Statement& stmt, const int depth) {
        std::visit([depth](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            indent(depth);

            if constexpr (std::is_same_v<T, ms::ExpressionStmt>) {
                std::cout << "ExpressionStmt:\n";
                printExpression(*arg.expression, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::LetStmt>) {
                std::cout << "LetStmt (name: " << arg.name << "):\n";
                printExpression(*arg.initializer, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::BlockStmt>) {
                std::cout << "BlockStmt:\n";
                for (const auto& s : arg.statements) printStatement(*s, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::FunctionStmt>) {
                std::cout << "FunctionStmt (name: " << arg.name << ", params: ";
                for (auto& p : arg.params) std::cout << p.value << " ";
                std::cout << "):\n";
                for (const auto& s : arg.body) printStatement(*s, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::ReturnStmt>) {
                std::cout << "ReturnStmt:\n";
                if (arg.value) printExpression(**arg.value, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::KeywordStmt>) {
                std::cout << "KeywordStmt: " << arg.keyword.value << "\n";
            }
            else if constexpr (std::is_same_v<T, ms::WhileStmt>) {
                std::cout << (arg.is_do_while ? "DoWhileStmt:\n" : "WhileStmt:\n");
                printExpression(*arg.condition, depth + 1);
                printStatement(*arg.body, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::ForStmt>) {
                std::cout << "ForStmt (var: " << arg.iterator_name << "):\n";
                printExpression(*arg.iterable, depth + 1);
                printStatement(*arg.body, depth + 1);
            }
        }, stmt.node);
    }

    void printExpression(const ms::Expression& expr, int depth) {
        std::visit([depth](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            indent(depth);

            if constexpr (std::is_same_v<T, ms::LiteralExpr>) {
                std::cout << "Literal (" << arg.value.value << ")\n";
            }
            else if constexpr (std::is_same_v<T, ms::VariableExpr>) {
                std::cout << "Variable (" << arg.name << ")\n";
            }
            else if constexpr (std::is_same_v<T, ms::BinaryExpr>) {
                std::cout << "Binary (" << arg.op.value << "):\n";
                printExpression(*arg.left, depth + 1);
                printExpression(*arg.right, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::UnaryExpr>) {
                std::cout << "Unary (" << arg.op.value << "):\n";
                printExpression(*arg.expr, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::GroupingExpr>) {
                std::cout << "Grouping:\n";
                printExpression(*arg.expression, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::IfExpr>) {
                std::cout << "IfExpr:\n";
                indent(depth + 1); std::cout << "Cond:\n";
                printExpression(*arg.condition, depth + 2);
                indent(depth + 1); std::cout << "Then:\n";
                printStatement(*arg.thenBranch, depth + 2);
                indent(depth + 1); std::cout << "Else:\n";
                printStatement(*arg.elseBranch, depth + 2);
            }
            else if constexpr (std::is_same_v<T, ms::MatchExpr>) {
                std::cout << "MatchExpr:\n";
                if (arg.target) printExpression(*arg.target, depth + 1);
                for (auto& c : arg.cases) {
                    indent(depth + 1);
                    std::cout << "Case " << (c.pattern ? "Pattern" : "Else") << ":\n";
                    if (c.pattern) printExpression(**c.pattern, depth + 2);
                    printStatement(*c.body, depth + 2);
                }
            }
            else if constexpr (std::is_same_v<T, ms::CallExpr>) {
                std::cout << "CallExpr:\n";
                printExpression(*arg.callee, depth + 1);
                for (auto& a : arg.arguments) printExpression(*a, depth + 2);
            }
            else if constexpr (std::is_same_v<T, ms::AssignExpr>) {
                std::cout << "Assign (" << arg.op.value << "):\n";
                printExpression(*arg.name, depth + 1);
                printExpression(*arg.value, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::RangeExpr>) {
                std::cout << "Range:\n";
                printExpression(*arg.from, depth + 1);
                printExpression(*arg.to, depth + 1);
                if (arg.step) printExpression(*arg.step, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ms::ArrayExpr>) {
                std::cout << "ArrayExpr:\n";
                for (const auto& el : arg.elements) {
                    printExpression(*el, depth + 1);
                }
            }
            else if constexpr (std::is_same_v<T, ms::DictExpr>) {
                std::cout << "DictExpr:\n";
                for (const auto& [key, val] : arg.properties) {
                    indent(depth + 1); std::cout << "Key:\n";
                    printExpression(*key, depth + 2);
                    indent(depth + 1); std::cout << "Value:\n";
                    printExpression(*val, depth + 2);
                }
            }
        }, expr.node);
    }

void interpret(const std::string_view& code) {
    auto lexer = ms::Lexer(code);
    const auto tokens = lexer.tokenize();
    for (const auto& token : tokens) {
        std::cout << to_string(token) << std::endl;
    }

    auto parser = ms::Parser(tokens, [](const ms::ParserError&) {});
    auto program = parser.getAST();
    for (const auto& statement : program) {
        printStatement(*statement, 0);
    }

    auto interpreter = ms::Interpreter();
    interpreter.interpret(program);
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
