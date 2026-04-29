#ifndef MSCRIPT_LEXER_HPP
#define MSCRIPT_LEXER_HPP

#include <string_view>
#include <vector>

#include "Token.hpp"

namespace ms {

    class Lexer final {
    public:
        explicit Lexer(std::string_view input);
        std::vector<Token> tokenize();

    private:
        Token lexNumber();
        Token lexIdentifier();
        Token lexCharacter();
        Token lexString();
        Token lexOperator();

        std::string_view m_input;
        size_t m_index;
        int m_line;
        int m_column;
    };

}

#endif //MSCRIPT_LEXER_HPP
