#ifndef AREG_BASE_NEMEMORY_HPP
#define AREG_BASE_NEMEMORY_HPP
/************************************************************************
 * \file        areg/src/base/NEMemory.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Basic functions and types to
 *              deal with objects located in memory.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/IEIOStream.hpp"
#include <new>

/************************************************************************
 * \brief   MACRO. Main defines to simplify data access.
 ************************************************************************/
/**
 * \brief   Alignment type
 **/
#define ALIGNED_TYPE            NEMemory::uAlign
/**
 * \brief   Aligns passed value to the size of uAlign object.
 **/
#define ALIGNED_TYPE_SIZE(x)    MACRO_ALIGN_SIZE(sizeof(x), sizeof(NEMemory::uAlign))

/**
 * \brief   Gets boolean value of align object.
 **/
#define ARG_BOOL(aln)           static_cast<NEMemory::uAlign>(aln).alignBool
/**
 * \brief   Gets char value of align object.
 **/
#define ARG_CHAR(aln)           static_cast<NEMemory::uAlign>(aln).alignChar
/**
 * \brief   Gets unsigned char value of align object.
 **/
#define ARG_UCHAR(aln)          static_cast<NEMemory::uAlign>(aln).alignUChar
/**
 * \brief   Gets wide char value of align object.
 **/
#define ARG_WCHAR(aln)          static_cast<NEMemory::uAlign>(aln).alignWChar
/**
 * \brief   Gets short value of align object.
 **/
#define ARG_SHORT(aln)          static_cast<NEMemory::uAlign>(aln).alignShort
/**
 * \brief   Gets unsigned short value of align object.
 **/
#define ARG_USHORT(aln)         static_cast<NEMemory::uAlign>(aln).alignUShort
/**
 * \brief   Gets int value of align object.
 **/
#define ARG_INT(aln)            static_cast<NEMemory::uAlign>(aln).alignInt
/**
 * \brief   Gets unsigned int value of align object.
 **/
#define ARG_UINT(aln)           static_cast<NEMemory::uAlign>(aln).alignUInt
/**
 * \brief   Gets long value of align object.
 **/
#define ARG_LONG(aln)           static_cast<NEMemory::uAlign>(aln).alignLong
/**
 * \brief   Gets unsigned long value of align object.
 **/
#define ARG_ULONG(aln)          static_cast<NEMemory::uAlign>(aln).algnULong
/**
 * \brief   Gets 64-bit integer value of align object.
 **/
#define ARG_INT64(aln)          static_cast<NEMemory::uAlign>(aln).alignInt64
/**
 * \brief   Gets 64-bit unsigned integer value of align object.
 **/
#define ARG_UINT64(aln)         static_cast<NEMemory::uAlign>(aln).alignUInt64
/**
 * \brief   Gets float value of align object.
 **/
#define ARG_FLOAT(aln)          static_cast<NEMemory::uAlign>(aln).alignFloat
/**
 * \brief   Gets double value of align object.
 **/
#define ARG_DOUBLE(aln)         static_cast<NEMemory::uAlign>(aln).alignDouble
/**
 * \brief   Gets long double value of align object.
 **/
#define ARG_LDOUBLE(aln)        static_cast<NEMemory::uAlign>(aln).alignLDouble
/**
 * \brief   Gets pointer value of align object.
 **/
#define ARG_PTR(aln)            static_cast<NEMemory::uAlign>(aln).alignPtr
/**
 * \brief   Gets pointer to class value of align object.
 **/
#define ARG_CLSPTR(aln)         static_cast<NEMemory::uAlign>(aln).alignClsPtr

#ifdef _DEBUG

    /**
     * \brief   Set 1 if want to zero allocated memory.
     *          Set 0 if no need to zero allocated memory
     **/
    #define DBG_SET_MEM_ZERO                0

    #if DBG_SET_MEM_ZERO

        #define DBG_ZERO_MEM(buf, size)     { if ((buf) != NULL)  NEMemory::ZeroBuffer((buf), size); }

    #else   // DBG_SET_MEM_ZERO

        #define DBG_ZERO_MEM(buf, size)

    #endif  // DBG_SET_MEM_ZERO

#else   // _DEBUG

    #define DBG_ZERO_MEM(buf, size)

