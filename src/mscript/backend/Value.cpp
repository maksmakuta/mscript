#include "mscript/backend/Value.hpp"

#include <sstream>

namespace ms {

    std::string getTypename(const Value& v) {
        return std::visit([]<typename T>(T&&) -> std::string {
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
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Range>>)
                return "range";
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<File>>)
                return "file";
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Array>>)
                return "array";
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Dict>>)
                return "dict";
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Function>>)
                return "function";
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<NativeFunction>>)
                return "native function";
            else
                return "unknown";
        }, v);
    }

    std::string toString(const DictKey& v) {
        return std::visit([]<typename T>(T&& value) -> std::string {
            using Decayed = std::decay_t<T>;
            if constexpr (std::is_same_v<Decayed, bool>)
                return value ? "true" : "false";
            else if constexpr (std::is_same_v<Decayed, int64_t>)
                return std::to_string(value);
            else if constexpr (std::is_same_v<Decayed, std::string>)
                return value;
            return "unknown";
        }, v);
    }

    std::string toString(const Value& v) {
        return std::visit([]<typename T>(T&& value) -> std::string {
            using Decayed = std::decay_t<T>;
            if constexpr (std::is_same_v<Decayed, std::monostate>)
                return "null";
            else if constexpr (std::is_same_v<Decayed, bool>)
                return value ? "true" : "false";
            else if constexpr (std::is_same_v<Decayed, int64_t> || std::is_same_v<Decayed, double>)
                return std::to_string(value);
            else if constexpr (std::is_same_v<Decayed, std::string>)
                return value;
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Range>>) {
                std::stringstream ss;
                ss << toString(value->from) << ".." << toString(value->to) << ":" << toString(value->step);
                return ss.str();
            } else if constexpr (std::is_same_v<Decayed, std::shared_ptr<File>>)
                return "file...";
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Array>>) {
                std::stringstream ss;
                ss << "[";
                for (std::size_t i = 0; i < value->values.size(); i++) {
                    ss << toString(value->values[i]);
                    if (i + 1 != value->values.size()) {
                        ss << ", ";
                    }
                }
                ss << "]";
                return ss.str();
            }else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Dict>>) {
                std::stringstream ss;
                ss << "{";
                size_t i = 0;
                for (const auto& [key,val] : value->values) {
                    ss << toString(key) << ": " << toString(val);
                    if (i + 1 < value->values.size()) {
                        ss << ", ";
                    }
                    i++;
                }
                ss << "}";
                return ss.str();
            } else if constexpr (std::is_same_v<Decayed, std::shared_ptr<Function>>)
                return std::format("func@{:p}", static_cast<const void*>(value->fn));
            else if constexpr (std::is_same_v<Decayed, std::shared_ptr<NativeFunction>>)
                return std::format("native_func@{:p}", static_cast<void*>(&value->fn));
            else
                return "unknown";
        }, v);
    }

}
