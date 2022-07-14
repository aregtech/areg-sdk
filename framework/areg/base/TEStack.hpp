#ifndef AREG_BASE_TESTACK_HPP
#define AREG_BASE_TESTACK_HPP
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
template <typename VALUE> class TEStack;
    template <typename VALUE> class TELockStack;
    template <typename VALUE> class TENolockStack;

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Simple FIFO Stack base object to queue elements, insert and 
 *          access by push and pop operations. TEStack requires instance 
 *          of resource lock object to synchronize access of stack elements.
 *          Whether the Stack is thread safe or not, depends on type of
 *          synchronization object passed in stack.
 * 
 * \tparam  VALUE   The type of stored items. Either should be primitive 
 *                  or should have default constructor and valid assigning 
 *                  operator. And should be possible to convert to type const VALUE &.
 **/
template <typename VALUE>
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
    TEStack( IEResourceLock & synchObject, const TEStack<VALUE> & source );

    /**
     * \brief   Initializes the resource lock object and move elements from given source.
     * \param   synchObject     The instance of synchronization object
     * \param   source          The Stack source, which contains elements to move.
     **/
    TEStack( IEResourceLock & synchObject, TEStack<VALUE> && source ) noexcept;

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
    inline TEStack<VALUE> & operator = ( const TEStack<VALUE> & source );

    /**
     * \brief   Assigning operator. It is moves stack elements from given source
     *          in the same order as they are created in the source stack.
     *          If stack had entries, all entries will be lost and new entries will be created.
     * \param   source  The instance of source to get stack entries.
     * \return  Returns stack object.
     **/
    inline TEStack<VALUE> & operator = ( TEStack<VALUE> && source ) noexcept;

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
    template<typename VALUE>
    friend const IEInStream & operator >> (const IEInStream & stream, TEStack<VALUE> & input);
    /**
     * \brief   Writes to the stream Stack values.
     *          The values will be written to the stream starting from head position.
     *          There should be possibility to stream every value of Stack and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Stack object to read out values.
     **/
    template<typename VALUE>
    friend IEOutStream & operator << (IEOutStream & stream, const TEStack<VALUE> & output);

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
    inline uint32_t copy( const TEStack<VALUE> & source );

    /**
     * \brief   Moves all elements from given source and returns the number of moved elements.
     *          If Stack previously had elements, they will be lost.
     * \param   source  The stack source to copy elements.
     * \return  Returns the number of copied elements.
     **/
    inline uint32_t move( TEStack<VALUE> && source ) noexcept;

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
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    //! The stack object, which contains the elements.
    std::deque<VALUE>   mValueList;

     //! The instance of synchronization object to be used to make object thread-safe.
    IEResourceLock &    mSynchObject;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    TEStack( void ) = delete;
    TEStack( const TEStack<VALUE> & /* source */ ) = delete;
    TEStack( TEStack<VALUE> && /* source */ ) = delete;
};

//////////////////////////////////////////////////////////////////////////
// TELockStack<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Thread safe FIFO stack class template declaration, where accessing
 *          data is synchronized by resource lock. Use this object if elements 
 *          of stack are accessed by more than one thread.
 **/
