#ifndef AREG_BASE_MEMORYDEFS_HPP
#define AREG_BASE_MEMORYDEFS_HPP    
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/MemoryDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Basic functions and types to deal with objects 
 *              located in memory.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/MathDefs.hpp"

#include <algorithm>
#include <new>
#include <string.h>

#if defined(_MSC_VER)
    #include <intrin.h>     // _InterlockedIncrement16 / _InterlockedDecrement16 for the intrusive refcount
#endif  // _MSC_VER

/**
 * \brief   Memory specifies basic functions and types for objects located in memory
 *          In this namespace are defined such types like Byte Buffer structure and 
 *          Align union. As well as some utility functions requiring working with 
 *          buffers of array of elements.
 *          For more details see types and functions description.
 *
 **/
namespace areg {

//////////////////////////////////////////////////////////////////////////
// Internal types used for alignment
//////////////////////////////////////////////////////////////////////////

class _EmptyClass;                                  //!< Dummy class declaration
typedef void   (*EmptyMethod)();                    //!< Dummy pointer to global function declaration
typedef void   (_EmptyClass::*_EmptyClassMethod)(); //!< Dummy pointer to class function declaration
typedef int32_t _EmptyClass::*_EmptyClassMember;    //!< Dummy pointer to class variable declaration

/**
 * \brief   areg::BufferData
 **/
using BufferData = uint8_t;

/**
 * \brief   Align structure template
 *          defining single element used in Align union.
 * \tparam  ELEM_TYPE   Any type.
 **/
template <class ELEM_TYPE>
struct Align
{
    ELEM_TYPE   mElement;
};

//////////////////////////////////////////////////////////////////////////
// areg::Primitive union declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   areg::Primitive
 *          Union of all C++ primitive types, sized to the largest member.
 *          Used as the alignment unit for buffer allocation and as a
 *          container for storing or serializing a single primitive value.
 *
 * \note    Do not serialize pointer-to-function or pointer-to-member members.
 **/
union Primitive
{
    Align<bool>                     valBool;    //!< boolean value
    Align<char>                     valChar;    //!< char value
    Align<uint8_t>                  valUChar;   //!< uint8_t value
    Align<wchar_t>                  valWChar;   //!< wide char value
    Align<int16_t>                  valShort;   //!< int16_t value
    Align<uint16_t>                 valUShort;  //!< uint16_t value
    Align<int32_t>                  valInt;     //!< int32_t value
    Align<uint32_t>                 valUInt;    //!< uint32_t value
    Align<long double>              valLDouble; //!< long double value
    Align<long>                     valLong;    //!< long value
    Align<unsigned long>            valULong;   //!< unsigned long value
    Align<int64_t>                  valInt64;   //!< 64-bit integer value
    Align<uint64_t>                 valUInt64;  //!< 64-bit unsigned integer value
    Align<float>                    valFloat;   //!< float value
    Align<double>                   valDouble;  //!< double
    Align<void *>                   valPtr;     //!< pointer value
    Align<void (*)()>               valFunc;    //!< pointer to function value
    Align<_EmptyClass *>            valClsPtr;  //!< pointer to class value
    Align<void (_EmptyClass::*)()>  valClsFunc; //!< pointer to class function value
    Align<int32_t _EmptyClass::*>   valClsVar;  //!< pointer to class variable value
};

/**
 * \brief   Message communication results
 **/
enum class MessageResult : int32_t
{
      UnknownError          = -1    //!< Error, unknown type
    , Succeed               =  0    //!< No error
    , NoConnection          =  1    //!< Error, there is no connection
    , ProviderUnavailable           //!< Error, service is unavailable
    , ConsumerRejected              //!< Error, service is rejected
    , TargetUnavailable             //!< Error, the target object is unavailable
};

/**
 * \brief   Converts areg::MessageResult enumeration to string representation for logging and output.
 *
 * \param   msgResult       MessageResult value to convert.
 **/
[[nodiscard]]
inline constexpr const char * as_string( areg::MessageResult msgResult ) noexcept;

/**
 * \brief   Types of data buffer
 **/
enum class BufferType : int16_t
{
      Unknown   = -1    //!< Unknown buffer type, not used
    , Internal  =  0    //!< Buffer type for internal communication
    , Remote    =  2    //!< Buffer type for remote communication
    , Custom    =  3
};
/**
 * \brief   Returns string representation of areg::BufferType enumeration value.
 *
 * \param   val     BufferType value to convert.
 **/
[[nodiscard]]
inline constexpr const char * as_string( areg::BufferType val ) noexcept;

/**
 * \brief   Constant. Defines the minimum size of Byte Buffer data
 *          Also defines the size to align buffer allocation.
 **/
constexpr uint32_t      BLOCK_SIZE      { sizeof( uint64_t ) * 8 };
/**
 * \brief   Defines the message result success.
 **/
constexpr uint32_t      MESSAGE_SUCCESS { static_cast<uint32_t>(areg::MessageResult::Succeed) };

/**
 * \brief   Predefined Invalid Element value. Used to defined invalid element.
 *          For example, it is used in thread local storage object (ThreadLocalStorage)
 *          in case if there is no element found in local storage by given name.
 **/
constexpr  areg::Primitive  InvalidElement{{0}};

/**
 * \brief   Returns true if two Primitive elements are equal.
 *
 * \param   lsh     Left-hand Primitive operand.
 * \param   rhs     Right-hand Primitive operand.
 * \return  Returns true if both Primitive elements are equal; false otherwise.
 **/
[[nodiscard]]
inline bool operator == (const areg::Primitive & lsh, const areg::Primitive & rhs) noexcept;

/**
 * \brief   Returns true if two Primitive elements are not equal.
 *
 * \param   lsh     Left-hand Primitive operand.
 * \param   rhs     Right-hand Primitive operand.
 * \return  Returns true if Primitive elements differ; false otherwise.
 **/
[[nodiscard]]
inline bool operator != (const areg::Primitive & lsh, const areg::Primitive & rhs) noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::BufferHeader structure declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   areg::BufferHeader
 *          Structure of Binary Buffer object data header info.
 *          Used in all binary buffers. It stores basic information
 *          buffer, it's type, allocated and used sizes.
 **/
struct BufferHeader
{
    constexpr BufferHeader() = default;
    constexpr BufferHeader(const BufferHeader& src) = default;
    constexpr BufferHeader(uint32_t length, uint32_t offset, BufferType bufType, uint32_t used)
        : biLength{length}, biOffset{offset}, biBufType{bufType}, biUsed{used}
    {
    }

