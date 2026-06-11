#ifndef AREG_BASE_STACK_HPP
#define AREG_BASE_STACK_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/Stack.hpp
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

#include <algorithm>
#include <deque>
#include <utility>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// StackBase<VALUE, SYNC> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronized FIFO stack container that requires a synchronization object (lock) to
 *          manage thread-safe access.
 **/
template <typename VALUE, typename SYNC = NolockSyncObject>
class StackBase   : private Constless<std::deque<VALUE>>
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////

public:
    using STACKPOS = typename std::deque<VALUE>::iterator;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes stack with a synchronization object. Protected to prevent direct
     *          instantiation; use Stack or ConcurrentStack instead.
     **/
    StackBase() = default;

    StackBase( const StackBase<VALUE, SYNC> & source );

    StackBase( StackBase<VALUE, SYNC> && source ) noexcept;

    /**
     * \brief   Initializes stack with a synchronization object and copies elements from the given array.
     *
     * \param   list    Array of elements to copy into the stack.
     * \param   count   The number of elements in the array.
     **/
    StackBase(const VALUE* list, uint32_t count);

    ~StackBase() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies all elements from source stack, replacing existing contents.
     *
     * \param   source      The source stack to copy from.
     * \return  Reference to this stack object.
     **/
    inline StackBase<VALUE, SYNC> & operator = ( const StackBase<VALUE, SYNC> & source );

    /**
     * \brief   Moves all elements from source stack, replacing existing contents.
     *
     * \param   source      The source stack to move from.
     * \return  Reference to this stack object.
     * \note    Move overload.
     **/
    inline StackBase<VALUE, SYNC> & operator = ( StackBase<VALUE, SYNC> && source ) noexcept;

    /**
     * \brief   Returns the constant reference to element at the specified position.
     **/
    [[nodiscard]]
    inline const VALUE& operator [] (STACKPOS atPosition) const noexcept;
    /**
     * \brief   Returns the mutable reference to element at the specified position. Allows
     *          modification of the returned value.
     **/
    [[nodiscard]]
    inline VALUE& operator [] (STACKPOS atPosition) noexcept;

    /**
     * \brief   Returns true if two stacks are equal; false otherwise.
     *
     * \param   other       The stack to compare with (same sync policy).
     * \return  True if stacks contain identical elements in the same order; false otherwise.
     **/
    [[nodiscard]]
    inline bool operator == (const StackBase<VALUE, SYNC> & other) const noexcept;

    /**
     * \brief   Returns true if two stacks are not equal; false otherwise.
     *
     * \param   other       The stack to compare with (same sync policy).
     * \return  True if stacks differ; false otherwise.
     **/
    [[nodiscard]]
    inline bool operator != (const StackBase<VALUE, SYNC> & other) const noexcept;

    /**
     * \brief   Returns true if two stacks with different sync policies are equal.
     *
     * Compares element values only; sync policy differences are ignored.
     * Allows comparing areg::Stack<T> with areg::ConcurrentStack<T>.
     *
     * \param   other       The stack to compare with (different sync policy).
     * \return  True if stacks contain identical elements in the same order; false otherwise.
     **/
    template <typename SYNC2>
    [[nodiscard]]
    inline bool operator == (const StackBase<VALUE, SYNC2> & other) const noexcept;

    /**
     * \brief   Returns true if two stacks with different sync policies are not equal.
     *
     * \param   other       The stack to compare with (different sync policy).
     * \return  True if stacks differ; false otherwise.
     **/
    template <typename SYNC2>
    [[nodiscard]]
    inline bool operator != (const StackBase<VALUE, SYNC2> & other) const noexcept;

public:
/************************************************************************/
// Friend global operators to make Stack streamable
/************************************************************************/

    /**
     * \brief   Deserializes stack values from the input stream. Replaces existing contents. VALUE
     *          type must support the streaming operator.
     *
     * \param   stream      The input stream to read from.
     * \param   input       The stack to receive deserialized values.
     * \return  Reference to the input stream.
     **/
    template<typename V, typename S>
    friend const InStream & operator >> (const InStream & stream, StackBase<V, S> & input);
    /**
     * \brief   Serializes all stack values to the output stream, starting from the head. VALUE type
     *          must support the streaming operator.
     *
     * \param   stream      The output stream to write to.
     * \param   output      The stack to serialize.
     * \return  Reference to the output stream.
     **/
    template<typename V, typename S>
    friend OutStream & operator << (OutStream & stream, const StackBase<V, S> & output);

