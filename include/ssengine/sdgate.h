#ifndef SDGATE_H
#define SDGATE_H

#include "sdbase.h"
#include "sdnet.h"
#include "sdnet_ver.h"

namespace SSCP
{

//
// SDGate module name
//
const char SDGATE_MODULENAME[] = "SSGate";

//
// SDGate version
//
const SSSVersion	SDGATE_VERSION = SDNET_MODULE_VERSION;

//
// Error code of SDGate
//
enum ESDGATEERR_CODE
{
	SDGATEERR_SUCCESS					 = 0,
	SDGATEERR_VESION_UNCOMPATIBLE        = 1001 , //protocol version uncompatible
	SDGATEERR_PLAYER_EXIST               = 1002 , //player exist
	SDGATEERR_PLAYER_NOEXIST             = 1003 , //player noexist
	SDGATEERR_SVR_NOT_CONNECTED			 = 1004 , //server not connected
	SDGATEERR_PLAYER_NOT_ENTER			 = 1005 , //plyer not enter
	SDGATEERR_PACKET_ERROR				 = 1006 , //packet error
	SDGATEERR_CONN_SVR_ERROR			 = 1007 , //connect server error
	SDGATEERR_CLI_KICK					 = 1008 , //client kicked by server
	SDGATEERR_SYS_ERROR                  = 1020   //system error
};

class ISSServerConnection;
class ISSServerSession;
class ISSClientConnection;
class ISSClientSession;
class ISSClientSessionFactory;
class ISSGate;


// 
// Name     : ISSServerConnection
// Function : An ISSServerConnection object represent an abstraction of a TCP connection with Game Server.
//
class ISSServerConnection
{
public:    

	//
	//	Name	: ~ISSServerConnection
	//
	virtual SSAPI ~ISSServerConnection(){}

    //
    // Name     : SetSession
    // Function : Get the user implemented ISSSession object, which bind
    //            to ISSConnection object.
    //
	virtual ISSServerSession* SSAPI GetSession(void) = 0;
	
	//
    // Name     : SetBufferSize
    // Function : Set the send and receive buffer size of the connection with the Game Server.
    //
	virtual void SSAPI SetBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize) = 0;

    //
    // Name     : Connect
    // Function : connect server,.bAutoReconnect means reconnect server or not
    //
	virtual int SSAPI Connect(const char* pszIP, UINT16 wPort, bool bAutoReconnect) = 0;

    //
    // Name     : Close
    // Function : Close server connect
    //
	virtual void SSAPI Close(void) = 0;
	
    //
    // Name     : SendServerData
    // Function : send data to server
    //
	virtual bool SSAPI SendServerData(const char* pData, UINT32 nLen) = 0;
	
    //
    // Name     : SendServerData
    // Function : send data to client
    //
	virtual bool SSAPI SendClientData(ISSClientConnection* poClient, const char* pData, UINT32 nLen) = 0;

    //
    // Name     : Enter
    // Function : Before client send data to one server, client should enter the server.
    //
	virtual bool SSAPI Enter(ISSClientConnection* poClient, UINT64 dwTransID) = 0;

    //
    // Name     : Enter
    // Function : Before client close one server's connect, client should leave the server.
    //
	virtual bool SSAPI Leave(ISSClientConnection* poClient, UINT64 dwTransID) = 0;
	

    //
    // Name     : GetRemoteIP
    // Function : Retrieves remote IP address for the connection.
    //
	virtual UINT32 SSAPI GetRemoteIP(void) = 0;
	
    //
    // Name     : GetRemotePort
    // Function : Retrieves remote port number for the connection.
    // 
	virtual UINT16 SSAPI GetRemotePort(void) = 0;
	
	//
    // Name     : GetLocalIP
    // Function : Retrieves local IP address for the connection.
    // 
	virtual UINT32 SSAPI GetLocalIP(void) = 0;

    //
    // Name     : GetLocalPort
    // Function : Retrieves local port number for the connection.
    // 
	virtual UINT16 SSAPI GetLocalPort(void) = 0;
	
    //
    // Name     : SetOpt
    // Function : set connect option
    // 
	virtual bool SSAPI SetOpt(INT32 nType, void* pOpt) = 0;
	
	
    //
    // Name     : Release
    // Function : Release the ISSServerConnection object.
    //
	virtual void SSAPI Release(void) = 0;
};

class ISSServerSession
{
public:

	// Name	: ~ISSServerSession
    //
	virtual SSAPI ~ISSServerSession(){}

    // Name     : SetConnection
    // Function : Set the ISSServerConnection object to the ISSServerSession object.
    //
	virtual void SSAPI SetConnection(ISSServerConnection* poConnection) = 0;
	
	//
    // Name     : OnError
    // Function : Callback when TCP connection has error or Gate has error.
    //
	virtual bool SSAPI OnError(INT32 nSDError, INT32 nSysError) = 0;
	
	//
    // Name     : OnConnect
    // Function : Callback when the connection is established.
    //
	virtual void SSAPI OnConnect(bool bSuccess) = 0;
	//
    // Name     : OnClose
    // Function : Callback when the connection is closed.
    //
	virtual void SSAPI OnClose(void) = 0;
    
    //
    // Name     : OnRecvServerData
    // Function : Callback when receive a server packet.
    //
	virtual void SSAPI OnRecvServerData(const char* pData, UINT32 nLen) = 0;
	
    //
    // Name     : OnRecvServerData
    // Function : Callback when receive a packet that need to transmit to client.
    //
	virtual void SSAPI OnRecvClientData(ISSClientConnection* poClient, const char* pData, UINT32 nLen) = 0;

