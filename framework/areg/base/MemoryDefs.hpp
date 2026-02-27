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
#include "areg/base/GEGlobal.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/MathDefs.hpp"

#include <algorithm>
#include <new>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
// NEMemory namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   NEMemory specifies basic functions and types for objects located in memory
 *          In this namespace are defined such types like Byte Buffer structure and 
 *          Align union. As well as some utility functions requiring working with 
 *          buffers of array of elements.
 *          For more details see types and functions description.
 *
 **/
namespace areg
{
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
     * \brief   Converts the values of type areg::MessageResult to string, used in logs and output messages.
     **/
    inline const char * getString( MessageResult msgResult );

    /**
     * \brief   Types of data buffer
     *          areg::BufferType
     **/
    enum class BufferType : int8_t
    {
          Unknown   = -1    //!< Unknown buffer type, not used
        , Internal  =  0    //!< Buffer type for internal communication
        , Remote    =  2    //!< Buffer type for remote communication
    };
    /**
     * \brief   Returns string value of areg::BufferType
     **/
    inline const char * getString( BufferType val );

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
    constexpr uint32_t      MESSAGE_SUCCESS { static_cast<uint32_t>(MessageResult::Succeed) };

    /**
     * \brief   areg::InvalidElement
     *          Predefined Invalid Element value. Used to defined invalid element.
     *          For example, it is used in thread local storage object (ThreadLocalStorage)
     *          in case if there is no element found in local storage by given name.
     *
     * \see     ThreadLocalStorage::GetStorageItem()
     **/
    constexpr  Primitive  InvalidElement{{0}};

    /**
     * \brief   Compares 2 areg::Primitive elements, returns true if they are equal
     * \param   lsh     Left-Hand Operand
     * \param   rhs     Right-Hand Operand
     * \return  Returns true if 2 areg::Primitive elements are equal
     **/
    inline bool operator == (const Primitive & lsh, const Primitive & rhs);

    /**
     * \brief   Compares 2 areg::Primitive elements, returns true if they are not equal
     * \param   lsh     Left-Hand Operand
     * \param   rhs     Right-Hand Operand
     * \return  Returns true if 2 areg::Primitive elements are not equal
     **/
    inline bool operator != (const Primitive & lsh, const Primitive & rhs);

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
        /**
         * \brief   The actual size of complete binary Buffer object,
         *          including header and allocated sizes
         **/
        uint32_t    biBufSize   { 0 };
        /**
         * \brief   The length in bytes of totally allocated buffer.
         **/
        uint32_t    biLength   { 0 };
        /**
         * \brief   The size of buffer header. The buffer data should start after header size offset
         **/
        uint32_t    biOffset   { 0 };
        /**
         * \brief   The type of buffer. For RPC communication this should be external type.
         **/
        BufferType     biBufType   { BufferType::Unknown };
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
        uint32_t        rbhChecksum{ 0 };
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
     * \brief   Returns the pointer to data buffer for writing. 
     *          Returns nullptr if pointer to byte buffer is invalid.
     * \param   byteBuffer  The pointer to byte-buffer object.
     * \return  Returns data buffer to write.
     **/
    inline uint8_t * getBufferDataWrite( RawBuffer * byteBuffer );

    /**
     * \brief   Returns the pointer to data buffer for reading. 
     *          Returns nullptr if pointer to byte buffer is invalid.
     * \param   byteBuffer  The pointer to byte-buffer object.
     * \return  Returns data buffer to read.
     **/
    inline const uint8_t * getBufferDataRead( const RawBuffer * byteBuffer );

    /**
     * \brief	Constructs elements in allocated buffer, i.e. calls default constructor to initialize element
     *          The type of element should be specified.
     * \param	begin	    Pointer to the buffer allocated in heap. 
     * \param	elemCount	Amount of elements to construct. If zero, no element will be constructed.
     * \return  Returns pointer to first constructed element in array.
     * \tparam  ELEM_TYPE   The name of object to construct. Can be primitives of objects. 
     *                      If objects (class or structure), it should have default constructor.
     * \note    The type ELEM_TYPE must have public available default constructor.
     **/
    template <typename ELEM_TYPE>
    inline ELEM_TYPE * constructElems(void *begin, uint32_t elemCount);

