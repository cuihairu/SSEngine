/******************************************************************************
            Copyright (C) YoFei Corporation. All rights reserved.

 sdnet.h  - High perforcement net module classes, macros, types and definitions.
            Include file for sdcore.dll. This library is designed to provide
            large scale network connection management.

 Class    - ISSConnection
          - ISSConnector
          - ISSListener
          - ISSNet
          - ISSPacketParser
          - ISSSession
          - ISSSessionFactory


 Remarks  - SDNet module provides functionality of high performance TCP 
            connection management and high throughput data transmission. SDNet 
            module also defines a simple network programming framework. User
            only need to follow the framework to implement high performance 
            network functionality.

            SDNet is pure virtual C++ function based. It contains a set of pure
            virtual C++ class, include both interfaces implemented inside SDNet
            and callback interfaces which to be implemented by user.
            
            To improvement performance, SDNet adopts high performance thread
            module inside. To simplify usage, SDNet provides single thread 
            module for users. Almost all the functions and callback functions
            are in a single thread, which is called main thread. All the 
            functions provided by SDNet are non-blocked. 

******************************************************************************/

#ifndef SSCP_SDNET_H_20070614
#define SSCP_SDNET_H_20070614

#include "sdbase.h"
#include "sdnet_ver.h"
#include "sdconfig.h"
#include "sdlogger.h"

namespace SSCP{

//
// SDNet module name
//
const char SDNET_MODULENAME[] = "SSNet";

//
// SDNet version
//
const SSSVersion	SDNET_VERSION = SDNET_MODULE_VERSION;

//
// Error code of SDNet
//
enum ESDNetErrCode{
	NET_BIND_FAIL	= -7,
    NET_CONNECT_FAIL    = -6,
    NET_SYSTEM_ERROR    = -5, 
    NET_RECV_ERROR      = -4, 
    NET_SEND_ERROR      = -3, 
    NET_SEND_OVERFLOW   = -2,
    NET_PACKET_ERROR    = -1,
    NET_SUCCESS         = 0
};

//
// When create ISSConnector or ISSListener, user need to specify the net I/O
// module. 
//

//
// I/O Completion port module, which only fits for Windows.
//
const UINT32 NETIO_COMPLETIONPORT   = 1;
const UINT32 NETIO_COMPLETIONPORT_GATE   = 11;

//
// I/O AsyncSelect module, which only fits for Windows and client application.
//
const UINT32 NETIO_ASYNCSELECT      = 2;

//
// epoll I/O module, which only fits for linux
//
const UINT32 NETIO_EPOLL            = 10;

const UINT32 NETIO_EPOLL_GATE       = 101;

const UINT32 INVALID_IO_TYPE        = 0xFFFFFFFF;

const UINT32 UNKNOWN_SIZE	= 0xFFFFFFFF;

class ISSPacketParser;
class ISSSession;
class ISSSessionFactory;

// 
// Name     : ISSConnection
// Function : An ISSConnection object represent an abstraction of a TCP connection.
//
class ISSConnection
{
public:
    virtual ~ISSConnection() {}
    //
    // Name     : IsConnected
    // Function : Check if the connection is connected.
    //
    virtual bool SSAPI IsConnected(void) = 0;

    //
    // Name     : Send
    // Function : Send data on the connection.
    //
    virtual void SSAPI Send(const char* pBuf,UINT32 dwLen) = 0;

	//
	// Name     : DelaySend
	// Function : Send data on the connection in another thread.
	//
	virtual void SSAPI DelaySend(const char* pBuf,UINT32 dwLen) = 0;
    //
    // Name     : SetOpt
    // Function : Set extension options.
    //
    virtual void SSAPI SetOpt(UINT32 dwType, void* pOpt) = 0;

    //
    // Name     : Disconnect
    // Function : Close the TCP connection.
    //
    virtual void SSAPI Disconnect(void) = 0;

    //
    // Name     : GetRemoteIP
    // Function : Retrieves remote IP address for the connection.
    //
    virtual const UINT32 SSAPI GetRemoteIP(void) = 0;

    //
    // Name     : GetRemoteIPStr
    // Function : Retrieves remote IP address in text format for the connection.
    //
    virtual const char* SSAPI GetRemoteIPStr(void) = 0;

