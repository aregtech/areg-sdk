/************************************************************************
 * \file        areg/src/base/private/linux/NEUtilitiesLinux.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Linux OS specific hidden utility methods.
 *
 ************************************************************************/

#include "areg/src/base/NEUtilities.hpp"

#ifdef  _LINUX

#include <sys/time.h>

namespace NEUtilities {

    void _generateName( const char * prefix, char * out_buffer, int length, const char * specChar )
    {
        static const char * formatStr = "%s{%03d%s%03d%s%03d%s%03d%s%03d%s%03d%s%03d%s%03d}";
        if ( out_buffer != NULL )
        {
            *out_buffer = '\0';
            const char * spec = specChar != NULL ? specChar : NEUtilities::DEFAULT_SPECIAL_CHAR;

            struct {
                uint32_t dwLowDateTime;
                uint32_t dwHighDateTime;
            } now;
            struct timeval tmp;
            if ( gettimeofday( &tmp, NULL ) == 0 ) {
                now.dwLowDateTime   = MACRO_64_LO_BYTE32( tmp.tv_usec );
                now.dwHighDateTime  = MACRO_64_HI_BYTE32( tmp.tv_sec );
            }
            else {
                OUTPUT_DBG( "calling gettimeofday faile with error code [ %p ].", errno );
                now.dwLowDateTime = rand( );
                now.dwHighDateTime = rand( );
            }

            int tick1 = static_cast<int>((now.dwHighDateTime >>  0) & 0xFF);
            int tick2 = static_cast<int>((now.dwHighDateTime >>  8) & 0xFF);
            int tick3 = static_cast<int>((now.dwHighDateTime >> 16) & 0xFF);
            int tick4 = static_cast<int>((now.dwHighDateTime >> 24) & 0xFF);
            int tick5 = static_cast<int>((now.dwLowDateTime  >>  0) & 0xFF);
            int tick6 = static_cast<int>((now.dwLowDateTime  >>  8) & 0xFF);
            int tick7 = static_cast<int>((now.dwLowDateTime  >> 16) & 0xFF);
            int tick8 = static_cast<int>((now.dwLowDateTime  >> 24) & 0xFF);

            NEStd::StringPrintfA( out_buffer, length, formatStr,
                prefix != NULL ? prefix : NEUtilities::DEFAULT_GENERATED_NAME,
                tick8, spec, tick7, spec, tick6, spec, tick5, spec, tick4, spec, tick3, spec, tick2, spec, tick1 );
        }
    }
}

#endif // _LINUX
