#ifndef AREG_BASE_TERINGSTACK_HPP
#define AREG_BASE_TERINGSTACK_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/TERingStack.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
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
#include "areg/base/TETemplateBase.hpp"
#include "areg/base/SynchObjects.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/NEMath.hpp"

/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
template <typename VALUE> class TERingStack;
    template <typename VALUE> class TELockRingStack;
    template <typename VALUE> class TENolockRingStack;

//////////////////////////////////////////////////////////////////////////
// TEStack<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Ring FIFO Stack base object to queue elements, insert and access
 *          by push and pop operations. RingStack requires instance of
 *          synchronization object to synchronize access of stack elements,
 *          capacity value and overlapping flag.
 *
 *          The capacity might be changed depending on overlapping flag.
 *          If ring stack is full, whether the capacity remains same or not,
 *          whether new element is pushed or not, depends on overlapping flag.
 *          For more details of capacity flag see NECommon::eRingOverlap
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
 * \see     TENolockRingStack, TELockRingStack.
 **/
template <typename VALUE>
class TERingStack
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Ring Stack initialization. Gets instance of synchronization object,
     *          initial capacity value and the overlapping flag, used when
     *          ring stack is full and new element should be pushed.
     * \param   synchObject     Reference to synchronization object.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          it is required to insert new element.
     **/
    explicit TERingStack( IEResourceLock & synchObject, uint32_t initCapacity = 0, NECommon::eRingOverlap onOverlap = NECommon::eRingOverlap::StopOnOverlap );

    /**
     * \brief   Destructor. Public
     **/
    ~TERingStack( void );