    //
    // Name     : GetRemotePort
    // Function : Retrieves remote port number for the connection.
    // 
    virtual UINT16 SSAPI GetRemotePort(void) = 0;

    //
    // Name     : GetLocalIP
    // Function : Retrieves local IP address for the connection.
    // 
    virtual const UINT32 SSAPI GetLocalIP(void) = 0;

    //
    // Name     : GetLocalIP
    // Function : Retrieves local IP address in text format for the connection.
    // 
    virtual const char* SSAPI GetLocalIPStr(void) = 0;

    //
    // Name     : GetLocalIP
    // Function : Retrieves local port number for the connection.
    // 
    virtual UINT16 SSAPI GetLocalPort(void) = 0;


	//
	// Name		: GetSendBufFree
	// Function	: Retrieves local send buffer's free size
	// 
	virtual UINT32 SSAPI GetSendBufFree(void) = 0;
};


//
// Name     : ISSListener
// Function : The interface provides functionality for TCP listening.
//
class ISSListener
{
public:
    virtual ~ISSListener() {}
    //
    // Name     : SetPacketParser
    // Function : Set an user implemented ISSPacketParser object, which process
    //            packet parse logic of the connection accepted by the ISSListener.
    //
    virtual void SSAPI SetPacketParser(ISSPacketParser* poPacketParser) = 0;

    //
    // Name     : SetSessionFactory
    // Function : Set the user implemented ISSSessionFactory object to ISSListener.
    //            When a TCP connection is accepted by ISSListener, 
    //            ISSSessionFactory object will be asked to create a ISSSession.
    //
    virtual void SSAPI SetSessionFactory(ISSSessionFactory* poSessionFactory) = 0;

    //
    // Name     : SetBufferSize
    // Function : Set the send and receive buffer size of the connection accepted
    //            by the ISSListener object.
    //
    virtual void SSAPI SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize) = 0;

    //
    // Name     : SetOpt
    // Function : Set extension options.
    //
    virtual void SSAPI SetOpt(UINT32 dwType, void* pOpt) = 0;

    //
    // Name     : Start
    // Function : Listen at specified IP and port.
    // 
    virtual bool SSAPI Start(const char* pszIP, UINT16 wPort, bool bReUseAddr = true) = 0;

    //
    // Name     : Stop
    // Function : Stop listen.
    //
    virtual bool SSAPI Stop(void) = 0;

    //
    // Name     : Release
    // Function : Release the ISSListener object.
    //
    virtual void SSAPI Release(void) = 0;
};

//
// Name     : ISSConnector
// Function : The interface provides functionality for initiative TCP connection.
//
class ISSConnector
{
public:
    virtual ~ISSConnector() {}
    //
    // Name     : SetPacketParser
    // Function : Set the user implemented ISSPacketParser object, which process
    //            packet parse logic of the connection connected by the ISSConnector
    //            object.
    //
    virtual void SSAPI SetPacketParser(ISSPacketParser* poPakcetParser) = 0;

    //
    // Name     : SetSession
    // Function : Set the user implemented ISSSession object, which will be bind
    //            to ISSConnection object when connection establishes.
    //
    virtual void SSAPI SetSession(ISSSession* poSession) = 0;

    virtual ISSSession* SSAPI GetSession() = 0;


    //
    // Name     : SetBufferSize
    // Function : Set the send and receive buffer size for the connection connected
    //            by the ISSConnector object.
    //
    virtual void SSAPI SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize) = 0;

    //
    // Name     : Connect
    // Function : Connect to the specified address.
    //
    virtual int SSAPI Connect(const char* pszIP, UINT16 wPort) = 0;

    //
    // Name     : ReConnect
    // Function : Reconnect to the address which is specified by the last Connect
    //            call.
    //
    virtual int SSAPI ReConnect(void) = 0;

    //
    // Name     : Release
    // Function : Release the ISSConnector object.
    //
    virtual void SSAPI Release(void) = 0;


	//
	// Name     : SetOpt
	// Function : Set extension options.
	//
	virtual void SSAPI SetOpt(UINT32 dwType, void* pOpt) = 0;


};

