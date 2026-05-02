#include "mscript/backend/Interpreter.hpp"

#include <iostream>

#include "mscript/backend/NativeFunctions.hpp"

namespace ms {

    struct ReturnException : std::exception {
        Value value;
        explicit ReturnException(Value v) : value(std::move(v)) {}
    };

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


    void Interpreter::executeStatement(const LetStmt& let) {
        const auto value = evaluate(*let.initializer);
        m_environment->define(std::string(let.name), value);
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

    Value Interpreter::evaluateExpression(const LiteralExpr& literal) {
        switch (literal.value.word) {
            case Word::Null:
                return std::monostate{};
            case Word::True:
                return Value{true};
            case Word::False:
                return Value{false};
            case Word::Integer: {
                int64_t value;
                const auto data = literal.value.value;
                std::from_chars(data.begin(), data.end(), value);
                return Value{value};
            }
            case Word::Real:{
                double value;
                const auto data = literal.value.value;
                std::from_chars(data.begin(), data.end(), value);
                return Value{value};
            }
            case Word::String: {}
                return Value{std::string(literal.value.value)};
            default:
                return Value{};
        }
    }

    Value Interpreter::evaluateExpression(const VariableExpr& var) const {
        return m_environment->get(std::string(var.name));
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

    Value Interpreter::evaluateExpression(const CallExpr& call) {
        Value callee = evaluate(*call.callee);

        std::vector<Value> arguments;
        for (const auto& arg : call.arguments) {
            arguments.push_back(evaluate(*arg));
        }

        return std::visit([this, &arguments](auto&& arg) -> Value {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, std::shared_ptr<NativeFunction>>) {
                return arg->fn(arguments);
            }
            else if constexpr (std::is_same_v<T, std::shared_ptr<Function>>) {
                const auto localEnv = std::make_shared<Environment>(m_environment);
                const auto& params = arg->fn->params;

                if (arguments.size() != params.size()) {
                    throw std::runtime_error("Argument count mismatch.");
                }

                for (size_t i = 0; i < params.size(); ++i) {
                    localEnv->define(std::string(params[i].value), arguments[i]);
                }

                auto previousEnv = m_environment;
                m_environment = localEnv;

                try {
                    execute(*arg->fn->body);
                } catch (const ReturnException& e) {
                    m_environment = previousEnv;
                    return e.value;
                }

                m_environment = previousEnv;
                return std::monostate{};
            }

            throw std::runtime_error("Object is not callable.");
        }, callee);
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