#endif  // _DEBUG


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
namespace NEMemory
{
//////////////////////////////////////////////////////////////////////////
// Internal types used for alignment
//////////////////////////////////////////////////////////////////////////
    
    class _EmptyClass;                                          //!< Dummy class declaration
    typedef void ( *                _EmptyMethod     ) (void);  //!< Dummy pointer to global function declaration
    typedef void (_EmptyClass::*    _EmptyClassMethod) (void);  //!< Dummy pointer to class function declaration
    typedef int   _EmptyClass::*    _EmptyClassMember;          //!< Dummy pointer to class variable declaration

    /**
     * \brief   TEAlign structure template
     *          defining single element used in Align union.
     * \tparam  ELEM_TYPE   Any type.
     **/
    template <class ELEM_TYPE>
    struct TEAlign
    {
        ELEM_TYPE   mElement;
    };

    //////////////////////////////////////////////////////////////////////////
    // NEMemory::uAlign union declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NEMemory::uAlign
     *          Align union, defines possible variation of
     *          primitives. It is used to align buffer allocation
     *          It can be also used for single primitive element 
     *          serialization / de-serializations.
     *          Pointers to class function and variables should be
     *          treated with special care. Do not serialize them.
     **/
    typedef union U_Align
    {
        TEAlign<bool>                          alignBool;      //!< boolean value
        TEAlign<char>                          alignChar;      //!< char value
        TEAlign<unsigned char>                 alignUChar;     //!< unsigned char value
        TEAlign<wchar_t>                       alignWChar;     //!< wide char value
        TEAlign<short>                         alignShort;     //!< short value
        TEAlign<unsigned short>                alignUShort;    //!< unsigned short value
        TEAlign<int>                           alignInt;       //!< int value
        TEAlign<unsigned int>                  alignUInt;      //!< unsigned int value
        TEAlign<long double>                   alignLDouble;   //!< long double value
        TEAlign<long>                          alignLong;      //!< long value
        TEAlign<unsigned long>                 alignULong;     //!< unsigned long value
        TEAlign<int64_t>                       alignInt64;     //!< 64-bit integer value
        TEAlign<uint64_t>                      alignUInt64;    //!< 64-bit unsigned integer value
        TEAlign<float>                         alignFloat;     //!< float value
        TEAlign<double>                        alignDouble;    //!< double
        TEAlign<void *>                        alignPtr;       //!< pointer value
        TEAlign<void (*)( void )>              alignFunc;      //!< pointer to function value
        TEAlign<_EmptyClass *>                 alignClsPtr;    //!< pointer to class value
        TEAlign<void (_EmptyClass::*)( void )> alignClsFunc;   //!< pointer to class function value
        TEAlign<int _EmptyClass::*>            alignClsVar;    //!< pointer to class variable value

    } uAlign;

    /**
     * \brief   NEMemory::BufferData
     *          Defines Buffer Data type. The size of buffer should be aligned
     *          to size of uAlign union type
     **/
    typedef unsigned char   BufferData;

    /**
     * \brief   NEMemory::InvalidElement
     *          Predefined Invalid Element value. Used to defined invalid element.
     *          For example, it is used in thread local storage object (CEThreadLocalStorage)
     *          in case if there is no element found in local storage by given name.
     *
     * \see     CEThreadLocalStorage::GetStorageItem()
     **/
    extern AREG_API const NEMemory::uAlign InvalidElement;

    /**
     * \brief   Compares 2 NEMemory::uAlign elements, returns true if they are equal
     * \param   lsh     Left-Hand Operand
     * \param   rhs     Right-Hand Operand
     * \return  Returns true if 2 NEMemory::uAlign elements are equal
     **/
    inline bool operator == (const NEMemory::uAlign & lsh, const NEMemory::uAlign & rhs);

    /**
     * \brief   Compares 2 NEMemory::uAlign elements, returns true if they are not equal
     * \param   lsh     Left-Hand Operand
     * \param   rhs     Right-Hand Operand
     * \return  Returns true if 2 NEMemory::uAlign elements are not equal
     **/
    inline bool operator != (const NEMemory::uAlign & lsh, const NEMemory::uAlign & rhs);