//
// Name     : ISSNet
// Function : Interface of SDNet module.
//
class ISSNet : public ISSBase
{
public:
	 virtual ~ISSNet() {};
    //
    // Name     : CreateConnector
    // Function : Create an ISSConnector object.
    //
    virtual ISSConnector* SSAPI CreateConnector(UINT32 dwNetIOType) = 0;

    //
    // Name     : CreateListener
    // Function : Create an ISSListener object.
    // 
    virtual ISSListener* SSAPI CreateListener(UINT32 dwNetIOType)  = 0;

    //
    // Name     : Run
    // Function : Process network event.
    //
    virtual bool SSAPI Run(INT32 nCount = -1) = 0;
};

//
// Name     : ISSPacketParser
// Function : Break packet from TCP data stream. The interface need to be
//            implemented by user.
//
class ISSPacketParser
{
public:
    virtual ~ISSPacketParser() {}
    //
    // Name     : ParsePacket
    // Function : Return the length in bytes of the packet in TCP data stream.
    //
    virtual INT32 SSAPI ParsePacket(const char* pBuf, UINT32 dwLen) = 0;

	virtual INT32 SSAPI ParseFirstPacket(const char* pBuf, UINT32 dwLen) { return ParsePacket(pBuf, dwLen); }
};

//
// Name     : ISSSession
// Function : When network event happens, the related function of ISSSession 
//            will be callback. User need to implement ISSSession.
//
class ISSSession
{
public:
    virtual ~ISSSession() {}
    //
    // Name     : SetConnection
    // Function : Set the ISSConnection object to the ISSSession object.
    //
    virtual void SSAPI SetConnection(ISSConnection* poConnection) = 0;

    //
    // Name     : OnEstablish
    // Function : Callback when the connection is established.
    //
    virtual void SSAPI OnEstablish(void) = 0;

    //
    // Name     : OnTerminate
    // Function : Callback when the connection is closed.
    //
    virtual void SSAPI OnTerminate(void) = 0;

    //
    // Name     : OnError
    // Function : Callback when TCP connection has error.
    //
    virtual bool SSAPI OnError(INT32 nModuleErr, INT32 nSysErr) = 0;

    //
    // Name     : OnRecv
    // Function : Callback when receive a packet, which is parsed by ISSPacketParser.
    //            
    //
    virtual void SSAPI OnRecv(const char* pBuf, UINT32 dwLen) = 0;

    //
    // Name     : Release
    // Function : Release ISSSession object. It will be callback when SDNet no
    //            more use it.
    //
    virtual void SSAPI Release(void) = 0;
	//
    // Author   : wqh
    // Function : set active close flag; which do not need reconnect
    //
    virtual void SSAPI ActiveClose(void) {}
    virtual bool SSAPI IsActiveClose() { return false; }
    virtual void SSAPI SetActiveClose(bool bFlag) {}
};

//
// Name     : ISSSessionFactory
// Function : Create user implemented ISSSession object. This interface need to
//            be implemented by user too.
//
class ISSSessionFactory
{
public:
    virtual ~ISSSessionFactory() {}
    //
    // Name     : CreateSession
    // Function : Create an user implemented ISSSession object.
    //
    virtual ISSSession* SSAPI CreateSession(ISSConnection* poConnection) = 0;
};

//
// Name     : SSNetGetModule
// Function : Get an SDNet object.
//
ISSNet* SSAPI SSNetGetModule(const SSSVersion* pstVersion);

//
// Function pointer definition of SSNetGetModule
//
typedef ISSNet* (SSAPI *PFN_SSNetGetModule)(const SSSVersion* pstVersion);

//
// Name     : SSNetSetLogger
// Function : Set ISSLogger object to the SDNet. The log of SDNet will export
//            to the ISSLogger object.
//
bool SSAPI SSNetSetLogger(ISSLogger* poLogger, UINT32 dwLevel);

//
// Function pointer definition of SSNetSetLogger
//
typedef bool (SSAPI *PFN_SSNetSetLogger)(ISSLogger* poLogger, UINT32 dwLevel);

//
// Name     : SSNetSetOpt
// Function : Set extension options.
//
void SSAPI SSNetSetOpt(UINT32 dwType, void* pOpt);

}

#endif
