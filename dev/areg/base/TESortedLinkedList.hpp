#ifndef AREG_BASE_TESORTEDLINKEDLIST_HPP
#define AREG_BASE_TESORTEDLINKEDLIST_HPP
/************************************************************************
 * \file        areg/base/TESortedLinkedList.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Sorted Linked List class template.
 *              The linked list contains list of values that can be
 *              accessed either by Position or by valid index.
 *              The size of Linked List is dynamic, Elements contain
 *              information of previous and next Block.
 *              When element is inserted, the class guaranties, that values
 *              are sorted either acceding or descending.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/ETemplateBase.hpp"
#include "areg/base/IEIOStream.hpp"

//////////////////////////////////////////////////////////////////////////
// TESortedLinkedList<VALUE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Sorted Linked List class template declaration. Derives private 
 *              class TemplateConstants to access defined constants.
 *              Accessing, Insert and Remove by Position operations
 *              are fast. Searching element and accessing by index
 *              are slower. Every inserted element in the class is sorted
 *              either acceding or descending by values.
 *              Blocks in Linked List are linked bi-directional, i.e.
 *              every Block contains information of next and previous
 *              Block. Sorted Linked List does not guaranty unique values,
 *              but it gives possibility to search element after
 *              certain Position. The type VALUE in Sorted Linked List 
 *              should have at least default constructor, comparison operators
 *              like operator >, operator < and operator ==, and
 *              valid assigning operator. By default, VALUE_TYPE
 *              and VALUE are equal.
 *              The Sorted Linked List does not allow modification of element
 *              by position and it does not give possibility to insert Head
 *              or Tail position. The developer should be careful when saving
 *              index of element in the list and inserts new element. Because 
 *              when new element is inserted, the object will sort and the previously
 *              saved index might indicate another element or be invalid.
 *              Accessing elements by position and index are read-only.
 *              The Sorted LinkedList object is not thread safe and data access 
 *              should be  synchronized manually.
 *
 * \tparam  VALUE       the type of stored items. Either should be 
 *                      primitive or should have default constructor 
 *                      and valid assigning operator. Also, should be 
 *                      possible to convert to type VALUE_TYPE.
 * \tparam  VALUE_TYPE  By default same as VALUE, but can be any other
 *                      type, which is converted from type VALUE.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TESortImpl<VALUE_TYPE>> 
class TESortedLinkedList    : protected Implement
                            , private TemplateConstants
{
//////////////////////////////////////////////////////////////////////////
// Internal objects and types declaration
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Hidden / Protected class declaration.
     *          The Block class contains information
     *          of element in linked list -- value, pointers to next and previous
     *          Block objects.
     **/
    //////////////////////////////////////////////////////////////////////////
    // TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block class declaration
    //////////////////////////////////////////////////////////////////////////
    class Block
    {
    //////////////////////////////////////////////////////////////////////////
    // constructor / destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor
         **/
        inline Block( void );
        /**
         * \brief   Initialization constructor.
         **/
        inline Block(VALUE_TYPE value);
        /**
         * \brief   Destructor.
         **/
        inline ~Block( void );
    //////////////////////////////////////////////////////////////////////////
    // member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Pointer to next element Block. The next element of 'tail' is always NULL
         **/
        Block*    mNext;
        /**
         * \brief   Pointer to previous element Block object. The previous element of 'head' is always NULL
         **/
        Block*    mPrev;
        /**
         * \brief   Container of value.
         **/
        VALUE       mValue;
    };

