#ifndef MSCRIPT_VALUE_HPP
#define MSCRIPT_VALUE_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace ms {

    struct FunctionStmt;

    struct Array;
    struct Dict;
    struct Function;

    using Value = std::variant<
        std::monostate,
        bool,
        int64_t,
        double,
        std::string,
        std::shared_ptr<Array>,
        std::shared_ptr<Dict>,
        std::shared_ptr<Function>
    >;

    using Values = std::vector<Value>;

    struct Array {
        Values values;
    };

    using DictKey = std::variant<
        bool,
        int64_t,
        std::string
    >;

    struct Dict {
        std::map<DictKey,Value> values;
    };

    struct Function {
        std::function<Value(Values)> native_fn;
        const FunctionStmt* fn = nullptr;

        [[nodiscard]] bool isNative() const {
            return !!native_fn;
        }
    };

    template<typename T>
    bool is(const Value& v) {
        return std::holds_alternative<T>(v);
    }

    inline std::string typeName(const Value& v) {
        return std::visit([]<typename T>(T&& val) -> std::string {
            using Decayed = std::decay_t<T>;
            if constexpr (std::is_same_v<Decayed, std::monostate>)
                return "null";
            else if constexpr (std::is_same_v<Decayed, bool>)
                return "bool";
            else if constexpr (std::is_same_v<Decayed, int64_t>)
                return "int";
            else if constexpr (std::is_same_v<Decayed, double>)
                return "real";
            else if constexpr (std::is_same_v<Decayed, std::string>)
                return "string";
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Array>>)
                return "array";
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Dict>>)
                return "dict";
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Function>>)
                return val->isNative() ? "native function" : "function";
            else
                return "unknown";
        }, v);
    }

}

#endif //MSCRIPT_VALUE_HPP
