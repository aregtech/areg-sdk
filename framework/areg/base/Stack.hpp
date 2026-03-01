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
#include <algorithm>
#include <deque>

#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/IOStream.hpp"

#include <algorithm>
#include <utility>
namespace areg {

/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
template <typename VALUE> class StackBase;
    template <typename VALUE> class ConcurrentStack;
    template <typename VALUE> class Stack;

//////////////////////////////////////////////////////////////////////////
// StackBase<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronized FIFO stack container that requires a synchronization object (lock) to
 *          manage thread-safe access.
 **/
template <typename VALUE>
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
protected:
    /**
     * \brief   Initializes stack with a synchronization object. Protected to prevent direct
     *          instantiation; use Stack or ConcurrentStack instead.
     *
     * \param   syncObject      The synchronization object to manage thread-safe access.
     **/
    explicit StackBase( Lockable & syncObject );

    /**
     * \brief   Initializes stack with a synchronization object and copies elements from source.
     *
     * \param   syncObject      The synchronization object to manage thread-safe access.
     * \param   source          The source stack from which to copy elements.
     **/
    StackBase( Lockable & syncObject, const StackBase<VALUE> & source );

    /**
     * \brief   Initializes stack with a synchronization object and moves elements from source.
     *
     * \param   syncObject      The synchronization object to manage thread-safe access.
     * \param   source          The source stack from which to move elements.
     * \note    Move constructor.
     **/
    StackBase( Lockable & syncObject, StackBase<VALUE> && source ) noexcept;

    /**
     * \brief   Initializes stack with a synchronization object and copies elements from the given
     *          array.
     *
     * \param   syncObject      The synchronization object to manage thread-safe access.
     * \param   list            Array of elements to copy into the stack.
     * \param   count           The number of elements in the array.
     **/
    StackBase(Lockable& syncObject, const VALUE* list, uint32_t count);

    /**
     * \brief   Destructor. Public
     **/
    ~StackBase();

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
    inline StackBase<VALUE> & operator = ( const StackBase<VALUE> & source );

    /**
     * \brief   Moves all elements from source stack, replacing existing contents.
     *
     * \param   source      The source stack to move from.
     * \return  Reference to this stack object.
     * \note    Move overload.
     **/
    inline StackBase<VALUE> & operator = ( StackBase<VALUE> && source ) noexcept;

    /**
     * \brief   Returns the constant reference to element at the specified position.
     **/
    inline const VALUE& operator [] (STACKPOS atPosition) const;
    /**
     * \brief   Returns the mutable reference to element at the specified position. Allows
     *          modification of the returned value.
     **/
    inline VALUE& operator [] (STACKPOS atPosition);

    /**
     * \brief   Returns true if two stacks are equal; false otherwise.
     *
     * \param   other       The stack to compare with.
     * \return  True if stacks contain identical elements in the same order; false otherwise.
     **/
    inline bool operator == (const StackBase<VALUE> & other) const;

    /**
     * \brief   Returns true if two stacks are not equal; false otherwise.
     *
     * \param   other       The stack to compare with.
     * \return  True if stacks differ; false otherwise.
     **/
    inline bool operator != (const StackBase<VALUE> & other) const;

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
    template<typename V>
    friend const InStream & operator >> (const InStream & stream, StackBase<V> & input);
    /**
     * \brief   Serializes all stack values to the output stream, starting from the head. VALUE type
     *          must support the streaming operator.
     *
     * \param   stream      The output stream to write to.
     * \param   output      The stack to serialize.
     * \return  Reference to the output stream.
     **/
    template<typename V>
    friend OutStream & operator << (OutStream & stream, const StackBase<V> & output);

//////////////////////////////////////////////////////////////////////////
// Operations and Attributes
//////////////////////////////////////////////////////////////////////////
public:

    inline uint32_t size() const;

    inline bool is_empty() const;

    /**
     * \brief   Returns true if the specified position points to the beginning of the stack.
     *
     * \param   pos     The position to check.
     * \return  True if position is the first; false otherwise.
     **/
    inline bool is_first_position(const STACKPOS pos) const;

    /**
     * \brief   Returns true if the specified position points to the end of the stack.
     *
     * \param   pos     The position to check.
     * \return  True if position is the last; false otherwise.
     **/
    inline bool is_last_position(const STACKPOS pos) const;