    /**
     * \brief   Types of data buffer
     *          NEMemory::eBufferType
     **/
    typedef enum E_BufferType
    {
          BufferUnknown     = -1    //!< Unknown buffer type, not used
        , BufferInternal    =  0    //!< Buffer type for internal communication
        , BufferRemote      =  2    //!< Buffer type for remote communication
    } eBufferType;

    //////////////////////////////////////////////////////////////////////////
    // NEMemory::sBuferHeader structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NEMemory::sBuferHeader
     *          Structure of Binary Buffer object data header info.
     *          Used in all binary buffers. It stores basic information 
     *          buffer, it's type, allocated and used sizes.
     **/
    typedef struct S_BuferHeaderInfo
    {
        /**
         * \brief   The actual size of complete binary Buffer object,
         *          including header and allocated sizes
         **/
        unsigned int    biBufSize;
        /**
         * \brief   The length in bytes of totally allocated buffer.
         **/
        unsigned int    biLength;
        /**
         * \brief   The size of buffer header. The buffer data should start after header size offset
         **/
        unsigned int    biOffset;
        /**
         * \brief   The type of buffer. For RPC communication this should be external type.
         **/
        eBufferType     biBufType;
        /**
         * \brief   The length in bytes of used space in buffer.
         *          Cannot be more than biLength value.
         **/
        unsigned int    biUsed;
        /**
         * \brief   Reference count. Used by Shared Buffer object.
         *          As long as reference count is more than zero,
         *          it cannot be deleted.
         **/
        unsigned int    biRefCount;
    } sBuferHeader;

    //////////////////////////////////////////////////////////////////////////
    // NEMemory::sRemoteBuferHeader structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NEMemory::sRemoteMessageHeader
     *          Structure of binary buffer for Remote data transfer.
     *          It is extended type of sBuferHeader with additions
     *          of message ID, message sequence number, cookie and checksum.
     **/
    typedef struct S_RemoteMessageHeader
    {
        /**
         * \brief   The common buffer header information
         **/
        sBuferHeader    rbhBufHeader;
        /**
         * \brief   An ID of target object, receiving message.
         *          In remote messaging, this is Cookie of target
         **/
        ITEM_ID         rbhTarget;
        /**
         * \brief   Data checksum value for validation check-up.
         *          Should be ignored if value is NEMemory::IGNORE_CHECKSUM
         **/
        unsigned int    rbhChecksum;
        /**
         * \brief   An ID of source object, sending message.
         *          In remote messaging, this is Cookie of source
         **/
        ITEM_ID         rbhSource;
        /**
         * \brief   The Remote message ID registered in the system
         **/
        unsigned int    rbhMessageId;
        /**
         * \brief   The result of processing message.
         **/
        unsigned int    rbhResult;
        /**
         * \brief   The Remote message sequence number set during messaging
         **/
        unsigned int    rbhSequenceNr;
    } sRemoteMessageHeader;

    //////////////////////////////////////////////////////////////////////////
    // NEMemory::sByteBuffer structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NEMemory::sByteBuffer
     *          Specify the Byte Buffer object.
     *          Contains the size of complete object,
     *          buffer information and elements followed Byte Buffer object.
     **/
    typedef struct S_ByteBuffer
    {
        /**
         * \brief   Byte Buffer information
         **/
        sBuferHeader    bufHeader;
        /**
         * \brief   Byte Buffer Data followed after structure.
         *          This is referring to the first element in the data buffer.
         **/
        BufferData      bufData[1];
    } sByteBuffer;


    //////////////////////////////////////////////////////////////////////////
    // NEMemory::sRpcMessageBuffer structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NEMemory::sRemoteMessage
     *          Specify the Byte Buffer object.
     *          Contains the size of complete object,
     *          buffer information and elements followed Byte Buffer object.
     **/
    typedef struct S_RemoteMessage
    {
        /**
         * \brief   Byte Buffer information
         **/
        sRemoteMessageHeader    rbHeader;
        /**
         * \brief   Byte Buffer Data followed after structure.
         *          This is referring to the first element in the data buffer.
         **/
        BufferData              rbData[1];
    } sRemoteMessage;

