#include "ssengine/sdcsvfile.h"

#include <algorithm>
#include <cstring>

namespace SSCP {

INT32 CSDCsvBase::GetLineCount() {
    return static_cast<INT32>(m_vLine.size());
}

INT32 CSDCsvBase::GetItemCount(INT32 line) {
    if (line < 0 || line >= static_cast<INT32>(m_vLine.size())) {
        return -1;
    }
    return static_cast<INT32>(m_vLine[line].size());
}

INT32 CSDCsvBase::InsertLine(INT32 line) {
    if (line < 0 || line > static_cast<INT32>(m_vLine.size())) {
        return -1;
    }
    m_vLine.insert(m_vLine.begin() + line, std::vector<std::string>{});
    return 0;
}

INT32 CSDCsvBase::AddLine() {
    m_vLine.emplace_back();
    return static_cast<INT32>(m_vLine.size() - 1);
}

INT32 CSDCsvBase::InsertItem(INT32 line, INT32 item, const CHAR* data) {
    if (line < 0 || line >= static_cast<INT32>(m_vLine.size()) || item < 0) {
        return -1;
    }
    auto& row = m_vLine[line];
    if (item > static_cast<INT32>(row.size())) {
        return -1;
    }
    row.insert(row.begin() + item, data ? data : "");
    return 0;
}

INT32 CSDCsvBase::DelItem(INT32 line, INT32 item) {
    if (line < 0 || line >= static_cast<INT32>(m_vLine.size())) {
        return -1;
    }
    auto& row = m_vLine[line];
    if (item < 0 || item >= static_cast<INT32>(row.size())) {
        return -1;
    }
    row.erase(row.begin() + item);
    return 0;
}

INT32 CSDCsvBase::DelLine(INT32 line) {
    if (line < 0 || line >= static_cast<INT32>(m_vLine.size())) {
        return -1;
    }
    m_vLine.erase(m_vLine.begin() + line);
    return 0;
}

INT32 CSDCsvBase::EmptyItem(INT32 line, INT32 item) {
    if (line < 0 || line >= static_cast<INT32>(m_vLine.size())) {
        return -1;
    }
    auto& row = m_vLine[line];
    if (item < 0 || item >= static_cast<INT32>(row.size())) {
        return -1;
    }
    row[item].clear();
    return 0;
}

INT32 CSDCsvBase::ModifyItem(INT32 line, INT32 item, const CHAR* data) {
    if (line < 0 || line >= static_cast<INT32>(m_vLine.size())) {
        return -1;
    }
    auto& row = m_vLine[line];
    if (item < 0 || item >= static_cast<INT32>(row.size())) {
        return -1;
    }
    row[item] = data ? data : "";
    return 0;
}

INT32 CSDCsvBase::ReadData(INT32 line, INT32 item, CHAR* data, INT32 size) {
    if (!data || size <= 0) {
        return -1;
    }
    if (line < 0 || line >= static_cast<INT32>(m_vLine.size())) {
        return -1;
    }
    auto& row = m_vLine[line];
    if (item < 0 || item >= static_cast<INT32>(row.size())) {
        return -1;
    }
    const std::string& src = row[item];
    std::strncpy(data, src.c_str(), static_cast<size_t>(size) - 1);
    data[size - 1] = '\0';
    return 0;
}

template<>
INT32 CSDCsvBase::ReadData<std::string>(INT32 line, INT32 item, std::string& strItem) {
    if (line < 0 || line >= static_cast<INT32>(m_vLine.size())) {
        return -1;
    }
    auto& row = m_vLine[line];
    if (item < 0 || item >= static_cast<INT32>(row.size())) {
        return -1;
    }
    strItem = row[item];
    return 0;
}

template<>
INT32 CSDCsvBase::ReadData<std::string>(INT32 line, INT32 item, std::string* strItem) {
    if (!strItem) {
        return -1;
    }
    return ReadData<std::string>(line, item, *strItem);
}

} // namespace SSCP

