/******************************************************************************
Copyright (C) 2025 Cui Hairu. All rights reserved.

sdmacros.h - sdu相关的编译宏开关	

******************************************************************************/


#ifndef SDUMACROS_H
#define SDUMACROS_H

//SDU组件内部使用了部分boost库的内容,如正规表达式处理,
//如需要使用正规表达式函数,请打开此宏
#define SDU_WITH_BOOST   0

#define SDU_WITH_OPENSSL 1

#define SDU_WITH_LIBICONV 1

#endif //