    /**
     * \brief   Message communication results
     **/
    typedef enum E_MessageResult
    {
          ResultUnknownError        = -1    //!< Error, unknown type
        , ResultSucceed             =  0    //!< No error
        , ResultNoConnection                //!< Error, there is no connection
        , ResultServiceUnavailable          //!< Error, service is unavailable
        , ResultServiceRejected             //!< Error, service is rejected
        , ResultTargetUnavailable           //!< Error, the target object is unavailable
    } eMessageResult;
    /**
     * \brief   Converts the values of type NEMemory::eMessageResult to string, used in logs and output messages.
     **/
    static const char * GetString( NEMemory::eMessageResult msgResult );

    /**
     * \brief   NEMemory::BLOCK_SIZE
     *          Constant. Defines the minimum size of Byte Buffer data
     *          Also defines the size to align buffer allocation.
     **/
    extern AREG_API const unsigned int      BLOCK_SIZE      /*= sizeof(NEMemory::uAlign)*/;
    /**
     * \brief   NEMemory::INVALID_SIZE
     *          Constant. Defines invalid buffer size.
     **/
    extern AREG_API const unsigned int      INVALID_SIZE    /*= INVALID_FILE_SIZE*/;

    /**
     * \brief   NEMemory::IGNORE_VALUE
     *          Constant. Defines ignore value for remote buffer, i.e. not set and should be ignored.
     **/
    extern AREG_API const unsigned int      IGNORE_VALUE    /*= static_cast<unsigned int>(0)*/;
    /**
     * \brief   NEMemory::INVALID_VALUE
     *          Constant. Defines invalid value for remote buffer.
     **/
    extern AREG_API const unsigned int      INVALID_VALUE   /*= static_cast<unsigned int>(-1)*/;

    /**
     * \brief   NEMemory::InvalidRemoteBuffer
     *          Constant. Predefined Invalid Remote Buffer object.
     **/
    extern AREG_API NEMemory::sRemoteMessage &      InvalidRemoteMessage;
    /**
     * \brief   NEMemory::InvalidBuffer
     *          Constant. Predefined Invalid Byte Buffer object.
     **/
    extern AREG_API NEMemory::sByteBuffer &         InvalidBuffer;

    /**
     * \brief   Checks whether the byte buffer object is valid or not.
     *          Byte buffer object is invalid if it does not refer to 
     *          NEMemory::InvaldBuffer object.
     * \param   byteBuffer  The instance of byte buffer object to check.
     * \return  Returns true if byte-buffer object is valid.
     **/
    inline bool IsBufferValid( const sByteBuffer * byteBuffer );

    /**
     * \brief   Checks whether the pointer to byte buffer object is valid or not.
     *          Byte buffer pointer is valid if it is not NULL and does not refer to 
     *          NEMemory::InvaldBuffer object.
     * \param   byteBuffer  The pointer to instance of byte buffer object to check.
     * \return  Returns true if byte-buffer object is valid.
     **/
    inline bool IsBufferValid( const sByteBuffer & byteBuffer );

    /**
     * \brief   Returns the pointer to data buffer for writing. 
     *          Returns NULL if byte buffer object is invalid.
     * \param   byteBuffer  The instance of byte-buffer object.
     * \return  Returns data buffer to write.
     **/
    inline unsigned char * GetBufferDataWrite( sByteBuffer & byteBuffer );

    /**
     * \brief   Returns the pointer to data buffer for writing. 
     *          Returns NULL if pointer to byte buffer is null 
     *          or refers to invalid byte buffer object..
     * \param   byteBuffer  The pointer to byte-buffer object.
     * \return  Returns data buffer to write.
     **/
    inline unsigned char * GetBufferDataWrite( sByteBuffer * byteBuffer );

    /**
     * \brief   Returns the pointer to data buffer for reading. 
     *          Returns NULL if byte buffer object is invalid.
     * \param   byteBuffer  The instance of byte-buffer object.
     * \return  Returns data buffer to read.
     **/
    inline const unsigned char * GetBufferDataRead( const sByteBuffer & byteBuffer );

    /**
     * \brief   Returns the pointer to data buffer for reading. 
     *          Returns NULL if pointer to byte buffer is null 
     *          or refers to invalid byte buffer object..
     * \param   byteBuffer  The pointer to byte-buffer object.
     * \return  Returns data buffer to read.
     **/
    inline const unsigned char * GetBufferDataRead( const sByteBuffer * byteBuffer );

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
    inline ELEM_TYPE * ConstructElems(void *begin, int elemCount);

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
    inline ELEM_TYPE * ConstructElemsWithArgument(void *begin, int elemCount, ARGUMENT_TYPE arg);

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
    inline void DestroyElems(ELEM_TYPE *begin, int elemCount);

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
    template <typename ELEM_DST, typename ELEM_SRC>
    inline void CopyElems(ELEM_DST *destination, const ELEM_SRC *source, int elemCount);

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
    void MoveElems(ELEM_TYPE * destination, const ELEM_TYPE * source, int elemCount);

