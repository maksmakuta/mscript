#include "mscript/frontend/Parser.hpp"

#include <algorithm>
#include <utility>

namespace ms {

    Parser::Parser(std::vector<Token> tokens, ErrorCallback callback)
        : m_tokens(std::move(tokens)), m_callback(std::move(callback)), m_index(0) {}

    std::vector<Box<Statement>> Parser::getAST() {
        std::vector<Box<Statement>> program;
        while (!isAtEnd()) {
            program.push_back(parseStatement());
        }
        return program;
    }

    const Token& Parser::peek() const {
        return m_tokens[m_index];
    }

    const Token& Parser::previous() const {
        return m_tokens[m_index - 1];
    }

    bool Parser::isAtEnd() const {
        return peek().word == Word::EndOfFile;
    }

    Token Parser::advance() {
        if (!isAtEnd()) m_index++;
        return previous();
    }

    bool Parser::match(const std::vector<Word>& types) {
        return std::ranges::any_of(types,[this](const Word& word) {
            if (!isAtEnd() && peek().word == word) {
                advance();
                return true;
            }
            return false;
        });
    }

    std::optional<Token> Parser::consume(const Word word, std::string error) {
        if (!isAtEnd() && peek().word == word) {
            return advance();
        }
        if (m_callback) {
            m_callback(ParserError{std::move(error), peek().line, peek().column});
        }
        return std::nullopt;
    }

    Box<Statement> Parser::parseStatement() {
        if (match({Word::Function}))  return parseFunction();
        if (match({Word::While}))     return parseWhile();
        if (match({Word::Let}))       return parseVariable();
        if (match({Word::For}))       return parseFor();
        if (match({Word::Do}))        return parseDoWhile();
        if (match({Word::Return}))    return parseReturn();

        if (match({Word::LeftBrace})) {
            return parseBlock();
        }

        if (match({Word::Break, Word::Continue}))
            return make_box<Statement>( KeywordStmt{advance()} );

        return make_box<Statement>( ExpressionStmt{parseExpression()} );
    }

    Box<Statement> Parser::parseFunction() {
        const auto nameToken = consume(Word::Identifier, "Expected function name.");
        if (!nameToken) return nullptr;
        const std::string_view name = nameToken->value;

        consume(Word::LeftParen, "Expected '(' after function name.");
        std::vector<Token> parameters;
        if (peek().word != Word::RightParen) {
            do {
                if (const auto param = consume(Word::Identifier, "Expected parameter name.")) parameters.push_back(*param);
            } while (match({Word::Comma}));
        }
        consume(Word::RightParen, "Expected ')' after parameters.");

        std::vector<Box<Statement>> body;

        if (match({Word::Assign})) {
            Box<Expression> expr = parseExpression();
            body.push_back(make_box<Statement>(ReturnStmt{std::move(expr)}));
        }
        else if (match({Word::LeftBrace})) {
            while (!isAtEnd() && peek().word != Word::RightBrace) {
                body.push_back(parseStatement());
            }
            consume(Word::RightBrace, "Expected '}' after function body.");
        }
        else {
            m_callback(ParserError{"Expected '{' or '=' after function signature.", peek().line, peek().column});
            return nullptr;
        }

        return make_box<Statement>(FunctionStmt{name, std::move(parameters), std::move(body)});
    }

    Box<Statement> Parser::parseVariable() {
        const auto nameToken = consume(Word::Identifier, "Expected variable name after 'let'.");
        if (!nameToken) {
            return nullptr;
        }

        if (!consume(Word::Assign, "Variables must be initialized. Expected '=' after variable name.")) {
            return nullptr;
        }

        return make_box<Statement>(LetStmt{
            std::string(nameToken->value),
            parseExpression()
        });
    }

    Box<Statement> Parser::parseFor() {
        if (!consume(Word::LeftParen, "Expected '(' after 'for'.")) return {};

        if (!consume(Word::Let, "Expected 'let' in for-loop declaration.")) return {};

        const auto varName = consume(Word::Identifier, "Expected variable name after 'let'.");
        if (!varName) return {};

        if (!consume(Word::In, "Expected 'in' keyword after variable name.")) return {};

        Box<Expression> iterable = parseExpression();

        if (!consume(Word::RightParen, "Expected ')' after for-loop clause.")) return {};

        Box<Statement> body = parseStatement();

        return make_box<Statement>(ForStmt{
            varName->value,
            std::move(iterable),
            std::move(body)
        });
    }

    Box<Statement> Parser::parseWhile() {
        if (!consume(Word::LeftParen, "Expected '(' after 'while'.")) return nullptr;
        Box<Expression> condition = parseExpression();
        if (!consume(Word::RightParen, "Expected ')' after while condition.")) return nullptr;
        Box<Statement> body = parseStatement();
        return make_box<Statement>(WhileStmt{
            std::move(condition),
            std::move(body),
            false
        });
    }

