#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>

#include "commands.hxx"
#include "parser.hxx"
#include "utils/error.hxx"

#define ANSI_BOLD   "\033[1m"
#define ANSI_RESET  "\033[0m"
#define ANSI_RED    "\033[91m"
#define ANSI_GREEN  "\033[92m"
#define ANSI_YELLOW "\033[93m"

namespace fs = std::filesystem;

auto get_repo_files_path(const enviroment& env) -> fs::path {
    return {
        fs::absolute(env.program).parent_path() / "files"
    };
}

auto get_config_file_path(const enviroment& env) -> fs::path {
    if (env.sourcePath.empty()) {
        const auto homePathStr { std::getenv("HOME") };
        if (!homePathStr)
            throw utils::system_error(
                    "Cannot find home path: try setting 'HOME' enviroment variable,\n"
                    "  or provide a source file with the '-s | --source' option.");

        return {
            fs::path { homePathStr } / ".config" / "dottracker" / "files.conf"
        };
    } else return env.sourcePath;
}

namespace commands {
    auto update(const enviroment& env) -> void {
        const auto repoFilesPath = get_repo_files_path(env);
        if (!fs::exists(repoFilesPath))
            throw utils::command_error(
                    std::format(
                        "Command {:?}: cannot find repository files directory: [{}].",
                        env.command, repoFilesPath.string()));

        parser p { get_config_file_path(env).string() };
        for (const auto& filePath : p.lines()) {
            const std::string parentPath { fs::path { filePath }.parent_path() };
            const std::string filename { fs::path { filePath }.filename() };

            const std::string hash {
                std::format("{:0>20}", std::hash<std::string> {}(filePath))
            };

            const auto repoFilePath { repoFilesPath / hash };

            if (env.target == "local") {
                if (!fs::exists(repoFilePath)) {
                    std::println(std::cerr,
                            "-- Cannot find repository file: [{}] [{}].",
                            hash, repoFilePath.string());
                    continue;
                }

                if (!fs::exists(parentPath)
                        && !fs::create_directories(parentPath)) {
                    std::println(std::cerr,
                            "-- File [{}]: cannot create local path: [{}].",
                            filename, parentPath);
                    continue;
                }

                std::println(std::clog,
                        "-- Updating local file: [{:20}] [{}].", filename,
                        repoFilePath.string());
                fs::copy(repoFilePath, filePath,
                        fs::copy_options::overwrite_existing);
            } else if (env.target == "repo") {
                if (!fs::exists(filePath)) {
                    std::println(std::cerr,
                            "-- Cannot find local file: [{}] [{}].",
                            env.command, filename, parentPath);
                    continue;
                }

                std::println(std::clog,
                        "-- Updating repository file: [{:20}] [{}].", hash,
                        repoFilePath.string());
                fs::copy(filePath, repoFilePath,
                        fs::copy_options::overwrite_existing);
            } else throw utils::command_error(
                    std::format(
                        "Command {:?}: unknown target: {:?}.",
                        env.command, env.target));
        }
    }

    auto diff(const enviroment& env) -> void {
        const auto repoFilesPath = get_repo_files_path(env);
        if (!fs::exists(repoFilesPath))
            throw utils::command_error(
                    std::format(
                        "Command {:?}: cannot find repository files directory: [{}].",
                        env.command, repoFilesPath.string()));

        parser p { get_config_file_path(env).string() };

        std::println("{:30} | {:28} | {}", ANSI_BOLD "Filename" ANSI_RESET,
                ANSI_BOLD "Hash" ANSI_RESET, ANSI_BOLD "Status" ANSI_RESET);
        for (const auto& filePath : p.lines()) {
            const std::string parentPath { fs::path { filePath }.parent_path() };
            const std::string filename { fs::path { filePath }.filename() };

            const std::string hash {
                std::format("{:0>20}", std::hash<std::string> {}(filePath))
            };

            const auto repoFilePath { repoFilesPath / hash };

            std::print("[{:20}] | {} | ", filename, hash);

            // check if file is missing
            if (!fs::exists(repoFilePath)) {
                if (env.colorized)
                    std::print(ANSI_YELLOW);
                std::println(ANSI_BOLD "A" ANSI_RESET);
                continue;
            } else if (!fs::exists(filePath)) {
                if (env.colorized)
                    std::print(ANSI_RED);
                std::println(ANSI_BOLD "D" ANSI_RESET);
                continue;
            }

            std::ifstream localStream { filePath };
            if (!localStream) {
                std::println(std::cerr,
                        "-- Cannot open local file: [{}].",
                        filePath);
                continue;
            }

            std::ifstream repoStream { repoFilePath.string() };
            if (!repoStream) {
                std::println(std::cerr,
                        "-- Cannot open repository file: [{}].",
                        repoFilePath.string());
                continue;
            }

            // check for size mismatch
            if (fs::file_size(fs::path { filePath })
                    != fs::file_size(repoFilePath)) {
                if (env.colorized)
                    std::print(ANSI_YELLOW);
                std::println(ANSI_BOLD "M" ANSI_RESET);
                continue;
            }

            // check for content mismatch
            if (std::equal(
                        std::istreambuf_iterator<char>(localStream.rdbuf()),
                        std::istreambuf_iterator<char>(),
                        std::istreambuf_iterator<char>(repoStream.rdbuf())
                    )) {
                if (env.colorized)
                    std::print(ANSI_GREEN);
                std::println(ANSI_BOLD "OK" ANSI_RESET);
            } else {
                if (env.colorized)
                    std::print(ANSI_YELLOW);
                std::println(ANSI_BOLD "M" ANSI_RESET);
            }
        }
    }
} // namespace commands