    /**
     * \brief	Sets certain element value to the allocated memory. The type of allocated elements and element value
     *          should be specified. The type ELEM should support at least assigning operator.
     *          The type ELEM_TYPE should support conversion to type ELEM.
     * \param	begin	    The pointer to allocated element buffer.
     * \param	elemValue	The value to set for every element in buffer.
     * \param	elemCount	The number of elements allocated in buffer.
     * \note    The type ELEM should have appropriate assignment operator available to set data from type ELEM_TYPE,
     *          i.e. it should have operator 'operator = (ELEM_TYPE src)'.
     * \example NEMath::SetMemory.
     *          The following example is creating 100 elements and sets same value:
     *
     *          struct MyStruct {
     *              int     data;   // since
     *          };
     *          MyStruct buffer[100];   // <= create buffer of 100 elements
     *          MyStruct zero = {0};    // <= create one element with value
     *          NEMemory::SetMemory<MyStruct, const MyStruct &>(buffer, zero, 100); // <= sets same value to all entries.
     **/
    template <typename ELEM, typename ELEM_TYPE>
    inline void SetMemory(ELEM * begin, ELEM_TYPE elemValue, int elemCount);

    /**
     * \brief	Compares 2 buffers of same elements. No need that ELEM type has comparing operator.
     *          It compares buffer byte-by-byte.
     * \param	lhs	    The buffer of elements on left-side
     * \param	rhs	    The buffer of elements on right-side
     * \param	count	The amount of elements to compare
     * \return	Returns true if 2 buffers are equal.
     * \tparam  ELEM    The type of elements to compare
     * \note    If element type is structure or class, there is no need to have
     *          defined comparing operators, because the function compares byte-wise.
     * \see     NEMemory::IsEqualElement
     **/
    template <typename ELEM>
    bool IsEqualBuffer(const ELEM * lhs, const ELEM * rhs, int count);

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
     *          If not, use method NEMemory::IsEqualBuffer
     * \see     NEMemory::IsEqualBuffer
     **/
    template <typename ELEM>
    bool IsEqualElement(const ELEM * lhs, const ELEM * rhs, int count);

    /**
     * \brief	NEMemory::IsEqualElement<ELEM>( )
     *          Compares 2 buffers of same or different elements.
     *          The types ELEM_LEFT and ELEM_RIGHT should have
     *          appropriate comparing operator. The comparing is
     *          done element-by-element. Every element will be
     *          converted to type <const ELEM_LEFT &> type.
     * \param	lhs	    The buffer of elements on left side
     * \param	rhs	    The buffer of elements on right side
     * \param	count	The amount of elements to compare
     * \return	Returns true if all elements are equal.
     **/
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
     * \see     NEMemory::IsEqualBuffer, NEMemory::IsEqualElement
     **/
    template <typename ELEM_LEFT, typename ELEM_RIGHT>
    bool IsEqualElement(const ELEM_LEFT * lhs, const ELEM_RIGHT * rhs, int count);

    /**
     * \brief   Sets zero in a given element. The data is set byte-wise.
     *          Normally used to set zero in a structure.
     * \param   elem    The element to set zero
     * \tparam  ELEM    The type of element. Can be primitive or object (structure or class).
     *                  Normally expecting structure.
     **/
    template<typename ELEM>
    inline void ZeroData( ELEM & elem );

    /**
     * \brief   Sets zero in given element list. The data is set to each entry.
     *          If ELEM is an object (struct or class), there should be possible 
     *          to apply assigning operator with integer parameter (operator = (int number))
     * \param   elemList    The list of elements to set zero.
     * \param   elemCount   The number of elements in the list.
     * \tparam  ELEM        The type of element entry. Can be primitive or objects (struct or class).
     **/
    template<typename ELEM>
    inline void ZeroElements( ELEM * elemList, uint32_t elemCount );

