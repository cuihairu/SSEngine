/******************************************************************************
            Copyright (C) YoFei Corporation. All rights reserved.

 This file defines the externtion option flags for sdnet module. In most cases, 
 you have no need to consider them for the module has already set them with 
 default value to saticified the normal demands. Make sure you know the usage of
 them throughly before setting them.

******************************************************************************/

#ifndef SSCP_SDNETOPT_H_20070614
#define SSCP_SDNETOPT_H_20070614

#include "sdbase.h"

namespace SSCP{

//
// The following flags start with CONNECTION_OPT_ are belong to ISSConnection.
//

//
// Set the socket's option flag, which implemented by calling setsockopt of system
//
const UINT32 CONNECTION_OPT_SOCKOPT = 1;

//
// CONNECTION_OPT_SOCKOPT is a data structure, its members correspond the parameters of setsockopt
//
struct SConnectionOptSockopt
{
    INT32       nLevel;
    INT32       nOptName;
    const char* pOptVal;
    INT32       nOptLen;
};

//
// The following flags start with LISTENER_OPT_ are the option flag of CSDGateListener.
//

//
// This option is used to set the SessionFactory of CSDGateListener, which is derived from
// ISSSessionFactory used to communicate with session manager class of GateServer.
// please refer to sdnet.h
//
const UINT32 LISTENER_OPT_SESSION_FACTORY = 101;
const UINT32 LISTENER_OPT_WAITMSECFORBUFFER = 102;

//
// Set the option of the  SessionFactory class yourself defined, please refer sdnet.h
//
struct SListenerOptSessionFactory
{
    void* pSessionFactory;
};
//
// the extern option for sdnet module
// You should set it before creating the sdnet module, or else it will make no effect.
//

//
// This option is used to set the max connection limit number for sdnet.
// Note: this flag is only apply for linux version of sdnet
//
const UINT32 NETLIN_OPT_MAX_CONNECTION = 201;

struct SNetLinOptMaxConnection
{
    INT32 nMaxConnection;
};

//
// Set the size of some queues of the sdnet module. You could adjust these parameters
// to satisfied your reserve. In most cases, the default value could satisfied. 
// Note: this flag is only apply for linux version of sdnet
//
const UINT32 NETLIN_OPT_QUEUE_SIZE = 202;

struct SNetLinOptQueueSize
{
    INT32 nRecvBufSize;         // total recv buffer size, -1 means use default value
    INT32 nSendBufSize;         // total send buffer size,-1 means use default value
    INT32 nRequestQueueSize;    // request queue size,-1 means use default value
    INT32 nEventQueueSize;      // event queue size,-1 means use default value
};

//
// This flag is used to set some advance flag for sdnet developer, the user should
// not touch it.
// Note: this flag is only apply for linux version of sdnet
//
const UINT32 NETLIN_OPT_ADVANCE_PARAM = 203;

struct SNetLinOptAdvanceParam
{
    INT32 nParam1;  // -1 means use default value
    INT32 nParam2;  // -1 means use default value
};

//
// This option is used to set the max connection limit number for sdnet.
// Note: this flag is only apply for windows version of sdnet
//
const UINT32 NETWIN_OPT_MAX_CONNECTION = 301;

struct SNetWinOptMaxConnection
{
    INT32 nMaxConnection;
};

//
// Set the size of some queues of the sdnet module. You could adjust these parameters
// to satisfied your reserve. In most cases, the default value could satisfied. 
// Note: this flag is only apply for windows version of sdnet
//
const UINT32 NETWIN_OPT_QUEUE_SIZE = 302;

struct SNetWinOptQueueSize
{
    INT32 nRecvBufSize;         // total recv buffer size, -1 means use default value
    INT32 nEventQueueSize;      // event queue size, -1 means use default value
};

//
// This flag is used to set some advance flag for sdnet developer, the user should
// not touch it.
// Note: this flag is only apply for windows version of sdnet
//
const UINT32 NETWIN_OPT_ADVANCE_PARAM = 303;

struct SNetWinOptAdvanceParam
{
    INT32 nParam1;  // -1 means use default value
};

//////////////////
//set iocp/epoll work thread num
const UINT32 NETWIN_OPT_WORKTHREAD_PARAM = 304;

struct SNetWinOptWorkThreadParam
{
	INT32 nParam1;  // -1 means use default value
};
//
// To bind the socket's IP and Port
//
//const UINT32 CONNECTOR_OPT_SOCKBIND = 401;

//
//	CONNECTION_OPT_SOCKBIND associates with SConnectorOptSockBind data structure, 
// which points out the IP and port to be bound
//
struct SConnectorOptSockBind
{	
	const char*	pszIP;	// 0-end IP string , if NULL won't bind
	UINT16			wPort;	// host byte order, if 0 won't bind
};

}

#endif