    constexpr BufferHeader(uint32_t length, uint32_t offset, BufferType bufType)
        : biLength{ length }, biOffset{ offset }, biBufType{ bufType }, biUsed{ 0u }
    {
    }

    constexpr BufferHeader& operator = (const BufferHeader& src) = default;

    /**
     * \brief   The length in bytes of totally allocated buffer.
     **/
    uint32_t    biLength    { 0 };
    /**
     * \brief   The size of buffer header. The buffer data should start after header size offset
     **/
    uint32_t    biOffset    { 0 };
    /**
     * \brief   The type of buffer. For RPC communication this should be external type.
     **/
    BufferType  biBufType   { areg::BufferType::Unknown };
    /**
     * \brief   Intrusive owner reference count of the heap block, managed by areg::RawBufferPtr.
     **/
    uint16_t    biRefCount  { 0 };
    /**
     * \brief   The length in bytes of used space in buffer. Cannot be more than biLength value.
     **/
    uint32_t    biUsed      { 0 };
};

static_assert(sizeof(areg::BufferHeader) == 16u, "BufferHeader must remain exactly 16 bytes (EventHeader layout depends on it)");

//////////////////////////////////////////////////////////////////////////
// areg::RawBuffer structure declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Specify the Byte Buffer object.
 *          Contains the size of complete object,
 *          buffer information and elements followed Byte Buffer object.
 **/
struct RawBuffer
{
    /**
     * \brief   Byte Buffer information
     **/
    BufferHeader    bufHeader   { };
    /**
     * \brief   Byte Buffer Data followed after structure.
     *          This is referring to the first element in the data buffer.
     **/
    BufferData      bufData[areg::BLOCK_SIZE]  { 0 };
};

//////////////////////////////////////////////////////////////////////////
// Intrusive reference-count atomics (BufferHeader::biRefCount)
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Atomically increments a 16-bit reference counter and returns the new value.
 *          Relaxed ordering: a new owner already has a happens-before relationship through
 *          the reference it copied from, so no additional synchronisation is required.
 **/
inline uint16_t atomic_inc16(uint16_t & ref) noexcept
{
#if defined(_MSC_VER)
    return static_cast<uint16_t>(_InterlockedIncrement16(reinterpret_cast<volatile short *>(&ref)));
#else   // !_MSC_VER
    return __atomic_add_fetch(&ref, 1, __ATOMIC_RELAXED);
#endif  // _MSC_VER
}

/**
 * \brief   Atomically decrements a 16-bit reference counter and returns the new value.
 *          Acquire-release ordering: guarantees that all writes by other owners are visible
 *          before the final owner (the one observing 0) frees the block.
 **/
inline uint16_t atomic_dec16(uint16_t & ref) noexcept
{
#if defined(_MSC_VER)
    return static_cast<uint16_t>(_InterlockedDecrement16(reinterpret_cast<volatile short *>(&ref)));
#else   // !_MSC_VER
    return __atomic_sub_fetch(&ref, 1, __ATOMIC_ACQ_REL);
#endif  // _MSC_VER
}

/**
 * \brief   Atomically loads a 16-bit reference counter (relaxed). Returns an approximate
 *          snapshot, matching std::shared_ptr::use_count() semantics.
 **/
inline uint16_t atomic_load16(const uint16_t & ref) noexcept
{
#if defined(_MSC_VER)
    return static_cast<uint16_t>(*reinterpret_cast<const volatile short *>(&ref));
#else   // !_MSC_VER
    return __atomic_load_n(&ref, __ATOMIC_RELAXED);
#endif  // _MSC_VER
}

//////////////////////////////////////////////////////////////////////////
// areg::RawBufferPtr class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   areg::RawBufferPtr
 *          Intrusive reference-counted owning pointer to a heap-allocated RawBuffer
 *          (or RawEnvelope -- both start with a BufferHeader at offset 0, so the count is
 *          always at the same location). The reference count lives inside the managed block
 *          (BufferHeader::biRefCount), so a fresh event costs exactly ONE heap allocation:
 *          the data block. This replaces the std::shared_ptr<RawBuffer> ownership, which
 *          needed a SECOND allocation for its separate control block.
 *
 *          Copy semantics mirror std::shared_ptr exactly: copy = atomic increment, destroy /
 *          reset = atomic decrement, free (delete[]) when the count reaches 0. Move transfers
 *          ownership without touching the count.
 *
 * \note    The block must have been allocated as `new uint8_t[]` and reinterpreted as
 *          RawBuffer* / RawEnvelope*; release frees it with the matching `delete[]`.
 *          ThreadSafe for the reference count only; the pointed-to buffer is not thread-safe.
 **/
class RawBufferPtr
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs an empty (null) owner.
     **/
    RawBufferPtr() noexcept = default;