    /**
     * \brief   Sets zero in a buffer.
     * \param   buffer  The buffer where zero should be set.
     * \param   length  The length of buffer in bytes.
     **/
    inline void ZeroBuffer( void * buffer, unsigned int length );

    /**
     * \brief	Moves elements starting from source to destination positions. The source and destination must refer
     *          to the same chunk of allocated memory. Do not call this function if buffers, which are allocated
     *          in different spaces. The function assumes that the allocated memory is enough to move data.
     * \param	memDst  The pointer to destination position to move data
     * \param	memSrc	The pointer of source of data to move
     * \param	count   The number of bytes to move.
     **/
    inline void MemMove(void * memDst, const void * memSrc, unsigned int count);

    /**
     * \brief	Copies data from source to destination. The allocated buffer of destination should be big enough
     *          to copy data. Otherwise, the function copies maximum available space in destination data.
     * \param	memDst      Pointer to destination buffer. It should have minimum size to keep elemCount elements.
     * \param   dstSpace    The space available in destination buffer.
     * \param	memSrc      Pointer to buffer containing sources of data to copy.
     * \param	count       Amount of data in bytes to copy.
     * \return  Returns amount of data actually copied in destination buffer.
     **/
    inline unsigned int MemCopy( void * memDst, unsigned int dstSpace, const void * memSrc, unsigned int count);
}

/************************************************************************
 * Include for streaming
 ************************************************************************/
#include "areg/src/base/IEIOStream.hpp"

/************************************************************************
 * \brief   Streaming of NEMemory::uAlign
 *          Global streaming operators to read and write to streaming object
 ************************************************************************/

/**
 * \brief   Support streaming operator for NEMemory::uAlign type.
 *          Read NEMemory::uAlign from streaming object
 * \param   stream  The streaming object to read
 * \param   input   The NEMemory::uAlign item to initialize from stream
 * \return  Reading streaming object
 **/
inline const IEInStream & operator >> (const IEInStream & stream, NEMemory::uAlign & input)
{   stream.Read( reinterpret_cast<unsigned char *>(&input), sizeof(NEMemory::uAlign) ); return stream; }

/**
 * \brief   Support streaming operator for NEMemory::uAlign type.
 *          Write NEMemory::uAlign to streaming object
 * \param   stream  The streaming object to write
 * \param   input   The NEMemory::uAlign item to write to stream
 * \return  Writing streaming object
 **/
inline IEOutStream & operator << (IEOutStream & stream, const NEMemory::uAlign & output)
{   stream.Write( reinterpret_cast<const unsigned char *>(&output), sizeof(NEMemory::uAlign) ); return stream; }

/************************************************************************
 * \brief   Streaming of NEMemory::uAlign
 *          Global comparing operators
 ************************************************************************/
 
/**
 * \brief   compares to NEMemory::uAlign values and returns true if they are equal.
 **/
inline bool NEMemory::operator == ( const NEMemory::uAlign& lsh, const NEMemory::uAlign& rhs )
{   return (&lsh != &rhs ? lsh.alignInt64.mElement == rhs.alignInt64.mElement : true);  }

/**
 * \brief   compares to NEMemory::uAlign values and returns true if they are not equal.
 **/
inline bool NEMemory::operator != ( const NEMemory::uAlign& lsh, const NEMemory::uAlign& rhs )
{   return (&lsh != &rhs ? lsh.alignInt64.mElement != rhs.alignInt64.mElement : false); }

//////////////////////////////////////////////////////////////////////////
// NEMemory namespace function implementation.
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Comparing operators
/************************************************************************/

inline void NEMemory::ZeroBuffer( void * buffer, unsigned int length )
{
    NEMemory::SetMemory<unsigned char, unsigned char>( reinterpret_cast<unsigned char *>(buffer), 0x00u, length );
}

inline void NEMemory::MemMove( void * memDst, const void * memSrc, unsigned int count )
{
    NEMemory::MoveElems<unsigned char>( reinterpret_cast<unsigned char *>(memDst), reinterpret_cast<const unsigned char *>(memSrc), count);
}

