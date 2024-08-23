#ifndef AREG_BASE_TERINGSTACK_HPP
#define AREG_BASE_TERINGSTACK_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/TERingStack.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, FIFO Stack class templates.
 *              Blocking and non-blocking FIFO Stacks, used to queue
 *              elements in the Stack without searching elements, 
 *              accessing by position or index.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TETemplateBase.hpp"
#include "areg/base/SynchObjects.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEMemory.hpp"

/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
template <typename VALUE> class TERingStack;
    template <typename VALUE> class TELockRingStack;
    template <typename VALUE> class TENolockRingStack;

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Ring FIFO Stack base object to queue elements, insert and access
 *          by push and pop operations. RingStack requires instance of
 *          synchronization object to synchronize access of stack elements,
 *          capacity value and overlapping flag.
 *
 *          The capacity might be changed depending on overlapping flag.
 *          If ring stack is full, whether the capacity remains same or not,
 *          whether new element is pushed or not, depends on overlapping flag.
 *          For more details of capacity flag see NECommon::eRingOverlap
 *          description. In Ring Stack the start and end position might point
 *          any index withing stack, but they cannot be more than capacity value.
 *
 *          Whether the ring stack is thread safe or not, depends on the
 *          instance of synchronization object passed to ring stack. There
 *          are 2 types of rings: Locking and Non-locking.
 *
 * \tparam  VALUE       The type of stored items. Either should be
 *                      primitive or should have default constructor
 *                      and valid assigning operator. Also, should be
 *                      possible to convert to type 'const VALUE&'.
 * 
 * \see     TENolockRingStack, TELockRingStack.
 **/
template <typename VALUE>
class TERingStack
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Ring Stack initialization. Gets instance of synchronization object,
     *          initial capacity value and the overlapping flag, used when
     *          ring stack is full and new element should be pushed.
     * \param   synchObject     Reference to synchronization object.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          it is required to insert new element.
     **/
    explicit TERingStack( IEResourceLock & synchObject, uint32_t initCapacity = 0, NECommon::eRingOverlap onOverlap = NECommon::eRingOverlap::StopOnOverlap );

    /**
     * \brief   Destructor. Public
     **/
    ~TERingStack( void );

