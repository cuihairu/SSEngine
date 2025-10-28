/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdregex.h - Regular expression utilities.
******************************************************************************/

#ifndef SDREGEX_H
#define SDREGEX_H

#include "sdmacros.h"
#include "sdtype.h"
#include <string>
#include <vector>

namespace SSCP {

using SMatchResult = std::vector<std::string>;

BOOL SSAPI SDRegexMatch(const CHAR* pzStr, SMatchResult& result, const CHAR* pattern);
BOOL SSAPI SDRegexMatch(const std::string& pzStr, SMatchResult& result, const std::string& pattern);
std::string SSAPI SDRegexReplace(const CHAR* pzStr, const CHAR* pattern, const CHAR* format);
std::string SSAPI SDRegexReplace(const std::string& pzStr, const std::string& pattern, const std::string& format);

} // namespace SSCP

#endif // SDREGEX_H
