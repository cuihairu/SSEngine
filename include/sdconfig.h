#ifndef SDCONFIG_H
#define SDCONFIG_H

// 
//#define  SSCP_HAS_MSMQ_LOGGER      

//#define  SDNET_HAS_SEND_REPORT           

//#define  SDNET_HAS_GATHER_SEND

#define   SDENT_HAS_RECV_QUEUE  // default is defined 

#if defined(WIN32) || defined(WIN64)
#define WINDOWS
#else
#define LINUX
#endif


/**
* @file sdu.h
* @brief SD Platform Definitions
* @remark This is a generated header generated from include/sdu.h.in by
* ./configure, or copied from include/sdu.hw or include/sdu.hnw 
* for Win32 or Netware by those build environments, respectively.
*/

#if defined(WINDOWS) 

/* Ignore most warnings (back down to /W3) for poorly constructed headers
*/
#if defined(_MSC_VER) && _MSC_VER >= 1200
#pragma warning(push, 3)
#endif


#endif /* !_WINDOWS_ */

#define SD_DECLARE_STATIC
#define SD_HAVE_IPV6 1

/* Done with badly written headers
*/

#endif /* WIN32 */