template <typename VALUE>
class TELockStack  : public TEStack<VALUE>
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
    explicit TELockStack( const TEStack<VALUE> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    explicit TELockStack( TEStack<VALUE> && source ) noexcept;

    /**
     * \brief   Copies data from given source.
     * \param   source  The source to copy data.
     **/
    TELockStack( const TELockStack<VALUE> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    TELockStack( TELockStack<VALUE> && source ) noexcept;

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
    inline TELockStack<VALUE> & operator = ( const TELockStack<VALUE> & source );

    /**
     * \brief   Moves entries from given sources. If stack had entries
     *          all entries will be lost and new entries will be created.
     * \param   source  The instance of source to move stack entries.
     * \return  Returns stack object.
     **/
    inline TELockStack<VALUE> & operator = ( TELockStack<VALUE> && source ) noexcept;

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
// TENolockStack<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   No thread safe FIFO stack class template declaration, where accessing
 *          data is not synchronized. Use this object if elements of stack 
 *          are accessed only by one thread.
 **/
template <typename VALUE>
class TENolockStack    : public TEStack<VALUE>
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
    explicit TENolockStack( const TEStack<VALUE> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    explicit TENolockStack( TEStack<VALUE> && source ) noexcept;

    /**
     * \brief   Copies data from given source.
     * \param   source  The source to copy data.
     **/
    TENolockStack( const TENolockStack<VALUE> & source );

    /**
     * \brief   Moves data from given source.
     * \param   source  The source to move data.
     **/
    TENolockStack( TENolockStack<VALUE> && source ) noexcept;

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
    inline TENolockStack<VALUE> & operator = ( const TENolockStack<VALUE> & source );

    /**
     * \brief   Moves entries from given sources. If stack had entries
     *          all entries will be lost and new entries will be created.
     * \param   source  The instance of source to move stack entries.
     * \return  Returns stack object.
     **/
    inline TENolockStack<VALUE> & operator = ( TENolockStack<VALUE> && source ) noexcept;

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
// TEStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TEStack<VALUE>::TEStack( IEResourceLock & synchObject )
    : mValueList    ( )
    , mSynchObject  ( synchObject )
{
}

template <typename VALUE>
TEStack<VALUE>::TEStack( IEResourceLock & synchObject, const TEStack<VALUE> & source )
    : mValueList    ( source.mValueList )
    , mSynchObject  ( synchObject )
{
}

template <typename VALUE>
TEStack<VALUE>::TEStack( IEResourceLock & synchObject, TEStack<VALUE> && source ) noexcept
    : mValueList    ( std::move(source.mValueList) )
    , mSynchObject  ( synchObject )
{
}

template <typename VALUE>
TEStack<VALUE>::~TEStack( void )
{
    mSynchObject.lock();
    mValueList.clear();
    mSynchObject.unlock();
}

template <typename VALUE>
inline TEStack<VALUE> & TEStack<VALUE>::operator = ( const TEStack<VALUE> & source )
{
    copy(source);
    return (*this);
}

template <typename VALUE>
inline TEStack<VALUE> & TEStack<VALUE>::operator = ( TEStack<VALUE> && source ) noexcept
{
    move( std::move(source) );
    return (*this);
}

template <typename VALUE>
inline const VALUE& TEStack<VALUE>::operator [] (STACKPOS atPosition) const
{
    Lock lock(mSynchObject);
    return (*atPosition);
}

template <typename VALUE>
inline VALUE& TEStack<VALUE>::operator [] (STACKPOS atPosition)
{
    Lock lock(mSynchObject);
    return (*atPosition);
}

template <typename VALUE>
inline uint32_t TEStack<VALUE>::getSize( void ) const
{
    Lock lock( mSynchObject );
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline bool TEStack<VALUE>::isEmpty( void ) const
{
    Lock lock( mSynchObject );
    return mValueList.empty();
}

template <typename VALUE>
inline bool TEStack<VALUE>::isStartPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    return (pos = mValueList.begin());
}

template <typename VALUE>
inline bool TEStack<VALUE>::isLastPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    return (mValueList.empty() == false) && (pos = --mValueList.end());
}

template <typename VALUE>
inline typename TEStack<VALUE>::STACKPOS TEStack<VALUE>::invalidPosition(void) const
{
    Lock lock(mSynchObject);
    return Constless::iter(mValueList, mValueList.end());
}

template <typename VALUE>
inline bool TEStack<VALUE>::isValidPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    return (pos != mValueList.end());
}

template <typename VALUE>
inline bool TEStack<VALUE>::isInvalidPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    return (pos == mValueList.end());
}

