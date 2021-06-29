#ifndef AREG_BASE_TERINGSTACK_HPP
#define AREG_BASE_TERINGSTACK_HPP
/************************************************************************
 * \file        areg/base/TERingStack.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
#include "areg/base/ESynchObjects.hpp"
#include "areg/base/ETemplateBase.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEMemory.hpp"

/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
template <typename VALUE, typename VALUE_TYPE, class Implement> class TERingStack;
    template <typename VALUE, typename VALUE_TYPE, class Implement> class TELockRingStack;
    template <typename VALUE, typename VALUE_TYPE, class Implement> class TENolockRingStack;

/************************************************************************
 * \brief   This file defines ring stack object. On the first step
 *          the stack allocates space equal to passed capacity parameter.
 *          In ring stack the position of start and end elements may
 *          start at any index. When ring is empty or full, the start
 *          and end positions are equal and only element count
 *          parameter determines whether stack is full or empty.
 *          Depending on overlapping value, the stack might change behavior
 *          when it is full. It might ignore adding new element,
 *          it might insert new element, but do not change capacity of stack,
 *          then the first entry in stack will be lost. Or it might resize 
 *          stack and add new element without loosing elements.
 *          Define overlapping type during initialization of stack.
 *          The overlapping type cannot be changed, regardless capacity
 *          might be changed on assigning or streaming operations.
 *          2 types of ring stack are defined -- locking and non-locking.
 *          The locking ring stack is thread safe and can be used in 
 *          multi-threading context. The non-locking should be used only within
 *          one thread context, it is not thread safe.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Ring stack type definition
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Overlapping operations in ring stack.
 *          Overlapping of elements in ring stack means case, when stack is
 *          full, i.e. start and end positions match and number of elements
 *          in stack is equal to stack capacity. In this case, on next request
 *          to push element, the stack may change behavior, depending on
 *          overlapping action value.
 *          1.  StopOnOverlap   --  Do not enter any new element if ring stack is full.
 *          2.  ShiftOnOverlap  --  Do not change ring stack capacity if stack is full,
 *                                  but shift start position, i.e. through out
 *                                  the very first element from ring stack and set new element.
 *          3. ResizeOnOvelap   --  Always resize stack if it is full.
 *                                  This will set new capacity of stack twice bigger
 *                                  than it was before. If initial capacity was
 *                                  zero, after resize new capacity length will be 2.
 **/
typedef enum E_OnRingOverlap
{
    /**
     * \brief   Do not copy new element, if ring stack is full
     **/
      StopOnOverlap     = 0
    /**
     * \brief   Shifts start position, if ring stack is full and does not resize stack.
     **/
    , ShiftOnOverlap    = 1
    /**
     * \brief   Resize ring stack, if stack is full. No element is lost.
     **/
    , ResizeOnOvelap    = 2

} eOnRingOverlap;

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       TERingStack<VALUE, VALUE_TYPE, Implement>
 *              Ring FIFO Stack base object to queue elements, insert and 
 *              access by push and pop operations. TCRingZStack requires
 *              instance of synchronization object to synchronize
 *              access of stack elements, capacity value and overlapping flag.
 * 
 * \details    	The Ring FIFO Stack is an array, the initialize size of array
 *              is equal to capacity parameter passed during initialization.
 *              The capacity might be changed depending on overlapping flag,
 *              passed during initialization of object. If ring stack is full,
 *              the capacity might remain unchanged and the new pushed element
 *              will be either inserted or not. Or the capacity or array might 
 *              be enlarged. In this case, new capacity will be twice more than
 *              it was before. When writing or reading position in Ring Stack
 *              reaches end of array, it starts from begging. In Ring Stack
 *              the start and end position might point at any index withing array,
 *              but they cannot be more than capacity value. If Ring Stack is empty
 *              or full, the start and end positions are having same value.
 *              The element count parameter indicates whether stack is full or empty.
 *
 * \tparam  VALUE       The type of stored items. Either should be 
 *                      primitive or should have default constructor 
 *                      and valid assigning operator. Also, should be 
 *                      possible to convert to type VALUE_TYPE.
 * \tparam  VALUE_TYPE  By default same as VALUE, but can be any other
 *                      type, which is converted from type VALUE.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEListImpl<VALUE_TYPE>>
