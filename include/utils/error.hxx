#pragma once
#include <stdexcept>
#include <string>

namespace utils {
    class system_error final : public std::runtime_error {
        public:
            system_error(std::string_view message = { "Unknown error." }) noexcept;
    };

    class argument_error final : public std::invalid_argument {
        public:
            argument_error(std::string_view message = { "Unknown error." }) noexcept;
    };

    class command_error final : public std::runtime_error {
        public:
            command_error(std::string_view message = { "Unknown error." }) noexcept;
    };

    class parser_error final : public std::runtime_error {
        public:
            parser_error(std::string_view message = { "Unknown error." }) noexcept;
    };
} // namespace utils
