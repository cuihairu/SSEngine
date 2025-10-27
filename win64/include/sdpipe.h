/******************************************************************************
            Copyright (C) YoFei Corporation. All rights reserved.

 sdpipe.h - Interfaces, classes, macros, types and definitions for pipe module.
            Include file for sdcore.dll. This library is based on net module
            and provide much user friendly API. It provides message-oriented 
            communication as well as connection reestablishing functionality.

 Class    - ISSPipeModule
          - ISSPipeReporter
          - ISSPipe
          - ISSPipeSink
          
 Struct   - ISSPipeId


 Remarks  - SDPipe module is based on SDNet module and provides an easy to use
            network programming interface. It reliefs users from network 
            programming details such as connection establishment, partial data
            sending/receiving, reconnecting etc, so users only need to concentr-
            ate on their business logics.

            SDPipe is pure virtual C++ function based. It contains a set of pure
            virtual C++ class, include both interfaces implemented inside SDPipe
            and callback interfaces which to be implemented by user.
            
******************************************************************************/

#ifndef SSCP_SDPIPE_H_20070613
#define SSCP_SDPIPE_H_20070613

#include "sdbase.h"
#include "sdnet.h"
#include "sdnet_ver.h"
#include <sstream>

namespace SSCP{


//
// SDPipe module name
//
const char SDPIPE_MODULENAME[] = "SSPipe";

//
// SDPipe version
//
const SSSVersion	SDPIPE_VERSION = SDNET_MODULE_VERSION;

//
// Error code of SDPipe
//
enum ESDPipeCode{
    //
    // Remote pipe id error
    //
    PIPE_REMOTEID_ERR   = -3,

    //
    // Duplicated pipe connection
    //
    PIPE_REPEAT_CONN    = -2,  

    //
    // Pipe disconnected
    //
    PIPE_DISCONNECT     = -1,  

    //
    // Pipe connected
    //
    PIPE_SUCCESS        = 0     
};

//
// Default parameters¡£
//

//
// Default send buffer size
//
#define PIPE_DFLT_SENDBUF_SIZE        (0x00000001<<12)

//
// Default receive buffer size
//
#define PIPE_DFLT_RECVBUF_SIZE       PIPE_DFLT_SENDBUF_SIZE

//
// Pipe id elements
//
struct SSDPipeId
{
	SSDPipeId() :byRegion(0), byGroup(0), byType(0), byIndex(0) {}

	UINT32 byRegion:8;      // Region
	UINT32 byGroup:16;      // Group
	UINT32 byType:4;        // Type
    UINT32 byIndex:4;       // Index

    std::string Info() {
        std::stringstream ss;
        ss << byRegion << "-"
            << byGroup << "-"
            << byType << "-"
            << byIndex;
        return ss.str();
    }

};

// 
// Name     : ISSPipeSink
// Function : Connection level callback interface.
//
class ISSPipeSink
{
public:
    virtual ~ISSPipeSink() {}

    //
    // Name     : OnRecv
    // Function : Data arriving callback.
    //
    virtual void SSAPI OnRecv(UINT16 wBusinessID, const char* pData, UINT32 dwLen) = 0;

    //
    // Name     : OnReport
    // Function : Pipe status changed callback.
    //
    virtual void SSAPI OnReport(UINT16 wBusinessID, INT32 nErrCode) = 0;

};

// 
// Name     : ISSPipe
// Function : Represent a remote pipe.
//
class ISSPipe
{
public:
    virtual ~ISSPipe() {}
    //
    // Name     : GetID
    // Function : Get remote pipe id.
    //
    virtual UINT32 SSAPI GetID(void) = 0;

    //
    // Name     : Send
    // Function : Send data to remote pipe.
    //
    virtual bool SSAPI Send(UINT16 wBusinessID, const char* pData, UINT32 dwLen) = 0;

    //
    // Name     : SetUserData
    // Function : Associate user data for business id.
    //
    virtual void SSAPI SetUserData(UINT16 wBusinessID, UINT32 dwData) = 0;

    //
    // Name     : GetUserData
    // Function : Get user data for business id.
    //
    virtual bool SSAPI GetUserData(UINT16 wBusinessID, UINT32* pdwData) = 0;

    //
    // Name     : SetSink
    // Function : Set connection level callback for business id.
    //
    virtual bool SSAPI SetSink(UINT16 wBusinessID, ISSPipeSink* pSink) = 0;

    //
    // Name     : GetSink
    // Function : Get connection level callback for business id.
    //
    virtual ISSPipeSink* SSAPI GetSink(UINT16 wBusinessID) = 0;

