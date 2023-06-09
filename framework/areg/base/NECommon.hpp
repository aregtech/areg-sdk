#ifndef AREG_BASE_NECOMMON_HPP
#define AREG_BASE_NECOMMON_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
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
    enum class eRingOverlap : uint8_t
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

    };

    /**
     * \brief   Sorting criteria for containers
     **/
    enum class eSort   : uint8_t
    {
          NoSorting         //!< No sorting.
        , SortDescending    //!< Sort descending
        , SortAscending     //!< Sort ascending
    };

    /**
     * \brief   Indicates the index of an element in the buffer.
     **/
    enum class eIndex  : ElemPos
    {
          InvalidIndex  = -1    //!< Indicates the invalid index in any buffer
        , FirtIndex     =  0    //!< Indicates the first index in a buffer
        , LastIndex     = -2    //!< Indicates the end of the buffer, i.e. the last position.
    };

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
     * \brief   NECommon::INVALID_POSITIOIN
     *          The invalid position as unsigned int.
     */
    constexpr unsigned int  INVALID_POSITION        { static_cast<unsigned int>(~0) };

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
    constexpr int           VALUE_MIN_INT32         { static_cast<int32_t>(0x80000000) };

    /**
     * \brief   NECommon::VALUE_MAX_UINT32
     *          The maximum 32-bit unsigned integer value.
     **/
    constexpr unsigned int  VALUE_MAX_UINT32        { static_cast<unsigned int>(0xFFFFFFFF) };

    /**
     * \brief   NECommon::MAX_CONTAINER_SIZE
     *          The maximum size of the containers such as array, list or map.
     **/
    constexpr unsigned int  MAX_CONTAINER_SIZE      { static_cast<unsigned int>(VALUE_MAX_INT32 - 1) };

    /**
     * \brief   NECommon::MAP_DEFAULT_HASH_SIZE
     *          The size of hash table by default created in map. For hash map internal use
     **/
    constexpr unsigned int  MAP_DEFAULT_HASH_SIZE   { static_cast<unsigned int>(64 - 1) };

    /**
     * \brief   NECommon::ARRAY_DEFAULT_CAPACITY
     *          The default capacity of the array.
     **/
    constexpr unsigned int  ARRAY_DEFAULT_CAPACITY  { static_cast<unsigned int>(2048) };

    /**
     * \brief   NECommon::RING_START_POSITION
     *          Defines first index in the ring.
     **/
    constexpr unsigned int  RING_START_POSITION     { static_cast<unsigned int>(MAX_UINT_32) };

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
     * \brief   NECommon::WAIT_1_MIN
     *          Waiting time 1 minute
     **/
    constexpr unsigned int  WAIT_1_MINUTE           { 60 * WAIT_1_SECOND };

    /**
     * \brief   NECommon::INVALID_TIMEOUT
     *          A value, indicating invalid timeout. The timers with invalid timeouts are invalid
     **/
    constexpr unsigned int   INVALID_TIMEOUT        { DO_NOT_WAIT };

    /**
     * \brief   NECommon::WATCHDOG_IGNORE
     *          A value used when declare component thread. It indicates that watchdog should be ignored.
     **/
    constexpr unsigned int   WATCHDOG_IGNORE        { DO_NOT_WAIT };

    /**
     * \brief   NECommon::TIMEOUT_1_MS
     *          Timeout 1 millisecond
     **/
    constexpr unsigned int   TIMEOUT_1_MS           { WAIT_1_MILLISECOND};

    /**
     * \brief   NECommon::TIMEOUT_10_MS
     *          Timeout 10 millisecond
     **/
    constexpr unsigned int   TIMEOUT_10_MS          { WAIT_10_MILLISECONDS };

    /**
     * \brief   NECommon::TIMEOUT_50_MS
     *          Timeout 50 millisecond
     **/
    constexpr unsigned int   TIMEOUT_50_MS          { WAIT_50_MILLISECONDS };

    /**
     * \brief   NECommon::TIMEOUT_100_MS
     *          Timeout 100 millisecond
     **/
    constexpr unsigned int  TIMEOUT_100_MS          { WAIT_100_MILLISECONDS };

    /**
     * \brief   NECommon::TIMEOUT_500_MS
     *          Timeout 500 millisecond
     **/
    constexpr unsigned int  TIMEOUT_500_MS          { WAIT_500_MILLISECONDS };

    /**
     * \brief   NECommon::TIMEOUT_1_SEC
     *          Timeout 1 second
     **/
    constexpr unsigned int  TIMEOUT_1_SEC           { WAIT_1_SECOND };

    /**
     * \brief   NECommon::TIMEOUT_1_MIN
     *          Timeout 1 minute
     **/
    constexpr unsigned int  TIMEOUT_1_MIN           { WAIT_1_MINUTE };

    /**
     * \brief   NECommon::DURATION_1_NS
     *          1 nanosecond duration.
     **/
    constexpr unsigned int  DURATION_1_NS           { 1 };

    /**
     * \brief   NECommon::STR_1_NS_SHORT
     *          1 nansecond short abbreviation.
     **/
    constexpr std::string_view  STR_1_NS_SHORT      { "ns" };

    /**
     * \brief   NECommon::DURATION_1_MICRO
     *          1 microsecond duration in nanoseconds.
     **/
    constexpr unsigned int  DURATION_1_MICRO        { 1'000 * DURATION_1_NS };

    /**
     * \brief   NECommon::STR_1_MICROSEC_SHORT
     *          1 microsecond short abbreviation.
     **/
    constexpr std::string_view  STR_1_MICROSEC_SHORT{ "us" };

    /**
     * \brief   NECommon::DURATION_1_MILLI
     *          1 millisecond duration in nanoseconds.
     **/
    constexpr unsigned int  DURATION_1_MILLI        { 1'000 * DURATION_1_MICRO };

    /**
     * \brief   NECommon::STR_1_MILLISEC_SHORT
     *          1 millisecond short abbreviation.
     **/
    constexpr std::string_view  STR_1_MILLISEC_SHORT{ "ms" };

    /**
     * \brief   NECommon::DURATION_1_SEC
     *          1 second duration in nanoseconds.
     **/
    constexpr unsigned int  DURATION_1_SEC          { 1'000 * DURATION_1_MILLI };

    /**
     * \brief   NECommon::STR_1_SEC_SHORT
     *          1 second short abbreviation.
     **/
    constexpr std::string_view  STR_1_SEC_SHORT     { "sec" };

    /**
     * \brief   NECommon::DURATION_DURATION_1_MIN1_SEC
     *          1 minute duration in nanoseconds.
     **/
    constexpr unsigned int  DURATION_1_MIN          { 60 * DURATION_1_SEC };

    /**
     * \brief   NECommon::STR_1_MIN_SHORT
     *          1 minute short abbreviation.
     **/
    constexpr std::string_view  STR_1_MIN_SHORT     { "min" };

    /**
     * \brief   NECommon::BITS_IN_BYTE
     *          Bits in one byte.
     **/
    constexpr unsigned int  BITS_IN_BYTE            { 8 };

    /**
     * \brief   NECommon::ONE_BYTE
     *          Size of one byte.
     **/
    constexpr unsigned int  ONE_BYTE                { 1 };

    /**
     * \brief   NECommon::STR_ONE_BYTE
     *          Name (aligned) of one byte.
     **/
    constexpr std::string_view  STR_ONE_BYTE        { " Byte" };

    /**
     * \brief   NECommon::ONE_KILOBYTE
     *          Size of one Kilobyte.
     **/
    constexpr unsigned int  ONE_KILOBYTE            { 1'000 * ONE_BYTE };

    /**
     * \brief   NECommon::STR_ONE_KILOBYTE
     *          Name (aligned) of one Kilobyte.
     **/
    constexpr std::string_view  STR_ONE_KILOBYTE    { "KByte" };

    /**
     * \brief   NECommon::ONE_MEGABYTE
     *          Size of one Megabyte.
     **/
    constexpr unsigned int  ONE_MEGABYTE            { 1'000 * ONE_KILOBYTE };

    /**
     * \brief   NECommon::STR_ONE_MEGABYTE
     *          Name (aligned) of one Megabyte.
     **/
    constexpr std::string_view  STR_ONE_MEGABYTE    { "MByte" };

    /**
     * \brief   NECommon::ONE_GIGABYTE
     *          Size of one Gigabyte.
     **/
    constexpr unsigned int  ONE_GIGABYTE            { 1'000 * ONE_MEGABYTE };

    /**
     * \brief   NECommon::STR_ONE_GIGABYTE
     *          Name (aligned) of one Gigabyte.
     **/
    constexpr std::string_view  STR_ONE_GIGABYTE    { "GByte" };

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

#endif  // AREG_BASE_NECOMMON_HPP
