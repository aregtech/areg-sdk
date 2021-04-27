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
                , private   CETemplateConstants
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
    // TEStack<VALUE, VALUE_TYPE, Implement>::CEBlock class declaration
    //////////////////////////////////////////////////////////////////////////
    class CEBlock
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Block default constructor.
         **/
        CEBlock( void );
        /**
         * \brief   Block initialization constructor. Sets value
         **/
        CEBlock( VALUE_TYPE newValue );
        /**
         * \brief   Destructor.
         **/
        ~CEBlock( void );

    //////////////////////////////////////////////////////////////////////////
    // Public member variables.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The pointer to block block in the stack
         **/
        CEBlock *   mNext;
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
    inline int GetSize( void ) const;

    /**
     * \brief   Returns true if Ring Stack is empty
     **/
    inline bool IsEmpty( void ) const;

    /**
     * \brief   Locks stack that methods can be accessed only from locking thread.
     *          In case if CENolockSynchObject is used, no locking will happen,
     *          the function will return immediately and thread will continue to run.
     * \return  Returns true if stack successfully locked
     **/
    inline bool LockStack( void ) const;

    /**
     * \brief   If stack previously was locked by thread, it will unlock stack
     *          In case if CENolockSynchObject is used, nothing will happen.
     * \return  Returns true if stack successfully unlocked
     **/
    inline bool UnlockStack( void ) const;

    /**
     * \brief   Returns first inserted element in the stack without changing stack.
     *          The caller of function should make sure that the stack is not empty,
     *          otherwise it may cause system crash.
     *          Check the size of stack before calling function.
     **/
    inline VALUE_TYPE GetFirstElement( void ) const;

    /**
     * \brief   Returns first inserted element in the stack without changing stack.
     *          The caller of function should make sure that the stack is not empty,
     *          otherwise it may cause system crash.
     *          Check the size of stack before calling function.
     **/
    inline VALUE_TYPE GetLastElement( void ) const;

    /**
     * \brief	Push new element at the end of stack.
     * \param	newElement  New Element to push into stack
     * \return  Returns size of stack.
     **/
    int PushElement( VALUE_TYPE newElement );

    /**
     * \brief	Push new element in the beginning of stack, that on pop
     *          operation will be be popped first.
     * \param	newElement  New Element to push into stack
     * \return  Returns size of stack.
     **/
    int PushElementBegin( VALUE_TYPE newElement );

    /**
     * \brief	Pops element from Stack and returns value of element.
     *          The stack should not be empty before calling this method.
     *          Otherwise assertion is raised.
     * \return	Returns Element from stack.
     **/
    VALUE PopElement( void );

    /**
     * \brief   Removes all elements from stack and makes it empty.
     **/
    inline void RemoveAll( void );

    /**
     * \brief   Copies all elements from given source and returns the number of copied elements.
     * \param   source  The stack source to copy elements.
     * \return  Returns the number of copied elements.
     **/
    int CopyElements( const TEStack<VALUE, VALUE_TYPE, Implement> & source );

    /**
     * \brief   Searches element in the stack starting at given position.
     *          The given position should be valid or equal to CETemplateConstants::START_POSITION
     *          to search at the beginning of stack.
     * \param   elem        The value to search in the stack.
     * \param   startAt     The starting position to search. It will start to search 
     *                      from beginning if equal to CETemplateConstants::START_POSITION.
     * \return  If found element, returns valid position. Otherwise, it returns CETemplateConstants::INVALID_POSITION.
     **/
    STACKPOS Find(VALUE_TYPE elem, STACKPOS startAt = CETemplateConstants::START_POSITION) const;

    STACKPOS GetFirstPosition( void ) const;

    VALUE_TYPE GetAt( const STACKPOS pos ) const;

    VALUE & GetAt( STACKPOS pos );

    STACKPOS GetNextPosition( const STACKPOS pos ) const;

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Compares and returns true if 2 values are equal.
     *          Specify own ImplEqualValues() method of TEListImpl
     *          class template for custom comparing method.
     * \param   Value1  Left-side value to compare.
     * \param   Value2  Right-side value to compare.
     * \return  Returns true if 2 value are equal.
     **/
    inline bool EqualValues(VALUE_TYPE Value1, VALUE_TYPE Value2) const;

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
    CEBlock *       mHead;
    /**
     * \brief   Tail of stack. Last element in stack.
     **/
    CEBlock *       mTail;

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
    CEResourceLock   mLock;
};

