#ifndef AREG_BASE_RINGSTACK_HPP
#define AREG_BASE_RINGSTACK_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/RingStack.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, FIFO Stack class templates.
 *              Blocking and non-blocking FIFO Stacks, used to queue
 *              elements in the Stack without searching elements, 
 *              accessing by position or index.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TemplateBase.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include "areg/base/IOStream.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/MathDefs.hpp"

#include <algorithm>

/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
template <typename VALUE> class RingStackBase;
    template <typename VALUE> class ConcurrentRingStack;
    template <typename VALUE> class RingStack;

//////////////////////////////////////////////////////////////////////////
// StackBase<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Ring FIFO Stack base object to queue elements, insert and access
 *          by push and pop operations. RingStackBase requires instance of
 *          synchronization object to synchronize access of stack elements,
 *          capacity value and overlapping flag.
 *
 *          The capacity might be changed depending on overlapping flag.
 *          If ring stack is full, whether the capacity remains same or not,
 *          whether new element is pushed or not, depends on overlapping flag.
 *          For more details of capacity flag see NECommon::OverlapPolicy
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
 * \see     RingStack, ConcurrentRingStack.
 **/
template <typename VALUE>
class RingStackBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Ring Stack initialization. Gets instance of synchronization object,
     *          initial capacity value and the overlapping flag, used when
     *          ring stack is full and new element should be pushed.
     * \param   syncObject      Reference to synchronization object.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          it is required to insert new element.
     **/
    explicit RingStackBase( Lockable & syncObject, uint32_t initCapacity = 0, NECommon::OverlapPolicy onOverlap = NECommon::OverlapPolicy::Stop );

    /**
     * \brief   Destructor. Public
     **/
    ~RingStackBase();