protected:
    /**
     * \brief   Creates a Ring Stack object and copies elements from the given source.
     * \param   synchObject     Reference to synchronization object.
     * \param   source          The source of Ring Stack elements.
     **/
    explicit TERingStack(IEResourceLock& synchObject, const TERingStack<VALUE>& source);

    /**
     * \brief   Creates a Ring Stack object and moves elements from the given source.
     * \param   synchObject     Reference to synchronization object.
     * \param   source          The source of Ring Stack elements.
     **/
    explicit TERingStack(IEResourceLock& synchObject, TERingStack<VALUE> && source) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies ring stack entries from given source.
     *          If capacity of destination stack is smaller, it will be enlarged.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    TERingStack<VALUE> & operator = ( const TERingStack<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from given source.
     *          It as well swaps all entries of source and destination, but
     *          does not change the overlapping type and synchronization object.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    TERingStack<VALUE>& operator = (TERingStack<VALUE> && source) noexcept;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are equal.
     **/
    bool operator == (const TERingStack<VALUE>& other) const;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are not equal.
     **/
    bool operator != (const TERingStack<VALUE>& other) const;

    const VALUE& operator [] (uint32_t index) const;
    VALUE& operator [] (uint32_t index);

/************************************************************************/
// Friend global operators to make Stack streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream Ring Stack values. If Ring Stack previously
     *          had values, they will be lost. If capacity of Ring Stack is smaller than
     *          the number of serialized elements in streaming object, it will be enlarged.
     *          The values in the Ring Stack will be initialized in the same sequence
     *          as they were written.
     *          There should be possibility to initialize values from streaming object and
     *          if VALUE is not a primitive, but an object, it should have implemented streaming operator.
     *
     * \param   stream  The streaming object for reading values
     * \param   input   The Ring Stack object to save initialized values.
     **/
    template<typename V>
    friend const IEInStream & operator >> ( const IEInStream & stream, TERingStack<V> & input );
    /**
     * \brief   Writes to the stream Ring Stack values. The values are written into the stream
     *          starting from head position.
     *          There should be possibility to stream every value of Stack and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     *
     * \param   stream  The streaming object to write values
     * \param   output  The Stack object to read out values.
     **/
    template<typename V>
    friend IEOutStream & operator << ( IEOutStream & stream, const TERingStack<V> & output );

//////////////////////////////////////////////////////////////////////////
// Operations and Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns number of elements saved in stack.
     **/
    inline uint32_t getSize( void ) const;

    /**
     * \brief   Returns true if Ring Stack is empty
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns the overlapping type of the Ring Stack
     **/
    inline NECommon::eRingOverlap getOverlap(void) const;

    /**
     * \brief   Locks stack that methods can be accessed only from locking thread.
     *          In case if NolockSynchObject is used, no locking will happen,
     *          the function will return immediately and thread will continue to run.
     * \return  Returns true if stack successfully locked
     **/
    inline bool lock( void ) const;

    /**
     * \brief   If stack previously was locked by thread, it will unlock stack
     *          In case if NolockSynchObject is used, nothing will happen.
     * \return  Returns true if stack successfully unlocked
     **/
    inline bool unlock( void ) const;

    /**
     * \brief   Returns capacity value of ring stack
     **/
    inline uint32_t capacity( void ) const;

    /**
     * \brief   Returns true if Ring Stack is full
     **/
    inline bool isFull( void ) const;

    const VALUE & getAt(uint32_t index) const;
    VALUE& getAt(uint32_t index);

    void setAt(uint32_t index, const VALUE& newValue);

    /**
     * \brief   Pushes new element at the end of Ring Stack
     *          If Ring Stack is full, the operation depends on
     *          overlapping flag:
     *          1.  If overlap flag is StopOnOverlap, the element will not be set.
     *          2.  If overlap flag is ShiftOnOverlap, the element will be set at the tail of stack,
     *              but the size of Ring Stack will not be changed. The element on head of stack 
     *              will be lost.
     *          3.  If overlap flag is ResizeOnOvelap, it will resize ring stack
     *              by increasing capacity twice. If capacity was zero, it will set to 2.
     * \param   newElement  New element to set at the end of Ring Stack.
     * \return  Returns size of stack.
     **/
    uint32_t pushLast( const VALUE& newElement );

    /**
     * \brief   Sets element at the head of ring stack that on pop call, the element will be
     *          popped first. If Ring stack is full, the operation depends on overlapping flag:
     *          1.  If overlap flag is StopOnOverlap, the element will not be set.
     *          2.  If overlap flag is ShiftOnOverlap, the element will be set at the head of stack,
     *              but the size of Ring Stack will not be changed. The element on tail of stack
     *              will be lost.
     *          3.  If overlap flag is ResizeOnOvelap, it will resize ring stack
     *              by increasing capacity twice. If capacity was zero, it will set to 2.
     * \param   newElement  New element to set at the head of stack.
     * \return  Returns size of stack.
     **/
    uint32_t pushFirst( const VALUE& newElement );

    /**
     * \brief   Removes element from head and returns value, decreases number of element by one.
     *          The stack should not be empty when method is called.
     * \return  Returns value of remove element.
     **/
    VALUE popFirst( void );

    /**
     * \brief   Removes all elements from Ring stack and makes it empty.
     *          The capacity of stack remains unchanged. The change capacity value, resize stack.
     **/
    inline void clear( void );

    /**
     * \brief   Clears the ring stack, deletes the list and sets capacity zero.
     **/
    void discard( void );

    /**
     * \brief   Copies elements from given source. The elements will be copied at the end of stack.
     *          If capacity of stack is small to set copy all elements, the results depends on 
     *          overlapping flag of stack:
     *          1.  If overlap flag is StopOnOverlap, the elements will be copied until the stack is not full.
     *              When stack is full, no more elements will be copied.
     *          2.  If overlap flag is ShiftOnOverlap, the element will be copied until the stack is not full.
     *              Then the elements will be set by removing head of stack until all elements from given source
     *              are not copied. The capacity of stack will remain unchanged. If during copying stack is full,
     *              the elements at head are lost.
     *          3.  If overlap flag is ResizeOnOvelap and if elements in source are bigger than capacity of stack,
     *              the capacity of stack will be increased that no elements are lost and all elements from source
     *              are copied. No data will be lost.
     * \param   source  The source of Ring stack to get elements.
     * \return  Returns number of elements copied in to the stack. The number of copied elements and elements in stack
     *          might differ depending on overlapping flag.
     **/
    uint32_t add( const TERingStack<VALUE> & source );

    /**
     * \brief   Reserves the space of given capacity entries. If new capacity is not more than the existing,
     *          the operation is ignored and the stack is not resized. Otherwise, the size of the stack is changed
     *          and all existing elements are copied.
     * \param   newCapacity     New capacity to set for Ring Stack.
     * \return  Returns capacity size of resized ring stack.
     **/
    uint32_t reserve( uint32_t newCapacity );

    /**
     * \brief   Searches element in the stack starting at given position (index).
     *          The given position should be valid or equal to NECommon::RING_START_POSITION
     *          to search at the beginning of stack.
     * \param   elem        The value to search in the stack.
     * \param   startAt     The starting position to search. It will start to search 
     *                      from beginning if equal to NECommon::RING_START_POSITION.
     * \return  If found element, returns valid position (index).
     *          Otherwise, it returns NECommon::INVALID_INDEX.
     **/
    uint32_t find(const VALUE& elem, uint32_t startAt = NECommon::RING_START_POSITION) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   The instance of synchronization object to be used to make object thread-safe.
     **/
    IEResourceLock &                mSynchObject;

    /**
     * \brief   The overlapping flag. Set when stack is initialized and cannot be changed anymore.
     **/
    const NECommon::eRingOverlap    mOnOverlap;

    /**
     * \brief   The array of element in stack.
     **/
    VALUE *                         mStackList;

    /**
     * \brief   The number of elements in the stack.
     **/
    uint32_t                        mElemCount;

    /**
     * \brief   The capacity value of stack.
     **/
    uint32_t                        mCapacity;

    /**
     * \brief   The index of head element in array of stack
     **/
    uint32_t                        mStartPosition;

    /**
     * \brief   The index of tail element in array of stack
     **/
    uint32_t                        mLastPosition;

//////////////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Empties stack without locking object.
     *          The capacity of stack remain unchanged. All elements will be lost.
     *          Need to have it to call from destructor.
     **/
    inline void _emptyStack( void );

    inline uint32_t _normIndex(uint32_t index) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TERingStack( void ) = delete;
    TERingStack( const TERingStack<VALUE> & /*src*/ ) = delete;
    TERingStack( TERingStack<VALUE> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// TELockRingStack<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Thread safe FIFO ring stack class template declaration.
 *              In this class data access is synchronized. Use this object 
 *              if elements of ring stack are accessed by more than one thread.
 *
 * \tparam  VALUE       The type of stored elements. Either should be
 *                      primitive or should have default constructor
 *                      and valid assigning operator. Also, should be
 *                      possible to convert to type const VALUE&.
 **/
template <typename VALUE> 
class TELockRingStack  : public TERingStack<VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialized locking ring stack, sets initial capacity of 
     *          the stack and sets overlapping flag.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          pushing new element is required.
     **/
    explicit TELockRingStack(uint32_t initCapacity = 0, NECommon::eRingOverlap onOverlap = NECommon::eRingOverlap::StopOnOverlap );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TELockRingStack( const TELockRingStack<VALUE> & source );
    TELockRingStack( const TERingStack<VALUE> & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source to move data.
     **/
    TELockRingStack(TELockRingStack<VALUE>&& source) noexcept;
    TELockRingStack( TERingStack<VALUE> && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TELockRingStack( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies ring stack entries from given source.
     *          If capacity of destination stack is smaller, it will be enlarged.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    inline TELockRingStack<VALUE> & operator = ( const TELockRingStack<VALUE> & source );
    inline TELockRingStack<VALUE> & operator = ( const TERingStack<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from given source.
     *          It as well swaps all entries of source and destination, but
     *          does not change the overlapping type and synchronization object.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    inline TELockRingStack<VALUE>& operator = (TELockRingStack<VALUE> && source) noexcept;
    inline TELockRingStack<VALUE>& operator = (TERingStack<VALUE> && source) noexcept;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are equal.
     **/
    inline bool operator == (const TERingStack<VALUE>& other) const;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are not equal.
     **/
    inline bool operator != (const TERingStack<VALUE>& other) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of ResourceLock to synchronize data access
     **/
    ResourceLock    mLock;
};

//////////////////////////////////////////////////////////////////////////
// TENolockRingStack<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   No blocking FIFO ring stack class template declaration.
 *          No data access synchronization is performed in this class.
 *          It is faster than blocking stack and not thread safe.
 *          Use this ring stack if elements are accessed only by one thread.
 *
 * \tparam  VALUE       The type of stored items. Either should be
 *                      primitive or should have default constructor
 *                      and valid assigning operator. Also, should be
 *                      possible to convert to type const VALUE&.
 **/
template <typename VALUE> 
class TENolockRingStack    : public TERingStack<VALUE>
{
public:
    /**
     * \brief   Initializes non-blocking ring stack, sets initial capacity of 
     *          the stack and sets overlapping flag.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          pushing new element is required.
     **/
    TENolockRingStack(uint32_t initCapacity = 0, NECommon::eRingOverlap onOverlap = NECommon::eRingOverlap::StopOnOverlap );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TENolockRingStack( const TENolockRingStack<VALUE> & source );
    TENolockRingStack( const TERingStack<VALUE> & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source to move data.
     **/
    TENolockRingStack(TENolockRingStack<VALUE> && source ) noexcept;
    TENolockRingStack( TERingStack<VALUE> && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TENolockRingStack( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies ring stack entries from given source.
     *          If capacity of destination stack is smaller, it will be enlarged.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    inline TENolockRingStack<VALUE> & operator = ( const TENolockRingStack<VALUE> & source );
    inline TENolockRingStack<VALUE> & operator = ( const TERingStack<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from given source.
     *          It as well swaps all entries of source and destination, but
     *          does not change the overlapping type and synchronization object.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    inline TENolockRingStack<VALUE>& operator = (TERingStack<VALUE> && source) noexcept;
    inline TENolockRingStack<VALUE>& operator = (TENolockRingStack<VALUE> && source) noexcept;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are equal.
     **/
    inline bool operator == (const TERingStack<VALUE>& other) const;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are not equal.
     **/
    inline bool operator != (const TERingStack<VALUE>& other) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Synchronization object simulation.
     **/
    NolockSynchObject mNoLock;
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TERingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TERingStack<VALUE>::TERingStack( IEResourceLock & synchObject, uint32_t initCapacity /*= 0*/, NECommon::eRingOverlap onOverlap /*= NECommon::eRingOverlap::StopOnOverlap*/ )
    : mSynchObject  ( synchObject )
    , mOnOverlap    ( onOverlap )
    , mStackList    ( initCapacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW unsigned char[initCapacity * sizeof(VALUE)]) : nullptr )
    , mElemCount    ( 0 )
    , mCapacity     ( mStackList != nullptr ? initCapacity : 0 )
    , mStartPosition( 0 )
    , mLastPosition ( 0 )
{
}

template <typename VALUE>
TERingStack<VALUE>::TERingStack(IEResourceLock& synchObject, const TERingStack<VALUE> & source)
    : mSynchObject  (synchObject)
    , mOnOverlap    (source.mOnOverlap)
    , mStackList    (source.mCapacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW unsigned char[source.mCapacity * sizeof(VALUE)]) : nullptr)
    , mElemCount    (0)
    , mCapacity     (mStackList != nullptr ? source.mCapacity : 0)
    , mStartPosition(0)
    , mLastPosition (0)
{
    if (mStackList != nullptr)
    {
        VALUE * list = NEMemory::constructElems<VALUE>(mStackList, source.mElemCount);
        NEMemory::copyElems<VALUE>(list, static_cast<const VALUE*>(source.mStackList), mCapacity);
        mElemCount      = source.mElemCount;
        mStartPosition  = source.mStartPosition;
        mLastPosition   = source.mLastPosition;
    }
}

template <typename VALUE>
TERingStack<VALUE>::TERingStack(IEResourceLock& synchObject, TERingStack<VALUE> && source) noexcept
    : mSynchObject  (synchObject)
    , mOnOverlap    (source.mOnOverlap)
    , mStackList    (source.mStackList)
    , mElemCount    (source.mElemCount)
    , mCapacity     (source.mCapacity)
    , mStartPosition(source.mStartPosition)
    , mLastPosition (source.mLastPosition)
{
    source.mStackList   = nullptr;
    source.mCapacity    = 0;
    source.mElemCount   = 0;
    source.mLastPosition= 0;
    source.mStartPosition=0;
}

template <typename VALUE>
TERingStack<VALUE>::~TERingStack( void )
{
    _emptyStack();
    delete[] reinterpret_cast<unsigned char*>(mStackList);
    mStackList = nullptr;
    mCapacity = 0;
}

template <typename VALUE>
TERingStack<VALUE> & TERingStack<VALUE>::operator = ( const TERingStack<VALUE> & source )
{
    if (static_cast<const TERingStack<VALUE> *>(this) != &source)
    {
        Lock lock(mSynchObject);

        _emptyStack();
        source.lock();

        reserve(static_cast<uint32_t>(source.mElemCount));
        uint32_t pos = source.mStartPosition;
        for (uint32_t i = 0; i < source.mElemCount; ++ i )
        {
            static_cast<void>(pushLast( source.mStackList[pos] ));
            pos = ( pos + 1 ) % source.mCapacity;
        }

        source.unlock();
    }

    return (*this);
}

template <typename VALUE>
TERingStack<VALUE>& TERingStack<VALUE>::operator = (TERingStack<VALUE> && source) noexcept
{
    if (static_cast<TERingStack<VALUE> *>(this) != &source)
    {
        Lock lock1(mSynchObject);
        Lock lock2(source.mSynchObject);

        std::swap(mStackList, source.mStackList);
        std::swap(mElemCount, source.mElemCount);
        std::swap(mCapacity, source.mCapacity);
        std::swap(mStartPosition, source.mStartPosition);
        std::swap(mLastPosition, source.mLastPosition);
    }

    return (*this);
}

template <typename VALUE>
bool TERingStack<VALUE>::operator == (const TERingStack<VALUE>& other) const
{
    Lock lock1(mSynchObject);
    Lock lock2(other.mSynchObject);

    bool result{ static_cast<const TERingStack<VALUE> *>(this) == &other };

    if ((static_cast<const TERingStack<VALUE> *>(this) != &other) && (this->mElemCount == other.mElemCount))
    {
        uint32_t thisPos = mStartPosition;
        uint32_t otherPos = other.mStartPosition;

        result = true;
        for (uint32_t i = 0; i < mElemCount; ++i)
        {
            if (mStackList[thisPos] != other.mStackList[otherPos])
            {
                result = false;
                break;
            }

            thisPos = (thisPos + 1) % mCapacity;
            otherPos = (otherPos + 1) % other.mCapacity;
        }
    }

    return result;
}

template <typename VALUE>
bool TERingStack<VALUE>::operator != (const TERingStack<VALUE>& other) const
{
    Lock lock1(mSynchObject);
    Lock lock2(other.mSynchObject);

    bool result{ (static_cast<const TERingStack<VALUE> *>(this) != &other) || (this->mElemCount != other.mElemCount)};

    if ((static_cast<const TERingStack<VALUE> *>(this) != &other) && (this->mElemCount == other.mElemCount))
    {
        uint32_t thisPos = mStartPosition;
        uint32_t otherPos = other.mStartPosition;

        result = false;
        for (uint32_t i = 0; i < mElemCount; ++i)
        {
            if (mStackList[thisPos] != other.mStackList[otherPos])
            {
                result = true;
                break;
            }

            thisPos = (thisPos + 1) % mCapacity;
            otherPos = (otherPos + 1) % other.mCapacity;
        }
    }

    return result;
}

template <typename VALUE>
const VALUE& TERingStack<VALUE>::operator [] (uint32_t index) const
{
    return getAt(index);
}

template <typename VALUE>
VALUE& TERingStack<VALUE>::operator [] (uint32_t index)
{
    return getAt(index);
}

template <typename VALUE>
inline uint32_t TERingStack<VALUE>::getSize( void ) const
{
    Lock lock( mSynchObject );
    return mElemCount;
}

template <typename VALUE>
inline bool TERingStack<VALUE>::isEmpty( void ) const
{
    Lock lock( mSynchObject );
    return (mElemCount == 0);
}

template <typename VALUE>
inline NECommon::eRingOverlap TERingStack<VALUE>::getOverlap(void) const
{
    return mOnOverlap;
}

template <typename VALUE>
inline bool TERingStack<VALUE>::lock( void ) const
{
    return mSynchObject.lock(NECommon::WAIT_INFINITE);
}

template <typename VALUE>
inline bool TERingStack<VALUE>::unlock( void ) const
{
    return mSynchObject.unlock();
}

template <typename VALUE>
inline uint32_t TERingStack<VALUE>::capacity( void ) const
{
    Lock lock(mSynchObject);
    return mCapacity;
}

template <typename VALUE>
inline bool TERingStack<VALUE>::isFull( void ) const
{
    Lock lock(mSynchObject);
    return (mElemCount == mCapacity);
}

template <typename VALUE>
const VALUE& TERingStack<VALUE>::getAt(uint32_t index) const
{
    Lock lock(mSynchObject);
    ASSERT(index < mElemCount);
    index = _normIndex(index);
    return mStackList[index];
}

template <typename VALUE>
VALUE& TERingStack<VALUE>::getAt(uint32_t index)
{
    Lock lock(mSynchObject);
    ASSERT(index < mElemCount);
    index = _normIndex(index);
    return mStackList[index];
}

template <typename VALUE>
void TERingStack<VALUE>::setAt(uint32_t index, const VALUE& newValue)
{
    Lock lock(mSynchObject);
    ASSERT(index < mElemCount);
    index = _normIndex(index);
    mStackList[index] = newValue;
}

template <typename VALUE>
void TERingStack<VALUE>::clear( void )
{
    Lock lock(mSynchObject);
    _emptyStack();
}

template<typename VALUE>
void TERingStack<VALUE>::discard(void)
{
    Lock lock(mSynchObject);
    _emptyStack();
    delete[] reinterpret_cast<unsigned char*>(mStackList);
    mStackList = nullptr;
    mCapacity = 0;
}

template <typename VALUE>
uint32_t TERingStack<VALUE>::pushLast( const VALUE& newElement )
{
    Lock lock(mSynchObject);

    if ( mElemCount < mCapacity )
    {
        ASSERT( (mStartPosition != mLastPosition) || (mElemCount == 0) );

        VALUE * block = mStackList + mLastPosition;
        NEMemory::constructElems<VALUE>(block, 1);
        *block = newElement;
        mLastPosition = (mLastPosition + 1) % mCapacity;
        ++ mElemCount;
    }
    else
    {
        switch ( mOnOverlap )
        {
        case NECommon::eRingOverlap::ShiftOnOverlap:
            ASSERT(mLastPosition == mStartPosition);
            if (mCapacity != 0)
            {
                ASSERT(mStackList != nullptr);
                VALUE* block = mStackList + mLastPosition;
                NEMemory::destroyElems<VALUE>(block, 1);
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                mLastPosition = (mLastPosition + 1) % mCapacity;
                mStartPosition = mLastPosition;
            }
            else
            {
                ASSERT(mStackList == nullptr);
                ASSERT(mElemCount == 0);
            }
            break;

        case NECommon::eRingOverlap::ResizeOnOvelap:
            if ( reserve(static_cast<uint32_t>(mCapacity != 0 ? mCapacity : 1) * 2) >= (mElemCount + 1) )
            {
                ASSERT(mCapacity >= mElemCount + 1);
                VALUE * block = mStackList + mLastPosition;
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                mLastPosition = (mLastPosition + 1) % mCapacity;
                ++ mElemCount;
            }
            break;

        case NECommon::eRingOverlap::StopOnOverlap:
            OUTPUT_WARN("The new element is not set in Ring Stack, there is no more free space for new element");
            break;  // do nothing

        default:
            OUTPUT_ERR("Invalid Overlap action!");
            ASSERT(false);
            break;
        }
    }

    return mElemCount;
}

template <typename VALUE>
uint32_t TERingStack<VALUE>::pushFirst( const VALUE& newElement )
{
    Lock lock(mSynchObject);

    if ( (mElemCount + 1) <= mCapacity )
    {
        ASSERT( (mStartPosition != mLastPosition) || (mElemCount == 0) );

        mStartPosition = mStartPosition != 0 ? mStartPosition - 1 : mCapacity - 1;
        VALUE * block = mStackList + mStartPosition;
        NEMemory::constructElems<VALUE>(block, 1);
        *block = newElement;
        ++ mElemCount;
    }
    else
    {
        switch ( mOnOverlap )
        {
        case NECommon::eRingOverlap::ShiftOnOverlap:
            ASSERT( mLastPosition == mStartPosition );
            if ( mCapacity != 0 )
            {
                ASSERT(mStackList != nullptr);
                mStartPosition = mStartPosition != 0 ? mStartPosition - 1 : mCapacity - 1;
                VALUE * block = mStackList + mStartPosition;
                NEMemory::destroyElems<VALUE>(block, 1);
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                mLastPosition = mStartPosition;
            }
            else
            {
                OUTPUT_WARN("The Ring Stack is not initialized, ignoring operation!");
                ASSERT(mStackList == nullptr);
                ASSERT(mElemCount == 0);
            }
            break;

        case NECommon::eRingOverlap::ResizeOnOvelap:
            if ( reserve(static_cast<uint32_t>(mCapacity != 0 ? mCapacity : 1) * 2) >= (mElemCount + 1) )
            {
                ASSERT(mCapacity >= mElemCount + 1);
                mStartPosition = mStartPosition != 0 ? mStartPosition - 1 : mCapacity - 1;
                VALUE * block = mStackList + mStartPosition;
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                ++ mElemCount;
            }
            break;

        case NECommon::eRingOverlap::StopOnOverlap:
            OUTPUT_WARN("The new element is not set in Ring Stack, there is no more free space for new element");
            break;  // do nothing

        default:
            OUTPUT_ERR("Invalid Overlap action!");
            ASSERT(false);
            break;
        }
    }

    return mElemCount;
}

template <typename VALUE>
VALUE TERingStack<VALUE>::popFirst( void )
{
    Lock lock(mSynchObject);
    ASSERT( isEmpty() == false );
    VALUE result{ };

    if ( mElemCount != 0 )
    {
        ASSERT( mCapacity != 0 );
        ASSERT( mStackList != nullptr );

        result = mStackList[mStartPosition];
        NEMemory::destroyElems<VALUE>( mStackList + mStartPosition, 1 );
        mStartPosition = (mStartPosition + 1) % mCapacity;
        -- mElemCount;
    }

    return result;
}

template <typename VALUE>
uint32_t TERingStack<VALUE>::add( const TERingStack<VALUE> & source )
{
    Lock lock(mSynchObject);
    uint32_t result = 0;
    if ( static_cast<const TERingStack<VALUE> *>(this) != &source )
    {
        Lock lock2(source.mSynchObject);

        uint32_t srcStart = source.mStartPosition;
        if ( (mCapacity - mElemCount) >= source.mElemCount )
        {
            for (uint32_t i = 0; i < source.mElemCount; ++ i, ++ result )
            {
                VALUE * block = mStackList + mLastPosition;
                NEMemory::constructElems<VALUE>(block, 1);
                *block          = source.mStackList[srcStart];
                mLastPosition   = (mLastPosition + 1) % mCapacity;
                srcStart        = ( srcStart + 1 ) % source.mCapacity;
                ++ mElemCount;
            }
        }
        else
        {
            switch ( mOnOverlap )
            {
            case NECommon::eRingOverlap::ShiftOnOverlap:
                ASSERT( mLastPosition == mStartPosition );
                if ( mCapacity != 0 )
                {
                    for (uint32_t i = 0; i < source.mElemCount; ++ i, ++ result )
                    {
                        VALUE * block = mStackList + mLastPosition;
                        NEMemory::destroyElems<VALUE>(block, 1);
                        NEMemory::constructElems<VALUE>(block, 1);
                        *block          = source.mStackList[srcStart];
                        mLastPosition   = (mLastPosition + 1) % mCapacity;
                        srcStart        = ( srcStart + 1 ) % source.mCapacity;
                        mStartPosition  = mLastPosition;
                    }
                }
                else
                {
                    OUTPUT_WARN("The Ring Stack is not initialized, ignoring operation!");
                    ASSERT(mStackList == nullptr);
                    ASSERT(mElemCount == 0);
                }
                break;

            case NECommon::eRingOverlap::ResizeOnOvelap:
                if ( reserve( static_cast<uint32_t>(mCapacity != 0 ? mCapacity : 1) * 2 ) > (mElemCount + 1 ))
                {
                    ASSERT(mCapacity >= (mElemCount + 1));
                    for (uint32_t i = 0; i < source.mElemCount; ++ i, ++ result )
                    {
                        VALUE * block = mStackList + mLastPosition;
                        NEMemory::constructElems<VALUE>(block, 1);
                        *block = source.mStackList[srcStart];
                        mLastPosition   = (mLastPosition + 1) % mCapacity;
                        srcStart        = ( srcStart + 1 ) % source.mCapacity;
                        ++ mElemCount;
                    }
                }
                break;

            case NECommon::eRingOverlap::StopOnOverlap:
                OUTPUT_WARN("The new element is not set in Ring Stack, there is no more free space for new element");
                break;  // do nothing

            default:
                OUTPUT_ERR("Invalid Overlap action!");
                ASSERT(false);
                break;
            }
        }
    }
    else
    {
        result = mElemCount;
    }

    return result;
}

template <typename VALUE>
uint32_t TERingStack<VALUE>::reserve(uint32_t newCapacity )
{
    Lock lock(mSynchObject);

    if ( newCapacity > mCapacity )
    {
        VALUE * newList     = newCapacity != 0 ? reinterpret_cast<VALUE *>( DEBUG_NEW unsigned char[ newCapacity * sizeof(VALUE)] ) : nullptr;
        if (newList != nullptr)
        {
            uint32_t posStart = mStartPosition;
            uint32_t elemCount = mElemCount;
            for (uint32_t i = 0; i < elemCount; ++i)
            {
                VALUE* elem = newList + i;
                NEMemory::constructElems<VALUE>(static_cast<void*>(elem), 1);
                *elem = mStackList[posStart];
                posStart = (posStart + 1) % mCapacity;
            }

            _emptyStack();
            delete[] reinterpret_cast<unsigned char*>(mStackList);
            mStackList      = newList;
            mStartPosition  = 0;
            mElemCount      = elemCount;
            mLastPosition   = elemCount % newCapacity;
            mCapacity       = newCapacity;
        }
    }

    return mCapacity;
}

template <typename VALUE>
uint32_t TERingStack<VALUE>::find(const VALUE& elem, uint32_t startAt /*= NECommon::RING_START_POSITION*/) const
{
    Lock lock(mSynchObject);

    uint32_t result = NECommon::INVALID_INDEX;
    uint32_t pos = startAt == NECommon::RING_START_POSITION ? mStartPosition : startAt + 1;
    if (pos <= mLastPosition)
    {
        while (pos <= mLastPosition)
        {
            if (elem == mStackList[pos])
            {
                result = pos;
                break;
            }

            ++ pos;
        }
    }
    else if (mOnOverlap == NECommon::eRingOverlap::ShiftOnOverlap)
    {
        while (pos < mElemCount)
        {
            if (elem == mStackList[pos])
            {
                result = pos;
                break;
            }

            ++ pos;
        }

        if (result == NECommon::INVALID_INDEX)
        {
            pos = 0;
            while (pos <= mLastPosition)
            {
                if (elem == mStackList[pos])
                {
                    result = pos;
                    break;
                }

                ++ pos;
            }
        }
    }

    return result;
}

template <typename VALUE>
inline void TERingStack<VALUE>::_emptyStack( void )
{
    // do not delete stack, only remove elements and reset data.
    // keep capacity same
    for (uint32_t i = 0; i < mElemCount; ++ i)
    {
        NEMemory::destroyElems<VALUE>( mStackList + mStartPosition, 1 );
        mStartPosition = ( mStartPosition + 1 ) % mCapacity;
    }

    mStartPosition  = 0;
    mLastPosition   = 0;
    mElemCount      = 0;
}

template <typename VALUE>
inline uint32_t TERingStack<VALUE>::_normIndex(uint32_t index) const
{
    return ((mStartPosition + index) % mCapacity);
}

//////////////////////////////////////////////////////////////////////////
// TELockRingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack(uint32_t initCapacity /*= 0*/, NECommon::eRingOverlap onOverlap /*= NECommon::eRingOverlap::StopOnOverlap*/ )
    : TERingStack<VALUE>    ( mLock, initCapacity, onOverlap )
    , mLock ( false )
{
}

template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack( const TELockRingStack<VALUE> & source )
    : TERingStack<VALUE>    ( mLock, static_cast<const TERingStack<VALUE> &>(source) )
    , mLock ( false )
{
}

template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack( const TERingStack<VALUE> & source )
    : TERingStack<VALUE>    ( mLock, source )
    , mLock ( false )
{
}

template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack( TERingStack<VALUE> && source ) noexcept
    : TERingStack<VALUE>    ( mLock, std::move(source) )
    , mLock ( false )
{
}

template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack(TELockRingStack<VALUE> && source ) noexcept
    : TERingStack<VALUE>    ( mLock, std::move(static_cast<TERingStack<VALUE> &&>(source)) )
    , mLock ( false )
{
}

template <typename VALUE>
inline TELockRingStack<VALUE> & TELockRingStack<VALUE>::operator = ( const TELockRingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE> &>(*this).operator = (static_cast<const TERingStack<VALUE> &>(source));
    return (*this);
}

template <typename VALUE>
inline TELockRingStack<VALUE> & TELockRingStack<VALUE>::operator = ( const TERingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (static_cast<const TERingStack<VALUE>&>(source));
    return (*this);
}

template <typename VALUE>
inline TELockRingStack<VALUE>& TELockRingStack<VALUE>::operator = (TELockRingStack<VALUE>&& source) noexcept
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (std::move(static_cast<TERingStack<VALUE> &&>(source)));
    return (*this);
}

template <typename VALUE>
inline TELockRingStack<VALUE>& TELockRingStack<VALUE>::operator = (TERingStack<VALUE>&& source) noexcept
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (std::move(static_cast<TERingStack<VALUE>&&>(source)));
    return (*this);
}

template <typename VALUE>
inline bool TELockRingStack<VALUE>::operator == (const TERingStack<VALUE>& other) const
{
    return static_cast<const TERingStack<VALUE>&>(*this).operator == (other);
}

template <typename VALUE>
inline bool TELockRingStack<VALUE>::operator != (const TERingStack<VALUE>& other) const
{
    return static_cast<const TERingStack<VALUE>&>(*this).operator != (other);
}

//////////////////////////////////////////////////////////////////////////
// TENolockRingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack(uint32_t initCapacity /*= 0*/, NECommon::eRingOverlap onOverlap /*= NECommon::eRingOverlap::StopOnOverlap*/ )
    : TERingStack<VALUE>    ( mNoLock, initCapacity, onOverlap )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack( const TERingStack<VALUE> & source )
    : TERingStack<VALUE>    ( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack( const TENolockRingStack<VALUE> & source )
    : TERingStack<VALUE>    ( mNoLock, static_cast<const TERingStack<VALUE> &>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack( TERingStack<VALUE> && source ) noexcept
    : TERingStack<VALUE>    ( mNoLock, std::move(source) )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack(TENolockRingStack<VALUE> && source ) noexcept
    : TERingStack<VALUE>    ( mNoLock, std::move(static_cast<TERingStack<VALUE> &&>(source)) )
    , mNoLock   ( )
{
}

template <typename VALUE>
inline TENolockRingStack<VALUE> & TENolockRingStack<VALUE>::operator = ( const TENolockRingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE> &>(*this).operator = (static_cast<const TERingStack<VALUE>&>(source));
    return (*this);
}

template <typename VALUE>
inline TENolockRingStack<VALUE> & TENolockRingStack<VALUE>::operator = ( const TERingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (static_cast<const TERingStack<VALUE>&>(source));
    return (*this);
}

template <typename VALUE>
inline TENolockRingStack<VALUE>& TENolockRingStack<VALUE>::operator = (TENolockRingStack<VALUE>&& source) noexcept
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (std::move(static_cast<TERingStack<VALUE> &&>(source)));
    return (*this);
}

template <typename VALUE>
inline TENolockRingStack<VALUE>& TENolockRingStack<VALUE>::operator = (TERingStack<VALUE>&& source) noexcept
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (std::move(static_cast<TERingStack<VALUE>&&>(source)));
    return (*this);
}

template <typename VALUE>
inline bool TENolockRingStack<VALUE>::operator == (const TERingStack<VALUE>& other) const
{
    return static_cast<const TERingStack<VALUE>&>(*this).operator == (other);
}

template <typename VALUE>
inline bool TENolockRingStack<VALUE>::operator != (const TERingStack<VALUE>& other) const
{
    return static_cast<const TERingStack<VALUE>&>(*this).operator!=(other);
}

//////////////////////////////////////////////////////////////////////////
// TERingStack<VALUE> class template friend operators
//////////////////////////////////////////////////////////////////////////

template <typename V>
const IEInStream & operator >> ( const IEInStream & stream, TERingStack<V> & input )
{
    Lock lock(input.mSynchObject);

    uint32_t size = 0;
    stream >> size;

    input.clear();
    input.reserve(static_cast<uint32_t>(size));

    for (uint32_t i = 0; i < size; i ++)
    {
        V newElement;
        stream >> newElement;
        static_cast<void>(input.pushLast(newElement));
    }

    return stream;
}

template <typename V>
IEOutStream & operator << ( IEOutStream & stream, const TERingStack<V> & output )
{
    Lock lock(output.mSynchObject);

    uint32_t size = output.mElemCount;
    stream << size;
    uint32_t pos = output.mStartPosition;
    for (uint32_t i = 0; i < size; i ++)
    {
        stream << output.mStackList[pos];
        pos = ( pos + 1 ) % output.mCapacity;
    }

    return stream;
}

#endif  // AREG_BASE_TERINGSTACK_HPP
