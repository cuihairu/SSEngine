#ifndef SDCONFIG_H
#define SDCONFIG_H

//
//#define  SSCP_HAS_MSMQ_LOGGER      

//#define  SDNET_HAS_SEND_REPORT           

//#define  SDNET_HAS_GATHER_SEND

#define   SDENT_HAS_RECV_QUEUE  // default is defined 

namespace SSCP {
const char* GetPlatformName();
}

#endif // SDCONFIG_H