    //
    // Name     : OnEnter
    // Function : Callback when enter the server that need to transmit to client.
    //
	virtual void SSAPI OnEnter(ISSClientConnection* poClient, INT32 nSDError, UINT64 dwTransID) = 0;

    //
    // Name     : OnLeave
    // Function : Callback when leave the server that need to transmit to client.
    //
	virtual void SSAPI OnLeave(ISSClientConnection* poClient, INT32 nSDError, UINT64 dwTransID) = 0;
};

//client connection
class ISSClientConnection
{
public:

	//
	// Name	: ~ISSClientConnection
	// 
	virtual SSAPI ~ISSClientConnection(){}

    //
    // Name     : GetSession
    // Function : Retrieves session for the connection.
    //
	virtual ISSClientSession* SSAPI GetSession(void) = 0;

    //
    // Name     : Send
    // Function : send data to client
    //
	virtual bool SSAPI Send(const char* pData, UINT32 nLen) = 0;

    //
    // Name     : Close
    // Function : Close connection from client
    //
	virtual void SSAPI Close(void) = 0;

    //
    // Name     : GetRemoteIP
    // Function : Retrieves remote IP address for the connection.
    //
	virtual UINT32 SSAPI GetRemoteIP(void) = 0;

    //
    // Name     : GetRemoteIPStr
    // Function : Retrieves remote IP address string for the connection.
    //
	virtual const char* SSAPI GetRemoteIPStr(void) = 0;

    //
    // Name     : GetRemotePort
    // Function : Retrieves remote IP port for the connection.
    //
	virtual UINT16 SSAPI GetRemotePort(void) = 0;

    //
    // Name     : GetLocalIP
    // Function : Retrieves local IP port for the connection.
    //
	virtual UINT32 SSAPI GetLocalIP(void) = 0;

    //
    // Name     : GetLocalIPStr
    // Function : Retrieves local IP address string for the connection.
    //
	virtual const char* SSAPI GetLocalIPStr(void) = 0;

    //
    // Name     : GetLocalPort
    // Function : Retrieves local IP port for the connection.
    //
	virtual UINT16 SSAPI GetLocalPort(void) = 0;

    //
    // Name     : SetOpt
    // Function : set connect option
    //
	virtual bool SSAPI SetOpt(INT32 nType, void* pOpt) = 0;
};

class ISSClientSession
{
public:

	//
	// Name	: ~ISSClientSession
	//
	virtual SSAPI ~ISSClientSession(){}

	// 
	// Name	: SetConnection
	//
	virtual void SSAPI SetConnection(ISSClientConnection* poConnection) = 0;

    //
    // Name     : OnError
    // Function : Callback when TCP connection has error or Gate has error.
    //
	virtual bool SSAPI OnError(INT32 nSDError, INT32 nSysError) = 0;

    //
    // Name     : OnConnect
    // Function : Callback when the connection is established.
    //
	virtual void SSAPI OnConnect(void) = 0;

    //
    // Name     : OnClose
    // Function : Callback when the connection is closed.
    //
	virtual void SSAPI OnClose(void) = 0;

    //
    // Name     : OnRecv
    // Function : Callback when receive a packet.
    //
	virtual void SSAPI OnRecv(const char* pData, UINT32 nLen) = 0;

    //
    // Name     : Release
    // Function : release the connection.
    //
	virtual void SSAPI Release(void) = 0;
};

//ISSClientSession factory
class ISSClientSessionFactory
{
public:

	virtual SSAPI ~ISSClientSessionFactory(){}

    //
    // Name     : CreateSession
    // Function : Create new client session.
    //
	virtual ISSClientSession* SSAPI CreateSession(void) = 0;
};

class ISSGate : public ISSBase
{
public:

    //
    // Name     : CreateServerConnection
    // Function : Create new server connection.
    //
	virtual ISSServerConnection * SSAPI CreateServerConnection(ISSServerSession* poSession, ISSPacketParser* poSvrPacketParser=NULL) = 0;

    //
    // Name     : SetClientPacketParser
    // Function : set client packet parser.
    //
	virtual void SSAPI SetClientPacketParser(ISSPacketParser* poPacketParser) = 0;

    //
    // Name     : SetClientSessionFactory
    // Function : set client session factory.
    //
	virtual void SSAPI SetClientSessionFactory(ISSClientSessionFactory* poFactory) = 0;

    //
    // Name     : SetClientBufferSize
    // Function : set client buffer size.
    //
	virtual bool SSAPI SetClientBufferSize(UINT32 dwRecvBufSize, UINT32 dwSendBufSize) = 0;

 
	virtual INT32 SSAPI AddListen(const char* pszIP, UINT16 wPort) = 0;

    //
    // Name     : DelListen
    // Function : release listener.
    //
	virtual bool SSAPI DelListen(INT32 nID) = 0;

	virtual bool SSAPI Run(INT32 nCount) = 0;
	
    //
    // Name     : SetOpt
    // Function : set sdgate option.
    //
	virtual bool SSAPI SetOpt(INT32 nType, void* pOpt) = 0;
};

typedef ISSGate* (SSAPI *PFN_SSGateCreateModule)(const SSSVersion* pstVersion, ISSNet* poNet);
ISSGate* SSAPI SSCreateGate(const SSSVersion* pstVersion, ISSNet* poNet);
bool SSAPI SSGateSetLogger(ISSLogger* poLogger, UINT32 dwLevel);

}

#endif
