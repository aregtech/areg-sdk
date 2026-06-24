#ifndef AREG_BASE_COMMONDEFS_HPP
#define AREG_BASE_COMMONDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/CommonDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Common constants
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <string_view>
#include <limits>
#include <utility>

namespace areg {

    using ElemPos   = int32_t;   //!< The type of character position in the string.

    using ElemCount = int32_t;   //!< The type of characters count in the string.
    
    /**
     * \brief   Overlapping of elements in ring buffer or stack means the cases
     *          when buffer or stack is full, i.e. start and end positions match
     *          and number of elements in buffer or stack is equal to stack
     *          capacity. On next request to push element, the stack may change
     *          behavior, depending on overlapping flag, which are:
     *          1.  Stop  --  Do not insert any new element if ring is full.
     *          2.  Shift --  Do not change ring capacity if it is full,
     *                        but shift start position, i.e. overwrite 
     *                        the first element of ring and insert new element.
     *          3. Resize --  Always resize ring if it is full. This changes
     *                        the actual capacity of ring, making it bigger.
     **/
    enum class OverlapPolicy  : uint8_t
    {
          Stop     = 0 //!< Do not insert new element, if ring is full. New element is lost.
        , Shift    = 1 //!< Shifts start position, if ring is full, insert new element, and do not resize ring. Last element is lost.
        , Resize   = 2 //!< Resize ring, if it is full. No element is lost.
    };

    /**
     * \brief   Sorting criteria for containers
     **/
    enum class SortOrder    : uint8_t
    {
          NoSorting     //!< No sorting.
        , Descending    //!< Sort descending
        , Ascending     //!< Sort ascending
    };

    /**
     * \brief   Indicates the index of an element in the buffer.
     **/
    enum class Index    : ElemPos
    {
          Invalid   = -1    //!< Indicates the invalid index in any buffer
        , First     =  0    //!< Indicates the first index in a buffer
        , Last      = -2    //!< Indicates the end of the buffer, i.e. the last position.
    };

    /**
     * \brief   Indicates the boolean state assigned of persisted.
     **/
    enum class Bool     : int8_t
    {
          Undefined = -1
        , False     = 0
        , True      = 1
    };

    /**
     * \brief   The success return code.
     **/
    constexpr int32_t       RETURNED_OK             { 0 };

    /**
     * \brief   The value of invalid position in buffer, list or array.
     **/
    constexpr ElemPos       INVALID_INDEX           { static_cast<ElemPos>(areg::Index::Invalid) };

    /**
     * \brief   The first valid index in the buffer.
     **/
    constexpr ElemPos       FIRST_INDEX             { static_cast<ElemPos>(areg::Index::First) };
    
    /**
     * \brief   The last valid position in the string.
     **/
    constexpr ElemPos       LAST_INDEX              { static_cast<ElemPos>(areg::Index::Last) };
    
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
     * \brief   The invalid position as uint32_t.
     */
    constexpr uint32_t      INVALID_POSITION        { static_cast<uint32_t>(~0) };

    /**
     * \brief   The maximum number of synchronization objects that is able to lock. 
     **/
    constexpr int32_t       MAXIMUM_WAITING_OBJECTS { 64 };

    /**
     * \brief   The maximum 8-bit signed integer value.
     **/
    constexpr int8_t        VALUE_MAX_INT8          { std::numeric_limits<int8_t>::max() };

    /**
     * \brief   The minimum 8-bit signed integer value.
     **/
    constexpr int8_t        VALUE_MIN_INT8          { std::numeric_limits<int8_t>::min() };

    /**
     * \brief   The maximum 8-bit unsigned integer value.
     **/
    constexpr uint8_t       VALUE_MAX_UINT8         { std::numeric_limits<uint8_t>::max() };

    /**
     * \brief   The maximum 16-bit signed integer value.
     **/
    constexpr int16_t       VALUE_MAX_INT16         { std::numeric_limits<int16_t>::max() };

    /**
     * \brief   The minimum 16-bit signed integer value.
     **/
    constexpr int16_t       VALUE_MIN_INT16         { std::numeric_limits<int16_t>::min() };

