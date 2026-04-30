#include "mscript/frontend/Parser.hpp"

#include <utility>

namespace ms {

    Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_index(0) {}

    std::vector<Statement> Parser::getAST() {
        return {};
    }


}