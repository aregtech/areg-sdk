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
// TELinkedList<VALUE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Bi-direction Linked List class template access, where insert and 
 *          remove by Position operations are fast. Searching and accessing by 
 *          index are slower.
 *
 *          Blocks in Linked List are linked bi-directional, where each Block
 *          knows the address of next and previous blocks. Linked List does not 
 *          guaranty unique values.  The type VALUE in Linked List should have 
 *          at least default constructor and valid assigning operator.
 *          By default, const VALUE & and VALUE are equal.
 * 
 *          The Linked List is not thread safe and data access should be  synchronized manually.
 *
 * \tparam  VALUE       the type of stored items. Either should be 
 *                      primitive or should have default constructor 
 *                      and valid assigning operator. Also, should be 
 *                      possible to convert to type const VALUE &.
 * \tparam  const VALUE &  By default same as VALUE, but can be any other
 *                      type, which is converted from type VALUE.
 **/
template <typename VALUE, class Implement = TEListImpl<VALUE>> 
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
     * \brief   Default constructor. Creates empty linked list
     **/
    TELinkedList( void ) = default;
    
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TELinkedList( const TELinkedList<VALUE, Implement> & src ) = default;
    
    /**
     * \brief   Move constructor.
     * \param   src     The source to copy data.
     **/
    TELinkedList( TELinkedList<VALUE, Implement> && src ) noexcept = default;

    /**
     * \brief   Destructor
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
     * \brief	Assigning operator, assigns elements of src to the Linked List
     * \param	src	Source Linked List to get elements.
     **/
    TELinkedList<VALUE, Implement> & operator = ( const TELinkedList<VALUE, Implement> & src );

    /**
     * \brief	Move operator, moves elements of src to the Linked List
     * \param	src	Source Linked List to get elements.
     **/
    TELinkedList<VALUE, Implement> & operator = ( TELinkedList<VALUE, Implement> && src ) noexcept;

    /**
     * \brief   Checks equality of 2 linked-list objects, and returns true if they are equal.
     *          There should be possible to compare VALUE types entries of linked list.
     * \param   other   The linked-list object to compare
     **/
    bool operator == ( const TELinkedList<VALUE, Implement> & other ) const;

    /**
     * \brief   Checks inequality of 2 linked-list objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE types entries of linked list.
     * \param   other   The linked-list object to compare
     **/
    bool operator != ( const TELinkedList<VALUE, Implement> & other ) const;

    /**
     * \brief   Subscript operator. Returns reference to value of element 
     *          by given valid index. The index should be valid, otherwise 
     *          assertion raised. May be used on either the right (r-value)
     *          or the left (l-value) of an assignment statement.
     * \param   atIndex     The valid zero-based index.
     * \return  Returns Value of Element at given index, the value can be modified
     **/
    inline VALUE & operator [] ( int atIndex );
    /**
     * \brief   Subscript operator. Returns reference to value of element 
     *          by given valid Position. May be used on either the right 
     *          (r-value) or the left (l-value) of an assignment statement.
     * \param   atPosition  The valid Linked List position.
     * \return  Returns Value of Element at given Linked List position, the value can be modified
     **/
    inline VALUE & operator [] ( LISTPOS atPosition );

    /**
     * \brief   Subscript operator. Returns value of element by given 
     *          valid index. The index should be valid, otherwise 
     *          assertion raised. May be used only on the right.
     * \param   atIndex     The valid zero-based index.
     * \return  Returns Value of Element at given index, the value should not be modified
     **/
    inline const VALUE & operator [] ( int atIndex ) const;
    /**
     * \brief   Subscript operator. Returns value of element by given 
     *          valid Position. May be used only on the right.
     * \param   atPosition  The valid Linked List position.
     * \return  Returns Value of Element at given Linked List position, the value can be modified
     **/
    inline const VALUE & operator [] ( const LISTPOS atPosition ) const;

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
    template<typename VALUE, class Implement>
    friend const IEInStream & operator >> ( const IEInStream & stream, TELinkedList<VALUE, Implement> & input );
    /**
     * \brief   Writes to the stream Linked List values.
     *          The values will be written to the stream starting from head position.
     *          There should be possibility to stream every value of Linked List and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Linked List object to read out values.
     **/
    template<typename VALUE, class Implement>
    friend IEOutStream & operator << ( IEOutStream & stream, const TELinkedList<VALUE, Implement> & output );

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
    inline uint32_t getSize( void ) const;

    /**
     * \brief	Returns the first position of the hash map, which is not invalid if
     *          the hash map is not empty. Otherwise, it is equal to the last position
     *          and not valid.
     **/
    inline LISTPOS firstPosition(void) const;

    /**
     * \brief	Returns the last position of the hash map, which is always invalid.
     **/
    inline LISTPOS lastPosition(void) const;

    /**
     * \brief   Returns invalid position object.
     **/
    inline LISTPOS invalidPosition(void) const;

    /**
     * \brief   Returns true if specified position pointing start of the linked list.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the linked list.
     **/
    inline bool isStartPosition(const LISTPOS& pos) const;

    /**
     * \brief   Returns true if specified position pointing start of the linked list.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the linked list.
     **/
    inline bool isLastPosition(const LISTPOS& pos) const;

    /**
     * \brief   Returns true if specified position reached the end of the hash map.
     * \param   pos     The position to check.
     * \return  Returns true if specified position reached the end of the hash map.
     **/
    inline bool isInvalidPosition(const LISTPOS& pos) const;

    /**
     * \brief   Returns true if specified position is not pointing the end of the hash map.
     *          NOTE:   This method does not ensure that the position object is pointing
     *                  to the valid entry in the hash map. To do so, use 'checkPos' method.
     * \param pos       The position to check.
     * \return Returns true if specified position is not pointing the end of the hash map.
     */
    inline bool isValidPosition(const LISTPOS& pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the hash map.
     *          The duration of checkup depends on the location of the position in the map and may run long.
     * \param pos       The position to check.
     * \return  Returns true if specified position points to the valid entry in the hash map.
     */
    inline bool checkPosition(const LISTPOS& pos) const;

/************************************************************************/
// Operations
/************************************************************************/

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
     * \brief	Returns given position value and on exit position of next element in Linked List container.
     *          On input, the value of 'in_out_NextPosition' should be valid (for example, should be equal to head position).
     *          Otherwise assertion is raised.
     * \param	in_out_NextPosition On input, this should be valid position of element in Linked List container.
     *                              On output, this contains position of next element in Linked List container
     *                              or nullptr (INVALID_POSITION) if passed position on input is position of tail element
     * \return	Returns value of element at the given position.
     **/
    inline const VALUE & getNext( LISTPOS & in_out_NextPosition ) const;
    inline VALUE& getNext(LISTPOS& in_out_NextPosition);

    /**
     * \brief	Returns position of next element in Linked List
     * \param	atPosition	Actual position to get the next position
     * \return	If element at given position has next element, returns position of next element in Linked List.
     *          Otherwise it returns nullptr
     **/
    inline LISTPOS nextPosition( LISTPOS atPosition ) const;

    /**
     * \brief	Returns given position value and on exit position of previous element in Linked List container.
     *          On input, the value of 'in_out_PrevPosition' should be valid (for example, should be equal to tail position).
     *          Otherwise assertion is raised.
     * \param	in_out_PrevPosition On input, this should be valid position of element in Linked List container.
     *                              On output, this contains position of previous element in Linked List container
     *                              or nullptr (INVALID_POSITION) if passed position on input is position of head element
     * \return	Returns value of element at the given position.
     **/
    inline const VALUE & getPrev( LISTPOS & in_out_PrevPosition ) const;
    inline VALUE& getPrev(LISTPOS& in_out_PrevPosition);

    /**
     * \brief	Returns position of previous element in Linked List
     * \param	atPosition	Actual position to get the previous position
     * \return	If element at given position has previous element, returns position of previous element in Linked List.
     *          Otherwise it returns nullptr
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
    inline const VALUE & valueAtPosition( LISTPOS atPosition ) const;
    /**
     * \brief   Returns value of element at the give position.
     *          On input, the value of in_pos should be valid. 
     *          Otherwise assertion is raised.
     *          Returned value can be modified.
     * \param   atPosition  The valid position in Linked List
     * \return  Returns Value at given position. 
     *          The returned value can be modified
     **/
    inline VALUE & valueAtPosition( LISTPOS atPosition );

    /**
     * \brief	Returns the instance of element by give index. The index of head element is 0.
     *          The index must not be more than GetSize() - 1.
     * \param	index	The index of element to get value
     * \return	The instance of element by give index.
     **/
    VALUE & getAt( uint32_t index );

    /**
     * \brief	Returns the value of element by give index. The index of head element is 0.
     *          The index must not be more than GetSize() - 1.
     * \param	index	The index of element to get value
     * \return	The value of element by give index.
     **/
    const VALUE & getAt(uint32_t index ) const;

    /**
     * \brief	On exit, returns position and value of next element in Linked List container.
     *          On input, the value of 'in_out_NextPosition' should be valid (for example, should be equal to head position).
     *          Otherwise assertion is raised.
     * \param	in_out_NextPosition	On input, this should be valid position of element in Linked List container.
     *                              On output, this contains position of next element in Linked List container 
     *                              or nullptr (INVALID_POSITION) if passed position on input is position of tail element
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
     *                              or nullptr (INVALID_POSITION) if passed position on input is position of head element
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
    void removeFirst( void );

    /**
     * \brief   Removes the head element from the linked list. On output, the 'value' contains
     *          data of removed element. Function returns true if linked list was not empty,
     *          succeeded to remove head and the 'value' contains the data of removed element.
     * \param   value [out]     If succeeded to remove head element, on output it contains 
     *                          the data of remove element.
     * \return  Returns true if succeeded to remove head. It as well indicates that the 'value'
     *          contains data of removed element.
     */
    bool removeFirst( VALUE & value );

    /**
     * \brief   Removes tail element from Linked List and returns value or removed element.
     *          On call the Linked List should not be empty. Otherwise assertion is raised.
     * \return  Returns value of removed element.
     **/
    void removeLast( void );

    /**
     * \brief   Removes the tails element from the linked list. On output, the 'value' contains
     *          data of removed element. Function returns true if linked list was not empty,
     *          succeeded to remove tails and the 'value' contains the data of removed element.
     * \param   value [out]     If succeeded to remove head element, on output it contains
     *                          the data of remove element.
     * \return  Returns true if succeeded to remove tail. It as well indicates that the 'value'
     *          contains data of removed element.
     */
    bool removeLast(VALUE& value);

    /**
     * \brief   Add element to head of Linked List and returns position new head element.
     * \param   newElement   New head element to add in Linked List
     **/
    void pushFirst( const VALUE & newElement );
    void pushFirst( VALUE && newElement );

    /**
     * \brief   Adds new entry at the head of linked list if it is not existing.
     *          If the entry with specified key exists, the entry is not added.
     *          The method returns true if new element is added.
     * \param   newElement   The entry to add to the head of linked list if does not exit.
     * \return  Returns true new element is added. Otherwise, returns false.
     */
    bool pushFirstIfNew(const VALUE& newElement);
    bool pushFirstIfNew(VALUE&& newElement);

    /**
     * \brief   Add element to tail of Linked List and returns position new tail element.
     * \param   newElement   New tail element to add in Linked List
     * \return  Returns position new tail element in Linked List.
     **/
    void pushLast( const VALUE & newElement );
    void pushLast( VALUE && newElement );

    /**
     * \brief   Adds new entry at the tail of linked list if it is not existing.
     *          If the entry with specified key exists, the entry is not added.
     *          The method returns true if new element is added.
     * \param   newElement   The entry to add to the tail of linked list if does not exit.
     * \return  Returns true new element is added. Otherwise, returns false.
     */
    bool pushLastIfNew(const VALUE& newElement);
    bool pushLastIfNew(VALUE&& newElement);

    /**
     * \brief   Pops the element at the head of linked list and returns the stored value.
     *          The linked list must not be empty.
     * \return  The popped out element from the header of the linked list.
     */
    VALUE popFirst(void);

    /**
     * \brief   Pops the element at the tails of linked list and returns the stored value.
     *          The linked list must not be empty.
     * \return  The popped out element from the tail of the linked list.
     */
    VALUE popLast(void);

    /**
     * \brief	Inserts new element before given position. 
     *          If given position is head element, it will add new head element
     * \param	beforePosition  The Linked List element position before new element should be inserted
     * \param	newElement	    Value of new element to insert
     * \return	Returns position of new inserted element.
     **/
    LISTPOS insertBefore( LISTPOS beforePosition, const VALUE & newElement );
    LISTPOS insertBefore(LISTPOS beforePosition, VALUE && newElement);

    /**
     * \brief	Inserts new element after given position. 
     *          If given position is tail element, it will add new tail element
     * \param	afterPosition   The Linked List element position after new element should be inserted
     * \param	newElement	    Value of new element to insert
     * \return	Returns position of new inserted element.
     **/
    LISTPOS insertAfter(LISTPOS afterPosition, const VALUE & newElement);
    LISTPOS insertAfter(LISTPOS afterPosition, VALUE && newElement);

    /**
     * \brief	Sets new value at give position and returns old value of same element.
     * \param	atPosition	The Linked List element position to change value
     * \param	newValue	New Value to change
     **/
    inline void setAt( LISTPOS atPosition, const VALUE & newValue );

    /**
     * \brief	Removes element at given position and returns value of removed element.
     * \param	atPosition  Position of element to remove from Linked List. 
     *                      It should be valid position, otherwise assertion is raised.
     * \brief   Returns position following nest to removed position or invalid position
     *          if last entry was removed.
     **/
    LISTPOS removeAt( LISTPOS atPosition );

    /**
     * \brief	Removes element at given position and on output returns value of removed element.
     * \param	atPosition  On input, position of element to remove from Linked List.
     *                      It should be valid position, otherwise assertion is raised.
     * \param   out_Value   On output, it contains value of removed element
     * \brief   Returns position following nest to removed position or invalid position
     *          if last entry was removed.
     **/
    LISTPOS removeAt( LISTPOS atPosition, VALUE & out_Value );

    /**
     * \brief   Removes all element in Linked List. After call, Linked List is empty.
     **/
    void removeAll( void );

    /**
     * \brief	Searches and removes first match of given element from Linked List.
     *          If 'searchAfter' is nullptr, it will start searching element from head.
     *          And returns true if element was found and successfully removed from Linked List.
     * \param	removeElement	Element to search and remove from Linked List
     * \return	Returns true if element was found and successfully removed from linked list.
     **/
    bool removeEntry( const VALUE & removeElement );

    /**
     * \brief	Searches and removes first match of given element from Linked List.
     *          If 'searchAfter' is nullptr, it will start searching element from head.
     *          And returns true if element was found and successfully removed from Linked List.
     * \param	removeElement	Element to search and remove from Linked List
     * \param	searchAfter	    Position at which starts searching, the searching done by moving to next element
     * \return	Returns true if element was found and successfully removed from linked list.
     **/
    bool removeEntry(const VALUE& removeElement, LISTPOS searchAfter);

    /**
     * \brief	Searches position of element by given value. If searchAfter is valid, the searching will be started
     *          from given position and will move to next element.
     * \param	searchValue	Value of element to search
     * \param	searchAfter	If valid position, the searching starts from next element specified by position. 
     *                      If invalid position, the searching starts from the beginning of the linked list.
     * \return	Returns position of the element in the linked list.
     **/
    LISTPOS find(const VALUE& searchValue) const;
    LISTPOS find( const VALUE & searchValue, LISTPOS searchAfter ) const;

    /**
     * \brief   Searches element Value in the linked list and returns true if there is any.
     * \param   Value   The value to search.
     * \return  Returns true if there is an element Value in the list.
     */
    bool contains(const VALUE& Value) const;

    /**
     * \brief	Returns position of element by given index. The index of head element is 0.
     *          The index of last element (tail element) is GetSize() - 1.
     *          If invalid index is passed, function returns nullptr
     * \param	index	The index of element in Linked List to get position
     * \return	Returns position of element if index is valid, i.e. more or equal to 0 and less than GetSize().
     *          Otherwise returns nullptr (INVALID_POSITION)
     **/
    LISTPOS findIndex( uint32_t index ) const;

    /**
     * \brief	Returns valid index of element by given valid position.
     *          Otherwise returns -1.
     * \param	atPosition  The position of element, which index should be returned.
     * \return	The index of element by given position
     **/
    int makeIndex( LISTPOS atPosition ) const;

    /**
     * \brief   Extracts elements from the given source and inserts into the linked list.
     *          No elements are copied. The container other becomes empty after the operation.
     * \param   source  The source of linked list to merge.
     */
    inline void merge(const TELinkedList<VALUE, Implement> & source);
    inline void merge(TELinkedList<VALUE, Implement> && source);

protected:

    /**
     * \brief   Called when comparing 2 values of element.
     *          Overwrite method when need to change comparison.
     * \param   value1  Value on left side to compare.
     * \param   value2  Value on right side to compare.
     * \return  If function returns true, 2 values are equal.
     *          Otherwise, they are not equal.
     **/
    inline bool isEqualValues( const VALUE & value1, const VALUE & value2) const;

    /**
     * \brief   Returns the position of the element at the given index.
     *          The value of returned position cannot be modified.
     * \param   index   The index of the element to return position.
     * \return  Returns the position of the element at the given index.
     *          The value of returned position cannot be modified.
     **/
    inline LISTPOS getPosition( uint32_t index ) const;

    /**
     * \brief   Returns the position of the element at the given index.
     * \param   index   The index of the element to return position.
     * \return  Returns the position of the element at the given index.
     **/
    inline LISTPOS getPosition(uint32_t index);

//////////////////////////////////////////////////////////////////////////
// Member Variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The linked list object
     **/
    std::list<VALUE>    mValueList;
    /**
     * \brief   Instance of object that compares values.
     **/
    Implement   mImplement;
};

