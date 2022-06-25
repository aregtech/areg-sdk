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
#include <deque>

/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
template <typename VALUE, class Compare> class TEStack;
    template <typename VALUE, class Compare> class TELockStack;
    template <typename VALUE, class Compare> class TENolockStack;

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, Compare> class template declaration
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
 *                      operator. And should be possible to convert to type const VALUE &.
 * \tparam  const VALUE &  By default same as VALUE.
 * \tparam  Compare     A helper class to compare elements.
 **/
template <typename VALUE, class Compare = TEListImpl<VALUE>>
class TEStack   : private Constless<std::deque<VALUE>>
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
protected:

public:
    using STACKPOS = typename std::deque<VALUE>::iterator;

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
    TEStack( IEResourceLock & synchObject, const TEStack<VALUE, Compare> & source );

    /**
     * \brief   Initializes the resource lock object and move elements from given source.
     * \param   synchObject     The instance of synchronization object
     * \param   source          The Stack source, which contains elements to move.
     **/
    TEStack( IEResourceLock & synchObject, TEStack<VALUE, Compare> && source ) noexcept;

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
    inline TEStack<VALUE, Compare> & operator = ( const TEStack<VALUE, Compare> & source );

    /**
     * \brief   Assigning operator. It is moves stack elements from given source
     *          in the same order as they are created in the source stack.
     *          If stack had entries, all entries will be lost and new entries will be created.
     * \param   source  The instance of source to get stack entries.
     * \return  Returns stack object.
     **/
    inline TEStack<VALUE, Compare> & operator = ( TEStack<VALUE, Compare> && source ) noexcept;

    /**
     * \brief   Returns element at specified position.
     */
    inline const VALUE& operator [] (STACKPOS atPosition) const;
    inline VALUE& operator [] (STACKPOS atPosition);

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
    template<typename VALUE, class Compare>
    friend const IEInStream & operator >> (const IEInStream & stream, TEStack<VALUE, Compare> & input);
    /**
     * \brief   Writes to the stream Stack values.
     *          The values will be written to the stream starting from head position.
     *          There should be possibility to stream every value of Stack and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Stack object to read out values.
     **/
    template<typename VALUE, class Compare>
    friend IEOutStream & operator << (IEOutStream & stream, const TEStack<VALUE, Compare> & output);

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
     * \brief   Returns true if specified position pointing start of the stack.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the stack.
     **/
    inline bool isStartPosition(const STACKPOS pos) const;

    /**
     * \brief   Returns true if specified position pointing start of the stack.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the stack.
     **/
    inline bool isLastPosition(const STACKPOS pos) const;

    /**
     * \brief   Returns the invalid position of the stack.
     **/
    STACKPOS invalidPosition( void ) const;

    /**
     * \brief   Returns true if the given position is valid, i.e. is not pointing the end of the stack.
     *          Note, it does not check whether there is a such position in the stack,
     *          The method ensures that the position is not pointing to the invalid value.
     **/
    bool isValidPosition(const STACKPOS pos) const;

    /**
     * \brief   Returns true if specified position is invalid, i.e. points the end of the stack.
     **/
    bool isInvalidPosition(const STACKPOS pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the stack.
     *          The duration of checkup depends on the location of the position in the stack.
     * \param pos       The position to check.
     * \return  Returns true if specified position points to the valid entry in the stack.
     */
    inline bool checkPosition(const STACKPOS pos) const;

/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief   Removes all elements from stack and makes it empty.
     **/
    inline void clear(void);

    /**
     * \brief   Delete extra entries in array.
     **/
    inline void freeExtra( void );

    /**
     * \brief   Sets the size of array to zero and deletes all unused capacity of the string.
     */
    inline void release(void);

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
    inline const VALUE & firstEntry( void ) const;

    /**
     * \brief   Returns first inserted element in the stack without changing stack.
     *          The caller of function should make sure that the stack is not empty,
     *          otherwise it may cause system crash.
     *          Check the size of stack before calling function.
     **/
    inline const VALUE & lastEntry( void ) const;

    /**
     * \brief	Push new element at the end of stack.
     * \param	newElement  New Element to push into stack
     * \return  Returns size of stack.
     **/
    inline uint32_t pushLast( const VALUE & newElement );
    inline uint32_t pushLast(VALUE && newElement);

    /**
     * \brief	Push new element in the beginning of stack, that on pop
     *          operation will be popped first.
     * \param	newElement  New Element to push into stack
     * \return  Returns size of stack.
     **/
    inline uint32_t pushFirst( const VALUE & newElement );
    inline uint32_t pushFirst( VALUE && newElement);

    /**
     * \brief	Pops element from Stack and returns value of element.
     *          The stack should not be empty before calling this method.
     *          Otherwise assertion is raised.
     * \return	Returns Element from stack.
     **/
    inline VALUE popFirst( void );

    /**
     * \brief   Copies all elements from given source and returns the number of copied elements.
     *          If Stack previously had elements, they will be lost.
     * \param   source  The stack source to copy elements.
     * \return  Returns the number of copied elements.
     **/
    inline uint32_t copy( const TEStack<VALUE, Compare> & source );

    /**
     * \brief   Moves all elements from given source and returns the number of moved elements.
     *          If Stack previously had elements, they will be lost.
     * \param   source  The stack source to copy elements.
     * \return  Returns the number of copied elements.
     **/
    inline uint32_t move( TEStack<VALUE, Compare> && source ) noexcept;

    /**
     * \brief   Searches element in the stack starting at given position.
     *          The given position should be valid or equal to NECommon::START_POSITION
     *          to search at the beginning of stack.
     * \param   Value       The element value to search in the stack.
     * \param	searchAfter	If valid position, the searching starts from next element specified by position.
     *                      If invalid position, the searching starts from the beginning of the linked list.
     * \return  If found element, returns valid position. Otherwise, it returns NECommon::INVALID_POSITION.
     **/
    inline STACKPOS find(const VALUE & Value) const;
    inline STACKPOS find(const VALUE& Value, STACKPOS searchAfter) const;

    /**
     * \brief   Returns the first (head) position of the stack. Returns nullptr if empty.
     **/
    inline STACKPOS firstPosition( void ) const;

    /**
     * \brief   Return value at given position. The position should be valid.
     * \param   pos     The position to get the value.
     * \return  Returns value at given position.
     **/
    inline const VALUE & getAt( const STACKPOS pos ) const;
    inline VALUE& getAt(STACKPOS pos);

    /**
     * \brief   Returns next valid position after given position value. 
     *          Returns nullptr if given position was last.
     * \param   pos     The position value to get next.
     * \return  If given position is not last in the stack, it returns next valid position value.
     *          Otherwise, it returns nullptr.
     **/
    inline STACKPOS nextPosition( STACKPOS pos ) const;

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
    inline bool isEqualValues(const VALUE & Value1, const VALUE & Value2) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    //! The stack object, which contains the elements.
    std::deque<VALUE>   mValueList;

     //! The instance of synchronization object to be used to make object thread-safe.
    IEResourceLock &    mSynchObject;

    //! THe Instance of helper object to compare values.
    Compare             mHelper;

//////////////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////////////
private:
//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    TEStack( void ) = delete;
    TEStack( const TEStack<VALUE, Compare> & /* source */ ) = delete;
    TEStack( TEStack<VALUE, Compare> && /* source */ ) = delete;
};

