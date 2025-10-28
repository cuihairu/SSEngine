#include "ssengine/sdregex.h"

#include <regex>
#include <string_view>

namespace SSCP {
namespace {

bool RegexMatchImpl(std::string_view input, SMatchResult& result, std::string_view pattern) {
    result.clear();
    try {
        std::regex re(pattern.begin(), pattern.end());
        std::match_results<std::string_view::const_iterator> matches;
        if (std::regex_match(input.begin(), input.end(), matches, re)) {
            for (size_t i = 0; i < matches.size(); ++i) {
                result.emplace_back(matches[i].first, matches[i].second);
            }
            return true;
        }
    } catch (const std::regex_error&) {
        result.clear();
    }
    return false;
}

std::string RegexReplaceImpl(std::string_view input, std::string_view pattern, std::string_view format) {
    try {
        std::regex re(pattern.begin(), pattern.end());
        return std::regex_replace(std::string(input), re, std::string(format));
    } catch (const std::regex_error&) {
        return std::string(input);
    }
}

} // namespace

BOOL SSAPI SDRegexMatch(const CHAR* pzStr, SMatchResult& result, const CHAR* pattern) {
    if (!pzStr || !pattern) {
        result.clear();
        return false;
    }
    return RegexMatchImpl(pzStr, result, pattern);
}

BOOL SSAPI SDRegexMatch(const std::string& pzStr, SMatchResult& result, const std::string& pattern) {
    return RegexMatchImpl(pzStr, result, pattern);
}

std::string SSAPI SDRegexReplace(const CHAR* pzStr, const CHAR* pattern, const CHAR* format) {
    if (!pzStr || !pattern || !format) {
        return {};
    }
    return RegexReplaceImpl(pzStr, pattern, format);
}

std::string SSAPI SDRegexReplace(const std::string& pzStr, const std::string& pattern, const std::string& format) {
    return RegexReplaceImpl(pzStr, pattern, format);
}

} // namespace SSCP
