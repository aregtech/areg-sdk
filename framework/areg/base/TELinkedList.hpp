#ifndef AREG_BASE_TELINKEDLIST_HPP
#define AREG_BASE_TELINKEDLIST_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/TELinkedList.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Linked List class template.
 *              The linked list contains list of values that can be
 *              accessed either by Position or by valid index.
 *              The size of Linked List is dynamic, Elements contain
 *              information of previous and next Block.
 *              Accessing element, insert and remove by Position
 *              operations are proceed fast. Accessing by index
 *              is slower.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TETemplateBase.hpp"
#include "areg/base/IEIOStream.hpp"

#include <algorithm>
#include <list>

//////////////////////////////////////////////////////////////////////////
// TELinkedList<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Bi-directional Linked List class template to access, insert and 
 *          remove entries by position, where is next and previous entry
 *          at the given position is accessed fast.
 *
 *          The type VALUE should have at least default constructor, applicable
 *          comparing and assigning operators. The TELinkedList object is not
 *          thread safe and data access should be synchronized manually.
 *
 * \tparam  VALUE   The type of stored elements should be either primitive or have
 *                  default constructor, applicable comparing and assigning operators.
 **/
template <typename VALUE> 
class TELinkedList  : private Constless<std::list<VALUE>>
{
//////////////////////////////////////////////////////////////////////////
// Internal objects and types declaration
//////////////////////////////////////////////////////////////////////////
public:
    using LISTPOS   = typename std::list<VALUE>::iterator;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs en empty linked list object.
     **/
    TELinkedList( void ) = default;
    
    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    TELinkedList( const TELinkedList<VALUE> & src ) = default;
    