    /**
     * \brief   Adopts a freshly-allocated raw block and initialises its reference count to 1.
     *          The block is not yet visible to any other thread, so the count is set with a
     *          plain store (no atomic needed).
     *
     * \param   adopt   Pointer to a `new uint8_t[]` block reinterpreted as RawBuffer*, or nullptr.
     **/
    explicit RawBufferPtr(RawBuffer * adopt) noexcept
        : mBuffer(adopt)
    {
        if (mBuffer != nullptr)
            mBuffer->bufHeader.biRefCount = 1u;
    }

    RawBufferPtr(const RawBufferPtr & src) noexcept
        : mBuffer(src.mBuffer)
    {
        if (mBuffer != nullptr)
            areg::atomic_inc16(mBuffer->bufHeader.biRefCount);
    }

    RawBufferPtr(RawBufferPtr && src) noexcept
        : mBuffer(src.mBuffer)
    {
        src.mBuffer = nullptr;
    }

    ~RawBufferPtr() noexcept
    {
        _release();
    }

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    RawBufferPtr & operator = (const RawBufferPtr & src) noexcept
    {
        if (mBuffer != src.mBuffer)
        {
            if (src.mBuffer != nullptr)
                areg::atomic_inc16(src.mBuffer->bufHeader.biRefCount);

            _release();
            mBuffer = src.mBuffer;
        }

        return (*this);
    }

    RawBufferPtr & operator = (RawBufferPtr && src) noexcept
    {
        if (this != &src)
        {
            _release();
            mBuffer     = src.mBuffer;
            src.mBuffer = nullptr;
        }

        return (*this);
    }

    [[nodiscard]]
    inline RawBuffer * operator -> () const noexcept
    {
        return mBuffer;
    }

    [[nodiscard]]
    explicit inline operator bool() const noexcept
    {
        return (mBuffer != nullptr);
    }

//////////////////////////////////////////////////////////////////////////
// Attributes / operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the managed raw pointer, or nullptr.
     **/
    [[nodiscard]]
    inline RawBuffer * get() const noexcept
    {
        return mBuffer;
    }

    /**
     * \brief   Releases this owner's reference (freeing the block if it was the last) and
     *          becomes null.
     **/
    inline void reset() noexcept
    {
        _release();
        mBuffer = nullptr;
    }

    /**
     * \brief   Returns an approximate snapshot of the owner count (relaxed atomic load),
     *          matching std::shared_ptr::use_count() semantics. Returns 0 when null.
     **/
    [[nodiscard]]
    inline uint32_t use_count() const noexcept
    {
        return (mBuffer != nullptr ? static_cast<uint32_t>(areg::atomic_load16(mBuffer->bufHeader.biRefCount)) : 0u);
    }

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    inline void _release() noexcept
    {
        if ((mBuffer != nullptr) && (areg::atomic_dec16(mBuffer->bufHeader.biRefCount) == 0u))
        {
            delete [] reinterpret_cast<uint8_t *>(mBuffer);
        }
    }

    RawBuffer * mBuffer { nullptr };
};

//////////////////////////////////////////////////////////////////////////
// areg::Endpoint structure declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   areg::Endpoint
 *          Numeric identity of one communication endpoint (consumer or provider).
 *          20 bytes, no padding, 4-byte aligned. Safe for memcpy on all platforms.
 *
 *  Field    Offset  Size  Description
 *  id            0     4  process identity cookie
 *  number        4     4  service instance identity: CRC32(service_name|type|role|thread)
 *  thread        8     4  thread routing key: CRC32(thread_name)
 *  major        12     2  service interface version: major
 *  minor        14     2  service interface version: minor
 *  patch        16     2  service interface version: patch
 *  type         18     2  ServiceType: Local=0x0040, Public=0x0080, Internet=0x0100
 *               20
 **/
struct Endpoint
{
    uint32_t    id      { 0 };  //!< process identity (COOKIE_UNKNOWN=0, COOKIE_LOCAL=1, or IPC cookie)
    uint32_t    number  { 0 };  //!< service instance identity: CRC32(service_name|type|role|thread)
    uint32_t    thread  { 0 };  //!< thread routing key: CRC32(thread_name)
    uint16_t    major   { 0 };  //!< service interface version: major
    uint16_t    minor   { 0 };  //!< service interface version: minor
    uint16_t    patch   { 0 };  //!< service interface version: patch
    uint16_t    type    { 0 };  //!< ServiceType value
};                              //!< 20 bytes, no padding

/**
 * \brief   areg::RawService
 *          Shared service interface identity, common to both consumer and provider endpoints.
 *          8 bytes, no padding, 4-byte aligned.
 *
 *  Field    Offset  Size  Description
 *  service       0     4  CRC32(service_name|service_type) = ServiceItem magic number
 *  role          4     4  CRC32(role_name)
 *                8
 **/
struct RawService
{
    uint32_t    service { 0 };  //!< service item hash: CRC32(service_name|service_type)
    uint32_t    role    { 0 };  //!< role name hash: CRC32(role_name)
};                              //!< 8 bytes, no padding