    /**
     * \brief	Constructs elements with argument in allocated buffer,  i.e. calls constructor with argument to initialize elements
     *          The type of element should be specified.
     * \param	begin	    Pointer to the buffer allocated in heap.
     * \param	elemCount	Amount of elements to construct. If zero, no element will be initialize.
     * \param	arg	        Argument value to pass during initialization.
     * \return  Returns pointer to first constructed element in array.
     * \tparam  ELEM_TYPE   The name of object to construct. Can be primitives of objects.
     *                      If objects (class or structure), it should have appropriate constructor with constructors.
     * \note    The type ELEM_TYPE must have public available appropriate constructor with arguments.
     **/
    template <typename ELEM_TYPE, typename ARGUMENT_TYPE>
    inline ELEM_TYPE * constructElemsWithArgument(void *begin, uint32_t elemCount, ARGUMENT_TYPE arg);

    /**
     * \brief	Destroys elements previously constructed / initialized in heap,
     *          i.e. calls destructor of every element.
     *          The type of element should be specified.
     * \param	begin	    Pointer to buffer with allocated elements.
     * \param	elemCount	Number of elements to destroy.
     * \tparam  ELEM_TYPE   The name of object to destroy. Can be primitives or objects.
     *                      If objects (class or structure), the destructor should be accessible (i.e. public)
     * \note    The type ELEM_TYPE must have public available destructor.
     **/
    template <typename ELEM_TYPE>
    inline void destroyElems(ELEM_TYPE *begin, uint32_t elemCount);

    /**
     * \brief	Copies element from source to destination. The allocated buffer of destination should be big enough
     *          to store elemCount elements. The type of element should be specified.
     *          The type ELEM_DST should support assigning operator. The type ELEM_SRC should support conversion operator 
     *          to type ELEM_LEFT.
     * \param	destination	Pointer to destination buffer. It should have minimum size to keep elemCount elements.
     * \param	source	    Pointer to buffer containing element sources.
     * \param	elemCount	Amount of elements to copy from source to destination buffers.
     *                      The source and destination buffers should have at least elemCount elements.
     * \tparam  ELEM_DST    The type of elements in destination. Can be primitives or objects (class or structure).
     * \tparam  ELEM_SRC    The type of elements in destination. Can be primitives or objects (class or structure).
     * \note    If ELEM_DST and ELEM_SRC are different types: 
     *              - it must be possible to convert from ELEM_SRC to ELEM_DST to apply static_cast;
     *              - the ELEM_SRC should support assigning operator ( operator = ).
     **/
    template <typename ELEM_DST, typename ELEM_SRC = ELEM_DST>
    inline void copyElems(ELEM_DST *destination, const ELEM_SRC *source, uint32_t elemCount);

    /**
     * \brief	Moves elements starting from source to destination positions. The source and destination must refer
     *          to the same chunk of allocated memory. Do not call this function if buffers, which are allocated
     *          in different spaces. Function makes checkup to avoid memory overlapping.
     *          The type of element should be specified.
     * \param	destination	The pointer to destination to move elements from source
     * \param	source	    The pointer of source elements to move
     * \param	elemCount	The number of elements to move.
     * \tparam  ELEM_TYPE   The name of element type. Can be primitive or object (class or structure).
     * \note    The type ELEM_TYPE must support assigning operator ( operator = ).
     **/
    template <typename ELEM_TYPE>
    void moveElems(ELEM_TYPE * destination, const ELEM_TYPE * source, uint32_t elemCount);

