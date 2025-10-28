#include "ssengine/sdrandom.h"

#include <chrono>
#include <mutex>
#include <random>

namespace SSCP {

namespace {
std::mt19937& globalEngine() {
    static std::mt19937 engine(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    return engine;
}
std::mutex& engineMutex() {
    static std::mutex mtx;
    return mtx;
}
}

void SDSetSeed(UINT32 dwSeed) {
    std::lock_guard<std::mutex> lk(engineMutex());
    globalEngine().seed(dwSeed);
}

INT32 SDRandom() {
    std::lock_guard<std::mutex> lk(engineMutex());
    std::uniform_int_distribution<UINT32> dist(0, 0xFFFFFFFFu);
    return static_cast<INT32>(dist(globalEngine()));
}

UINT32 SDRandom(UINT32 dwRange) {
    if (dwRange == 0) {
        return 0;
    }
    std::lock_guard<std::mutex> lk(engineMutex());
    std::uniform_int_distribution<UINT32> dist(0, dwRange - 1);
    return dist(globalEngine());
}

CSDRandom::CSDRandom() {
    auto seed = static_cast<UINT32>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    SetSeed(seed);
}

UINT32 CSDRandom::Random() {
    // simple linear congruential generator
    m_seed = static_cast<UINT32>(1664525u * m_seed + 1013904223u);
    return m_seed;
}

UINT32 CSDRandom::Random(UINT32 dwRange) {
    if (dwRange == 0) {
        return 0;
    }
    return Random() % dwRange;
}

UINT32 CSDRandom::Random(UINT32 dwMin, UINT32 dwMax) {
    if (dwMax <= dwMin) {
        return dwMin;
    }
    UINT32 span = dwMax - dwMin;
    return dwMin + Random(span);
}

} // namespace SSCP
