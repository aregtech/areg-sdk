#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/NECommon.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Common constants
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include <string_view>
#include <limits>
#include <utility>

namespace NECommon
{

    using ElemPos   = signed int;   //!< The type of character position in the string.

    using ElemCount = signed int;   //!< The type of characters count in the string.
    
    /**
     * \brief   Overlapping of elements in ring buffer or stack means the cases
     *          when buffer or stack is full, i.e. start and end positions match
     *          and number of elements in buffer or stack is equal to stack
     *          capacity. On next request to push element, the stack may change
     *          behavior, depending on overlapping flag, which are:
     *          1.  StopOnOverlap   --  Do not insert any new element if ring is full.
     *          2.  ShiftOnOverlap  --  Do not change ring capacity if it is full,
     *                                  but shift start position, i.e. overwrite 
     *                                  the first element of ring and insert new element.
     *          3. ResizeOnOvelap   --  Always resize ring if it is full. This changes
     *                                  the actual capacity of ring, making it bigger.
     **/
    typedef enum class E_RingOverlap    : uint8_t
    {
        /**
         * \brief   Do not insert new element, if ring is full.
         *          New element is lost.
         **/
          StopOnOverlap     = 0
        /**
         * \brief   Shifts start position, if ring is full, insert new element,
         *          and do not resize ring. Last element is lost.
         **/
        , ShiftOnOverlap    = 1
        /**
         * \brief   Resize ring, if it is full. No element is lost.
         **/
        , ResizeOnOvelap    = 2

    } eRingOverlap;

    /**
     * \brief   Sorting criteria for containers
     **/
    typedef enum class E_Sort   : uint8_t
    {
          NoSorting         //!< No sorting.
        , SortDescending    //!< Sort descending
        , SortAscending     //!< Sort ascending
    } eSort;

    /**
     * \brief   Indicates the index of an element in the buffer.
     **/
    typedef enum class E_Index  : ElemPos
    {
          InvalidIndex  = -1    //!< Indicates the invalid index in any buffer
        , FirtIndex     =  0    //!< Indicates the first index in a buffer
        , LastIndex     = -2    //!< Indicates the end of the buffer, i.e. the last position.
    } eIndex;

    /**
     * \brief   The value of invalid position in buffer, list or array.
     **/
    constexpr ElemPos      INVALID_INDEX            { static_cast<ElemPos>(NECommon::eIndex::InvalidIndex) };

    /**
     * \brief   The first valid index in the buffer.
     **/
    constexpr ElemPos       FIRST_INDEX             { static_cast<ElemPos>(NECommon::eIndex::FirtIndex) };
    
    /**
     * \brief   The last valid position in the string.
     **/
    constexpr ElemPos       LAST_INDEX              { static_cast<ElemPos>(NECommon::eIndex::LastIndex) };
    
    /**
     * \brief   The value of all characters in the string
     **/
    constexpr ElemCount     COUNT_ALL               { static_cast<ElemCount>(LAST_INDEX) };
    
    /**
     * \brief   The value of default size.
     **/
    constexpr ElemCount     DEFAULT_SIZE            { static_cast<ElemCount>(-2) };

    /**
     * \brief   The default block size to allocate string buffer.
     **/
    constexpr ElemCount     DEFAULT_BLOCK_SIZE      { 32 };

    /**
     * \brief   NECommon::MAXIMUM_WAITING_OBJECTS
     *          The maximum number of synchronization objects that is able to lock. 
     **/
    constexpr int           MAXIMUM_WAITING_OBJECTS { 64 };

    /**
     * \brief   NECommon::VALUE_MAX_INT8
     *          The maximum 8-bit signed integer value.
     **/
    constexpr int8_t        VALUE_MAX_INT8          { static_cast<int8_t>(0x7F) };

    /**
     * \brief   NECommon::VALUE_MIN_INT8
     *          The minimum 8-bit signed integer value.
     **/
    constexpr int8_t        VALUE_MIN_INT8          { static_cast<int8_t>(0x80) };

    /**
     * \brief   NECommon::VALUE_MAX_UINT8
     *          The maximum 8-bit unsigned integer value.
     **/
    constexpr uint8_t       VALUE_MAX_UINT8         { static_cast<uint8_t>(0xFFu) };

    /**
     * \brief   NECommon::VALUE_MAX_INT16
     *          The maximum 16-bit signed integer value.
     **/
    constexpr int16_t       VALUE_MAX_INT16         { static_cast<int16_t>(0x7FFF) };

    /**
     * \brief   NECommon::VALUE_MIN_INT16
     *          The minimum 16-bit signed integer value.
     **/
    constexpr int16_t       VALUE_MIN_INT16         { static_cast<int16_t>(0x8000) };

    /**
     * \brief   NECommon::VALUE_MAX_UINT16
     *          The maximum 16-bit unsigned integer value.
     **/
    constexpr uint16_t      VALUE_MAX_UINT16        { static_cast<uint16_t>(0xFFFFu) };

    /**
     * \brief   NECommon::VALUE_MAX_INT32
     *          The maximum 32-bit signed integer value.
     **/
    constexpr int32_t       VALUE_MAX_INT32         { static_cast<int32_t>(0x7FFFFFFF) };

    /**
     * \brief   NECommon::VALUE_MIN_INT32
     *          The minimum 32-bit signed integer value.
     **/
    constexpr int32_t       VALUE_MIN_INT32         { static_cast<int32_t>(0x80000000) };

    /**
     * \brief   NECommon::VALUE_MAX_UINT32
     *          The maximum 32-bit unsigned integer value.
     **/
    constexpr uint32_t      VALUE_MAX_UINT32        { static_cast<uint32_t>(0xFFFFFFFF) };

