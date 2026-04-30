#include "mscript/frontend/Token.hpp"

#include <format>

namespace ms {

    std::string to_string(const Token& token) {
        return std::format(
            "{:>3}:{:<3} | {:14} | {} ",
            token.line,
            token.column,
            to_string(token.word),
            token.value.empty() ? "" : token.value);
    }

}
