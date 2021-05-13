#ifndef AREG_BASE_TESTACK_HPP
#define AREG_BASE_TESTACK_HPP
/************************************************************************
 * \file        areg/base/TEStack.hpp
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
/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
template <typename VALUE, typename VALUE_TYPE, class Implement> class TEStack;
    template <typename VALUE, typename VALUE_TYPE, class Implement> class TELockStack;
    template <typename VALUE, typename VALUE_TYPE, class Implement> class TENolockStack;

/************************************************************************
 * \brief   In this file defined stack object. The stack is a Single
 *          Linked List, where Elements are pushed and popped according
 *          'first-in-first-out' (FIFO) mechanism, i.e. the first element
 *          is pushed, popped out first.
 *          There are 2 types of stacks -- locking (thread safe) and
 *          non-locking (no thread safe).
 *
 *          Following template classes are defined:
 *
 *              1.  TEStack<VALUE, VALUE_TYPE, Implement>,
 *                  which is a generic base stack object.
 *
 *              2.  TELockStack<VALUE, VALUE_TYPE, Implement>,
 *                  which is a thread safe stack and can be used when
 *                  multiple threads are accessing data in the stack.
 *
 *              3.  TENolockStack<VALUE, VALUE_TYPE, Implement>,
 *                  which is not thread safe stack and should be used
 *                  in objects where data is accessed only from single thread.
 *
 *      For more details, see description of every class.
 *
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       TEStack<VALUE, VALUE_TYPE, Implement>
 *              Simple FIFO Stack base object to queue elements, insert and 
 *              access by push and pop operations. TEStack requires
 *              instance of synchronization object to synchronize
 *              access of stack elements.
 * 
 * \details     The FIFO Stack is a simple single linked object, where
 *              every Block has a link to next Block. Unlike Linked List,
 *              it is not possible to access elements by position or index,
 *              and walking in the stack is only in one direction. The fist
 *              pushed element will be popped out first.
 *              Instead of this object, use defined blocking and non-blocking
 *              stacks, depending whether it is needed to synchronize thread
 *              access in the stack.
 *
 * \tparam  VALUE       the type of stored items. Either should be 
 *                      primitive or should have default constructor 
 *                      and valid assigning operator. Also, should be 
 *                      possible to convert to type VALUE_TYPE.
 * \tparam  VALUE_TYPE  By default same as VALUE, but can be any other
 *                      type, which is converted from type VALUE.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEListImpl<VALUE_TYPE>>
class TEStack   : protected Implement
                , private   TemplateConstants
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
    // TEStack<VALUE, VALUE_TYPE, Implement>::Block class declaration
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
        ~Block( void );

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
    TEStack( IEResourceLock & synchObject );

    /**
     * \brief   Copy constructor.
     *          It is declared as protected to prevent direct call.
     *          Use TENolockStack or TELockStack objects instead.
     * \param   synchObject     The instance of synchronization object
     * \param   source          The Stack source, which contains elements
     **/
    TEStack( IEResourceLock & synchObject, const TEStack<VALUE, VALUE_TYPE, Implement> & source );

public:
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
    inline const TEStack<VALUE, VALUE_TYPE, Implement> & operator = ( const TEStack<VALUE, VALUE_TYPE, Implement> & source );

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
     * \param   source  The stack source to copy elements.
     * \return  Returns the number of copied elements.
     **/
    int copy( const TEStack<VALUE, VALUE_TYPE, Implement> & source );

    /**
     * \brief   Searches element in the stack starting at given position.
     *          The given position should be valid or equal to TemplateConstants::START_POSITION
     *          to search at the beginning of stack.
     * \param   elem        The value to search in the stack.
     * \param   startAt     The starting position to search. It will start to search 
     *                      from beginning if equal to TemplateConstants::START_POSITION.
     * \return  If found element, returns valid position. Otherwise, it returns TemplateConstants::INVALID_POSITION.
     **/
    STACKPOS find(VALUE_TYPE elem, STACKPOS startAt = TemplateConstants::START_POSITION) const;

    /**
     * \brief   Returns the first (head) position of the stack. Returns NULL if empty.
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
     *          Returns NULL if given position was last.
     * \param   pos     The position value to get next.
     * \return  If given position is not last in the stack, it returns next valid position value.
     *          Otherwise, it returns NULL.
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
    inline bool equalValues(VALUE_TYPE Value1, VALUE_TYPE Value2) const;

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
    Block *       mHead;
    /**
     * \brief   Tail of stack. Last element in stack.
     **/
    Block *       mTail;

    /**
     * \brief   The number of elements in the stack.
     **/
    int             mElemCount;

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
    inline void _copyElements( const TEStack<VALUE, VALUE_TYPE, Implement> & source );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    TEStack( void );
    TEStack( const TEStack<VALUE, VALUE_TYPE, Implement> & /* source */ );
};

