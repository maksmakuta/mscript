#ifndef MSCRIPT_VALUE_HPP
#define MSCRIPT_VALUE_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <fstream>
#include <vector>

namespace ms {

    struct FunctionStmt;

    struct File;
    struct Range;
    struct Array;
    struct Dict;
    struct Function;
    struct NativeFunction;

    using Value = std::variant<
        std::monostate,
        bool,
        int64_t,
        double,
        std::string,
        std::shared_ptr<Range>,
        std::shared_ptr<File>,
        std::shared_ptr<Array>,
        std::shared_ptr<Dict>,
        std::shared_ptr<Function>,
        std::shared_ptr<NativeFunction>
    >;

    using DictKey = std::variant<
        bool,
        int64_t,
        std::string
    >;

    using Values = std::vector<Value>;

    struct Range {
        Value from;
        Value to;
        Value step;
    };

    struct File {
        std::fstream stream;
    };

    struct Array {
        Values values;
    };

    struct Dict {
        std::map<DictKey,Value> values;
    };

    struct Function {
        const FunctionStmt* fn = nullptr;
    };

    struct NativeFunction {
        std::function<Value(Values)> fn;
    };

    template<typename T>
    bool is(const Value& v) {
        return std::holds_alternative<T>(v);
    }

    std::string getTypename(const Value& v);
    std::string toString(const Value& v);

}

#endif //MSCRIPT_VALUE_HPP
