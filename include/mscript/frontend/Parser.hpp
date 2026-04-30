#ifndef MSCRIPT_PARSER_HPP
#define MSCRIPT_PARSER_HPP

#include <vector>

#include "mscript/frontend/Token.hpp"
#include "mscript/core/AST.hpp"

namespace ms {

    class Parser final {
    public:
        explicit Parser(std::vector<Token> tokens);
        std::vector<Statement> getAST();

    private:
        std::vector<Token> m_tokens;
        std::size_t m_index;
    };

}

#endif //MSCRIPT_PARSER_HPP
