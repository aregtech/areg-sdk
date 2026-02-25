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
#include "areg/base/GEGlobal.h"
#include "areg/base/TemplateBase.hpp"
#include <algorithm>
#include <deque>

#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/IOStream.hpp"

#include <algorithm>
#include <utility>

namespace areg
{

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
     * \brief   Simple FIFO Stack base object to queue elements, insert and
     *          access by push and pop operations. StackBase requires instance
     *          of resource lock object to synchronize access of stack elements.
     *          Whether the Stack is thread safe or not, depends on type of
     *          synchronization object passed in stack.
     *
     * \tparam  VALUE   The type of stored items. Either should be primitive
     *                  or should have default constructor and valid assigning
     *                  operator. And should be possible to convert to type const VALUE &.
     **/
    template <typename VALUE>
    class StackBase   : private areg::Constless<std::deque<VALUE>>
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
         * \brief   Stack initialization constructor.
         *          Receives reference to synchronization object.
         *          It is declared as protected to prevent direct call.
         *          Use Stack or ConcurrentStack objects instead.
         * \param   syncObject  Reference to synchronization object.
         **/
        explicit StackBase( Lockable & syncObject );

        /**
         * \brief   Initializes the resource lock object and copies elements from given source.
         * \param   syncObject  The instance of synchronization object
         * \param   source      The Stack source, which contains elements to copy.
         **/
        StackBase( Lockable & syncObject, const StackBase<VALUE> & source );

        /**
         * \brief   Initializes the resource lock object and move elements from given source.
         * \param   syncObject  The instance of synchronization object
         * \param   source      The Stack source, which contains elements to move.
         **/
        StackBase( Lockable & syncObject, StackBase<VALUE> && source ) noexcept;

        /**
         * \brief   Compiles entries from the given array of objects.
         * \param   syncObject  The instance of synchronization object
         * \param   list        The list of entries to copy.
         * \param   count       The number of entries in the array.
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
         * \brief   Assigning operator. It is copying stack elements from given source
         *          in the same order as they are created in the source stack.
         *          If stack had entries, all entries will be lost and new entries will be created.
         * \param   source  The instance of source to get stack entries.
         * \return  Returns stack object.
         **/
        inline StackBase<VALUE> & operator = ( const StackBase<VALUE> & source );

        /**
         * \brief   Assigning operator. It is moves stack elements from given source
         *          in the same order as they are created in the source stack.
         *          If stack had entries, all entries will be lost and new entries will be created.
         * \param   source  The instance of source to get stack entries.
         * \return  Returns stack object.
         **/
        inline StackBase<VALUE> & operator = ( StackBase<VALUE> && source ) noexcept;

        /**
         * \brief   Returns element at specified position.
         */
        inline const VALUE& operator [] (STACKPOS atPosition) const;
        inline VALUE& operator [] (STACKPOS atPosition);

        /**
         * \brief   Compares 2 stack object and returns true if they are equal.
         * \param   other   A stack object to compare.
         * \return  Returns true if 2 stack objects are equal.
         **/
        inline bool operator == (const StackBase<VALUE> & other) const;

        /**
         * \brief   Compares 2 stack object and returns true if they are not equal.
         * \param   other   A stack object to compare.
         * \return  Returns true if 2 stack objects are not equal.
         **/
        inline bool operator != (const StackBase<VALUE> & other) const;

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
        template<typename V>
        friend const InStream & operator >> (const InStream & stream, StackBase<V> & input);
        /**
         * \brief   Writes to the stream Stack values.
         *          The values will be written to the stream starting from head position.
         *          There should be possibility to stream every value of Stack and if VALUE
         *          is not a primitive, but an object, it should have implemented streaming operator.
         * \param   stream  The streaming object to write values
         * \param   output  The Stack object to read out values.
         **/
        template<typename V>
        friend OutStream & operator << (OutStream & stream, const StackBase<V> & output);

    //////////////////////////////////////////////////////////////////////////
    // Operations and Attributes
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Returns number of elements saved in stack.
         **/
        inline uint32_t getSize() const;

        /**
         * \brief   Returns true if Ring Stack is empty
         **/
        inline bool isEmpty() const;

        /**
         * \brief   Returns true if specified position pointing start of the stack.
         * \param   pos     The position to check.
         * \return  Returns true if specified position pointing start of the stack.
         **/
        inline bool isFirstPosition(const STACKPOS pos) const;