    STACKPOS invalid_position() const;

    /**
     * \brief   Returns true if the given position is valid and not pointing to the end of the
     *          stack.
     *
     * \param   pos     The position to validate.
     * \return  True if position is valid; false otherwise.
     **/
    bool is_valid_position(const STACKPOS pos) const;

    /**
     * \brief   Returns true if the specified position is invalid or points to the end of the stack.
     *
     * \param   pos     The position to check.
     * \return  True if position is invalid; false otherwise.
     **/
    bool is_invalid_position(const STACKPOS pos) const;

    /**
     * \brief   Validates that the specified position points to a valid entry in the stack.
     *
     * \param   pos     The position to check.
     * \return  True if the position points to a valid entry; false otherwise.
     **/
    inline bool check_position(const STACKPOS pos) const;

    /**
     * \brief   Returns true if the element is found in the stack. VALUE type must have comparison
     *          operators defined.
     *
     * \param   elemSearch      The element to search for.
     * \return  True if element is found; false otherwise.
     **/
    inline bool contains(const VALUE& elemSearch) const;

    /**
     * \brief   Returns true if the element is found in the stack, starting from the specified
     *          position.
     *
     * \param   elemSearch      The element to search for.
     * \param   startAt         The position to start searching from.
     * \return  True if element is found from the start position onward; false otherwise.
     **/
    inline bool contains(const VALUE& elemSearch, STACKPOS startAt) const;

    inline const std::deque<VALUE>& data() const;

/************************************************************************/
// Operations
/************************************************************************/

    inline void clear();

    inline void free_extra();

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
     * \brief   Returns the first inserted element without removing it. Caller must ensure the stack
     *          is not empty.
     **/
    inline const VALUE & first_entry() const;

    /**
     * \brief   Returns the last inserted element without removing it. Caller must ensure the stack
     *          is not empty.
     **/
    inline const VALUE & last_entry() const;

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
     * \brief   Removes and returns the first element. Stack must not be empty; assertion fails
     *          otherwise.
     *
     * \return  The removed element.
     **/
    inline VALUE pop_first();

    /**
     * \brief   Copies all elements from source stack. Existing contents are replaced.
     *
     * \param   source      The source stack to copy from.
     * \return  The number of copied elements.
     **/
    inline uint32_t copy( const StackBase<VALUE> & source );

    /**
     * \brief   Moves all elements from source stack. Existing contents are replaced.
     *
     * \param   source      The source stack to move from.
     * \return  The number of moved elements.
     **/
    inline uint32_t move( StackBase<VALUE> && source ) noexcept;

    /**
     * \brief   Searches for the element in the stack from the beginning.
     *
     * \param   Value       The element value to search for.
     * \return  Valid position if found; invalid position otherwise.
     **/
    inline STACKPOS find(const VALUE & Value) const;

    /**
     * \brief   Searches for the element in the stack, starting after the specified position.
     *
     * \param   Value           The element value to search for.
     * \param   searchAfter     If valid, search starts from the next element after this position.
     *                          If invalid, search starts from the beginning.
     * \return  Valid position if found; invalid position otherwise.
     **/
    inline STACKPOS find(const VALUE& Value, STACKPOS searchAfter) const;

    inline STACKPOS first_position() const;

    /**
     * \brief   Returns the constant reference to the element at the given position. Position must
     *          be valid.
     *
     * \param   pos     The valid position in the stack.
     * \return  Constant reference to the element at the position.
     **/
    inline const VALUE & at( const STACKPOS pos ) const;
    /**
     * \brief   Returns the mutable reference to the element at the given position. Allows
     *          modification. Position must be valid.
     *
     * \param   pos     The valid position in the stack.
     * \return  Mutable reference to the element at the position.
     **/
    inline VALUE& at(STACKPOS pos);


    /**
     * \brief   Returns the constant reference to the element at the specified position.
     *
     * \param   atPosition      The valid position in the stack.
     * \return  Constant reference to the element.
     **/
    inline const VALUE& value_at_position( const STACKPOS atPosition ) const;
    /**
     * \brief   Returns the mutable reference to the element at the specified position. Allows
     *          modification.
     *
     * \param   atPosition      The valid position in the stack.
     * \return  Mutable reference to the element.
     **/
    inline VALUE& value_at_position( STACKPOS atPosition );