//////////////////////////////////////////////////////////////////////////
// Operations and Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns size of the stack.
     **/
    [[nodiscard]]
    inline uint32_t size() const noexcept;

    /**
     * \brief   Returns true if the stack is empty and has no elements.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Returns true if the specified position points to the beginning of the stack.
     *
     * \param   pos     The position to check.
     * \return  True if position is the first; false otherwise.
     **/
    [[nodiscard]]
    inline bool is_first_position(const STACKPOS pos) const noexcept;

    /**
     * \brief   Returns true if the specified position points to the end of the stack.
     *
     * \param   pos     The position to check.
     * \return  True if position is the last; false otherwise.
     **/
    [[nodiscard]]
    inline bool is_last_position(const STACKPOS pos) const noexcept;

    /**
     * \brief   Returns the invalid position value.
     **/
    [[nodiscard]]
    STACKPOS invalid_position() const noexcept;

    /**
     * \brief   Returns true if the given position is valid and not pointing to the end of the stack.
     *
     * \param   pos     The position to validate.
     * \return  True if position is valid; false otherwise.
     **/
    [[nodiscard]]
    bool is_valid_position(const STACKPOS pos) const noexcept;

    /**
     * \brief   Validates that the specified position points to a valid entry in the stack.
     *
     * \param   pos     The position to check.
     * \return  True if the position points to a valid entry; false otherwise.
     **/
    [[nodiscard]]
    inline bool check_position(const STACKPOS pos) const noexcept;

    /**
     * \brief   Returns true if the element is found in the stack. VALUE type must have comparison
     *          operators defined.
     *
     * \param   elemSearch      The element to search for.
     * \return  True if element is found; false otherwise.
     **/
    [[nodiscard]]
    inline bool contains(const VALUE& elemSearch) const noexcept;

    /**
     * \brief   Returns true if the element is found in the stack, starting from the specified position.
     *
     * \param   elemSearch      The element to search for.
     * \param   startAt         The position to start searching from.
     * \return  True if element is found from the start position onward; false otherwise.
     **/
    [[nodiscard]]
    inline bool contains(const VALUE& elemSearch, STACKPOS startAt) const noexcept;

    /**
     * \brief   Returns stack std::deque object.
     **/
    [[nodiscard]]
    inline const std::deque<VALUE>& data() const noexcept;

