#ifndef AREG_BASE_NECOMMON_HPP
#define AREG_BASE_NECOMMON_HPP
/************************************************************************
 * \file        areg/base/NECommon.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Common constants
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

namespace NECommon
{

    typedef unsigned int    CharPos;    //!< The type of character position in the string.
    typedef unsigned int    CharCount;  //!< The type of characters count in the string.
    typedef signed int      IndexPos;   //!< The type of index position in buffer, list or array.

    const CharCount DEFAULT_BLOCK_SIZE  = 32;                               //!< The default block size to allocate string buffer.
    const CharPos   InvalidPos          = static_cast<CharPos>(-1);         //!< The value of invalid position in the string.
    const CharPos   StartPos            = static_cast<CharPos>( 0);         //!< The value of first position in the string.
    const CharPos   EndPos              = static_cast<CharPos>(-2);         //!< The value of last position in the string.
    const CharCount CountAll            = static_cast<CharCount>(EndPos);   //!< The value of all characters in the string
    const IndexPos  InvalidIndex        = static_cast<IndexPos>(-1);        //!< The value of invalid position in buffer, list or array.
}

#endif  // AREG_BASE_NECOMMON_HPP