        /**
         * \brief   Returns true if specified position pointing start of the stack.
         * \param   pos     The position to check.
         * \return  Returns true if specified position pointing start of the stack.
         **/
        inline bool isLastPosition(const STACKPOS pos) const;

        /**
         * \brief   Returns the invalid position of the stack.
         **/
        STACKPOS invalidPosition() const;

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

        /**
         * \brief	Checks whether given element exist in the stack or not. The elements of type
         *          VALUE should have comparing operators.
         * \param	elemSearch  The element to search.
         * \return	Returns true if could find element starting at given position.
         **/
        inline bool contains(const VALUE& elemSearch) const;

        /**
         * \brief	Checks whether given element exist in the stack or not. The elements of type
         *          VALUE should have comparing operators.
         * \param	elemSearch  The element to search.
         * \param	startAt	    The position to start searching.
         * \return	Returns true if could find element starting at given position.
         **/
        inline bool contains(const VALUE& elemSearch, STACKPOS startAt) const;

        /**
         * \brief   Returns the vector object where the data are stored.
         **/
        inline const std::deque<VALUE>& getData() const;

    /************************************************************************/
    // Operations
    /************************************************************************/

        /**
         * \brief   Removes all elements from stack and makes it empty.
         **/
        inline void clear();

        /**
         * \brief   Delete extra entries in array.
         **/
        inline void freeExtra();

        /**
         * \brief   Sets the size of array to zero and deletes all unused capacity of the string.
         */
        inline void release();

        /**
         * \brief   Locks stack that methods can be accessed only from locking thread.
         *          In case if NolockSyncObject is used, no locking will happen,
         *          the function will return immediately and thread will continue to run.
         * \return  Returns true if stack successfully locked
         **/
        inline bool lock() const;

        /**
         * \brief   If stack previously was locked by thread, it will unlock stack
         *          In case if NolockSyncObject is used, nothing will happen.
         * \return  Returns true if stack successfully unlocked
         **/
        inline bool unlock() const;

        /**
         * \brief	Sets new size of stack. If needed, either increases or truncates
         *          elements in the stack. The elements of type VALUE should have default
         *          constructor create and initialize elements.
         * \param	newSize	    New size to set. If zero, stack is emptied.
         **/
        inline void resize(uint32_t newSize);

        /**
         * \brief   Returns first inserted element in the stack without changing stack.
         *          The caller of function should make sure that the stack is not empty,
         *          otherwise it may cause system crash.
         *          Check the size of stack before calling function.
         **/
        inline const VALUE & firstEntry() const;

        /**
         * \brief   Returns first inserted element in the stack without changing stack.
         *          The caller of function should make sure that the stack is not empty,
         *          otherwise it may cause system crash.
         *          Check the size of stack before calling function.
         **/
        inline const VALUE & lastEntry() const;

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
        inline VALUE popFirst();

        /**
         * \brief   Copies all elements from given source and returns the number of copied elements.
         *          If Stack previously had elements, they will be lost.
         * \param   source  The stack source to copy elements.
         * \return  Returns the number of copied elements.
         **/
        inline uint32_t copy( const StackBase<VALUE> & source );

        /**
         * \brief   Moves all elements from given source and returns the number of moved elements.
         *          If Stack previously had elements, they will be lost.
         * \param   source  The stack source to copy elements.
         * \return  Returns the number of copied elements.
         **/
        inline uint32_t move( StackBase<VALUE> && source ) noexcept;

        /**
         * \brief   Searches element in the stack starting at given position.
         *          The given position should be valid or equal to areg::START_POSITION
         *          to search at the beginning of stack.
         * \param   Value       The element value to search in the stack.
         * \return  If found element, returns valid position. Otherwise, it returns invalid position.
         **/
        inline STACKPOS find(const VALUE & Value) const;

        /**
         * \brief   Searches element in the stack starting at given position.
         *          The given position should be valid or equal to areg::START_POSITION
         *          to search at the beginning of stack.
         * \param   Value       The element value to search in the stack.
         * \param	searchAfter	If valid position, the searching starts from next element specified by position.
         *                      If invalid position, the searching starts from the beginning of the stack.
         * \return  If found element, returns valid position. Otherwise, it returns invalid position.
         **/
        inline STACKPOS find(const VALUE& Value, STACKPOS searchAfter) const;

        /**
         * \brief   Returns the first (head) position of the stack. Returns nullptr if empty.
         **/
        inline STACKPOS firstPosition() const;

