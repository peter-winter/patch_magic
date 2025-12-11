#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <cctype>


using ExampleMap = std::unordered_map<std::string, int (*)()>;

inline ExampleMap& get_examples()
{
    static ExampleMap map;
    return map;
}

#define REGISTER_EXAMPLE(name) \
    struct name##_registrar { \
        name##_registrar() { \
            auto& map = get_examples(); \
            map[#name] = &name; \
        } \
    }; \
    static name##_registrar name##_registrar_instance