    /**
     * \brief   The maximum 16-bit unsigned integer value.
     **/
    constexpr uint16_t      VALUE_MAX_UINT16        { std::numeric_limits<uint16_t>::max() };

    /**
     * \brief   The maximum 32-bit signed integer value.
     **/
    constexpr int32_t       VALUE_MAX_INT32         { std::numeric_limits<int32_t>::max() };

    /**
     * \brief   The minimum 32-bit signed integer value.
     **/
    constexpr int32_t   VALUE_MIN_INT32             { std::numeric_limits<int32_t>::min() };

    /**
     * \brief   The maximum 32-bit unsigned integer value.
     **/
    constexpr uint32_t  VALUE_MAX_UINT32            { std::numeric_limits<uint32_t>::max() };

    /**
     * \brief   The maximum size of the containers such as array, list or map.
     **/
    constexpr uint32_t  MAX_CONTAINER_SIZE          { static_cast<uint32_t>(VALUE_MAX_INT32 - 1) };

    /**
     * \brief   The size of hash table by default created in map. For hash map internal use
     **/
    constexpr uint32_t  MAP_DEFAULT_HASH_SIZE       { static_cast<uint32_t>(64u - 1u) };

    /**
     * \brief   The default capacity of the array.
     **/
    constexpr uint32_t  ARRAY_DEFAULT_CAPACITY      { static_cast<uint32_t>(64u) };

    /**
     * \brief   Defines the first index in the ring.
     **/
    constexpr uint32_t  RING_START_POSITION         { std::numeric_limits<uint32_t>::max()};