//////////////////////////////////////////////////////////////////////////
// areg::EventHeader structure declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   areg::EventHeader
 *          Flat binary header for all event types: local, IPC, and custom.
 *          128 bytes, no implicit padding, naturally aligned (8 bytes, from its uint64_t
 *          members). Layout is identical on x86, x86_64, arm32, aarch64, 32-bit and
 *          64-bit, and is pinned by the offset/size static_asserts below,
 *          so it stays safe for direct memcpy across the IPC wire.
 *
 *  Field        Offset  Size  Description
 *  bufHeader         0    16  BufferHeader: type, capacity, offset, used
 *  target           16     4  routing destination cookie (mtrouter reads at fixed offset 16)
 *  source           20     4  routing source cookie
 *  provider         24    20  Endpoint: stub side (always, regardless of direction)
 *  consumer         44    20  Endpoint: proxy side (always, regardless of direction)
 *  rawService       64     8  RawService: shared service interface identity
 *  channel          72     4  routing thread magic (0=local, CRC32(RC_thread)=IPC)
 *  messageId        76     4  service message or attribute ID
 *  sequenceNr       80     8  per-proxy sequence number
 *  result           88     4  ResultType / ServiceConnectionState / eRequestType
 *  eventType        92     2  EventType flags
 *  callType         94     1  EventCallType (0..5)
 *  priority         95     1  EventPriority
 *  checksum         96     4  CRC32 of payload; 0=skip validation
 *  classId         100     4  class ID assigned by the final event class
 *  internal1       104     8  LOCAL-ONLY: DispatcherThread*  — zeroed on IPC wire
 *  internal2       112     8  LOCAL-ONLY: EventConsumer*     — zeroed on IPC wire
 *  custom          120     8  LOCAL-ONLY: DATA_CLASS cleanup hook — zeroed on IPC wire.
 *                             For AREG_DECLARE_EVENT custom events: stores void(*)(void*)
 *                             noexcept pointing to the DATA_CLASS destructor, invoked by
 *                             MessageEnvelope::destroy_event() / Event::~Event() while this is
 *                             the sole owner (before the refcount reaches zero). MUST NOT be
 *                             used for any other purpose.
 *                  128
 **/
struct EventHeader
{
    areg::BufferHeader  bufHeader   {   };  //!< [0..15]    allocation metadata
    uint32_t            target      { 0 };  //!< [16..19]   routing destination cookie
    uint32_t            source      { 0 };  //!< [20..23]   routing source cookie
    areg::Endpoint      provider    {   };  //!< [24..43]   stub endpoint (always)
    areg::Endpoint      consumer    {   };  //!< [44..63]   proxy endpoint (always)
    areg::RawService    rawService  {   };  //!< [64..71]   service interface identity
    uint32_t            channel     { 0 };  //!< [72..75]   routing thread magic (0=local)
    uint32_t            messageId   { 0 };  //!< [76..79]   function or attribute ID
    SequenceNumber      sequenceNr  { 0 };  //!< [80..87]   per-proxy sequence number
    uint32_t            result      { 0 };  //!< [88..91]   ResultType / ServiceConnectionState
    uint16_t            eventType   { 0 };  //!< [92..93]   EventType flags
    uint8_t             callType    { 0 };  //!< [94]       Event actions, depends on EventType (0..5)
    uint8_t             priority    { 0 };  //!< [95]       EventPriority
    uint32_t            checksum    { 0 };  //!< [96..99]   CRC32 of payload; 0=skip
    uint32_t            eventId     { 0 };  //!< [100..103] class ID of the final event class
    uint64_t            internal1   { 0 };  //!< [104..111] LOCAL-ONLY: DispatcherThread* — zeroed on IPC wire
    uint64_t            internal2   { 0 };  //!< [112..119] LOCAL-ONLY: EventConsumer*    — zeroed on IPC wire
    uint64_t            custom      { 0 };  //!< [120..127] LOCAL-ONLY: DATA_CLASS cleanup hook — zeroed on IPC wire; see struct docs
};                                          //!< 128 bytes

//////////////////////////////////////////////////////////////////////////
// areg::RawEnvelope structure declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   areg::RawEnvelope
 *          Heap layout type for MessageEnvelope.
 *          envData[8] marks the first bytes of the payload area;
 *          the actual payload continues in the heap allocation past this struct.
 **/
struct RawEnvelope
{
    RawEnvelope() = default;
    constexpr RawEnvelope(const RawEnvelope& src) = default;
    constexpr RawEnvelope& operator = (const RawEnvelope& src) = default;

    areg::EventHeader   envHeader { };      //!< event header at start of allocation
    areg::BufferData    envData[16];
};

static_assert(sizeof(areg::Endpoint)    == 20               , "Endpoint must be exactly 20 bytes");
static_assert(sizeof(areg::RawService)  ==  8               , "RawService must be exactly 8 bytes");
static_assert(sizeof(areg::EventHeader) == 128              , "EventHeader must be exactly 128 bytes");
static_assert(sizeof(areg::RawEnvelope) == 128 + 16         , "RawEnvelope must be exactly 136 bytes");
static_assert(offsetof(areg::EventHeader, target)    == 16  , "EventHeader.target must be at offset 16");
static_assert(offsetof(areg::EventHeader, provider)  == 24  , "EventHeader.provider must be at offset 24");
static_assert(offsetof(areg::EventHeader, consumer)  == 44  , "EventHeader.consumer must be at offset 44");
static_assert(offsetof(areg::EventHeader, rawService)== 64  , "EventHeader.rawService must be at offset 64");
static_assert(offsetof(areg::EventHeader, channel)   == 72  , "EventHeader.channel must be at offset 72");

