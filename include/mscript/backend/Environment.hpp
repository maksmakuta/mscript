#ifndef MSCRIPT_ENVIRONMENT_HPP
#define MSCRIPT_ENVIRONMENT_HPP

#include <memory>

#include "Value.hpp"

namespace ms {

    class Environment final : public std::enable_shared_from_this<Environment> {
    public:
        explicit Environment(std::shared_ptr<Environment> enclosing = nullptr);

        void define(const std::string& name, Value value);
        void assign(const std::string& name, Value value);

        Value get(const std::string& name);

    private:
        std::shared_ptr<Environment> m_enclosing;
        std::unordered_map<std::string, Value> m_values;
    };

}

#endif //MSCRIPT_ENVIRONMENT_HPP