//////////////////////////////////////////////////////////////////////////
// TELockStack<VALUE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Blocking FIFO stack class template declaration.
 *              In this class data access is synchronized.
 *
 * \details     This blocking class template contains instance of
 *              IEResourceLock to synchronize data access by more
 *              than one thread. Any other thread trying to access elements
 *              will be blocked.
 *              Use this object if elements of stack are accessed more than
 *              one thread.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEListImpl<VALUE_TYPE>>
class TELockStack  : public TEStack<VALUE, VALUE_TYPE, Implement>
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
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TELockStack( const TEStack<VALUE, VALUE_TYPE, Implement> & source );

    /**
     * \brief   Destructor
     **/
    ~TELockStack( void );

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
    inline const TELockStack<VALUE, VALUE_TYPE, Implement> & operator = ( const TEStack<VALUE, VALUE_TYPE, Implement> & source );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Resource lock synchronization object.
     **/
    ResourceLock   mLock;
};

//////////////////////////////////////////////////////////////////////////
// TENolockStack<VALUE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       No blocking FIFO stack class template declaration.
 *              No data access synchronization is performed in this class.
 *
 * \details     This non-blocking class template contains instance of
 *              NolockSynchObject to imitate access synchronization. 
 *              No thread will be blocked accessing stack elements.
 *              Use this object if there is no need to synchronize
 *              element access.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEListImpl<VALUE_TYPE>>