        /**
         * \brief   Return value at given position. The position should be valid.
         * \param   pos     The position to get the value.
         * \return  Returns value at given position.
         **/
        inline const VALUE & getAt( const STACKPOS pos ) const;
        inline VALUE& getAt(STACKPOS pos);


        /**
         * \brief   Returns value of element at the give position.
         * \param   atPosition  The valid position in stack
         **/
        inline const VALUE& valueAtPosition( const STACKPOS atPosition ) const;
        inline VALUE& valueAtPosition( STACKPOS atPosition );

        /**
         * \brief   Returns next valid position after given position value.
         *          Returns nullptr if given position was last.
         * \param   pos     The position value to get next.
         * \return  If given position is not last in the stack, it returns next valid position value.
         *          Otherwise, it returns nullptr.
         **/
        inline STACKPOS nextPosition( STACKPOS pos ) const;

        /**
         * \brief   Sorts the stack, compares the elements by given Compare functionality.
         * \param   comp    The comparing method, similar to the method  std::greater()
         * \return  Sorts and returns the Stack object.
         **/
        template <class Compare>
        inline StackBase< VALUE >& sort(Compare comp);

        /**
         * \brief   Copies elements from the stack into the provided pre-allocated buffer.
         *          If `elemCount` is less than the number of elements in the stack,
         *          only the first `elemCount` elements are copied. Otherwise, all elements
         *          in the stack are copied. No elements are copied if `elemCount` is 0.
         * \param   list [in, out]  A pre-allocated buffer where the stack elements will be copied.
         *                          Must be large enough to hold at least `elemCount` elements.
         * \param   elemCount [in]  The maximum number of elements to copy into the `list` buffer.
         *                          If set to 0, no elements are copied.
         * \return  The number of elements successfully copied into the `list` buffer.
         **/
        inline uint32_t getElements(VALUE* list, uint32_t elemCount);

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
     * \brief   Thread safe FIFO stack class template declaration, where accessing
     *          data is synchronized by resource lock. Use this object if elements
     *          of stack are accessed by more than one thread.
     **/
    template <typename VALUE>
    class ConcurrentStack  : public StackBase<VALUE>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor
         **/
        ConcurrentStack();

        /**
         * \brief   Copies data from given source.
         * \param   source  The source to copy data.
         **/
        explicit ConcurrentStack( const StackBase<VALUE> & source );

        /**
         * \brief   Moves data from given source.
         * \param   source  The source to move data.
         **/
        explicit ConcurrentStack( StackBase<VALUE> && source ) noexcept;

        /**
         * \brief   Copies data from given source.
         * \param   source  The source to copy data.
         **/
        ConcurrentStack( const ConcurrentStack<VALUE> & source );

        /**
         * \brief   Moves data from given source.
         * \param   source  The source to move data.
         **/
        ConcurrentStack( ConcurrentStack<VALUE> && source ) noexcept;

        /**
         * \brief   Compiles entries from the given array of objects.
         * \param   list    The list of entries to copy.
         * \param   count   The number of entries in the array.
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
         * \brief   Copies entries from given sources. If stack had entries
         *          all entries will be lost and new entries will be created.
         * \param   source  The instance of source to copy stack entries.
         * \return  Returns stack object.
         **/
        inline ConcurrentStack<VALUE> & operator = ( const ConcurrentStack<VALUE> & source );

        /**
         * \brief   Moves entries from given sources. If stack had entries
         *          all entries will be lost and new entries will be created.
         * \param   source  The instance of source to move stack entries.
         * \return  Returns stack object.
         **/
        inline ConcurrentStack<VALUE> & operator = ( ConcurrentStack<VALUE> && source ) noexcept;

        /**
         * \brief   Copies entries from given sources. If stack had entries
         *          all entries will be lost and new entries will be created.
         * \param   source  The instance of source to copy stack entries.
         * \return  Returns stack object.
         **/
        inline ConcurrentStack<VALUE> & operator = ( const StackBase<VALUE> & source );

        /**
         * \brief   Moves entries from given sources. If stack had entries
         *          all entries will be lost and new entries will be created.
         * \param   source  The instance of source to move stack entries.
         * \return  Returns stack object.
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
     * \brief   No thread safe FIFO stack class template declaration, where accessing
     *          data is not synchronized. Use this object if elements of stack
     *          are accessed only by one thread.
     **/
    template <typename VALUE>
    class Stack    : public StackBase<VALUE>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor
         **/
        Stack();

        /**
         * \brief   Copies data from given source.
         * \param   source  The source to copy data.
         **/
        explicit Stack( const StackBase<VALUE> & source );

