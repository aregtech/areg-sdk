/************************************************************************
 * \file        areg/src/base/private/posix/NEDebugWinPosix.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX implementation of Debugging utilities
 *
 ************************************************************************/
#include "areg/src/base/private/NEDebug.hpp"

#ifdef  _POSIX

#include "areg/src/base/NEString.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/EContainers.hpp"

#ifdef  _DEBUG

void AREG_API NEDebug::outputMessageOS( const char * msg )
{
    if ( NEString::isEmpty<char>( msg ) == false )
    {
        fprintf( stdout, msg );
        fflush( stdout );
    }
}

#else

void AREG_API NEDebug::outputMessageOS( const char * /*msg*/ )
{
}

#endif  // _DEBUG

#endif  // _POSIX
