#include "mscript/backend/NativeFunctions.hpp"

#include <iostream>

namespace ms {

    Value native_print(const Values& args) {
        for (const auto& item : args) {
            std::cout << toString(item);
            if (item != args.back()) {
                std::cout << " ";
            }
        }
        return std::monostate{};
    }

    Value native_println(const Values& args) {
        const auto ret = native_print(args);
        std::cout << std::endl;
        return ret;
    }

    Value native_type(const Values& args) {
        return getTypename(args.front());
    }

}
