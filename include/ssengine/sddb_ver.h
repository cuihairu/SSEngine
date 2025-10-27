/************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sddb_ver.h - Defines the version number of sddb module and the structured change record of 
					each update.
author : cwy
date : 2009-09-18
************************************************************************/

#ifndef __SDDB_VER_H__
#define __SDDB_VER_H__

/*
@Version : V3.0.5.10#
@Date : 2010-07-20#
@Update component : sddb.#
@Update reason : upate#
*/

/*
@Version : V3.0.5.9#
@Date : 2009-10-15#
@Update component : sddb.#
@Update reason : resolve character set display problem caused by using old version mysql, 
which do not have 'character_set_system' variable.#
*/

/* 
@Version : V3.0.5.8#
@Date : 2009-09-18#
@Update component : NONE.#
@Update reason : create version head file.#
*/

namespace SSCP
{
	static const SSSVersion SDDB_VERSION = {5, 0, 0, 0};
}

#endif