    Box<Statement> Parser::parseDoWhile() {
        Box<Statement> body = parseStatement();
        if (!consume(Word::While, "Expected 'while' after 'do' block.")) return nullptr;
        if (!consume(Word::LeftParen, "Expected '(' after 'while'.")) return nullptr;
        Box<Expression> condition = parseExpression();
        if (!consume(Word::RightParen, "Expected ')' after while condition.")) return nullptr;
        return make_box<Statement>(WhileStmt{
            std::move(condition),
            std::move(body),
            true
        });
    }

    Box<Statement> Parser::parseReturn() {
        Box<Expression> value = nullptr;

        if (peek().word != Word::RightBrace) {
            value = parseExpression();
        }

        return make_box<Statement>(ReturnStmt{std::move(value)});
    }

    Box<Statement> Parser::parseMatchBody() {
        if (peek().word == Word::LeftBrace) {
            advance();
            std::vector<Box<Statement>> statements;
            while (!isAtEnd() && peek().word != Word::RightBrace) {
                statements.push_back(parseStatement());
            }
            consume(Word::RightBrace, "Expected '}' after block.");
            return make_box<Statement>(BlockStmt{std::move(statements)});
        }

        return make_box<Statement>( ExpressionStmt{ parseExpression() });
    }

    Box<Statement> Parser::parseBlock() {
        std::vector<Box<Statement>> statements;
        while (!isAtEnd() && peek().word != Word::RightBrace) {
            statements.push_back(parseStatement());
        }
        consume(Word::RightBrace, "Expected '}' after block.");
        return make_box<Statement>(BlockStmt{std::move(statements)});
    }

    Box<Expression> Parser::parseMatch() {
        Box<Expression> target;

        if (match({Word::LeftParen})) {
            target = parseExpression();
            consume(Word::RightParen, "Expected ')' after match target.");
        } else {
            target = nullptr;
        }

        consume(Word::LeftBrace, "Expected '{' to start match body.");

        std::vector<MatchCase> cases;
        while (!isAtEnd() && peek().word != Word::RightBrace) {
            std::optional<Box<Expression>> pattern;

            if (match({Word::Else})) {
                pattern = std::nullopt;
            } else {
                pattern = parseExpression();
            }

            consume(Word::Arrow, "Expected '->' after match case.");

            Box<Statement> body = parseMatchBody();

            cases.push_back(MatchCase{std::move(pattern), std::move(body)});
        }

        consume(Word::RightBrace, "Expected '}' after match body.");

        return make_box<Expression>(MatchExpr{std::move(target), std::move(cases)});
    }

    Box<Expression> Parser::parseIf() {
        if (!consume(Word::LeftParen, "Expected '(' after 'if'.")) return nullptr;
        Box<Expression> condition = parseExpression();
        if (!consume(Word::RightParen, "Expected ')' after if condition.")) return nullptr;

        if (!consume(Word::LeftBrace, "Expected '{' to start 'then' block.")) return nullptr;

        std::vector<Box<Statement>> thenBranch;
        while (!isAtEnd() && peek().word != Word::RightBrace) {
            thenBranch.push_back(parseStatement());
        }

        if (!consume(Word::RightBrace, "Expected '}' after 'then' block.")) return nullptr;

        std::vector<Box<Statement>> elseBranch;
        if (match({Word::Else})) {
            if (!consume(Word::LeftBrace, "Expected '{' to start 'else' block.")) return nullptr;

            while (!isAtEnd() && peek().word != Word::RightBrace) {
                elseBranch.push_back(parseStatement());
            }

            if (!consume(Word::RightBrace, "Expected '}' after 'else' block.")) return nullptr;
        }

        return make_box<Expression>(IfExpr{
            std::move(condition),
            make_box<Statement>(BlockStmt{std::move(thenBranch)}),
            make_box<Statement>(BlockStmt{std::move(elseBranch)})
        });
    }

    Box<Expression> Parser::parseExpression() {
        if (peek().word == Word::If) return parseIf();
        if (peek().word == Word::Match) return parseMatch();

        return parseAssignment();
    }

    Box<Expression> Parser::parseAssignment() {
        Box<Expression> expr = parseLogicalOr();

        if (match({Word::Assign, Word::PlusAssign, Word::MinusAssign,
                   Word::MultiplyAssign, Word::DivideAssign, Word::ModulusAssign,
                   Word::BAndAssign, Word::BOrAssign, Word::BXorAssign,
                   Word::LeftShiftAssign, Word::RightShiftAssign})) {

            const Token op = previous();
            Box<Expression> value = parseAssignment();
            return make_box<Expression>(AssignExpr{std::move(expr), op, std::move(value)});
        }

        return expr;
    }

    Box<Expression> Parser::parseLogicalOr() {
        Box<Expression> expr = parseLogicalAnd();
        while (match({Word::Or})) {
            const Token op = previous();
            Box<Expression> right = parseLogicalAnd();
            expr = make_box<Expression>(BinaryExpr{std::move(expr), op, std::move(right)});
        }
        return expr;
    }