class TENolockStack    : public TEStack<VALUE, VALUE_TYPE, Implement>
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
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TENolockStack( const TEStack<VALUE, VALUE_TYPE, Implement> & source );

    /**
     * \brief   Destructor
     **/
    ~TENolockStack( void );

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
    inline const TENolockStack<VALUE, VALUE_TYPE, Implement> & operator = ( const TEStack<VALUE, VALUE_TYPE, Implement> & source );

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
// TEStack<VALUE, VALUE_TYPE, Implement>::Block class template implementation
// TEStack<VALUE, VALUE_TYPE, Implement> internal class
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Implement>::Block::Block( void ) 
    : mNext (NULL   )
    , mValue(       )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Implement>::Block::Block( VALUE_TYPE newValue ) 
    : mNext (NULL)
    , mValue((VALUE)newValue)
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Implement>::Block::~Block( void )
{   mNext = NULL;   }

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TEStack<VALUE, VALUE_TYPE, Implement>::TEStack( IEResourceLock & synchObject )
    : Implement             ( )
    , TemplateConstants   ( )
    
    , mSynchObject          ( synchObject )
    , mHead                 ( NULL )
    , mTail                 ( NULL )
    , mElemCount            ( 0 )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TEStack<VALUE, VALUE_TYPE, Implement>::TEStack( IEResourceLock & synchObject, const TEStack<VALUE, VALUE_TYPE, Implement> & source )
    : Implement             ( )
    , TemplateConstants   ( )

    , mSynchObject          ( synchObject )
    , mHead                 ( NULL )
    , mTail                 ( NULL )
    , mElemCount            ( 0 )
{
    _copyElements(source);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TEStack<VALUE, VALUE_TYPE, Implement>::~TEStack( void )
{
    _cleanStack();
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline const TEStack<VALUE, VALUE_TYPE, Implement> & TEStack<VALUE, VALUE_TYPE, Implement>::operator = ( const TEStack<VALUE, VALUE_TYPE, Implement> & source )
{   copy(source); return (*this);                       }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline int TEStack<VALUE, VALUE_TYPE, Implement>::getSize( void ) const
{   Lock lock( mSynchObject ); return mElemCount;             }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Implement>::isEmpty( void ) const
{   Lock lock( mSynchObject ); return (mElemCount == 0);      }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Implement>::lock( void ) const
{   return mSynchObject.lock(IESynchObject::WAIT_INFINITE);     }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Implement>::unlock( void ) const
{   return mSynchObject.unlock();                               }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TEStack<VALUE, VALUE_TYPE, Implement>::firstEntry( void ) const
{   Lock lock(mSynchObject); ASSERT(mHead != static_cast<Block *>(NULL)); return mHead->mValue;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TEStack<VALUE, VALUE_TYPE, Implement>::lastEntry( void ) const
{   Lock lock(mSynchObject); ASSERT(mTail != static_cast<Block *>(NULL)); return mTail->mValue;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TEStack<VALUE, VALUE_TYPE, Implement>::copy( const TEStack<VALUE, VALUE_TYPE, Implement> & source )
{
    Lock lock( mSynchObject );
    if (static_cast<const TEStack<VALUE, VALUE_TYPE, Implement> *>(this) != &source)
    {
        Lock lockThis(mSynchObject);
        Lock lockSource(source.mSynchObject);

        _cleanStack();
        _copyElements(source);
    }

    return mElemCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TEStack<VALUE, VALUE_TYPE, Implement>::pushLast( VALUE_TYPE newElement )
{
    Lock lock(mSynchObject);

    Block * newBlock = DEBUG_NEW Block(newElement);
    if (newBlock != NULL)
    {
        if ( mHead == NULL )
        {
            ASSERT(mTail        == NULL);
            ASSERT(mElemCount   == 0);
            mHead = mTail   = newBlock;
        }
        else
        {
            ASSERT(mTail        != NULL);
            ASSERT(mElemCount   != 0);
            mTail->mNext = newBlock;
            mTail = newBlock;
        }

        ++ mElemCount;
    }

    return mElemCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TEStack<VALUE, VALUE_TYPE, Implement>::pushFirst( VALUE_TYPE newElement )
{
    Lock lock(mSynchObject);

    Block* newBlock = DEBUG_NEW Block(newElement);
    if (newBlock != NULL)
    {
        if ( mHead == NULL )
        {
            ASSERT(mTail        == NULL);
            ASSERT(mElemCount   == 0);
            mHead = mTail = newBlock;
        }
        else
        {
            ASSERT(mTail        != NULL);
            ASSERT(mElemCount   != 0);
            newBlock->mNext = mHead;
            mHead = newBlock;
        }

        ++ mElemCount;
    }

    return mElemCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
VALUE TEStack<VALUE, VALUE_TYPE, Implement>::popFirst( void )
{
    Lock lock(mSynchObject);

    VALUE result = VALUE();

    if (mElemCount != 0 )
    {
        Block* block = mHead;
        mHead = block->mNext;
        -- mElemCount;
        if ( mHead == NULL )
        {
            ASSERT( mElemCount == 0 );
            mTail = NULL;
        }

        result = block->mValue;
        delete block;
    }

    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline void TEStack<VALUE, VALUE_TYPE, Implement>::removeAll( void )
{   Lock lock(mSynchObject); _cleanStack();           }


template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
STACKPOS TEStack<VALUE, VALUE_TYPE, Implement>::find(VALUE_TYPE elem, STACKPOS startAt /*= TemplateConstants::START_POSITION*/) const
{
    Lock lock(mSynchObject);

    STACKPOS result = static_cast<STACKPOS>(TemplateConstants::INVALID_POSITION);
    Block * header = startAt == TemplateConstants::START_POSITION ? mHead : reinterpret_cast<Block *>(startAt)->mNext;
    while (header != static_cast<Block *>(NULL))
    {
        if (equalValues(elem, header->mValue))
        {
            result = static_cast<STACKPOS>(header);
            break;
        }

        header = header->mNext;
    }

    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline STACKPOS TEStack<VALUE, VALUE_TYPE, Implement>::firstPosition( void ) const
{
    Lock lock(mSynchObject);
    return static_cast<STACKPOS>(mHead);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TEStack<VALUE, VALUE_TYPE, Implement>::getAt( const STACKPOS pos ) const
{
    Lock lock(mSynchObject);
    ASSERT(pos != NULL);
    return reinterpret_cast<const Block *>(pos)->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE & TEStack<VALUE, VALUE_TYPE, Implement>::getAt( STACKPOS pos )
{
    Lock lock(mSynchObject);
    ASSERT(pos != NULL);
    return reinterpret_cast<Block *>(pos)->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline STACKPOS TEStack<VALUE, VALUE_TYPE, Implement>::nextPosition( const STACKPOS pos ) const
{
    Lock lock(mSynchObject);
    return static_cast<STACKPOS>(pos != NULL ? reinterpret_cast<const Block *>(pos)->mNext : NULL);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Implement>::equalValues(VALUE_TYPE Value1, VALUE_TYPE Value2) const
{   return Implement::implEqualValues(Value1, Value2);      }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline void TEStack<VALUE, VALUE_TYPE, Implement>::_cleanStack( void )
{
    while (mHead != NULL)
    {
        Block* block = mHead;
        mHead = block->mNext;
        delete block;
    }

    mHead = mTail = NULL;
    mElemCount = 0;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline void TEStack<VALUE, VALUE_TYPE, Implement>::_copyElements( const TEStack<VALUE, VALUE_TYPE, Implement> & source )
{
    for ( Block * srcBlock = source.mHead; srcBlock != NULL; srcBlock = srcBlock->mNext )
    {
        Block * newBlock = DEBUG_NEW Block( srcBlock->mValue );
        if ( newBlock != NULL )
        {
            if ( mHead == NULL )
            {
                ASSERT( mTail       == NULL );
                ASSERT( mElemCount  == 0 );
                mHead = mTail = newBlock;
            }
            else
            {
                ASSERT( mTail       != NULL );
                ASSERT( mElemCount  != 0 );
                mTail->mNext = newBlock;
                mTail = newBlock;
            }

            ++ mElemCount;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// TELockStack<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TELockStack<VALUE, VALUE_TYPE, Implement>::TELockStack( void )
    : TEStack<VALUE, VALUE_TYPE, Implement>(mLock)
    , mLock ( false )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TELockStack<VALUE, VALUE_TYPE, Implement>::TELockStack( const TEStack<VALUE, VALUE_TYPE, Implement> & source )
    : TEStack<VALUE, VALUE_TYPE, Implement>( mLock, source )
    , mLock ( false )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TELockStack<VALUE, VALUE_TYPE, Implement>::~TELockStack( void )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline const TELockStack<VALUE, VALUE_TYPE, Implement> & TELockStack<VALUE, VALUE_TYPE, Implement>::operator = ( const TEStack<VALUE, VALUE_TYPE, Implement> & source )
{   static_cast<TEStack<VALUE, VALUE_TYPE, Implement> &>(*this) = source; return (*this);     }

//////////////////////////////////////////////////////////////////////////
// TENolockStack<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TENolockStack<VALUE, VALUE_TYPE, Implement>::TENolockStack( void )
    : TEStack<VALUE, VALUE_TYPE, Implement>(mNoLock)

    , mNoLock   ( false )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TENolockStack<VALUE, VALUE_TYPE, Implement>::TENolockStack( const TEStack<VALUE, VALUE_TYPE, Implement> & source )
    : TEStack<VALUE, VALUE_TYPE, Implement>( mNoLock, source )
    
    , mNoLock   ( false )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TENolockStack<VALUE, VALUE_TYPE, Implement>::~TENolockStack( void )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline const TENolockStack<VALUE, VALUE_TYPE, Implement> & TENolockStack<VALUE, VALUE_TYPE, Implement>::operator = ( const TEStack<VALUE, VALUE_TYPE, Implement> & source )
{   static_cast<TEStack<VALUE, VALUE_TYPE, Implement> &>(*this) = source; return (*this);     }

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Implement> friend operators implementation
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
    for ( ; block != NULL; block = block->mNext)
        stream << block->mValue;

    return stream;
}

#endif  // AREG_BASE_TESTACK_HPP