// Portability guard: BufferHeader, RawBuffer, EventHeader and RawEnvelope are all
// placement-constructed into plain `new uint8_t[]` heap blocks.
static_assert(alignof(areg::BufferHeader) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__, "BufferHeader is over-aligned vs the new uint8_t[] block it is constructed into (would fault on 32-bit)");
static_assert(alignof(areg::RawBuffer)    <= __STDCPP_DEFAULT_NEW_ALIGNMENT__, "RawBuffer is over-aligned vs the new uint8_t[] block it is constructed into (would fault on 32-bit)");
static_assert(alignof(areg::EventHeader)  <= __STDCPP_DEFAULT_NEW_ALIGNMENT__, "EventHeader is over-aligned vs the new uint8_t[] block it is constructed into (would fault on 32-bit)");
static_assert(alignof(areg::RawEnvelope)  <= __STDCPP_DEFAULT_NEW_ALIGNMENT__, "RawEnvelope is over-aligned vs the new uint8_t[] block it is constructed into (would fault on 32-bit)");

/**
 * \brief   Returns writable pointer to data buffer, or nullptr if buffer pointer is invalid.
 *
 * \param   byteBuffer      Byte-buffer object pointer.
 * \return  Writable data buffer pointer, or nullptr if invalid.
 **/
[[nodiscard]]
inline uint8_t * buffer_data_write( RawBuffer * byteBuffer ) noexcept;

/**
 * \brief   Returns read-only pointer to data buffer, or nullptr if buffer pointer is invalid.
 *
 * \param   byteBuffer      Byte-buffer object pointer.
 * \return  Read-only data buffer pointer, or nullptr if invalid.
 **/
[[nodiscard]]
inline const uint8_t * buffer_data_read( const RawBuffer * byteBuffer ) noexcept;

/**
 * \brief   Constructs elements in allocated buffer by calling the default constructor for each element.
 *
 * \param   begin           Pointer to heap-allocated buffer.
 * \param   elemCount       Number of elements to construct. If zero, no elements are constructed.
 * \return  Pointer to first constructed element in array.
 * \note    ELEM_TYPE must have a public default constructor.
 **/
template <typename ELEM_TYPE>
inline ELEM_TYPE * construct_elems(void *begin, uint32_t elemCount);

/**
 * \brief   Constructs elements in allocated buffer by calling the constructor with an argument
 *          for each element.
 *
 * \param   begin           Pointer to heap-allocated buffer.
 * \param   elemCount       Number of elements to construct. If zero, no elements are initialized.
 * \param   arg             Argument value to pass to constructor.
 * \return  Pointer to first constructed element in array.
 * \note    ELEM_TYPE must have a public constructor accepting ARGUMENT_TYPE.
 **/
template <typename ELEM_TYPE, typename ARGUMENT_TYPE>
[[nodiscard]]
inline ELEM_TYPE * construct_with_arg(void *begin, uint32_t elemCount, ARGUMENT_TYPE arg);

/**
 * \brief   Constructs a single element of type T at the given memory location,
 *          forwarding the provided arguments to T's constructor.
 *          Use this overload to initialize the object immediately with values
 *          or via the copy / move constructor.
 *
 * \tparam  T       The type to construct.
 * \tparam  Args    Constructor argument types.
 * \param   ptr     Pointer to raw memory where the object is constructed.
 * \param   args    Arguments forwarded to T's constructor.
 * \return  Pointer to the constructed T object.
 **/
template<typename T, typename... Args>
[[nodiscard]]
inline T* construct_elem(void* ptr, Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)));

/**
 * \brief   Destroys previously constructed elements in heap by calling the destructor for each.
 *
 * \param   begin           Pointer to buffer with allocated elements.
 * \param   elemCount       Number of elements to destroy.
 * \note    ELEM_TYPE must have a public destructor.
 **/
template <typename ELEM_TYPE>
inline void destroy_elems(ELEM_TYPE *begin, uint32_t elemCount);

/**
 * \brief   Copies elements from source to destination buffer using assignment operator.
 *
 * \param[out] destination  Pre-allocated destination buffer with at least elemCount elements capacity.
 * \param   source          The source buffer containing elements to copy.
 * \param   elemCount       Number of elements to copy. Both buffers must have at least elemCount elements.
 * \note    If ELEM_DST and ELEM_SRC differ: ELEM_SRC must be convertible to ELEM_DST (via
 *          static_cast), and ELEM_DST must support operator=.
 **/
template <typename ELEM_DST, typename ELEM_SRC = ELEM_DST>
inline void copy_elems(ELEM_DST *destination, const ELEM_SRC *source, uint32_t elemCount);

/**
 * \brief   Moves elements from source to destination within the same allocated memory block,
 *          handling overlapping regions.
 *
 * \param[out] destination  The destination pointer within same memory block.
 * \param   source          The source pointer within same memory block.
 * \param   elemCount       Number of elements to move.
 * \note    Source and destination must refer to the same allocated memory chunk. Do not use if
 *          buffers are in different memory spaces. ELEM_TYPE must support operator=.
 **/
template <typename ELEM_TYPE>
void move_elems(ELEM_TYPE * destination, const ELEM_TYPE * source, uint32_t elemCount);

