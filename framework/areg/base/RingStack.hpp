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
#include "areg/base/areg_global.h"
#include "areg/base/TemplateBase.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include "areg/base/IOStream.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/MathDefs.hpp"

#include <algorithm>
namespace areg {

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
 * \brief   Ring FIFO stack container with configurable capacity and overlap policy. Supports
 *          synchronized or unsynchronized access depending on the provided synchronization object.
 *          Capacity may be fixed or dynamically resized based on overlap policy when full.
 **/
template <typename VALUE>
class RingStackBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes ring stack with synchronization object, initial capacity, and overlap
     *          behavior.
     *
     * \param   syncObject      Reference to synchronization object.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlap policy applied when ring stack is full and new element must
     *                          be inserted.
     **/
    explicit RingStackBase( Lockable & syncObject, uint32_t initCapacity = 0, areg::OverlapPolicy onOverlap = areg::OverlapPolicy::Stop );

    ~RingStackBase();

protected:
    /**
     * \brief   Creates ring stack and copies elements from source.
     *
     * \param   syncObject      Reference to synchronization object.
     * \param   source          The source ring stack to copy elements from.
     **/
    explicit RingStackBase(Lockable& syncObject, const RingStackBase<VALUE>& source);

    /**
     * \brief   Creates ring stack and moves elements from source.
     *
     * \param   syncObject      Reference to synchronization object.
     * \param   source          The source ring stack to move elements from.
     * \note    Move overload. Takes ownership of source elements.
     **/
    explicit RingStackBase(Lockable& syncObject, RingStackBase<VALUE> && source) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies ring stack entries from source; enlarges destination capacity if needed.
     *
     * \param   source      The source ring stack to copy entries from.
     * \return  Returns reference to this ring stack object.
     **/
    RingStackBase<VALUE> & operator = ( const RingStackBase<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from source; swaps all entries without changing overlap
     *          policy or synchronization object.
     *
     * \param   source      The source ring stack to move entries from.
     * \return  Returns reference to this ring stack object.
     * \note    Move overload. Swaps content with source.
     **/
    RingStackBase<VALUE>& operator = (RingStackBase<VALUE> && source) noexcept;

    /**
     * \brief   Compares two ring stacks for equality.
     *
     * \param   other       The ring stack to compare with.
     * \return  Returns true if ring stacks are equal; false otherwise.
     **/
    [[nodiscard]]
    bool operator == (const RingStackBase<VALUE>& other) const noexcept;

    /**
     * \brief   Compares two ring stacks for inequality.
     *
     * \param   other       The ring stack to compare with.
     * \return  Returns true if ring stacks are not equal; false otherwise.
     **/
    [[nodiscard]]
    bool operator != (const RingStackBase<VALUE>& other) const noexcept;

    /**
     * \brief   Returns const reference to element at given zero-based index.
     *
     * \param   index       Zero-based index of element in stack; must be less than size.
     * \return  Returns const reference to element at index.
     **/
    [[nodiscard]]
    const VALUE& operator [] (uint32_t index) const noexcept;

    /**
     * \brief   Returns mutable reference to element at given zero-based index.
     *
     * \param   index       Zero-based index of element in stack; must be less than size.
     * \return  Returns mutable reference to element at index.
     **/
    [[nodiscard]]
    VALUE& operator [] (uint32_t index) noexcept;

/************************************************************************/
// Friend global operators to make Stack streamable
/************************************************************************/

    /**
     * \brief   Deserializes ring stack elements from input stream; clears existing elements and
     *          resizes if necessary.
     *
     * \param   stream      The input stream to read values from.
     * \param[out] input       The ring stack to initialize with deserialized values.
     * \return  Returns const reference to input stream.
     * \note    VALUE type must support streaming deserialization operator.
     **/
    template<typename V>
    friend const InStream & operator >> ( const InStream & stream, RingStackBase<V> & input );
    /**
     * \brief   Serializes ring stack elements to output stream starting from head position.
     *
     * \param[out] stream      The output stream to write values to.
     * \param   output      The ring stack to serialize.
     * \return  Returns reference to output stream.
     * \note    VALUE type must support streaming serialization operator.
     **/
    template<typename V>
    friend OutStream & operator << ( OutStream & stream, const RingStackBase<V> & output );

//////////////////////////////////////////////////////////////////////////
// Operations and Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns number of elements in stack.
     *
     * \return  Returns element count.
     **/
    [[nodiscard]]
    uint32_t size() const noexcept;

    /**
     * \brief   Returns true if ring stack is empty.
     *
     * \return  Returns true if stack is empty; false otherwise.
     **/
    [[nodiscard]]
    bool is_empty() const noexcept;

    /**
     * \brief   Returns the overlap policy of the ring stack.
     *
     * \return  Returns overlap policy applied when stack is full.
     **/
    [[nodiscard]]
    areg::OverlapPolicy overlap() const noexcept;

    /**
     * \brief   Locks stack for exclusive access; no-op if using NoLockSyncObject.
     *
     * \return  Returns true if successfully locked; false otherwise.
     **/
    bool lock() const;

    /**
     * \brief   Unlocks previously locked stack; no-op if using NoLockSyncObject.
     *
     * \return  Returns true if successfully unlocked; false otherwise.
     **/
    bool unlock() const;

    /**
     * \brief   Returns the capacity of the ring stack.
     *
     * \return  Returns capacity.
     **/
    [[nodiscard]]
    uint32_t capacity() const noexcept;

    /**
     * \brief   Returns true if ring stack is full; always false for resize-on-overlap policy.
     *
     * \return  Returns true if stack is full and not auto-resizing; false otherwise.
     **/
    [[nodiscard]]
    bool is_full() const noexcept;

    /**
     * \brief   Returns true if zero-based index is valid (less than number of elements).
     *
     * \param   index       The index to validate.
     * \return  Returns true if index is valid; false otherwise.
     **/
    [[nodiscard]]
    bool is_valid_index(uint32_t index) const noexcept;

    /**
     * \brief   Returns const reference to element at given zero-based index.
     *
     * \param   index       Zero-based index of element in stack; must be less than size.
     * \return  Returns const reference to element at index.
     **/
    [[nodiscard]]
    const VALUE & value_at(uint32_t index) const noexcept;

    /**
     * \brief   Returns mutable reference to element at given zero-based index.
     *
     * \param   index       Zero-based index of element in stack; must be less than size.
     * \return  Returns mutable reference to element at index.
     **/
    [[nodiscard]]
    VALUE& value_at(uint32_t index) noexcept;

    /**
     * \brief   Sets new value at given zero-based index.
     *
     * \param   index       Zero-based index of element to update; must be valid.
     * \param   newValue    The new value to set at index.
     **/
    void set_value_at(uint32_t index, const VALUE& newValue);

    /**
     * \brief   Pushes element at end of stack; behavior on full stack depends on overlap policy.
     *
     * \param   newElement      New element to add at end of stack.
     * \return  Returns size of stack after push.
     * \note    If stack is full: Stop policy prevents insertion, Shift policy overwrites head
     *          element, Resize policy enlarges capacity (or sets to 2 if zero).
     **/
    uint32_t push( const VALUE& newElement );

    /**
     * \brief   Removes and returns element from head of stack; decrements size.
     *
     * \return  Returns value of removed element.
     * \note    Stack must not be empty when called.
     **/
    [[nodiscard]]
    VALUE pop();

    /**
     * \brief   Removes all elements from stack; capacity remains unchanged.
     **/
    void clear() noexcept;

    /**
     * \brief   Clears stack, frees internal buffer, and resets capacity to zero.
     **/
    void release();

    /**
     * \brief   Removes unused capacity; shrinks capacity to equal element count.
     **/
    void free_extra();

    /**
     * \brief   Adds elements from source stack; behavior on full stack depends on overlap policy.
     *
     * \param   source      The source ring stack to copy elements from.
     * \return  Returns number of elements actually copied (may differ from source size depending on
     *          overlap policy).
     * \note    If insufficient capacity: Stop policy copies until full then stops, Shift policy
     *          overwrites head elements as needed, Resize policy enlarges to fit all elements.
     **/
    uint32_t add( const RingStackBase<VALUE> & source );

    /**
     * \brief   Reserves space for given capacity; enlarges if needed, ignores if new capacity is
     *          not larger.
     *
     * \param   newCapacity     New capacity to set for ring stack.
     * \return  Returns new capacity after reservation.
     * \note    If new capacity exceeds current, stack is resized and all elements are preserved.
     **/
    uint32_t reserve( uint32_t newCapacity );

    /**
     * \brief   Copies entries from source; previous entries are replaced.
     *
     * \param   source      The source ring stack to copy from.
     **/
    void copy(const RingStackBase<VALUE> & source);

    /**
     * \brief   Moves entries from source; swaps content with source.
     *
     * \param   source      The source ring stack to move from.
     * \note    Move operation. Exchanges content with source.
     **/
    void move(RingStackBase<VALUE> && source) noexcept;

    /**
     * \brief   Searches for element in stack starting at given position.
     *
     * \param   elem        The element value to search for.
     * \param   startAt     Starting position index; use areg::RING_START_POSITION to search
     *                      from beginning.
     * \return  Returns valid index if found; returns areg::INVALID_INDEX if not found.
     **/
    [[nodiscard]]
    uint32_t find(const VALUE& elem, uint32_t startAt = areg::RING_START_POSITION) const noexcept;

    /**
     * \brief   Returns true if ring stack contains specified element starting at given position.
     *
     * \param   elem        The element to search for.
     * \param   startAt     Starting position index; use areg::RING_START_POSITION to search
     *                      from beginning.
     * \return  Returns true if element found; false otherwise.
     **/
    [[nodiscard]]
    bool contains(const VALUE& elem, uint32_t startAt = areg::RING_START_POSITION) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   The instance of synchronization object to be used to make object thread-safe.
     **/
    Lockable &                  mSyncObj;

    /**
     * \brief   The overlapping flag. Set when stack is initialized and cannot be changed anymore.
     **/
    const areg::OverlapPolicy   mOnOverlap;

    /**
     * \brief   The array of element in stack.
     **/
    VALUE *                     mStackList;

    /**
     * \brief   The number of elements in the stack.
     **/
    uint32_t                    mElemCount;

    /**
     * \brief   The capacity value of stack.
     **/
    uint32_t                    mCapacity;

    /**
     * \brief   The index of head element in array of stack
     **/
    uint32_t                    mHeadPos;

    /**
     * \brief   The index of tail element in array of stack
     **/
    uint32_t                    mTailPos;

//////////////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Clears all elements without acquiring lock; capacity unchanged.
     **/
    void _empty_stack() noexcept;

    /**
     * \brief   Copies elements from source ring buffer to destination; destination must have
     *          sufficient capacity.
     *
     * \param[out] dst             The destination buffer to copy elements into.
     * \param   src             The source ring buffer to copy elements from.
     * \param   srcStart        Starting position index in source ring.
     * \param   srcEnd          Ending position index in source ring (inclusive).
     * \param   srcCount        Number of elements to copy.
     * \param   srcCapacity     Capacity of source ring (used for wraparound calculation).
     **/
    void _copy_elems(VALUE* dst, VALUE* src, uint32_t srcStart, uint32_t srcEnd, uint32_t srcCount, uint32_t srcCapacity);

    /**
     * \brief   Converts normalized zero-based index to internal ring buffer index.
     *
     * \param   index       Normalized zero-based index to convert.
     * \return  Returns internal ring buffer index (not exceeding capacity).
     * \note    Index 0 corresponds to head position.
     **/
    [[nodiscard]]
    uint32_t _norm2_ring_index(uint32_t index) const noexcept;

    /**
     * \brief   Converts internal ring buffer index to normalized zero-based index.
     *
     * \param   ring    Internal ring buffer index to convert.
     * \return  Returns normalized zero-based index (not exceeding element count).
     * \note    Head position corresponds to normalized index 0.
     **/
    [[nodiscard]]
    uint32_t _ring2_norm_index(uint32_t ring) const noexcept;

    /**
     * \brief   Compares elements from two ring buffers and returns ordering result.
     *
     * \param   left                Left ring buffer elements.
     * \param   leftStart           Starting position index in left ring.
     * \param   leftCapacity        Capacity of left ring.
     * \param   leftCount           Number of elements in left ring.
     * \param   right               Right ring buffer elements.
     * \param   rightStart          Starting position index in right ring.
     * \param   rightCapacity       Capacity of right ring.
     * \param   rightCount          Number of elements in right ring.
     * \return  Returns areg::Smaller if left < right, areg::Equal if equal, areg::Bigger if
     *          left > right.
     **/
    [[nodiscard]]
    areg::Ordering _compare_rings(const VALUE* left, uint32_t leftStart, uint32_t leftCapacity, uint32_t leftCount, const VALUE* right, uint32_t rightStart, uint32_t rightCapacity, uint32_t rightCount) const noexcept;

    /**
     * \brief   Copies stack data from source.
     *
     * \param   source      The source ring stack to copy from.
     **/
    void _copy_stack(const RingStackBase<VALUE>& source);

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
 * \brief   Thread-safe FIFO ring stack class template. Data access is synchronized. Use this class
 *          when elements are accessed by multiple threads.
 **/
template <typename VALUE> 
class ConcurrentRingStack  : public RingStackBase<VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes locking ring stack with initial capacity and overlap policy.
     *
     * \param   initCapacity    The initial capacity size of the ring stack.
     * \param   onOverlap       Overlap policy applied when the ring stack is full and a new element
     *                          is pushed.
     **/
    explicit ConcurrentRingStack(uint32_t initCapacity = 0, areg::OverlapPolicy onOverlap = areg::OverlapPolicy::Stop );

    /**
     * \brief
     *
     * \param   source      The source to copy data from.
     **/
    ConcurrentRingStack( const ConcurrentRingStack<VALUE> & source );
    /**
     * \brief
     *
     * \param   source      The source to copy data from.
     **/
    ConcurrentRingStack( const RingStackBase<VALUE> & source );

    /**
     * \brief
     *
     * \param   source      The source to move data from.
     * \note    Move overload.
     **/
    ConcurrentRingStack(ConcurrentRingStack<VALUE>&& source) noexcept;
    /**
     * \brief
     *
     * \param   source      The source to move data from.
     * \note    Move overload.
     **/
    ConcurrentRingStack( RingStackBase<VALUE> && source ) noexcept;

    ~ConcurrentRingStack() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies ring stack entries from given source. If destination capacity is smaller, it
     *          is enlarged.
     *
     * \param   source      The source ring stack.
     * \return  Reference to this object.
     **/
    ConcurrentRingStack<VALUE> & operator = ( const ConcurrentRingStack<VALUE> & source );
    /**
     * \brief   Copies ring stack entries from given source. If destination capacity is smaller, it
     *          is enlarged.
     *
     * \param   source      The source ring stack.
     * \return  Reference to this object.
     **/
    ConcurrentRingStack<VALUE> & operator = ( const RingStackBase<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from source. Swaps entries but does not change overlap
     *          policy or synchronization.
     *
     * \param   source      The source ring stack.
     * \return  Reference to this object.
     **/
    ConcurrentRingStack<VALUE>& operator = (ConcurrentRingStack<VALUE> && source) noexcept;
    /**
     * \brief   Moves ring stack entries from source. Swaps entries but does not change overlap
     *          policy or synchronization.
     *
     * \param   source      The source ring stack.
     * \return  Reference to this object.
     **/
    ConcurrentRingStack<VALUE>& operator = (RingStackBase<VALUE> && source) noexcept;

    /**
     * \brief   Returns true if both ring stacks are equal.
     **/
    bool operator == (const RingStackBase<VALUE>& other) const;

    /**
     * \brief   Returns true if both ring stacks are not equal.
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
 * \brief   Non-blocking FIFO ring stack class template. No data access synchronization. Not
 *          thread-safe. Use this class when elements are accessed by a single thread.
 **/
template <typename VALUE> 
class RingStack    : public RingStackBase<VALUE>
{
public:
    /**
     * \brief   Initializes non-blocking ring stack with initial capacity and overlap policy.
     *
     * \param   initCapacity    The initial capacity size of the ring stack.
     * \param   onOverlap       Overlap policy applied when the ring stack is full and a new element
     *                          is pushed.
     **/
    RingStack(uint32_t initCapacity = 0, areg::OverlapPolicy onOverlap = areg::OverlapPolicy::Stop );

    /**
     * \brief
     *
     * \param   source      The source to copy data from.
     **/
    RingStack( const RingStack<VALUE> & source );
    /**
     * \brief
     *
     * \param   source      The source to copy data from.
     **/
    RingStack( const RingStackBase<VALUE> & source );

    /**
     * \brief
     *
     * \param   source      The source to move data from.
     * \note    Move overload.
     **/
    RingStack(RingStack<VALUE> && source ) noexcept;
    /**
     * \brief
     *
     * \param   source      The source to move data from.
     * \note    Move overload.
     **/
    RingStack( RingStackBase<VALUE> && source ) noexcept;

    ~RingStack() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies ring stack entries from given source. If destination capacity is smaller, it
     *          is enlarged.
     *
     * \param   source      The source ring stack.
     * \return  Reference to this object.
     **/
    RingStack<VALUE> & operator = ( const RingStack<VALUE> & source );
    /**
     * \brief   Copies ring stack entries from given source. If destination capacity is smaller, it
     *          is enlarged.
     *
     * \param   source      The source ring stack.
     * \return  Reference to this object.
     **/
    RingStack<VALUE> & operator = ( const RingStackBase<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from source. Swaps entries but does not change overlap
     *          policy or synchronization.
     *
     * \param   source      The source ring stack.
     * \return  Reference to this object.
     **/
    RingStack<VALUE>& operator = (RingStack<VALUE> && source) noexcept;
    /**
     * \brief   Moves ring stack entries from source. Swaps entries but does not change overlap
     *          policy or synchronization.
     *
     * \param   source      The source ring stack.
     * \return  Reference to this object.
     **/
    RingStack<VALUE>& operator = (RingStackBase<VALUE> && source) noexcept;

    /**
     * \brief   Returns true if both ring stacks are equal.
     **/
    bool operator == (const RingStackBase<VALUE>& other) const;

    /**
     * \brief   Returns true if both ring stacks are not equal.
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
RingStackBase<VALUE>::RingStackBase( Lockable & syncObject, uint32_t initCapacity /*= 0*/, areg::OverlapPolicy onOverlap /*= areg::OverlapPolicy::Stop*/ )
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
    _copy_stack(source);
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
    _empty_stack();
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
bool RingStackBase<VALUE>::operator == (const RingStackBase<VALUE>& other) const noexcept
{
    if (static_cast<const RingStackBase<VALUE> *>(this) == &other)
        return true;

    Lock lock1(mSyncObj);
    Lock lock2(other.mSyncObj);
    bool result{ false };

    if (mElemCount == other.mElemCount)
    {
        result = _compare_rings(mStackList, mHeadPos, mCapacity, mElemCount, other.mStackList, other.mHeadPos, other.mCapacity, other.mElemCount) == areg::Ordering::Equal;
    }

    return result;
}

template <typename VALUE>
bool RingStackBase<VALUE>::operator != (const RingStackBase<VALUE>& other) const noexcept
{
    if (static_cast<const RingStackBase<VALUE> *>(this) == &other)
        return false;

    Lock lock1(mSyncObj);
    Lock lock2(other.mSyncObj);
    bool result{ true };

    if (mElemCount == other.mElemCount)
    {
        result = _compare_rings(mStackList, mHeadPos, mCapacity, mElemCount, other.mStackList, other.mHeadPos, other.mCapacity, other.mElemCount) != areg::Ordering::Equal;
    }

    return result;
}

template <typename VALUE>
const VALUE& RingStackBase<VALUE>::operator [] (uint32_t index) const noexcept
{
    return value_at(index);
}

template <typename VALUE>
VALUE& RingStackBase<VALUE>::operator [] (uint32_t index) noexcept
{
    return value_at(index);
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::size() const noexcept
{
    Lock lock( mSyncObj );
    return mElemCount;
}

template <typename VALUE>
bool RingStackBase<VALUE>::is_empty() const noexcept
{
    Lock lock( mSyncObj );
    return (mElemCount == 0);
}

template <typename VALUE>
areg::OverlapPolicy RingStackBase<VALUE>::overlap() const noexcept
{
    return mOnOverlap;
}

template <typename VALUE>
bool RingStackBase<VALUE>::lock() const
{
    return mSyncObj.lock(areg::WAIT_INFINITE);
}

template <typename VALUE>
bool RingStackBase<VALUE>::unlock() const
{
    return mSyncObj.unlock();
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::capacity() const noexcept
{
    Lock lock(mSyncObj);
    return mCapacity;
}

template <typename VALUE>
bool RingStackBase<VALUE>::is_full() const noexcept
{
    Lock lock(mSyncObj);
    return (mOnOverlap != areg::OverlapPolicy::Resize) && (mElemCount == mCapacity);
}

template <typename VALUE>
bool RingStackBase<VALUE>::is_valid_index(uint32_t index) const noexcept
{
    Lock lock(mSyncObj);
    return (index < mElemCount);
}

template <typename VALUE>
const VALUE& RingStackBase<VALUE>::value_at(uint32_t index) const noexcept
{
    Lock lock(mSyncObj);
    ASSERT(index < mElemCount);
    ASSERT(mCapacity != 0);
    index = _norm2_ring_index(index);
    return mStackList[index];
}

template <typename VALUE>
VALUE& RingStackBase<VALUE>::value_at(uint32_t index) noexcept
{
    Lock lock(mSyncObj);
    ASSERT(index < mElemCount);
    ASSERT(mCapacity != 0);
    index = _norm2_ring_index(index);
    return mStackList[index];
}

template <typename VALUE>
void RingStackBase<VALUE>::set_value_at(uint32_t index, const VALUE& newValue)
{
    Lock lock(mSyncObj);
    ASSERT(index < mElemCount);
    ASSERT(mCapacity != 0);
    index = _norm2_ring_index(index);
    mStackList[index] = newValue;
}

template <typename VALUE>
void RingStackBase<VALUE>::clear() noexcept
{
    Lock lock(mSyncObj);
    _empty_stack();
}

template<typename VALUE>
void RingStackBase<VALUE>::release()
{
    Lock lock(mSyncObj);
    _empty_stack();
    delete[] reinterpret_cast<uint8_t*>(mStackList);
    mStackList = nullptr;
    mCapacity = 0;
}

template<typename VALUE>
void RingStackBase<VALUE>::free_extra()
{
    if (mCapacity == mElemCount)
        return;

    uint32_t capacity = mElemCount;
    VALUE* newList = capacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW uint8_t[capacity * sizeof(VALUE)]) : nullptr;
    if (newList != nullptr)
    {
        if (mStackList != nullptr)
        {
            _copy_elems(newList, mStackList, mHeadPos, mTailPos, mElemCount, capacity);
            _empty_stack();
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
            _empty_stack();
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

        areg::construct_elems<VALUE>(static_cast<void *>(block), 1);
        *block = newElement;
        ++mElemCount;
    }
    else
    {
        switch ( mOnOverlap )
        {
        case areg::OverlapPolicy::Shift:
            if (mCapacity != 0u)
            {
                ASSERT(mStackList != nullptr);
                mTailPos = (mTailPos + 1u) % mCapacity;
                mHeadPos = (mHeadPos + 1u) % mCapacity;
                VALUE* block = mStackList + mTailPos;
                areg::destroy_elems<VALUE>(block, 1);
                areg::construct_elems<VALUE>(block, 1);
                *block = newElement;
            }
            // else capacity == 0 => nothing to do
            break;

        case areg::OverlapPolicy::Resize:
            // grow buffer (double or at least 1)
            if ( reserve(static_cast<uint32_t>(mCapacity != 0 ? mCapacity : 1) * 2) >= (mElemCount + 1) )
            {
                ASSERT(mCapacity >= mElemCount + 1u);
                mTailPos = (mTailPos + 1u) % mCapacity;
                VALUE * block = mStackList + mTailPos;
                areg::construct_elems<VALUE>(block, 1);
                *block = newElement;
                ++ mElemCount;
            }
            break;

        case areg::OverlapPolicy::Stop:
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
    ASSERT( mElemCount != 0u );
    VALUE result{ };

    if (mElemCount == 0u)
        return result;

    ASSERT( mCapacity != 0 );
    ASSERT( mStackList != nullptr );
    ASSERT((mHeadPos != mTailPos) || (mElemCount == 1u));

    result = std::move(mStackList[mHeadPos]);
    areg::destroy_elems<VALUE>( mStackList + mHeadPos, 1 );
    mHeadPos = (mHeadPos + 1u) % mCapacity;
    -- mElemCount;

    if (mElemCount == 0u)
    {
        mHeadPos = mTailPos = 0u;
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

    if (mCapacity >= newCapacity)
        return mCapacity;

    VALUE* newList = newCapacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW uint8_t[newCapacity * sizeof(VALUE)]) : nullptr;
    if (newList == nullptr)
        return mCapacity;

    uint32_t elemCount = mElemCount;
    if (mStackList != nullptr)
    {
        _copy_elems(newList, mStackList, mHeadPos, mTailPos, elemCount, mCapacity);
        _empty_stack();
        delete[] reinterpret_cast<uint8_t*>(mStackList);
    }

    mStackList = newList;
    mHeadPos = 0u;
    mTailPos = (elemCount == 0u ? 0u : (elemCount - 1u));
    mElemCount = elemCount;
    mCapacity = newCapacity;

    return mCapacity;
}

template <typename VALUE>
void RingStackBase<VALUE>::copy(const RingStackBase<VALUE>& source)
{
    if (static_cast<const RingStackBase<VALUE> *>(this) != &source)
    {
        Lock lock1(mSyncObj);
        Lock lock2(source.mSyncObj);
        _copy_stack(source);
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
uint32_t RingStackBase<VALUE>::find(const VALUE& elem, uint32_t startAt /*= areg::RING_START_POSITION*/) const noexcept
{
    Lock lock(mSyncObj);

    uint32_t result = static_cast<uint32_t>(areg::INVALID_INDEX);
    startAt = startAt == areg::RING_START_POSITION ? 0u : startAt;
    for (uint32_t i = startAt; i < mElemCount; ++i)
    {
        if (elem == mStackList[_norm2_ring_index(i)])
        {
            result = i;
            break;
        }
    }

    return result;
}

template <typename VALUE>
bool RingStackBase<VALUE>::contains(const VALUE& elem, uint32_t startAt /*= areg::RING_START_POSITION*/) const noexcept
{
    return (find(elem, startAt) != static_cast<uint32_t>(areg::INVALID_INDEX));
}

template <typename VALUE>
void RingStackBase<VALUE>::_empty_stack() noexcept
{
    // do not delete stack, only remove elements and reset data.
    // keep capacity same
    for (uint32_t i = 0; i < mElemCount; ++ i)
    {
        areg::destroy_elems<VALUE>( mStackList + mHeadPos, 1 );
        mHeadPos = (mHeadPos + 1 ) % mCapacity;
    }

    mHeadPos    = 0;
    mTailPos    = 0;
    mElemCount  = 0;
}

template <typename VALUE>
void RingStackBase<VALUE>::_copy_elems(VALUE* dst, VALUE* src, uint32_t srcStart, uint32_t srcEnd, uint32_t srcCount, uint32_t srcCapacity)
{
    ASSERT(srcCapacity >= srcCount);
    ASSERT((dst != nullptr) && (src != nullptr));

    if (srcCount == 0)
        return;

    areg::construct_elems<VALUE>(static_cast<void*>(dst), srcCount);
    if (srcEnd >= srcStart)
    {
        ASSERT((srcEnd - srcStart + 1u) == srcCount);
        areg::copy_elems<VALUE>(dst, src + srcStart, srcCount);
    }
    else
    {
        uint32_t elemCopy = srcCapacity - srcStart;
        ASSERT((elemCopy + srcEnd + 1u) == srcCount);
        areg::copy_elems<VALUE>(dst, src + srcStart, elemCopy);
        areg::copy_elems<VALUE>(dst + elemCopy, src, srcEnd + 1);
    }
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::_norm2_ring_index(uint32_t index) const noexcept
{
    return (mCapacity != 0 ? ((mHeadPos + index) % mCapacity) : 0u);
}

template <typename VALUE>
uint32_t RingStackBase<VALUE>::_ring2_norm_index(uint32_t ring) const noexcept
{
    if (mCapacity == 0)
        return 0u;

    return (ring >= mHeadPos ? ring - mHeadPos : (mCapacity - mHeadPos) + ring);
}

template <typename VALUE>
areg::Ordering RingStackBase<VALUE>::_compare_rings( const VALUE* left, uint32_t leftStart, uint32_t leftCapacity, uint32_t leftCount
                                                   , const VALUE* right, uint32_t rightStart, uint32_t rightCapacity, uint32_t rightCount) const noexcept
{
    ASSERT((leftStart < leftCapacity) && (rightStart < rightCapacity));
    areg::Ordering result{ areg::Ordering::Equal };
    uint32_t count = std::min(leftCount, rightCount);
    while (count != 0u)
    {
        result = areg::compare<VALUE>(left[leftStart], right[rightStart]);
        if (result != areg::Ordering::Equal)
        {
            return result;
        }

        leftStart  = (leftStart  + 1u) % leftCapacity;
        rightStart = (rightStart + 1u) % rightCapacity;
        --count;
    }

    return areg::compare<uint32_t>(leftCount, rightCount);
}

template <typename VALUE>
void RingStackBase<VALUE>::_copy_stack(const RingStackBase<VALUE>& source)
{
    _empty_stack();
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
            _copy_elems(newList, source.mStackList, source.mHeadPos, source.mTailPos, source.mElemCount, srcCapacity);
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
ConcurrentRingStack<VALUE>::ConcurrentRingStack(uint32_t initCapacity /*= 0*/, areg::OverlapPolicy onOverlap /*= areg::OverlapPolicy::Stop*/ )
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
RingStack<VALUE>::RingStack(uint32_t initCapacity /*= 0*/, areg::OverlapPolicy onOverlap /*= areg::OverlapPolicy::Stop*/ )
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
const areg::InStream & operator >> ( const areg::InStream & stream, areg::RingStackBase<V> & input )
{
    areg::Lock lock(input.mSyncObj);

    uint32_t size{ 0u };
    stream >> size;

    input.clear();
    if ((input.reserve(size)) >= size && (size != 0))
    {
        areg::construct_elems<V>(input.mStackList, size);
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
areg::OutStream & operator << (areg::OutStream & stream, const areg::RingStackBase<V> & output )
{
    areg::Lock lock(output.mSyncObj);

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

} // namespace areg

#endif  // AREG_BASE_RINGSTACK_HPP
