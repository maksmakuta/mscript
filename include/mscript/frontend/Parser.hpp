#ifndef MSCRIPT_PARSER_HPP
#define MSCRIPT_PARSER_HPP

#include <functional>
#include <vector>

#include "mscript/frontend/Token.hpp"
#include "mscript/core/AST.hpp"

namespace ms {

    struct ParserError final {
        std::string error;
        int line;
        int column;
    };

    using ErrorCallback = std::function<void(const ParserError&)>;

    class Parser final {
    public:
        explicit Parser(std::vector<Token> tokens, ErrorCallback callback);
        std::vector<Box<Statement>> getAST();

    private:
        [[nodiscard]] const Token& peek() const;
        [[nodiscard]] const Token& previous() const;
        [[nodiscard]] bool isAtEnd() const;
        [[nodiscard]] bool check(Word type) const;

        Token advance();
        bool match(std::initializer_list<Word> types);
        std::optional<Token> consume(Word word, std::string error);

        Box<Statement> parseStatement();
        Box<Statement> parseVariable();
        Box<Statement> parseFunction();
        Box<Statement> parseDoWhile();
        Box<Statement> parseReturn();
        Box<Statement> parseWhile();
        Box<Statement> parseFor();

        Box<Expression> parseExpression();
        Box<Expression> parseMatch();
        Box<Expression> parseIf();

        Box<Expression> parseAssignment();
        Box<Expression> parseLogicalOr();
        Box<Expression> parseLogicalAnd();
        Box<Expression> parseBitwiseOr();
        Box<Expression> parseEquality();
        Box<Expression> parseComparison();
        Box<Expression> parseShift();
        Box<Expression> parseTerm();
        Box<Expression> parseFactor();
        Box<Expression> parseUnary();
        Box<Expression> parseCall();
        Box<Expression> parsePrimary();
        Box<Expression> parseRange();

        Box<Expression> finishCall(Box<Expression> callee);
        Box<Statement> parseMatchBody();
        Box<Statement> parseBlock();

        Box<Expression> parseArray();
        Box<Expression> parseDict();

        std::vector<Token> m_tokens;
        ErrorCallback m_callback;
        std::size_t m_index;
    };

}

#endif //MSCRIPT_PARSER_HPP