    /**
     * \brief	Sets certain element value to the allocated memory. The type of allocated elements and element value
     *          should be specified. The type ELEM should support at least assigning operator.
     *          The type ELEM_TYPE should support conversion to type ELEM.
     * \param	begin	    The pointer to allocated element buffer.
     * \param	elemValue	The value to set for every element in buffer.
     * \param	elemCount	The number of elements allocated in buffer.
     * \note    The type ELEM should have appropriate assignment operator available to set data from type ELEM_TYPE,
     *          i.e. it should have operator 'operator = (ELEM_TYPE src)'.
     * \example areg::SetMemory.
     *          The following example is creating 100 elements and sets same value:
     *
     *          struct MyStruct {
     *              int32_t     data;   // since
     *          };
     *          MyStruct buffer[100];   // <= create buffer of 100 elements
     *          MyStruct zero = {0};    // <= create one element with value
     *          areg::SetMemory<MyStruct, const MyStruct &>(buffer, zero, 100); // <= sets same value to all entries.
     **/
    template <typename ELEM, typename ELEM_TYPE = ELEM>
    inline void setMemory(ELEM * begin, ELEM_TYPE elemValue, uint32_t elemCount);

    /**
     * \brief	Compares 2 buffers of same elements. The type ELEM should have comparing
     *          operator (operator == ). The comparing is done element-by-element.
     * \param	lhs	    The buffer of elements on left side
     * \param	rhs	    The buffer of elements on right side
     * \param	count	The amount of elements to compare
     * \return	Returns true if 2 buffers have same elements.
     * \tparam  ELEM    The type of elements to compare
     * \note    If elements type is structure or class, it should be possible
     *          to compare elements by applying comparing operator '==' ( operator == ).
     *          If not, use method areg::memEqual
     * \see     areg::memEqual
     **/
    template <typename ELEM>
    bool equalElements(const ELEM * lhs, const ELEM * rhs, uint32_t count);

    /**
     * \brief	Compares 2 buffers of different elements. 
     *          The type ELEM_LEFT should have comparing operator (operator == ). 
     *          The type ELEM_RIGHT should be possible to convert to type 'const ELEM_LEFT'
     *          The comparing is done element-by-element.
     * \param	lhs	    The buffer of elements on left side
     * \param	rhs	    The buffer of elements on right side
     * \param	count	The amount of elements to compare
     * \return	Returns true if 2 buffers have same elements.
     * \tparam  ELEM_LEFT   The type of elements to compare with
     * \tparam  ELEM_RIGHT  The type of elements to compare
     * \note    If elements type is structure or class, it should be possible
     *          to convert type of elements ELEM_RIGHT to the type 'const ELEM_LEFT' and
     *          ELEM_LEFT should have valid comparing operator (operator ==).
     * \see     areg::memEqual, areg::equalElement
     **/
    template <typename ELEM_LEFT, typename ELEM_RIGHT = ELEM_LEFT>
    bool equalElements(const ELEM_LEFT * lhs, const ELEM_RIGHT * rhs, uint32_t count);

    /**
     * \brief   Sets zero in a given element. The data is set byte-wise.
     *          Normally used to set zero in a structure.
     * \param   elem    The element to set zero
     * \tparam  ELEM    The type of element. Can be primitive or object (structure or class).
     *                  Normally expecting structure.
     **/
    template<typename ELEM>
    inline void zeroElement( ELEM & elem );

    /**
     * \brief   Sets zero in given element list. The data is set to each entry.
     *          If ELEM is an object (struct or class), there should be possible 
     *          to apply assigning operator with integer parameter (operator = (uint32_t number))
     * \param   elemList    The list of elements to set zero.
     * \param   elemCount   The number of elements in the list.
     * \tparam  ELEM        The type of element entry. Can be primitive or objects (struct or class).
     **/
    template<typename ELEM>
    inline void zeroElements( ELEM * elemList, uint32_t elemCount );

    /**
     * \brief   Fills specified buffer with the specified symbol.
     * \param   buffer  The buffer to fill.
     * \param   length  The length of buffer in bytes.
     * \param   symbol  The symbol to fill the buffer.
     **/
    inline void memSet( void * buffer, uint32_t length, uint8_t symbol );