//////////////////////////////////////////////////////////////////////////
// TELockStack<VALUE, Compare> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Thread safe FIFO stack class template declaration, where accessing
 *          data is synchronized by resource lock. Use this object if elements 
 *          of stack are accessed by more than one thread.
 **/
template <typename VALUE, class Compare = TEListImpl<VALUE>>
class TELockStack  : public TEStack<VALUE, Compare>
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
    explicit TELockStack( const TEStack<VALUE, Compare> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    explicit TELockStack( TEStack<VALUE, Compare> && source ) noexcept;

    /**
     * \brief   Copies data from given source.
     * \param   source  The source to copy data.
     **/
    TELockStack( const TELockStack<VALUE, Compare> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    TELockStack( TELockStack<VALUE, Compare> && source ) noexcept;

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
    inline TELockStack<VALUE, Compare> & operator = ( const TELockStack<VALUE, Compare> & source );

    /**
     * \brief   Moves entries from given sources. If stack had entries
     *          all entries will be lost and new entries will be created.
     * \param   source  The instance of source to move stack entries.
     * \return  Returns stack object.
     **/
    inline TELockStack<VALUE, Compare> & operator = ( TELockStack<VALUE, Compare> && source ) noexcept;

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
// TENolockStack<VALUE, Compare> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   No thread safe FIFO stack class template declaration, where accessing
 *          data is not synchronized. Use this object if elements of stack 
 *          are accessed only by one thread.
 **/
template <typename VALUE, class Compare = TEListImpl<VALUE>>
class TENolockStack    : public TEStack<VALUE, Compare>
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
    explicit TENolockStack( const TEStack<VALUE, Compare> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    explicit TENolockStack( TEStack<VALUE, Compare> && source ) noexcept;

    /**
     * \brief   Copies data from given source.
     * \param   source  The source to copy data.
     **/
    TENolockStack( const TENolockStack<VALUE, Compare> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    TENolockStack( TENolockStack<VALUE, Compare> && source ) noexcept;

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
    inline TENolockStack<VALUE, Compare> & operator = ( const TENolockStack<VALUE, Compare> & source );

    /**
     * \brief   Moves entries from given sources. If stack had entries
     *          all entries will be lost and new entries will be created.
     * \param   source  The instance of source to move stack entries.
     * \return  Returns stack object.
     **/
    inline TENolockStack<VALUE, Compare> & operator = ( TENolockStack<VALUE, Compare> && source ) noexcept;

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
// TEStack<VALUE, Compare> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TEStack<VALUE, Compare>::TEStack( IEResourceLock & synchObject )
    : mValueList    ( )
    , mSynchObject  ( synchObject )
    , mHelper       ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TEStack<VALUE, Compare>::TEStack( IEResourceLock & synchObject, const TEStack<VALUE, Compare> & source )
    : mValueList    ( source.mValueList )
    , mSynchObject  ( synchObject )
    , mHelper       ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TEStack<VALUE, Compare>::TEStack( IEResourceLock & synchObject, TEStack<VALUE, Compare> && source ) noexcept
    : mValueList    ( std::move(source.mValueList) )
    , mSynchObject  ( synchObject )
    , mHelper       ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TEStack<VALUE, Compare>::~TEStack( void )
{
    mSynchObject.lock();
    mValueList.clear();
    mSynchObject.unlock();
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline TEStack<VALUE, Compare> & TEStack<VALUE, Compare>::operator = ( const TEStack<VALUE, Compare> & source )
{
    copy(source);
    return (*this);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline TEStack<VALUE, Compare> & TEStack<VALUE, Compare>::operator = ( TEStack<VALUE, Compare> && source ) noexcept
{
    move( std::move(source) );
    return (*this);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline const VALUE& TEStack<VALUE, Compare>::operator [] (STACKPOS atPosition) const
{
    Lock lock(mSynchObject);
    return (*atPosition);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline VALUE& TEStack<VALUE, Compare>::operator [] (STACKPOS atPosition)
{
    Lock lock(mSynchObject);
    return (*atPosition);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline uint32_t TEStack<VALUE, Compare>::getSize( void ) const
{
    Lock lock( mSynchObject );
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline bool TEStack<VALUE, Compare>::isEmpty( void ) const
{
    Lock lock( mSynchObject );
    return mValueList.empty();
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline bool TEStack<VALUE, Compare>::isStartPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    return (pos = mValueList.begin());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline bool TEStack<VALUE, Compare>::isLastPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    return (mValueList.empty() == false) && (pos = --mValueList.end());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline typename TEStack<VALUE, Compare>::STACKPOS TEStack<VALUE, Compare>::invalidPosition(void) const
{
    Lock lock(mSynchObject);
    return Constless::iter(mValueList, mValueList.end());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline bool TEStack<VALUE, Compare>::isValidPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    return (pos != mValueList.end());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline bool TEStack<VALUE, Compare>::isInvalidPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    return (pos == mValueList.end());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline bool TEStack<VALUE, Compare>::checkPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    std::deque<VALUE>::const_iterator it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline void TEStack<VALUE, Compare>::clear(void)
{
    Lock lock(mSynchObject);
    mValueList.clear();
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline void TEStack<VALUE, Compare>::freeExtra(void)
{
    Lock lock(mSynchObject);
    mValueList.shrink_to_fit();
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline void TEStack<VALUE, Compare>::release(void)
{
    Lock lock(mSynchObject);
    mValueList.clear();
    mValueList.shrink_to_fit();
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline bool TEStack<VALUE, Compare>::lock( void ) const
{
    return mSynchObject.lock(NECommon::WAIT_INFINITE);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline bool TEStack<VALUE, Compare>::unlock( void ) const
{
    return mSynchObject.unlock();
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline const VALUE & TEStack<VALUE, Compare>::firstEntry( void ) const
{
    Lock lock(mSynchObject);
    return mValueList.front();
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline const VALUE & TEStack<VALUE, Compare>::lastEntry( void ) const
{
    Lock lock(mSynchObject);
    return mValueList.back();
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline uint32_t TEStack<VALUE, Compare>::copy( const TEStack<VALUE, Compare> & source )
{
    Lock lock(mSynchObject);
    Lock lockSource(source.mSynchObject);

    mValueList = source.mValueList;
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline uint32_t TEStack<VALUE, Compare>::move( TEStack<VALUE, Compare> && source ) noexcept
{
    Lock lock(mSynchObject);
    Lock lockSource(source.mSynchObject);

    mValueList = std::move(source.mValueList);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline uint32_t TEStack<VALUE, Compare>::pushLast( const VALUE & newElement )
{
    Lock lock(mSynchObject);
    mValueList.push_back(newElement);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline uint32_t TEStack<VALUE, Compare>::pushLast(VALUE && newElement)
{
    Lock lock(mSynchObject);
    mValueList.push_back(std::move(newElement));
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline uint32_t TEStack<VALUE, Compare>::pushFirst( const VALUE & newElement )
{
    Lock lock(mSynchObject);
    mValueList.push_front(newElement);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline uint32_t TEStack<VALUE, Compare>::pushFirst(VALUE && newElement)
{
    Lock lock(mSynchObject);
    mValueList.push_front(std::move(newElement));
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
VALUE TEStack<VALUE, Compare>::popFirst( void )
{
    Lock lock(mSynchObject);

    VALUE result = mValueList.front();
    mValueList.pop_front();
    return result;
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline typename TEStack<VALUE, Compare>::STACKPOS TEStack<VALUE, Compare>::find(const VALUE& Value) const
{
    return find(Value, invalidPosition());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline typename TEStack<VALUE, Compare>::STACKPOS TEStack<VALUE, Compare>::find(const VALUE & Value, STACKPOS searchAfter ) const
{
    Lock lock(mSynchObject);

    STACKPOS pos = searchAfter == invalidPosition() ? Constless::iter(mValueList.begin()) : ++searchAfter;
    for (; pos != mValueList.end(); ++pos)
    {
        if (*pos == Value)
            break;
    }

    return pos;
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline typename TEStack<VALUE, Compare>::STACKPOS TEStack<VALUE, Compare>::firstPosition( void ) const
{
    Lock lock(mSynchObject);
    return Constless::iter(mValueList, mValueList.begin());
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline const VALUE & TEStack<VALUE, Compare>::getAt( const STACKPOS pos ) const
{
    Lock lock(mSynchObject);

    ASSERT(pos != mValueList.end());
    return (*pos);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline VALUE & TEStack<VALUE, Compare>::getAt( STACKPOS pos )
{
    Lock lock(mSynchObject);

    ASSERT(pos != mValueList.end());
    return (*pos);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline typename TEStack<VALUE, Compare>::STACKPOS TEStack<VALUE, Compare>::nextPosition( STACKPOS pos ) const
{
    Lock lock(mSynchObject);

    ASSERT(pos != mValueList.end());
    return (++pos);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline bool TEStack<VALUE, Compare>::isEqualValues(const VALUE & Value1, const VALUE & Value2) const
{
    return mHelper.implEqualValues(Value1, Value2);
}

//////////////////////////////////////////////////////////////////////////
// TELockStack<VALUE, Compare> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TELockStack<VALUE, Compare>::TELockStack( void )
    : TEStack<VALUE, Compare>(mLock)
    , mLock ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TELockStack<VALUE, Compare>::TELockStack( const TEStack<VALUE, Compare> & source )
    : TEStack<VALUE, Compare>( mLock, source )
    , mLock ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TELockStack<VALUE, Compare>::TELockStack( TEStack<VALUE, Compare> && source ) noexcept
    : TEStack<VALUE, Compare>( mLock, static_cast<TEStack<VALUE, Compare> &&>(source) )
    , mLock ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TELockStack<VALUE, Compare>::TELockStack( const TELockStack<VALUE, Compare> & source )
    : TEStack<VALUE, Compare>( mLock, source )
    , mLock ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TELockStack<VALUE, Compare>::TELockStack( TELockStack<VALUE, Compare> && source ) noexcept
    : TEStack<VALUE, Compare>( mLock, static_cast<TEStack<VALUE, Compare> &&>(source) )
    , mLock ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline TELockStack<VALUE, Compare> & TELockStack<VALUE, Compare>::operator = ( const TELockStack<VALUE, Compare> & source )
{
    static_cast<TEStack<VALUE, Compare> &>(*this) = source;
    return (*this);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline TELockStack<VALUE, Compare> & TELockStack<VALUE, Compare>::operator = ( TELockStack<VALUE, Compare> && source ) noexcept
{
    static_cast<TEStack<VALUE, Compare> &>(*this) = static_cast<TEStack<VALUE, Compare> &&>(source);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// TENolockStack<VALUE, Compare> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TENolockStack<VALUE, Compare>::TENolockStack( void )
    : TEStack<VALUE, Compare>(mNoLock)
    , mNoLock   ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TENolockStack<VALUE, Compare>::TENolockStack( const TEStack<VALUE, Compare> & source )
    : TEStack<VALUE, Compare>( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TENolockStack<VALUE, Compare>::TENolockStack( TEStack<VALUE, Compare> && source ) noexcept
    : TEStack<VALUE, Compare>( mNoLock, static_cast<TEStack<VALUE, Compare> &&>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TENolockStack<VALUE, Compare>::TENolockStack( const TENolockStack<VALUE, Compare> & source )
    : TEStack<VALUE, Compare>( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
TENolockStack<VALUE, Compare>::TENolockStack( TENolockStack<VALUE, Compare> && source ) noexcept
    : TEStack<VALUE, Compare>( mNoLock, static_cast<TEStack<VALUE, Compare> &&>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline TENolockStack<VALUE, Compare> & TENolockStack<VALUE, Compare>::operator = ( const TENolockStack<VALUE, Compare> & source )
{
    static_cast<TEStack<VALUE, Compare> &>(*this) = source;
    return (*this);
}

template <typename VALUE, class Compare /*= TEListImpl<VALUE>*/>
inline TENolockStack<VALUE, Compare> & TENolockStack<VALUE, Compare>::operator = ( TENolockStack<VALUE, Compare> && source ) noexcept
{
    static_cast<TEStack<VALUE, Compare> &>(*this) = static_cast<TEStack<VALUE, Compare> &&>(source);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, Compare> friend operators implementation
//////////////////////////////////////////////////////////////////////////
template<typename VALUE, class Compare>
const IEInStream & operator >> ( const IEInStream & stream, TEStack<VALUE, Compare> & input )
{
    Lock lock(input.mSynchObject);

    input.mValueList.clear();
    uint32_t size = 0;
    stream >> size;

    for (uint32_t i = 0; i < size; ++ i)
    {
        VALUE value;
        stream >> value;
        input.mValueList.push_back(std::move(value));
    }

    return stream;
}

template<typename VALUE, class Compare>
IEOutStream & operator << ( IEOutStream & stream, const TEStack<VALUE, Compare> & output )
{
    Lock lock(output.mSynchObject);

    uint32_t size = output.getSize();
    stream << size;

    for (const VALUE value : output.mValueList)
    {
        stream << value;
    }

    return stream;
}
