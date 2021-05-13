//============================================================================
// Name        : src/TestCommon.hpp
// Author      : Artak Avetyan
// Version     : 1.0.0
// Copyright   : Aregtech © 2021
// Description : AREG SDK examples, common header to include in every project.
//============================================================================

#ifndef _AREG_SDK_TEST_COMMON_HPP_
#define _AREG_SDK_TEST_COMMON_HPP_

#ifndef POSIX
    #define POSIX
#endif  // POSIX

#ifndef IMPORT_STATIC_SIMBOLS
    #define IMPORT_STATIC_SIMBOLS
#endif  // IMPORT_STATIC_SIMBOLS

#ifndef BIT32
    #define BIT32
#endif  // BIT32

#ifndef OUTPUT_DEBUG_LEVEL
    #define OUTPUT_DEBUG_LEVEL      4
#endif  // OUTPUT_DEBUG_LEVEL

#include "areg/base/GEGlobal.h"

#endif // _AREG_SDK_TEST_COMMON_HPP_