        /**
         * \brief   Moves data from given source.
         * \param   source  The source to move data.
         **/
        explicit Stack( StackBase<VALUE> && source ) noexcept;

        /**
         * \brief   Copies data from given source.
         * \param   source  The source to copy data.
         **/
        Stack( const Stack<VALUE> & source );

        /**
         * \brief   Moves data from given source.
         * \param   source  The source to move data.
         **/
        Stack( Stack<VALUE> && source ) noexcept;

        /**
         * \brief   Compiles entries from the given array of objects.
         * \param   list    The list of entries to copy.
         * \param   count   The number of entries in the array.
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
         * \brief   Copies entries from given sources. If stack had entries
         *          all entries will be lost and new entries will be created.
         * \param   source  The instance of source to copy stack entries.
         * \return  Returns stack object.
         **/
        inline Stack<VALUE> & operator = ( const Stack<VALUE> & source );

        /**
         * \brief   Moves entries from given sources. If stack had entries
         *          all entries will be lost and new entries will be created.
         * \param   source  The instance of source to move stack entries.
         * \return  Returns stack object.
         **/
        inline Stack<VALUE> & operator = ( Stack<VALUE> && source ) noexcept;

        /**
         * \brief   Copies entries from given sources. If stack had entries
         *          all entries will be lost and new entries will be created.
         * \param   source  The instance of source to copy stack entries.
         * \return  Returns stack object.
         **/
        inline Stack<VALUE> & operator = ( const StackBase<VALUE> & source );