/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief   Removes all elements from the stack, setting size to zero.
     **/
    inline void clear() noexcept;

    /**
     * \brief   Deletes extra unused capacity in the stack, reducing memory usage.
     **/
    inline void free_extra();

    /**
     * \brief   Sets the size of the stack to zero and deletes all capacity space.
     **/
    inline void release();

    /**
     * \brief   Acquires the synchronization lock. If NolockSyncObject is used, returns immediately
     *          without blocking.
     *
     * \return  True if lock acquired successfully; false otherwise.
     **/
    inline bool lock() const;

    /**
     * \brief   Releases the synchronization lock. Has no effect if NolockSyncObject is used.
     *
     * \return  True if unlock successful; false otherwise.
     **/
    inline bool unlock() const;

    /**
     * \brief   Changes the stack size. Increases capacity and initializes new elements, or
     *          truncates if shrinking. VALUE type must have a default constructor.
     *
     * \param   newSize     The new size. If zero, the stack is emptied.
     **/
    inline void resize(uint32_t newSize);

    /**
     * \brief   Returns the first inserted element without removing it. Caller must ensure the stack is not empty.
     **/
    [[nodiscard]]
    inline const VALUE & first_entry() const noexcept;

    /**
     * \brief   Returns the last inserted element without removing it. Caller must ensure the stack is not empty.
     **/
    [[nodiscard]]
    inline const VALUE & last_entry() const noexcept;

    /**
     * \brief   Adds a new element to the end of the stack.
     *
     * \param   newElement      The element to add.
     * \return  The new size of the stack.
     **/
    inline uint32_t push_last( const VALUE & newElement );
    /**
     * \brief   Adds a new element to the end of the stack.
     *
     * \param   newElement      The element to add.
     * \return  The new size of the stack.
     * \note    Move overload. Takes ownership of the element.
     **/
    inline uint32_t push_last(VALUE && newElement);

    /**
     * \brief   Adds a new element to the beginning of the stack; pops first when removing.
     *
     * \param   newElement      The element to add.
     * \return  The new size of the stack.
     **/
    inline uint32_t push_first( const VALUE & newElement );
    /**
     * \brief   Adds a new element to the beginning of the stack; pops first when removing.
     *
     * \param   newElement      The element to add.
     * \return  The new size of the stack.
     * \note    Move overload. Takes ownership of the element.
     **/
    inline uint32_t push_first( VALUE && newElement);

    /**
     * \brief   Removes and returns the first element. Stack must not be empty; assertion fails otherwise.
     *
     * \return  The removed element.
     **/
    inline VALUE pop_first() noexcept;

    /**
     * \brief   Copies all elements from source stack. Existing contents are replaced.
     *
     * \param   source      The source stack to copy from.
     * \return  The number of copied elements.
     **/
    inline uint32_t copy( const StackBase<VALUE, SYNC> & source );

    /**
     * \brief   Moves all elements from source stack. Existing contents are replaced.
     *
     * \param   source      The source stack to move from.
     * \return  The number of moved elements.
     **/
    inline uint32_t move( StackBase<VALUE, SYNC> && source ) noexcept;

    /**
     * \brief   Searches for the element in the stack from the beginning.
     *
     * \param   Value       The element value to search for.
     * \return  Valid position if found; invalid position otherwise.
     **/
    [[nodiscard]]
    inline STACKPOS find(const VALUE & Value) const noexcept;

    /**
     * \brief   Searches for the element in the stack, starting after the specified position.
     *
     * \param   Value           The element value to search for.
     * \param   searchAfter     If valid, search starts from the next element after this position.
     *                          If invalid, search starts from the beginning.
     * \return  Valid position if found; invalid position otherwise.
     **/
    [[nodiscard]]
    inline STACKPOS find(const VALUE& Value, STACKPOS searchAfter) const noexcept;

    /**
     * \brief   Returns the valid position of the element of zero-based index.
     **/
    [[nodiscard]]
    inline STACKPOS first_position() const noexcept;

    /**
     * \brief   Returns the constant reference to the element at the specified position.
     *
     * \param   atPosition      The valid position in the stack.
     * \return  Constant reference to the element.
     **/
    [[nodiscard]]
    inline const VALUE& value_at( const STACKPOS atPosition ) const noexcept;

    /**
     * \brief   Returns the mutable reference to the element at the specified position. Allows modification.
     *
     * \param   atPosition      The valid position in the stack.
     * \return  Mutable reference to the element.
     **/
    [[nodiscard]]
    inline VALUE& value_at( STACKPOS atPosition ) noexcept;

    /**
     * \brief   Returns the next valid position after the given one, or an invalid position if at the end.
     *
     * \param   pos     The current position.
     * \return  Valid position if one exists after the given position; invalid position if at the end.
     **/
    [[nodiscard]]
    inline STACKPOS next_position( STACKPOS pos ) const noexcept;

    /**
     * \brief   Sorts the stack using the provided comparison function. Returns this stack  chaining.
     *
     * \param   comp    The comparison function, similar to std::greater().
     * \return  Reference to this stack object.
     **/
    template <class Compare>
    inline StackBase<VALUE, SYNC>& sort(Compare comp);

    /**
     * \brief   Copies elements from the stack into the provided buffer. If elemCount is smaller
     *          than the stack size, only the first elemCount elements are copied.
     *
     * \param[in,out] list      Pre-allocated buffer where elements will be copied. Must be
     *                          large enough to hold elemCount elements.
     * \param   elemCount       Maximum number of elements to copy. If zero, no elements are copied.
     * \return  The number of elements successfully copied.
     **/
    inline uint32_t elements(VALUE* list, uint32_t elemCount);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    Lockable& lockable() const  noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    //! The stack object, which contains the elements.
    std::deque<VALUE>   mValueList;

    //! The instance of synchronization object to be used to make object thread-safe.
    mutable SYNC        mSyncObject;
};

