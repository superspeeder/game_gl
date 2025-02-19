//
// Created by andy on 2/19/2025.
//

#pragma once

#include <exception>
#include <string>
#include <format>

namespace game {
    namespace exception {
        constexpr char shader_compile_error[] = "shader compilation error";
        constexpr char shader_link_error[] = "shader link error";
    }

    template<const char* kind>
    class generic_exception : public std::exception {
    public:
        explicit generic_exception(const std::string& message) : message(std::format("{}: {}", kind, message)) {};

      [[nodiscard]] inline const char *what() const override { return message.c_str(); };

    private:
        std::string message;
    };

    namespace render {
        class shader_compile_error final : public generic_exception<exception::shader_compile_error> {
          public:
            explicit shader_compile_error(const std::string &message) : generic_exception(message) {}
        };

        class shader_link_error final : public generic_exception<exception::shader_link_error> {
        public:
            explicit shader_link_error(const std::string &message) : generic_exception(message) {}
        };
    }

}