class TERingStack   : protected Implement
                    , private   TemplateConstants
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Ring Stack initialization constructor.
     *          Receives reference to synchronization object, initial capacity value
     *          and the overlapping flag, used when ring stack is full and pushing 
     *          new element is called. The overlapping flag cannot be changed.
     *          The capacity is changed when resize() method is called.
     *          It is declared as protected to prevent direct call.
     *          Use TENolockRingStack or TELockRingStack objects instead.
     * \param   synchObject     Reference to synchronization object.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          pushing new element is required.
     **/
    TERingStack( IEResourceLock & synchObject, int initCapacity = 0, eOnRingOverlap onOverlap = StopOnOverlap );

    /**
     * \brief   Copy constructor.
     *          It is declared as protected to prevent direct call.
     *          Use TENolockRingStack or TELockRingStack objects instead.
     * \param   synchObject     The instance of synchronization object
     * \param   source          The Ring Stack source, which contains elements
     **/
    TERingStack( IEResourceLock & synchObject, const TERingStack<VALUE, VALUE_TYPE, Implement> & source );

public:
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
    const TERingStack<VALUE, VALUE_TYPE, Implement> & operator = ( const TERingStack<VALUE, VALUE_TYPE, Implement> & source );

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
    template<typename V, typename VT, class Impl>
    friend const IEInStream & operator >> ( const IEInStream & stream, TERingStack<V, VT, Impl> & input );
    /**
     * \brief   Writes to the stream Ring Stack values.
     *          The values will be written to the stream starting from head position.
     *          There should be possibility to stream every value of Stack and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Stack object to read out values.
     **/
    template<typename V, typename VT, class Impl>
    friend IEOutStream & operator << ( IEOutStream & stream, const TERingStack<V, VT, Impl> & output );

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
    int pushLast( VALUE_TYPE newElement );

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
    int pushFirst( VALUE_TYPE newElement );

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
    inline void removeAll( void );

    /**
     * \brief   Copies elements from given source. The elements will be copied at the end of stack.
     *          If capacity of stack is small to set copy all elements, the results depends on 
     *          overlapping flag of stack:
     *          1.  If overlap flag is StopOnOverlap, the elements will be copied until the stack is not full.
     *              When stack is full, no more elements will be copied.
     *          2.  If overlap flag is ShiftOnOverlap, the element will be copied until the stack is not full.
     *              Then the elements will be set by removing head of of stack until all elements from given source
     *              are not copied. The capacity of stack will remain unchanged. If during copying stack is full,
     *              the elements at head are lost.
     *          3.  If overlap flag is ResizeOnOvelap and if elements in source are bigger than capacity of stack,
     *              the capacity of stack will be increased that no elements are lost and all elements from source
     *              are copied. No data will be lost.
     * \param   source  The source of Ring stack to get elements.
     * \return  Returns number of elements copied in to the stack. The number of copied elements and elements in stack
     *          might differ depending on overlapping flag.
     **/
    int copy( const TERingStack<VALUE, VALUE_TYPE, Implement> & source );

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
     *          The given position should be valid or equal to TemplateConstants::START_INDEX
     *          to search at the beginning of stack.
     * \param   elem        The value to search in the stack.
     * \param   startAt     The starting position to search. It will start to search 
     *                      from beginning if equal to TemplateConstants::START_INDEX.
     * \return  If found element, returns valid position (index).
     *          Otherwise, it returns TemplateConstants::INVALID_INDEX.
     **/
    int find(VALUE_TYPE elem, int startAt = TemplateConstants::START_INDEX) const;

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Compares and returns true if 2 values are equal.
     *          Specify own implEqualValues() method of TEListImpl
     *          class template for custom comparing method.
     * \param   Value1  Left-side value to compare.
     * \param   Value2  Right-side value to compare.
     * \return  Returns true if 2 value are equal.
     **/
    inline bool isEqualValues(VALUE_TYPE Value1, VALUE_TYPE Value2) const;

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
    const eOnRingOverlap    mOnOverlap;

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
    TERingStack( void );
    TERingStack( const TERingStack<VALUE, VALUE_TYPE, Implement> & );
};