    /**
     * \brief   Returns the next valid position after the given one, or an invalid position if at
     *          the end.
     *
     * \param   pos     The current position.
     * \return  Valid position if one exists after the given position; invalid position if at the
     *          end.
     **/
    inline STACKPOS next_position( STACKPOS pos ) const;

    /**
     * \brief   Sorts the stack using the provided comparison function. Returns this stack for
     *          chaining.
     *
     * \param   comp    The comparison function, similar to std::greater().
     * \return  Reference to this stack object.
     **/
    template <class Compare>
    inline StackBase< VALUE >& sort(Compare comp);

    /**
     * \brief   Copies elements from the stack into the provided buffer. If elemCount is smaller
     *          than the stack size, only the first elemCount elements are copied.
     *
     * \param[in,out] list            Pre-allocated buffer where elements will be copied. Must be
     *                                large enough to hold elemCount elements.
     * \param   elemCount       Maximum number of elements to copy. If zero, no elements are copied.
     * \return  The number of elements successfully copied.
     **/
    inline uint32_t elements(VALUE* list, uint32_t elemCount);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    //! The stack object, which contains the elements.
    std::deque<VALUE>   mValueList;

     //! The instance of synchronization object to be used to make object thread-safe.
    Lockable &    mSyncObject;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    StackBase() = delete;
    StackBase( const StackBase<VALUE> & /* source */ ) = delete;
    StackBase( StackBase<VALUE> && /* source */ ) = delete;
};

//////////////////////////////////////////////////////////////////////////
// ConcurrentStack<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Thread-safe FIFO stack with synchronized data access. Use when multiple threads access
 *          the stack.
 **/
template <typename VALUE>
class ConcurrentStack  : public StackBase<VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty concurrent stack.
     **/
    ConcurrentStack();

    /**
     * \brief   Creates a stack by copying from a base stack.
     *
     * \param   source      The source stack to copy from.
     **/
    explicit ConcurrentStack( const StackBase<VALUE> & source );

    /**
     * \brief   Creates a stack by moving from a base stack.
     *
     * \param   source      The source stack to move from.
     **/
    explicit ConcurrentStack( StackBase<VALUE> && source ) noexcept;

    /**
     * \brief   Copies from another concurrent stack.
     *
     * \param   source      The source stack to copy from.
     **/
    ConcurrentStack( const ConcurrentStack<VALUE> & source );

    /**
     * \brief   Moves from another concurrent stack.
     *
     * \param   source      The source stack to move from.
     **/
    ConcurrentStack( ConcurrentStack<VALUE> && source ) noexcept;

    /**
     * \brief   Creates a stack from an array of elements.
     *
     * \param   list        Array of elements to copy.
     * \param   count       Number of elements in the array.
     **/
    ConcurrentStack(const VALUE* list, uint32_t count);

    /**
     * \brief   Destructor
     **/
    ~ConcurrentStack() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies entries from source. Previous entries are discarded.
     *
     * \param   source      The source stack.
     * \return  Reference to this object.
     **/
    inline ConcurrentStack<VALUE> & operator = ( const ConcurrentStack<VALUE> & source );

    /**
     * \brief   Moves entries from source. Previous entries are discarded.
     *
     * \param   source      The source stack.
     * \return  Reference to this object.
     **/
    inline ConcurrentStack<VALUE> & operator = ( ConcurrentStack<VALUE> && source ) noexcept;

    /**
     * \brief   Copies entries from source. Previous entries are discarded.
     *
     * \param   source      The source stack.
     * \return  Reference to this object.
     **/
    inline ConcurrentStack<VALUE> & operator = ( const StackBase<VALUE> & source );

    /**
     * \brief   Moves entries from source. Previous entries are discarded.
     *
     * \param   source      The source stack.
     * \return  Reference to this object.
     **/
    inline ConcurrentStack<VALUE> & operator = ( StackBase<VALUE> && source ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Resource lock synchronization object.
     **/
    mutable ResourceLock    mLock;
};

//////////////////////////////////////////////////////////////////////////
// Stack<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Non-blocking FIFO stack without synchronization. Not thread-safe. Use when a single
 *          thread accesses the stack.
 **/
