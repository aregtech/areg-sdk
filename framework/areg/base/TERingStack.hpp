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
 * \file        areg/base/TERingStack.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
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
 *                      possible to convert to type const VALUE&.
 **/
template <typename VALUE>
class TERingStack
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Ring Stack initialization. Gets instance of synchronization object,
     *          initial capacity value and the overlapping flag, used when
     *          ring stack is full and new element should be pushed.
     * \param   synchObject     Reference to synchronization object.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          it is required to insert new element.
     * \see     TENolockRingStack, TELockRingStack.
     **/
    explicit TERingStack( IEResourceLock & synchObject, int initCapacity = 0, NECommon::eRingOverlap onOverlap = NECommon::eRingOverlap::StopOnOverlap );

    /**
     * \brief   Destructor. Public
     **/
    ~TERingStack( void );

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
    TERingStack<VALUE> & operator = ( const TERingStack<VALUE> & source );

/************************************************************************/
// Friend global operators to make Stack streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream Ring Stack values.
     *          If Ring Stack previously had values, they will be lost.
     *          If capacity of Ring Stack is smaller than number of serialized
     *          elements in streaming object, it will be enlarged.
     *          The values in the Ring Stack will be initialized in the same sequence
     *          as they were written.
     *          There should be possibility to initialize values from streaming object and
     *          if VALUE is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object for reading values
     * \param   input   The Ring Stack object to save initialized values.
     **/
    template<typename VALUE>
    friend const IEInStream & operator >> ( const IEInStream & stream, TERingStack<VALUE> & input );
    /**
     * \brief   Writes to the stream Ring Stack values.
     *          The values will be written to the stream starting from head position.
     *          There should be possibility to stream every value of Stack and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Stack object to read out values.
     **/
    template<typename VALUE>
    friend IEOutStream & operator << ( IEOutStream & stream, const TERingStack<VALUE> & output );

//////////////////////////////////////////////////////////////////////////
// Operations and Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns number of elements saved in stack.
     **/
    inline int getSize( void ) const;

    /**
     * \brief   Returns true if Ring Stack is empty
     **/
    inline bool isEmpty( void ) const;

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
    inline int capacity( void ) const;

    /**
     * \brief   Returns true if Ring Stack is full
     **/
    inline bool isFull( void ) const;

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
    int pushLast( const VALUE& newElement );

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
    int pushFirst( const VALUE& newElement );

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
    int copy( const TERingStack<VALUE> & source );

    /**
     * \brief   Resizes the capacity of stack. The operation will copy saved elements if 
     *          given size is more or equal to the size of existing capacity.
     *          If new capacity is smaller, the operation will copy elements until
     *          either all elements are not copied or until stack with new capacity is not full.
     *          If stack is empty, no elements will be copied.
     *          If new capacity is zero, it will also delete complete stack.
     * \param   newCapacity     New capacity to set for Ring Stack.
     * \return  Returns capacity size of resized ring stack.
     **/
    int resize( int newCapacity );

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
    int find(const VALUE& elem, int startAt = NECommon::RING_START_POSITION) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   The instance of synchronization object to be used to make object thread-safe.
     **/
    IEResourceLock &        mSynchObject;

    /**
     * \brief   The overlapping flag. Set when stack is initialized and cannot be changed anymore.
     **/
    const NECommon::eRingOverlap    mOnOverlap;

    /**
     * \brief   The array of element in stack.
     **/
    VALUE *                 mStackList;

    /**
     * \brief   The number of elements in the stack.
     **/
    int                     mElemCount;

    /**
     * \brief   The capacity value of stack.
     **/
    int                     mCapacity;

    /**
     * \brief   The index of head element in array of stack
     **/
    int                     mStartPosition;

    /**
     * \brief   The index of tail element in array of stack
     **/
    int                     mLastPosition;

