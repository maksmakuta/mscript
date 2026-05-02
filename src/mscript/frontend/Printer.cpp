#include "mscript/frontend/Printer.hpp"

#include <iostream>

namespace ms {

    static void indent(const int depth) {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
    }

    void printExpression(const Expression& expr, int depth);

    void printStatement(const Statement& stmt, const int depth) {
        std::visit([depth]<typename Q>(Q&& arg) {
            using T = std::decay_t<Q>;
            indent(depth);

            if constexpr (std::is_same_v<T, ExpressionStmt>) {
                std::cout << "ExpressionStmt:\n";
                printExpression(*arg.expression, depth + 1);
            }
            else if constexpr (std::is_same_v<T, LetStmt>) {
                std::cout << "LetStmt (name: " << arg.name << "):\n";
                printExpression(*arg.initializer, depth + 1);
            }
            else if constexpr (std::is_same_v<T, BlockStmt>) {
                std::cout << "BlockStmt:\n";
                for (const auto& s : arg.statements) printStatement(*s, depth + 1);
            }
            else if constexpr (std::is_same_v<T, FunctionStmt>) {
                std::cout << "FunctionStmt (name: " << arg.name << ", params: ";
                for (auto& p : arg.params) std::cout << p.value << " ";
                std::cout << "):\n";
                printStatement(*arg.body, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ReturnStmt>) {
                std::cout << "ReturnStmt:\n";
                if (arg.value) printExpression(**arg.value, depth + 1);
            }
            else if constexpr (std::is_same_v<T, KeywordStmt>) {
                std::cout << "KeywordStmt: " << arg.keyword.value << "\n";
            }
            else if constexpr (std::is_same_v<T, WhileStmt>) {
                std::cout << (arg.is_do_while ? "DoWhileStmt:\n" : "WhileStmt:\n");
                printExpression(*arg.condition, depth + 1);
                printStatement(*arg.body, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ForStmt>) {
                std::cout << "ForStmt (var: " << arg.iterator_name << "):\n";
                printExpression(*arg.iterable, depth + 1);
                printStatement(*arg.body, depth + 1);
            }
        }, stmt.node);
    }

    void printExpression(const Expression& expr, int depth) {
        std::visit([depth]<typename Q>(Q&& arg) {
            using T = std::decay_t<Q>;
            indent(depth);

            if constexpr (std::is_same_v<T, LiteralExpr>) {
                std::cout << "Literal (" << arg.value.value << ")\n";
            }
            else if constexpr (std::is_same_v<T, VariableExpr>) {
                std::cout << "Variable (" << arg.name << ")\n";
            }
            else if constexpr (std::is_same_v<T, BinaryExpr>) {
                std::cout << "Binary (" << arg.op.value << "):\n";
                printExpression(*arg.left, depth + 1);
                printExpression(*arg.right, depth + 1);
            }
            else if constexpr (std::is_same_v<T, UnaryExpr>) {
                std::cout << "Unary (" << arg.op.value << "):\n";
                printExpression(*arg.expr, depth + 1);
            }
            else if constexpr (std::is_same_v<T, GroupingExpr>) {
                std::cout << "Grouping:\n";
                printExpression(*arg.expression, depth + 1);
            }
            else if constexpr (std::is_same_v<T, IfExpr>) {
                std::cout << "IfExpr:\n";
                indent(depth + 1); std::cout << "Cond:\n";
                printExpression(*arg.condition, depth + 2);
                indent(depth + 1); std::cout << "Then:\n";
                printStatement(*arg.thenBranch, depth + 2);
                indent(depth + 1); std::cout << "Else:\n";
                printStatement(*arg.elseBranch, depth + 2);
            }
            else if constexpr (std::is_same_v<T, MatchExpr>) {
                std::cout << "MatchExpr:\n";
                if (arg.target) printExpression(*arg.target, depth + 1);
                for (auto& c : arg.cases) {
                    indent(depth + 1);
                    std::cout << "Case " << (c.pattern ? "Pattern" : "Else") << ":\n";
                    if (c.pattern) printExpression(**c.pattern, depth + 2);
                    printStatement(*c.body, depth + 2);
                }
            }
            else if constexpr (std::is_same_v<T, CallExpr>) {
                std::cout << "CallExpr:\n";
                printExpression(*arg.callee, depth + 1);
                for (auto& a : arg.arguments) printExpression(*a, depth + 2);
            }
            else if constexpr (std::is_same_v<T, AssignExpr>) {
                std::cout << "Assign (" << arg.op.value << "):\n";
                printExpression(*arg.name, depth + 1);
                printExpression(*arg.value, depth + 1);
            }
            else if constexpr (std::is_same_v<T, RangeExpr>) {
                std::cout << "Range:\n";
                printExpression(*arg.from, depth + 1);
                printExpression(*arg.to, depth + 1);
                if (arg.step) printExpression(*arg.step, depth + 1);
            }
            else if constexpr (std::is_same_v<T, ArrayExpr>) {
                std::cout << "ArrayExpr:\n";
                for (const auto& el : arg.elements) {
                    printExpression(*el, depth + 1);
                }
            }
            else if constexpr (std::is_same_v<T, DictExpr>) {
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

    void print(const Program& program) {
        for (const auto& statement : program) {
            printStatement(*statement, 0);
        }
    }

}