//////////////////////////////////////////////////////////////////////////
// Constants and types
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Sorting criteria of linked list
     **/
    typedef enum E_Sort
    {
          SortDescending    //!< Sort descending
        , SortAcceding      //!< Sort acceding
    } eSort;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty linked list
     **/
    TESortedLinkedList( bool sortAcceding = true );
    
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TESortedLinkedList( const TESortedLinkedList<VALUE, VALUE_TYPE, Implement> & src );
    
    /**
     * \brief   Destructor
     **/
    ~TESortedLinkedList( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief	Assigning operator, assigns elements of src to the Linked List
     * \param	src	    Source Linked List to get elements.
     **/
    const TESortedLinkedList<VALUE, VALUE_TYPE, Implement> & operator = ( const TESortedLinkedList<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief	Comparing operator, compares elements of otherList
     * \param	otherList	Linked List instance to compare elements
     **/
    bool operator == ( const TESortedLinkedList<VALUE, VALUE_TYPE, Implement> & otherList ) const;

    /**
     * \brief   Subscript operator. Returns reference to value of element 
     *          by given valid index. The index should be valid, otherwise 
     *          assertion raised. May be used on either the right (r-value)
     *          or the left (l-value) of an assignment statement.
     * \param   atIndex     The valid zero-based index.
     * \return  Returns Value of Element at given index, the value can be modified
     **/
    inline VALUE_TYPE operator [] ( int atIndex ) const;
    /**
     * \brief   Subscript operator. Returns reference to value of element 
     *          by given valid Position. May be used on either the right 
     *          (r-value) or the left (l-value) of an assignment statement.
     * \param   atPosition  The valid Linked List position.
     * \return  Returns Value of Element at given Linked List position, the value can be modified
     **/
    inline VALUE_TYPE operator [] ( LISTPOS atPosition ) const;

/************************************************************************/
// Friend global operators to make Linked List streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream Linked List values.
     *          If Linked List previously had values, they will be lost.
     *          The values in the Linked List will be initialized in the same sequence
     *          as they were written.
     *          There should be possibility to initialize values from streaming object and
     *          if VALUE is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object for reading values
     * \param   input   The Linked List object to save initialized values.
     **/
    template <typename V, typename VT, class Impl>
    friend const IEInStream & operator >> ( const IEInStream & stream, TESortedLinkedList<V, VT, Impl> & input );
    /**
     * \brief   Writes to the stream Linked List values.
     *          The values will be written to the stream starting from head position.
     *          There should be possibility to stream every value of Linked List and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Linked List object to read out values.
     **/
    template <typename V, typename VT, class Impl>
    friend IEOutStream & operator << ( IEOutStream & stream, const TESortedLinkedList<V, VT, Impl> & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Attributes
/************************************************************************/

    /**
     * \brief   Returns true, if Linked List container is empty.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns the size of Linked List container
     **/
    inline int getSize( void ) const;

    /**
     * \brief   Returns true if the list is sorting acceding
     **/
    inline bool isAcceding( void ) const;

    /**
    * \brief   Returns true if the list is sorting descending
    **/
    inline bool isDescending( void ) const;

/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief   Returns position of head element in Linked List container.
     *          If Linked List is empty, function returns NULL (INVALID_POSITION)
     **/
    inline LISTPOS firstPosition( void ) const;

    /**
     * \brief   Returns position of tail element in Linked List container
     *          If Linked List is empty, function returns NULL (INVALID_POSITION)
     **/
    inline LISTPOS lastPosition( void ) const;

    /**
     * \brief   Returns value of head element in Linked List container.
     *          On call the Linked List should not be empty, otherwise assertion is raised.
     **/
    inline VALUE_TYPE firstEntry( void ) const;

    /**
     * \brief   Returns value of tail element in Linked List container.
     *          On call the Linked List should not be empty, otherwise assertion is raised.
     **/
    inline VALUE_TYPE lastEntry( void ) const;

    /**
     * \brief	Returns given position value and on exit position of next element in Linked List container.
     *          On input, the value of 'in_out_NextPosition' should be valid (for example, should be equal to head position).
     *          Otherwise assertion is raised.
     * \param	in_out_NextPosition On input, this should be valid position of element in Linked List container.
     *                              On output, this contains position of next element in Linked List container
     *                              or NULL (INVALID_POSITION) if passed position on input is position of tail element
     * \return	Returns value of element at the given position.
     **/
    inline VALUE_TYPE getNext( LISTPOS & in_out_NextPosition ) const;

    /**
     * \brief	Returns position of next element in Linked List
     * \param	atPosition	Actual position to get the next position
     * \return	If element at given position has next element, returns position of next element in Linked List.
     *          Otherwise it returns NULL
     **/
    inline LISTPOS nextPosition( LISTPOS atPosition ) const;

    /**
     * \brief	Returns given position value and on exit position of previous element in Linked List container.
     *          On input, the value of 'in_out_PrevPosition' should be valid (for example, should be equal to tail position).
     *          Otherwise assertion is raised.
     * \param	in_out_PrevPosition On input, this should be valid position of element in Linked List container.
     *                              On output, this contains position of previous element in Linked List container
     *                              or NULL (INVALID_POSITION) if passed position on input is position of head element
     * \return	Returns value of element at the given position.
     **/
    inline VALUE_TYPE getPrev( LISTPOS & in_out_PrevPosition ) const;

    /**
     * \brief	Returns position of previous element in Linked List
     * \param	atPosition	Actual position to get the previous position
     * \return	If element at given position has previous element, returns position of previous element in Linked List.
     *          Otherwise it returns NULL
     **/
    inline LISTPOS prevPosition( LISTPOS atPosition ) const;

    /**
     * \brief   Returns value of element at the give position.
     *          On input, the value of atPosition should be valid. 
     *          Returned value should not be modified.
     * \param   atPosition  The valid position in Linked List
     * \return  Returns Value at given position. 
     *          The returned value should not be modified
     **/
    inline VALUE_TYPE getAt( LISTPOS atPosition ) const;

    /**
     * \brief	Returns the value of element by give index. The index of head element is 0
     *          The index must not be more than GetSize() - 1.
     *          If invalid index is passed, function fails with assertion.
     * \param	index	The index of element to get value
     * \return	The value of element by give index.
     **/
    inline VALUE_TYPE getAt( int index ) const;

    /**
     * \brief	On exit, returns position and value of next element in Linked List container.
     *          On input, the value of 'in_out_NextPosition' should be valid (for example, should be equal to head position).
     *          Otherwise assertion is raised.
     * \param	in_out_NextPosition	On input, this should be valid position of element in Linked List container.
     *                              On output, this contains position of next element in Linked List container 
     *                              or NULL (INVALID_POSITION) if passed position on input is position of tail element
     * \param   out_NextValue       If Linked List has next element, this contains value of next element.
     *                              Otherwise, the value is unchanged. Check function return value before using parameter
     * \return	Returns true if Linked List has next element. Before using out_value function return value
     *          should be validated.
     **/
    bool nextEntry( LISTPOS & in_out_NextPosition, VALUE & out_NextValue ) const;

    /**
     * \brief	On exit, returns position and value of previous element in Linked List container.
     *          On input, the value of 'in_out_PrevPosition' should be valid (for example, should be equal to tail position).
     *          Otherwise assertion is raised.
     * \param	in_out_PrevPosition	On input, this should be valid position of element in Linked List container.
     *                              On output, this contains position of previous element in Linked List container
     *                              or NULL (INVALID_POSITION) if passed position on input is position of head element
     * \param   out_PrevValue       If Linked List has previous element, this contains value of previous element.
     *                              Otherwise, the value is unchanged. Check function return value before using parameter
     *                              value of this parameter.
     * \return	Returns true if Linked List has previous element. Before using out_value function return value
     *          should be validated.
     **/
    bool prevEntry( LISTPOS & in_out_PrevPosition, VALUE & out_PrevValue ) const;

    /**
     * \brief   Removes head element from Linked List and returns value or removed element.
     *          On call the Linked List should not be empty. Otherwise assertion is raised.
     * \return  Returns value of removed element.
     **/
    VALUE removeFirst( void );

    /**
     * \brief   Removes tail element from Linked List and returns value or removed element.
     *          On call the Linked List should not be empty. Otherwise assertion is raised.
     * \return  Returns value of removed element.
     **/
    VALUE removeLast( void );

    /**
     * \brief   Adds element to Linked List and returns position of new head element.
     * \param   newElemnt   New head element to add in Linked List
     * \return  Returns position new head element in Linked List.
     **/
    inline LISTPOS add( VALUE_TYPE newElemnt );

    /**
     * \brief   Removes all element in Linked List. After call, Linked List is empty.
     **/
    inline void removeAll( void );

    /**
     * \brief	Removes element at given position and returns value of removed element.
     * \param	atPosition  Position of element to remove from Linked List. 
     *                      It should be valid position, otherwise assertion is raised.
     * \return	Returns value of removed Element.
     **/
    VALUE removeAt( LISTPOS atPosition );

    /**
     * \brief	Removes element at given position and on output returns value of removed element.
     * \param	atPosition  On input, position of element to remove from Linked List.
     *                      It should be valid position, otherwise assertion is raised.
     * \param   out_Value   On output, it contains value of removed element
     * \return	Returns true if element was removed.
     **/
    bool removeAt( LISTPOS atPosition, VALUE & out_Value );

    /**
     * \brief	Searches and removes first match of given element from Linked List.
     *          If 'searchAfter' is NULL, it will start searching element from head.
     *          And returns true if element was found and successfully removed from Linked List.
     * \param	removeElement	Element to search and remove from Linked List
     * \param	searchAfter	    Position at which starts searching, the searching done by moving to next element
     * \return	Returns true if element was found and successfully removed from linked list.
     **/
    bool removeEntry( VALUE_TYPE removeElement, LISTPOS searchAfter = NULL );

    /**
     * \brief	Searches position of element by given value. If searchAfter is valid, the searching will be started
     *          from given position and will move to next element.
     * \param	searchValue	Value of element to search
     * \param	searchAfter	If valid value, the position to start searching. 
     *                      Otherwise searching will be started from position head element
     * \return	Returns true if element was found in Linked List
     **/
    LISTPOS find( VALUE_TYPE searchValue, LISTPOS searchAfter = NULL ) const;

    /**
     * \brief	Returns position of element by given index. The index of head element is 0.
     *          The index of last element (tail element) is GetSize() - 1.
     *          If invalid index is passed, function returns NULL
     * \param	index	The index of element in Linked List to get position
     * \return	Returns position of element if index is valid, i.e. more or equal to 0 and less than GetSize().
     *          Otherwise returns NULL (INVALID_POSITION)
     **/
    LISTPOS findIndex( int index ) const;

    /**
     * \brief	Returns valid index of element by given valid position.
     *          Otherwise returns -1.
     * \param	atPosition  The position of element, which index should be returned.
     * \return	The index of element by given position
     **/
    int makeIndex( LISTPOS atPosition ) const;

    /**
     * \brief	Search element by given searchValue value starting at position searchAfter
     *          and returns valid index of found element. Otherwise it returns invalid 
     *          index -1. If searchAfter is NULL, it will start searching from Head position.
     * \param	searchValue	Value to search
     * \param	startAfter	Position to start searching. If NULL, it starts searching
     *                      from Head position.
     * \return	Returns valid index of found element.
     **/
    inline int makeIndex( VALUE_TYPE searchValue, LISTPOS startAfter = NULL ) const;

    /**
     * \brief   Call to resort the linked list.
     * \param   sortAcceding    If true, the sorting is acceding. Otherwise, it sorts descending.
     *                          By default, the sorting is acceding.
     **/
    void resort( bool sortAcceding = true );

//////////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Called when comparing 2 values of element.
     *          Overwrite method when need to change comparison.
     * \param   value1  Value on left side to compare.
     * \param   value2  Value on right side to compare.
     * \return  If function returns true, 2 values are equal.
     *          Otherwise, they are not equal.
     **/
    inline bool isEqualValues( VALUE_TYPE value1, VALUE_TYPE value2) const;

    /**
     * \brief   Called when comparing 2 values of element.
     *          Overwrite method when need to change comparison.
     * \param   value1  Value on left side to compare.
     * \param   value2  Value on right side to compare.
     * \return  Function returns:
     *              returns  1, if value1 is greater than value2
     *              returns  0, if value1 is equal to value2
     *              returns -1, if value1 is less than value2
     **/
    inline int compareValues( VALUE_TYPE value1, VALUE_TYPE value2) const;

//////////////////////////////////////////////////////////////////////////
// Member Variables
//////////////////////////////////////////////////////////////////////////
protected:
    eSort   mSorting;
    /**
     * \brief   The size of Linked List, contains amount of elements in Linked List.
     *          If Linked List is empty, this is zero.
     **/
    int     mCount;
    /**
     * \brief   The Head element (position) of Linked List.
     *          If Linked List is empty, this is NULL.
     *          If it has only one element, Head is equal to Tail position
     **/
    Block * mHead;
    /**
     * \brief   The Tail element (position) of Linked List.
     *          If Linked List is empty, this is NULL
     *          If it has only one element, Tails is equal to Head position.
     **/
    Block * mTail;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Adds new block in sorted linked list.
     *          First, it will search the right position of new block in the list,
     *          depending whether the sorting is acceding or descending, and add new block in the right position
     * \param   newBlock    New block to add in the linked list.
     * \return  Returns new position in the linked list where new block is added.
     **/
    LISTPOS _add( Block * newBlock );

    /**
     * \brief   Inserts given new block before indicated position of linked list.
     * \param   newBlock        New block to insert at indicated position.
     * \param   beforePosition  The indicated position of linked list. The new block
     *                          is inserted before this indicated position.
     * \return  Returns new position in the linked list where new block is inserted.
     **/
    LISTPOS _insertElementBefore( Block * newBlock, LISTPOS beforePosition );

    /**
    * \brief   Inserts given new block after indicated position of linked list.
    * \param   newBlock        New block to insert at indicated position.
    * \param   beforePosition  The indicated position of linked list. The new block
    *                          is inserted after this indicated position.
    * \return  Returns new position in the linked list where new block is inserted.
    **/
    LISTPOS _insertElementAfter( Block * newBlock, LISTPOS afterPosition );
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block class implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block::Block( void )
    : mNext (NULL)
    , mPrev (NULL)
    , mValue( )
{   ; }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block::Block(VALUE_TYPE value)
    : mNext (NULL)
    , mPrev (NULL)
    , mValue((VALUE)value)
{   ; }

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block::~Block( void )
{   ; }


//////////////////////////////////////////////////////////////////////////
// TESortedLinkedList<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::TESortedLinkedList( bool sortAcceding /*= true*/ )
    : Implement         ( )
    , TemplateConstants ( )
    
    , mSorting              ( sortAcceding ? TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::SortAcceding : TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::SortDescending )
    , mCount                ( 0 )
    , mHead                 ( NULL )
    , mTail                 ( NULL )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::TESortedLinkedList( const TESortedLinkedList<VALUE, VALUE_TYPE, Implement> & src )
    : Implement         ( )
    , TemplateConstants ( )

    , mSorting  ( src.mSorting )
    , mCount    ( 0 )
    , mHead     ( NULL )
    , mTail     ( NULL )
{
    LISTPOS pos = NULL;
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = src.mHead;
    for ( ; block != NULL; block = block->mNext)
        pos = _insertElementAfter(DEBUG_NEW TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block(block->mValue), pos);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::~TESortedLinkedList( void )
{
    removeAll();
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
const TESortedLinkedList<VALUE, VALUE_TYPE, Implement> & TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::operator = (const TESortedLinkedList<VALUE, VALUE_TYPE, Implement> & src)
{
    if ( static_cast<const TESortedLinkedList<VALUE, VALUE_TYPE, Implement> *>(this) != &src)
    {
        removeAll();
        mSorting    = src.mSorting;
        LISTPOS pos = NULL;
        const TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = src.mHead;
        for ( ; block != NULL; block = block->mNext)
            pos = _insertElementAfter(DEBUG_NEW TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block(block->mValue), pos);
    }
    
    return (*this);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
bool TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::operator == ( const TESortedLinkedList<VALUE, VALUE_TYPE, Implement> & otherList ) const
{
    bool result = true;
    if (this != &otherList) // <== if same list, no need to compare
    {
        result = false; // <== initially lists are not equal
        if (mCount == otherList.mCount) // <== if list contain different amount of items, no need to compare
        {
            const TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * thisBlock   = mHead;
            const TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * otherBlock  = otherList.mHead;
            for (int i = 0; i < mCount; ++ i)
            {
                if ( isEqualValues(thisBlock->mValue, otherBlock->mValue) )
                {
                    thisBlock = thisBlock->mNext;
                    otherBlock = otherBlock->mNext;
                }
                else
                {
                    break;
                }
            }

            result = thisBlock == otherBlock == NULL; // <== we reached end, items are equal
        }
    }
    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::operator []( int atIndex ) const
{
    ASSERT(atIndex >= 0 && atIndex < mCount);
    const TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block= mHead;
    for ( int i = 0; i < atIndex && block != NULL; ++ i, block = block->mNext)
        ;

    ASSERT(block != NULL);
    return block->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::operator []( LISTPOS atPosition ) const
{
    ASSERT(atPosition != NULL);
    return reinterpret_cast<const TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(atPosition)->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline bool TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::isEmpty( void ) const	
{
    return (mCount == 0);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline int TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::getSize( void ) const	
{
    return mCount;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline bool TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::isAcceding( void ) const	
{
    return (mSorting == TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::SortAcceding);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline bool TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::isDescending( void ) const	
{
    return (mSorting == TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::SortDescending);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::firstPosition( void )	const
{
    return static_cast<LISTPOS>(mHead);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::lastPosition( void ) const
{
    return static_cast<LISTPOS>(mTail);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::firstEntry( void ) const
{
    ASSERT(mHead != NULL);
    return static_cast<VALUE_TYPE>(mHead->mValue);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::lastEntry( void ) const	
{
    ASSERT(mTail != NULL);
    return static_cast<VALUE_TYPE>(mTail->mValue);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::getNext( LISTPOS & in_out_NextPosition ) const
{
    ASSERT(in_out_NextPosition != TemplateConstants::INVALID_POSITION && mCount != 0);
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(in_out_NextPosition);
    in_out_NextPosition = static_cast<LISTPOS>(block->mNext);
    return block->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::nextPosition(LISTPOS atPosition) const
{
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(atPosition);
    return static_cast<LISTPOS>(block != NULL ? block->mNext : NULL);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::getPrev(LISTPOS & in_out_PrevPosition) const
{
    ASSERT(in_out_PrevPosition != TemplateConstants::INVALID_POSITION && mCount != 0);
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(in_out_PrevPosition);
    in_out_PrevPosition = static_cast<LISTPOS>(block->mPrev);
    return block->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::prevPosition(LISTPOS atPosition) const
{
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(atPosition);
    return static_cast<LISTPOS>(block != NULL ? block->mPrev : NULL);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::getAt( LISTPOS atPosition ) const
{
    ASSERT(atPosition != TemplateConstants::INVALID_POSITION && mCount != 0);
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(atPosition);
    return block->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline VALUE_TYPE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::getAt(int index) const
{
    ASSERT(index >= 0 && index < mCount);
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block   = mHead;
    for (int i = 0; i < index; ++ i)
        block = block->mNext;
    return block->mValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
bool TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::nextEntry(LISTPOS & in_out_NextPosition, VALUE & out_NextValue) const
{
    ASSERT(in_out_NextPosition != TemplateConstants::INVALID_POSITION && mCount != 0);
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(in_out_NextPosition);
    in_out_NextPosition = static_cast<LISTPOS>(block->mNext);
    if (in_out_NextPosition != TemplateConstants::INVALID_POSITION)
        out_NextValue = static_cast<VALUE>(block->mNext->mValue);
    
    return (in_out_NextPosition != TemplateConstants::INVALID_POSITION);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
bool TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::prevEntry(LISTPOS &in_out_PrevPosition, VALUE & out_PrevValue) const
{
    ASSERT(in_out_PrevPosition != TemplateConstants::INVALID_POSITION && mCount != 0);
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(in_out_PrevPosition);
    in_out_PrevPosition = static_cast<LISTPOS>(block->mPrev);
    if (in_out_PrevPosition != TemplateConstants::INVALID_POSITION)
        out_PrevValue = static_cast<VALUE>(block->mValue);

    return (in_out_PrevPosition != TemplateConstants::INVALID_POSITION);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
VALUE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::removeFirst( void )
{
    ASSERT(mHead != NULL && mCount != 0);

    VALUE result = mHead->mValue;
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = mHead->mNext; 

    delete	mHead;
    -- mCount;

    if (mCount == 0)	
    {
        ASSERT(block == NULL);
        mHead = mTail = NULL;
    }
    else if (mCount == 1) 
    {
        ASSERT(block == mTail);
        mTail->mPrev = NULL;
        mHead        = mTail;
    }
    else
    {
        ASSERT(block != NULL);
        mHead        = block;
        mHead->mPrev = NULL;
    }
    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
VALUE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::removeLast( void )
{
    ASSERT(mTail != NULL && mCount != 0);

    VALUE result = mTail->mValue;
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = mTail->mPrev;
    
    delete	mTail;
    -- mCount;

    if (mCount == 0)
    {
        ASSERT(block == NULL);
        mHead = mTail = NULL;
    }
    else if (mCount == 1)
    {
        ASSERT(block == mHead);
        mHead->mNext = NULL;
        mTail        = mHead;
    }
    else
    {
        ASSERT(block != NULL);
        mTail        = block;
        mTail->mNext = NULL;
    }
    return result;
}


template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::add(VALUE_TYPE newElement)
{
    return _add( DEBUG_NEW TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block(newElement) );
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline void TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::removeAll( void )
{
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = mHead;
    while (block != NULL)
    {
        block = block->mNext;
        delete mHead;
        mHead = block;
    }

    mHead   = mTail = static_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(NULL);
    mCount  = 0;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
VALUE TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::removeAt(LISTPOS atPosition)
{
    ASSERT(atPosition != TemplateConstants::INVALID_POSITION && mCount != 0);

    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block   = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(atPosition);

    if (mCount == 1)
    {
        ASSERT(mHead == block && mTail == block);
        mTail = mHead = NULL;
    }
    else if (mCount == 2)
    {
        ASSERT(mHead == block || mTail == block);
        mHead = mTail = (block->mPrev != NULL ? block->mPrev : block->mNext);

        ASSERT(mHead != NULL);
        mHead->mPrev = mHead->mNext = mTail->mPrev = mTail->mNext = NULL;
    }
    else
    {
        if (block == mTail)
        {
            block->mPrev->mNext = NULL;
            mTail               = block->mPrev;
        }
        else if (mHead == block)
        {
            block->mNext->mPrev = NULL;
            mHead               = block->mNext;
        }
        else
        {
            ASSERT(block->mNext != NULL && block->mPrev != NULL);
            block->mPrev->mNext = block->mNext;
            block->mNext->mPrev = block->mPrev;
        }
    }

    VALUE oldValue  = block->mValue;
    delete	block;
    mCount --;
    
    return oldValue;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
bool TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::removeAt(LISTPOS atPosition, VALUE &out_value)
{
    ASSERT(atPosition != TemplateConstants::INVALID_POSITION && mCount != 0);

    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(atPosition);
    bool result = false;
    if (block != NULL)
    {
        out_value   = block->mValue;
        result      = true;
    }

    if (mCount == 1)
    {
        ASSERT(mHead == block && mTail == block);
        mTail = mHead = NULL;
    }
    else if (mCount == 2)
    {
        ASSERT(mHead == block || mTail == block);
        mHead = mTail = (block->mPrev != NULL ? block->mPrev : block->mNext);

        ASSERT(mHead != NULL);
        mHead->mPrev = mHead->mNext = mTail->mPrev = mTail->mNext = NULL;
    }
    else
    {
        if (block == mTail)
        {
            block->mPrev->mNext = NULL;
            mTail               = block->mPrev;
        }
        else if (mHead == block)
        {
            block->mNext->mPrev = NULL;
            mHead               = block->mNext;
        }
        else
        {
            ASSERT(block->mNext != NULL && block->mPrev != NULL);
            block->mPrev->mNext = block->mNext;
            block->mNext->mPrev = block->mPrev;
        }
    }
    delete	block;
    -- mCount;
    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
bool TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::removeEntry(VALUE_TYPE removeElement, LISTPOS searchAfter /*= NULL*/)
{
    bool result = false;
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = mHead;
    if (searchAfter != TemplateConstants::INVALID_POSITION && mCount != 0)
        block = static_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(searchAfter)->mNext;

    for ( ; block != NULL; block = block->mNext)
    {
        if ( isEqualValues(removeElement, block->mValue) )
            break;
    }

    if (block != NULL)
    {
        removeAt(static_cast<LISTPOS>(block));
        result = true;
    }

    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::find(VALUE_TYPE searchValue, LISTPOS searchAfter /*= NULL*/) const
{	
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block   = mHead;
    if (searchAfter != TemplateConstants::INVALID_POSITION && mCount != 0)
        block = static_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(searchAfter)->mNext;
    
    for ( ; block != NULL; block = block->mNext)
    {
        if ( isEqualValues(block->mValue, searchValue) )
            break;
    }

    return static_cast<LISTPOS>(block);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::findIndex(int index) const
{
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *block   = static_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(index >= 0 && index < mCount? mHead : NULL);
    if (block != NULL)
    {
        for (int i = 0; i < index; ++ i)
            block = block->mNext;
    }

    return static_cast<LISTPOS>(block);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
int TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::makeIndex(LISTPOS atPosition) const
{
    int result = NECommon::InvalidIndex;
    if (mCount != 0 && atPosition != TemplateConstants::INVALID_POSITION)
    {
        TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * search = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(atPosition);
        TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block  = mHead;
        for (result = 0; result < mCount; result ++)
        {
            ASSERT(block != NULL);
            if (block == search)
                break;

            block = block->mNext;
        }
    }
    return (result != mCount ? result : -1);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline int TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::makeIndex(VALUE_TYPE searchValue, LISTPOS startAfter /*= NULL*/) const
{
    return makeIndex(find(searchValue, startAfter));
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
void TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::resort(bool sortAcceding /*= true */)
{
    TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = mHead;

    mHead   = NULL;
    mTail   = NULL;
    mCount  = 0;
    mSorting= sortAcceding ? TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::SortAcceding : TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::SortDescending;
    while ( block != NULL )
    {
        TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * next = block->mNext;
        block->mNext = block->mPrev = NULL;
        _add(block);
        block = next;
    }
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::_insertElementBefore( Block * newBlock, LISTPOS beforePosition )
{
    if (newBlock != NULL)
    {
        TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(beforePosition);
        if ( mCount == 0 )
        {
            ASSERT( mHead == NULL );
            ASSERT( mTail == NULL );
            ASSERT( block == NULL );
            mHead = mTail = newBlock;
        }
        else if ( mCount == 1 )
        {
            ASSERT(mHead == block && mTail == block);
            mHead       = newBlock;
            mHead->mNext= mTail;
            mTail->mPrev= mHead;
        }
        else if ( block == mHead )
        {
            mHead->mPrev    = newBlock;
            newBlock->mNext = mHead;
            mHead           = newBlock;
        }
        else
        {
            ASSERT(block->mPrev != NULL);
            newBlock->mPrev         = block->mPrev;
            newBlock->mPrev->mNext  = newBlock;
            block->mPrev            = newBlock;
            newBlock->mNext         = block;
        }

        ++ mCount;
    }

    return static_cast<LISTPOS>(newBlock);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::_insertElementAfter( Block * newBlock, LISTPOS afterPosition )
{
    if (newBlock != NULL)
    {
        TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * block = reinterpret_cast<TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block *>(afterPosition);
        if ( mCount == 0 )
        {
            ASSERT( mHead == NULL );
            ASSERT( mTail == NULL );
            ASSERT( block == NULL );
            mHead = mTail = newBlock;
        }
        else if ( mCount == 1 )
        {
            ASSERT(mHead == block && mTail == block);
            mTail       = newBlock;
            mTail->mPrev= mHead; 
            mHead->mNext= mTail;
        }
        else if ( block == mTail )
        {
            mTail->mNext    = newBlock;
            newBlock->mPrev = mTail;
            mTail           = newBlock;
        }
        else
        {
            ASSERT(block->mNext != NULL);
            newBlock->mNext         = block->mNext;
            newBlock->mNext->mPrev  = newBlock;
            block->mNext            = newBlock;
            newBlock->mPrev         = block;
        }

        mCount ++;
    }

    return static_cast<LISTPOS>(newBlock);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
LISTPOS TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::_add( Block * newBlock )
{
    LISTPOS result = NULL;

    if ( newBlock != NULL )
    {
        LISTPOS before  = NULL;
        LISTPOS after   = NULL; 
        VALUE_TYPE newElement = newBlock->mValue;

        if ( mSorting == TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::SortAcceding )
        {
            TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * pos = mHead;
            while ( (pos != NULL) && (compareValues(newElement, pos->mValue) < 0) )
                pos = pos->mNext;

            before= static_cast<LISTPOS>(pos);
            after = static_cast<LISTPOS>(mTail);
        }
        else
        {
            TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::Block * pos = mTail;
            while ( (pos != NULL) && (compareValues(newElement, pos->mValue) > 0) )
                pos = pos->mPrev;

            after = static_cast<LISTPOS>(pos);
            before= static_cast<LISTPOS>(mHead);            
        }

        if ( result != NULL)
        {
            result = _insertElementAfter(newBlock, after);
        }
        else
        {
            result = _insertElementBefore(newBlock, before);
        }
    }

    return result;
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline bool TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::isEqualValues(VALUE_TYPE value1, VALUE_TYPE value2) const
{
    return Implement::implEqualValues(value1, value2);
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TESortImpl<VALUE_TYPE>*/>
inline int TESortedLinkedList<VALUE, VALUE_TYPE, Implement>::compareValues(VALUE_TYPE value1, VALUE_TYPE value2) const
{
    return Implement::implCompareValues(value1, value2);
}

//////////////////////////////////////////////////////////////////////////
// TESortedLinkedList<VALUE, VALUE_TYPE, Implement> class template friend operators
//////////////////////////////////////////////////////////////////////////

template <typename V, typename VT, class Impl> 
const IEInStream & operator >> ( const IEInStream & stream, TESortedLinkedList<V, VT, Impl> & input )
{
    input.removeAll();

    int size = 0;
    stream >> size;

    LISTPOS pos = NULL;
    for (int i = 0; i < size; ++ i)
    {
        V newValue;
        stream >> newValue;
        pos = input._insertElementAfter(DEBUG_NEW class TESortedLinkedList<V, VT, Impl>::Block(newValue), pos);
    }

    if ( stream.read(reinterpret_cast<unsigned char *>(&input.mSorting), sizeof(int)) == 0 )
        input.resort(true);

    return stream;
}

template <typename V, typename VT, class Impl> 
IEOutStream & operator << ( IEOutStream & stream, const TESortedLinkedList<V, VT, Impl> & output )
{
    int size = output.mCount;
    stream << size;

    const class TESortedLinkedList<V, VT, Impl>::Block * block = output.mHead;
    for ( ; block != NULL; block = block->mNext)
        stream << block->mValue;

    stream << static_cast<int>(output.mSorting);
    return stream;
}

#endif  // AREG_BASE_TESORTEDLINKEDLIST_HPP