    /**
     * \brief   Sets zero in a buffer.
     * \param   buffer  The buffer where zero should be set.
     * \param   length  The length of buffer in bytes.
     **/
    inline void memZero( void * buffer, uint32_t length );

    /**
     * \brief	Moves elements starting from source to destination positions. The source and destination must refer
     *          to the same chunk of allocated memory. Do not call this function if buffers, which are allocated
     *          in different spaces. The function assumes that the allocated memory is enough to move data.
     * \param	memDst  The pointer to destination position to move data
     * \param	memSrc	The pointer of source of data to move
     * \param	count   The number of bytes to move.
     **/
    inline void memMove(void * memDst, const void * memSrc, uint32_t count);

    /**
     * \brief	Copies data from source to destination. The allocated buffer of destination should be big enough
     *          to copy data. Otherwise, the function copies maximum available space in destination data.
     * \param	memDst      Pointer to destination buffer. It should have minimum size to keep elemCount elements.
     * \param   dstSpace    The space available in destination buffer.
     * \param	memSrc      Pointer to buffer containing sources of data to copy.
     * \param	count       Amount of data in bytes to copy.
     * \return  Returns amount of data actually copied in destination buffer.
     **/
    inline uint32_t memCopy( void * memDst, uint32_t dstSpace, const void * memSrc, uint32_t count);

    /**
     * \brief   Compares 2 chunks of memory object and returns the compare results:
     *          -   areg::Smaller if the content of memLeft is smaller than memRight;
     *          -   areg::Equal   if the content of memLeft is same as memRight;
     *          -   areg::Bigger if the content of memLeft is greater than memRight.
     *          The comparison is done byte by byte.
     * \param   memLeft     The pointer of chunk of buffer to compare.
     * \param   memRight    The pointer of chunk of buffer to compare with.
     * \param   count       The size if bytes to compare.
     * \return  The results are one of:
     *          -   areg::Smaller if the content of memLeft is smaller than memRight;
     *          -   areg::Equal   if the content of memLeft is same as memRight;
     *          -   areg::Bigger if the content of memLeft is greater than memRight.
     **/
    inline Ordering memCompare( const void * memLeft, const void * memRight, uint32_t count);

    /**
     * \brief   Compares 2 chunks of memories and return true if they are equal.
     *          2 chunks of memory are equal if either they are same or have same content.
     * \param   memLeft     The pointer of chunk of buffer to compare.
     * \param   memRight    The pointer of chunk of buffer to compare with.
     * \param   count       The size if bytes to compare.
     * \return  Returns true if both chunks are equal.
     **/
    inline bool memEqual( const void * memLeft, const void * memRight, uint32_t count);

    /**
     * \brief   The custom buffer allocator.
     **/
    template<typename BufType>
    struct BufferAllocator
    {
        /**
         * \brief   The operator is called when buffer object should be allocated.
         **/
        BufType* operator ( ) (uint32_t space);
    };

    /**
     * \brief   The custom buffer deleter.
     **/
    template<typename BufType>
    struct BufferDeleter
    {
        /**
         * \brief   The operator is called when buffer object should be deleted.
         **/
        void operator ( ) (void * buffer);
    };
}

/************************************************************************
 * Include for streaming
 ************************************************************************/
#include "areg/base/IOStream.hpp"

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
inline bool areg::operator == ( const areg::Primitive& lsh, const areg::Primitive& rhs )
{
    return ((&lsh == &rhs) || (lsh.valInt64.mElement == rhs.valInt64.mElement));
}

/**
 * \brief   compares to areg::Primitive values and returns true if they are not equal.
 **/
inline bool areg::operator != ( const areg::Primitive& lsh, const areg::Primitive& rhs )
{
    return ((&lsh != &rhs) && (lsh.valInt64.mElement != rhs.valInt64.mElement));
}

//////////////////////////////////////////////////////////////////////////
// NEMemory namespace function implementation.
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Comparing operators
/************************************************************************/