    /**
     * \brief   Moves entries from given source.
     * \param   src     The source to move data.
     **/
    TELinkedList( TELinkedList<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor.
     **/
    ~TELinkedList( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid zero-based index.
     *          May be used on either the right (r-value) or the left (l-value) of an assignment statement.
     **/
    inline VALUE& operator [] (int atIndex);
    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid zero-based index.
     *          The index should be valid number between 0 and (mSize -1).
     *          May be used on the right (r-value).
     **/
    inline const VALUE& operator [] (int atIndex) const;

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid position value.
     *          May be used on either the right (r-value) or the left (l-value) of an assignment statement.
     **/
    inline VALUE& operator [] (LISTPOS atPosition);
    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid position value.
     *          May be used on the right (r-value).
     **/
    inline const VALUE& operator [] (const LISTPOS atPosition) const;

    /**
     * \brief   Assigning operator. Copies all values from given source.
     *          If linked list previously had values, they will be removed and new values
     *          from source linked list will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of linked list values.
     **/
    inline TELinkedList<VALUE> & operator = ( const TELinkedList<VALUE> & src );

    /**
     * \brief   Move operator. Moves all values from given source.
     *          If Array previously had values, they will be removed and new values
     *          from source Array will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of linked list values.
     **/
    inline TELinkedList<VALUE> & operator = ( TELinkedList<VALUE> && src ) noexcept;

    /**
     * \brief   Checks equality of 2 linked list objects, and returns true if they are equal.
     *          There should be possible to compare VALUE type entries in the linked list.
     * \param   other   The linked list object to compare.
     **/
    inline bool operator == ( const TELinkedList<VALUE> & other ) const;

    /**
     * \brief   Checks inequality of 2 linked list objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries in the linked list.
     * \param   other   The linked list object to compare.
     **/
    inline bool operator != ( const TELinkedList<VALUE> & other ) const;

/************************************************************************/
// Friend global operators to make Linked List streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream linked list values.
     *          If linked list previously had values, they will be removed and new values
     *          from the stream will be set in the same sequence as they are present
     *          in the stream. There should be possibility to initialize values from
     *          streaming object and if VALUE is not a primitive, but an object, it
     *          should have implemented streaming operator.
     * \param   stream  The streaming object to read values.
     * \param   input   The linked list object to save initialized values.
     **/
    template<typename VALUE>
    friend inline const IEInStream & operator >> ( const IEInStream & stream, TELinkedList<VALUE> & input );

    /**
     * \brief   Writes to the stream the values of linked list.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream values and if VALUE is not a
     *          primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The stream to write values.
     * \param   input   The linked list object containing value to stream.
     **/
    template<typename VALUE>
    friend inline IEOutStream & operator << ( IEOutStream & stream, const TELinkedList<VALUE> & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Attributes
/************************************************************************/

    /**
     * \brief   Returns true if the linked list is empty and has no elements.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief	Returns the current size of the linked list.
     **/
    inline uint32_t getSize( void ) const;

    /**
     * \brief	Returns the position of the first value entry in the linked list, which is
     *          not invalid if the linked list is not empty. Otherwise, returns invalid position.
     **/
    inline LISTPOS firstPosition(void) const;

    /**
     * \brief	Returns the position of the last value entry in the linked list, which is
     *          not invalid if the linked list is not empty. Otherwise, returns invalid position.
     **/
    inline LISTPOS lastPosition(void) const;

    /**
     * \brief   Returns true if specified position points the first entry in the linked list.
     * \param   pos     The position to check.
     **/
    inline bool isStartPosition(const LISTPOS pos) const;

    /**
     * \brief   Returns true if specified position points the last entry in the linked list.
     * \param   pos     The position to check.
     **/
    inline bool isLastPosition(const LISTPOS pos) const;

    /**
     * \brief   Returns the invalid position of the linked list.
     **/
    inline LISTPOS invalidPosition(void) const;

    /**
     * \brief   Returns true if specified position is invalid, i.e. points the end of the linked list.
     **/
    inline bool isInvalidPosition(const LISTPOS pos) const;

    /**
     * \brief   Returns true if the given position is not pointing the end of the linked list.
     *          Note, it does not check whether there is a such position existing in the linked list.
     */
    inline bool isValidPosition(const LISTPOS pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the linked list.
     *          The duration of checkup depends on the location of the position in the linked list.
     * \param   pos     The position to check.
     **/
    inline bool checkPosition(const LISTPOS pos) const;

    /**
     * \brief	Checks whether given element exist in the linked list or not. The elements of type
     *          VALUE should have comparing operators.
     * \param	elemSearch  The element to search.
     * \param	startAt	    The position to start searching.
     * \return	Returns true if could find element starting at given position.
     **/
    inline bool contains(const VALUE& elemSearch) const;
    inline bool contains( const VALUE & elemSearch, LISTPOS startAt) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Remove all entries of the linked list.
     **/
    inline void clear(void);

    /**
     * \brief   Delete extra entries in the linked list.
     **/
    inline void freeExtra(void);

    /**
     * \brief   Sets the size of the linked list to zero and deletes all capacity space.
     */
    inline void release(void);

    /**
     * \brief   Returns value of head element in Linked List container.
     *          On call the Linked List should not be empty, otherwise assertion is raised.
     **/
    inline const VALUE & getFirstEntry( void ) const;
    inline VALUE & getFirstEntry(void);

    /**
     * \brief   Returns value of tail element in Linked List container.
     *          On call the Linked List should not be empty, otherwise assertion is raised.
     **/
    inline const VALUE & getLastEntry( void ) const;
    inline VALUE & getLastEntry(void);

    /**
     * \brief	Returns the value at the given position of the linked list and on output the value
     *          of in_out_NextPosition points the next entry in the linked list or points to the 
     *          invalid position if no more elements exist.
     * \param	in_out_NextPosition On input, this should be valid position of the element in the Linked List.
     *                              On output, this contains position of the next entry in the Linked List
     *                              or invalid position if reached end of the linked list.
     **/
    inline const VALUE & getNext( LISTPOS & IN OUT in_out_NextPosition ) const;
    inline VALUE& getNext(LISTPOS& IN OUT in_out_NextPosition);

    /**
     * \brief	Returns either valid position of the next entry in the Linked List or invalid position if
     *          reached end of the linked list.
     * \param	atPosition	Actual position to get the next position. Should be valid position.
     **/
    inline LISTPOS nextPosition( LISTPOS atPosition ) const;

    /**
     * \brief	Returns the value at the given position of the linked list and on output the value
     *          of in_out_PrevPosition points the previous entry in the linked list or points to the
     *          invalid position if it was the fist entry in the linked list.
     * \param	in_out_PrevPosition On input, this should be valid position of the element in the Linked List.
     *                              On output, this contains position of the previous entry in the Linked List
     *                              or invalid position if there is no more previous position in the linked list.
     **/
    inline const VALUE & getPrev( LISTPOS & IN OUT in_out_PrevPosition ) const;
    inline VALUE& getPrev(LISTPOS& IN OUT in_out_PrevPosition);

    /**
     * \brief	Returns either valid position of the previous entry in the Linked List or invalid position if
     *          indicates the first entry (no previous entry) of the linked list.
     * \param	atPosition	Actual position to get the previous position. Should be valid position.
     **/
    inline LISTPOS prevPosition( LISTPOS atPosition ) const;

    /**
     * \brief   Returns the value of entry at the given valid position.
     * \param   atPosition  The valid position in Linked List
     **/
    inline VALUE& valueAtPosition(LISTPOS atPosition);
    inline const VALUE & valueAtPosition( LISTPOS atPosition ) const;

    /**
     * \brief   Returns element value by valid zero-based index, which can be used by left (l-value) and
     *          right operation (r-value). The index should be valid.
     **/
    inline VALUE & getAt( uint32_t index );

    /**
     * \brief   Returns element value by valid zero-based index, which can be used by right operation (r-value).
     *          The index should be valid.
     **/
    inline const VALUE & getAt(uint32_t index ) const;

    /**
     * \brief	Extracts next position and value of the element in the linked list followed position.
     *
     * \param	in_out_NextPosition	On input this indicates the valid position of the entry in the linked list.
     *                              On output, this parameter points either next valid entry in the linked list
     *                              or invalid entry if no more entry is following.
     * \param	out_NextValue       On output, this contain value of the next entry in the linked list.
     * \return	Returns true, if there is a next element and the output value is valid.
     **/
    inline bool nextEntry( LISTPOS & IN OUT in_out_NextPosition, VALUE & OUT out_NextValue ) const;

    /**
     * \brief	Extracts previous position and value of the element in the linked list prior position.
     *
     * \param	in_out_PrevPosition	On input this indicates the valid position of the entry in the linked list.
     *                              On output, this parameter points either previous valid entry in the linked list
     *                              or invalid entry if has no previous entry, i.e. indicates first entry.
     * \param	out_PrevValue       On output, this contain value of the previous entry in the linked list.
     * \return	Returns true, if there is a previous element and the output value is valid.
     **/
    inline bool prevEntry( LISTPOS & IN OUT in_out_PrevPosition, VALUE & OUT out_PrevValue ) const;

    /**
     * \brief   Removes head element from Linked List.
     **/
    inline void removeFirst( void );

    /**
     * \brief   Removes the head element from the linked list. On output, the 'value' contains
     *          data of removed element. Function returns true if linked list was not empty 
     *          and the 'value' contains the data of removed element.
     **/
    inline bool removeFirst( VALUE & OUT value );

    /**
     * \brief   Removes tail element from Linked List.
     **/
    inline void removeLast( void );

    /**
     * \brief   Removes the tails element from the linked list. On output, the 'value' contains
     *          data of removed element. Function returns true if linked list was not empty
     *          and the 'value' contains the data of removed element.
     **/
    inline bool removeLast(VALUE& value);

    /**
     * \brief   Add element to head of Linked List.
     * \param   newElement   New head element to add in Linked List
     **/
    inline void pushFirst( const VALUE & newElement );
    inline void pushFirst( VALUE && newElement );

    /**
     * \brief   Adds new entry at the head of linked list if it is not existing.
     *          If the entry with specified value exists, the entry is not added.
     *          The method returns true if new element is added.
     **/
    inline bool pushFirstIfUnique(const VALUE& newElement);
    inline bool pushFirstIfUnique(VALUE&& newElement);

    /**
     * \brief   Add element to tail of Linked List.
     * \param   newElement   New tail element to add in Linked List
     **/
    inline void pushLast( const VALUE & newElement );
    inline void pushLast( VALUE && newElement );

    /**
     * \brief   Adds new entry at the tails of linked list if it is not existing.
     *          If the entry with specified value exists, the entry is not added.
     *          The method returns true if new element is added.
     **/
    inline bool pushLastIfUnique(const VALUE& newElement);
    inline bool pushLastIfUnique(VALUE&& newElement);

    /**
     * \brief   Pops the element at the head of linked list and returns the stored value.
     *          The linked list must not be empty.
     **/
    inline VALUE popFirst(void);

    /**
     * \brief   Pops the element at the tails of linked list and returns the stored value.
     *          The linked list must not be empty.
     **/
    inline VALUE popLast(void);

    /**
     * \brief	Inserts new element before given position. If given position is head element, 
     *          it adds new head element.
     * \param	beforePosition  The Linked List element valid position before new element should be inserted.
     * \param	newElement	    Value of new element to insert
     * \return	Returns position of new inserted element.
     **/
    inline LISTPOS insertBefore( LISTPOS beforePosition, const VALUE & newElement );
    inline LISTPOS insertBefore(LISTPOS beforePosition, VALUE && newElement);

    /**
     * \brief	Inserts new element after given position. If given position is tail element,
     *          it adds new tail element.
     * \param	afterPosition   The Linked List element position after new element should be inserted
     * \param	newElement	    Value of new element to insert
     * \return	Returns position of new inserted element.
     **/
    inline LISTPOS insertAfter(LISTPOS afterPosition, const VALUE & newElement);
    inline LISTPOS insertAfter(LISTPOS afterPosition, VALUE && newElement);

    /**
     * \brief	Sets new value at give position.
     * \param	atPosition	The Linked List element valid position to change value.
     * \param	newValue	The Value to update.
     **/
    inline void setAt( LISTPOS atPosition, const VALUE & newValue );

    /**
     * \brief	Removes element at given position and returns position of the next entry in the linked list.
     *          Returns invalid position if tail entry is removed.
     * \param	atPosition  Position of the element to remove from Linked List. 
     * \param   out_Value   On output, it contains value of removed element
     **/
    inline LISTPOS removeAt( LISTPOS atPosition );
    inline LISTPOS removeAt( LISTPOS atPosition, VALUE & OUT out_Value );

    /**
     * \brief	Searches and removes first match of entry, which value is equal to the given element.
     *          Returns true if found and removed entry with success.
     * \param	removeElement	Element to search and remove from Linked List
     * \param	searchAfter	    The valid position in the linked list to start searching.
     **/
    inline bool removeEntry( const VALUE & removeElement );
    inline bool removeEntry(const VALUE& removeElement, LISTPOS searchAfter);

    /**
     * \brief	Searches position of the entry by given value and returns valid position if found an entry.
     *          Otherwise, it returns invalid position.
     * \param	searchValue	    Value of element to search.
     * \param	searchAfter	    The valid position in the linked list to start searching.
     **/
    inline LISTPOS find(const VALUE& searchValue) const;
    inline LISTPOS find( const VALUE & searchValue, LISTPOS searchAfter ) const;

    /**
     * \brief	Returns valid position of the element by given zero-based index.
     *          Returns invalid position if the index is invalid.
     * \param	index	The index of element in Linked List to get position.
     **/
    inline LISTPOS findIndex( uint32_t index ) const;

    /**
     * \brief	Returns valid zero-based index of the element by given valid position.
     *          Returns invalid index (-1) if the position is invalid.
     * \param	atPosition  The valid position in the linked list to return index.
     **/
    inline int makeIndex( LISTPOS atPosition ) const;

    /**
     * \brief   Extracts elements from the given source and inserts into the linked list.
     *          No elements are copied. The container other becomes empty after the operation.
     * \param   source  The source of linked list to merge.
     */
    inline void merge(const TELinkedList<VALUE> & source);
    inline void merge(TELinkedList<VALUE> && source);

protected:

    /**
     * \brief   Returns the position of the element at the given zero-based index.
     * \param   index   The index of the element to return position.
     **/
    inline LISTPOS getPosition( uint32_t index ) const;
    inline LISTPOS getPosition(uint32_t index);

//////////////////////////////////////////////////////////////////////////
// Member Variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The linked list object
     **/
    std::list<VALUE>    mValueList;
};

//////////////////////////////////////////////////////////////////////////
// TELinkedList<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE >
inline TELinkedList<VALUE>& TELinkedList<VALUE>::operator = (const TELinkedList<VALUE>& src)
{
    mValueList = src.mValueList;
    return (*this);
}

template <typename VALUE >
inline TELinkedList<VALUE>& TELinkedList<VALUE>::operator = (TELinkedList<VALUE>&& src) noexcept
{
    mValueList = std::move(src.mValueList);
    return (*this);
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::operator == (const TELinkedList<VALUE>& other) const
{
    return (mValueList == other.mValueList);
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::operator != (const TELinkedList<VALUE>& other) const
{
    return (mValueList != other.mValueList);
}

template <typename VALUE >
inline VALUE & TELinkedList<VALUE>::operator []( int atIndex )
{
    return getAt(atIndex);
}

template <typename VALUE >
inline VALUE & TELinkedList<VALUE>::operator []( LISTPOS atPosition )
{
    return valueAtPosition(atPosition);
}

template <typename VALUE >
inline const VALUE & TELinkedList<VALUE>::operator []( int atIndex ) const
{
    return getAt(atIndex);
}

template <typename VALUE >
inline const VALUE & TELinkedList<VALUE>::operator [] ( const LISTPOS atPosition ) const
{
    return valueAtPosition(atPosition);
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::isEmpty( void ) const	
{
    return mValueList.empty();
}

template <typename VALUE >
inline uint32_t TELinkedList<VALUE>::getSize( void ) const	
{
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::firstPosition( void ) const
{
    std::list<VALUE>::const_iterator cit = mValueList.begin();
    return iter(mValueList, cit);
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::lastPosition( void ) const
{
    std::list<VALUE>::const_iterator cit = mValueList.empty() == false ? --mValueList.end() : mValueList.end();
    return iter(mValueList, cit);
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::isStartPosition(const LISTPOS pos) const
{
    return (pos == mValueList.begin());
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::isLastPosition(const LISTPOS pos) const
{
    return ((mValueList.empty() == false) && (pos == --mValueList.end()));
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::invalidPosition(void) const
{
    return Constless::iter(mValueList, mValueList.end());
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::isInvalidPosition(const LISTPOS pos) const
{
    return (pos == mValueList.end());
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::isValidPosition(const LISTPOS pos) const
{
    return (pos != mValueList.end());
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::checkPosition(const LISTPOS pos) const
{
    std::list<VALUE>::const_iterator it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template <typename VALUE >
inline void TELinkedList<VALUE>::clear(void)
{
    mValueList.clear();
}

template <typename VALUE >
inline void TELinkedList<VALUE>::freeExtra(void)
{
    mValueList.clear();
}

template <typename VALUE >
inline void TELinkedList<VALUE>::release(void)
{
    mValueList.clear();
}

template <typename VALUE >
inline const VALUE & TELinkedList<VALUE>::getFirstEntry( void ) const
{
    ASSERT(mValueList.empty() == false);
    return mValueList.front();
}

template <typename VALUE >
inline VALUE& TELinkedList<VALUE>::getFirstEntry(void)
{
    ASSERT(mValueList.empty() == false);
    return mValueList.front();
}

template <typename VALUE >
inline const VALUE & TELinkedList<VALUE>::getLastEntry( void ) const	
{
    ASSERT(mValueList.empty() == false);
    return mValueList.back();
}

template <typename VALUE >
inline VALUE& TELinkedList<VALUE>::getLastEntry(void)
{
    ASSERT(mValueList.empty() == false);
    return mValueList.back();
}

template <typename VALUE >
inline const VALUE & TELinkedList<VALUE>::getNext(LISTPOS& IN OUT in_out_NextPosition) const
{
    return *in_out_NextPosition++;
}

template <typename VALUE >
inline VALUE& TELinkedList<VALUE>::getNext(LISTPOS& IN OUT in_out_NextPosition)
{
    LISTPOS pos = in_out_NextPosition++;
    return *pos;
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::nextPosition(LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return ++atPosition;
}

template <typename VALUE >
inline const VALUE & TELinkedList<VALUE>::getPrev(LISTPOS& IN OUT in_out_PrevPosition) const
{
    ASSERT(in_out_PrevPosition != mValueList.end());
    LISTPOS pos = in_out_PrevPosition;
    in_out_PrevPosition = in_out_PrevPosition == mValueList.begin() ? mValueList.end() : --in_out_PrevPosition;
    return *pos;
}

template <typename VALUE >
inline VALUE & TELinkedList<VALUE>::getPrev(LISTPOS& IN OUT in_out_PrevPosition)
{
    ASSERT(in_out_PrevPosition != mValueList.end());
    LISTPOS pos = in_out_PrevPosition;
    in_out_PrevPosition = in_out_PrevPosition == mValueList.begin() ? mValueList.end() : --in_out_PrevPosition;
    return *pos;
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::prevPosition(LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return (atPosition == mValueList.begin() ? invalidPosition() : --atPosition);
}

template <typename VALUE >
inline const VALUE & TELinkedList<VALUE>::valueAtPosition( LISTPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE >
inline VALUE & TELinkedList<VALUE>::valueAtPosition(LISTPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE >
inline const VALUE & TELinkedList<VALUE>::getAt(uint32_t index) const
{
    LISTPOS pos = getPosition(index);
    ASSERT(isValidPosition(pos));
    
    return *pos;
}

template <typename VALUE >
inline VALUE & TELinkedList<VALUE>::getAt(uint32_t index)
{
    LISTPOS pos = getPosition(index);
    ASSERT(isValidPosition(pos));

    return *pos;
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::nextEntry(LISTPOS & IN OUT in_out_NextPosition, VALUE & OUT out_NextValue) const
{
    bool result = false;
    ASSERT(in_out_NextPosition != mValueList.end());
    in_out_NextPosition = nextPosition(in_out_NextPosition);
    if (in_out_NextPosition != mValueList.end())
    {
        out_NextValue = *in_out_NextPosition;
        result = true;
    }

    return result;
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::prevEntry(LISTPOS& IN OUT in_out_PrevPosition, VALUE & OUT out_PrevValue) const
{
    bool result = false;
    ASSERT(in_out_PrevPosition != mValueList.end());
    in_out_NextPosition = prevPosition(in_out_PrevPosition);
    if (in_out_PrevPosition != mValueList.end())
    {
        out_PrevValue = *in_out_PrevPosition;
        result = true;
    }

    return result;
}

template <typename VALUE >
inline void TELinkedList<VALUE>::removeFirst( void )
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_front();
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::removeFirst(VALUE & OUT value)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        value = mValueList.front();
        mValueList.pop_front();
        result = true;
    }

    return result;
}

template <typename VALUE >
inline void TELinkedList<VALUE>::removeLast( void )
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_back();
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::removeLast(VALUE & value)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        value = mValueList.back();
        mValueList.pop_back();
        result = true;
    }

    return result;
}

template <typename VALUE >
inline void TELinkedList<VALUE>::pushFirst(const VALUE & newElement)
{
    mValueList.push_front(newElement);
}

template <typename VALUE >
inline void TELinkedList<VALUE>::pushFirst( VALUE && newElement )
{
    mValueList.push_front(std::move(newElement));
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::pushFirstIfUnique(const VALUE& newElement)
{
    bool result{ false };
    if (std::find(mValueList.begin(), mValueList.end(), newElement) == mValueList.end())
    {
        result = true;
        mValueList.push_front(newElement);
    }

    return result;
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::pushFirstIfUnique(VALUE&& newElement)
{
    bool result{ false };
    if (std::find(mValueList.begin(), mValueList.end(), newElement) == mValueList.end())
    {
        result = true;
        mValueList.push_front(std::move(newElement));
    }

    return result;
}

template <typename VALUE >
inline void TELinkedList<VALUE>::pushLast(const VALUE & newElement)
{
    mValueList.push_back(newElement);
}

template <typename VALUE >
inline void TELinkedList<VALUE>::pushLast(VALUE && newElement)
{
    mValueList.push_back(std::move(newElement));
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::pushLastIfUnique(const VALUE& newElement)
{
    bool result{ false };
    if (std::find(mValueList.rbegin(), mValueList.rend(), newElement) == mValueList.rend())
    {
        result = true;
        mValueList.push_back(newElement);
    }

    return result;
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::pushLastIfUnique(VALUE&& newElement)
{
    bool result{ false };
    if (std::find(mValueList.rbegin(), mValueList.rend(), newElement) == mValueList.rend())
    {
        result = true;
        mValueList.push_back(std::move(newElement));
    }

    return result;
}

template <typename VALUE >
inline VALUE TELinkedList<VALUE>::popFirst(void)
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.front();
    mValueList.pop_front();
    return result;
}

template <typename VALUE >
inline VALUE TELinkedList<VALUE>::popLast(void)
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.back();
    mValueList.pop_back();
    return result;
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::insertBefore(LISTPOS beforePosition, const VALUE & newElement)
{
    return mValueList.insert(beforePosition, std::move(newElement));
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::insertBefore(LISTPOS beforePosition, VALUE && newElement)
{
    return mValueList.insert(beforePosition, std::move(newElement));
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::insertAfter(LISTPOS afterPosition, const VALUE & newElement)
{
    if (afterPosition == mValueList.end())
    {
        mValueList.push_back(newElement);
        return lastPosition();
    }
    else
    {
        return mValueList.insert(++afterPosition, newElement);
    }
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::insertAfter(LISTPOS afterPosition, VALUE && newElement)
{
    if (afterPosition == mValueList.end())
    {
        mValueList.push_back(newElement);
        return lastPosition();
    }
    else
    {
        std::list<VALUE>::const_iterator cit(afterPosition);
        return mValueList.insert(++cit, std::move(newElement));
    }
}

template <typename VALUE >
inline void TELinkedList<VALUE>::setAt(LISTPOS atPosition, const VALUE & newValue)
{
    ASSERT(atPosition != mValueList.end());
    *atPosition = newValue;
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::removeAt(LISTPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::removeAt(LISTPOS atPosition, VALUE& OUT out_value)
{
    LISTPOS result = invalidPosition();

    if (atPosition != result)
    {
        out_value = *atPosition;
        result = mValueList.erase(atPosition);
    }

    return result;
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::removeEntry( const VALUE & removeElement )
{
    bool result = false;
    auto it = std::find(mValueList.begin(), mValueList.end(), removeElement);
    if (it != mValueList.end())
    {
        mValueList.erase(it);
        result = true;
    }

    return result;
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::removeEntry(const VALUE & removeElement, TELinkedList<VALUE>::LISTPOS searchAfter /*= nullptr*/)
{
    bool result = false;
    LISTPOS end = invalidPosition();
    if (searchAfter != end)
    {
        auto it = std::find(++searchAfter, end, removeElement);
        if (it != mValueList.end())
        {
            mValueList.erase(it);
            result = true;
        }
    }

    return result;
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::find(const VALUE& searchValue) const
{
    auto it = std::find(mValueList.begin(), mValueList.end(), searchValue);
    return Constless::iter(mValueList, it);
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::find(const VALUE & searchValue, TELinkedList<VALUE>::LISTPOS searchAfter ) const
{
    LISTPOS end = invalidPosition();
    return (searchAfter != end ? static_cast<LISTPOS>(std::find(++searchAfter, end, searchValue)) : end);
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::contains(const VALUE& elemSearch) const
{
    return contains(elemSearch, firstPosition());
}

template <typename VALUE >
inline bool TELinkedList<VALUE>::contains(const VALUE& elemSearch, LISTPOS startAt) const
{
    return (startAt != mValueList.end() ? std::find(startAt, invalidPosition(), elemSearch) != mValueList.end() : false);
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::findIndex(uint32_t index) const
{
    return Constless::iter(mValueList, index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() + index : mValueList.end());
}

template <typename VALUE >
inline int TELinkedList<VALUE>::makeIndex(LISTPOS atPosition) const
{
    int result  = 0;
    LISTPOS pos = mValueList.begin();
    LISTPOS end = mValueList.end();
    for (; (pos != end) && (pos != atPosition); ++pos, ++result)
        ;

    return (pos != end ? result : NECommon::INVALID_INDEX);
}

template <typename VALUE >
inline void TELinkedList<VALUE>::merge(const TELinkedList<VALUE>& source)
{
    mValueList.merge(source.mValueList);
}

template <typename VALUE >
inline void TELinkedList<VALUE>::merge(TELinkedList<VALUE>&& source)
{
    mValueList.merge(std::move(source.mValueList));
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::getPosition(uint32_t index) const
{
    typename std::list<VALUE>::const_iterator pos = index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() : mValueList.end();
    uint32_t count = index + 1;
    for (uint32_t i = 1; i < count; ++i)
        ++pos;

    return Constless::iter(mValueList, pos);
}

template <typename VALUE >
inline typename TELinkedList<VALUE>::LISTPOS TELinkedList<VALUE>::getPosition(uint32_t index)
{
    return static_cast<const TELinkedList<VALUE> *>(this)->getPosition(index);
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE> 
inline const IEInStream & operator >> ( const IEInStream & stream, TELinkedList<VALUE> & input )
{
    input.clear();

    uint32_t size = 0;
    stream >> size;
    input.mValueList.resize(size);
    for (auto& elem : input.mValueList)
    {
        stream >> elem;
    }

    return stream;
}

template <typename VALUE> 
inline IEOutStream & operator << ( IEOutStream & stream, const TELinkedList<VALUE> & output )
{
    uint32_t size = output.getSize();
    stream << size;

    for (const auto& elem : output.mValueList)
    {
        stream << elem;
    }

    return stream;
}

#endif  // AREG_BASE_TELINKEDLIST_HPP