//////////////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Empties stack without locking object.
     *          The capacity of stack remain unchanged. All elements will be lost.
     *          Need to have it to call from destructor.
     **/
    void _emptyStack( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TERingStack( void ) = delete;
    TERingStack( const TERingStack<VALUE> & /*src*/ ) = delete;
    TERingStack( TERingStack<VALUE> && /*src*/ ) noexcept = delete;
    TERingStack<VALUE> & operator = ( TERingStack<VALUE> && /*src*/ ) noexcept = delete;
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
    explicit TELockRingStack( int initCapacity = 0, NECommon::eRingOverlap onOverlap = NECommon::eRingOverlap::StopOnOverlap );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TELockRingStack( const TERingStack<VALUE> & source );

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
    inline TELockRingStack<VALUE> & operator = ( const TERingStack<VALUE> & source );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of ResourceLock to synchronize data access
     **/
    ResourceLock    mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOMOVE( TELockRingStack );
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
    TENolockRingStack( int initCapacity = 0, NECommon::eRingOverlap onOverlap = NECommon::eRingOverlap::StopOnOverlap );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TENolockRingStack( const TERingStack<VALUE> & source );

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
    inline TENolockRingStack<VALUE> & operator = ( const TERingStack<VALUE> & source );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Synchronization object simulation.
     **/
    NolockSynchObject mNoLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOMOVE( TENolockRingStack );
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TERingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TERingStack<VALUE>::TERingStack( IEResourceLock & synchObject, int initCapacity /*= 0*/, NECommon::eRingOverlap onOverlap /*= NECommon::eRingOverlap::StopOnOverlap*/ )
    : mSynchObject  ( synchObject )
    , mOnOverlap    ( onOverlap )
    , mStackList    ( nullptr )
    , mElemCount    ( 0 )
    , mCapacity     ( 0 )
    , mStartPosition( 0 )
    , mLastPosition ( 0 )
{
    if ( initCapacity > 0 )
    {
        mStackList  = reinterpret_cast<VALUE *>(DEBUG_NEW unsigned char [ initCapacity * sizeof(VALUE) ]);
        mCapacity   = mStackList != nullptr ? initCapacity : 0;
    }
}

template <typename VALUE>
TERingStack<VALUE>::~TERingStack( void )
{
    _emptyStack();
    delete[] reinterpret_cast<unsigned char*>(mStackList);
    mStackList = nullptr;
}

template <typename VALUE>
TERingStack<VALUE> & TERingStack<VALUE>::operator = ( const TERingStack<VALUE> & source )
{
    if (static_cast<const TERingStack<VALUE> *>(this) != &source)
    {
        Lock lock(mSynchObject);

        _emptyStack();
        source.lock();

        if ( mCapacity < source.mElemCount )
        {
            resize( source.mElemCount );
        }

        int pos = source.mStartPosition;
        for ( int i = 0; i < source.mElemCount; ++ i )
        {
            static_cast<void>(pushLast( source.mStackList[pos] ));
            pos = ( pos + 1 ) % source.mCapacity;
        }

        source.unlock();
    }
    return (*this);
}

template <typename VALUE>
inline int TERingStack<VALUE>::getSize( void ) const
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
inline int TERingStack<VALUE>::capacity( void ) const
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
void TERingStack<VALUE>::clear( void )
{
    Lock lock(mSynchObject);
    _emptyStack();
}

template <typename VALUE>
int TERingStack<VALUE>::pushLast( const VALUE& newElement )
{
    Lock lock(mSynchObject);

    if ( mElemCount + 1 <= mCapacity )
    {
        ASSERT( mCapacity > 0 );
        ASSERT( mStartPosition != mLastPosition || mElemCount == 0 );

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
            ASSERT( mLastPosition == mStartPosition );
            if ( mCapacity > 0 )
            {
                VALUE * block = mStackList + mLastPosition;
                NEMemory::destroyElems<VALUE>(block, 1);
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                mLastPosition = (mLastPosition + 1) % mCapacity;
                mStartPosition = mLastPosition;
            }
            else
            {
                OUTPUT_ERR("The Ring Stack has no initial capacity, there is no space to insert element!");
                ASSERT(false);
            }
            break;

        case NECommon::eRingOverlap::ResizeOnOvelap:
            if ( resize( (mCapacity > 0 ? mCapacity : 1) * 2 ) > (mElemCount + 1 ))
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
int TERingStack<VALUE>::pushFirst( const VALUE& newElement )
{
    Lock lock(mSynchObject);

    if ( mElemCount + 1 <= mCapacity )
    {
        ASSERT( mCapacity > 0 );
        ASSERT( mStartPosition != mLastPosition || mElemCount == 0 );

        mStartPosition = mStartPosition > 0 ? mStartPosition - 1 : mCapacity - 1;
        VALUE * block = mStackList + mStartPosition;
        NEMemory::constructElems<VALUE>(block, 1);
        *block = newElement;
        ++ mElemCount;

        ASSERT( mStartPosition != mLastPosition || mElemCount == mCapacity );
    }
    else
    {
        switch ( mOnOverlap )
        {
        case NECommon::eRingOverlap::ShiftOnOverlap:
            ASSERT( mLastPosition == mStartPosition );
            if ( mCapacity > 0 )
            {
                mStartPosition = mStartPosition > 0 ? mStartPosition - 1 : mCapacity - 1;
                VALUE * block = mStackList + mStartPosition;
                NEMemory::destroyElems<VALUE>(block, 1);
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                mLastPosition = mStartPosition;
            }
            else
            {
                OUTPUT_ERR("The Ring Stack has no initial capacity, there is no space to insert element!");
                ASSERT(false);
            }
            break;

        case NECommon::eRingOverlap::ResizeOnOvelap:
            if ( resize( (mCapacity > 0 ? mCapacity : 1) * 2 ) > (mElemCount + 1 ))
            {
                ASSERT(mCapacity >= mElemCount + 1);
                mStartPosition = mStartPosition > 0 ? mStartPosition - 1 : mCapacity - 1;
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
    VALUE result;

    if ( mElemCount > 0 )
    {
        ASSERT( mCapacity > 0 );
        ASSERT( mStartPosition != mLastPosition );

        result = mStackList[mStartPosition];
        NEMemory::destroyElems<VALUE>( mStackList + mStartPosition, 1 );
        mStartPosition = (mStartPosition + 1) % mCapacity;
        -- mElemCount;

        ASSERT( (mStartPosition != mLastPosition) || (mElemCount == 0) );
    }
    else
    {
        OUTPUT_ERR("The Ring Stack is empty, there is element to pop");
    }
    return result;
}

template <typename VALUE>
int TERingStack<VALUE>::copy( const TERingStack<VALUE> & source )
{
    Lock lock(mSynchObject);
    int result = 0;
    if ( static_cast<const TERingStack<VALUE> *>(this) != &source )
    {
        Lock lock2(source.mSynchObject);

        int srcStart = source.mStartPosition;
        if ( mCapacity - mElemCount >= source.mElemCount )
        {
            for ( int i = 0; i < source.mElemCount; i ++, result ++ )
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
                if ( mCapacity > 0 )
                {
                    for ( int i = 0; i < source.mElemCount; i ++, result ++ )
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
                    OUTPUT_ERR("The Ring Stack has no initial capacity, there is no space to insert element!");
                    ASSERT(false);
                }
                break;

            case NECommon::eRingOverlap::ResizeOnOvelap:
                if ( resize( (mCapacity > 0 ? mCapacity : 1) * 2 ) > (mElemCount + 1 ))
                {
                    ASSERT(mCapacity >= mElemCount + 1);
                    for ( int i = 0; i < source.mElemCount; i ++, result ++ )
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
int TERingStack<VALUE>::resize( int newCapacity )
{
    Lock lock(mSynchObject);

    if ( newCapacity != mCapacity )
    {
        VALUE * newList = newCapacity > 0 ? reinterpret_cast<VALUE *>( DEBUG_NEW unsigned char[ newCapacity ] ) : nullptr;
        int dstLast     = 0;
        int srcStart    = mStartPosition;
        int elemCount   = newCapacity > mElemCount ? mElemCount : newCapacity;

        if ( newList != nullptr )
        {
            NEMemory::constructElems(newList, elemCount);
            for ( int i = 0; i < elemCount; i ++ )
            {
                newList[dstLast ++] = mStackList[srcStart];
                srcStart = ( srcStart + 1 ) % mCapacity;
            }
        }

        _emptyStack();
        if ( mStackList != nullptr )
        {
            delete[] reinterpret_cast<unsigned char *>(mStackList);
        }

        mStackList      = newList;
        mStartPosition  = 0;
        mLastPosition   = dstLast;
        mElemCount      = dstLast;
        mCapacity       = newList != nullptr ? newCapacity : 0;
    }

    return mCapacity;
}

template <typename VALUE>
int TERingStack<VALUE>::find(const VALUE& elem, int startAt /*= NECommon::RING_START_POSITION*/) const
{
    Lock lock(mSynchObject);

    int result = NECommon::INVALID_INDEX;
    int pos = startAt == NECommon::RING_START_POSITION ? mStartPosition : startAt + 1;
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
void TERingStack<VALUE>::_emptyStack( void )
{
    // do not delete stack, only remove elements and reset data.
    // keep capacity same
    if ( mStackList != nullptr )
    {
        for ( int i = 0; i < mElemCount; i ++ )
        {
            NEMemory::destroyElems<VALUE>( mStackList + mStartPosition, 1 );
            mStartPosition = ( mStartPosition + 1 ) % mCapacity;
        }
    }

    mStartPosition  = 0;
    mLastPosition   = 0;
    mElemCount      = 0;
}

//////////////////////////////////////////////////////////////////////////
// TELockRingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack( int initCapacity /*= 0*/, NECommon::eRingOverlap onOverlap /*= NECommon::eRingOverlap::StopOnOverlap*/ )
    : TERingStack<VALUE>    ( mLock, initCapacity, onOverlap )
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
inline TELockRingStack<VALUE> & TELockRingStack<VALUE>::operator = ( const TERingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE> &>(*this) = source;
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// TENolockRingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack( int initCapacity /*= 0*/, NECommon::eRingOverlap onOverlap /*= NECommon::eRingOverlap::StopOnOverlap*/ )
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
inline TENolockRingStack<VALUE> & TENolockRingStack<VALUE>::operator = ( const TERingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE> &>(*this) = source;
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// TERingStack<VALUE> class template friend operators
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
const IEInStream & operator >> ( const IEInStream & stream, TERingStack<VALUE> & input )
{
    Lock lock(input.mSynchObject);

    int size = 0;
    stream >> size;

    input.Remove();
    if ( input.capacity() < size )
        input.resize(size);

    for (int i = 0; i < size; i ++)
    {
        V newElement;
        stream >> newElement;
        static_cast<void>(input.pushLast(newElement));
    }

    return stream;
}

template <typename VALUE>
IEOutStream & operator << ( IEOutStream & stream, const TERingStack<VALUE> & output )
{
    Lock lock(output.mSynchObject);

    int size = output.mElemCount;
    stream << size;
    int pos = output.mStartPosition;
    for ( int i = 0; i < size; i ++)
    {
        stream << output.mStackList[pos];
        pos = ( pos + 1 ) % output.mCapacity;
    }

    return stream;
}
