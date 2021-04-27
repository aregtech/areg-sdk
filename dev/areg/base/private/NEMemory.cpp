/************************************************************************
 * \file        areg/base/private/NEMemory.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Switches
 *
 ************************************************************************/

#include "areg/base/NEMemory.hpp"


/**
 * \brief   NEMemory::BLOCK_SIZE prefedefined block size value aligned to sizeof NEMemory::uAlign
 **/
AREG_API const unsigned int     NEMemory::BLOCK_SIZE       = sizeof(NEMemory::uAlign);
/**
 * \brief   NEMemory::INVALID_SIZE predefined invalid size value
 **/
AREG_API const unsigned int     NEMemory::INVALID_SIZE     = static_cast<unsigned int>(-1);

/**
 * \brief   Defines ignore stamp value for remote buffer, i.e. not set and should be ignored.
 **/
AREG_API const unsigned int      NEMemory::IGNORE_VALUE    = static_cast<unsigned int>(0);
/**
 * \brief   Defines invalid stamp value for remote buffer.
 **/
AREG_API const unsigned int      NEMemory::INVALID_VALUE   = static_cast<unsigned int>(-1);

static NEMemory::sRemoteMessage    __InvalidRemoteMessage  =
{
    {
        {   /*rbhBufHeader*/
              sizeof(NEMemory::sRemoteMessage) // biBufSize
            , NEMemory::IGNORE_VALUE           // biLength
            , NEMemory::IGNORE_VALUE           // biOffset
            , NEMemory::BufferUnknown          // biBufType
            , NEMemory::IGNORE_VALUE           // biUsed
            , NEMemory::IGNORE_VALUE           // biRefCount
        }
        , NEMemory::INVALID_VALUE              // rbhTarget
        , NEMemory::INVALID_VALUE              // rbhChecksum
        , NEMemory::INVALID_VALUE              // rbhSource
        , NEMemory::INVALID_VALUE              // rbhMessageId
        , static_cast<unsigned int>(NEMemory::ResultUnknownError)	// rbhResult
        , NEMemory::INVALID_VALUE              // rbhSequenceNr
    }
    , {static_cast<char>(0)}
};

/**
 * \brief   NEMemory::InvalidElement prefedefined invalid element
 **/
AREG_API const NEMemory::uAlign NEMemory::InvalidElement  = {0};

/**
 * \brief   NEMemory::InvalidBuffer prefedefined invalid buffer
 **/
AREG_API NEMemory::sByteBuffer &   NEMemory::InvalidBuffer        = reinterpret_cast<NEMemory::sByteBuffer &>(__InvalidRemoteMessage);

AREG_API NEMemory::sRemoteMessage & NEMemory::InvalidRemoteMessage= __InvalidRemoteMessage;