    /**
     * \brief   Infinitive timeout waiting time -1 / 0xFFFFFFFF
     *          until a certain waiting event did not happen
     **/
    constexpr uint32_t   WAIT_INFINITE              { std::numeric_limits<uint32_t>::max() };
    /**
     * \brief   Do not wait for certain waiting event
     **/
    constexpr uint32_t   DO_NOT_WAIT                { 0u };
    /**
     * \brief   Constant, used to switch the thread. Minimum waiting time.
     **/
    constexpr uint32_t   WAIT_SWITCH                { 1u };
    /**
     * \brief   Constant, wait for 1 millisecond of created thread startup
     **/
    constexpr uint32_t   WAIT_1_MILLISECOND         { 1u };
    /**
     * \brief   Constant, wait for 5 milliseconds of created thread startup
     **/
    constexpr uint32_t   WAIT_5_MILLISECONDS        { 5u };
    /**
     * \brief   Constant, wait for 10 milliseconds of created thread startup
     **/
    constexpr uint32_t   WAIT_10_MILLISECONDS       { 10u };
    /**
     * \brief   Constant, wait for 10 milliseconds of created thread startup
     **/
    constexpr uint32_t   WAIT_50_MILLISECONDS       { 50u };
    /**
     * \brief   Constant, wait for 100 milliseconds of created thread startup
     **/
    constexpr uint32_t   WAIT_100_MILLISECONDS      { 100u };
    /**
     * \brief   Constant, wait for 500 milliseconds of created thread startup
     **/
    constexpr uint32_t   WAIT_500_MILLISECONDS      { 500u };
    /**
     * \brief   Constant, wait for 1 second of created thread startup
     **/
    constexpr uint32_t  WAIT_1_SECOND               { 1 * 1'000u };
    /**
     * \brief   Constant, wait for 5 seconds of created thread startup
     **/
    constexpr uint32_t  WAIT_5_SECONDS              { 5 * WAIT_1_SECOND };
    /**
     * \brief   Constant, wait for 10 seconds of created thread startup
     **/
    constexpr uint32_t  WAIT_10_SECONDS             { 10 * WAIT_1_SECOND };
    /**
     * \brief   Waiting time 1 minute
     **/
    constexpr uint32_t  WAIT_1_MINUTE               { 60 * WAIT_1_SECOND };

    /**
     * \brief   A value, indicating invalid timeout. The timers with invalid timeouts are invalid
     **/
    constexpr uint32_t   INVALID_TIMEOUT            { DO_NOT_WAIT };

    /**
     * \brief   A value used when declare component thread. It indicates that watchdog should be ignored.
     **/
    constexpr uint32_t   WATCHDOG_IGNORE            { DO_NOT_WAIT };

    /**
     * \brief   A value used when set the stack size of the thread. It indicates that setting the stack size should be ignored.
     **/
    constexpr uint32_t   DEFAULT_STACK_SIZE         { 0u };

    /**
     * \brief   The minimum stack size for a system thread.
     **/
    constexpr uint32_t   SYSTEM_THREAD_STACK_MIN    { 32u };

    /**
     * \brief   The normal stack size for a system thread.
     **/
    constexpr uint32_t   SYSTEM_THREAD_STACK_NORMAL { SYSTEM_THREAD_STACK_MIN * 2u };

    /**
     * \brief   The big stack size for a system thread.
     **/
    constexpr uint32_t   SYSTEM_THREAD_STACK_BIG    { SYSTEM_THREAD_STACK_NORMAL * 4u };

    /**
     * \brief   Maximum queue size value used when declaring a component thread.
     **/
    constexpr uint32_t  QUEUE_SIZE_MAXIMUM          { std::numeric_limits<uint32_t>::max() };

    /**
     * \brief   areg::IGNORE_VALUE
     *          A value used to indicate that it should be ignored.
     **/
    constexpr uint32_t  IGNORE_VALUE                { 0u };

    /**
     * \brief   areg::TIMEOUT_1_MS
     *          Timeout 1 millisecond
     **/
    constexpr uint32_t   TIMEOUT_1_MS               { WAIT_1_MILLISECOND};

    /**
     * \brief   areg::TIMEOUT_10_MS
     *          Timeout 10 millisecond
     **/
    constexpr uint32_t   TIMEOUT_10_MS              { WAIT_10_MILLISECONDS };

    /**
     * \brief   areg::TIMEOUT_50_MS
     *          Timeout 50 millisecond
     **/
    constexpr uint32_t   TIMEOUT_50_MS              { WAIT_50_MILLISECONDS };

    /**
     * \brief   areg::TIMEOUT_100_MS
     *          Timeout 100 millisecond
     **/
    constexpr uint32_t  TIMEOUT_100_MS              { WAIT_100_MILLISECONDS };

    /**
     * \brief   areg::TIMEOUT_500_MS
     *          Timeout 500 millisecond
     **/
    constexpr uint32_t  TIMEOUT_500_MS              { WAIT_500_MILLISECONDS };

    /**
     * \brief   areg::TIMEOUT_1_SEC
     *          Timeout 1 second
     **/
    constexpr uint32_t  TIMEOUT_1_SEC               { WAIT_1_SECOND };

    /**
     * \brief   areg::TIMEOUT_1_MIN
     *          Timeout 1 minute
     **/
    constexpr uint32_t  TIMEOUT_1_MIN               { WAIT_1_MINUTE };

    /**
     * \brief   areg::DURATION_1_NS
     *          1 nanosecond duration.
     **/
    constexpr uint32_t  DURATION_1_NS               { 1u };

    /**
     * \brief   areg::STR_1_NS_SHORT
     *          1 nanosecond int16_t abbreviation.
     **/
    constexpr std::string_view  STR_1_NS_SHORT      { "ns" };

    /**
     * \brief   areg::DURATION_1_MICRO
     *          1 microsecond duration in nanoseconds.
     **/
    constexpr uint32_t  DURATION_1_MICRO            { 1'000 * DURATION_1_NS };

    /**
     * \brief   areg::STR_1_MICROSEC_SHORT
     *          1 microsecond int16_t abbreviation.
     **/
    constexpr std::string_view  STR_1_MICROSEC_SHORT{ "us" };

    /**
     * \brief   areg::DURATION_1_MILLI
     *          1 millisecond duration in nanoseconds.
     **/
    constexpr uint32_t  DURATION_1_MILLI            { 1'000 * DURATION_1_MICRO };

    /**
     * \brief   areg::STR_1_MILLISEC_SHORT
     *          1 millisecond int16_t abbreviation.
     **/
    constexpr std::string_view  STR_1_MILLISEC_SHORT{ "ms" };

    /**
     * \brief   areg::DURATION_1_SEC
     *          1 second duration in nanoseconds.
     **/
    constexpr uint32_t  DURATION_1_SEC              { 1'000 * DURATION_1_MILLI };

    /**
     * \brief   areg::STR_1_SEC_SHORT
     *          1 second int16_t abbreviation.
     **/
    constexpr std::string_view  STR_1_SEC_SHORT     { "sec" };

    /**
     * \brief   areg::DURATION_DURATION_1_MIN1_SEC
     *          1 minute duration in nanoseconds.
     **/
    constexpr uint64_t          DURATION_1_MIN      { 60 * static_cast<uint64_t>(DURATION_1_SEC) };

    /**
     * \brief   areg::STR_1_MIN_SHORT
     *          1 minute int16_t abbreviation.
     **/
    constexpr std::string_view  STR_1_MIN_SHORT     { "min" };

    /**
     * \brief   areg::BITS_IN_BYTE
     *          Bits in one byte.
     **/
    constexpr uint32_t  BITS_IN_BYTE                { 8u };

    /**
     * \brief   areg::ONE_BYTE
     *          Size of one byte.
     **/
    constexpr uint32_t  ONE_BYTE                    { 1u };

    /**
     * \brief   areg::STR_ONE_BYTE
     *          name (aligned) of one byte.
     **/
    constexpr std::string_view  STR_ONE_BYTE        { " Byte" };

    /**
     * \brief   areg::ONE_KILOBYTE
     *          Size of one Kilobyte.
     **/
    constexpr uint32_t  ONE_KILOBYTE                { 1'024 * ONE_BYTE };

    /**
     * \brief   areg::STR_ONE_KILOBYTE
     *          name (aligned) of one Kilobyte.
     **/
    constexpr std::string_view  STR_ONE_KILOBYTE    { "KByte" };

    /**
     * \brief   areg::ONE_MEGABYTE
     *          Size of one Megabyte.
     **/
    constexpr uint32_t  ONE_MEGABYTE                { ONE_KILOBYTE * ONE_KILOBYTE };

    /**
     * \brief   areg::STR_ONE_MEGABYTE
     *          name (aligned) of one Megabyte.
     **/
    constexpr std::string_view  STR_ONE_MEGABYTE    { "MByte" };

    /**
     * \brief   areg::ONE_GIGABYTE
     *          Size of one Gigabyte.
     **/
    constexpr uint32_t  ONE_GIGABYTE                { ONE_KILOBYTE * ONE_MEGABYTE };

    /**
     * \brief   areg::STR_ONE_GIGABYTE
     *          name (aligned) of one Gigabyte.
     **/
    constexpr std::string_view  STR_ONE_GIGABYTE    { "GByte" };

    /**
     * \brief   areg::COMPONENT_PATH_SEPARATOR
     *          Constant. Predefined path separator for address of
     *          threads, components, proxies or stubs.
     **/
    constexpr std::string_view  COMPONENT_PATH_SEPARATOR        { "::" };
    /**
     * \brief   areg::COMPONENT_ITEM_SEPARATOR
     *          Constant. Predefined path separator of component named item.
     *          The name items can be Worker threads and/or timers of
     *          component. If Service Interface requires worker threads
     *          and Service Interface is used in different components with
     *          different role names, the named items can differ by
     *          unique role name contained in the named items.
     **/
    constexpr std::string_view  COMPONENT_ITEM_SEPARATOR        { ":" };
    /**
     * \brief   areg::DEFAULT_SPECIAL_CHAR
     *          Constant. Defines default special character used in generated names.
     **/
    constexpr std::string_view  DEFAULT_SPECIAL_CHAR            { "_" };
    /**
     * \brief   areg::BOOLEAN_TRUE
     *          Boolean 'true' value as a string.
     **/
    constexpr std::string_view  BOOLEAN_TRUE                    { "true" };   //!< Boolean value 'true' as string
    /**
     * \brief   areg::BOOLEAN_FALSE
     *          Boolean 'false' value as a string.
     **/
    constexpr std::string_view  BOOLEAN_FALSE                   { "false" };  //!< Boolean value 'false' as string

    /**
     * \brief   areg::OBJECT_SEPARATOR
     *          Object separator such as channel or version numbers.
     **/
    constexpr char              OBJECT_SEPARATOR                { '.' };

    /**
     * \brief   areg::Cookie
     *          The list of reserved cookie values
     **/
    enum class  Cookie  : ITEM_ID
    {
          Invalid       = 0     //!< Invalid cookie value
        , Local         = 1     //!< Valid cookie value of local services
        , Router        = 2     //!< Valid cookie value of the message routing system service
        , Logger        = 3     //!< Valid cookie value of the log collector system service
        , Any           = 255   //!< Any valid cookie
        , FirstRemote   = 256   //!< First valid cookie of any other remote service
    };

    /**
     * \brief   Sequence number predefining notification message ID
     **/
    constexpr SequenceNumber    SEQUENCE_NUMBER_NOTIFY  { std::numeric_limits<SequenceNumber>::min() }; /*0x00000000*/
    /**
     * \brief   Any sequence number, used in messages. "Any sequence number" used to find any listener object with same message ID.
     **/
    constexpr SequenceNumber    SEQUENCE_NUMBER_ANY     { std::numeric_limits<SequenceNumber>::max() }; /*0xFFFFFFFF*/

    /**
     * \brief   areg::INVALID_VALUE
     *          Constant. Defines invalid value for remote buffer.
     **/
    constexpr uint32_t  INVALID_VALUE               { ~0u };

    /**
     * \brief   areg::INVALID_SIZE
     *          Constant. Defines invalid buffer size.
     **/
    constexpr uint32_t  INVALID_SIZE                { areg::INVALID_VALUE };

    /**
     * \brief   The invalid message ID
     **/
    constexpr uint32_t  INVALID_MESSAGE_ID          { areg::INVALID_VALUE };    /*0xFFFFFFFF*/

    /**
     * \brief   Unknown cookie
     **/
    constexpr ITEM_ID   COOKIE_UNKNOWN              { static_cast<ITEM_ID>(areg::Cookie::Invalid) };
    /**
     * \brief   The indication of local service.
     **/
    constexpr ITEM_ID   COOKIE_LOCAL                { static_cast<ITEM_ID>(areg::Cookie::Local) };
    /**
     * \brief   Indicates message router cookie
     **/
    constexpr ITEM_ID   COOKIE_ROUTER               { static_cast<ITEM_ID>(areg::Cookie::Router) };
    /**
     * \brief   Indicates log collector cookie
     **/
    constexpr ITEM_ID   COOKIE_LOGGER               { static_cast<ITEM_ID>(areg::Cookie::Logger) };
    /**
     * \brief   Indicates any valid cookie
     **/
    constexpr ITEM_ID   COOKIE_ANY                  { static_cast<ITEM_ID>(areg::Cookie::Any) };
    /**
     * \brief   The unknown target ID
     **/
    constexpr ITEM_ID   TARGET_UNKNOWN              { static_cast<ITEM_ID>(areg::Cookie::Invalid) };
    /**
     * \brief   The local target ID
     **/
    constexpr ITEM_ID   TARGET_LOCAL                { static_cast<ITEM_ID>(areg::Cookie::Local) };
    /**
     * \brief   The undefined (all) target ID
     **/
    constexpr ITEM_ID   TARGET_ALL                  { static_cast<ITEM_ID>(areg::COOKIE_ANY) };
    /**
     * \brief   The unknown source ID
     **/
    constexpr ITEM_ID   SOURCE_UNKNOWN              { static_cast<ITEM_ID>(areg::Cookie::Invalid) };
    /**
     * \brief   The unknown source ID.
     **/
    constexpr ITEM_ID   SOURCE_LOCAL                { static_cast<ITEM_ID>(areg::Cookie::Local) };
    /**
     * \brief   The ID of first valid remote cookie.
     **/
    constexpr ITEM_ID   COOKIE_REMOTE_SERVICE       { static_cast<ITEM_ID>(areg::Cookie::FirstRemote) };

    /**
     * \brief   Sentinel tag for zero-allocation ("null") construction.
     **/
    struct NullTag {};

} // namespace areg

#endif  // AREG_BASE_COMMONDEFS_HPP