template <typename VALUE> using ConcurrentStack = StackBase<VALUE, ResourceLock>;
template <typename VALUE> using Stack           = StackBase<VALUE, NolockSyncObject>;

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// StackBase<VALUE, SYNC> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename SYNC>
StackBase<VALUE, SYNC>::StackBase( const StackBase<VALUE, SYNC> & source )
    : Constless<std::deque<VALUE>>( )
    , mValueList    (source.mValueList)
    , mSyncObject   ( )
{
}

template <typename VALUE, typename SYNC>
StackBase<VALUE, SYNC>::StackBase( StackBase<VALUE, SYNC> && source ) noexcept
    : Constless<std::deque<VALUE>>( )
    , mValueList    (std::move(source.mValueList))
    , mSyncObject   ( )
{
}

template <typename VALUE, typename SYNC>
StackBase<VALUE, SYNC>::StackBase(const VALUE* list, uint32_t count)
    : Constless<std::deque<VALUE>>()
    , mValueList ()
    , mSyncObject()
{
    mValueList.assign(list, list + count);
}

template <typename VALUE, typename SYNC>
inline StackBase<VALUE, SYNC> & StackBase<VALUE, SYNC>::operator = ( const StackBase<VALUE, SYNC> & source )
{
    copy(source);
    return (*this);
}

template <typename VALUE, typename SYNC>
inline StackBase<VALUE, SYNC> & StackBase<VALUE, SYNC>::operator = ( StackBase<VALUE, SYNC> && source ) noexcept
{
    move( std::move(source) );
    return (*this);
}

template <typename VALUE, typename SYNC>
inline const VALUE& StackBase<VALUE, SYNC>::operator [] (STACKPOS atPosition) const noexcept
{
    Lock lock(mSyncObject);
    return (*atPosition);
}

