#include <format>
#include <fstream>
#include <ranges>

#include "parser.hxx"
#include "utils/error.hxx"

namespace fs = std::filesystem;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

parser::parser(fs::path filePath) :
    _filePath(fs::absolute(filePath)) {
        if (!fs::exists(_filePath))
            throw utils::parser_error(
                    std::format(
                        "Cannot find file: [{}].",
                        _filePath.string()));
        else if (!fs::is_regular_file(_filePath))
            throw utils::parser_error(
                    std::format(
                        "File [{}]: not a regular file.",
                        _filePath.string()));
    }

auto trim(std::string_view str) noexcept -> std::string_view {
    if (str.empty())
        return {};

    size_t start = 0;
    size_t end   = str.size() - 1;
    for (; start < end && std::isspace(str[start]); start++);
    for (; end > start && std::isspace(str[end]); end--);

    return str.substr(start, end - start + 1);
}

auto parser::lines(void) noexcept -> std::generator<std::string> {
    std::ifstream streamIn { _filePath };
    if (!streamIn)
        throw utils::parser_error(
                std::format(
                    "Cannot open file: [{}].",
                    _filePath.string()));

    std::string line;
    while (std::getline(streamIn, line)) {
        if (line.empty())
            continue;

        line = trim(line);
        if (line.empty())
            continue;
        else if (line[0] == '#')
            continue;

        fs::path realFilePath {};
        if (line[0] == '/')
            realFilePath /= "/";

        for (const auto part : views::split(line, '/')) {
            const std::string partStr { std::string_view { part } };
            if (part.empty())
                continue;
            else if (part[0] == '$') {
                const auto realPart = std::getenv(
                        partStr.substr(1).c_str());
                if (!realPart)
                    throw utils::parser_error(
                            std::format(
                                "[{}:{}]: cannot evaluate enviroment variable: {:?}.",
                                _filePath.string(), line, partStr));
                else realFilePath /= std::string { realPart };
            } else realFilePath /= partStr;
        }

        co_yield realFilePath.string();
    };
}