protected:
    /**
     * \brief   Creates a Ring Stack object and copies elements from the given source.
     * \param   synchObject     Reference to synchronization object.
     * \param   source          The source of Ring Stack elements.
     **/
    explicit TERingStack(IEResourceLock& synchObject, const TERingStack<VALUE>& source);

    /**
     * \brief   Creates a Ring Stack object and moves elements from the given source.
     * \param   synchObject     Reference to synchronization object.
     * \param   source          The source of Ring Stack elements.
     **/
    explicit TERingStack(IEResourceLock& synchObject, TERingStack<VALUE> && source) noexcept;

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
    TERingStack<VALUE> & operator = ( const TERingStack<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from given source.
     *          It as well swaps all entries of source and destination, but
     *          does not change the overlapping type and synchronization object.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    TERingStack<VALUE>& operator = (TERingStack<VALUE> && source) noexcept;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are equal.
     **/
    bool operator == (const TERingStack<VALUE>& other) const;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are not equal.
     **/
    bool operator != (const TERingStack<VALUE>& other) const;

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
    friend const IEInStream & operator >> ( const IEInStream & stream, TERingStack<V> & input );
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
    friend IEOutStream & operator << ( IEOutStream & stream, const TERingStack<V> & output );

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
     * \brief   Returns the overlapping type of the Ring Stack
     **/
    inline NECommon::eRingOverlap getOverlap(void) const;

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
    inline uint32_t capacity( void ) const;

    /**
     * \brief   Returns true if Ring Stack is full. The function returns false if
     *          the ring stack is of 'resize of overlap' type, because it automatically changes the size.
     **/
    inline bool isFull( void ) const;

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
     *          1.  If overlap flag is StopOnOverlap, the element will not be set.
     *          2.  If overlap flag is ShiftOnOverlap, the element will be set at the tail of stack,
     *              but the size of Ring Stack will not be changed. The element on head of stack 
     *              will be lost.
     *          3.  If overlap flag is ResizeOnOvelap, it will resize ring stack
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
    VALUE pop( void );

    /**
     * \brief   Removes all elements from Ring stack and makes it empty.
     *          The capacity of stack remains unchanged. The change capacity value, resize stack.
     **/
    inline void clear( void );

    /**
     * \brief   Clears the ring stack, deletes the list and sets capacity zero.
     **/
    void release( void );

    /**
     * \brief   Removes the extra entries in the ring stack and makes capacity equal to the number of elements in the stack.
     **/
    void freeExtra(void);

    /**
     * \brief   Adds elements from given source. The elements will be copied at the end of stack.
     *          If capacity of stack is small to copy all elements from the source, the results depends on 
     *          overlapping flag of stack:
     *          1.  If overlap flag is StopOnOverlap, the elements will be copied until the stack is not full.
     *              When stack is full, no more elements will be copied.
     *          2.  If overlap flag is ShiftOnOverlap, the element will be copied until the stack is not full.
     *              Then the elements will be set by removing head of stack until all elements from given source
     *              are not copied. The capacity of stack will remain unchanged. If during copying stack is full,
     *              the elements at head are lost.
     *          3.  If overlap flag is ResizeOnOvelap and if elements in source are bigger than capacity of stack,
     *              the capacity of stack will be increased that no elements are lost and all elements from source
     *              are copied. No data will be lost.
     * \param   source  The source of Ring stack to get elements.
     * \return  Returns number of elements copied in to the stack. The number of copied elements and elements in stack
     *          might differ depending on overlapping flag.
     **/
    uint32_t add( const TERingStack<VALUE> & source );

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
    void copy(const TERingStack<VALUE> & source);

    /**
     * \brief   Moves entries from the given source. The previous entries are swapped with the source.
     * \param   source  The source of entries to move data.
     **/
    void move(TERingStack<VALUE> && source);

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
    IEResourceLock &                mSynchObj;

    /**
     * \brief   The overlapping flag. Set when stack is initialized and cannot be changed anymore.
     **/
    const NECommon::eRingOverlap    mOnOverlap;

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
    inline void _emptyStack( void );

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
    inline void _copyElems(VALUE* dst, VALUE* src, uint32_t srcStart, uint32_t srcEnd, uint32_t srcCount, uint32_t srcCapacity);

    /**
     * \brief   Converts normalized index, such as zero-based index, into the appropriate internal index in the ring stack.
     *          For example, the index 0 is equal to the head position.
     * \param   index   The normalized index to convert.
     * \return  Returns the internal index of the ring buffer and the index cannot be more than the capacity of the ring stack.
     **/
    inline uint32_t _norm2RingIndex(uint32_t index) const;

    /**
     * \brief   Converts the internal ring stack index into the normalized zero-based index.
     *          For example, the head position of the ring is equal to normalized index 0.
     * \param   ring    The internal ring stack index to convert.
     * \return  Returns normalized zero-based index, which cannot be more than the number of elements in the ring stack.
     **/
    inline uint32_t _ring2NormIndex(uint32_t ring) const;

    /**
     * \brief   Compares 2 ring stack entries in the list and returns `Bigger`, `Equal` or `Smaller` depending on comparing results.
     * \param   left            The list of elements in the ring stack on the left side.
     * \param   leftSide        The head position in the left ring stack.
     * \param   leftCapacity    The capacity of the left ring stack.
     * \param   leftCount       The number of entries in the left ring stack.
     * \param   right           The list of elements in the ring stack on the right side.
     * \param   rightSide       The head position in the right ring stack.
     * \param   rightCapacity   The capacity of the right ring stack.
     * \param   rightCount      The number of entries in the right ring stack.
     * \return  Returns one of the values:
     *              1. NEMath::eCompare::Equal  -- if 2 lists are equal.
     *              2. NEMath::eCompare::Bigger -- if `left` stack is bigger than the `right`.
     *              2. NEMath::eCompare::Smaller-- if `left` stack is smaller than the `right`.
     **/
    inline NEMath::eCompare _compareRings(const VALUE* left, uint32_t leftStart, uint32_t leftCapacity, uint32_t leftCount, const VALUE* right, uint32_t rightStart, uint32_t rightCapacity, uint32_t rightCount) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TERingStack( void ) = delete;
    TERingStack( const TERingStack<VALUE> & /*src*/ ) = delete;
    TERingStack( TERingStack<VALUE> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// TELockRingStack<VALUE> class template declaration
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
class TELockRingStack  : public TERingStack<VALUE>
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
    explicit TELockRingStack(uint32_t initCapacity = 0, NECommon::eRingOverlap onOverlap = NECommon::eRingOverlap::StopOnOverlap );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TELockRingStack( const TELockRingStack<VALUE> & source );
    TELockRingStack( const TERingStack<VALUE> & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source to move data.
     **/
    TELockRingStack(TELockRingStack<VALUE>&& source) noexcept;
    TELockRingStack( TERingStack<VALUE> && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TELockRingStack( void ) = default;

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
    inline TELockRingStack<VALUE> & operator = ( const TELockRingStack<VALUE> & source );
    inline TELockRingStack<VALUE> & operator = ( const TERingStack<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from given source.
     *          It as well swaps all entries of source and destination, but
     *          does not change the overlapping type and synchronization object.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    inline TELockRingStack<VALUE>& operator = (TELockRingStack<VALUE> && source) noexcept;
    inline TELockRingStack<VALUE>& operator = (TERingStack<VALUE> && source) noexcept;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are equal.
     **/
    inline bool operator == (const TERingStack<VALUE>& other) const;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are not equal.
     **/
    inline bool operator != (const TERingStack<VALUE>& other) const;

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
// TENolockRingStack<VALUE> class template declaration
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
class TENolockRingStack    : public TERingStack<VALUE>
{
public:
    /**
     * \brief   Initializes non-blocking ring stack, sets initial capacity of 
     *          the stack and sets overlapping flag.
     * \param   initCapacity    The initial capacity size of ring stack.
     * \param   onOverlap       Overlapping flag, used when ring stack is full and 
     *                          pushing new element is required.
     **/
    TENolockRingStack(uint32_t initCapacity = 0, NECommon::eRingOverlap onOverlap = NECommon::eRingOverlap::StopOnOverlap );

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    TENolockRingStack( const TENolockRingStack<VALUE> & source );
    TENolockRingStack( const TERingStack<VALUE> & source );

    /**
     * \brief   Move constructor.
     * \param   source  The source to move data.
     **/
    TENolockRingStack(TENolockRingStack<VALUE> && source ) noexcept;
    TENolockRingStack( TERingStack<VALUE> && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TENolockRingStack( void ) = default;

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
    inline TENolockRingStack<VALUE> & operator = ( const TENolockRingStack<VALUE> & source );
    inline TENolockRingStack<VALUE> & operator = ( const TERingStack<VALUE> & source );

    /**
     * \brief   Moves ring stack entries from given source.
     *          It as well swaps all entries of source and destination, but
     *          does not change the overlapping type and synchronization object.
     * \param   source  The source of ring stack entries to get entries.
     * \return  Returns ring stack object
     **/
    inline TENolockRingStack<VALUE>& operator = (TERingStack<VALUE> && source) noexcept;
    inline TENolockRingStack<VALUE>& operator = (TENolockRingStack<VALUE> && source) noexcept;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are equal.
     **/
    inline bool operator == (const TERingStack<VALUE>& other) const;

    /**
     * \brief   Compares 2 ring stack object and returns true if they are not equal.
     **/
    inline bool operator != (const TERingStack<VALUE>& other) const;

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
// TERingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TERingStack<VALUE>::TERingStack( IEResourceLock & synchObject, uint32_t initCapacity /*= 0*/, NECommon::eRingOverlap onOverlap /*= NECommon::eRingOverlap::StopOnOverlap*/ )
    : mSynchObj ( synchObject )
    , mOnOverlap( onOverlap )
    , mStackList( initCapacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW unsigned char[initCapacity * sizeof(VALUE)]) : nullptr )
    , mElemCount( 0 )
    , mCapacity ( mStackList != nullptr ? initCapacity : 0 )
    , mHeadPos  ( 0 )
    , mTailPos  ( 0 )
{
}

template <typename VALUE>
TERingStack<VALUE>::TERingStack(IEResourceLock& synchObject, const TERingStack<VALUE> & source)
    : mSynchObj (synchObject)
    , mOnOverlap(source.mOnOverlap)
    , mStackList(nullptr)
    , mElemCount(0)
    , mCapacity (0)
    , mHeadPos  (0)
    , mTailPos  (0)
{
    copy(source);
}

template <typename VALUE>
TERingStack<VALUE>::TERingStack(IEResourceLock& synchObject, TERingStack<VALUE> && source) noexcept
    : mSynchObj (synchObject)
    , mOnOverlap(source.mOnOverlap)
    , mStackList(source.mStackList)
    , mElemCount(source.mElemCount)
    , mCapacity (source.mCapacity)
    , mHeadPos  (source.mHeadPos)
    , mTailPos  (source.mTailPos)
{
    source.mStackList   = nullptr;
    source.mCapacity    = 0;
    source.mElemCount   = 0;
    source.mHeadPos     = 0;
    source.mTailPos     = 0;
}

template <typename VALUE>
TERingStack<VALUE>::~TERingStack( void )
{
    _emptyStack();
    delete[] reinterpret_cast<unsigned char*>(mStackList);
    mStackList = nullptr;
    mCapacity = 0;
}

template <typename VALUE>
TERingStack<VALUE> & TERingStack<VALUE>::operator = ( const TERingStack<VALUE> & source )
{
    copy(source);
    return (*this);
}

template <typename VALUE>
TERingStack<VALUE>& TERingStack<VALUE>::operator = (TERingStack<VALUE> && source) noexcept
{
    move(std::move(source));
    return (*this);
}

template <typename VALUE>
bool TERingStack<VALUE>::operator == (const TERingStack<VALUE>& other) const
{
    Lock lock1(mSynchObj);
    Lock lock2(other.mSynchObj);

    bool result{ static_cast<const TERingStack<VALUE> *>(this) == &other };

    if ((static_cast<const TERingStack<VALUE> *>(this) != &other) && (mElemCount == other.mElemCount))
    {
        result = _compareRings(mStackList, mHeadPos, mCapacity, mElemCount, other.mStackList, other.mHeadPos, other.mCapacity, other.mElemCount) == NEMath::eCompare::Equal;
    }

    return result;
}

template <typename VALUE>
bool TERingStack<VALUE>::operator != (const TERingStack<VALUE>& other) const
{
    Lock lock1(mSynchObj);
    Lock lock2(other.mSynchObj);

    bool result{ (static_cast<const TERingStack<VALUE> *>(this) != &other) || (this->mElemCount != other.mElemCount)};

    if ((static_cast<const TERingStack<VALUE> *>(this) != &other) && (this->mElemCount == other.mElemCount))
    {
        result = _compareRings(mStackList, mHeadPos, mCapacity, mElemCount, other.mStackList, other.mHeadPos, other.mCapacity, other.mElemCount) != NEMath::eCompare::Equal;
    }

    return result;
}

template <typename VALUE>
const VALUE& TERingStack<VALUE>::operator [] (uint32_t index) const
{
    return getAt(index);
}

template <typename VALUE>
VALUE& TERingStack<VALUE>::operator [] (uint32_t index)
{
    return getAt(index);
}

template <typename VALUE>
inline uint32_t TERingStack<VALUE>::getSize( void ) const
{
    Lock lock( mSynchObj );
    return mElemCount;
}

template <typename VALUE>
inline bool TERingStack<VALUE>::isEmpty( void ) const
{
    Lock lock( mSynchObj );
    return (mElemCount == 0);
}

template <typename VALUE>
inline NECommon::eRingOverlap TERingStack<VALUE>::getOverlap(void) const
{
    return mOnOverlap;
}

template <typename VALUE>
inline bool TERingStack<VALUE>::lock( void ) const
{
    return mSynchObj.lock(NECommon::WAIT_INFINITE);
}

template <typename VALUE>
inline bool TERingStack<VALUE>::unlock( void ) const
{
    return mSynchObj.unlock();
}

template <typename VALUE>
inline uint32_t TERingStack<VALUE>::capacity( void ) const
{
    Lock lock(mSynchObj);
    return mCapacity;
}

template <typename VALUE>
inline bool TERingStack<VALUE>::isFull( void ) const
{
    Lock lock(mSynchObj);
    return (mOnOverlap != NECommon::eRingOverlap::ResizeOnOvelap) && (mElemCount == mCapacity);
}

template <typename VALUE>
const VALUE& TERingStack<VALUE>::getAt(uint32_t index) const
{
    Lock lock(mSynchObj);
    ASSERT(index < mElemCount);
    index = _norm2RingIndex(index);
    return mStackList[index];
}

template <typename VALUE>
VALUE& TERingStack<VALUE>::getAt(uint32_t index)
{
    Lock lock(mSynchObj);
    ASSERT(index < mElemCount);
    index = _norm2RingIndex(index);
    return mStackList[index];
}

template <typename VALUE>
void TERingStack<VALUE>::setAt(uint32_t index, const VALUE& newValue)
{
    Lock lock(mSynchObj);
    ASSERT(index < mElemCount);
    index = _norm2RingIndex(index);
    mStackList[index] = newValue;
}

template <typename VALUE>
void TERingStack<VALUE>::clear( void )
{
    Lock lock(mSynchObj);
    _emptyStack();
}

template<typename VALUE>
void TERingStack<VALUE>::release(void)
{
    Lock lock(mSynchObj);
    _emptyStack();
    delete[] reinterpret_cast<unsigned char*>(mStackList);
    mStackList = nullptr;
    mCapacity = 0;
}

template<typename VALUE>
void TERingStack<VALUE>::freeExtra(void)
{
    if (mCapacity != mElemCount)
    {
        uint32_t capacity = mElemCount;
        VALUE* newList = capacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW unsigned char[capacity * sizeof(VALUE)]) : nullptr;
        if (newList != nullptr)
        {
            _copyElems(newList, mStackList, mHeadPos, mTailPos, mElemCount, capacity);
            _emptyStack();
            delete[] reinterpret_cast<unsigned char*>(mStackList);
            mStackList  = newList;
            mHeadPos    = 0;
            mTailPos    = capacity - 1;
            mElemCount  = capacity;
            mCapacity   = capacity;
        }
        else if (capacity == 0)
        {
            _emptyStack();
            delete[] reinterpret_cast<unsigned char*>(mStackList);
            mStackList  = nullptr;
            mCapacity   = 0u;
        }
    }
}

template <typename VALUE>
uint32_t TERingStack<VALUE>::push( const VALUE& newElement )
{
    Lock lock(mSynchObj);

    if ( mElemCount < mCapacity )
    {
        ASSERT( (mHeadPos != mTailPos) || (mElemCount <= 1) );
        if (mElemCount == 0)
        {
            VALUE* block = mStackList;
            NEMemory::constructElems<VALUE>(block, 1);
            *block = newElement;
            mHeadPos = 0;
            mTailPos = 0;
            mElemCount = 1u;
        }
        else
        {
            mTailPos = (mTailPos + 1) % mCapacity;
            VALUE* block = mStackList + mTailPos;
            NEMemory::constructElems<VALUE>(block, 1);
            *block = newElement;
            ++mElemCount;
        }
    }
    else
    {
        switch ( mOnOverlap )
        {
        case NECommon::eRingOverlap::ShiftOnOverlap:
            if (mCapacity != 0)
            {
                ASSERT(mStackList != nullptr);
                mTailPos = (mTailPos + 1) % mCapacity;
                mHeadPos = (mHeadPos + 1) % mCapacity;
                VALUE* block = mStackList + mTailPos;
                NEMemory::destroyElems<VALUE>(block, 1);
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
            }
            else
            {
                ASSERT(mStackList == nullptr);
                ASSERT(mElemCount == 0);
            }
            break;

        case NECommon::eRingOverlap::ResizeOnOvelap:
            if ( reserve(static_cast<uint32_t>(mCapacity != 0 ? mCapacity : 1) * 2) >= (mElemCount + 1) )
            {
                ASSERT(mCapacity >= mElemCount + 1);
                mTailPos = (mTailPos + 1) % mCapacity;
                VALUE * block = mStackList + mTailPos;
                NEMemory::constructElems<VALUE>(block, 1);
                *block = newElement;
                ++ mElemCount;
            }
            break;

        case NECommon::eRingOverlap::StopOnOverlap:
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

template <typename VALUE>
VALUE TERingStack<VALUE>::pop( void )
{
    Lock lock(mSynchObj);
    ASSERT( isEmpty() == false );
    VALUE result{ };

    if ( mElemCount != 0 )
    {
        ASSERT( mCapacity != 0 );
        ASSERT( mStackList != nullptr );
        ASSERT((mHeadPos != mTailPos) || (mElemCount == 1u));

        result = mStackList[mHeadPos];
        NEMemory::destroyElems<VALUE>( mStackList + mHeadPos, 1 );
        mHeadPos = (mHeadPos + 1) % mCapacity;
        -- mElemCount;

        if (mElemCount == 0)
        {
            mHeadPos = mTailPos = 0u;
        }
    }

    return result;
}

template <typename VALUE>
uint32_t TERingStack<VALUE>::add( const TERingStack<VALUE> & source )
{
    Lock lock(mSynchObj);
    uint32_t result = mElemCount;
    if (static_cast<const TERingStack<VALUE> *>(this) != &source)
    {
        Lock lock2(source.mSynchObj);
        uint32_t pos = source.mHeadPos;
        for (uint32_t i = 0u; i < source.mElemCount; ++i)
        {
            pushLast(source[i]);
        }
    }

    return (mElemCount - result);
}

template <typename VALUE>
uint32_t TERingStack<VALUE>::reserve(uint32_t newCapacity )
{
    Lock lock(mSynchObj);

    if ( newCapacity > mCapacity )
    {
        VALUE * newList = newCapacity != 0 ? reinterpret_cast<VALUE *>( DEBUG_NEW unsigned char[ newCapacity * sizeof(VALUE)] ) : nullptr;
        if (newList != nullptr)
        {
            uint32_t elemCount = mElemCount;
            _copyElems(newList, mStackList, mHeadPos, mTailPos, elemCount, mCapacity);
            _emptyStack();
            delete[] reinterpret_cast<unsigned char*>(mStackList);
            mStackList = newList;
            mHeadPos   = 0;
            mTailPos   = elemCount - 1;
            mElemCount = elemCount;
            mCapacity  = newCapacity;
        }
    }

    return mCapacity;
}

template <typename VALUE>
void TERingStack<VALUE>::copy(const TERingStack<VALUE>& source)
{
    if (static_cast<const TERingStack<VALUE> *>(this) != &source)
    {
        Lock lock1(mSynchObj);
        Lock lock2(source.mSynchObj);

        _emptyStack();
        VALUE* newList = mStackList;
        uint32_t capacity = source.mCapacity;

        if (mCapacity < capacity) 
        {
            delete[] reinterpret_cast<unsigned char*>(mStackList);
            mStackList = nullptr;
            mCapacity = 0;
            newList = capacity != 0 ? reinterpret_cast<VALUE*>(DEBUG_NEW unsigned char[capacity * sizeof(VALUE)]) : nullptr;
        }
        
        if ((newList != nullptr) && (source.mElemCount != 0))
        {
            _copyElems(newList, source.mStackList, source.mHeadPos, source.mTailPos, source.mElemCount, capacity);
            mStackList  = newList;
            mCapacity   = capacity;
            mElemCount  = source.mElemCount;
            mHeadPos    = 0u;
            mTailPos    = source.mElemCount - 1;
        }
    }
}

template <typename VALUE>
void TERingStack<VALUE>::move(TERingStack<VALUE> && source)
{
    if (static_cast<const TERingStack<VALUE> *>(this) != &source)
    {
        Lock lock1(mSynchObj);
        Lock lock2(source.mSynchObj);

        std::swap(mStackList, source.mStackList);
        std::swap(mElemCount, source.mElemCount);
        std::swap(mCapacity, source.mCapacity);
        std::swap(mHeadPos, source.mHeadPos);
        std::swap(mTailPos, source.mTailPos);
    }
}

template <typename VALUE>
uint32_t TERingStack<VALUE>::find(const VALUE& elem, uint32_t startAt /*= NECommon::RING_START_POSITION*/) const
{
    Lock lock(mSynchObj);

    uint32_t result = NECommon::INVALID_INDEX;
    startAt = startAt == NECommon::RING_START_POSITION ? 0 : startAt;
    for (uint32_t i = 0; i < mElemCount; ++i)
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
bool TERingStack<VALUE>::contains(const VALUE& elem, uint32_t startAt /*= NECommon::RING_START_POSITION*/) const
{
    return (find(elem, startAt) != NECommon::INVALID_INDEX);
}

template <typename VALUE>
inline void TERingStack<VALUE>::_emptyStack( void )
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
inline void TERingStack<VALUE>::_copyElems(VALUE* dst, VALUE* src, uint32_t srcStart, uint32_t srcEnd, uint32_t srcCount, uint32_t srcCapacity)
{
    ASSERT(srcCapacity >= srcCount);
    ASSERT((dst != nullptr) && (src != nullptr));

    NEMemory::constructElems<VALUE>(static_cast<void*>(dst), srcCount);
    if (srcEnd >= srcStart)
    {
        ASSERT((srcEnd - srcStart + 1u) == srcCount);
        NEMemory::copyElems<VALUE>(dst, src + srcStart, srcCount);
    }
    else
    {
        uint32_t elemCopy = srcCapacity - srcStart;
        ASSERT((elemCopy + srcEnd) == srcCount);
        NEMemory::copyElems<VALUE>(dst, src + srcStart, elemCopy);
        NEMemory::copyElems<VALUE>(dst + elemCopy, src, srcEnd);
    }
}

template <typename VALUE>
inline uint32_t TERingStack<VALUE>::_norm2RingIndex(uint32_t index) const
{
    return ((mHeadPos + index) % mCapacity);
}

template <typename VALUE>
inline uint32_t TERingStack<VALUE>::_ring2NormIndex(uint32_t ring) const
{
    return (ring >= mHeadPos ? ring - mHeadPos : (mCapacity - mHeadPos) + ring);
}

template <typename VALUE>
inline NEMath::eCompare TERingStack<VALUE>::_compareRings( const VALUE* left, uint32_t leftStart, uint32_t leftCapacity, uint32_t leftCount
                                                         , const VALUE* right, uint32_t rightStart, uint32_t rightCapacity, uint32_t rightCount) const
{
    ASSERT((leftStart < leftCapacity) && (rightStart < rightCapacity));
    NEMath::eCompare result{ NEMath::eCompare::Equal };
    uint32_t count = MACRO_MIN(leftCount, rightCount);
    while (count != 0)
    {
        result = NEMath::compare<VALUE>(left[leftStart], right[rightStart]);
        if (result != NEMath::eCompare::Equal)
        {
            return result;
        }

        leftStart = (leftStart + 1) % leftCapacity;
        rightStart = (rightStart + 1) % rightCapacity;
        --count;
    }

    return NEMath::compare<uint32_t>(leftCount, rightCount);
}


//////////////////////////////////////////////////////////////////////////
// TELockRingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack(uint32_t initCapacity /*= 0*/, NECommon::eRingOverlap onOverlap /*= NECommon::eRingOverlap::StopOnOverlap*/ )
    : TERingStack<VALUE>    ( mLock, initCapacity, onOverlap )
    , mLock ( false )
{
}

template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack( const TELockRingStack<VALUE> & source )
    : TERingStack<VALUE>    ( mLock, static_cast<const TERingStack<VALUE> &>(source) )
    , mLock ( false )
{
}

template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack( const TERingStack<VALUE> & source )
    : TERingStack<VALUE>    ( mLock, source )
    , mLock ( false )
{
}

template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack( TERingStack<VALUE> && source ) noexcept
    : TERingStack<VALUE>    ( mLock, std::move(source) )
    , mLock ( false )
{
}

template <typename VALUE>
TELockRingStack<VALUE>::TELockRingStack(TELockRingStack<VALUE> && source ) noexcept
    : TERingStack<VALUE>    ( mLock, std::move(static_cast<TERingStack<VALUE> &&>(source)) )
    , mLock ( false )
{
}

template <typename VALUE>
inline TELockRingStack<VALUE> & TELockRingStack<VALUE>::operator = ( const TELockRingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE> &>(*this).operator = (static_cast<const TERingStack<VALUE> &>(source));
    return (*this);
}

template <typename VALUE>
inline TELockRingStack<VALUE> & TELockRingStack<VALUE>::operator = ( const TERingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (static_cast<const TERingStack<VALUE>&>(source));
    return (*this);
}

template <typename VALUE>
inline TELockRingStack<VALUE>& TELockRingStack<VALUE>::operator = (TELockRingStack<VALUE>&& source) noexcept
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (std::move(static_cast<TERingStack<VALUE> &&>(source)));
    return (*this);
}

template <typename VALUE>
inline TELockRingStack<VALUE>& TELockRingStack<VALUE>::operator = (TERingStack<VALUE>&& source) noexcept
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (std::move(static_cast<TERingStack<VALUE>&&>(source)));
    return (*this);
}

template <typename VALUE>
inline bool TELockRingStack<VALUE>::operator == (const TERingStack<VALUE>& other) const
{
    return static_cast<const TERingStack<VALUE>&>(*this).operator == (other);
}

template <typename VALUE>
inline bool TELockRingStack<VALUE>::operator != (const TERingStack<VALUE>& other) const
{
    return static_cast<const TERingStack<VALUE>&>(*this).operator != (other);
}

//////////////////////////////////////////////////////////////////////////
// TENolockRingStack<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack(uint32_t initCapacity /*= 0*/, NECommon::eRingOverlap onOverlap /*= NECommon::eRingOverlap::StopOnOverlap*/ )
    : TERingStack<VALUE>    ( mNoLock, initCapacity, onOverlap )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack( const TERingStack<VALUE> & source )
    : TERingStack<VALUE>    ( mNoLock, source )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack( const TENolockRingStack<VALUE> & source )
    : TERingStack<VALUE>    ( mNoLock, static_cast<const TERingStack<VALUE> &>(source) )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack( TERingStack<VALUE> && source ) noexcept
    : TERingStack<VALUE>    ( mNoLock, std::move(source) )
    , mNoLock   ( )
{
}

template <typename VALUE>
TENolockRingStack<VALUE>::TENolockRingStack(TENolockRingStack<VALUE> && source ) noexcept
    : TERingStack<VALUE>    ( mNoLock, std::move(static_cast<TERingStack<VALUE> &&>(source)) )
    , mNoLock   ( )
{
}

template <typename VALUE>
inline TENolockRingStack<VALUE> & TENolockRingStack<VALUE>::operator = ( const TENolockRingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE> &>(*this).operator = (static_cast<const TERingStack<VALUE>&>(source));
    return (*this);
}

template <typename VALUE>
inline TENolockRingStack<VALUE> & TENolockRingStack<VALUE>::operator = ( const TERingStack<VALUE> & source )
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (static_cast<const TERingStack<VALUE>&>(source));
    return (*this);
}

template <typename VALUE>
inline TENolockRingStack<VALUE>& TENolockRingStack<VALUE>::operator = (TENolockRingStack<VALUE>&& source) noexcept
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (std::move(static_cast<TERingStack<VALUE> &&>(source)));
    return (*this);
}

template <typename VALUE>
inline TENolockRingStack<VALUE>& TENolockRingStack<VALUE>::operator = (TERingStack<VALUE>&& source) noexcept
{
    static_cast<TERingStack<VALUE>&>(*this).operator = (std::move(static_cast<TERingStack<VALUE>&&>(source)));
    return (*this);
}

template <typename VALUE>
inline bool TENolockRingStack<VALUE>::operator == (const TERingStack<VALUE>& other) const
{
    return static_cast<const TERingStack<VALUE>&>(*this).operator == (other);
}

template <typename VALUE>
inline bool TENolockRingStack<VALUE>::operator != (const TERingStack<VALUE>& other) const
{
    return static_cast<const TERingStack<VALUE>&>(*this).operator!=(other);
}

//////////////////////////////////////////////////////////////////////////
// TERingStack<VALUE> class template friend operators
//////////////////////////////////////////////////////////////////////////

template <typename V>
const IEInStream & operator >> ( const IEInStream & stream, TERingStack<V> & input )
{
    Lock lock(input.mSynchObj);

    uint32_t size = 0;
    stream >> size;

    input.clear();
    input.reserve(static_cast<uint32_t>(size));

    for (uint32_t i = 0; i < size; i ++)
    {
        V newElement;
        stream >> newElement;
        static_cast<void>(input.pushLast(newElement));
    }

    return stream;
}

template <typename V>
IEOutStream & operator << ( IEOutStream & stream, const TERingStack<V> & output )
{
    Lock lock(output.mSynchObj);

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

#endif  // AREG_BASE_TERINGSTACK_HPP