template <typename VALUE>
class Stack    : public StackBase<VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty stack.
     **/
    Stack();

    /**
     * \brief   Creates a stack by copying from a base stack.
     *
     * \param   source      The source stack to copy from.
     **/
    explicit Stack( const StackBase<VALUE> & source );

    /**
     * \brief   Creates a stack by moving from a base stack.
     *
     * \param   source      The source stack to move from.
     **/
    explicit Stack( StackBase<VALUE> && source ) noexcept;

    /**
     * \brief   Copies from another stack.
     *
     * \param   source      The source stack to copy from.
     **/
    Stack( const Stack<VALUE> & source );

    /**
     * \brief   Moves from another stack.
     *
     * \param   source      The source stack to move from.
     **/
    Stack( Stack<VALUE> && source ) noexcept;

    /**
     * \brief   Creates a stack from an array of elements.
     *
     * \param   list        Array of elements to copy.
     * \param   count       Number of elements in the array.
     **/
    Stack(const VALUE* list, uint32_t count);

    /**
     * \brief   Destructor
     **/
    ~Stack() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies entries from source. Previous entries are discarded.
     *
     * \param   source      The source stack.
     * \return  Reference to this object.
     **/
    inline Stack<VALUE> & operator = ( const Stack<VALUE> & source );

    /**
     * \brief   Moves entries from source. Previous entries are discarded.
     *
     * \param   source      The source stack.
     * \return  Reference to this object.
     **/
    inline Stack<VALUE> & operator = ( Stack<VALUE> && source ) noexcept;

    /**
     * \brief   Copies entries from source. Previous entries are discarded.
     *
     * \param   source      The source stack.
     * \return  Reference to this object.
     **/
    inline Stack<VALUE> & operator = ( const StackBase<VALUE> & source );

    /**
     * \brief   Moves entries from source. Previous entries are discarded.
     *
     * \param   source      The source stack.
     * \return  Reference to this object.
     **/
    inline Stack<VALUE> & operator = ( StackBase<VALUE> && source ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Synchronization object simulation.
     **/
    mutable NolockSyncObject mNoLock;
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// StackBase<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
StackBase<VALUE>::StackBase( Lockable & syncObject )
    : Constless<std::deque<VALUE>>( )
    , mValueList    ( )
    , mSyncObject   ( syncObject )
{
}

template <typename VALUE>
StackBase<VALUE>::StackBase( Lockable & syncObject, const StackBase<VALUE> & source )
    : Constless<std::deque<VALUE>>( )
    , mValueList    ( )
    , mSyncObject   ( syncObject )
{
    Lock lock(source.mSyncObject);
    mValueList = source.mValueList;
}

template <typename VALUE>
StackBase<VALUE>::StackBase( Lockable & syncObject, StackBase<VALUE> && source ) noexcept
    : Constless<std::deque<VALUE>>( )
    , mValueList    ( )
    , mSyncObject   ( syncObject )
{
    Lock lock(source.mSyncObject);
    mValueList = std::move(source.mValueList);
}

template<typename VALUE>
StackBase<VALUE>::StackBase(Lockable& syncObject, const VALUE* list, uint32_t count)
    : Constless<std::deque<VALUE>>()
    , mValueList ()
    , mSyncObject(syncObject)
{
    mValueList.resize(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        mValueList[i] = list[i];
    }
}

template <typename VALUE>
StackBase<VALUE>::~StackBase()
{
    mSyncObject.lock();
    mValueList.clear();
    mSyncObject.unlock();
}

template <typename VALUE>
inline StackBase<VALUE> & StackBase<VALUE>::operator = ( const StackBase<VALUE> & source )
{
    copy(source);
    return (*this);
}

template <typename VALUE>
inline StackBase<VALUE> & StackBase<VALUE>::operator = ( StackBase<VALUE> && source ) noexcept
{
    move( std::move(source) );
    return (*this);
}

template <typename VALUE>
inline const VALUE& StackBase<VALUE>::operator [] (STACKPOS atPosition) const
{
    Lock lock(mSyncObject);
    return (*atPosition);
}

template <typename VALUE>
inline VALUE& StackBase<VALUE>::operator [] (STACKPOS atPosition)
{
    Lock lock(mSyncObject);
    return (*atPosition);
}

template <typename VALUE>
inline bool StackBase<VALUE>::operator == (const StackBase<VALUE>& other) const
{
    Lock lock(mSyncObject);
    Lock lockOther(other.mSyncObject);
    return (mValueList == other.mValueList);
}

template <typename VALUE>
inline bool StackBase<VALUE>::operator != (const StackBase<VALUE>& other) const
{
    Lock lock(mSyncObject);
    Lock lockOther(other.mSyncObject);
    return (mValueList != other.mValueList);
}

template <typename VALUE>
inline uint32_t StackBase<VALUE>::size() const
{
    Lock lock( mSyncObject );
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline bool StackBase<VALUE>::is_empty() const
{
    Lock lock( mSyncObject );
    return mValueList.empty();
}

template <typename VALUE>
inline bool StackBase<VALUE>::is_first_position(STACKPOS pos) const
{
    Lock lock(mSyncObject);
    return (pos == mValueList.begin());
}

template <typename VALUE>
inline bool StackBase<VALUE>::is_last_position(STACKPOS pos) const
{
    Lock lock(mSyncObject);
    return (mValueList.empty() == false) && (pos == --mValueList.end());
}

template <typename VALUE>
inline typename StackBase<VALUE>::STACKPOS StackBase<VALUE>::invalid_position() const
{
    Lock lock(mSyncObject);
    auto end = mValueList.end();
    return Constless<std::deque<VALUE>>::iter(mValueList, end);
}

template <typename VALUE>
inline bool StackBase<VALUE>::is_valid_position(STACKPOS pos) const
{
    Lock lock(mSyncObject);
    return (pos != mValueList.end());
}

template <typename VALUE>
inline bool StackBase<VALUE>::is_invalid_position(STACKPOS pos) const
{
    Lock lock(mSyncObject);
    return (pos == mValueList.end());
}

template <typename VALUE>
inline bool StackBase<VALUE>::check_position(STACKPOS pos) const
{
    Lock lock(mSyncObject);
    auto it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template<typename VALUE>
inline bool StackBase<VALUE>::contains(const VALUE& elemSearch) const
{
    return contains(elemSearch, first_position());
}

template<typename VALUE>
inline bool StackBase<VALUE>::contains(const VALUE& elemSearch, STACKPOS startAt) const
{
    return (startAt != mValueList.end() ? std::find(startAt, invalid_position(), elemSearch) != mValueList.end() : false);
}

template<typename VALUE>
inline const std::deque<VALUE>& StackBase<VALUE>::data() const
{
    return mValueList;
}

template <typename VALUE>
inline void StackBase<VALUE>::clear()
{
    Lock lock(mSyncObject);
    mValueList.clear();
}

template <typename VALUE>
inline void StackBase<VALUE>::free_extra()
{
    Lock lock(mSyncObject);
    mValueList.shrink_to_fit();
}

template <typename VALUE>
inline void StackBase<VALUE>::release()
{
    Lock lock(mSyncObject);
    mValueList.clear();
    mValueList.shrink_to_fit();
}

template <typename VALUE>
inline bool StackBase<VALUE>::lock() const
{
    return mSyncObject.lock(areg::WAIT_INFINITE);
}

template <typename VALUE>
inline bool StackBase<VALUE>::unlock() const
{
    return mSyncObject.unlock();
}

template<typename VALUE >
inline void StackBase< VALUE >::resize(uint32_t newSize)
{
    Lock lock(mSyncObject);
    mValueList.resize(newSize > areg::MAX_CONTAINER_SIZE ? areg::MAX_CONTAINER_SIZE : newSize);
}

template <typename VALUE>
inline const VALUE & StackBase<VALUE>::first_entry() const
{
    Lock lock(mSyncObject);
    return mValueList.front();
}

template <typename VALUE>
inline const VALUE & StackBase<VALUE>::last_entry() const
{
    Lock lock(mSyncObject);
    return mValueList.back();
}

template <typename VALUE>
inline uint32_t StackBase<VALUE>::copy( const StackBase<VALUE> & source )
{
    Lock lock(mSyncObject);
    Lock lockSource(source.mSyncObject);

    mValueList = source.mValueList;
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t StackBase<VALUE>::move( StackBase<VALUE> && source ) noexcept
{
    Lock lock(mSyncObject);
    Lock lockSource(source.mSyncObject);

    mValueList = std::move(source.mValueList);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t StackBase<VALUE>::push_last( const VALUE & newElement )
{
    Lock lock(mSyncObject);
    mValueList.push_back(newElement);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t StackBase<VALUE>::push_last(VALUE && newElement)
{
    Lock lock(mSyncObject);
    mValueList.push_back(std::move(newElement));
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t StackBase<VALUE>::push_first( const VALUE & newElement )
{
    Lock lock(mSyncObject);
    mValueList.push_front(newElement);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t StackBase<VALUE>::push_first(VALUE && newElement)
{
    Lock lock(mSyncObject);
    mValueList.push_front(std::move(newElement));
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
VALUE StackBase<VALUE>::pop_first()
{
    Lock lock(mSyncObject);

    VALUE result = mValueList.front();
    mValueList.pop_front();
    return result;
}

template <typename VALUE>
inline typename StackBase<VALUE>::STACKPOS StackBase<VALUE>::find(const VALUE& Value) const
{
    Lock lock(mSyncObject);
    auto it = std::find(mValueList.begin(), mValueList.end(), Value);
    return Constless<std::deque<VALUE>>::iter(mValueList, it);
}

template <typename VALUE>
inline typename StackBase<VALUE>::STACKPOS StackBase<VALUE>::find(const VALUE & Value, STACKPOS searchAfter ) const
{
    Lock lock(mSyncObject);
    STACKPOS end = invalid_position();
    return (searchAfter != end ? std::find(++searchAfter, end, Value) : end);
}

template <typename VALUE>
inline typename StackBase<VALUE>::STACKPOS StackBase<VALUE>::first_position() const
{
    Lock lock(mSyncObject);
    auto it = mValueList.begin();
    return Constless<std::deque<VALUE>>::iter(mValueList, it);
}

template <typename VALUE>
inline const VALUE & StackBase<VALUE>::at( const STACKPOS pos ) const
{
    Lock lock(mSyncObject);

    ASSERT(pos != mValueList.end());
    return (*pos);
}

template <typename VALUE>
inline VALUE & StackBase<VALUE>::at( STACKPOS pos )
{
    Lock lock(mSyncObject);

    ASSERT(pos != mValueList.end());
    return (*pos);
}

template <typename VALUE>
inline const VALUE & StackBase<VALUE>::value_at_position( const STACKPOS atPosition ) const
{
    Lock lock( mSyncObject );

    ASSERT( atPosition != mValueList.end( ) );
    return (*atPosition);
}

template <typename VALUE>
inline VALUE & StackBase<VALUE>::value_at_position( STACKPOS atPosition )
{
    Lock lock( mSyncObject );

    ASSERT( atPosition != mValueList.end( ) );
    return (*atPosition);
}

template <typename VALUE>
inline typename StackBase<VALUE>::STACKPOS StackBase<VALUE>::next_position( STACKPOS pos ) const
{
    Lock lock(mSyncObject);

    ASSERT(pos != mValueList.end());
    return (++pos);
}

template<typename VALUE>
inline uint32_t StackBase<VALUE>::elements(VALUE* list, uint32_t elemCount)
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

template<typename VALUE>
template<class Compare>
inline StackBase<VALUE>& StackBase<VALUE>::sort(Compare comp)
{
    std::sort(mValueList.begin(), mValueList.end(), comp);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ConcurrentStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
ConcurrentStack<VALUE>::ConcurrentStack()
    : StackBase<VALUE>(mLock)
    , mLock ( )
{
}

template <typename VALUE>
ConcurrentStack<VALUE>::ConcurrentStack( const StackBase<VALUE> & source )
    : StackBase<VALUE>( mLock, source )
    , mLock ( )
{
}

template <typename VALUE>
ConcurrentStack<VALUE>::ConcurrentStack( StackBase<VALUE> && source ) noexcept
    : StackBase<VALUE>( mLock, static_cast<StackBase<VALUE> &&>(source) )
    , mLock ( )
{
}

template <typename VALUE>
ConcurrentStack<VALUE>::ConcurrentStack( const ConcurrentStack<VALUE> & source )
    : StackBase<VALUE>( mLock, source )
    , mLock ( )
{
}

template <typename VALUE>
ConcurrentStack<VALUE>::ConcurrentStack( ConcurrentStack<VALUE> && source ) noexcept
    : StackBase<VALUE>( mLock, static_cast<StackBase<VALUE> &&>(source) )
    , mLock ( )
{
}

template<typename VALUE>
ConcurrentStack<VALUE>::ConcurrentStack(const VALUE* list, uint32_t count)
    : StackBase<VALUE>(mLock, list, count)
    , mLock()
{
}

template <typename VALUE>
inline ConcurrentStack<VALUE> & ConcurrentStack<VALUE>::operator = ( const ConcurrentStack<VALUE> & source )
{
    static_cast<StackBase<VALUE> &>(*this) = source;
    return (*this);
}

template <typename VALUE>
inline ConcurrentStack<VALUE> & ConcurrentStack<VALUE>::operator = ( ConcurrentStack<VALUE> && source ) noexcept
{
    static_cast<StackBase<VALUE> &>(*this) = static_cast<StackBase<VALUE> &&>(source);
    return (*this);
}

template <typename VALUE>
inline ConcurrentStack<VALUE>& ConcurrentStack<VALUE>::operator = (const StackBase<VALUE> & source)
{
    static_cast<StackBase<VALUE> &>(*this) = source;
    return (*this);
}

template <typename VALUE>
inline ConcurrentStack<VALUE>& ConcurrentStack<VALUE>::operator = (StackBase<VALUE> && source) noexcept
{
    static_cast<StackBase<VALUE> &>(*this) = static_cast<StackBase<VALUE> &&>(source);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Stack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
Stack<VALUE>::Stack()
    : StackBase<VALUE>(mNoLock)
    , mNoLock   ( )
{
}

template <typename VALUE>
Stack<VALUE>::Stack( const StackBase<VALUE> & source )
    : StackBase<VALUE>( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE>
Stack<VALUE>::Stack( StackBase<VALUE> && source ) noexcept
    : StackBase<VALUE>( mNoLock, static_cast<StackBase<VALUE> &&>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE>
Stack<VALUE>::Stack( const Stack<VALUE> & source )
    : StackBase<VALUE>( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE>
Stack<VALUE>::Stack( Stack<VALUE> && source ) noexcept
    : StackBase<VALUE>( mNoLock, static_cast<StackBase<VALUE> &&>(source) )
    , mNoLock   ( )
{
}

template<typename VALUE>
Stack<VALUE>::Stack(const VALUE* list, uint32_t count)
    : StackBase<VALUE>(mNoLock, list, count)
    , mNoLock()
{
}

template <typename VALUE>
inline Stack<VALUE> & Stack<VALUE>::operator = ( const Stack<VALUE> & source )
{
    static_cast<StackBase<VALUE> &>(*this) = source;
    return (*this);
}

template <typename VALUE>
inline Stack<VALUE> & Stack<VALUE>::operator = ( Stack<VALUE> && source ) noexcept
{
    static_cast<StackBase<VALUE> &>(*this) = static_cast<StackBase<VALUE> &&>(source);
    return (*this);
}

template <typename VALUE>
inline Stack<VALUE> & Stack<VALUE>::operator = ( const StackBase<VALUE> & source )
{
    static_cast<StackBase<VALUE> &>(*this) = source;
    return (*this);
}

template <typename VALUE>
inline Stack<VALUE> & Stack<VALUE>::operator = ( StackBase<VALUE> && source ) noexcept
{
    static_cast<StackBase<VALUE> &>(*this) = static_cast<StackBase<VALUE> &&>(source);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// StackBase<VALUE> friend operators implementation
//////////////////////////////////////////////////////////////////////////
template<typename V>
const areg::InStream & operator >> ( const areg::InStream & stream, areg::StackBase<V> & input )
{
    areg::Lock lock(input.mSyncObject);
    return (stream >> input.mValueList);
}

template<typename V>
areg::OutStream & operator << (areg::OutStream & stream, const areg::StackBase<V> & output )
{
    areg::Lock lock(output.mSyncObject);
    return (stream << output.mValueList);
}

} // namespace areg

#endif  // AREG_BASE_STACK_HPP
