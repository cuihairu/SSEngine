// Server ID packing/unpacking helpers implementation.
// Implements functions and classes declared in include/ssengine/sdserverid.h

#include "ssengine/sdserverid.h"
#include <sstream>
#include <vector>
#include <cstring>

namespace SSCP {

namespace {

inline void parseMask(UINT32 mask, UINT32 lens[4], UINT32 shifts[4], UINT64 submasks[4], UINT32 totalBits)
{
    // Extract 4 one-byte fields: high byte is part1 length in bits.
    lens[0] = (mask >> 24) & 0xFF;
    lens[1] = (mask >> 16) & 0xFF;
    lens[2] = (mask >> 8)  & 0xFF;
    lens[3] = (mask >> 0)  & 0xFF;

    UINT32 pos = totalBits; // bit cursor from MSB to LSB
    for (int i = 0; i < 4; ++i) {
        UINT32 L = lens[i] > totalBits ? totalBits : lens[i];
        if (L > pos) L = pos; // clamp when sum exceeds totalBits
        pos -= L;
        shifts[i] = pos;
        if (L == 0) submasks[i] = 0;
        else if (L >= 64) submasks[i] = ~UINT64(0);
        else submasks[i] = (UINT64(1) << L) - 1;
        lens[i] = L; // stored clamped len
    }
}

inline std::vector<UINT32> splitParts(const char* s)
{
    std::vector<UINT32> parts; parts.reserve(4);
    if (!s) return parts;
    const char* p = s;
    while (*p && parts.size() < 4) {
        // parse unsigned int until '-' or end
        UINT32 v = 0;
        bool any = false;
        while (*p && *p != '-') {
            if (*p >= '0' && *p <= '9') { any = true; v = v * 10 + UINT32(*p - '0'); }
            ++p;
        }
        parts.push_back(any ? v : 0u);
        if (*p == '-') ++p; // skip delimiter
    }
    while (parts.size() < 4) parts.push_back(0);
    return parts;
}

} // anonymous

std::string SSAPI SDServerIDUtoa(UINT32 dwServerID, UINT32 dwMask)
{
    UINT32 lens[4]{}; UINT32 shifts[4]{}; UINT64 subs[4]{};
    parseMask(dwMask, lens, shifts, subs, 32);
    UINT32 parts[4]{};
    for (int i = 0; i < 4; ++i) {
        if (lens[i] == 0) { parts[i] = 0; continue; }
        UINT32 v = (dwServerID >> shifts[i]) & static_cast<UINT32>(subs[i]);
        parts[i] = v;
    }
    std::ostringstream oss; oss << parts[0] << '-' << parts[1] << '-' << parts[2] << '-' << parts[3];
    return oss.str();
}

UINT32 SSAPI SDServerIDAtou(const CHAR* pszServerID, UINT32 dwMask)
{
    UINT32 lens[4]{}; UINT32 shifts[4]{}; UINT64 subs[4]{};
    parseMask(dwMask, lens, shifts, subs, 32);
    auto parts = splitParts(pszServerID);
    UINT32 id = 0;
    for (int i = 0; i < 4; ++i) {
        UINT32 v = (i < static_cast<int>(parts.size())) ? parts[i] : 0u;
        v &= static_cast<UINT32>(subs[i]);
        id |= (v << shifts[i]);
    }
    return id;
}

// ----- CSDServerId -----

CSDServerId::CSDServerId(UINT32 dwId, UINT32 dwMask)
    : m_id(dwId), m_mask(dwMask)
{
    // Precompute mask lengths and sub masks
    m_maskLen[0] = (dwMask >> 24) & 0xFF;
    m_maskLen[1] = (dwMask >> 16) & 0xFF;
    m_maskLen[2] = (dwMask >> 8)  & 0xFF;
    m_maskLen[3] = (dwMask >> 0)  & 0xFF;
    for (int i = 0; i < 4; ++i) m_subMask[i] = (m_maskLen[i] == 0) ? 0 : ((m_maskLen[i] >= 32) ? 0xFFFFFFFFu : ((1u << m_maskLen[i]) - 1));
    m_strId = SDServerIDUtoa(m_id, m_mask);
}

void SSAPI CSDServerId::SetID(UINT32 dwID, UINT32 dwMask)
{
    m_id = dwID; m_mask = dwMask; *this = CSDServerId(dwID, dwMask);
}

void SSAPI CSDServerId::SetID(const CHAR* pszID, UINT32 dwMask)
{
    m_mask = dwMask; *this = CSDServerId(SDServerIDAtou(pszID, dwMask), dwMask);
}

void SSAPI CSDServerId::SetMask(UINT32 dwMask)
{
    *this = CSDServerId(m_id, dwMask);
}

UINT32 SSAPI CSDServerId::GetMask()
{
    return m_mask;
}

static inline UINT32 extract_part32(UINT32 id, const UINT32 lens[4], const UINT32 mask, int idx)
{
    UINT32 shifts[4]{}; UINT64 subs[4]{}; UINT32 tmpLens[4];
    tmpLens[0] = (mask >> 24) & 0xFF; tmpLens[1] = (mask >> 16) & 0xFF; tmpLens[2] = (mask >> 8) & 0xFF; tmpLens[3] = (mask) & 0xFF;
    parseMask(mask, tmpLens, shifts, subs, 32);
    (void)lens;
    return static_cast<UINT32>((id >> shifts[idx]) & static_cast<UINT32>(subs[idx]));
}

UINT32 SSAPI CSDServerId::GetAreaId() { return extract_part32(m_id, m_maskLen, m_mask, 0); }
UINT32 SSAPI CSDServerId::GetGroupId() { return extract_part32(m_id, m_maskLen, m_mask, 1); }
UINT32 SSAPI CSDServerId::GetServerType() { return extract_part32(m_id, m_maskLen, m_mask, 2); }
UINT32 SSAPI CSDServerId::GetServerIndex() { return extract_part32(m_id, m_maskLen, m_mask, 3); }

std::string SSAPI CSDServerId::AsString() { m_strId = SDServerIDUtoa(m_id, m_mask); return m_strId; }
UINT32 SSAPI CSDServerId::AsNumber() { return m_id; }

// ----- 64-bit extended versions -----

std::string SSAPI SDServerIDExUtoa(UINT64 dwServerID, UINT32 dwMask)
{
    UINT32 lens[4]{}; UINT32 shifts[4]{}; UINT64 subs[4]{};
    parseMask(dwMask, lens, shifts, subs, 64);
    UINT64 parts[4]{};
    for (int i = 0; i < 4; ++i) {
        if (lens[i] == 0) { parts[i] = 0; continue; }
        UINT64 v = (dwServerID >> shifts[i]) & subs[i];
        parts[i] = v;
    }
    std::ostringstream oss; oss << parts[0] << '-' << parts[1] << '-' << parts[2] << '-' << parts[3];
    return oss.str();
}

UINT64 SSAPI SDServerIDExAtou(const CHAR* pszServerID, UINT32 dwMask)
{
    UINT32 lens[4]{}; UINT32 shifts[4]{}; UINT64 subs[4]{};
    parseMask(dwMask, lens, shifts, subs, 64);
    auto parts = splitParts(pszServerID);
    UINT64 id = 0;
    for (int i = 0; i < 4; ++i) {
        UINT64 v = (i < static_cast<int>(parts.size())) ? parts[i] : 0u;
        v &= subs[i];
        id |= (v << shifts[i]);
    }
    return id;
}

CSDServerIdEx::CSDServerIdEx(UINT64 dwId, UINT32 dwMask)
    : m_mask(dwMask), m_id(dwId)
{
    m_maskLen[0] = (dwMask >> 24) & 0xFF;
    m_maskLen[1] = (dwMask >> 16) & 0xFF;
    m_maskLen[2] = (dwMask >> 8)  & 0xFF;
    m_maskLen[3] = (dwMask >> 0)  & 0xFF;
    for (int i = 0; i < 4; ++i) m_subMask[i] = (m_maskLen[i] == 0) ? 0 : ((m_maskLen[i] >= 32) ? 0xFFFFFFFFu : ((1u << m_maskLen[i]) - 1));
    m_strId = SDServerIDExUtoa(m_id, m_mask);
}

void SSAPI CSDServerIdEx::SetID(UINT64 dwID, UINT32 dwMask)
{
    *this = CSDServerIdEx(dwID, dwMask);
}

void SSAPI CSDServerIdEx::SetID(const CHAR* pszID, UINT32 dwMask)
{
    *this = CSDServerIdEx(SDServerIDExAtou(pszID, dwMask), dwMask);
}

void SSAPI CSDServerIdEx::SetMask(UINT32 dwMask)
{
    *this = CSDServerIdEx(m_id, dwMask);
}

UINT32 SSAPI CSDServerIdEx::GetMask()
{
    return m_mask;
}

static inline UINT32 extract_part64(UINT64 id, UINT32 mask, int idx)
{
    UINT32 lens[4]{}; UINT32 shifts[4]{}; UINT64 subs[4]{};
    parseMask(mask, lens, shifts, subs, 64);
    return static_cast<UINT32>((id >> shifts[idx]) & subs[idx]);
}

UINT32 SSAPI CSDServerIdEx::GetGameId() { return extract_part64(m_id, m_mask, 0); }
UINT32 SSAPI CSDServerIdEx::GetAreaId() { return extract_part64(m_id, m_mask, 0); }
UINT32 SSAPI CSDServerIdEx::GetGroupId() { return extract_part64(m_id, m_mask, 1); }
UINT32 SSAPI CSDServerIdEx::GetServerType() { return extract_part64(m_id, m_mask, 2); }
UINT32 SSAPI CSDServerIdEx::GetServerIndex() { return extract_part64(m_id, m_mask, 3); }

std::string SSAPI CSDServerIdEx::AsString() { m_strId = SDServerIDExUtoa(m_id, m_mask); return m_strId; }
UINT64 SSAPI CSDServerIdEx::AsNumber() { return m_id; }

} // namespace SSCP