    Box<Expression> Parser::parseLogicalAnd() {
        Box<Expression> expr = parseBitwiseOr();
        while (match({Word::And})) {
            const Token op = previous();
            Box<Expression> right = parseBitwiseOr();
            expr = make_box<Expression>(BinaryExpr{std::move(expr), op, std::move(right)});
        }
        return expr;
    }

    Box<Expression> Parser::parseBitwiseOr() {
        Box<Expression> expr = parseEquality();
        while (match({Word::BOr, Word::BXor, Word::BAnd})) {
            const Token op = previous();
            Box<Expression> right = parseEquality();
            expr = make_box<Expression>(BinaryExpr{std::move(expr), op, std::move(right)});
        }
        return expr;
    }

    Box<Expression> Parser::parseEquality() {
        Box<Expression> expr = parseComparison();
        while (match({Word::Equals, Word::NotEquals})) {
            const Token op = previous();
            Box<Expression> right = parseComparison();
            expr = make_box<Expression>(BinaryExpr{std::move(expr), op, std::move(right)});
        }
        return expr;
    }

    Box<Expression> Parser::parseComparison() {
        Box<Expression> expr = parseShift();
        while (match({Word::Less, Word::Greater, Word::LessEqual, Word::GreaterEqual})) {
            const Token op = previous();
            Box<Expression> right = parseShift();
            expr = make_box<Expression>(BinaryExpr{std::move(expr), op, std::move(right)});
        }
        return expr;
    }

    Box<Expression> Parser::parseShift() {
        Box<Expression> expr = parseRange();
        while (match({Word::LeftShift, Word::RightShift})) {
            const Token op = previous();
            Box<Expression> right = parseRange();
            expr = make_box<Expression>(BinaryExpr{std::move(expr), op, std::move(right)});
        }
        return expr;
    }

    Box<Expression> Parser::parseTerm() {
        Box<Expression> expr = parseFactor();
        while (match({Word::Plus, Word::Minus})) {
            const Token op = previous();
            Box<Expression> right = parseFactor();
            expr = make_box<Expression>(BinaryExpr{std::move(expr), op, std::move(right)});
        }
        return expr;
    }

    Box<Expression> Parser::parseFactor() {
        Box<Expression> expr = parseUnary();
        while (match({Word::Multiply, Word::Divide, Word::Modulus})) {
            const Token op = previous();
            Box<Expression> right = parseUnary();
            expr = make_box<Expression>(BinaryExpr{std::move(expr), op, std::move(right)});
        }
        return expr;
    }

    Box<Expression> Parser::parseUnary() {
        if (match({Word::Not, Word::Invert, Word::Minus})) {
            const Token op = previous();
            Box<Expression> right = parseUnary();
            return make_box<Expression>(UnaryExpr{std::move(right),op});
        }
        return parseCall();
    }

    Box<Expression> Parser::finishCall(Box<Expression> callee) {
        std::vector<Box<Expression>> arguments;
        if (peek().word != Word::RightParen) {
            do {
                if (arguments.size() >= 255) {
                    m_callback(ParserError{"Cannot have more than 255 arguments.", peek().line, peek().column});
                }
                arguments.push_back(parseExpression());
            } while (match({Word::Comma}));
        }

        consume(Word::RightParen, "Expected ')' after arguments.");

        return make_box<Expression>( CallExpr{std::move(callee), std::move(arguments)});
    }

    Box<Expression> Parser::parseCall() {
        Box<Expression> expr = parsePrimary();

        while (true) {
            if (match({Word::LeftParen})) {
                expr = finishCall(std::move(expr));
            } else if (match({Word::Dot})) {
                const std::string_view name = consume(Word::Identifier, "Expected property name after '.'.")
                             .value_or(Token{}).value;
                expr = make_box<Expression>(GetExpr{std::move(expr), name});
            } else if (match({Word::LeftBracket})) {
                Box<Expression> index = parseExpression();
                consume(Word::RightBracket, "Expected ']' after index.");
                expr = make_box<Expression>(IndexExpr{std::move(expr), std::move(index)});
            } else {
                break;
            }
        }
        return expr;
    }

    Box<Expression> Parser::parsePrimary() {
        if (match({Word::Integer, Word::Real, Word::String, Word::True, Word::False})) {
            return make_box<Expression>(LiteralExpr{previous()});
        }
        if (match({Word::Identifier})) {
            return make_box<Expression>(VariableExpr{previous().value});
        }
        if (match({Word::LeftParen})) {
            Box<Expression> expr = parseExpression();
            consume(Word::RightParen, "Expected ')' after grouping expression.");
            return make_box<Expression>(GroupingExpr{std::move(expr)});
        }

        consume(Word::Identifier, "Expected expression.");
        return {};
    }

    Box<Expression> Parser::parseRange() {
        Box<Expression> expr = parseTerm();
        if (match({Word::Range})) {
            Box<Expression> to = parseTerm();
            if (match({Word::Colon})) {
                return make_box<Expression>(RangeExpr{std::move(expr), std::move(to), parseTerm()});
            }
            return make_box<Expression>(RangeExpr{std::move(expr), std::move(to), nullptr});
        }
        return expr;
    }

}