//////////////////////////////////////////////////////////////////////////
// TELockRingStack<VALUE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Blocking FIFO ring stack class template declaration.
 *              In this class data access is synchronized.
 *
 * \details     This blocking class template contains instance of
 *              IEResourceLock to synchronize data access by more
 *              than one thread. Any other thread trying to access elements
 *              will be blocked.
 *              Use this object if elements of ring stack are accessed more than
 *              one thread.
 *
 * \tparam  VALUE       The type of stored items. Either should be
 *                      primitive or should have default constructor
 *                      and valid assigning operator. Also, should be
 *                      possible to convert to type VALUE_TYPE.
 * \tparam  VALUE_TYPE  By default same as VALUE, but can be any other
 *                      type, which is converted from type VALUE.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEListImpl<VALUE_TYPE>> 
class TELockRingStack  : public TERingStack<VALUE, VALUE_TYPE, Implement>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. Initialized locking ring stack,
     *          sets initial capacity of stack and sets overlapping flag.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          pushing new element is required.
     **/
    TELockRingStack( int initCapacity = 0, eOnRingOverlap onOverlap = StopOnOverlap );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TELockRingStack( const TERingStack<VALUE, VALUE_TYPE, Implement> & source );

    /**
     * \brief   Destructor
     **/
    ~TELockRingStack( void );

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
    inline const TELockRingStack<VALUE, VALUE_TYPE, Implement> & operator = ( const TERingStack<VALUE, VALUE_TYPE, Implement> & source );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of Critical Section to synchronize data access
     **/
    ResourceLock  mLock;
};

//////////////////////////////////////////////////////////////////////////
// TENolockRingStack<VALUE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       No blocking FIFO ring stack class template declaration.
 *              No data access synchronization is performed in this class.
 *
 * \details     This non-blocking class template contains instance of
 *              NolockSynchObject to imitate access synchronization. 
 *              No thread will be blocked accessing ring stack elements.
 *              Use this object if there is no need to synchronize
 *              element access.
 *
 * \tparam  VALUE       The type of stored items. Either should be
 *                      primitive or should have default constructor
 *                      and valid assigning operator. Also, should be
 *                      possible to convert to type VALUE_TYPE.
 * \tparam  VALUE_TYPE  By default same as VALUE, but can be any other
 *                      type, which is converted from type VALUE.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEListImpl<VALUE_TYPE>> 
class TENolockRingStack    : public TERingStack<VALUE, VALUE_TYPE, Implement>
{
public:
    /**
     * \brief   Initialization constructor. Initialized locking ring stack,
     *          sets initial capacity of stack and sets overlapping flag.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          pushing new element is required.
     **/
    TENolockRingStack( int initCapacity = 0, eOnRingOverlap onOverlap = StopOnOverlap );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TENolockRingStack( const TERingStack<VALUE, VALUE_TYPE, Implement> & source );

    /**
     * \brief   Destructor
     **/
    ~TENolockRingStack( void );

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
    inline const TENolockRingStack<VALUE, VALUE_TYPE, Implement> & operator = ( const TERingStack<VALUE, VALUE_TYPE, Implement> & source );

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
// TERingStack<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TERingStack<VALUE, VALUE_TYPE, Implement>::TERingStack( IEResourceLock & synchObject, int initCapacity /*= 0*/, eOnRingOverlap onOverlap /*= StopOnOverlap*/ )
    : Implement             ( )
    , TemplateConstants   ( )
    
