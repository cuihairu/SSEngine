/************************************************************************
						Copyright (C) YoFei Corporation. All rights reserved.

sdnet_ver.h - Defines the version number of sschannel module and the structured change record of 
					each update.

************************************************************************/

#ifndef __SDNET_VERSION_H__
#define __SDNET_VERSION_H__

/*
@Update component : sspipe.#
@Update reason : resolved a bug that  calls to application crash during termination.#
*/

namespace SSCP
{
	static const SSSVersion SDNET_MODULE_VERSION = {3, 0, 5, 11};
}

#endif