    //
    // Name     : GetIP
    // Function : Get ip for remote ip.
    //
    virtual UINT32 SSAPI GetIP(void) = 0;

	//close remote pipe.
	virtual void SSAPI Close(void) = 0;

};

// 
// Name     : ISSPipeReporter
// Function : Module level callback.
//
class ISSPipeReporter
{
public:
    virtual ~ISSPipeReporter() {}
    //
    // Name     : OnReport
    // Function : Pipe status changed callback.
    //
    virtual void SSAPI OnReport(INT32 nErrCode, UINT32 dwID) = 0;

};

// 
// Name     : ISSPipeReporter
// Function : Represent a pipe module(a local pipe).
//
class ISSPipeModule : public ISSBase
{
public:
    //
    // Name     : Init
    // Function : Init pipe module.
    //
    virtual bool SSAPI Init(const char* pszConfFile, const char* pszIPListFile, ISSPipeReporter* pReporter, ISSNet* pNetModule) = 0;

    //
    // Name     : GetPipe
    // Function : Find remote pipe by id.
    //
    virtual ISSPipe* SSAPI GetPipe(UINT32 dwID) = 0;

    //
    // Name     : Run
    // Function : Process pipe event.
    //
    virtual bool SSAPI Run(INT32 nCount = -1) = 0;

	//
	// Name		: ReplaceConn
	// Function	: Replace a connecting pipe.
	//
	virtual bool SSAPI ReplaceConn(UINT32 dwID, const char* pszRemoteIP, UINT16 wRemotePort, UINT32 dwRecvBuf = PIPE_DFLT_RECVBUF_SIZE, UINT32 dwSendBuf = PIPE_DFLT_SENDBUF_SIZE) = 0;

    //
    // Name     : AddConn
    // Function : Add a actively connecting pipe.
    //
    virtual bool SSAPI AddConn(UINT32 dwID, const char* pszRemoteIP, UINT16 wRemotePort, UINT32 dwRecvBuf=PIPE_DFLT_RECVBUF_SIZE, UINT32 dwSendBuf=PIPE_DFLT_SENDBUF_SIZE) = 0;

    //
    // Name     : RemoveConn
    // Function : Remove a actively connected pipe.
    //
    virtual bool SSAPI RemoveConn(UINT32 dwID)=0;

 
    //
    // Name     : AddListen
    // Function : Add a passively connecting pipe.
    //
    virtual bool SSAPI AddListen(const char* pszLocalIP, UINT16 wLocalPort, UINT32 dwRecvBuf=PIPE_DFLT_RECVBUF_SIZE, UINT32 dwSendBuf=PIPE_DFLT_SENDBUF_SIZE) = 0;

    //
    // Name     : GetLocalID
    // Function : Get local pipe id.
    //
    virtual UINT32 SSAPI GetLocalID(void) = 0;

    //
    // Name     : ReloadIPList
    // Function : Reload ip list.
    //
    virtual bool SSAPI ReloadIPList(const char* pszIPListFile) = 0;

    //
	// Name     : ReloadPipeConfig
	// Function : Reload pipe config.
	//
    virtual bool SSAPI ReloadPipeConfig(const char* pszConfFile, const UINT32 dwGroup) = 0;


	//
	// Name     : ReloadIPList
	// Function : Reload ip list.
	//
	virtual bool SSAPI CheckIpValid(const char* ip) = 0;
};

//
// Function prototype for SSPipeGetModule
//
typedef ISSPipeModule* (SSAPI *PFN_SSPipeGetModule)(const SSSVersion* pstVersion);

//
// Name     : SSPipeGetModule
// Function : Get an SDPipe object.
//
ISSPipeModule* SSAPI SSPipeGetModule(const SSSVersion* pstVersion);

//
// Function prototype for SSPipeSetLogger
//
typedef bool (SSAPI *PFN_SSPipeSetLogger)(ISSLogger* poLogger, UINT32 dwLevel);

//
// Name     : SSPipeSetLogger
// Function : Set ISSLogger object for the SDPipe. The log of SDPipe will export
//            to the ISSLogger object.
//
bool SSAPI SSPipeSetLogger(ISSLogger* poLogger, UINT32 dwLevel);


/*
//
// Name     : idtoa
// Function : Convert numberic id to string id.
//
const char * SSAPI idtoa(UINT32 dwID, char *pszID);

//
// Name     : atoid
// Function : Convert string id to numberic id.
//
UINT32 SSAPI atoid(const char *pszID);
*/

} // namespace SSCP

#endif