    , mSynchObject          ( synchObject )
    , mOnOverlap            ( onOverlap )
    , mStackList            ( NULL )
    , mElemCount            ( 0 )
    , mCapacity             ( 0 )
    , mStartPosition        ( 0 )
    , mLastPosition         ( 0 )
{
    if ( initCapacity > 0 )
    {
        mStackList  = reinterpret_cast<VALUE *>(DEBUG_NEW unsigned char [ initCapacity * sizeof(VALUE) ]);
        mCapacity   = mStackList != NULL ? initCapacity : 0;
    }
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TERingStack<VALUE, VALUE_TYPE, Implement>::TERingStack( IEResourceLock & synchObject, const TERingStack<VALUE, VALUE_TYPE, Implement> & source )
    : mSynchObject      ( synchObject )
    , mOnOverlap        ( source.mOnOverlap )
    , mStackList        ( NULL )
    , mElemCount        ( 0 )
    , mCapacity         ( 0 )
    , mStartPosition    ( 0 )
    , mLastPosition     ( 0 )
{
    if ( source.mCapacity > 0 )
    {
        mStackList  = reinterpret_cast<VALUE *>(DEBUG_NEW unsigned char [ source.mCapacity * sizeof(VALUE) ]);
        mCapacity   = mStackList != NULL ? source.mCapacity : 0;

        if ( mStackList != NULL )
        {
            NEMemory::constructElems<VALUE>(mStackList, source.mElemCount);
            int pos = source.mStartPosition;
            for ( mLastPosition = 0; mLastPosition < source.mElemCount; mLastPosition ++ )
            {
                mStackList[mLastPosition] = source.mStackList[pos];
                pos = ( pos + 1 ) % source.mCapacity;
            }
            
            mElemCount   = source.mElemCount;
        }
    }
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TERingStack<VALUE, VALUE_TYPE, Implement>::~TERingStack( void )
{
    _emptyStack();

    if ( mStackList != NULL )
        delete [] reinterpret_cast<unsigned char *>(mStackList);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
const TERingStack<VALUE, VALUE_TYPE, Implement> & TERingStack<VALUE, VALUE_TYPE, Implement>::operator = ( const TERingStack<VALUE, VALUE_TYPE, Implement> & source )
{
    if (static_cast<const TERingStack<VALUE, VALUE_TYPE, Implement> *>(this) != &source)
    {
        Lock lock(mSynchObject);

        _emptyStack();
        source.lock();

        if ( mCapacity < source.mElemCount )
            resize( source.mElemCount );
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

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline int TERingStack<VALUE, VALUE_TYPE, Implement>::getSize( void ) const
{
    Lock lock( mSynchObject );
    return mElemCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TERingStack<VALUE, VALUE_TYPE, Implement>::isEmpty( void ) const
{
    Lock lock( mSynchObject );
    return (mElemCount == 0);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TERingStack<VALUE, VALUE_TYPE, Implement>::lock( void ) const
{
    return mSynchObject.lock(IESynchObject::WAIT_INFINITE);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TERingStack<VALUE, VALUE_TYPE, Implement>::unlock( void ) const
{
    return mSynchObject.unlock();
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline int TERingStack<VALUE, VALUE_TYPE, Implement>::capacity( void ) const
{
    Lock lock(mSynchObject); return mCapacity;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TERingStack<VALUE, VALUE_TYPE, Implement>::isFull( void ) const
{
    Lock lock(mSynchObject);
    return (mElemCount == mCapacity);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
void TERingStack<VALUE, VALUE_TYPE, Implement>::removeAll( void )
{
    Lock lock(mSynchObject);
    _emptyStack();
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TERingStack<VALUE, VALUE_TYPE, Implement>::pushLast( VALUE_TYPE newElement )
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
        mElemCount ++;
    }
    else
    {
        switch ( mOnOverlap )
        {
        case ShiftOnOverlap:
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

        case ResizeOnOvelap:
            if ( resize( (mCapacity > 0 ? mCapacity : 1) * 2 ) > (mElemCount + 1 ))
            {
                ASSERT(mCapacity >= mElemCount + 1);
                VALUE * block = mStackList + mLastPosition;
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                mLastPosition = (mLastPosition + 1) % mCapacity;
                mElemCount ++;
            }
            break;

        case StopOnOverlap:
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

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TERingStack<VALUE, VALUE_TYPE, Implement>::pushFirst( VALUE_TYPE newElement )
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
        mElemCount ++;

        ASSERT( mStartPosition != mLastPosition || mElemCount == mCapacity );
    }
    else
    {
        switch ( mOnOverlap )
        {
        case ShiftOnOverlap:
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

        case ResizeOnOvelap:
            if ( resize( (mCapacity > 0 ? mCapacity : 1) * 2 ) > (mElemCount + 1 ))
            {
                ASSERT(mCapacity >= mElemCount + 1);
                mStartPosition = mStartPosition > 0 ? mStartPosition - 1 : mCapacity - 1;
                VALUE * block = mStackList + mStartPosition;
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                mElemCount ++;
            }
            break;

        case StopOnOverlap:
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

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
VALUE TERingStack<VALUE, VALUE_TYPE, Implement>::popFirst( void )
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

        ASSERT( mStartPosition != mLastPosition || mElemCount == 0 );
    }
    else
    {
        OUTPUT_ERR("The Ring Stack is empty, there is element to pop");
    }
    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TERingStack<VALUE, VALUE_TYPE, Implement>::copy( const TERingStack<VALUE, VALUE_TYPE, Implement> & source )
{
    Lock lock(mSynchObject);
    int result = 0;
    if ( static_cast<const TERingStack<VALUE, VALUE_TYPE, Implement> *>(this) != &source )
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
                mElemCount ++;
            }
        }
        else
        {
            switch ( mOnOverlap )
            {
            case ShiftOnOverlap:
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

            case ResizeOnOvelap:
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
                        mElemCount ++;
                    }
                }
                break;

            case StopOnOverlap:
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

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TERingStack<VALUE, VALUE_TYPE, Implement>::resize( int newCapacity )
{
    Lock lock(mSynchObject);

    if ( newCapacity != mCapacity )
    {
        VALUE * newList = newCapacity > 0 ? reinterpret_cast<VALUE *>( DEBUG_NEW unsigned char[ newCapacity ] ) : NULL;
        int dstLast     = 0;
        int srcStart    = mStartPosition;
        int elemCount   = newCapacity > mElemCount ? mElemCount : newCapacity;

        if ( newList != NULL )
        {
            NEMemory::constructElems(newList, elemCount);
            for ( int i = 0; i < elemCount; i ++ )
            {
                newList[dstLast ++] = mStackList[srcStart];
                srcStart = ( srcStart + 1 ) % mCapacity;
            }
        }

        _emptyStack();
        if ( mStackList != NULL )
            delete [] reinterpret_cast<unsigned char *>(mStackList);

        mStackList      = newList;
        mStartPosition  = 0;
        mLastPosition   = dstLast;
        mElemCount      = dstLast;
        mCapacity       = newList != NULL ? newCapacity : 0;
    }

    return mCapacity;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TERingStack<VALUE, VALUE_TYPE, Implement>::find(VALUE_TYPE elem, int startAt /*= TemplateConstants::START_INDEX*/) const
{
    Lock lock(mSynchObject);

    int result = TemplateConstants::INVALID_INDEX;
    int pos = startAt == TemplateConstants::START_INDEX ? mStartPosition : startAt + 1;
    if (pos <= mLastPosition)
    {
        while (pos <= mLastPosition)
        {
            if (isEqualValues(elem, mStackList[pos]))
            {
                result = pos;
                break;
            }

            ++ pos;
        }
    }
    else if (mOnOverlap == ShiftOnOverlap)
    {
        while (pos < mElemCount)
        {
            if (isEqualValues(elem, mStackList[pos]))
            {
                result = pos;
                break;
            }

            ++ pos;
        }

        if (result == TemplateConstants::INVALID_INDEX)
        {
            pos = 0;
            while (pos <= mLastPosition)
            {
                if (isEqualValues(elem, mStackList[pos]))
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

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TERingStack<VALUE, VALUE_TYPE, Implement>::isEqualValues(VALUE_TYPE Value1, VALUE_TYPE Value2) const
{
    return Implement::ImplEqualVales(Value1, Value2);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
void TERingStack<VALUE, VALUE_TYPE, Implement>::_emptyStack( void )
{
    // do not delete stack, only remove elements and reset data.
    // keep capacity same
    if ( mStackList != NULL )
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
// TELockRingStack<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TELockRingStack<VALUE, VALUE_TYPE, Implement>::TELockRingStack( int initCapacity /*= 0*/, eOnRingOverlap onOverlap /*= StopOnOverlap*/ )
    : TERingStack<VALUE, VALUE_TYPE, Implement>    ( mLock, initCapacity, onOverlap )
    , mLock ( false )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TELockRingStack<VALUE, VALUE_TYPE, Implement>::TELockRingStack( const TERingStack<VALUE, VALUE_TYPE, Implement> & source )
    : TERingStack<VALUE, VALUE_TYPE, Implement>    ( mLock, source )
    , mLock ( false )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TELockRingStack<VALUE, VALUE_TYPE, Implement>::~TELockRingStack( void )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline const TELockRingStack<VALUE, VALUE_TYPE, Implement> & TELockRingStack<VALUE, VALUE_TYPE, Implement>::operator = ( const TERingStack<VALUE, VALUE_TYPE, Implement> & source )
{   static_cast<TERingStack<VALUE, VALUE_TYPE, Implement> &>(*this) = source; return (*this);     }

//////////////////////////////////////////////////////////////////////////
// TENolockRingStack<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TENolockRingStack<VALUE, VALUE_TYPE, Implement>::TENolockRingStack( int initCapacity /*= 0*/, eOnRingOverlap onOverlap /*= StopOnOverlap*/ )
    : TERingStack<VALUE, VALUE_TYPE, Implement>    ( mNoLock, initCapacity, onOverlap )
    , mNoLock   ( )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TENolockRingStack<VALUE, VALUE_TYPE, Implement>::TENolockRingStack( const TERingStack<VALUE, VALUE_TYPE, Implement> & source )
    : TERingStack<VALUE, VALUE_TYPE, Implement>    ( mNoLock, source )
    , mNoLock   ( )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TENolockRingStack<VALUE, VALUE_TYPE, Implement>::~TENolockRingStack( void )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline const TENolockRingStack<VALUE, VALUE_TYPE, Implement> & TENolockRingStack<VALUE, VALUE_TYPE, Implement>::operator = ( const TERingStack<VALUE, VALUE_TYPE, Implement> & source )
{   static_cast<TERingStack<VALUE, VALUE_TYPE, Implement> &>(*this) = source; return (*this);     }

//////////////////////////////////////////////////////////////////////////
// TERingStack<VALUE, VALUE_TYPE, Implement> class template friend operators
//////////////////////////////////////////////////////////////////////////

template<typename V, typename VT, class Impl>
const IEInStream & operator >> ( const IEInStream & stream, TERingStack<V, VT, Impl> & input )
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

template<typename V, typename VT, class Impl>
IEOutStream & operator << ( IEOutStream & stream, const TERingStack<V, VT, Impl> & output )
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

#endif  // AREG_BASE_TERINGSTACK_HPP
