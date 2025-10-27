/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

 This file defines a set of wrapper functions used to access the shared memory.

******************************************************************************/

#ifndef SSCP_SDSHM_H_20070614
#define SSCP_SDSHM_H_20070614

#include "sdbase.h"

namespace SSCP
{
    enum ESDShmErrCode
    {
        SDSHM_SUCCESS=0,        ///< operator success
        SDSHM_ALREADYINUSE=-8,  ///< the key has been used
        SDSHM_OVERSIZE,         ///< out of memory
        SDSHM_INVALIDPARAM      ///< used wrong param
    };

    ///
    /// @fn     void* SSAPI SSCP::SGSYS_NewShm(INT32 nKey, UINT32 dwSize, INT32* pnErrno) 
    /// @brief  Create a shared memory object.
    /// @param[in]  nKey    the key id of the shared memory object
    /// @param[in]  dwSize  the size of the shared memory
    /// @param[out] pnErrno a error code if failure
    /// @return a handle to the shared memory object.
    ///       User should use SHM_HANDLE_CHECK to check if the function failed.
    void* SSAPI SGSYS_NewShm(INT32 nKey, UINT32 dwSize, INT32* pnErrno);

    ///
    /// @fn     INT32 SSAPI SSCP::SGSYS_DeleteShm(INT32 nKey) 
    /// @brief  Close the handle of the shared memory object by the key id.
    /// @param[in] nKey the key id of the shared memory object
    /// @return  1 if success, or -1 if failed.
    INT32 SSAPI SGSYS_DeleteShm(INT32 nKey);
}

#endif
