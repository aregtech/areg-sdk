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
        , ProviderUnavailable            //!< Error, service is unavailable
        , ConsumerRejected               //!< Error, service is rejected
        , TargetUnavailable             //!< Error, the target object is unavailable
    };

    /**
     * \brief   Converts areg::MessageResult enumeration to string representation for logging
     *          and output.
     *
     * \param   msgResult       MessageResult value to convert.
     **/
    [[nodiscard]]
    inline constexpr const char * as_string( areg::MessageResult msgResult ) noexcept;

    /**
     * \brief   Types of data buffer
     *          areg::BufferType
     **/
    enum class BufferType : int32_t
    {
          Unknown   = -1    //!< Unknown buffer type, not used
        , Internal  =  0    //!< Buffer type for internal communication
        , Remote    =  2    //!< Buffer type for remote communication
    };
    /**
     * \brief   Returns string representation of areg::BufferType enumeration value.
     *
     * \param   val     BufferType value to convert.
     **/
    [[nodiscard]]
    inline constexpr const char * as_string( areg::BufferType val ) noexcept;

    /**
     * \brief   areg::BLOCK_SIZE
     *          Constant. Defines the minimum size of Byte Buffer data
     *          Also defines the size to align buffer allocation.
     **/
    constexpr uint32_t      BLOCK_SIZE      { sizeof( uint64_t ) * 8 };
    /**
     * \brief   areg::INVALID_SIZE
     *          Constant. Defines invalid buffer size.
     **/
    constexpr uint32_t      INVALID_SIZE    { ~0u };

    /**
     * \brief   areg::INVALID_VALUE
     *          Constant. Defines invalid value for remote buffer.
     **/
    constexpr uint32_t      INVALID_VALUE   { ~0u };
    /**
     * \brief   areg::MESSAGE_SUCCESS
     *          Constants. Defines the message result success.
     **/
    constexpr uint32_t      MESSAGE_SUCCESS { static_cast<uint32_t>(areg::MessageResult::Succeed) };

    /**
     * \brief   areg::InvalidElement
     *          Predefined Invalid Element value. Used to defined invalid element.
     *          For example, it is used in thread local storage object (ThreadLocalStorage)
     *          in case if there is no element found in local storage by given name.
     *
     * \see     ThreadLocalStorage::GetStorageItem()
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
        BufferType  biBufType   { BufferType::Unknown };
        /**
         * \brief   The length in bytes of used space in buffer.
         *          Cannot be more than biLength value.
         **/
        uint32_t    biUsed      { 0 };
    };

    //////////////////////////////////////////////////////////////////////////
    // areg::sRemoteBuferHeader structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::MessageHeader
     *          Structure of binary buffer for Remote data transfer.
     *          It is extended type of BufferHeader with additions
     *          of message ID, message sequence number, cookie and checksum.
     **/
    struct MessageHeader
    {
        constexpr MessageHeader(uint32_t length, uint32_t offset, BufferType bufType, uint32_t used, ITEM_ID target, uint32_t checksum, ITEM_ID source, uint32_t  msgId, uint32_t result, SequenceNumber seq)
            : rbhBufHeader{length, offset, bufType, used}, rbhTarget{target}, rbhChecksum{checksum}, rbhSource{source}, rbhMessageId{msgId}, rbhResult{result}, rbhSequenceNr{seq}
        {
        }

        constexpr MessageHeader(uint32_t length, uint32_t offset, BufferType bufType, ITEM_ID target, ITEM_ID source, uint32_t  msgId)
            : rbhBufHeader{ length, offset, bufType}, rbhTarget{ target }, rbhChecksum{ areg::INVALID_VALUE }, rbhSource{ source }, rbhMessageId{ msgId }, rbhResult{ 0u }, rbhSequenceNr{ 0u }
        {
        }

        constexpr MessageHeader() = default;
        constexpr MessageHeader(const MessageHeader& src) = default;
        constexpr MessageHeader& operator = (const MessageHeader& src) = default;

        /**
         * \brief   The common buffer header information
         **/
        BufferHeader    rbhBufHeader{ };
        /**
         * \brief   An ID of target object, receiving message.
         *          In remote messaging, this is Cookie of target
         **/
        ITEM_ID         rbhTarget   { 0 };
        /**
         * \brief   Data checksum value for validation check-up.
         *          Should be ignored if value is areg::IGNORE_CHECKSUM
         **/
        uint32_t        rbhChecksum{ INVALID_VALUE };
        /**
         * \brief   An ID of source object, sending message.
         *          In remote messaging, this is Cookie of source
         **/
        ITEM_ID         rbhSource{ 0 };
        /**
         * \brief   The Remote message ID registered in the system
         **/
        uint32_t        rbhMessageId{ 0 };
        /**
         * \brief   The result of processing message.
         **/
        uint32_t        rbhResult{ 0 };
        /**
         * \brief   The Remote message sequence number set during messaging
         **/
        SequenceNumber  rbhSequenceNr{ 0 };
    };

    //////////////////////////////////////////////////////////////////////////
    // areg::RawBuffer structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::RawBuffer
     *          Specify the Byte Buffer object.
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
        BufferData      bufData[4]  { 0 };
    };


    //////////////////////////////////////////////////////////////////////////
    // areg::sRpcMessageBuffer structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::RawMessage
     *          Specify the Byte Buffer object.
     *          Contains the size of complete object,
     *          buffer information and elements followed Byte Buffer object.
     **/
    struct RawMessage
    {
        constexpr RawMessage() = default;
        constexpr RawMessage(const RawMessage& src) = default;
        constexpr RawMessage(uint32_t length, uint32_t offset, BufferType bufType, uint32_t used, ITEM_ID target, uint32_t checksum, ITEM_ID source, uint32_t  msgId, uint32_t result, SequenceNumber seq)
            : rbHeader{ length, offset, bufType, used, target, checksum, source, msgId, result, seq }, rbData{0}
        {
        }

        constexpr RawMessage(uint32_t length, uint32_t offset, BufferType bufType, ITEM_ID target, ITEM_ID source, uint32_t  msgId)
            : rbHeader{ length, offset, bufType, target, source, msgId}, rbData{ 0 }
        {
        }

        constexpr RawMessage& operator = (const RawMessage& src) = default;

        /**
         * \brief   Byte Buffer information
         **/
        MessageHeader    rbHeader{ };
        /**
         * \brief   Byte Buffer Data followed after structure.
         *          This is referring to the first element in the data buffer.
         **/
        BufferData              rbData[4]{ 0 };
    };

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
     * \brief   Constructs elements in allocated buffer by calling the default constructor for each
     *          element.
     *
     * \param   begin           Pointer to heap-allocated buffer.
     * \param   elemCount       Number of elements to construct. If zero, no elements are
     *                          constructed.
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
     * \param   elemCount       Number of elements to construct. If zero, no elements are
     *                          initialized.
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
     * \param[out] destination     Pre-allocated destination buffer with at least elemCount elements
     *                             capacity.
     * \param   source          Source buffer containing elements to copy.
     * \param   elemCount       Number of elements to copy. Both buffers must have at least
     *                          elemCount elements.
     * \note    If ELEM_DST and ELEM_SRC differ: ELEM_SRC must be convertible to ELEM_DST (via
     *          static_cast), and ELEM_DST must support operator=.
     **/
    template <typename ELEM_DST, typename ELEM_SRC = ELEM_DST>
    inline void copy_elems(ELEM_DST *destination, const ELEM_SRC *source, uint32_t elemCount);

    /**
     * \brief   Moves elements from source to destination within the same allocated memory block,
     *          handling overlapping regions.
     *
     * \param[out] destination     Destination pointer within same memory block.
     * \param   source          Source pointer within same memory block.
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
     * \param[out] memDst      Destination pointer within same memory block.
     * \param   memSrc      Source pointer within same memory block.
     * \param   count       Number of bytes to move.
     **/
    inline void mem_move(void * memDst, const void * memSrc, uint32_t count) noexcept;

    /**
     * \brief   Copies data from source to destination, respecting destination buffer capacity.
     *
     * \param[out] memDst      Destination buffer with minimum size dstSpace.
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
     * \return  areg::Smaller if memLeft < memRight, Equal if identical, Bigger if memLeft >
     *          memRight.
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
         * \brief   Deallocates a buffer.
         **/
        void operator ( ) (void * buffer);
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
    uint32_t result = 0;
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