protected:
    /**
     * \brief   Creates a Ring Stack object and copies elements from the given source.
     * \param   syncObject  Reference to synchronization object.
     * \param   source      The source of Ring Stack elements.
     **/
    explicit RingStackBase(Lockable& syncObject, const RingStackBase<VALUE>& source);

    /**
     * \brief   Creates a Ring Stack object and moves elements from the given source.
     * \param   syncObject  Reference to synchronization object.
     * \param   source      The source of Ring Stack elements.
     **/
    explicit RingStackBase(Lockable& syncObject, RingStackBase<VALUE> && source) noexcept;

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
    RingStackBase<VALUE> & operator = ( const RingStackBase<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from given source.
     *          It as well swaps all entries of source and destination, but
     *          does not change the overlapping type and synchronization object.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    RingStackBase<VALUE>& operator = (RingStackBase<VALUE> && source) noexcept;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are equal.
     **/
    bool operator == (const RingStackBase<VALUE>& other) const;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are not equal.
     **/
    bool operator != (const RingStackBase<VALUE>& other) const;

    /**
     * \brief   Returns element by its index. The index should not be more than the number of elements in the stack.
     * \param   index   Zero-based index of the element in the stack.
     * \return  Returns element by its index.
     **/
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
    friend const InStream & operator >> ( const InStream & stream, RingStackBase<V> & input );
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
    friend OutStream & operator << ( OutStream & stream, const RingStackBase<V> & output );

//////////////////////////////////////////////////////////////////////////
// Operations and Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns number of elements saved in stack.
     **/
    uint32_t getSize() const;

    /**
     * \brief   Returns true if Ring Stack is empty
     **/
    bool isEmpty() const;

    /**
     * \brief   Returns the overlapping type of the Ring Stack
     **/
    NECommon::OverlapPolicy getOverlap() const;

    /**
     * \brief   Locks stack that methods can be accessed only from locking thread.
     *          In case if NolockSyncObject is used, no locking will happen,
     *          the function will return immediately and thread will continue to run.
     * \return  Returns true if stack successfully locked
     **/
    bool lock() const;

    /**
     * \brief   If stack previously was locked by thread, it will unlock stack
     *          In case if NolockSyncObject is used, nothing will happen.
     * \return  Returns true if stack successfully unlocked
     **/
    bool unlock() const;

    /**
     * \brief   Returns capacity value of ring stack
     **/
    uint32_t capacity() const;

    /**
     * \brief   Returns true if Ring Stack is full. The function returns false if
     *          the ring stack is of 'resize of overlap' type, because it automatically changes the size.
     **/
    bool isFull() const;

    /**
     * \brief   Returns true if specified zero-based normalized index is valid.
     *          The index is valid if it is smaller than the number of valid entries in the ring.
     * \param   index   The index to check.
     **/
    bool isValidIndex(uint32_t index) const;

    /**
     * \brief   Returns element by its index. The index should not be more than the number of elements in the stack.
     * \param   index   Zero-based index of the element in the stack.
     * \return  Returns element by its index.
     **/
    const VALUE & getAt(uint32_t index) const;
    VALUE& getAt(uint32_t index);

    /**
     * \brief   Sets new value at the passed index. The index is zero-based and should be valid.
     * \param   index   Zero-based index of the entry to set the value.
     **/
    void setAt(uint32_t index, const VALUE& newValue);

    /**
     * \brief   Pushes new element at the end of Ring Stack
     *          If Ring Stack is full, the operation depends on
     *          overlapping flag:
     *          1.  If overlap flag is Stop, the element will not be set.
     *          2.  If overlap flag is Shift, the element will be set at the tail of stack,
     *              but the size of Ring Stack will not be changed. The element on head of stack 
     *              will be lost.
     *          3.  If overlap flag is Resize, it will resize ring stack
     *              by increasing capacity twice. If capacity was zero, it will set to 2.
     * \param   newElement  New element to set at the end of Ring Stack.
     * \return  Returns size of stack.
     **/
    uint32_t push( const VALUE& newElement );

    /**
     * \brief   Removes element from head and returns value, decreases number of element by one.
     *          The stack should not be empty when method is called.
     * \return  Returns value of remove element.
     **/
    VALUE pop();

    /**
     * \brief   Removes all elements from Ring stack and makes it empty.
     *          The capacity of stack remains unchanged. The change capacity value, resize stack.
     **/
    void clear();

    /**
     * \brief   Clears the ring stack, deletes the list and sets capacity zero.
     **/
    void release();

    /**
     * \brief   Removes the extra entries in the ring stack and makes capacity equal to the number of elements in the stack.
     **/
    void freeExtra();

    /**
     * \brief   Adds elements from given source. The elements will be copied at the end of stack.
     *          If capacity of stack is small to copy all elements from the source, the results depends on 
     *          overlapping flag of stack:
     *          1.  If overlap flag is Stop, the elements will be copied until the stack is not full.
     *              When stack is full, no more elements will be copied.
     *          2.  If overlap flag is Shift, the element will be copied until the stack is not full.
     *              Then the elements will be set by removing head of stack until all elements from given source
     *              are not copied. The capacity of stack will remain unchanged. If during copying stack is full,
     *              the elements at head are lost.
     *          3.  If overlap flag is Resize and if elements in source are bigger than capacity of stack,
     *              the capacity of stack will be increased that no elements are lost and all elements from source
     *              are copied. No data will be lost.
     * \param   source  The source of Ring stack to get elements.
     * \return  Returns number of elements copied in to the stack. The number of copied elements and elements in stack
     *          might differ depending on overlapping flag.
     **/
    uint32_t add( const RingStackBase<VALUE> & source );

    /**
     * \brief   Reserves the space of given capacity entries. If new capacity is not more than the existing,
     *          the operation is ignored and the stack is not resized. Otherwise, the size of the stack is changed
     *          and all existing elements are copied.
     * \param   newCapacity     New capacity to set for Ring Stack.
     * \return  Returns capacity size of resized ring stack.
     **/
    uint32_t reserve( uint32_t newCapacity );

    /**
     * \brief   Copies entries from the given source. The previous entries will be lost and new entries will be set.
     * \param   source  The source of entries to copy data.
     **/
    void copy(const RingStackBase<VALUE> & source);

    /**
     * \brief   Moves entries from the given source. The previous entries are swapped with the source.
     * \param   source  The source of entries to move data.
     **/
    void move(RingStackBase<VALUE> && source) noexcept;

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

    /**
     * \brief   Searches the specified element in the ring stack starting at the mentioned position
     *          and returns true if the ring-stack contains specified element. The starting position
     *          is zero-based and should not be more than the number of elements in the ring stack.
     *          If the starting position is NECommon::RING_START_POSITION, it searches at the begin of
     *          the ring stack.
     * \param   elem    The element to search.
     * \param   startAt The starting position to search. If the starting position is
     *                  NECommon::RING_START_POSITION it searches from the begin of the ring stack.
     * \return  Returns true if found an entry in the ring stack. Otherwise, returns false.
     **/
    bool contains(const VALUE& elem, uint32_t startAt = NECommon::RING_START_POSITION) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   The instance of synchronization object to be used to make object thread-safe.
     **/
    Lockable &                mSyncObj;

    /**
     * \brief   The overlapping flag. Set when stack is initialized and cannot be changed anymore.
     **/
    const NECommon::OverlapPolicy    mOnOverlap;

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
    uint32_t                        mHeadPos;

    /**
     * \brief   The index of tail element in array of stack
     **/
    uint32_t                        mTailPos;

//////////////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Empties stack without locking object. The capacity of stack remain unchanged. All elements are lost.
     **/
    void _emptyStack();

    /**
     * \brief   Copies entries from the give source `src` to the destination `dst.
     *          The destination must have enough space (capacity) to copy elements.
     * \param   dst         The destination to copy elements.
     * \param   src         The source of elements to copy data.
     * \param   srcStart    The starting position in the source of stack to copy elements.
     * \param   srcEnd      The last position in the source of stack to stop copying elements.
     * \param   srcCount    The number of elements to copy elements.
     * \param   srcCapacity The capacity of source, used to compute right position in the ring.
     **/
    void _copyElems(VALUE* dst, VALUE* src, uint32_t srcStart, uint32_t srcEnd, uint32_t srcCount, uint32_t srcCapacity);

    /**
     * \brief   Converts normalized index, such as zero-based index, into the appropriate internal index in the ring stack.
     *          For example, the index 0 is equal to the head position.
     * \param   index   The normalized index to convert.
     * \return  Returns the internal index of the ring buffer and the index cannot be more than the capacity of the ring stack.
     **/
    uint32_t _norm2RingIndex(uint32_t index) const;

    /**
     * \brief   Converts the internal ring stack index into the normalized zero-based index.
     *          For example, the head position of the ring is equal to normalized index 0.
     * \param   ring    The internal ring stack index to convert.
     * \return  Returns normalized zero-based index, which cannot be more than the number of elements in the ring stack.
     **/
    uint32_t _ring2NormIndex(uint32_t ring) const;

    /**
     * \brief   Compares 2 ring stack entries in the list and returns `Bigger`, `Equal` or `Smaller` depending on comparing results.
     * \param   left            The list of elements in the ring stack on the left side.
     * \param   leftStart       The head position in the left ring stack.
     * \param   leftCapacity    The capacity of the left ring stack.
     * \param   leftCount       The number of entries in the left ring stack.
     * \param   right           The list of elements in the ring stack on the right side.
     * \param   rightStart      The head position in the right ring stack.
     * \param   rightCapacity   The capacity of the right ring stack.
     * \param   rightCount      The number of entries in the right ring stack.
     * \return  Returns one of the values:
     *              1. NEMath::Ordering::Equal  -- if 2 lists are equal.
     *              2. NEMath::Ordering::Bigger -- if `left` stack is bigger than the `right`.
     *              2. NEMath::Ordering::Smaller-- if `left` stack is smaller than the `right`.
     **/
    NEMath::Ordering _compareRings(const VALUE* left, uint32_t leftStart, uint32_t leftCapacity, uint32_t leftCount, const VALUE* right, uint32_t rightStart, uint32_t rightCapacity, uint32_t rightCount) const;

    /**
     * \brief   Copies the stack from the given source.
     * \param   source  The source of stack data to copy elements.
     **/
    void _copyStack(const RingStackBase<VALUE>& source);

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RingStackBase() = delete;
    RingStackBase( const RingStackBase<VALUE> & /*src*/ ) = delete;
    RingStackBase( RingStackBase<VALUE> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// ConcurrentRingStack<VALUE> class template declaration
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
class ConcurrentRingStack  : public RingStackBase<VALUE>
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
    explicit ConcurrentRingStack(uint32_t initCapacity = 0, NECommon::OverlapPolicy onOverlap = NECommon::OverlapPolicy::Stop );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    ConcurrentRingStack( const ConcurrentRingStack<VALUE> & source );
    ConcurrentRingStack( const RingStackBase<VALUE> & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source to move data.
     **/
    ConcurrentRingStack(ConcurrentRingStack<VALUE>&& source) noexcept;
    ConcurrentRingStack( RingStackBase<VALUE> && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~ConcurrentRingStack() = default;

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
    ConcurrentRingStack<VALUE> & operator = ( const ConcurrentRingStack<VALUE> & source );
    ConcurrentRingStack<VALUE> & operator = ( const RingStackBase<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from given source.
     *          It as well swaps all entries of source and destination, but
     *          does not change the overlapping type and synchronization object.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    ConcurrentRingStack<VALUE>& operator = (ConcurrentRingStack<VALUE> && source) noexcept;
    ConcurrentRingStack<VALUE>& operator = (RingStackBase<VALUE> && source) noexcept;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are equal.
     **/
    bool operator == (const RingStackBase<VALUE>& other) const;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are not equal.
     **/
    bool operator != (const RingStackBase<VALUE>& other) const;

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
// RingStack<VALUE> class template declaration
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
class RingStack    : public RingStackBase<VALUE>
{
public:
    /**
     * \brief   Initializes non-blocking ring stack, sets initial capacity of 
     *          the stack and sets overlapping flag.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          pushing new element is required.
     **/
    RingStack(uint32_t initCapacity = 0, NECommon::OverlapPolicy onOverlap = NECommon::OverlapPolicy::Stop );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    RingStack( const RingStack<VALUE> & source );
    RingStack( const RingStackBase<VALUE> & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source to move data.
     **/
    RingStack(RingStack<VALUE> && source ) noexcept;
    RingStack( RingStackBase<VALUE> && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~RingStack() = default;

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
    RingStack<VALUE> & operator = ( const RingStack<VALUE> & source );
    RingStack<VALUE> & operator = ( const RingStackBase<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from given source.
     *          It as well swaps all entries of source and destination, but
     *          does not change the overlapping type and synchronization object.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    RingStack<VALUE>& operator = (RingStack<VALUE> && source) noexcept;
    RingStack<VALUE>& operator = (RingStackBase<VALUE> && source) noexcept;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are equal.
     **/
    bool operator == (const RingStackBase<VALUE>& other) const;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are not equal.
     **/
    bool operator != (const RingStackBase<VALUE>& other) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Synchronization object simulation.
     **/
    NolockSyncObject mNoLock;
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RingStackBase<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
RingStackBase<VALUE>::RingStackBase( Lockable & syncObject, uint32_t initCapacity /*= 0*/, NECommon::OverlapPolicy onOverlap /*= NECommon::OverlapPolicy::Stop*/ )
    : mSyncObj  ( syncObject )
    , mOnOverlap( onOverlap )
    , mStackList( initCapacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW uint8_t[initCapacity * sizeof(VALUE)]) : nullptr )
    , mElemCount( 0u )
    , mCapacity ( mStackList != nullptr ? initCapacity : 0 )
    , mHeadPos  ( 0u )
    , mTailPos  ( 0u )
{
}

template <typename VALUE>
RingStackBase<VALUE>::RingStackBase(Lockable& syncObject, const RingStackBase<VALUE> & source)
    : mSyncObj  ( syncObject )
    , mOnOverlap( source.mOnOverlap )
    , mStackList( nullptr )
    , mElemCount( 0u )
    , mCapacity ( 0u )
    , mHeadPos  ( 0u )
    , mTailPos  ( 0u )
{
    Lock lock(source.mSyncObj);
    _copyStack(source);
}

template <typename VALUE>
RingStackBase<VALUE>::RingStackBase(Lockable& syncObject, RingStackBase<VALUE> && source) noexcept
    : mSyncObj  ( syncObject )
    , mOnOverlap( source.mOnOverlap )
    , mStackList( source.mStackList )
    , mElemCount( source.mElemCount )
    , mCapacity ( source.mCapacity )
    , mHeadPos  ( source.mHeadPos )
    , mTailPos  ( source.mTailPos )
{
    Lock lock(source.mSyncObj);

    source.mStackList   = nullptr;
    source.mCapacity    = 0;
    source.mElemCount   = 0;
    source.mHeadPos     = 0;
    source.mTailPos     = 0;
}

template <typename VALUE>
RingStackBase<VALUE>::~RingStackBase()
{
    _emptyStack();
    delete[] reinterpret_cast<uint8_t*>(mStackList);
    mStackList = nullptr;
    mCapacity = 0;
}

template <typename VALUE>
RingStackBase<VALUE> & RingStackBase<VALUE>::operator = ( const RingStackBase<VALUE> & source )
{
    copy(source);
    return (*this);
}

template <typename VALUE>
RingStackBase<VALUE>& RingStackBase<VALUE>::operator = (RingStackBase<VALUE> && source) noexcept
{
    move(std::move(source));
    return (*this);
}

template <typename VALUE>
bool RingStackBase<VALUE>::operator == (const RingStackBase<VALUE>& other) const
{
    if (static_cast<const RingStackBase<VALUE> *>(this) == &other)
        return true;

    Lock lock1(mSyncObj);
    Lock lock2(other.mSyncObj);
    bool result{ false };

    if (mElemCount == other.mElemCount)
    {
        result = _compareRings(mStackList, mHeadPos, mCapacity, mElemCount, other.mStackList, other.mHeadPos, other.mCapacity, other.mElemCount) == NEMath::Ordering::Equal;
    }

    return result;
}

template <typename VALUE>
bool RingStackBase<VALUE>::operator != (const RingStackBase<VALUE>& other) const
{
    if (static_cast<const RingStackBase<VALUE> *>(this) == &other)
        return false;

    Lock lock1(mSyncObj);
    Lock lock2(other.mSyncObj);
    bool result{ true };

    if (mElemCount == other.mElemCount)
    {
        result = _compareRings(mStackList, mHeadPos, mCapacity, mElemCount, other.mStackList, other.mHeadPos, other.mCapacity, other.mElemCount) != NEMath::Ordering::Equal;
    }

    return result;
}

template <typename VALUE>
const VALUE& RingStackBase<VALUE>::operator [] (uint32_t index) const
{
    return getAt(index);
}

template <typename VALUE>
VALUE& RingStackBase<VALUE>::operator [] (uint32_t index)
{
    return getAt(index);
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::getSize() const
{
    Lock lock( mSyncObj );
    return mElemCount;
}

template <typename VALUE>
bool RingStackBase<VALUE>::isEmpty() const
{
    Lock lock( mSyncObj );
    return (mElemCount == 0);
}

template <typename VALUE>
NECommon::OverlapPolicy RingStackBase<VALUE>::getOverlap() const
{
    return mOnOverlap;
}

template <typename VALUE>
bool RingStackBase<VALUE>::lock() const
{
    return mSyncObj.lock(NECommon::WAIT_INFINITE);
}

template <typename VALUE>
bool RingStackBase<VALUE>::unlock() const
{
    return mSyncObj.unlock();
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::capacity() const
{
    Lock lock(mSyncObj);
    return mCapacity;
}

template <typename VALUE>
bool RingStackBase<VALUE>::isFull() const
{
    Lock lock(mSyncObj);
    return (mOnOverlap != NECommon::OverlapPolicy::Resize) && (mElemCount == mCapacity);
}

template <typename VALUE>
bool RingStackBase<VALUE>::isValidIndex(uint32_t index) const
{
    Lock lock(mSyncObj);
    return (index < mElemCount);
}

template <typename VALUE>
const VALUE& RingStackBase<VALUE>::getAt(uint32_t index) const
{
    Lock lock(mSyncObj);
    ASSERT(index < mElemCount);
    ASSERT(mCapacity != 0);
    index = _norm2RingIndex(index);
    return mStackList[index];
}

template <typename VALUE>
VALUE& RingStackBase<VALUE>::getAt(uint32_t index)
{
    Lock lock(mSyncObj);
    ASSERT(index < mElemCount);
    ASSERT(mCapacity != 0);
    index = _norm2RingIndex(index);
    return mStackList[index];
}

template <typename VALUE>
void RingStackBase<VALUE>::setAt(uint32_t index, const VALUE& newValue)
{
    Lock lock(mSyncObj);
    ASSERT(index < mElemCount);
    ASSERT(mCapacity != 0);
    index = _norm2RingIndex(index);
    mStackList[index] = newValue;
}

template <typename VALUE>
void RingStackBase<VALUE>::clear()
{
    Lock lock(mSyncObj);
    _emptyStack();
}

template<typename VALUE>
void RingStackBase<VALUE>::release()
{
    Lock lock(mSyncObj);
    _emptyStack();
    delete[] reinterpret_cast<uint8_t*>(mStackList);
    mStackList = nullptr;
    mCapacity = 0;
}

template<typename VALUE>
void RingStackBase<VALUE>::freeExtra()
{
    if (mCapacity == mElemCount)
        return;

    uint32_t capacity = mElemCount;
    VALUE* newList = capacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW uint8_t[capacity * sizeof(VALUE)]) : nullptr;
    if (newList != nullptr)
    {
        if (mStackList != nullptr)
        {
            _copyElems(newList, mStackList, mHeadPos, mTailPos, mElemCount, capacity);
            _emptyStack();
            delete[] reinterpret_cast<uint8_t*>(mStackList);
        }

        mStackList  = newList;
        mHeadPos    = 0;
        mTailPos    = capacity - 1;
        mElemCount  = capacity;
        mCapacity   = capacity;
    }
    else if (capacity == 0)
    {
        if (mStackList != nullptr)
        {
            _emptyStack();
            delete[] reinterpret_cast<uint8_t*>(mStackList);
            mStackList = nullptr;
        }

        mCapacity   = 0u;
    }
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::push( const VALUE& newElement )
{
    Lock lock(mSyncObj);

    if ( mElemCount < mCapacity )
    {
        ASSERT( (mHeadPos != mTailPos) || (mElemCount <= 1u) );
        ASSERT( (mCapacity != 0u) && (mStackList != nullptr) );

        VALUE* block = nullptr;
        if (mElemCount == 0u)
        {
            block = mStackList;
            mHeadPos = 0u;
            mTailPos = 0u;
        }
        else
        {
            mTailPos = (mTailPos + 1u) % mCapacity;
            block = mStackList + mTailPos;
        }

        NEMemory::constructElems<VALUE>(block, 1);
        *block = newElement;
        ++mElemCount;
    }
    else
    {
        switch ( mOnOverlap )
        {
        case NECommon::OverlapPolicy::Shift:
            if (mCapacity != 0u)
            {
                ASSERT(mStackList != nullptr);
                mTailPos = (mTailPos + 1u) % mCapacity;
                mHeadPos = (mHeadPos + 1u) % mCapacity;
                VALUE* block = mStackList + mTailPos;
                NEMemory::destroyElems<VALUE>(block, 1);
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
            }
            // else capacity == 0 => nothing to do
            break;

        case NECommon::OverlapPolicy::Resize:
            // grow buffer (double or at least 1)
            if ( reserve(static_cast<uint32_t>(mCapacity != 0 ? mCapacity : 1) * 2) >= (mElemCount + 1) )
            {
                ASSERT(mCapacity >= mElemCount + 1u);
                mTailPos = (mTailPos + 1u) % mCapacity;
                VALUE * block = mStackList + mTailPos;
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                ++ mElemCount;
            }
            break;

        case NECommon::OverlapPolicy::Stop:
            AREG_OUTPUT_WARN("The new element is not set in Ring Stack, there is no more free space for new element");
            break;  // do nothing

        default:
            AREG_OUTPUT_ERR("Invalid Overlap action in RingStackBase::push()");
            ASSERT(false);
            break;
        }
    }

    return mElemCount;
}

template <typename VALUE>
VALUE RingStackBase<VALUE>::pop()
{
    Lock lock(mSyncObj);
    ASSERT( isEmpty() == false );
    VALUE result{ };

    if ( mElemCount != 0u )
    {
        ASSERT( mCapacity != 0 );
        ASSERT( mStackList != nullptr );
        ASSERT((mHeadPos != mTailPos) || (mElemCount == 1u));

        result = mStackList[mHeadPos];
        NEMemory::destroyElems<VALUE>( mStackList + mHeadPos, 1 );
        mHeadPos = (mHeadPos + 1u) % mCapacity;
        -- mElemCount;

        if (mElemCount == 0u)
        {
            mHeadPos = mTailPos = 0u;
        }
    }

    return result;
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::add( const RingStackBase<VALUE> & source )
{
    Lock lock(mSyncObj);
    uint32_t initial = mElemCount;
    if (static_cast<const RingStackBase<VALUE> *>(this) != &source)
    {
        Lock lock2(source.mSyncObj);
        for (uint32_t i = 0u; i < source.mElemCount; ++i)
        {
            push(source[i]);
        }
    }

    return (mElemCount - initial);
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::reserve(uint32_t newCapacity )
{
    Lock lock(mSyncObj);

    if ( newCapacity > mCapacity )
    {
        VALUE * newList = newCapacity != 0 ? reinterpret_cast<VALUE *>( DEBUG_NEW uint8_t[ newCapacity * sizeof(VALUE)] ) : nullptr;
        if (newList != nullptr)
        {
            uint32_t elemCount = mElemCount;
            if (mStackList != nullptr)
            {
                _copyElems(newList, mStackList, mHeadPos, mTailPos, elemCount, mCapacity);
                _emptyStack();
                delete[] reinterpret_cast<uint8_t*>(mStackList);
            }

            mStackList = newList;
            mHeadPos   = 0u;
            mTailPos   = (elemCount == 0u ? 0u : (elemCount - 1u));
            mElemCount = elemCount;
            mCapacity  = newCapacity;
        }
    }

    return mCapacity;
}

template <typename VALUE>
void RingStackBase<VALUE>::copy(const RingStackBase<VALUE>& source)
{
    if (static_cast<const RingStackBase<VALUE> *>(this) != &source)
    {
        Lock lock1(mSyncObj);
        Lock lock2(source.mSyncObj);
        _copyStack(source);
    }
}

template <typename VALUE>
void RingStackBase<VALUE>::move(RingStackBase<VALUE> && source) noexcept
{
    if (static_cast<const RingStackBase<VALUE> *>(this) != &source)
    {
        Lock lock1(mSyncObj);
        Lock lock2(source.mSyncObj);

        std::swap(mStackList, source.mStackList);
        std::swap(mElemCount, source.mElemCount);
        std::swap(mCapacity, source.mCapacity);
        std::swap(mHeadPos, source.mHeadPos);
        std::swap(mTailPos, source.mTailPos);
    }
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::find(const VALUE& elem, uint32_t startAt /*= NECommon::RING_START_POSITION*/) const
{
    Lock lock(mSyncObj);

    uint32_t result = static_cast<uint32_t>(NECommon::INVALID_INDEX);
    startAt = startAt == NECommon::RING_START_POSITION ? 0u : startAt;
    for (uint32_t i = startAt; i < mElemCount; ++i)
    {
        if (elem == mStackList[_norm2RingIndex(i)])
        {
            result = i;
            break;
        }
    }

    return result;
}

template <typename VALUE>
bool RingStackBase<VALUE>::contains(const VALUE& elem, uint32_t startAt /*= NECommon::RING_START_POSITION*/) const
{
    return (find(elem, startAt) != static_cast<uint32_t>(NECommon::INVALID_INDEX));
}

template <typename VALUE>
void RingStackBase<VALUE>::_emptyStack()
{
    // do not delete stack, only remove elements and reset data.
    // keep capacity same
    for (uint32_t i = 0; i < mElemCount; ++ i)
    {
        NEMemory::destroyElems<VALUE>( mStackList + mHeadPos, 1 );
        mHeadPos = (mHeadPos + 1 ) % mCapacity;
    }

    mHeadPos    = 0;
    mTailPos    = 0;
    mElemCount  = 0;
}

template <typename VALUE>
void RingStackBase<VALUE>::_copyElems(VALUE* dst, VALUE* src, uint32_t srcStart, uint32_t srcEnd, uint32_t srcCount, uint32_t srcCapacity)
{
    ASSERT(srcCapacity >= srcCount);
    ASSERT((dst != nullptr) && (src != nullptr));

    if (srcCount == 0)
        return;

    NEMemory::constructElems<VALUE>(static_cast<void*>(dst), srcCount);
    if (srcEnd >= srcStart)
    {
        ASSERT((srcEnd - srcStart + 1u) == srcCount);
        NEMemory::copyElems<VALUE>(dst, src + srcStart, srcCount);
    }
    else
    {
        uint32_t elemCopy = srcCapacity - srcStart;
        ASSERT((elemCopy + srcEnd + 1u) == srcCount);
        NEMemory::copyElems<VALUE>(dst, src + srcStart, elemCopy);
        NEMemory::copyElems<VALUE>(dst + elemCopy, src, srcEnd + 1);
    }
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::_norm2RingIndex(uint32_t index) const
{
    return (mCapacity != 0 ? ((mHeadPos + index) % mCapacity) : 0u);
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::_ring2NormIndex(uint32_t ring) const
{
    if (mCapacity == 0)
        return 0u;

    return (ring >= mHeadPos ? ring - mHeadPos : (mCapacity - mHeadPos) + ring);
}

template <typename VALUE>
NEMath::Ordering RingStackBase<VALUE>::_compareRings( const VALUE* left, uint32_t leftStart, uint32_t leftCapacity, uint32_t leftCount
                                                         , const VALUE* right, uint32_t rightStart, uint32_t rightCapacity, uint32_t rightCount) const
{
    ASSERT((leftStart < leftCapacity) && (rightStart < rightCapacity));
    NEMath::Ordering result{ NEMath::Ordering::Equal };
    uint32_t count = std::min(leftCount, rightCount);
    while (count != 0u)
    {
        result = NEMath::compare<VALUE>(left[leftStart], right[rightStart]);
        if (result != NEMath::Ordering::Equal)
        {
            return result;
        }

        leftStart  = (leftStart  + 1u) % leftCapacity;
        rightStart = (rightStart + 1u) % rightCapacity;
        --count;
    }

    return NEMath::compare<uint32_t>(leftCount, rightCount);
}

template <typename VALUE>
void RingStackBase<VALUE>::_copyStack(const RingStackBase<VALUE>& source)
{
    _emptyStack();
    uint32_t srcCapacity{ source.mCapacity };
    VALUE*   newList    { mStackList };

    if (mCapacity < srcCapacity)
    {
        delete[] reinterpret_cast<uint8_t*>(mStackList);
        mStackList  = nullptr;
        newList     = srcCapacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW uint8_t[srcCapacity * sizeof(VALUE)]) : nullptr;
        mCapacity   = srcCapacity;
    }

    if ((newList != nullptr) && (source.mElemCount != 0))
    {
        if (source.mStackList != nullptr)
        {
            _copyElems(newList, source.mStackList, source.mHeadPos, source.mTailPos, source.mElemCount, srcCapacity);
        }

        mStackList  = newList;
        mElemCount  = source.mElemCount;
        mHeadPos    = 0u;
        mTailPos    = source.mElemCount - 1;
    }
}

//////////////////////////////////////////////////////////////////////////
// ConcurrentRingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
ConcurrentRingStack<VALUE>::ConcurrentRingStack(uint32_t initCapacity /*= 0*/, NECommon::OverlapPolicy onOverlap /*= NECommon::OverlapPolicy::Stop*/ )
    : RingStackBase<VALUE>    ( mLock, initCapacity, onOverlap )
    , mLock ( false )
{
}

template <typename VALUE>
ConcurrentRingStack<VALUE>::ConcurrentRingStack( const ConcurrentRingStack<VALUE> & source )
    : RingStackBase<VALUE>    ( mLock, static_cast<const RingStackBase<VALUE> &>(source) )
    , mLock ( false )
{
}

template <typename VALUE>
ConcurrentRingStack<VALUE>::ConcurrentRingStack( const RingStackBase<VALUE> & source )
    : RingStackBase<VALUE>    ( mLock, source )
    , mLock ( false )
{
}

template <typename VALUE>
ConcurrentRingStack<VALUE>::ConcurrentRingStack( RingStackBase<VALUE> && source ) noexcept
    : RingStackBase<VALUE>    ( mLock, std::move(source) )
    , mLock ( false )
{
}

template <typename VALUE>
ConcurrentRingStack<VALUE>::ConcurrentRingStack(ConcurrentRingStack<VALUE> && source ) noexcept
    : RingStackBase<VALUE>    ( mLock, std::move(static_cast<RingStackBase<VALUE> &&>(source)) )
    , mLock ( false )
{
}

template <typename VALUE>
ConcurrentRingStack<VALUE> & ConcurrentRingStack<VALUE>::operator = ( const ConcurrentRingStack<VALUE> & source )
{
    static_cast<RingStackBase<VALUE> &>(*this).operator = (static_cast<const RingStackBase<VALUE> &>(source));
    return (*this);
}

template <typename VALUE>
ConcurrentRingStack<VALUE> & ConcurrentRingStack<VALUE>::operator = ( const RingStackBase<VALUE> & source )
{
    static_cast<RingStackBase<VALUE>&>(*this).operator = (static_cast<const RingStackBase<VALUE>&>(source));
    return (*this);
}

template <typename VALUE>
ConcurrentRingStack<VALUE>& ConcurrentRingStack<VALUE>::operator = (ConcurrentRingStack<VALUE>&& source) noexcept
{
    RingStackBase<VALUE>& temp = static_cast<RingStackBase<VALUE> &>(source);
    RingStackBase<VALUE>::move(std::move(temp));
    return (*this);
}

template <typename VALUE>
ConcurrentRingStack<VALUE>& ConcurrentRingStack<VALUE>::operator = (RingStackBase<VALUE>&& source) noexcept
{
    RingStackBase<VALUE>::move(std::move(source));
    return (*this);
}

template <typename VALUE>
bool ConcurrentRingStack<VALUE>::operator == (const RingStackBase<VALUE>& other) const
{
    return static_cast<const RingStackBase<VALUE>&>(*this).operator == (other);
}

template <typename VALUE>
bool ConcurrentRingStack<VALUE>::operator != (const RingStackBase<VALUE>& other) const
{
    return static_cast<const RingStackBase<VALUE>&>(*this).operator != (other);
}

//////////////////////////////////////////////////////////////////////////
// RingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
RingStack<VALUE>::RingStack(uint32_t initCapacity /*= 0*/, NECommon::OverlapPolicy onOverlap /*= NECommon::OverlapPolicy::Stop*/ )
    : RingStackBase<VALUE>    ( mNoLock, initCapacity, onOverlap )
    , mNoLock   ( )
{
}

template <typename VALUE>
RingStack<VALUE>::RingStack( const RingStackBase<VALUE> & source )
    : RingStackBase<VALUE>    ( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE>
RingStack<VALUE>::RingStack( const RingStack<VALUE> & source )
    : RingStackBase<VALUE>    ( mNoLock, static_cast<const RingStackBase<VALUE> &>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE>
RingStack<VALUE>::RingStack(RingStackBase<VALUE> && source ) noexcept
    : RingStackBase<VALUE>    ( mNoLock, std::move(source) )
    , mNoLock   ( )
{
}

template <typename VALUE>
RingStack<VALUE>::RingStack(RingStack<VALUE> && source ) noexcept
    : RingStackBase<VALUE>    ( mNoLock, std::move(static_cast<RingStackBase<VALUE> &&>(source)) )
    , mNoLock   ( )
{
}

template <typename VALUE>
RingStack<VALUE> & RingStack<VALUE>::operator = ( const RingStack<VALUE> & source )
{
    static_cast<RingStackBase<VALUE> &>(*this).operator = (static_cast<const RingStackBase<VALUE>&>(source));
    return (*this);
}

template <typename VALUE>
RingStack<VALUE> & RingStack<VALUE>::operator = ( const RingStackBase<VALUE> & source )
{
    static_cast<RingStackBase<VALUE>&>(*this).operator = (source);
    return (*this);
}

template <typename VALUE>
RingStack<VALUE>& RingStack<VALUE>::operator = (RingStack<VALUE>&& source) noexcept
{
    static_cast<RingStackBase<VALUE>&>(*this).operator = (std::move(static_cast<RingStackBase<VALUE> &&>(source)));
    return (*this);
}

template <typename VALUE>
RingStack<VALUE>& RingStack<VALUE>::operator = (RingStackBase<VALUE>&& source) noexcept
{
    static_cast<RingStackBase<VALUE>&>(*this).operator = (std::move(source));
    return (*this);
}

template <typename VALUE>
bool RingStack<VALUE>::operator == (const RingStackBase<VALUE>& other) const
{
    return static_cast<const RingStackBase<VALUE>&>(*this).operator == (other);
}

template <typename VALUE>
bool RingStack<VALUE>::operator != (const RingStackBase<VALUE>& other) const
{
    return static_cast<const RingStackBase<VALUE>&>(*this).operator!=(other);
}

//////////////////////////////////////////////////////////////////////////
// RingStackBase<VALUE> class template friend operators
//////////////////////////////////////////////////////////////////////////

template <typename V>
const InStream & operator >> ( const InStream & stream, RingStackBase<V> & input )
{
    Lock lock(input.mSyncObj);

    uint32_t size = 0;
    stream >> size;

    input.clear();
    if ((input.reserve(size)) >= size && (size != 0))
    {
        NEMemory::constructElems<V>(input.mStackList, size);
        for (uint32_t i = 0; i < size; i++)
        {
            stream >> input.mStackList[i];
        }

        input.mHeadPos = 0;
        input.mTailPos = size - 1u;
        input.mElemCount = size;
    }

    return stream;
}

template <typename V>
OutStream & operator << ( OutStream & stream, const RingStackBase<V> & output )
{
    Lock lock(output.mSyncObj);

    uint32_t size = output.mElemCount;
    stream << size;
    uint32_t pos = output.mHeadPos;
    for (uint32_t i = 0; i < size; i ++)
    {
        stream << output.mStackList[pos];
        pos = ( pos + 1 ) % output.mCapacity;
    }

    return stream;
}

#endif  // AREG_BASE_RINGSTACK_HPP
