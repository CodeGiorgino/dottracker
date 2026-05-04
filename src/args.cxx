#include <print>

#include "commands.hxx"
#include "utils/args.hxx"
#include "utils/error.hxx"

#define VERSION    "1.1.1"
#define ANSI_BOLD  "\033[1m"
#define ANSI_RESET "\033[0m"

auto show_version(void) noexcept -> void {
    std::println("dottracker version {}", VERSION);
}

auto show_help(void) noexcept -> void {
    std::println(
ANSI_BOLD
"NAME                                                                          \n"
ANSI_RESET
"    dottracker - dotfiles tracker                                             \n"
"                                                                              \n"
ANSI_BOLD
"SYNOPSIS                                                                      \n"
ANSI_RESET
"    dottracker [-v | --version] [-h | --help] <command> [<args>]              \n"
"                                                                              \n"
ANSI_BOLD
"DESCRIPTION                                                                   \n"
ANSI_RESET
"    dottracker is a tracking program for UNIX dotfiles and configurations.    \n"
"                                                                              \n"
ANSI_BOLD
"OPTIONS                                                                       \n"
ANSI_RESET
"    -v, --version                                                             \n"
"        Prints the program version and exit.                                  \n"
"                                                                              \n"
"    -h, --help                                                                \n"
"        Prints the program help message and exit.                             \n"
"                                                                              \n"
"    --no-color                                                                \n"
"        Disable colorized output.                                             \n"
"                                                                              \n"
ANSI_BOLD
"COMMANDS                                                                      \n"
ANSI_RESET
"    update <target> [<-s | --source> <file_path>]                             \n"
"        Update files in target:                                               \n"
"            - local: repository files overwrites local files.                 \n"
"            - repo:  local files overwrites repository files.                 \n"
"                                                                              \n"
ANSI_BOLD
"        Options:                                                              \n"
ANSI_RESET
"            -s, --source <file_path>                                          \n"
"                If specified, use the file provided for the update, otherwise \n"
"                '$HOME/.config/dottracker/files.conf' is used.                \n"
"                                                                              \n"
"    diff [<-s | --source> <file_path>]                                        \n"
"        Print a diff of the tracked files:                                    \n"
"            - OK: local and repository file are in sync.                      \n"
"            - A: local file has been added                                    \n"
"            - M: local file has been modified                                 \n"
"            - D: local file has been deleted                                  \n"
"                                                                              \n"
ANSI_BOLD
"        Options:                                                              \n"
ANSI_RESET
"            -s, --source <file_path>                                          \n"
"                If specified, use the file provided for the diff, otherwise   \n"
"                '$HOME/.config/dottracker/files.conf' is used.");
}

namespace utils {
    auto parse_args(enviroment& env, int argc, char** argv) -> void {
        env.program = *argv;

        if (argc == 1) {
            show_help();
            return;
        }

        int argn = 1;
        const auto arg_next = [&](void) -> std::string {
            if (argn < argc) {
                const std::string ret { *(argv + argn) };
                argn++;
                return ret;
            } else return {};
        };

        // parse options
        while (argn < argc) {
            const auto option = arg_next();
            if (option.empty())
                continue;
            else if (option == "-v"
                    || option == "--version") {
                show_version();
                exit(0);
            } else if (option == "-h"
                    || option == "--help") {
                show_help();
                exit(0);
            } else if (option == "--no-color") {
                env.colorized = false;
            } else if (option[0] == '-') {
                throw utils::argument_error(
                        std::format(
                            "Unknown option: {:?}.", option));
            } else {
                argn--;
                break;
            }
        };

        // parse command
        env.command = arg_next();
        if (env.command.empty())
            throw utils::argument_error("Missing required command.");
        else if (env.command == "update") {
            env.target = arg_next();
            if (env.target.empty())
                throw utils::argument_error(
                        std::format(
                            "Command {:?}: missing required value.",
                            env.command));

            // parse options
            for (; argn < argc; argn++) {
                const auto option = arg_next();
                if (option.empty())
                    continue;
                else if (option == "-s"
                        || option == "--source") {
                    env.sourcePath = arg_next();
                    if (env.sourcePath.empty())
                        throw utils::argument_error(
                                std::format(
                                    "Command {:?}: missing required value for option: {:?}.",
                                    env.command, option));
                } else throw utils::argument_error(
                        std::format(
                            "Command {:?}: unknown option: {:?}.",
                            env.command, option));
            }

            commands::update(env);
        } else if (env.command == "diff") {
            // parse options
            for (; argn < argc; argn++) {
                const auto option = arg_next();
                if (option.empty())
                    continue;
                else if (option == "-s"
                        || option == "--source") {
                    env.sourcePath = arg_next();
                    if (env.sourcePath.empty())
                        throw utils::argument_error(
                                std::format(
                                    "Command {:?}: missing required value for option: {:?}.",
                                    env.command, option));
                } else throw utils::argument_error(
                        std::format(
                            "Command {:?}: unknown option: {:?}.",
                            env.command, option));
            }

            commands::diff(env);
        } else throw utils::argument_error(
                std::format(
                    "Unknown command: {:?}.",
                    env.command));
    }
} // namespace utils