//////////////////////////////////////////////////////////////////////////
// TELinkedList<VALUE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
TELinkedList<VALUE, Implement>& TELinkedList<VALUE, Implement>::operator = (const TELinkedList<VALUE, Implement>& src)
{
    mValueList = src.mValueList;
    return (*this);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
TELinkedList<VALUE, Implement>& TELinkedList<VALUE, Implement>::operator = (TELinkedList<VALUE, Implement>&& src) noexcept
{
    mValueList = std::move(src.mValueList);
    return (*this);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::operator == (const TELinkedList<VALUE, Implement>& other) const
{
    return (mValueList == other.mValueList);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::operator != (const TELinkedList<VALUE, Implement>& other) const
{
    return (mValueList != other.mValueList);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline VALUE & TELinkedList<VALUE, Implement>::operator []( int atIndex )
{
    return getAt(atIndex);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline VALUE & TELinkedList<VALUE, Implement>::operator []( LISTPOS atPosition )
{
    return valueAtPosition(atPosition);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline const VALUE & TELinkedList<VALUE, Implement>::operator []( int atIndex ) const
{
    return getAt(atIndex);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline const VALUE & TELinkedList<VALUE, Implement>::operator [] ( const LISTPOS atPosition ) const
{
    return valueAtPosition(atPosition);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline bool TELinkedList<VALUE, Implement>::isEmpty( void ) const	
{
    return mValueList.empty();
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline uint32_t TELinkedList<VALUE, Implement>::getSize( void ) const	
{
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline bool TELinkedList<VALUE, Implement>::isValidPosition(const LISTPOS& pos) const
{
    return (pos != mValueList.end());
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::firstPosition( void ) const
{
    std::list<VALUE>::const_iterator cit = mValueList.begin();
    return iter(mValueList, cit);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::lastPosition( void ) const
{
    std::list<VALUE>::const_iterator cit = mValueList.empty() == false ? --mValueList.end() : mValueList.end();
    return iter(mValueList, cit);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline const VALUE & TELinkedList<VALUE, Implement>::getFirstEntry( void ) const
{
    ASSERT(mValueList.empty() == false);
    return mValueList.front();
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline VALUE& TELinkedList<VALUE, Implement>::getFirstEntry(void)
{
    ASSERT(mValueList.empty() == false);
    return mValueList.front();
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline const VALUE & TELinkedList<VALUE, Implement>::getLastEntry( void ) const	
{
    ASSERT(mValueList.empty() == false);
    return mValueList.back();
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline VALUE& TELinkedList<VALUE, Implement>::getLastEntry(void)
{
    ASSERT(mValueList.empty() == false);
    return mValueList.back();
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline const VALUE & TELinkedList<VALUE, Implement>::getNext(LISTPOS &in_out_NextPosition) const
{
    return *in_out_NextPosition++;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline VALUE& TELinkedList<VALUE, Implement>::getNext(LISTPOS& in_out_NextPosition)
{
    LISTPOS pos = in_out_NextPosition++;
    return *pos;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::nextPosition(LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return ++atPosition;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline bool TELinkedList<VALUE, Implement>::isInvalidPosition(const LISTPOS& pos) const
{
    return (pos == mValueList.end());
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline bool TELinkedList<VALUE, Implement>::isStartPosition(const LISTPOS& pos) const
{
    return (pos == mValueList.begin());
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline bool TELinkedList<VALUE, Implement>::isLastPosition(const LISTPOS& pos) const
{
    return ((mValueList.empty() == false) && (pos == --mValueList.end()));
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline bool TELinkedList<VALUE, Implement>::checkPosition(const LISTPOS& pos) const
{
    std::list<VALUE>::const_iterator it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::invalidPosition(void) const
{
    return Constless::iter(mValueList, mValueList.end());
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline const VALUE & TELinkedList<VALUE, Implement>::getPrev(LISTPOS & in_out_PrevPosition) const
{
    ASSERT(in_out_PrevPosition != mValueList.end());
    LISTPOS pos = in_out_PrevPosition;
    in_out_PrevPosition = in_out_PrevPosition == mValueList.begin() ? mValueList.end() : --in_out_PrevPosition;
    return *pos;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline VALUE & TELinkedList<VALUE, Implement>::getPrev(LISTPOS& in_out_PrevPosition)
{
    ASSERT(in_out_PrevPosition != mValueList.end());
    LISTPOS pos = in_out_PrevPosition;
    in_out_PrevPosition = in_out_PrevPosition == mValueList.begin() ? mValueList.end() : --in_out_PrevPosition;
    return *pos;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::prevPosition(LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return (atPosition == mValueList.begin() ? invalidPosition() : --atPosition);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline const VALUE & TELinkedList<VALUE, Implement>::valueAtPosition( LISTPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline VALUE & TELinkedList<VALUE, Implement>::valueAtPosition(LISTPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
const VALUE & TELinkedList<VALUE, Implement>::getAt(uint32_t index) const
{
    LISTPOS pos = getPosition(index);
    ASSERT(isValidPosition(pos));
    
    return *pos;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
VALUE & TELinkedList<VALUE, Implement>::getAt(uint32_t index)
{
    LISTPOS pos = getPosition(index);
    ASSERT(isValidPosition(pos));

    return *pos;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::nextEntry(LISTPOS & in_out_NextPosition, VALUE & out_NextValue) const
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

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::prevEntry(LISTPOS &in_out_PrevPosition, VALUE & out_PrevValue) const
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

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
void TELinkedList<VALUE, Implement>::removeFirst( void )
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_front();
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::removeFirst(VALUE & value)
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

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
void TELinkedList<VALUE, Implement>::removeLast( void )
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_back();
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::removeLast(VALUE & value)
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

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
void TELinkedList<VALUE, Implement>::pushFirst(const VALUE & newElement)
{
    mValueList.push_front(newElement);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
void TELinkedList<VALUE, Implement>::pushFirst( VALUE && newElement )
{
    mValueList.push_front(std::move(newElement));
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::pushFirstIfNew(const VALUE& newElement)
{
    bool add = true;
    for (const VALUE& value : mValueList)
    {
        if (isEqualValues(value, newElement))
        {
            add = false;
            break;
        }
    }

    if (add)
    {
        mValueList.push_front(newElement);
    }

    return add;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::pushFirstIfNew(VALUE&& newElement)
{
    bool add = true;
    for (const VALUE& value : mValueList)
    {
        if (isEqualValues(value, newElement))
        {
            add = false;
            break;
        }
    }

    if (add)
    {
        mValueList.push_front(std::move(newElement));
    }

    return add;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
void TELinkedList<VALUE, Implement>::pushLast(const VALUE & newElement)
{
    mValueList.push_back(newElement);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
void TELinkedList<VALUE, Implement>::pushLast(VALUE && newElement)
{
    mValueList.push_back(std::move(newElement));
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::pushLastIfNew(const VALUE& newElement)
{
    bool add = true;
    for (const VALUE& value : mValueList)
    {
        if (isEqualValues(value, newElement))
        {
            add = false;
            break;
        }
    }

    if (add)
    {
        mValueList.push_back(newElement);
    }

    return add;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::pushLastIfNew(VALUE&& newElement)
{
    bool add = true;
    for (const VALUE& value : mValueList)
    {
        if (isEqualValues(value, newElement))
        {
            add = false;
            break;
        }
    }

    if (add)
    {
        mValueList.push_back(std::move(newElement));
    }

    return add;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
VALUE TELinkedList<VALUE, Implement>::popFirst(void)
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.front();
    mValueList.pop_front();
    return result;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
VALUE TELinkedList<VALUE, Implement>::popLast(void)
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.back();
    mValueList.pop_back();
    return result;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::insertBefore(LISTPOS beforePosition, const VALUE & newElement)
{
    ASSERT(beforePosition != mValueList.end());
    LISTPOS result = mValueList.end();
    if (beforePosition == mValueList.begin())
    {
        mValueList.push_front(newElement);
        result = mValueList.begin();
    }
    else
    {
        result = mValueList.insert(--beforePosition, newElement);
    }

    return result;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::insertBefore(LISTPOS beforePosition, VALUE && newElement)
{
    ASSERT(beforePosition != mValueList.end());
    LISTPOS result = mValueList.end();
    if (beforePosition == mValueList.begin())
    {
        mValueList.push_front(std::move(newElement));
        result = mValueList.begin();
    }
    else
    {
        result = mValueList.insert(--beforePosition, std::move(newElement));
    }

    return result;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::insertAfter(LISTPOS afterPosition, const VALUE & newElement)
{
    ASSERT(afterPosition != mValueList.end());
    return mValueList.insert(afterPosition, newElement);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::insertAfter(LISTPOS afterPosition, VALUE && newElement)
{
    ASSERT(afterPosition != mValueList.end());
    return mValueList.insert(afterPosition, std::move(newElement));
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline void TELinkedList<VALUE, Implement>::setAt(LISTPOS atPosition, const VALUE & newValue)
{
    ASSERT(atPosition != mValueList.end());
    *atPosition = newValue;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
void TELinkedList<VALUE, Implement>::removeAll( void )
{
    mValueList.clear();
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::removeAt(LISTPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::removeAt(LISTPOS atPosition, VALUE &out_value)
{
    LISTPOS result = invalidPosition();

    if (atPosition != mValueList.end())
    {
        out_value = *atPosition;
        result = mValueList.erase(atPosition);
    }

    return result;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::removeEntry( const VALUE & removeElement )
{
    return removeEntry(removeElement, invalidPosition());
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::removeEntry(const VALUE & removeElement, LISTPOS searchAfter /*= nullptr*/)
{
    bool result = false;
    LISTPOS pos = searchAfter == invalidPosition() ? mValueList.begin() : ++ searchAfter;
    for ( ; pos != mValueList.end(); ++pos)
    {
        if (isEqualValues(*pos, removeElement))
        {
            mValueList.erase(pos);
            result = true;
            break;
        }
    }

    return result;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::find(const VALUE& searchValue) const
{
    return find(searchValue, invalidPosition());
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::find(const VALUE & searchValue, LISTPOS searchAfter ) const
{
    LISTPOS end = invalidPosition();
    LISTPOS pos = searchAfter == end ? firstPosition() : ++searchAfter;
    for (; pos != end; ++pos)
    {
        if (isEqualValues(*pos, searchValue))
            break;
    }

    return pos;
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
bool TELinkedList<VALUE, Implement>::contains(const VALUE & Value) const
{
    typename std::list<VALUE>::const_iterator end = mValueList.end();
    typename std::list<VALUE>::const_iterator pos = mValueList.begin();
    for (; pos != end; ++pos)
    {
        if (isEqualValues(*pos, Value))
            break;
    }

    return (pos != end);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::findIndex(uint32_t index) const
{
    typename std::list<VALUE>::const_iterator end = mValueList.end();
    typename std::list<VALUE>::const_iterator pos = mValueList.begin();
    for (; pos != end; ++pos)
    {
        if (isEqualValues(*pos, Value))
            break;
    }

    return (pos != end);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
int TELinkedList<VALUE, Implement>::makeIndex(LISTPOS atPosition) const
{
    int result  = 0;
    LISTPOS pos = mValueList.begin();
    LISTPOS end = mValueList.end();
    for (; (pos != end) && (pos != atPosition); ++pos, ++result)
        ;

    return (pos != end ? result : NECommon::INVALID_INDEX);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline void TELinkedList<VALUE, Implement>::merge(const TELinkedList<VALUE, Implement>& source)
{
    mValueList.merge(source.mValueList);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline void TELinkedList<VALUE, Implement>::merge(TELinkedList<VALUE, Implement>&& source)
{
    mValueList.merge(std::move(source.mValueList));
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline bool TELinkedList<VALUE, Implement>::isEqualValues(const VALUE & value1, const VALUE & value2) const
{
    return mImplement.implEqualValues(value1, value2);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::getPosition(uint32_t index) const
{
    typename std::list<VALUE>::const_iterator pos = index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() : mValueList.end();
    uint32_t count = index + 1;
    for (uint32_t i = 1; i < count; ++i)
        ++pos;

    return Constless::iter(mValueList, pos);
}

template <typename VALUE, class Implement /* = TEListImpl<VALUE> */>
inline typename TELinkedList<VALUE, Implement>::LISTPOS TELinkedList<VALUE, Implement>::getPosition(uint32_t index)
{
    return static_cast<const TELinkedList<VALUE, Implement> *>(this)->getPosition(index);
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, class Implement> 
const IEInStream & operator >> ( const IEInStream & stream, TELinkedList<VALUE, Implement> & input )
{
    input.removeAll();

    uint32_t size = 0;
    stream >> size;
    for (uint32_t i = 0; i < size; ++ i)
    {
        VALUE newValue;
        stream >> newValue;
        input.mValueList.push_back(newValue);
    }

    return stream;
}

template <typename VALUE, class Implement> 
IEOutStream & operator << ( IEOutStream & stream, const TELinkedList<VALUE, Implement> & output )
{
    uint32_t size = output.getSize();
    stream << size;

    typename std::list<VALUE>::const_iterator end = output.mValueList.end();
    typename std::list<VALUE>::const_iterator cit = output.mValueList.begin();
    for (; cit != end; ++cit)
    {
        stream << static_cast<const VALUE &>(*cit);
    }

    return stream;
}
