#include "mscript/backend/Environment.hpp"

namespace ms {

    Environment::Environment(std::shared_ptr<Environment> enclosing)
                : m_enclosing(std::move(enclosing)) {}

    void Environment::define(const std::string& name, Value value) {
        m_values[name] = std::move(value);
    }

    Value Environment::get(const std::string& name) {
        if (m_values.contains(name)) {
            return m_values.at(name);
        }

        if (m_enclosing) {
            return m_enclosing->get(name);
        }

        throw std::runtime_error("Undefined variable '" + name + "'.");
    }

    void Environment::assign(const std::string& name, Value value) {
        if (m_values.contains(name)) {
            m_values[name] = std::move(value);
            return;
        }

        if (m_enclosing) {
            m_enclosing->assign(name, std::move(value));
            return;
        }

        throw std::runtime_error("Undefined variable '" + name + "'.");
    }

}