template <typename VALUE, typename SYNC>
inline VALUE& StackBase<VALUE, SYNC>::operator [] (STACKPOS atPosition) noexcept
{
    Lock lock(mSyncObject);
    return (*atPosition);
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::operator == (const StackBase<VALUE, SYNC>& other) const noexcept
{
    Lock lock(mSyncObject);
    Lock lockOther(other.mSyncObject);
    return (mValueList == other.mValueList);
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::operator != (const StackBase<VALUE, SYNC>& other) const noexcept
{
    Lock lock(mSyncObject);
    Lock lockOther(other.mSyncObject);
    return (mValueList != other.mValueList);
}

template <typename VALUE, typename SYNC>
template <typename SYNC2>
inline bool StackBase<VALUE, SYNC>::operator == (const StackBase<VALUE, SYNC2>& other) const noexcept
{
    Lock lock(mSyncObject);
    return (mValueList == other.data());
}

template <typename VALUE, typename SYNC>
template <typename SYNC2>
inline bool StackBase<VALUE, SYNC>::operator != (const StackBase<VALUE, SYNC2>& other) const noexcept
{
    Lock lock(mSyncObject);
    return (mValueList != other.data());
}

template <typename VALUE, typename SYNC>
inline uint32_t StackBase<VALUE, SYNC>::size() const noexcept
{
    Lock lock( mSyncObject );
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::is_empty() const noexcept
{
    Lock lock( mSyncObject );
    return mValueList.empty();
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::is_first_position(STACKPOS pos) const noexcept
{
    Lock lock(mSyncObject);
    return (pos == mValueList.begin());
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::is_last_position(STACKPOS pos) const noexcept
{
    Lock lock(mSyncObject);
    return (mValueList.empty() == false) && (pos == --mValueList.end());
}

template <typename VALUE, typename SYNC>
inline typename StackBase<VALUE, SYNC>::STACKPOS StackBase<VALUE, SYNC>::invalid_position() const noexcept
{
    Lock lock(mSyncObject);
    auto end = mValueList.end();
    return Constless<std::deque<VALUE>>::iter(mValueList, end);
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::is_valid_position(STACKPOS pos) const noexcept
{
    Lock lock(mSyncObject);
    return (pos != mValueList.end());
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::check_position(STACKPOS pos) const noexcept
{
    Lock lock(mSyncObject);
    auto it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::contains(const VALUE& elemSearch) const noexcept
{
    return contains(elemSearch, first_position());
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::contains(const VALUE& elemSearch, STACKPOS startAt) const noexcept
{
    typename std::deque<VALUE>::const_iterator cit{ startAt };
    return (cit != mValueList.cend()) && (std::find(cit, mValueList.cend(), elemSearch) != mValueList.cend());
}

template <typename VALUE, typename SYNC>
inline const std::deque<VALUE>& StackBase<VALUE, SYNC>::data() const noexcept
{
    return mValueList;
}

template <typename VALUE, typename SYNC>
inline void StackBase<VALUE, SYNC>::clear() noexcept
{
    Lock lock(mSyncObject);
    mValueList.clear();
}

template <typename VALUE, typename SYNC>
inline void StackBase<VALUE, SYNC>::free_extra()
{
    Lock lock(mSyncObject);
    mValueList.shrink_to_fit();
}

template <typename VALUE, typename SYNC>
inline void StackBase<VALUE, SYNC>::release()
{
    Lock lock(mSyncObject);
    mValueList.clear();
    mValueList.shrink_to_fit();
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::lock() const
{
    return mSyncObject.lock(areg::WAIT_INFINITE);
}

template <typename VALUE, typename SYNC>
inline bool StackBase<VALUE, SYNC>::unlock() const
{
    return mSyncObject.unlock();
}

template <typename VALUE, typename SYNC>
inline void StackBase<VALUE, SYNC>::resize(uint32_t newSize)
{
    Lock lock(mSyncObject);
    mValueList.resize(newSize > areg::MAX_CONTAINER_SIZE ? areg::MAX_CONTAINER_SIZE : newSize);
}

template <typename VALUE, typename SYNC>
inline const VALUE & StackBase<VALUE, SYNC>::first_entry() const noexcept
{
    Lock lock(mSyncObject);
    return mValueList.front();
}

template <typename VALUE, typename SYNC>
inline const VALUE & StackBase<VALUE, SYNC>::last_entry() const noexcept
{
    Lock lock(mSyncObject);
    return mValueList.back();
}

template <typename VALUE, typename SYNC>
inline uint32_t StackBase<VALUE, SYNC>::copy( const StackBase<VALUE, SYNC> & source )
{
    Lock lock(mSyncObject);
    Lock lockSource(source.mSyncObject);

    mValueList = source.mValueList;
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, typename SYNC>
inline uint32_t StackBase<VALUE, SYNC>::move( StackBase<VALUE, SYNC> && source ) noexcept
{
    Lock lock(mSyncObject);
    Lock lockSource(source.mSyncObject);

    mValueList = std::move(source.mValueList);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, typename SYNC>
inline uint32_t StackBase<VALUE, SYNC>::push_last( const VALUE & newElement )
{
    Lock lock(mSyncObject);
    mValueList.push_back(newElement);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, typename SYNC>
inline uint32_t StackBase<VALUE, SYNC>::push_last(VALUE && newElement)
{
    Lock lock(mSyncObject);
    mValueList.push_back(std::move(newElement));
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, typename SYNC>
inline uint32_t StackBase<VALUE, SYNC>::push_first( const VALUE & newElement )
{
    Lock lock(mSyncObject);
    mValueList.push_front(newElement);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, typename SYNC>
inline uint32_t StackBase<VALUE, SYNC>::push_first(VALUE && newElement)
{
    Lock lock(mSyncObject);
    mValueList.push_front(std::move(newElement));
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, typename SYNC>
VALUE StackBase<VALUE, SYNC>::pop_first() noexcept
{
    Lock lock(mSyncObject);

    VALUE result = std::move(mValueList.front());
    mValueList.pop_front();
    return result;
}

template <typename VALUE, typename SYNC>
inline typename StackBase<VALUE, SYNC>::STACKPOS StackBase<VALUE, SYNC>::find(const VALUE& Value) const noexcept
{
    Lock lock(mSyncObject);
    auto it = std::find(mValueList.begin(), mValueList.end(), Value);
    return Constless<std::deque<VALUE>>::iter(mValueList, it);
}

template <typename VALUE, typename SYNC>
inline typename StackBase<VALUE, SYNC>::STACKPOS StackBase<VALUE, SYNC>::find(const VALUE & Value, STACKPOS searchAfter ) const noexcept
{
    Lock lock(mSyncObject);
    typename std::deque<VALUE>::iterator endIt{ Constless<std::deque<VALUE>>::iter(mValueList, mValueList.end()) };
    if (searchAfter == endIt)
        return endIt;

    typename std::deque<VALUE>::iterator it{ Constless<std::deque<VALUE>>::iter(mValueList, ++searchAfter) };
    return Constless<std::deque<VALUE>>::iter(mValueList, std::find(it, endIt, Value));
}

template <typename VALUE, typename SYNC>
inline typename StackBase<VALUE, SYNC>::STACKPOS StackBase<VALUE, SYNC>::first_position() const noexcept
{
    Lock lock(mSyncObject);
    auto it = mValueList.begin();
    return Constless<std::deque<VALUE>>::iter(mValueList, it);
}

template <typename VALUE, typename SYNC>
inline const VALUE & StackBase<VALUE, SYNC>::value_at( const STACKPOS atPosition ) const noexcept
{
    Lock lock( mSyncObject );

    ASSERT( atPosition != mValueList.end( ) );
    return (*atPosition);
}

template <typename VALUE, typename SYNC>
inline VALUE & StackBase<VALUE, SYNC>::value_at( STACKPOS atPosition ) noexcept
{
    Lock lock( mSyncObject );

    ASSERT( atPosition != mValueList.end( ) );
    return (*atPosition);
}

template <typename VALUE, typename SYNC>
inline typename StackBase<VALUE, SYNC>::STACKPOS StackBase<VALUE, SYNC>::next_position( STACKPOS pos ) const noexcept
{
    Lock lock(mSyncObject);

    ASSERT(pos != mValueList.end());
    return (++pos);
}

template <typename VALUE, typename SYNC>
inline uint32_t StackBase<VALUE, SYNC>::elements(VALUE* list, uint32_t elemCount)
{
    uint32_t result{ std::min(static_cast<uint32_t>(mValueList.size()), elemCount) };
    uint32_t i = 0;
    for (const auto& entry : mValueList)
    {
        list[i++] = entry;
        if (i == result)
        {
            break;
        }
    }

    return result;
}

template <typename VALUE, typename SYNC>
template<class Compare>
inline StackBase<VALUE, SYNC>& StackBase<VALUE, SYNC>::sort(Compare comp)
{
    std::sort(mValueList.begin(), mValueList.end(), comp);
    return (*this);
}

template <typename VALUE, typename SYNC>
Lockable& StackBase<VALUE, SYNC>::lockable() const  noexcept
{
    return mSyncObject;
}

//////////////////////////////////////////////////////////////////////////
// StackBase<VALUE, SYNC> friend operators implementation
//////////////////////////////////////////////////////////////////////////
template<typename V, typename S>
const areg::InStream & operator >> ( const areg::InStream & stream, areg::StackBase<V, S> & input )
{
    areg::Lock lock(input.mSyncObject);
    return (stream >> input.mValueList);
}

template<typename V, typename S>
areg::OutStream & operator << (areg::OutStream & stream, const areg::StackBase<V, S> & output )
{
    areg::Lock lock(output.mSyncObject);
    return (stream << output.mValueList);
}

} // namespace areg

#endif  // AREG_BASE_STACK_HPP