inline unsigned int NEMemory::MemCopy( void * memDst, unsigned int dstSpace, const void * memSrc, unsigned int count )
{
    unsigned int result = 0;
    if ( (memDst != memSrc) && (memDst != static_cast<const unsigned char *>(NULL)) && (memSrc != static_cast<const unsigned char *>(NULL)) )
    {
        result = MACRO_MIN(dstSpace, count);
        NEMemory::CopyElems<unsigned char, unsigned char>( reinterpret_cast<unsigned char *>(memDst), reinterpret_cast<const unsigned char *>(memSrc), result );
    }

    return result;
}

/************************************************************************/
// Byte buffer functions
/************************************************************************/

inline bool NEMemory::IsBufferValid(const NEMemory::sByteBuffer * byteBuffer)
{
    return ((byteBuffer != NULL) && (byteBuffer != reinterpret_cast<const NEMemory::sByteBuffer *>(&NEMemory::InvalidBuffer)));
}

inline bool NEMemory::IsBufferValid(const NEMemory::sByteBuffer & byteBuffer)
{
    return (&byteBuffer != reinterpret_cast<const NEMemory::sByteBuffer *>(&NEMemory::InvalidBuffer));
}

inline unsigned char * NEMemory::GetBufferDataWrite(NEMemory::sByteBuffer & byteBuffer)
{
    return (NEMemory::IsBufferValid(byteBuffer) ? reinterpret_cast<unsigned char *>(&byteBuffer) + byteBuffer.bufHeader.biOffset : 0);
}

inline unsigned char * NEMemory::GetBufferDataWrite(NEMemory::sByteBuffer * byteBuffer)
{
    return (NEMemory::IsBufferValid(byteBuffer) ? reinterpret_cast<unsigned char *>(byteBuffer) + byteBuffer->bufHeader.biOffset : 0);
}

inline const unsigned char * NEMemory::GetBufferDataRead(const sByteBuffer & byteBuffer)
{
    return (NEMemory::IsBufferValid(byteBuffer) ? reinterpret_cast<const unsigned char *>(&byteBuffer) + byteBuffer.bufHeader.biOffset : 0);
}

inline const unsigned char * NEMemory::GetBufferDataRead(const sByteBuffer * byteBuffer)
{
    return (NEMemory::IsBufferValid(byteBuffer) ? reinterpret_cast<const unsigned char *>(byteBuffer) + byteBuffer->bufHeader.biOffset : 0);
}

/************************************************************************/
// Function templates
/************************************************************************/

