#include <format>

#include "utils/error.hxx"

namespace utils {
    system_error::system_error(std::string_view message) noexcept
        : std::runtime_error(
                std::format(
                    "-- System error - {}",
                    message)) {}

    argument_error::argument_error(std::string_view message) noexcept
        : std::invalid_argument(
                std::format(
                    "-- Argument error - {}",
                    message)) {}

    command_error::command_error(std::string_view message) noexcept
        : std::runtime_error(
                std::format(
                    "-- Command error - {}",
                    message)) {}

    parser_error::parser_error(std::string_view message) noexcept
        : std::runtime_error(
                std::format(
                    "-- Parser error - {}",
                    message)) {}
} // namespace utils