inline void areg::memSet( void * buffer, uint32_t length, uint8_t symbol )
{
    if ( (buffer != nullptr) && (length > 0) )
    {
        ::memset(buffer, symbol, length);
    }
}

inline void areg::memZero( void * buffer, uint32_t length )
{
    areg::memSet( buffer, length, 0x00u );
}

inline void areg::memMove( void * memDst, const void * memSrc, uint32_t count )
{
    if ( (memDst != nullptr) && (memSrc != nullptr) && (count > 0) )
    {
        ::memmove( memDst, memSrc, count );
    }
}

inline uint32_t areg::memCopy( void * memDst, uint32_t dstSpace, const void * memSrc, uint32_t count )
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

inline areg::Ordering areg::memCompare( const void * memLeft, const void * memRight, uint32_t count )
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

inline bool areg::memEqual( const void * memLeft, const void * memRight, uint32_t count )
{
    return (areg::Ordering::Equal == memCompare(memLeft, memRight, count));
}


/************************************************************************/
// Byte buffer functions
/************************************************************************/

inline uint8_t * areg::getBufferDataWrite(areg::RawBuffer * byteBuffer)
{
    return (byteBuffer != nullptr ? reinterpret_cast<uint8_t *>(byteBuffer) + byteBuffer->bufHeader.biOffset : nullptr);
}

inline const uint8_t * areg::getBufferDataRead(const RawBuffer * byteBuffer)
{
    return (byteBuffer != nullptr ? reinterpret_cast<const uint8_t *>(byteBuffer) + byteBuffer->bufHeader.biOffset : nullptr);
}

/************************************************************************/
// Function templates
/************************************************************************/

template <typename ELEM_TYPE>
inline ELEM_TYPE * areg::constructElems(void *begin, uint32_t elemCount)
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
inline ELEM_TYPE * areg::constructElemsWithArgument(void *begin, uint32_t elemCount, ARGUMENT_TYPE arg)
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

template <typename ELEM_TYPE>
inline void areg::destroyElems(ELEM_TYPE *begin, uint32_t elemCount)
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
inline void areg::copyElems(ELEM_DST *destination, const ELEM_SRC *source, uint32_t elemCount)
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
void areg::moveElems(ELEM_TYPE *destination, const ELEM_TYPE *source, uint32_t elemCount)
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
inline void areg::setMemory(ELEM* begin, ELEM_TYPE elemValue, uint32_t elemCount)
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
inline bool areg::equalElements(const ELEM * lhs, const ELEM * rhs, uint32_t count)
{
    return equalElements<ELEM, ELEM>(lhs, rhs, count);
}


template <typename ELEM_LEFT, typename ELEM_RIGHT /*= ELEM_LEFT*/>
inline bool areg::equalElements(const ELEM_LEFT* lhs, const ELEM_RIGHT* rhs, uint32_t count)
{
    bool result = false;
    if ( (count == 0) || (lhs == rhs) )
    {
        result = true;
    }
    else if ( (lhs != nullptr) && (rhs != nullptr) )
    {
        result = true;
        while (count-- != 0)
        {
            if (*lhs != static_cast<const ELEM_LEFT&>(*rhs))
            {
                result = false;
                break;
            }
        }
    }

    return result;
}

template<typename ELEM>
inline void areg::zeroElement( ELEM & elem )
{
    areg::zeroElements<ELEM>(&elem, 1);
}

template<typename ELEM>
inline void areg::zeroElements( ELEM * elemList, uint32_t elemCount )
{
    if ( elemCount > 0 )
    {
        constexpr uint32_t one = static_cast<int32_t>(sizeof(ELEM));
        areg::memZero( reinterpret_cast<void *>(elemList), elemCount * one );
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

inline const char * areg::getString( areg::MessageResult msgResult )
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

inline const char * areg::getString(areg::BufferType val )
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

#endif  // AREG_BASE_MEMORYDEFS_HPP
