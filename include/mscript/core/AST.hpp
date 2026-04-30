#ifndef MSCRIPT_AST_HPP
#define MSCRIPT_AST_HPP

#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "mscript/frontend/Token.hpp"

namespace ms {

    template <typename T>
    using Box = std::unique_ptr<T>;

    template <typename T, typename... Args>
    Box<T> make_box(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    struct Expression;
    struct Statement;

    struct LiteralExpr {
        Token value;
    };

    struct VariableExpr {
        Token name;
    };

    struct BinaryExpr {
        Box<Expression> left;
        Token op;
        Box<Expression> right;
    };

    struct UnaryExpr {
        Box<Expression> expr;
        Token op;
    };

    struct GroupingExpr {
        Box<Expression> expression;
    };

    struct IfExpr {
        Box<Expression> condition;
        Box<Expression> thenBranch;
        Box<Expression> elseBranch;
    };

    struct CallExpr {
        Box<Expression> callee;
        std::vector<Expression> arguments;
    };

    struct ArrayExpr {
        std::vector<Expression> elements;
    };

    struct DictExpr {
        std::vector<std::pair<Expression, Expression>> properties;
    };

    struct MatchCase {
        std::optional<Box<Expression>> pattern;
        Box<Expression> body;
    };

    struct MatchExpr {
        Box<Expression> target;
        std::vector<MatchCase> cases;
    };

    struct AssignExpr {
        Token name;
        Token op;
        Box<Expression> value;
    };

    struct LambdaExpr {
        std::vector<Token> params;
        std::variant<Box<Expression>, Box<Statement>> body;
    };

    struct Expression {
        std::variant<
            LiteralExpr,
            VariableExpr,
            BinaryExpr,
            UnaryExpr,
            GroupingExpr,
            IfExpr,
            CallExpr,
            ArrayExpr,
            DictExpr,
            MatchExpr,
            AssignExpr,
            LambdaExpr
        > node;
    };

    struct ExpressionStmt {
        Expression expression;
    };

    struct LetStmt {
        Token name;
        Expression initializer;
    };

    struct BlockStmt {
        std::vector<Statement> statements;
    };

    struct FunctionStmt {
        Token name;
        std::vector<Token> params;
        std::variant<Expression, BlockStmt> body;
    };

    struct ReturnStmt {
        Token keyword;
        std::optional<Expression> value;
    };

    struct BreakStmt {
        Token keyword;
    };

    struct ContinueStmt {
        Token keyword;
    };

    struct WhileStmt {
        Expression condition;
        Box<Statement> body;
    };

    struct DoWhileStmt {
        Box<Statement> body;
        Expression condition;
    };

    struct ForInStmt {
        Token iterator_name;
        Expression iterable;
        Box<Statement> body;
    };

    struct Statement {
        std::variant<
            ExpressionStmt,
            LetStmt,
            BlockStmt,
            FunctionStmt,
            ReturnStmt,
            BreakStmt,
            ContinueStmt,
            WhileStmt,
            DoWhileStmt,
            ForInStmt
        > node;
    };

}

#endif //MSCRIPT_AST_HPP
