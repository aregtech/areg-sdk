#ifndef AREG_SYSTEM_GCC_NEW_H
#define AREG_SYSTEM_GCC_NEW_H
/************************************************************************
 * \file        system/gcc/new.h
 * \ingroup     Framework, GCC proxies
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Framework, platform dependent includes
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

// instead of new.h, use new
#include <new>

#ifdef _DEBUG
// there is no new operator that takes blocktype, filename and line.
inline void* operator new(size_t size, int/*blocktype*/, const char*/*file*/, int/*line*/) 
{ return operator new(size); }

inline void* operator new[](size_t size, int/*blocktype*/, const char*/*file*/, int/*line*/) 
{ return ::operator new[](size); }

#ifndef _NORMAL_BLOCK
    #define _NORMAL_BLOCK 1
#endif

#endif // _DEBUG

#endif // AREG_SYSTEM_GCC_NEW_H