template <typename ELEM_TYPE>
inline ELEM_TYPE * NEMemory::ConstructElems(void *begin, int elemCount)
{
    if ( static_cast<const void *>(begin) != static_cast<const void *>(NULL) )
    {
        ELEM_TYPE* elems    = reinterpret_cast<ELEM_TYPE *>(begin);

#if _MSC_VER
    #pragma warning(disable: 4345)
#endif  // _MSC_VER
        for (int i = 0; i < elemCount; ++ i, ++ elems)
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
inline ELEM_TYPE * NEMemory::ConstructElemsWithArgument(void *begin, int elemCount, ARGUMENT_TYPE arg)
{
    if ( static_cast<const void *>(begin) != static_cast<const void *>(NULL) )
    {
        ELEM_TYPE* elems    = reinterpret_cast<ELEM_TYPE *>(begin);

#if _MSC_VER
    #pragma warning(disable: 4345)
#endif  // _MSC_VER
        for ( int i = 0; i < elemCount; ++ i, ++ elems )
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
inline void NEMemory::DestroyElems(ELEM_TYPE *begin, int elemCount)
{
    if ( static_cast<const void *>(begin) != static_cast<const void *>(NULL) )
    {
        ELEM_TYPE* elems    = static_cast<ELEM_TYPE *>(begin);
        for (int i = 0; i < elemCount; ++ i, ++ elems)
            elems->~ELEM_TYPE();
    }
}

template <typename ELEM_DST, typename ELEM_SRC>
inline void NEMemory::CopyElems(ELEM_DST *destination, const ELEM_SRC *source, int elemCount)
{
    if ((destination != source) && (destination != static_cast<const ELEM_DST *>(NULL)) && (source != static_cast<const ELEM_SRC *>(NULL)) )
    {
        for ( ; elemCount > 0; -- elemCount, ++ destination, ++ source )
            *destination = static_cast<const ELEM_DST &>(*source);
    }
}

template <typename ELEM_TYPE>
void NEMemory::MoveElems(ELEM_TYPE *destination, const ELEM_TYPE *source, int elemCount)
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
            * --destination = * --source;
    }
    else
    {
        ; // do nothing
    }
}

template <typename ELEM, typename ELEM_TYPE>
inline void NEMemory::SetMemory(ELEM* begin, ELEM_TYPE elemValue, int elemCount)
{
    if (begin != static_cast<const ELEM *>(NULL) && elemCount > 0)
    {
        while (elemCount -- > 0)
            *begin ++ = elemValue;
    }
}

template <typename ELEM>
inline bool NEMemory::IsEqualBuffer(const ELEM * lhs, const ELEM * rhs, int count)
{
    bool result = false;
    if ((lhs != static_cast<const ELEM *>(NULL)) && (rhs != static_cast<const ELEM *>(NULL)))
    {
        result = true;
        if (lhs != rhs)
        {
            const unsigned char * left  = reinterpret_cast<const unsigned char *>(reinterpret_cast<const void *>(lhs));
            const unsigned char * right = reinterpret_cast<const unsigned char *>(reinterpret_cast<const void *>(rhs));
            count = count * sizeof(ELEM);

            for ( ; (count != 0) && (*left == *right); ++left, ++right)
                --count;

            result = count == 0;
        }
    }
    else
    {
        result = (lhs == static_cast<const ELEM *>(NULL)) && (rhs == static_cast<const ELEM *>(NULL));
    }

    return result;
}

template <typename ELEM>
inline bool NEMemory::IsEqualElement(const ELEM * lhs, const ELEM * rhs, int count)
{
    bool result = false;
    if ((lhs != static_cast<const ELEM *>(NULL)) && (rhs != static_cast<const ELEM *>(NULL)))
    {
        result = true;
        if (lhs != rhs)
        {
            for ( ; (count != 0) && (*lhs == *rhs); ++lhs, ++rhs)
                --count;

            result = count == 0;
        }
    }
    else
    {
        result = (lhs == static_cast<const ELEM *>(NULL)) && (rhs == static_cast<const ELEM *>(NULL));
    }
    
    return result;
}


template <typename ELEM_LEFT, typename ELEM_RIGHT>
inline bool NEMemory::IsEqualElement(const ELEM_LEFT* lhs, const ELEM_RIGHT* rhs, int count)
{
    bool result = false;
    if ((lhs != static_cast<const ELEM_LEFT *>(NULL)) && (rhs != static_cast<const ELEM_RIGHT *>(NULL)))
    {
        result = true;
        if ( reinterpret_cast<const void *>(lhs) != reinterpret_cast<const void *>(rhs) )
        {
            for ( ;(count != 0) && (static_cast<const ELEM_LEFT &>(*lhs) == static_cast<const ELEM_LEFT &>(*rhs)); ++lhs, ++rhs)
                -- count;

            result = count == 0;
        }
    }
    else
    {
        result = (lhs == NULL) && (rhs == NULL);
    }

    return result;
}

template<typename ELEM>
inline void NEMemory::ZeroData( ELEM & elem )
{
    NEMemory::ZeroBuffer( reinterpret_cast<void *>(&elem), sizeof( ELEM ) );
}

template<typename ELEM>
inline void NEMemory::ZeroElements( ELEM * elemList, uint32_t elemCount )
{
    NEMemory::ZeroBuffer( reinterpret_cast<void *>(elemList), elemCount * sizeof( ELEM ) );
}

inline const char * NEMemory::GetString( NEMemory::eMessageResult msgResult )
{
    switch ( msgResult )
    {
    case NEMemory::ResultUnknownError:
        return "NEMemory::ResultUnknownError";
    case NEMemory::ResultSucceed:
        return "NEMemory::ResultSucceed";
    case NEMemory::ResultNoConnection:
        return "NEMemory::ResultNoConnection";
    case NEMemory::ResultServiceUnavailable:
        return "NEMemory::ResultServiceUnavailable";
    case NEMemory::ResultServiceRejected:
        return "NEMemory::ResultServiceRejected";
    case NEMemory::ResultTargetUnavailable:
        return "NEMemory::ResultTargetUnavailable";
    default:
        return "ERR: Invalid NEMemory::eMessageResult value!!!";
    }
}

#endif  // AREG_BASE_NEMEMORY_HPP
