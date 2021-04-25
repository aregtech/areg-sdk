/************************************************************************
 * \file        areg/src/base/private/ETemplateBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, templates constants class.
 *
 ************************************************************************/
#include "areg/src/base/ETemplateBase.hpp"

/**
 * \brief   The number of blocks by default created when increase list in map. For hash map internal use
 **/
const int    CETemplateConstants::MAP_DEFAULT_BLOCK_SIZE    = static_cast<int>(64);         /*0x00000040*/
/**
 * \brief   The number of maximum blocks created at once when increase list in map. For hash map internal use
 **/
const int    CETemplateConstants::MAP_MAX_BLOCK_SIZE        = static_cast<int>(2048);       /*0x00000800*/
/**
 * \brief   The size of hash table by default created in map. For hash map internal use
 **/
const int    CETemplateConstants::MAP_DEFAULT_HASH_SIZE     = static_cast<int>(64);         /*0x00000040*/
/**
 * \brief   The maximum size of hash table created in map. For hash map internal use
 **/
const int    CETemplateConstants::MAP_MAX_TABLE_SIZE        = static_cast<int>(2048);       /*0x00000800*/
/**
 * \brief   Bit-shift used to generate has key number
 **/
const int    CETemplateConstants::MAP_SHIFT                 = static_cast<int>(5);          /*0x00000005*/
/**
 * \brief   Defines invalid Hash value
 **/
const unsigned int  CETemplateConstants::MAP_INVALID_HASH   = static_cast<unsigned int>(~0);/*0xFFFFFFFF*/
/**
 * \brief   Defines starting position in hash-map
 **/
void * const CETemplateConstants::START_POSITION            = reinterpret_cast<void *>(~0); /*0xFFFFFFFF*/

/**
 * \brief   The minimum new created entries when grow array
 **/
const int    CETemplateConstants::ARRAY_DEFAULT_MIN_GROW    = static_cast<int>(4);          /*0x00000004*/
/**
 * \brief   The maximum new created entries when grow array
 **/
const int    CETemplateConstants::ARRAY_DEFAULT_MAX_GROW    = static_cast<int>(2048);       /*0x00000800*/
/**
 * \brief   Defines default increase. When used, default values should be used.
 **/
const int    CETemplateConstants::ARRAY_DEFAULT_INCREASE    = static_cast<int>(~0);         /*0xFFFFFFFF*/

/**
 * \brief   Defines invalid position.
 **/
void* const CETemplateConstants::INVALID_POSITION           = static_cast<void *>(NULL);    /*0x00000000*/

/**
 * \brief   Defines starting index in the lists.
 **/
const int   CETemplateConstants::START_INDEX                = static_cast<int>(MIN_INT_32); /*0x80000000*/

/**
 * \brief   Defines invalid index in the lists.
 **/
const int   CETemplateConstants::INVALID_INDEX              = static_cast<int>(-1);         /*-1*/