//////////////////////////////////////////////////////////////////////////
// TENolockStack<VALUE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       No blocking FIFO stack class template declaration.
 *              No data access synchronization is performed in this class.
 *
 * \details     This non-blocking class template contains instance of
 *              CENolockSynchObject to imitate access synchronization. 
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
    CENolockSynchObject mNoLock;
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Implement>::CEBlock class template implementation
// TEStack<VALUE, VALUE_TYPE, Implement> internal class
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Implement>::CEBlock::CEBlock( void ) 
    : mNext (NULL   )
    , mValue(       )
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Implement>::CEBlock::CEBlock( VALUE_TYPE newValue ) 
    : mNext (NULL)
    , mValue((VALUE)newValue)
{   ;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline TEStack<VALUE, VALUE_TYPE, Implement>::CEBlock::~CEBlock( void )
{   mNext = NULL;   }

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
TEStack<VALUE, VALUE_TYPE, Implement>::TEStack( IEResourceLock & synchObject )
    : Implement             ( )
    , CETemplateConstants   ( )
    
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
    , CETemplateConstants   ( )

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
{   CopyElements(source); return (*this);                       }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline int TEStack<VALUE, VALUE_TYPE, Implement>::GetSize( void ) const
{   CELock lock( mSynchObject ); return mElemCount;             }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Implement>::IsEmpty( void ) const
{   CELock lock( mSynchObject ); return (mElemCount == 0);      }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Implement>::LockStack( void ) const
{   return mSynchObject.Lock(IESynchObject::WAIT_INFINITE);     }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Implement>::UnlockStack( void ) const
{   return mSynchObject.Unlock();                               }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TEStack<VALUE, VALUE_TYPE, Implement>::GetFirstElement( void ) const
{   CELock lock(mSynchObject); ASSERT(mHead != static_cast<CEBlock *>(NULL)); return mHead->mValue;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TEStack<VALUE, VALUE_TYPE, Implement>::GetLastElement( void ) const
{   CELock lock(mSynchObject); ASSERT(mTail != static_cast<CEBlock *>(NULL)); return mTail->mValue;   }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TEStack<VALUE, VALUE_TYPE, Implement>::CopyElements( const TEStack<VALUE, VALUE_TYPE, Implement> & source )
{
    CELock lock( mSynchObject );
    if (static_cast<const TEStack<VALUE, VALUE_TYPE, Implement> *>(this) != &source)
    {
        CELock lockThis(mSynchObject);
        CELock lockSource(source.mSynchObject);

        _cleanStack();
        _copyElements(source);
    }

    return mElemCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
int TEStack<VALUE, VALUE_TYPE, Implement>::PushElement( VALUE_TYPE newElement )
{
    CELock lock(mSynchObject);

    CEBlock * newBlock = DEBUG_NEW CEBlock(newElement);
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
int TEStack<VALUE, VALUE_TYPE, Implement>::PushElementBegin( VALUE_TYPE newElement )
{
    CELock lock(mSynchObject);

    CEBlock* newBlock = DEBUG_NEW CEBlock(newElement);
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
VALUE TEStack<VALUE, VALUE_TYPE, Implement>::PopElement( void )
{
    CELock lock(mSynchObject);

    VALUE result = VALUE();

    if (mElemCount != 0 )
    {
        CEBlock* block = mHead;
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
inline void TEStack<VALUE, VALUE_TYPE, Implement>::RemoveAll( void )
{   CELock lock(mSynchObject); _cleanStack();           }


template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
STACKPOS TEStack<VALUE, VALUE_TYPE, Implement>::Find(VALUE_TYPE elem, STACKPOS startAt /*= CETemplateConstants::START_POSITION*/) const
{
    CELock lock(mSynchObject);

    STACKPOS result = static_cast<STACKPOS>(CETemplateConstants::INVALID_POSITION);
    CEBlock * header = startAt == CETemplateConstants::START_POSITION ? mHead : reinterpret_cast<CEBlock *>(startAt)->mNext;
    while (header != static_cast<CEBlock *>(NULL))
    {
        if (EqualValues(elem, header->mValue))
        {
            result = static_cast<STACKPOS>(header);
            break;
        }

        header = header->mNext;
    }

    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline STACKPOS TEStack<VALUE, VALUE_TYPE, Implement>::GetFirstPosition( void ) const
{
    CELock lock(mSynchObject);
    return static_cast<STACKPOS>(mHead);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TEStack<VALUE, VALUE_TYPE, Implement>::GetAt( const STACKPOS pos ) const
{
    CELock lock(mSynchObject);
    ASSERT(pos != NULL);
    return reinterpret_cast<const CEBlock *>(pos)->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline VALUE & TEStack<VALUE, VALUE_TYPE, Implement>::GetAt( STACKPOS pos )
{
    CELock lock(mSynchObject);
    ASSERT(pos != NULL);
    return reinterpret_cast<CEBlock *>(pos)->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline STACKPOS TEStack<VALUE, VALUE_TYPE, Implement>::GetNextPosition( const STACKPOS pos ) const
{
    CELock lock(mSynchObject);
    return static_cast<STACKPOS>(pos != NULL ? reinterpret_cast<const CEBlock *>(pos)->mNext : NULL);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline bool TEStack<VALUE, VALUE_TYPE, Implement>::EqualValues(VALUE_TYPE Value1, VALUE_TYPE Value2) const
{   return Implement::ImplEqualValues(Value1, Value2);      }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline void TEStack<VALUE, VALUE_TYPE, Implement>::_cleanStack( void )
{
    while (mHead != NULL)
    {
        CEBlock* block = mHead;
        mHead = block->mNext;
        delete block;
    }

    mHead = mTail = NULL;
    mElemCount = 0;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
inline void TEStack<VALUE, VALUE_TYPE, Implement>::_copyElements( const TEStack<VALUE, VALUE_TYPE, Implement> & source )
{
    for ( CEBlock * srcBlock = source.mHead; srcBlock != NULL; srcBlock = srcBlock->mNext )
    {
        CEBlock * newBlock = DEBUG_NEW CEBlock( srcBlock->mValue );
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
    CELock lock(input.mSynchObject);

    input._cleanStack();
    int size = 0;
    stream >> size;
    for (int i = 0; i < size; ++ i)
    {
        V newElement;
        stream >> newElement;
        static_cast<void>(input.PushElement(newElement));
    }

    return stream;
}

template<typename V, typename VT, class Impl>
IEOutStream & operator << ( IEOutStream & stream, const TEStack<V, VT, Impl> & output )
{
    CELock lock(output.mSynchObject);

    int size = output.GetSize();
    stream << size;
    class TEStack<V, VT, Impl>::CEBlock* block = output.mHead;
    for ( ; block != NULL; block = block->mNext)
        stream << block->mValue;

    return stream;
}

#endif  // AREG_BASE_TESTACK_HPP
