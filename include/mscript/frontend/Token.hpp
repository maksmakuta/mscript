#ifndef MSCRIPT_TOKEN_HPP
#define MSCRIPT_TOKEN_HPP

#include "mscript/core/Word.hpp"

namespace ms {

    struct Token final {
        Word word;
        std::string_view value;
        int line;
        int column;
    };

    std::string to_string(const Token& token);

}

#endif //MSCRIPT_TOKEN_HPP
