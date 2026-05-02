#ifndef MSCRIPT_INTERPRETER_HPP
#define MSCRIPT_INTERPRETER_HPP

#include "Environment.hpp"
#include "mscript/core/AST.hpp"

namespace ms {

    class Interpreter final {
    public:
        Interpreter();
        void interpret(const Program& program);

    private:
        void setupNativeFunctions() const;

        void execute(const Statement &stmt);
        void executeStatement(const ExpressionStmt&);
        void executeStatement(const LetStmt&);
        void executeStatement(const BlockStmt&);
        void executeStatement(const FunctionStmt&);
        void executeStatement(const ReturnStmt&);
        void executeStatement(const KeywordStmt&);
        void executeStatement(const WhileStmt&);
        void executeStatement(const ForStmt&);

        Value evaluate(const Expression &expr);
        Value evaluateExpression(const LiteralExpr&);
        Value evaluateExpression(const VariableExpr&);
        Value evaluateExpression(const BinaryExpr&);
        Value evaluateExpression(const UnaryExpr&);
        Value evaluateExpression(const GroupingExpr&);
        Value evaluateExpression(const IfExpr&);
        Value evaluateExpression(const CallExpr&);
        Value evaluateExpression(const ArrayExpr&);
        Value evaluateExpression(const DictExpr&);
        Value evaluateExpression(const MatchExpr&);
        Value evaluateExpression(const AssignExpr&);
        Value evaluateExpression(const LambdaExpr&);
        Value evaluateExpression(const GetExpr&);
        Value evaluateExpression(const IndexExpr&);
        Value evaluateExpression(const RangeExpr&);


        std::shared_ptr<Environment> m_environment;
    };

}

#endif //MSCRIPT_INTERPRETER_HPP
