#ifndef AREG_EXTENSIONS_EXTENSIONS_HPP
#define AREG_EXTENSIONS_EXTENSIONS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        extensions/Extensions.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, global include header file
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

/**
 * \brief   This is a global file for all extensions.
 **/
#include "areg/base/GEGlobal.h"

#if !defined(AREG_EXTENSIONS) || (AREG_EXTENSIONS == 0)
    #define _AREG_EXT   0
#else   // !defined(AREG_EXTENSIONS) || (AREG_EXTENSIONS == 0)
    #define _AREG_EXT   1
#endif  // !defined(AREG_EXTENSIONS) || (AREG_EXTENSIONS == 0)

#endif  // AREG_EXTENSIONS_EXTENSIONS_HPP