    /**
     * \brief   NECommon::MAX_CONTAINER_SIZE
     *          The maximum size of the containers such as array, list or map.
     **/
    constexpr uint32_t      MAX_CONTAINER_SIZE      { static_cast<uint32_t>(VALUE_MAX_INT32) };

    /**
     * \brief   NECommon::MAP_DEFAULT_HASH_SIZE
     *          The size of hash table by default created in map. For hash map internal use
     **/
    constexpr uint32_t      MAP_DEFAULT_HASH_SIZE   { static_cast<int>(64) };

    /**
     * \brief   NECommon::ARRAY_DEFAULT_CAPACITY
     *          The default capacity of the array.
     **/
    constexpr uint32_t      ARRAY_DEFAULT_CAPACITY  { static_cast<int>(2048) };

    /**
     * \brief   NECommon::RING_START_POSITION
     *          Defines first index in the ring.
     **/
    constexpr uint32_t      RING_START_POSITION     { static_cast<uint32_t>(MAX_UINT_32) };

    /**
     * \brief   NECommon::WAIT_INFINITE
     *          Infinitive timeout waiting time -1 / 0xFFFFFFFF
     *          until a certain waiting event did not happen
     **/
    constexpr unsigned int   WAIT_INFINITE          { ~0u };
    /**
     * \brief   NECommon::DO_NOT_WAIT
     *          Do not wait for certain waiting event
     **/
    constexpr unsigned int   DO_NOT_WAIT            { 0u };
    /**
     * \brief   NECommon::WAIT_SWITCH
     *          Constant, used to switch the thread. Minimum waiting time.
     **/
    constexpr unsigned int   WAIT_SWITCH            { 1u };
    /**
     * \brief   NECommon::WAIT_1_MILLISECOND
     *          Constant, wait for 1 millisecond of created thread startup
     **/
    constexpr unsigned int   WAIT_1_MILLISECOND     { 1u };
    /**
     * \brief   NECommon::WAIT_5_MILLISECONDS
     *          Constant, wait for 5 milliseconds of created thread startup
     **/
    constexpr unsigned int   WAIT_5_MILLISECONDS    { 5u };
    /**
     * \brief   NECommon::WAIT_10_MILLISECONDS
     *          Constant, wait for 10 milliseconds of created thread startup
     **/
    constexpr unsigned int   WAIT_10_MILLISECONDS   { 10u };
    /**
     * \brief   NECommon::WAIT_50_MILLISECONDS
     *          Constant, wait for 10 milliseconds of created thread startup
     **/
    constexpr unsigned int   WAIT_50_MILLISECONDS   { 50u };
    /**
     * \brief   NECommon::WAIT_100_MILLISECONDS
     *          Constant, wait for 100 milliseconds of created thread startup
     **/
    constexpr unsigned int   WAIT_100_MILLISECONDS  { 100u };
    /**
     * \brief   NECommon::WAIT_500_MILLISECONDS
     *          Constant, wait for 500 milliseconds of created thread startup
     **/
    constexpr unsigned int   WAIT_500_MILLISECONDS  { 500u };
    /**
     * \brief   NECommon::WAIT_1_SECOND
     *          Constant, wait for 1 second of created thread startup
     **/
    constexpr unsigned int  WAIT_1_SECOND           { 1 * 1'000u };
    /**
     * \brief   NECommon::WAIT_5_SECONDS
     *          Constant, wait for 5 seconds of created thread startup
     **/
    constexpr unsigned int  WAIT_5_SECONDS          { 5 * WAIT_1_SECOND };
    /**
     * \brief   NECommon::WAIT_10_SECONDS
     *          Constant, wait for 10 seconds of created thread startup
     **/
    constexpr unsigned int  WAIT_10_SECONDS         { 10 * WAIT_1_SECOND };
    /**
     * \brief   IESynchObject::WAIT_1_MIN
     *          Waiting time 1 minute
     **/
    constexpr unsigned int  WAIT_1_MINUTE           { 60 * WAIT_1_SECOND };

    /**
     * \brief   NECommon::COMPONENT_PATH_SEPARATOR
     *          Constant. Predefined path separator for address of
     *          threads, components, proxies or stubs.
     **/
    constexpr std::string_view  COMPONENT_PATH_SEPARATOR        { "::" };
    /**
     * \brief   NECommon::COMPONENT_ITEM_SEPARATOR
     *          Constant. Predefined path separator of component named item.
     *          The name items can be Worker threads and/or timers of
     *          component. If Service Interface requires worker threads
     *          and Service Interface is used in different components with
     *          different role names, the named items can differ by
     *          unique role name contained in the named items.
     **/
    constexpr std::string_view  COMPONENT_ITEM_SEPARATOR        { ":" };
    /**
     * \brief   NECommon::DEFAULT_SPECIAL_CHAR
     *          Constant. Defines default special character used in generated names.
     **/
    constexpr std::string_view  DEFAULT_SPECIAL_CHAR            { "_" };
    /**
     * \brief   NECommon::BOOLEAN_TRUE
     *          Boolean 'true' value as a string.
     **/
    constexpr std::string_view  BOOLEAN_TRUE                    { "true" };   //!< Boolean value 'true' as string
    /**
     * \brief   NECommon::BOOLEAN_FALSE
     *          Boolean 'false' value as a string.
     **/
    constexpr std::string_view  BOOLEAN_FALSE                   { "false" };  //!< Boolean value 'false' as string

    /**
     * \brief   NECommon::OBJECT_SEPARATOR
     *          Object separator such as channel or version numbers.
     **/
    constexpr char              OBJECT_SEPARATOR                { '.' };
}
