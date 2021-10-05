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
 * \file        areg/base/TEStack.hpp
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
#include "areg/base/SynchObjects.hpp"
#include "areg/base/TETemplateBase.hpp"

#include "areg/base/IEIOStream.hpp"

#include <utility>

/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
template <typename VALUE, typename VALUE_TYPE, class Compare> class TEStack;
    template <typename VALUE, typename VALUE_TYPE, class Compare> class TELockStack;
    template <typename VALUE, typename VALUE_TYPE, class Compare> class TENolockStack;

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Compare> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Simple FIFO Stack base object to queue elements, insert and 
 *          access by push and pop operations. TEStack requires instance 
 *          of resource lock object to synchronize access of stack elements.
 *          Whether the Stack is thread safe or not, depends on type of
 *          synchronization object passed in stack.
 * 
 * \tparam  VALUE       The type of stored items. Either should be primitive 
 *                      or should have default constructor and valid assigning 
 *                      operator. And should be possible to convert to type VALUE_TYPE.
 * \tparam  VALUE_TYPE  By default same as VALUE.
 * \tparam  Compare     A helper class to compare elements.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Compare = TEListImpl<VALUE_TYPE>>
class TEStack
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   A stack block object. 
     *          Contains value and link to the next block.
     **/
    //////////////////////////////////////////////////////////////////////////
    // TEStack<VALUE, VALUE_TYPE, Compare>::Block class declaration
    //////////////////////////////////////////////////////////////////////////
    class Block
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Block default constructor.
         **/
        Block( void );
        /**
         * \brief   Block initialization constructor. Sets value
         **/
        Block( VALUE_TYPE newValue );
        /**
         * \brief   Destructor.
         **/
        ~Block( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Public member variables.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The pointer to block block in the stack
         **/
        Block *   mNext;
        /**
         * \brief   The value saved in the block
         **/
        VALUE       mValue;
    };


