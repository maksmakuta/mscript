#include <iostream>

#include "mscript/frontend/Lexer.hpp"

const auto MSCRIPT_CODE = "let a \n= 5";

int main() {
    auto lexer = ms::Lexer(MSCRIPT_CODE);
    for (const auto& token : lexer.tokenize()) {
        std::cout << to_string(token) << std::endl;
    }
    return 0;
}
