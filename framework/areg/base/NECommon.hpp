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

    typedef signed int      ElemPos;    //!< The type of character position in the string.
    typedef signed int      ElemCount;  //!< The type of characters count in the string.
    typedef signed int      IndexPos;   //!< The type of index position in buffer, list or array.

    const ElemCount DEFAULT_BLOCK_SIZE  = 32;                               //!< The default block size to allocate string buffer.
    const ElemPos   InvalidPos          = static_cast<ElemPos>(-1);         //!< The value of invalid position in the string.
    const ElemPos   StartPos            = static_cast<ElemPos>( 0);         //!< The value of first position in the string.
    const ElemPos   EndPos              = static_cast<ElemPos>(-2);         //!< The value of last position in the string.
    const ElemCount CountAll            = static_cast<ElemCount>(EndPos);   //!< The value of all characters in the string
    const IndexPos  InvalidIndex        = static_cast<IndexPos>(-1);        //!< The value of invalid position in buffer, list or array.
    const ElemCount DefaultSize         = static_cast<ElemCount>(-2);       //!< The value of default size.

    /**
     * \brief   NECommon::MAXIMUM_WAITING_OBJECTS
     *          The maximum number of synchronization objects that is able to lock. 
     **/
    const int MAXIMUM_WAITING_OBJECTS   = 64;

}

#endif  // AREG_BASE_NECOMMON_HPP
