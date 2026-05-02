#ifndef MSCRIPT_NATIVEFUNCTIONS_HPP
#define MSCRIPT_NATIVEFUNCTIONS_HPP

#include "Value.hpp"

namespace ms {

    Value native_print(const Values& args);
    Value native_println(const Values& args);
    Value native_type(const Values& args);

}

#endif //MSCRIPT_NATIVEFUNCTIONS_HPP
