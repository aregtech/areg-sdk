#ifndef AREG_SYSTEM_GCC_ASSERT_H
#define AREG_SYSTEM_GCC_ASSERT_H

/************************************************************************
 * \file        system/gcc/assert.h
 * \ingroup     Framework, GCC proxies
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Framework, platform dependent includes for GCC
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include_next <assert.h>

#ifndef ASSERT
    // #define ASSERT assert
#endif

#endif // AREG_SYSTEM_GCC_ASSERT_H
