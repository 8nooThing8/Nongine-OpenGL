#pragma once
#include <functional>
#include <memory>
#include <string>

#include "Component.h"

#include "utility"

class TypeRegistry {
public:
    using FactoryFunction = std::function<std::unique_ptr<Component>()>;

    static TypeRegistry& instance() {
        static TypeRegistry registry;
        return registry;
    }

    void registerType(const std::string& typeName, int size, FactoryFunction factory) {
        registry[typeName] = { std::move(factory), size };
    }

    std::unique_ptr<Component> create(const std::string& typeName, int& outSize) {
        auto it = registry.find(typeName);
        if (it != registry.end()) 
        {
            outSize = it->second.second;
            return it->second.first();
        }
        return nullptr;
    }

    std::unique_ptr<Component> create(const std::string& typeName) {
        int size;
        return create(typeName, size);
    }

private:
    std::unordered_map<std::string, std::pair<FactoryFunction, int>> registry;
};

#define REGISTER_TYPE(Type) \
    namespace { \
        static struct Type##Registrar { \
            Type##Registrar() { \
                TypeRegistry::instance().registerType(#Type, sizeof(Type), []() { \
                    return std::make_unique<Type>(); \
                }); \
            } \
        } Type##RegistrarInstance; \
    }