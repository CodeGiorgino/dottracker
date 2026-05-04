#include <iostream>
#include <print>

#include "utils/args.hxx"

auto main(int argc, char** argv) -> int {
    enviroment env;
    try {
        utils::parse_args(env, argc, argv);
    } catch (const std::invalid_argument& ex) {
        const std::string err = ex.what();
        std::println(std::cerr,
                "{}\n"
                "-- Run 'dottracker --help' for all supported commands",
                err);
        return 1;
    } catch (const std::exception& ex) {
        const std::string err = ex.what();
        std::println(std::cerr, "{}", err);
        return 1;
    }

    return 0;
}
