#include "ssengine/sdatomic.h"

#ifdef _WIN32
#  include <windows.h>
#endif

namespace SSCP {

INT32 SSAPI SDAtomicRead32(volatile INT32 *pdwMem) {
#ifdef _WIN32
    return InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(pdwMem), 0);
#else
    return __atomic_load_n(pdwMem, __ATOMIC_SEQ_CST);
#endif
}

INT64 SSAPI SDAtomicRead64(volatile INT64 *pqwMem) {
#ifdef _WIN32
    return InterlockedCompareExchange64(reinterpret_cast<volatile LONG64*>(pqwMem), 0, 0);
#else
    return __atomic_load_n(pqwMem, __ATOMIC_SEQ_CST);
#endif
}

void SSAPI SDAtomicSet32(volatile INT32 *pdwMem, INT32 dwValue) {
#ifdef _WIN32
    InterlockedExchange(reinterpret_cast<volatile LONG*>(pdwMem), static_cast<LONG>(dwValue));
#else
    __atomic_store_n(pdwMem, dwValue, __ATOMIC_SEQ_CST);
#endif
}

void SSAPI SDAtomicSet64(volatile INT64 *pqwMem, INT64 qwValue) {
#ifdef _WIN32
    InterlockedExchange64(reinterpret_cast<volatile LONG64*>(pqwMem), static_cast<LONG64>(qwValue));
#else
    __atomic_store_n(pqwMem, qwValue, __ATOMIC_SEQ_CST);
#endif
}

INT32 SSAPI SDAtomicAdd32(volatile INT32 *pdwMem, INT32 dwValue) {
#ifdef _WIN32
    return InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(pdwMem), static_cast<LONG>(dwValue));
#else
    return __atomic_fetch_add(pdwMem, dwValue, __ATOMIC_SEQ_CST);
#endif
}

INT64 SSAPI SDAtomicAdd64(volatile INT64 *pqwMem, INT64 qwValue) {
#ifdef _WIN32
    return InterlockedExchangeAdd64(reinterpret_cast<volatile LONG64*>(pqwMem), static_cast<LONG64>(qwValue));
#else
    return __atomic_fetch_add(pqwMem, qwValue, __ATOMIC_SEQ_CST);
#endif
}

INT32 SSAPI SDAtomicSub32(volatile INT32 *pdwMem, INT32 dwValue) {
    return SDAtomicAdd32(pdwMem, -dwValue);
}

INT64 SSAPI SDAtomicSub64(volatile INT64 *pqwMem, INT64 qwValue) {
    return SDAtomicAdd64(pqwMem, -qwValue);
}

INT32 SSAPI SDAtomicInc32(volatile INT32 *pdwMem) {
    return SDAtomicAdd32(pdwMem, 1);
}

INT64 SSAPI SDAtomicInc64(volatile INT64 *pqwMem) {
    return SDAtomicAdd64(pqwMem, 1);
}

INT32 SSAPI SDAtomicDec32(volatile INT32 *pdwMem) {
    return SDAtomicAdd32(pdwMem, -1);
}

INT64 SSAPI SDAtomicDec64(volatile INT64 *pqwMem) {
    return SDAtomicAdd64(pqwMem, -1);
}

INT32 SSAPI SDAtomicCas32(volatile INT32 *pdwMem, INT32 dwValue, INT32 dwCmp) {
#ifdef _WIN32
    return InterlockedCompareExchange(reinterpret_cast<volatile LONG*>(pdwMem), static_cast<LONG>(dwValue), static_cast<LONG>(dwCmp));
#else
    INT32 expected = dwCmp;
    __atomic_compare_exchange_n(pdwMem, &expected, dwValue, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return expected;
#endif
}

INT64 SSAPI SDAtomicCas64(volatile INT64 *pqwMem, INT64 qwValue, INT64 qwCmp) {
#ifdef _WIN32
    return InterlockedCompareExchange64(reinterpret_cast<volatile LONG64*>(pqwMem), static_cast<LONG64>(qwValue), static_cast<LONG64>(qwCmp));
#else
    INT64 expected = qwCmp;
    __atomic_compare_exchange_n(pqwMem, &expected, qwValue, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return expected;
#endif
}

INT32 SSAPI SDAtomicXchg32(volatile INT32 *pdwMem, INT32 dwValue) {
#ifdef _WIN32
    return InterlockedExchange(reinterpret_cast<volatile LONG*>(pdwMem), static_cast<LONG>(dwValue));
#else
    return __atomic_exchange_n(pdwMem, dwValue, __ATOMIC_SEQ_CST);
#endif
}

INT64 SSAPI SDAtomicXchg64(volatile INT64 *pqwMem, INT64 qwValue) {
#ifdef _WIN32
    return InterlockedExchange64(reinterpret_cast<volatile LONG64*>(pqwMem), static_cast<LONG64>(qwValue));
#else
    return __atomic_exchange_n(pqwMem, qwValue, __ATOMIC_SEQ_CST);
#endif
}

void* SSAPI SDAtomicCasptr(volatile void **pPtr, void *pWith, const void *pCmp) {
#ifdef _WIN32
    return InterlockedCompareExchangePointer(const_cast<PVOID*>(pPtr), pWith, const_cast<void*>(pCmp));
#else
    void* expected = const_cast<void*>(pCmp);
    __atomic_compare_exchange_n(const_cast<void**>(pPtr), &expected, pWith, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return expected;
#endif
}

void* SSAPI SDAtomicXchgptr(volatile void **pPtr, void *pWith) {
#ifdef _WIN32
    return InterlockedExchangePointer(const_cast<PVOID*>(pPtr), pWith);
#else
    return __atomic_exchange_n(const_cast<void**>(pPtr), pWith, __ATOMIC_SEQ_CST);
#endif
}

} // namespace SSCP