        /**
         * \brief   Moves entries from given sources. If stack had entries
         *          all entries will be lost and new entries will be created.
         * \param   source  The instance of source to move stack entries.
         * \return  Returns stack object.
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
        : areg::Constless<std::deque<VALUE>>( )
        , mValueList    ( )
        , mSyncObject   ( syncObject )
    {
    }

    template <typename VALUE>
    StackBase<VALUE>::StackBase( Lockable & syncObject, const StackBase<VALUE> & source )
        : areg::Constless<std::deque<VALUE>>( )
        , mValueList    ( )
        , mSyncObject   ( syncObject )
    {
        Lock lock(source.mSyncObject);
        mValueList = source.mValueList;
    }

    template <typename VALUE>
    StackBase<VALUE>::StackBase( Lockable & syncObject, StackBase<VALUE> && source ) noexcept
        : areg::Constless<std::deque<VALUE>>( )
        , mValueList    ( )
        , mSyncObject   ( syncObject )
    {
        Lock lock(source.mSyncObject);
        mValueList = std::move(source.mValueList);
    }

    template<typename VALUE>
    StackBase<VALUE>::StackBase(Lockable& syncObject, const VALUE* list, uint32_t count)
        : areg::Constless<std::deque<VALUE>>()
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
    inline uint32_t StackBase<VALUE>::getSize() const
    {
        Lock lock( mSyncObject );
        return static_cast<uint32_t>(mValueList.size());
    }

    template <typename VALUE>
    inline bool StackBase<VALUE>::isEmpty() const
    {
        Lock lock( mSyncObject );
        return mValueList.empty();
    }

    template <typename VALUE>
    inline bool StackBase<VALUE>::isFirstPosition(STACKPOS pos) const
    {
        Lock lock(mSyncObject);
        return (pos == mValueList.begin());
    }

    template <typename VALUE>
    inline bool StackBase<VALUE>::isLastPosition(STACKPOS pos) const
    {
        Lock lock(mSyncObject);
        return (mValueList.empty() == false) && (pos == --mValueList.end());
    }

    template <typename VALUE>
    inline typename StackBase<VALUE>::STACKPOS StackBase<VALUE>::invalidPosition() const
    {
        Lock lock(mSyncObject);
        auto end = mValueList.end();
        return areg::Constless<std::deque<VALUE>>::iter(mValueList, end);
    }

    template <typename VALUE>
    inline bool StackBase<VALUE>::isValidPosition(STACKPOS pos) const
    {
        Lock lock(mSyncObject);
        return (pos != mValueList.end());
    }

    template <typename VALUE>
    inline bool StackBase<VALUE>::isInvalidPosition(STACKPOS pos) const
    {
        Lock lock(mSyncObject);
        return (pos == mValueList.end());
    }

    template <typename VALUE>
    inline bool StackBase<VALUE>::checkPosition(STACKPOS pos) const
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
        return contains(elemSearch, firstPosition());
    }

    template<typename VALUE>
    inline bool StackBase<VALUE>::contains(const VALUE& elemSearch, STACKPOS startAt) const
    {
        return (startAt != mValueList.end() ? std::find(startAt, invalidPosition(), elemSearch) != mValueList.end() : false);
    }

    template<typename VALUE>
    inline const std::deque<VALUE>& StackBase<VALUE>::getData() const
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
    inline void StackBase<VALUE>::freeExtra()
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
    inline const VALUE & StackBase<VALUE>::firstEntry() const
    {
        Lock lock(mSyncObject);
        return mValueList.front();
    }

    template <typename VALUE>
    inline const VALUE & StackBase<VALUE>::lastEntry() const
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
    inline uint32_t StackBase<VALUE>::pushLast( const VALUE & newElement )
    {
        Lock lock(mSyncObject);
        mValueList.push_back(newElement);
        return static_cast<uint32_t>(mValueList.size());
    }

    template <typename VALUE>
    inline uint32_t StackBase<VALUE>::pushLast(VALUE && newElement)
    {
        Lock lock(mSyncObject);
        mValueList.push_back(std::move(newElement));
        return static_cast<uint32_t>(mValueList.size());
    }

    template <typename VALUE>
    inline uint32_t StackBase<VALUE>::pushFirst( const VALUE & newElement )
    {
        Lock lock(mSyncObject);
        mValueList.push_front(newElement);
        return static_cast<uint32_t>(mValueList.size());
    }

    template <typename VALUE>
    inline uint32_t StackBase<VALUE>::pushFirst(VALUE && newElement)
    {
        Lock lock(mSyncObject);
        mValueList.push_front(std::move(newElement));
        return static_cast<uint32_t>(mValueList.size());
    }

    template <typename VALUE>
    VALUE StackBase<VALUE>::popFirst()
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
        return areg::Constless<std::deque<VALUE>>::iter(mValueList, it);
    }

    template <typename VALUE>
    inline typename StackBase<VALUE>::STACKPOS StackBase<VALUE>::find(const VALUE & Value, STACKPOS searchAfter ) const
    {
        Lock lock(mSyncObject);
        STACKPOS end = invalidPosition();
        return (searchAfter != end ? std::find(++searchAfter, end, Value) : end);
    }

    template <typename VALUE>
    inline typename StackBase<VALUE>::STACKPOS StackBase<VALUE>::firstPosition() const
    {
        Lock lock(mSyncObject);
        auto it = mValueList.begin();
        return areg::Constless<std::deque<VALUE>>::iter(mValueList, it);
    }

    template <typename VALUE>
    inline const VALUE & StackBase<VALUE>::getAt( const STACKPOS pos ) const
    {
        Lock lock(mSyncObject);

        ASSERT(pos != mValueList.end());
        return (*pos);
    }

    template <typename VALUE>
    inline VALUE & StackBase<VALUE>::getAt( STACKPOS pos )
    {
        Lock lock(mSyncObject);

        ASSERT(pos != mValueList.end());
        return (*pos);
    }

    template <typename VALUE>
    inline const VALUE & StackBase<VALUE>::valueAtPosition( const STACKPOS atPosition ) const
    {
        Lock lock( mSyncObject );

        ASSERT( atPosition != mValueList.end( ) );
        return (*atPosition);
    }

    template <typename VALUE>
    inline VALUE & StackBase<VALUE>::valueAtPosition( STACKPOS atPosition )
    {
        Lock lock( mSyncObject );

        ASSERT( atPosition != mValueList.end( ) );
        return (*atPosition);
    }

    template <typename VALUE>
    inline typename StackBase<VALUE>::STACKPOS StackBase<VALUE>::nextPosition( STACKPOS pos ) const
    {
        Lock lock(mSyncObject);

        ASSERT(pos != mValueList.end());
        return (++pos);
    }

    template<typename VALUE>
    inline uint32_t StackBase<VALUE>::getElements(VALUE* list, uint32_t elemCount)
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
    const InStream & operator >> ( const InStream & stream, StackBase<V> & input )
    {
        Lock lock(input.mSyncObject);

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

    template<typename V>
    OutStream & operator << ( OutStream & stream, const StackBase<V> & output )
    {
        Lock lock(output.mSyncObject);

        uint32_t size = output.getSize();
        stream << size;

        for (const auto& value : output.mValueList)
        {
            stream << value;
        }

        return stream;
    }

} // namespace areg

#endif  // AREG_BASE_STACK_HPP
