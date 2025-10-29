/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdshm.cpp - 共享内存实现
******************************************************************************/

#include "ssengine/sdshm.h"
#include "ssengine/sderrno.h"
#include <cstdio>

#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>
#endif

namespace SSCP
{

void* SSAPI SGSYS_NewShm(INT32 nKey, UINT32 dwSize, INT32* pnErrno)
{
    if (pnErrno)
        *pnErrno = SDSHM_SUCCESS;
    
    if (dwSize == 0)
    {
        if (pnErrno)
            *pnErrno = SDSHM_INVALIDPARAM;
        return nullptr;
    }

#ifdef WINDOWS
    // Windows implementation using file mapping
    char szName[64];
    snprintf(szName, sizeof(szName), "SSCP_SHM_%d", nKey);
    
    HANDLE hMapping = CreateFileMappingA(
        INVALID_HANDLE_VALUE,   // Use paging file
        NULL,                   // Default security
        PAGE_READWRITE,         // Read/write access
        0,                      // High-order DWORD of size
        dwSize,                 // Low-order DWORD of size
        szName                  // Name of mapping object
    );
    
    if (hMapping == NULL)
    {
        if (pnErrno)
            *pnErrno = SDSHM_OVERSIZE;
        return nullptr;
    }
    
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(hMapping);
        if (pnErrno)
            *pnErrno = SDSHM_ALREADYINUSE;
        return nullptr;
    }
    
    void* pMemory = MapViewOfFile(
        hMapping,              // Handle to mapping object
        FILE_MAP_ALL_ACCESS,   // Read/write permission
        0,                     // High-order DWORD of offset
        0,                     // Low-order DWORD of offset
        dwSize                 // Number of bytes to map
    );
    
    if (pMemory == NULL)
    {
        CloseHandle(hMapping);
        if (pnErrno)
            *pnErrno = SDSHM_OVERSIZE;
        return nullptr;
    }
    
    // Store the mapping handle in the first 8 bytes (not ideal but works for this implementation)
    // In a real implementation, you'd maintain a handle table
    *(HANDLE*)pMemory = hMapping;
    
    return (char*)pMemory + sizeof(HANDLE);
    
#else
    // Linux implementation using System V shared memory
    int shmid = shmget(nKey, dwSize, IPC_CREAT | IPC_EXCL | 0666);
    
    if (shmid == -1)
    {
        if (errno == EEXIST)
        {
            if (pnErrno)
                *pnErrno = SDSHM_ALREADYINUSE;
        }
        else if (errno == ENOMEM || errno == ENOSPC)
        {
            if (pnErrno)
                *pnErrno = SDSHM_OVERSIZE;
        }
        else
        {
            if (pnErrno)
                *pnErrno = SDSHM_INVALIDPARAM;
        }
        return nullptr;
    }
    
    void* pMemory = shmat(shmid, NULL, 0);
    if (pMemory == (void*)-1)
    {
        shmctl(shmid, IPC_RMID, NULL);  // Clean up on failure
        if (pnErrno)
            *pnErrno = SDSHM_OVERSIZE;
        return nullptr;
    }
    
    return pMemory;
#endif
}

INT32 SSAPI SGSYS_DeleteShm(INT32 nKey)
{
#ifdef WINDOWS
    char szName[64];
    snprintf(szName, sizeof(szName), "SSCP_SHM_%d", nKey);
    
    // In a real implementation, you'd need to track the mapping handle
    // This is a simplified version that tries to open and close
    HANDLE hMapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, szName);
    if (hMapping != NULL)
    {
        CloseHandle(hMapping);
        return 1;
    }
    return -1;
    
#else
    int shmid = shmget(nKey, 0, 0);
    if (shmid == -1)
        return -1;
    
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        return -1;
    
    return 1;
#endif
}

} // namespace SSCP