/**
 * \brief   Sets every element in buffer to the specified value using assignment operator.
 *
 * \param   begin           Pointer to allocated element buffer.
 * \param   elemValue       Value to assign to every element.
 * \param   elemCount       Number of elements in buffer.
 * \note    ELEM must support operator= that accepts ELEM_TYPE.
 **/
template <typename ELEM, typename ELEM_TYPE = ELEM>
inline void set_memory(ELEM * begin, ELEM_TYPE elemValue, uint32_t elemCount);

/**
 * \brief   Compares two buffers of same element type, returning true if all elements match.
 *
 * \param   lhs         Left-hand buffer of elements.
 * \param   rhs         Right-hand buffer of elements.
 * \param   count       Number of elements to compare.
 * \return  Returns true if all elements in both buffers are equal; false otherwise.
 * \note    ELEM must support operator== for element-by-element comparison. For
 *          structures/classes without operator==, use areg::mem_equal instead.
 **/
template <typename ELEM>
[[nodiscard]]
bool equal_elements(const ELEM * lhs, const ELEM * rhs, uint32_t count);

/**
 * \brief   Compares two buffers of different element types, returning true if all elements
 *          match after conversion.
 *
 * \param   lhs         Left-hand buffer.
 * \param   rhs         Right-hand buffer.
 * \param   count       Number of elements to compare.
 * \return  Returns true if all elements are equal after conversion; false otherwise.
 * \note    ELEM_RIGHT must be convertible to ELEM_LEFT (via static_cast), and ELEM_LEFT must
 *          support operator==.
 **/
template <typename ELEM_LEFT, typename ELEM_RIGHT = ELEM_LEFT>
[[nodiscard]]
bool equal_elements(const ELEM_LEFT * lhs, const ELEM_RIGHT * rhs, uint32_t count);

/**
 * \brief   Sets all bytes of a single element to zero, typically used for structures.
 *
 * \param   elem    Element to zero.
 **/
template<typename ELEM>
inline void zero_element( ELEM & elem );

/**
 * \brief   Sets all elements in a list to zero using byte-wise clearing.
 *
 * \param   elemList        List of elements to zero.
 * \param   elemCount       Number of elements in list.
 **/
template<typename ELEM>
inline void zero_elements( ELEM * elemList, uint32_t elemCount );

/**
 * \brief   Fills buffer with the specified byte value.
 *
 * \param   buffer      Buffer to fill.
 * \param   length      Buffer length in bytes.
 * \param   symbol      Byte value to fill buffer with.
 **/
inline void mem_set( void * buffer, uint32_t length, uint8_t symbol ) noexcept;

/**
 * \brief   Sets all bytes in buffer to zero.
 *
 * \param   buffer      Buffer to clear.
 * \param   length      Buffer length in bytes.
 **/
inline void mem_zero( void * buffer, uint32_t length ) noexcept;

/**
 * \brief   Moves bytes from source to destination within the same allocated memory, handling
 *          overlapping regions.
 *
 * \param[out] memDst   Destination pointer within same memory block.
 * \param   memSrc      Source pointer within same memory block.
 * \param   count       Number of bytes to move.
 **/
inline void mem_move(void * memDst, const void * memSrc, uint32_t count) noexcept;

/**
 * \brief   Copies data from source to destination, respecting destination buffer capacity.
 *
 * \param[out] memDst   Destination buffer with minimum size dstSpace.
 * \param   dstSpace    Available space in destination buffer in bytes.
 * \param   memSrc      Source buffer containing data to copy.
 * \param   count       Desired number of bytes to copy.
 * \return  Number of bytes actually copied (limited by dstSpace if it is smaller than count).
 **/
inline uint32_t mem_copy( void * memDst, uint32_t dstSpace, const void * memSrc, uint32_t count) noexcept;

/**
 * \brief   Compares two memory blocks byte-by-byte, returning areg::Ordering result.
 *
 * \param   memLeft     Left-hand memory buffer to compare.
 * \param   memRight    Right-hand memory buffer to compare.
 * \param   count       Number of bytes to compare.
 * \return  areg::Smaller if memLeft < memRight, Equal if identical, Bigger if memLeft > memRight.
 **/
[[nodiscard]]
inline areg::Ordering mem_compare( const void * memLeft, const void * memRight, uint32_t count) noexcept;

/**
 * \brief   Returns true if two memory blocks contain identical data.
 *
 * \param   memLeft     Left-hand memory buffer.
 * \param   memRight    Right-hand memory buffer.
 * \param   count       Number of bytes to compare.
 * \return  Returns true if both memory chunks are identical; false otherwise.
 **/
[[nodiscard]]
inline bool mem_equal( const void * memLeft, const void * memRight, uint32_t count) noexcept;

/**
 * \brief   Functor for custom buffer allocation.
 **/
template<typename BufType>
struct BufferAllocator
{
    /**
     * \brief   Allocates a buffer of the specified size.
     **/
    BufType* operator ( ) (uint32_t space);
};

/**
 * \brief   Functor for custom buffer deallocation.
 **/
template<typename BufType>
struct BufferDeleter
{
    /**
     * \brief   De-allocates a buffer.
     **/
    void operator ( ) (void * buffer);
};

template<typename Type>
struct required_size
{
    [[nodiscard]]
    inline constexpr uint32_t operator()() const noexcept
    {
        return sizeof(Type);
    }

    [[nodiscard]]
    inline constexpr operator uint32_t() const noexcept
    {
        return sizeof(Type);
    }
};

