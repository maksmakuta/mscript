#include "mscript/backend/Interpreter.hpp"

#include <iostream>

#include "mscript/backend/NativeFunctions.hpp"

namespace ms {
    Interpreter::Interpreter() : m_environment(std::make_shared<Environment>()) {
        setupNativeFunctions();
    }

    void Interpreter::interpret(const Program& program) {
        try {
            for (const auto& stmt : program) {
                if (std::holds_alternative<FunctionStmt>(stmt->node)) {
                    const auto& fnStmt = std::get<FunctionStmt>(stmt->node);
                    auto fn = std::make_shared<Function>();
                    fn->fn = &fnStmt;
                    m_environment->define(std::string(fnStmt.name), fn);
                }
            }

            for (const auto& stmt : program) {
                execute(*stmt);
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Runtime Error: " << e.what() << std::endl;
        }
    }

    void Interpreter::setupNativeFunctions() const {
        m_environment->define("print", std::make_shared<NativeFunction>(native_print));
        m_environment->define("println", std::make_shared<NativeFunction>(native_println));
        m_environment->define("type", std::make_shared<NativeFunction>(native_type));
    }

    void Interpreter::execute(const Statement& stmt) {
        std::visit([this](auto&& arg) { executeStatement(arg); }, stmt.node);
    }

    void Interpreter::executeStatement(const ExpressionStmt& expr) {
        evaluate(*expr.expression);
    }

    void Interpreter::executeStatement(const LetStmt&) {

    }

    void Interpreter::executeStatement(const BlockStmt&) {

    }

    void Interpreter::executeStatement(const FunctionStmt&) {

    }

    void Interpreter::executeStatement(const ReturnStmt&) {

    }

    void Interpreter::executeStatement(const KeywordStmt&) {

    }

    void Interpreter::executeStatement(const WhileStmt&) {

    }

    void Interpreter::executeStatement(const ForStmt&) {

    }

    Value Interpreter::evaluate(const Expression& expr) {
        return std::visit([this](auto&& arg) -> Value { return evaluateExpression(arg); }, expr.node);
    }

    Value Interpreter::evaluateExpression(const LiteralExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const VariableExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const BinaryExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const UnaryExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const GroupingExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const IfExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const CallExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const ArrayExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const DictExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const MatchExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const AssignExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const LambdaExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const GetExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const IndexExpr&) {
        return Value{};
    }

    Value Interpreter::evaluateExpression(const RangeExpr&) {
        return Value{};
    }

}