//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Stack initialization constructor.
     *          Receives reference to synchronization object.
     *          It is declared as protected to prevent direct call.
     *          Use TENolockStack or TELockStack objects instead.
     * \param   synchObject Reference to synchronization object.
     **/
    explicit TEStack( IEResourceLock & synchObject );

    /**
     * \brief   Initializes the resource lock object and copies elements from given source.
     * \param   synchObject     The instance of synchronization object
     * \param   source          The Stack source, which contains elements to copy.
     **/
    TEStack( IEResourceLock & synchObject, const TEStack<VALUE, VALUE_TYPE, Compare> & source );

    /**
     * \brief   Initializes the resource lock object and move elements from given source.
     * \param   synchObject     The instance of synchronization object
     * \param   source          The Stack source, which contains elements to move.
     **/
    TEStack( IEResourceLock & synchObject, TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept;

    /**
     * \brief   Destructor. Public
     **/
    ~TEStack( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. It is copying stack elements from given source
     *          in the same order as they are created in the source stack.
     *          If stack had entries, all entries will be lost and new entries will be created.
     * \param   source  The instance of source to get stack entries.
     * \return  Returns stack object.
     **/
    inline TEStack<VALUE, VALUE_TYPE, Compare> & operator = ( const TEStack<VALUE, VALUE_TYPE, Compare> & source );

    /**
     * \brief   Assigning operator. It is moves stack elements from given source
     *          in the same order as they are created in the source stack.
     *          If stack had entries, all entries will be lost and new entries will be created.
     * \param   source  The instance of source to get stack entries.
     * \return  Returns stack object.
     **/
    inline TEStack<VALUE, VALUE_TYPE, Compare> & operator = ( TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept;

public:
/************************************************************************/
// Friend global operators to make Stack streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream Stack values.
     *          If Stack previously had values, they will be lost.
     *          The values in the Stack will be initialized in the same sequence
     *          as they were written.
     *          There should be possibility to initialize values from streaming object and
     *          if VALUE is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object for reading values
     * \param   input   The Stack object to save initialized values.
     **/
    template<typename V, typename VT, class Impl>
    friend const IEInStream & operator >> (const IEInStream & stream, TEStack<V, VT, Impl> & input);
    /**
     * \brief   Writes to the stream Stack values.
     *          The values will be written to the stream starting from head position.
     *          There should be possibility to stream every value of Stack and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Stack object to read out values.
     **/
    template<typename V, typename VT, class Impl>
    friend IEOutStream & operator << (IEOutStream & stream, const TEStack<V, VT, Impl> & output);

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
     * \brief   Returns first inserted element in the stack without changing stack.
     *          The caller of function should make sure that the stack is not empty,
     *          otherwise it may cause system crash.
     *          Check the size of stack before calling function.
     **/
    inline VALUE_TYPE firstEntry( void ) const;

    /**
     * \brief   Returns first inserted element in the stack without changing stack.
     *          The caller of function should make sure that the stack is not empty,
     *          otherwise it may cause system crash.
     *          Check the size of stack before calling function.
     **/
    inline VALUE_TYPE lastEntry( void ) const;

    /**
     * \brief	Push new element at the end of stack.
     * \param	newElement  New Element to push into stack
     * \return  Returns size of stack.
     **/
    int pushLast( VALUE_TYPE newElement );

    /**
     * \brief	Push new element in the beginning of stack, that on pop
     *          operation will be be popped first.
     * \param	newElement  New Element to push into stack
     * \return  Returns size of stack.
     **/
    int pushFirst( VALUE_TYPE newElement );

    /**
     * \brief	Pops element from Stack and returns value of element.
     *          The stack should not be empty before calling this method.
     *          Otherwise assertion is raised.
     * \return	Returns Element from stack.
     **/
    VALUE popFirst( void );

    /**
     * \brief   Removes all elements from stack and makes it empty.
     **/
    inline void removeAll( void );

    /**
     * \brief   Copies all elements from given source and returns the number of copied elements.
     *          If Stack previousely had elements, they will be lost.
     * \param   source  The stack source to copy elements.
     * \return  Returns the number of copied elements.
     **/
    int copy( const TEStack<VALUE, VALUE_TYPE, Compare> & source );

    /**
     * \brief   Moves all elements from given source and returns the number of moved elements.
     *          If Stack previousely had elements, they will be lost.
     * \param   source  The stack source to copy elements.
     * \return  Returns the number of copied elements.
     **/
    int move( TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept;

    /**
     * \brief   Searches element in the stack starting at given position.
     *          The given position should be valid or equal to NECommon::START_POSITION
     *          to search at the beginning of stack.
     * \param   elem        The value to search in the stack.
     * \param   startAt     The starting position to search. It will start to search 
     *                      from beginning if equal to NECommon::START_POSITION.
     * \return  If found element, returns valid position. Otherwise, it returns NECommon::INVALID_POSITION.
     **/
    STACKPOS find(VALUE_TYPE elem, STACKPOS startAt = NECommon::START_POSITION) const;

    /**
     * \brief   Returns the first (head) position of the stack. Returns nullptr if empty.
     **/
    STACKPOS firstPosition( void ) const;

    /**
     * \brief   Return value at given position. The position should be valid.
     * \param   pos     The position to get the value.
     * \return  Returns value at given position.
     **/
    VALUE_TYPE getAt( const STACKPOS pos ) const;

    /**
     * \brief   Return value at given position. The position should be valid.
     * \param   pos     The position to get the value.
     * \return  Returns value at given position.
     **/
    VALUE & getAt( STACKPOS pos );

    /**
     * \brief   Returns next valid position after given position value. 
     *          Returns nullptr if given position was last.
     * \param   pos     The position value to get next.
     * \return  If given position is not last in the stack, it returns next valid position value.
     *          Otherwise, it returns nullptr.
     **/
    STACKPOS nextPosition( const STACKPOS pos ) const;

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
    IEResourceLock & mSynchObject;

    /**
     * \brief   Head of stack. Element will be popped first
     **/
    Block *         mHead;
    /**
     * \brief   Tail of stack. Last element in stack.
     **/
    Block *         mTail;

    /**
     * \brief   The number of elements in the stack.
     **/
    int             mCount;

    /**
     * \brief   Instance of heper object to compare values.
     **/
    Compare         mHelper;

//////////////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Cleans stack without locking object.
     *          Need to have it to call from destructor.
     **/
    inline void _cleanStack( void );

    /**
     * \brief   Copies elements from given source of stack.
     * \param   source  The source of elements to copy.
     **/
    inline void _copyElements( const TEStack<VALUE, VALUE_TYPE, Compare> & source );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    TEStack( void ) = delete;
    TEStack( const TEStack<VALUE, VALUE_TYPE, Compare> & /* source */ ) = delete;
    TEStack( TEStack<VALUE, VALUE_TYPE, Compare> && /* source */ ) = delete;
};

//////////////////////////////////////////////////////////////////////////
// TELockStack<VALUE, VALUE_TYPE, Compare> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Thread safe FIFO stack class template declaration, where accessing
 *          data is synchronized by resource lock. Use this object if elements 
 *          of stack are accessed by more than one thread.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Compare = TEListImpl<VALUE_TYPE>>
class TELockStack  : public TEStack<VALUE, VALUE_TYPE, Compare>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    TELockStack( void );

    /**
     * \brief   Copies data from given source.
     * \param   source  The source to copy data.
     **/
    explicit TELockStack( const TEStack<VALUE, VALUE_TYPE, Compare> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    explicit TELockStack( TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept;

    /**
     * \brief   Copies data from given source.
     * \param   source  The source to copy data.
     **/
    TELockStack( const TELockStack<VALUE, VALUE_TYPE, Compare> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    TELockStack( TELockStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TELockStack( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies entries from given sources. If stack had entries
     *          all entries will be lost and new entries will be created.
     * \param   source  The instance of source to copy stack entries.
     * \return  Returns stack object.
     **/
    inline TELockStack<VALUE, VALUE_TYPE, Compare> & operator = ( const TELockStack<VALUE, VALUE_TYPE, Compare> & source );

    /**
     * \brief   Moves entries from given sources. If stack had entries
     *          all entries will be lost and new entries will be created.
     * \param   source  The instance of source to move stack entries.
     * \return  Returns stack object.
     **/
    inline TELockStack<VALUE, VALUE_TYPE, Compare> & operator = ( TELockStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept;

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
// TENolockStack<VALUE, VALUE_TYPE, Compare> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   No thread safe FIFO stack class template declaration, where accessing
 *          data is not synchronized. Use this object if elements of stack 
 *          are accessed only by one thread.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Compare = TEListImpl<VALUE_TYPE>>
class TENolockStack    : public TEStack<VALUE, VALUE_TYPE, Compare>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    TENolockStack( void );

    /**
     * \brief   Copies data from given source.
     * \param   source  The source to copy data.
     **/
    explicit TENolockStack( const TEStack<VALUE, VALUE_TYPE, Compare> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    explicit TENolockStack( TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept;

    /**
     * \brief   Copies data from given source.
     * \param   source  The source to copy data.
     **/
    TENolockStack( const TENolockStack<VALUE, VALUE_TYPE, Compare> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    TENolockStack( TENolockStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TENolockStack( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies entries from given sources. If stack had entries
     *          all entries will be lost and new entries will be created.
     * \param   source  The instance of source to copy stack entries.
     * \return  Returns stack object.
     **/
    inline TENolockStack<VALUE, VALUE_TYPE, Compare> & operator = ( const TENolockStack<VALUE, VALUE_TYPE, Compare> & source );

    /**
     * \brief   Moves entries from given sources. If stack had entries
     *          all entries will be lost and new entries will be created.
     * \param   source  The instance of source to move stack entries.
     * \return  Returns stack object.
     **/
    inline TENolockStack<VALUE, VALUE_TYPE, Compare> & operator = ( TENolockStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Synchronization object simulation.
     **/
    mutable NolockSynchObject mNoLock;
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Compare>::Block class template implementation
// TEStack<VALUE, VALUE_TYPE, Compare> internal class
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Compare>::Block::Block( void ) 
    : mNext (nullptr)
    , mValue(       )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Compare>::Block::Block( VALUE_TYPE newValue ) 
    : mNext (nullptr)
    , mValue(newValue)
{
}

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Compare> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TEStack<VALUE, VALUE_TYPE, Compare>::TEStack( IEResourceLock & synchObject )
    : mSynchObject  ( synchObject )
    , mHead         ( nullptr )
    , mTail         ( nullptr )
    , mCount        ( 0 )
    , mHelper       ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TEStack<VALUE, VALUE_TYPE, Compare>::TEStack( IEResourceLock & synchObject, const TEStack<VALUE, VALUE_TYPE, Compare> & source )
    : mSynchObject  ( synchObject )
    , mHead         ( nullptr )
    , mTail         ( nullptr )
    , mCount        ( 0 )
    , mHelper       ( )
{
    _copyElements(source);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TEStack<VALUE, VALUE_TYPE, Compare>::TEStack( IEResourceLock & synchObject, TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept
    : mSynchObject  ( synchObject )
    , mHead         ( source.mHead )
    , mTail         ( source.mTail )
    , mCount        ( source.mCount )
    , mHelper       ( )
{
    source.mHead    = nullptr;
    source.mTail    = nullptr;
    source.mCount   = 0;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TEStack<VALUE, VALUE_TYPE, Compare>::~TEStack( void )
{
    _cleanStack();
    mSynchObject.unlock();
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Compare> & TEStack<VALUE, VALUE_TYPE, Compare>::operator = ( const TEStack<VALUE, VALUE_TYPE, Compare> & source )
{
    copy(source);
    return (*this);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Compare> & TEStack<VALUE, VALUE_TYPE, Compare>::operator = ( TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept
{
    move( static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &&>(source) );
    return (*this);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline int TEStack<VALUE, VALUE_TYPE, Compare>::getSize( void ) const
{
    Lock lock( mSynchObject );
    return mCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Compare>::isEmpty( void ) const
{
    Lock lock( mSynchObject );
    return (mCount == 0);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Compare>::lock( void ) const
{
    return mSynchObject.lock(NECommon::WAIT_INFINITE);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Compare>::unlock( void ) const
{
    return mSynchObject.unlock();
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TEStack<VALUE, VALUE_TYPE, Compare>::firstEntry( void ) const
{
    Lock lock(mSynchObject);
    ASSERT(mHead != nullptr);
    return mHead->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TEStack<VALUE, VALUE_TYPE, Compare>::lastEntry( void ) const
{
    Lock lock(mSynchObject);
    ASSERT(mTail == nullptr);
    return mTail->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
int TEStack<VALUE, VALUE_TYPE, Compare>::copy( const TEStack<VALUE, VALUE_TYPE, Compare> & source )
{
    Lock lock( mSynchObject );

    if (static_cast<const TEStack<VALUE, VALUE_TYPE, Compare> *>(this) != &source)
    {
        Lock lockSource(source.mSynchObject);

        _cleanStack();
        _copyElements(source);
    }

    return mCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
int TEStack<VALUE, VALUE_TYPE, Compare>::move( TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept
{
    ASSERT(this != &source);

    Lock lock( mSynchObject );
    Lock lockSource( source.mSynchObject );

    std::swap( mHead, source.mHead );
    std::swap( mTail, source.mTail );
    std::swap( mCount, source.mCount );

    return mCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
int TEStack<VALUE, VALUE_TYPE, Compare>::pushLast( VALUE_TYPE newElement )
{
    Lock lock(mSynchObject);

    Block * newBlock = DEBUG_NEW Block(newElement);
    if (newBlock != nullptr)
    {
        if ( mHead == nullptr )
        {
            ASSERT(mTail  == nullptr);
            ASSERT(mCount == 0);
            mHead = mTail = newBlock;
        }
        else
        {
            ASSERT(mTail  != nullptr);
            ASSERT(mCount != 0);
            mTail->mNext = newBlock;
            mTail = newBlock;
        }

        ++ mCount;
    }

    return mCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
int TEStack<VALUE, VALUE_TYPE, Compare>::pushFirst( VALUE_TYPE newElement )
{
    Lock lock(mSynchObject);

    Block* newBlock = DEBUG_NEW Block(newElement);
    if (newBlock != nullptr)
    {
        if ( mHead == nullptr )
        {
            ASSERT(mTail  == nullptr);
            ASSERT(mCount == 0);
            mHead = mTail = newBlock;
        }
        else
        {
            ASSERT(mTail  != nullptr);
            ASSERT(mCount != 0);
            newBlock->mNext = mHead;
            mHead = newBlock;
        }

        ++ mCount;
    }

    return mCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
VALUE TEStack<VALUE, VALUE_TYPE, Compare>::popFirst( void )
{
    Lock lock(mSynchObject);

    VALUE result = VALUE();

    if (mCount != 0 )
    {
        Block* block = mHead;
        mHead = block->mNext;
        -- mCount;

        if ( mHead == nullptr )
        {
            ASSERT( mCount == 0 );
            mTail = nullptr;
        }

        result = block->mValue;
        delete block;
    }

    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline void TEStack<VALUE, VALUE_TYPE, Compare>::removeAll( void )
{
    Lock lock(mSynchObject);
    _cleanStack();
}


template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
STACKPOS TEStack<VALUE, VALUE_TYPE, Compare>::find(VALUE_TYPE elem, STACKPOS startAt /*= NECommon::START_POSITION*/) const
{
    Lock lock(mSynchObject);

    STACKPOS result = static_cast<STACKPOS>(NECommon::INVALID_POSITION);
    Block * head = startAt == NECommon::START_POSITION ? mHead : reinterpret_cast<Block *>(startAt)->mNext;
    while ( head != nullptr)
    {
        if (isEqualValues(elem, head->mValue))
        {
            result = static_cast<STACKPOS>(head);
            break;
        }

        head = head->mNext;
    }

    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline STACKPOS TEStack<VALUE, VALUE_TYPE, Compare>::firstPosition( void ) const
{
    Lock lock(mSynchObject);
    return static_cast<STACKPOS>(mHead);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TEStack<VALUE, VALUE_TYPE, Compare>::getAt( const STACKPOS pos ) const
{
    Lock lock(mSynchObject);
    ASSERT(pos != nullptr);
    return reinterpret_cast<const Block *>(pos)->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE & TEStack<VALUE, VALUE_TYPE, Compare>::getAt( STACKPOS pos )
{
    Lock lock(mSynchObject);
    ASSERT(pos != nullptr);
    return reinterpret_cast<Block *>(pos)->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline STACKPOS TEStack<VALUE, VALUE_TYPE, Compare>::nextPosition( const STACKPOS pos ) const
{
    Lock lock(mSynchObject);
    return static_cast<STACKPOS>(pos != nullptr ? reinterpret_cast<const Block *>(pos)->mNext : nullptr);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Compare>::isEqualValues(VALUE_TYPE Value1, VALUE_TYPE Value2) const
{
    return mHelper.implEqualValues(Value1, Value2);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline void TEStack<VALUE, VALUE_TYPE, Compare>::_cleanStack( void )
{
    while (mHead != nullptr)
    {
        Block* block = mHead;
        mHead = block->mNext;
        delete block;
    }

    mHead = mTail = nullptr;
    mCount = 0;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline void TEStack<VALUE, VALUE_TYPE, Compare>::_copyElements( const TEStack<VALUE, VALUE_TYPE, Compare> & source )
{
    for ( Block * srcBlock = source.mHead; srcBlock != nullptr; srcBlock = srcBlock->mNext )
    {
        Block * newBlock = DEBUG_NEW Block( srcBlock->mValue );
        if ( newBlock != nullptr )
        {
            if ( mHead == nullptr )
            {
                ASSERT( mTail   == nullptr );
                ASSERT( mCount  == 0 );
                mHead = mTail = newBlock;
            }
            else
            {
                ASSERT( mTail   != nullptr );
                ASSERT( mCount  != 0 );
                mTail->mNext = newBlock;
                mTail = newBlock;
            }

            ++ mCount;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// TELockStack<VALUE, VALUE_TYPE, Compare> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TELockStack<VALUE, VALUE_TYPE, Compare>::TELockStack( void )
    : TEStack<VALUE, VALUE_TYPE, Compare>(mLock)
    , mLock ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TELockStack<VALUE, VALUE_TYPE, Compare>::TELockStack( const TEStack<VALUE, VALUE_TYPE, Compare> & source )
    : TEStack<VALUE, VALUE_TYPE, Compare>( mLock, source )
    , mLock ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TELockStack<VALUE, VALUE_TYPE, Compare>::TELockStack( TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept
    : TEStack<VALUE, VALUE_TYPE, Compare>( mLock, static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &&>(source) )
    , mLock ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TELockStack<VALUE, VALUE_TYPE, Compare>::TELockStack( const TELockStack<VALUE, VALUE_TYPE, Compare> & source )
    : TEStack<VALUE, VALUE_TYPE, Compare>( mLock, source )
    , mLock ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TELockStack<VALUE, VALUE_TYPE, Compare>::TELockStack( TELockStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept
    : TEStack<VALUE, VALUE_TYPE, Compare>( mLock, static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &&>(source) )
    , mLock ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline TELockStack<VALUE, VALUE_TYPE, Compare> & TELockStack<VALUE, VALUE_TYPE, Compare>::operator = ( const TELockStack<VALUE, VALUE_TYPE, Compare> & source )
{
    static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &>(*this) = source;
    return (*this);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline TELockStack<VALUE, VALUE_TYPE, Compare> & TELockStack<VALUE, VALUE_TYPE, Compare>::operator = ( TELockStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept
{
    static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &>(*this) = static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &&>(source);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// TENolockStack<VALUE, VALUE_TYPE, Compare> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TENolockStack<VALUE, VALUE_TYPE, Compare>::TENolockStack( void )
    : TEStack<VALUE, VALUE_TYPE, Compare>(mNoLock)
    , mNoLock   ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TENolockStack<VALUE, VALUE_TYPE, Compare>::TENolockStack( const TEStack<VALUE, VALUE_TYPE, Compare> & source )
    : TEStack<VALUE, VALUE_TYPE, Compare>( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TENolockStack<VALUE, VALUE_TYPE, Compare>::TENolockStack( TEStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept
    : TEStack<VALUE, VALUE_TYPE, Compare>( mNoLock, static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &&>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TENolockStack<VALUE, VALUE_TYPE, Compare>::TENolockStack( const TENolockStack<VALUE, VALUE_TYPE, Compare> & source )
    : TEStack<VALUE, VALUE_TYPE, Compare>( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
TENolockStack<VALUE, VALUE_TYPE, Compare>::TENolockStack( TENolockStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept
    : TEStack<VALUE, VALUE_TYPE, Compare>( mNoLock, static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &&>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline TENolockStack<VALUE, VALUE_TYPE, Compare> & TENolockStack<VALUE, VALUE_TYPE, Compare>::operator = ( const TENolockStack<VALUE, VALUE_TYPE, Compare> & source )
{
    static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &>(*this) = source;
    return (*this);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Compare /*= TEListImpl<VALUE_TYPE>*/>
inline TENolockStack<VALUE, VALUE_TYPE, Compare> & TENolockStack<VALUE, VALUE_TYPE, Compare>::operator = ( TENolockStack<VALUE, VALUE_TYPE, Compare> && source ) noexcept
{
    static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &>(*this) = static_cast<TEStack<VALUE, VALUE_TYPE, Compare> &&>(source);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Compare> friend operators implementation
//////////////////////////////////////////////////////////////////////////
template<typename V, typename VT, class Impl>
const IEInStream & operator >> ( const IEInStream & stream, TEStack<V, VT, Impl> & input )
{
    Lock lock(input.mSynchObject);

    input._cleanStack();
    int size = 0;
    stream >> size;
    for (int i = 0; i < size; ++ i)
    {
        V newElement;
        stream >> newElement;
        static_cast<void>(input.pushLast(newElement));
    }

    return stream;
}

template<typename V, typename VT, class Impl>
IEOutStream & operator << ( IEOutStream & stream, const TEStack<V, VT, Impl> & output )
{
    Lock lock(output.mSynchObject);

    int size = output.getSize();
    stream << size;
    class TEStack<V, VT, Impl>::Block* block = output.mHead;
    for ( ; block != nullptr; block = block->mNext)
        stream << block->mValue;

    return stream;
}