/************************************************************************
 * \brief   Streaming of areg::Primitive
 *          Global streaming operators to read and write to streaming object
 ************************************************************************/

/**
 * \brief   Support streaming operator for areg::Primitive type.
 *          Read areg::Primitive from streaming object
 * \param   stream  The streaming object to read
 * \param   input   The areg::Primitive item to initialize from stream
 * \return  Reading streaming object
 **/
inline const areg::InStream & operator >> (const areg::InStream & stream, areg::Primitive & input)
{
    stream.read( reinterpret_cast<uint8_t *>(&input), sizeof(areg::Primitive) );
    return stream;
}

/**
 * \brief   Support streaming operator for areg::Primitive type.
 *          Write areg::Primitive to streaming object
 * \param   stream  The streaming object to write
 * \param   output  The areg::Primitive item to write to stream
 * \return  Writing streaming object
 **/
inline areg::OutStream & operator << (areg::OutStream & stream, const areg::Primitive & output)
{
    stream.write( reinterpret_cast<const uint8_t *>(&output), sizeof(areg::Primitive) );
    return stream;
}

/************************************************************************
 * \brief   Streaming of areg::Primitive
 *          Global comparing operators
 ************************************************************************/
 
/**
 * \brief   compares to areg::Primitive values and returns true if they are equal.
 **/
inline bool operator == ( const areg::Primitive& lsh, const areg::Primitive& rhs ) noexcept
{
    return ((&lsh == &rhs) || (lsh.valInt64.mElement == rhs.valInt64.mElement));
}

/**
 * \brief   compares to areg::Primitive values and returns true if they are not equal.
 **/
inline bool operator != ( const areg::Primitive& lsh, const areg::Primitive& rhs ) noexcept
{
    return ((&lsh != &rhs) && (lsh.valInt64.mElement != rhs.valInt64.mElement));
}

//////////////////////////////////////////////////////////////////////////
// areg namespace function implementation.
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Comparing operators
/************************************************************************/

inline void mem_set( void * buffer, uint32_t length, uint8_t symbol ) noexcept
{
    if ( (buffer != nullptr) && (length > 0) )
    {
        ::memset(buffer, symbol, length);
    }
}

inline void mem_zero( void * buffer, uint32_t length ) noexcept
{
    areg::mem_set( buffer, length, 0x00u );
}

inline void mem_move( void * memDst, const void * memSrc, uint32_t count ) noexcept
{
    if ( (memDst != nullptr) && (memSrc != nullptr) && (count > 0) )
    {
        ::memmove( memDst, memSrc, count );
    }
}

inline uint32_t mem_copy( void * memDst, uint32_t dstSpace, const void * memSrc, uint32_t count ) noexcept
{
    uint32_t result = 0u;
    if (memDst != memSrc)
    {
        if ((memDst != nullptr) && (memSrc != nullptr) && (count > 0) && (dstSpace > 0))
        {
            result = std::min(dstSpace, count);
            ::memcpy(memDst, memSrc, result);
        }
    }
    else
    {
        result = count;
    }

    return result;
}

inline areg::Ordering mem_compare( const void * memLeft, const void * memRight, uint32_t count ) noexcept
{
    areg::Ordering result = areg::Ordering::Equal;

    if ( (count == 0) || (memLeft == memRight) )
    {
        result = areg::Ordering::Equal;
    }
    else if ( (memLeft != nullptr) && (memRight != nullptr) )
    {
        int32_t cmp = ::memcmp(memLeft, memRight, count);
        result = (cmp > 0 ? areg::Ordering::Bigger : (cmp < 0 ?  areg::Ordering::Smaller : areg::Ordering::Equal));
    }
    else if ( memLeft != nullptr )
    {
        result = areg::Ordering::Bigger;
    }
    else
    {
        result = areg::Ordering::Smaller;
    }

    return result;
}

inline bool mem_equal( const void * memLeft, const void * memRight, uint32_t count ) noexcept
{
    return (areg::Ordering::Equal == mem_compare(memLeft, memRight, count));
}


/************************************************************************/
// Byte buffer functions
/************************************************************************/

inline uint8_t * buffer_data_write(areg::RawBuffer * byteBuffer) noexcept
{
    return (byteBuffer != nullptr ? reinterpret_cast<uint8_t *>(byteBuffer) + byteBuffer->bufHeader.biOffset : nullptr);
}

inline const uint8_t * buffer_data_read(const RawBuffer * byteBuffer) noexcept
{
    return (byteBuffer != nullptr ? reinterpret_cast<const uint8_t *>(byteBuffer) + byteBuffer->bufHeader.biOffset : nullptr);
}

/************************************************************************/
// Function templates
/************************************************************************/

template <typename ELEM_TYPE>
inline ELEM_TYPE * construct_elems(void *begin, uint32_t elemCount)
{
    if ( begin != nullptr )
    {
        ELEM_TYPE* elems    = reinterpret_cast<ELEM_TYPE *>(begin);

#if _MSC_VER
    #pragma warning(disable: 4345)
#endif  // _MSC_VER
        for (uint32_t i = 0; i < elemCount; ++ i, ++ elems)
        {
            ::new(elems) ELEM_TYPE;
        }
#if _MSC_VER
        #pragma warning(default: 4345)
#endif  // _MSC_VER
    }

    return reinterpret_cast<ELEM_TYPE *>(begin);
}