template <typename VALUE>
inline bool TEStack<VALUE>::checkPosition(STACKPOS pos) const
{
    Lock lock(mSynchObject);
    std::deque<VALUE>::const_iterator it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template <typename VALUE>
inline void TEStack<VALUE>::clear(void)
{
    Lock lock(mSynchObject);
    mValueList.clear();
}

template <typename VALUE>
inline void TEStack<VALUE>::freeExtra(void)
{
    Lock lock(mSynchObject);
    mValueList.shrink_to_fit();
}

template <typename VALUE>
inline void TEStack<VALUE>::release(void)
{
    Lock lock(mSynchObject);
    mValueList.clear();
    mValueList.shrink_to_fit();
}

template <typename VALUE>
inline bool TEStack<VALUE>::lock( void ) const
{
    return mSynchObject.lock(NECommon::WAIT_INFINITE);
}

template <typename VALUE>
inline bool TEStack<VALUE>::unlock( void ) const
{
    return mSynchObject.unlock();
}

template <typename VALUE>
inline const VALUE & TEStack<VALUE>::firstEntry( void ) const
{
    Lock lock(mSynchObject);
    return mValueList.front();
}

template <typename VALUE>
inline const VALUE & TEStack<VALUE>::lastEntry( void ) const
{
    Lock lock(mSynchObject);
    return mValueList.back();
}

template <typename VALUE>
inline uint32_t TEStack<VALUE>::copy( const TEStack<VALUE> & source )
{
    Lock lock(mSynchObject);
    Lock lockSource(source.mSynchObject);

    mValueList = source.mValueList;
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t TEStack<VALUE>::move( TEStack<VALUE> && source ) noexcept
{
    Lock lock(mSynchObject);
    Lock lockSource(source.mSynchObject);

    mValueList = std::move(source.mValueList);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t TEStack<VALUE>::pushLast( const VALUE & newElement )
{
    Lock lock(mSynchObject);
    mValueList.push_back(newElement);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t TEStack<VALUE>::pushLast(VALUE && newElement)
{
    Lock lock(mSynchObject);
    mValueList.push_back(std::move(newElement));
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t TEStack<VALUE>::pushFirst( const VALUE & newElement )
{
    Lock lock(mSynchObject);
    mValueList.push_front(newElement);
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
inline uint32_t TEStack<VALUE>::pushFirst(VALUE && newElement)
{
    Lock lock(mSynchObject);
    mValueList.push_front(std::move(newElement));
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE>
VALUE TEStack<VALUE>::popFirst( void )
{
    Lock lock(mSynchObject);

    VALUE result = mValueList.front();
    mValueList.pop_front();
    return result;
}

template <typename VALUE>
inline typename TEStack<VALUE>::STACKPOS TEStack<VALUE>::find(const VALUE& Value) const
{
    Lock lock(mSynchObject);
    auto it = std::find(mValueList.begin(), mValueList.end(), Value);
    return Constless::iter(it);
}

template <typename VALUE>
inline typename TEStack<VALUE>::STACKPOS TEStack<VALUE>::find(const VALUE & Value, STACKPOS searchAfter ) const
{
    Lock lock(mSynchObject);
    STACKPOS end = invalidPosition();
    return (searchAfter != end ? std::find(++searchAfter, end, Value) : end);
}

template <typename VALUE>
inline typename TEStack<VALUE>::STACKPOS TEStack<VALUE>::firstPosition( void ) const
{
    Lock lock(mSynchObject);
    return Constless::iter(mValueList, mValueList.begin());
}

template <typename VALUE>
inline const VALUE & TEStack<VALUE>::getAt( const STACKPOS pos ) const
{
    Lock lock(mSynchObject);

    ASSERT(pos != mValueList.end());
    return (*pos);
}

template <typename VALUE>
inline VALUE & TEStack<VALUE>::getAt( STACKPOS pos )
{
    Lock lock(mSynchObject);

    ASSERT(pos != mValueList.end());
    return (*pos);
}

template <typename VALUE>
inline typename TEStack<VALUE>::STACKPOS TEStack<VALUE>::nextPosition( STACKPOS pos ) const
{
    Lock lock(mSynchObject);

    ASSERT(pos != mValueList.end());
    return (++pos);
}

//////////////////////////////////////////////////////////////////////////
// TELockStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
TELockStack<VALUE>::TELockStack( void )
    : TEStack<VALUE>(mLock)
    , mLock ( )
{
}

template <typename VALUE>
TELockStack<VALUE>::TELockStack( const TEStack<VALUE> & source )
    : TEStack<VALUE>( mLock, source )
    , mLock ( )
{
}

template <typename VALUE>
TELockStack<VALUE>::TELockStack( TEStack<VALUE> && source ) noexcept
    : TEStack<VALUE>( mLock, static_cast<TEStack<VALUE> &&>(source) )
    , mLock ( )
{
}

template <typename VALUE>
TELockStack<VALUE>::TELockStack( const TELockStack<VALUE> & source )
    : TEStack<VALUE>( mLock, source )
    , mLock ( )
{
}

template <typename VALUE>
TELockStack<VALUE>::TELockStack( TELockStack<VALUE> && source ) noexcept
    : TEStack<VALUE>( mLock, static_cast<TEStack<VALUE> &&>(source) )
    , mLock ( )
{
}

template <typename VALUE>
inline TELockStack<VALUE> & TELockStack<VALUE>::operator = ( const TELockStack<VALUE> & source )
{
    static_cast<TEStack<VALUE> &>(*this) = source;
    return (*this);
}

template <typename VALUE>
inline TELockStack<VALUE> & TELockStack<VALUE>::operator = ( TELockStack<VALUE> && source ) noexcept
{
    static_cast<TEStack<VALUE> &>(*this) = static_cast<TEStack<VALUE> &&>(source);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// TENolockStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
TENolockStack<VALUE>::TENolockStack( void )
    : TEStack<VALUE>(mNoLock)
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockStack<VALUE>::TENolockStack( const TEStack<VALUE> & source )
    : TEStack<VALUE>( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockStack<VALUE>::TENolockStack( TEStack<VALUE> && source ) noexcept
    : TEStack<VALUE>( mNoLock, static_cast<TEStack<VALUE> &&>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockStack<VALUE>::TENolockStack( const TENolockStack<VALUE> & source )
    : TEStack<VALUE>( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockStack<VALUE>::TENolockStack( TENolockStack<VALUE> && source ) noexcept
    : TEStack<VALUE>( mNoLock, static_cast<TEStack<VALUE> &&>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE>
inline TENolockStack<VALUE> & TENolockStack<VALUE>::operator = ( const TENolockStack<VALUE> & source )
{
    static_cast<TEStack<VALUE> &>(*this) = source;
    return (*this);
}

template <typename VALUE>
inline TENolockStack<VALUE> & TENolockStack<VALUE>::operator = ( TENolockStack<VALUE> && source ) noexcept
{
    static_cast<TEStack<VALUE> &>(*this) = static_cast<TEStack<VALUE> &&>(source);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE> friend operators implementation
//////////////////////////////////////////////////////////////////////////
template<typename VALUE>
const IEInStream & operator >> ( const IEInStream & stream, TEStack<VALUE> & input )
{
    Lock lock(input.mSynchObject);

    input.mValueList.clear();
    uint32_t size = 0;
    stream >> size;
    input.mValueList.resize(size);
    for (auto& elem : input.mValueList)
    {
        stream >> elem;
    }

    return stream;
}

template<typename VALUE>
IEOutStream & operator << ( IEOutStream & stream, const TEStack<VALUE> & output )
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

#endif  // AREG_BASE_TESTACK_HPP