template <typename ELEM_TYPE, typename ARGUMENT_TYPE>
inline ELEM_TYPE * construct_with_arg(void *begin, uint32_t elemCount, ARGUMENT_TYPE arg)
{
    if ( begin != nullptr )
    {
        ELEM_TYPE* elems    = reinterpret_cast<ELEM_TYPE *>(begin);

#if _MSC_VER
    #pragma warning(disable: 4345)
#endif  // _MSC_VER
        for ( uint32_t i = 0; i < elemCount; ++ i, ++ elems )
        {
            ::new (elems) ELEM_TYPE(arg);
        }
#if _MSC_VER
        #pragma warning(default: 4345)
#endif  // _MSC_VER
    }

    return reinterpret_cast<ELEM_TYPE *>(begin);
}

template<typename T, typename... Args>
inline T* construct_elem(void* ptr, Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
{
    return ::new(ptr) T(std::forward<Args>(args)...);
}

template <typename ELEM_TYPE>
inline void destroy_elems(ELEM_TYPE *begin, uint32_t elemCount)
{
    if ( begin != nullptr )
    {
        ELEM_TYPE* elems    = static_cast<ELEM_TYPE *>(begin);
        for (uint32_t i = 0; i < elemCount; ++ i, ++ elems)
        {
            elems->~ELEM_TYPE( );
        }
    }
}

template <typename ELEM_DST, typename ELEM_SRC /*= ELEM_DST*/>
inline void copy_elems(ELEM_DST *destination, const ELEM_SRC *source, uint32_t elemCount)
{
    if ((destination != source) && (destination != nullptr) && (source != nullptr) )
    {
        for ( ; elemCount > 0; -- elemCount, ++ destination, ++ source )
        {
            *destination = static_cast<const ELEM_DST &>(*source);
        }
    }
}

template <typename ELEM_TYPE>
void move_elems(ELEM_TYPE *destination, const ELEM_TYPE *source, uint32_t elemCount)
{
    if (destination < source)
    {
        while (elemCount -- > 0)
            *destination ++ = *source ++;
    }
    else if (destination > source)
    {
        // avoid memory overlapping, copy elements from back to front
        destination += elemCount;
        source      += elemCount;
        while (elemCount -- > 0)
        {
            * --destination = * --source;
        }
    }
}

template <typename ELEM, typename ELEM_TYPE /*= ELEM*/>
inline void set_memory(ELEM* begin, ELEM_TYPE elemValue, uint32_t elemCount)
{
    if (begin != nullptr )
    {
        while (elemCount -- > 0)
        {
            *begin ++ = elemValue;
        }
    }
}

template <typename ELEM>
inline bool equal_elements(const ELEM * lhs, const ELEM * rhs, uint32_t count)
{
    return equal_elements<ELEM, ELEM>(lhs, rhs, count);
}

template <typename ELEM_LEFT, typename ELEM_RIGHT /*= ELEM_LEFT*/>
inline bool equal_elements(const ELEM_LEFT* lhs, const ELEM_RIGHT* rhs, uint32_t count)
{
    if ((count == 0) || (lhs == rhs))
        return true;
    else if ((lhs == nullptr) || (rhs == nullptr))
        return false;

    bool result{ true };
    for ( uint32_t i = 0; i < count; ++ i)
    {
        if (lhs[i] != static_cast<const ELEM_LEFT &>(rhs[i]))
        {
            result = false;
            break;
        }


    }

    return result;
}

template<typename ELEM>
inline void zero_element( ELEM & elem )
{
    areg::zero_elements<ELEM>(&elem, 1);
}

template<typename ELEM>
inline void zero_elements( ELEM * elemList, uint32_t elemCount )
{
    if ( elemCount > 0 )
    {
        constexpr uint32_t one = static_cast<uint32_t>(sizeof(ELEM));
        areg::mem_zero( reinterpret_cast<void *>(elemList), elemCount * one );
    }
}

template<typename BufType>
BufType* areg::BufferAllocator<BufType>::operator ( ) (uint32_t space)
{
    uint8_t* result = DEBUG_NEW uint8_t[space];
    return ::new(result) BufType;
}

template<typename BufType>
void areg::BufferDeleter<BufType>::operator ( ) (void * buffer)
{
    if ( buffer != nullptr )
    {
        delete [] reinterpret_cast<uint8_t *>(buffer);
    }
}

inline constexpr const char * as_string( areg::MessageResult msgResult ) noexcept
{
    switch ( msgResult )
    {
    case areg::MessageResult::UnknownError:
        return "areg::UnknownError";
    case areg::MessageResult::Succeed:
        return "areg::Succeed";
    case areg::MessageResult::NoConnection:
        return "areg::NoConnection";
    case areg::MessageResult::ProviderUnavailable:
        return "areg::ProviderUnavailable";
    case areg::MessageResult::ConsumerRejected:
        return "areg::ConsumerRejected";
    case areg::MessageResult::TargetUnavailable:
        return "areg::TargetUnavailable";
    default:
        return "ERR: Invalid areg::MessageResult value!!!";
    }
}

inline constexpr const char * as_string(areg::BufferType val ) noexcept
{
    switch (val)
    {
    case areg::BufferType::Unknown:
        return "areg::Unknown";
    case areg::BufferType::Internal:
        return "areg::Internal";
    case areg::BufferType::Remote:
        return "areg::Remote";
    default:
        return "ERR: Invalid areg::BufferType value!!!";
    }
}

} // namespace areg

#endif  // AREG_BASE_MEMORYDEFS_HPP
