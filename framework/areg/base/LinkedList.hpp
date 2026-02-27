#ifndef AREG_BASE_LINKEDLIST_HPP
#define AREG_BASE_LINKEDLIST_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/LinkedList.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Linked List class template.
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
#include "areg/base/TemplateBase.hpp"
#include "areg/base/IOStream.hpp"

#include <algorithm>

#include <algorithm>
#include <list>

//////////////////////////////////////////////////////////////////////////
// LinkedList<VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Bi-directional Linked List class template to access, insert and 
 *          remove entries by position, where is next and previous entry
 *          at the given position is accessed fast.
 *
 *          The type VALUE should have at least default constructor, applicable
 *          comparing and assigning operators. The LinkedList object is not
 *          thread safe and data access should be synchronized manually.
 *
 * \tparam  VALUE   The type of stored elements should be either primitive or have
 *                  default constructor, applicable comparing and assigning operators.
 **/
template <typename VALUE> 
/**
 * \brief   Bi-directional Linked List class template to access, insert and remove entries by
 *          position, where the next and previous entry at the given position is accessed fast. The
 *          type VALUE should have at least a default constructor, applicable comparing and
 *          assigning operators. The LinkedList object is not thread safe and data access should be
 *          synchronized manually.
 **/
class LinkedList  : private Constless<std::list<VALUE>>
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
    LinkedList() = default;
    
    LinkedList( const LinkedList<VALUE> & src ) = default;
    
    LinkedList( LinkedList<VALUE> && src ) noexcept = default;

    /**
     * \brief   Constructs a linked list from an array of objects.
     *
     * \param   list        The array of entries to copy.
     * \param   count       The number of entries in the array.
     **/
    LinkedList(const VALUE* list, uint32_t count);

    /**
     * \brief   Destructor.
     **/
    ~LinkedList() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid zero-based
     *          index. May be used on either the right (r-value) or the left (l-value) of an
     *          assignment statement.
     **/
    inline VALUE& operator [] (uint32_t atIndex);
    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid zero-based
     *          index. The index should be valid number between 0 and (mSize -1). May be used on the
     *          right (r-value).
     **/
    inline const VALUE& operator [] (uint32_t atIndex) const;

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid position
     *          value. May be used on either the right (r-value) or the left (l-value) of an
     *          assignment statement.
     **/
    inline VALUE& operator [] (LISTPOS atPosition);
    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid position
     *          value. May be used on the right (r-value).
     **/
    inline const VALUE& operator [] (const LISTPOS atPosition) const;

    /**
     * \brief   Assigning operator. Copies all values from given source. If linked list previously
     *          had values, they will be removed and new values from source linked list will be set
     *          in the same sequence as they are present in the source.
     *
     * \param   src     The source of linked list values.
     **/
    inline LinkedList<VALUE> & operator = ( const LinkedList<VALUE> & src );

    /**
     * \brief   Move operator. Moves all values from given source. If linked list previously had
     *          values, they will be removed and new values from source linked list will be set in
     *          the same sequence as they are present in the source.
     *
     * \param   src     The source of linked list values.
     * \note    Move overload. Takes ownership of the source list.
     **/
    inline LinkedList<VALUE> & operator = ( LinkedList<VALUE> && src ) noexcept;

    /**
     * \brief   Checks equality of 2 linked list objects, and returns true if they are equal. There
     *          should be possible to compare VALUE type entries in the linked list.
     *
     * \param   other       The linked list object to compare.
     **/
    inline bool operator == ( const LinkedList<VALUE> & other ) const;

    /**
     * \brief   Checks inequality of 2 linked list objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries in the linked list.
     *
     * \param   other       The linked list object to compare.
     **/
    inline bool operator != ( const LinkedList<VALUE> & other ) const;

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
    template<typename V>
    /**
     * \brief   Reads out from the stream linked list values. If linked list previously had values,
     *          they will be removed and new values from the stream will be set in the same sequence
     *          as they are present in the stream. There should be possibility to initialize values
     *          from streaming object and if VALUE is not a primitive, but an object, it should have
     *          implemented streaming operator.
     *
     * \param   stream      The streaming object to read values.
     * \param   input       The linked list object to save initialized values.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, LinkedList<V> & input );

    /**
     * \brief   Writes to the stream the values of linked list.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream values and if VALUE is not a
     *          primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The stream to write values.
     * \param   output  The linked list object containing value to stream.
     **/
    template<typename V>
    /**
     * \brief   Writes to the stream the values of linked list. The values will be written to the
     *          stream starting from first entry. There should be possibility to stream values and
     *          if VALUE is not a primitive, but an object, it should have implemented streaming
     *          operator.
     *
     * \param   stream      The stream to write values.
     * \param   output      The linked list object containing value to stream.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const LinkedList<V> & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Attributes
/************************************************************************/

    inline bool is_empty() const;

    inline uint32_t size() const;

    /**
     * \brief   Returns the position of the first value entry in the linked list, which is not
     *          invalid if the linked list is not empty. Otherwise, returns invalid position.
     **/
    inline LISTPOS first_position() const;

    /**
     * \brief   Returns the position of the last value entry in the linked list, which is not
     *          invalid if the linked list is not empty. Otherwise, returns invalid position.
     **/
    inline LISTPOS last_position() const;

    /**
     * \brief   Returns true if specified position points to the first entry in the linked list.
     *
     * \param   pos     The position to check.
     **/
    inline bool is_first_position(const LISTPOS pos) const;

    /**
     * \brief   Returns true if specified position points to the last entry in the linked list.
     *
     * \param   pos     The position to check.
     **/
    inline bool is_last_position(const LISTPOS pos) const;

    /**
     * \brief   Returns the invalid position of the linked list.
     **/
    inline LISTPOS invalid_position() const;

    /**
     * \brief   Returns true if specified position is invalid, i.e. points to the end of the linked
     *          list.
     *
     * \param   pos     The position to check.
     **/
    inline bool is_invalid_position(const LISTPOS pos) const;

    /**
     * \brief   Returns true if the given position is not pointing to the end of the linked list.
     *          Note, it does not check whether such a position exists in the linked list.
     **/
    inline bool is_valid_position(const LISTPOS pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing to a valid entry in the
     *          linked list. The duration of checkup depends on the location of the position in the
     *          linked list.
     *
     * \param   pos     The position to check.
     **/
    inline bool check_position(const LISTPOS pos) const;

    /**
     * \brief   Checks whether given element exist in the linked list or not. The elements of type
     *          VALUE should have comparing operators.
     *
     * \param   elemSearch      The element to search.
     * \return  Returns true if the element is found.
     **/
    inline bool contains(const VALUE& elemSearch) const;

    /**
     * \brief   Checks whether given element exist in the linked list or not, starting from the
     *          given position. The elements of type VALUE should have comparing operators.
     *
     * \param   elemSearch      The element to search.
     * \param   startAt         The position to start searching.
     * \return  Returns true if the element is found starting from the given position.
     **/
    inline bool contains( const VALUE & elemSearch, LISTPOS startAt) const;

    /**
     * \brief   Returns the list object where the data are stored.
     **/
    inline const std::list<VALUE>& data() const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Removes all entries of the linked list.
     **/
    inline void clear();

    /**
     * \brief   Deletes extra entries in the linked list.
     **/
    inline void free_extra();

    /**
     * \brief   Sets the size of the linked list to zero and deletes all capacity space.
     **/
    inline void release();

    /**
     * \brief   Returns the value of the head element in the linked list. The linked list must not
     *          be empty.
     **/
    inline const VALUE & first_entry() const;
    /**
     * \brief   Returns the value of the head element in the linked list. The linked list must not
     *          be empty.
     * \note    Non-const overload. Allows modification of the returned value.
     **/
    inline VALUE & first_entry();

    /**
     * \brief   Returns the value of the tail element in the linked list. The linked list must not
     *          be empty.
     **/
    inline const VALUE & last_entry() const;
    /**
     * \brief   Returns the value of the tail element in the linked list. The linked list must not
     *          be empty.
     * \note    Non-const overload. Allows modification of the returned value.
     **/
    inline VALUE & last_entry();

    /**
     * \brief   Returns the value at the given position of the linked list and on output advances
     *          the position to the next entry in the linked list or to the invalid position if no
     *          more elements exist.
     *
     * \param[in,out] nextPos     On input, this should be valid position of the element in the
     *                            Linked List. On output, this contains position of the next entry
     *                            in the Linked List or invalid position if reached end of the
     *                            linked list.
     **/
    inline const VALUE & next( LISTPOS & nextPos ) const;
    /**
     * \brief   Returns the value at the given position of the linked list and on output advances
     *          the position to the next entry in the linked list or to the invalid position if no
     *          more elements exist.
     *
     * \param[in,out] nextPos     On input, this should be valid position of the element in the
     *                            Linked List. On output, this contains position of the next entry
     *                            in the Linked List or invalid position if reached end of the
     *                            linked list.
     * \note    Non-const overload. Allows modification of the returned value.
     **/
    inline VALUE& next(LISTPOS& nextPos);

    /**
     * \brief   Returns either valid position of the next entry in the Linked List or invalid
     *          position if reached end of the linked list.
     *
     * \param   atPosition      Actual position to get the next position. Should be valid position.
     **/
    inline LISTPOS next_position( LISTPOS atPosition ) const;

    /**
     * \brief   Returns the value at the given position of the linked list and on output advances
     *          the position to the previous entry in the linked list or to the invalid position if
     *          it was the first entry in the linked list.
     *
     * \param[in,out] prevPos     On input, this should be valid position of the element in the
     *                            Linked List. On output, this contains position of the previous
     *                            entry in the Linked List or invalid position if there is no more
     *                            previous position in the linked list.
     **/
    inline const VALUE & prev( LISTPOS & prevPos ) const;
    /**
     * \brief   Returns the value at the given position of the linked list and on output advances
     *          the position to the previous entry in the linked list or to the invalid position if
     *          it was the first entry in the linked list.
     *
     * \param[in,out] prevPos     On input, this should be valid position of the element in the
     *                            Linked List. On output, this contains position of the previous
     *                            entry in the Linked List or invalid position if there is no more
     *                            previous position in the linked list.
     * \note    Non-const overload. Allows modification of the returned value.
     **/
    inline VALUE& prev(LISTPOS& prevPos);

    /**
     * \brief   Returns either valid position of the previous entry in the Linked List or invalid
     *          position if it indicates the first entry (no previous entry) of the linked list.
     *
     * \param   atPosition      Actual position to get the previous position. Should be valid
     *                          position.
     **/
    inline LISTPOS prev_position( LISTPOS atPosition ) const;

    /**
     * \brief   Returns the value of entry at the given valid position. The position must be valid.
     *          The returned value can be used by left (l-value) and right (r-value) operations.
     *
     * \param   atPosition      The valid position in Linked List
     * \return  Returns the value of entry at the given valid position.
     **/
    inline VALUE& value_at_position(LISTPOS atPosition);

    /**
     * \brief   Returns the value of entry at the given valid position. The position must be valid.
     *          The returned value can be used by right (r-value) operation.
     *
     * \param   atPosition      The valid position in Linked List
     * \return  Returns the value of entry at the given valid position.
     **/
    inline const VALUE & value_at_position( const LISTPOS atPosition ) const;

    /**
     * \brief   Returns element value by valid zero-based index, which can be used by left (l-value)
     *          and right (r-value) operations. The index should be valid.
     **/
    inline VALUE & at( uint32_t index );

    /**
     * \brief   Returns element value by valid zero-based index, which can be used by right
     *          operation (r-value). The index should be valid.
     **/
    inline const VALUE & at(uint32_t index ) const;

    /**
     * \brief   Returns the value of entry at the given valid position. The position must be valid.
     *          The returned value can be used by left (l-value) and right (r-value) operations.
     *
     * \param   atPosition      The valid position in Linked List
     * \return  Returns the value of entry at the given valid position.
     **/
    inline VALUE& at(LISTPOS atPosition);

    /**
     * \brief   Returns the value of entry at the given valid position. The position must be valid.
     *          The returned value can be used by right (r-value) operation.
     *
     * \param   atPosition      The valid position in Linked List
     * \return  Returns the value of entry at the given valid position.
     **/
    inline const VALUE& at(const LISTPOS atPosition) const;

    /**
     * \brief   Extracts the next position and value of the element following the given position.
     *
     * \param[in,out] nextPos         On input this indicates the valid position of the entry in the
     *                                linked list. On output, this parameter points either next
     *                                valid entry in the linked list or invalid entry if no more
     *                                entry is following.
     * \param[out] nextValue       On output, this contains value of the next entry in the linked
     *                             list.
     * \return  Returns true if there is a next element and the output value is valid.
     **/
    inline bool next_entry( LISTPOS & nextPos, VALUE & nextValue ) const;

    /**
     * \brief   Extracts the previous position and value of the element preceding the given
     *          position.
     *
     * \param[in,out] prevPos         On input this indicates the valid position of the entry in the
     *                                linked list. On output, this parameter points either previous
     *                                valid entry in the linked list or invalid entry if has no
     *                                previous entry, i.e. indicates first entry.
     * \param[out] prevValue       On output, this contains value of the previous entry in the
     *                             linked list.
     * \return  Returns true if there is a previous element and the output value is valid.
     **/
    inline bool prev_entry( LISTPOS & prevPos, VALUE & prevValue ) const;

    /**
     * \brief   Removes the head element from the linked list.
     **/
    inline void remove_first();

    /**
     * \brief   Removes the head element from the linked list. On output, the 'value' contains data
     *          of removed element. Function returns true if linked list was not empty and the
     *          'value' contains the data of removed element.
     *
     * \param[out] value       On output, this contains the data of the removed element.
     * \return  Returns true if linked list was not empty and the 'value' contains the data of
     *          removed element.
     **/
    inline bool remove_first( VALUE & value );

    /**
     * \brief   Removes the tail element from the linked list.
     **/
    inline void remove_last();

    /**
     * \brief   Removes the tail element from the linked list. On output, the 'value' contains data
     *          of removed element. Function returns true if linked list was not empty and the
     *          'value' contains the data of removed element.
     *
     * \param[out] value       On output, this contains the data of the removed element.
     * \return  Returns true if linked list was not empty and the 'value' contains the data of
     *          removed element.
     **/
    inline bool remove_last(VALUE& value);

    /**
     * \brief   Add element to head of Linked List.
     *
     * \param   newElement      New head element to add in Linked List
     **/
    inline void push_first( const VALUE & newElement );
    /**
     * \brief   Add element to head of Linked List.
     *
     * \param   newElement      New head element to add in Linked List
     * \note    Move overload. Takes ownership of the element.
     **/
    inline void push_first( VALUE && newElement );

    /**
     * \brief   Adds new entry at the head of linked list if it is not existing. If parameter
     *          'updateExisting' is true, it updates the existing entry. The method returns true if
     *          new element is added.
     *
     * \param   newElement          The element to add at the head of linked list.
     * \param   updateExisting      If true, updates the existing element. If, for example, 2
     *                              objects are compared by the name and not by absolute values,
     *                              setting this parameter true updates the existing entry.
     * \return  Returns true if inserted new entry at the head of the linked list. Otherwise,
     *          returns false.
     **/
    inline bool push_first_unique(const VALUE& newElement, bool updateExisting = false );
    /**
     * \brief   Adds new entry at the head of linked list if it is not existing. If parameter
     *          'updateExisting' is true, it updates the existing entry. The method returns true if
     *          new element is added.
     *
     * \param   newElement          The element to add at the head of linked list.
     * \param   updateExisting      If true, updates the existing element. If, for example, 2
     *                              objects are compared by the name and not by absolute values,
     *                              setting this parameter true updates the existing entry.
     * \return  Returns true if inserted new entry at the head of the linked list. Otherwise,
     *          returns false.
     * \note    Move overload. Takes ownership of the element.
     **/
    inline bool push_first_unique(VALUE&& newElement, bool updateExisting = false );

    /**
     * \brief   Add element to tail of Linked List.
     *
     * \param   newElement      New tail element to add in Linked List
     **/
    inline void push_last( const VALUE & newElement );
    /**
     * \brief   Add element to tail of Linked List.
     *
     * \param   newElement      New tail element to add in Linked List
     * \note    Move overload. Takes ownership of the element.
     **/
    inline void push_last( VALUE && newElement );

    /**
     * \brief   Adds new entry at the tail of linked list if it is not existing. If parameter
     *          'updateExisting' is true, it updates the existing entry. The method returns true if
     *          new element is added.
     *
     * \param   newElement          The element to add at the tail of linked list.
     * \param   updateExisting      If true, updates the existing element. If, for example, 2
     *                              objects are compared by the name and not by absolute values, by
     *                              setting this parameter true, you can replace the existing
     *                              object.
     * \return  Returns true if inserted new entry at the tail of the linked list. Otherwise,
     *          returns false.
     **/
    inline bool push_last_unique(const VALUE& newElement, bool updateExisting = false );
    /**
     * \brief   Adds new entry at the tail of linked list if it is not existing. If parameter
     *          'updateExisting' is true, it updates the existing entry. The method returns true if
     *          new element is added.
     *
     * \param   newElement          The element to add at the tail of linked list.
     * \param   updateExisting      If true, updates the existing element. If, for example, 2
     *                              objects are compared by the name and not by absolute values, by
     *                              setting this parameter true, you can replace the existing
     *                              object.
     * \return  Returns true if inserted new entry at the tail of the linked list. Otherwise,
     *          returns false.
     * \note    Move overload. Takes ownership of the element.
     **/
    inline bool push_last_unique(VALUE&& newElement, bool updateExisting = false );

    /**
     * \brief   Pops and returns the element at the head of linked list. The linked list must not be
     *          empty.
     **/
    inline VALUE pop_first();

    /**
     * \brief   Pops and returns the element at the tail of linked list. The linked list must not be
     *          empty.
     **/
    inline VALUE pop_last();

    /**
     * \brief   Inserts new element before given position. If given position is head element, it
     *          adds new head element.
     *
     * \param   beforePosition      The Linked List element valid position before new element should
     *                              be inserted.
     * \param   newElement          Value of new element to insert
     * \return  Returns position of new inserted element.
     **/
    inline LISTPOS insert_before( LISTPOS beforePosition, const VALUE & newElement );
    /**
     * \brief   Inserts new element before given position. If given position is head element, it
     *          adds new head element.
     *
     * \param   beforePosition      The Linked List element valid position before new element should
     *                              be inserted.
     * \param   newElement          Value of new element to insert
     * \return  Returns position of new inserted element.
     * \note    Move overload. Takes ownership of the element.
     **/
    inline LISTPOS insert_before(LISTPOS beforePosition, VALUE && newElement);

    /**
     * \brief   Inserts new element after given position. If given position is tail element, it adds
     *          new tail element.
     *
     * \param   afterPosition       The Linked List element position after new element should be
     *                              inserted
     * \param   newElement          Value of new element to insert
     * \return  Returns position of new inserted element.
     **/
    inline LISTPOS insert_after(LISTPOS afterPosition, const VALUE & newElement);
    /**
     * \brief   Inserts new element after given position. If given position is tail element, it adds
     *          new tail element.
     *
     * \param   afterPosition       The Linked List element position after new element should be
     *                              inserted
     * \param   newElement          Value of new element to insert
     * \return  Returns position of new inserted element.
     * \note    Move overload. Takes ownership of the element.
     **/
    inline LISTPOS insert_after(LISTPOS afterPosition, VALUE && newElement);

    /**
     * \brief   Sets new value at given position.
     *
     * \param   atPosition      The Linked List element valid position to change value.
     * \param   newValue        The value to update.
     **/
    inline void set_at( LISTPOS atPosition, const VALUE & newValue );

    /**
     * \brief   Removes element at given position and returns position of the next entry in the
     *          linked list. Returns invalid position if tail entry is removed.
     *
     * \param   atPosition      Position of the element to remove from Linked List.
     * \return  Returns the position of the next entry in the linked-list. If the tail entry is
     *          removed, returns invalid position.
     **/
    inline LISTPOS remove_at( LISTPOS atPosition );

    /**
     * \brief   Removes element at given position and returns position of the next entry in the
     *          linked list. Returns invalid position if tail entry is removed.
     *
     * \param   atPosition      Position of the element to remove from Linked List.
     * \param[out] Value           On output, it contains value of removed element
     * \return  Returns the position of the next entry in the linked-list. If the tail entry is
     *          removed, returns invalid position.
     **/
    inline LISTPOS remove_at( LISTPOS atPosition, VALUE & Value );

    /**
     * \brief   Searches and removes first match of entry, which value is equal to the given
     *          element. Returns true if found and removed entry with success.
     *
     * \param   removeElement       Element to search and remove from Linked List
     * \return  Returns true if found and removed entry.
     **/
    inline bool remove_entry( const VALUE & removeElement );

    /**
     * \brief   Searches and removes first match of entry, which value is equal to the given
     *          element, starting from the given position. Returns true if found and removed entry
     *          with success.
     *
     * \param   removeElement       Element to search and remove from Linked List
     * \param   searchAfter         The valid position in the linked list to start searching.
     * \return  Returns true if found and removed entry.
     **/
    inline bool remove_entry(const VALUE& removeElement, LISTPOS searchAfter);

    /**
     * \brief   Searches position of the entry by given value and returns valid position if found an
     *          entry. Otherwise, it returns invalid position.
     *
     * \param   searchValue     Value of element to search.
     * \return  If found, returns valid position of the entry in the linked-list. Otherwise, returns
     *          invalid position.
     **/
    inline LISTPOS find(const VALUE& searchValue) const;

    /**
     * \brief   Searches position of the entry by given value starting from the given position and
     *          returns valid position if found an entry. Otherwise, it returns invalid position.
     *
     * \param   searchValue     Value of element to search.
     * \param   searchAfter     The valid position in the linked list to start searching.
     * \return  If found, returns valid position of the entry in the linked-list. Otherwise, returns
     *          invalid position.
     **/
    inline LISTPOS find( const VALUE & searchValue, LISTPOS searchAfter ) const;

    /**
     * \brief   Returns valid position of the element by given zero-based index. Returns invalid
     *          position if the index is invalid.
     *
     * \param   index       The index of element in Linked List to get position.
     **/
    inline LISTPOS find_index( uint32_t index ) const;

    /**
     * \brief   Returns valid zero-based index of the element by given valid position. Returns
     *          invalid index (0xFFFFFFFF) if the position is invalid.
     *
     * \param   atPosition      The valid position in the linked list to return index.
     **/
    inline uint32_t make_index( LISTPOS atPosition ) const;

    /**
     * \brief   Merges entries from source into this list. Both lists should be sorted. The source
     *          list becomes empty after merge.
     *
     * \param   source      The source linked list to merge.
     * \note    For equivalent elements in the two lists, elements from this list precede elements
     *          from the source, preserving the sort order.
     **/
    inline void merge(LinkedList<VALUE> & source);
    /**
     * \brief   Merges entries from source into this list. Both lists should be sorted. The source
     *          list becomes empty after merge.
     *
     * \param   source      The source linked list to merge.
     * \note    Move overload. Takes ownership of the source list.
     * \note    For equivalent elements in the two lists, elements from this list precede elements
     *          from the source, preserving the sort order.
     **/
    inline void merge(LinkedList<VALUE> && source);

    /**
     * \brief   Sorts the linked list, compares the elements by given Compare functionality.
     * \param   comp    The comparing method, similar to the method  std::greater()
     * \return  Sorts and returns the linked list object.
     **/
    template <class Compare>
    /**
     * \brief   Sorts the linked list using the given comparator function. Returns a reference to
     *          this sorted list.
     *
     * \param   comp    The comparator function, similar to std::greater() or other comparison
     *                  functors.
     * \return  Returns a reference to this list after sorting.
     **/
    inline LinkedList< VALUE >& sort(Compare comp);

    /**
     * \brief   Copies elements from the linked list into the provided pre-allocated buffer. If
     *          `elemCount` is less than the number of elements in the linked list, only the first
     *          `elemCount` elements are copied. Otherwise, all elements in the linked list are
     *          copied. No elements are copied if `elemCount` is 0.
     *
     * \param[in,out] list            A pre-allocated buffer where the linked list elements will be
     *                                copied. Must be large enough to hold at least `elemCount`
     *                                elements.
     * \param   elemCount       The maximum number of elements to copy into the `list` buffer. If
     *                          set to 0, no elements are copied.
     * \return  The number of elements successfully copied into the `list` buffer.
     **/
    inline uint32_t elements(VALUE* list, uint32_t elemCount);

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns the position of the element at the given zero-based index.
     *
     * \param   index       The index of the element to return position.
     **/
    inline LISTPOS position( uint32_t index ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Converts the constant iterator of the list into the LISTPOS type.
     *
     * \param   cit     The constant iterator of the list.
     * \return  Returns converted LISTPOS type.
     **/
    inline LISTPOS _citer2pos(typename std::list<VALUE>::const_iterator cit) const;

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
// LinkedList<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE>
LinkedList<VALUE>::LinkedList(const VALUE* list, uint32_t count)
    : Constless<std::list<VALUE>>()
    , mValueList()
{
    for (uint32_t i = 0; i < count; ++i)
    {
        mValueList.push_back(list[i]);
    }
}

template <typename VALUE >
inline LinkedList<VALUE>& LinkedList<VALUE>::operator = (const LinkedList<VALUE>& src)
{
    mValueList = src.mValueList;
    return (*this);
}

template <typename VALUE >
inline LinkedList<VALUE>& LinkedList<VALUE>::operator = (LinkedList<VALUE>&& src) noexcept
{
    mValueList = std::move(src.mValueList);
    return (*this);
}

template <typename VALUE >
inline bool LinkedList<VALUE>::operator == (const LinkedList<VALUE>& other) const
{
    return (mValueList == other.mValueList);
}

template <typename VALUE >
inline bool LinkedList<VALUE>::operator != (const LinkedList<VALUE>& other) const
{
    return (mValueList != other.mValueList);
}

template <typename VALUE >
inline VALUE & LinkedList<VALUE>::operator [](uint32_t atIndex )
{
    return at(atIndex);
}

template <typename VALUE >
inline VALUE & LinkedList<VALUE>::operator []( LISTPOS atPosition )
{
    return value_at_position(atPosition);
}

template <typename VALUE >
inline const VALUE & LinkedList<VALUE>::operator [](uint32_t atIndex ) const
{
    return at(atIndex);
}

template <typename VALUE >
inline const VALUE & LinkedList<VALUE>::operator [] ( const LISTPOS atPosition ) const
{
    return value_at_position(atPosition);
}

template <typename VALUE >
inline bool LinkedList<VALUE>::is_empty() const
{
    return mValueList.empty();
}

template <typename VALUE >
inline uint32_t LinkedList<VALUE>::size() const
{
    return static_cast<uint32_t>(mValueList.size());
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::first_position() const
{
    return _citer2pos(mValueList.begin());
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::last_position() const
{
    return _citer2pos(mValueList.empty() == false ? --mValueList.end() : mValueList.end());
}

template <typename VALUE >
inline bool LinkedList<VALUE>::is_first_position(const LISTPOS pos) const
{
    return (pos == mValueList.begin());
}

template <typename VALUE >
inline bool LinkedList<VALUE>::is_last_position(const LISTPOS pos) const
{
    return ((mValueList.empty() == false) && (pos == --mValueList.end()));
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::invalid_position() const
{
	return _citer2pos(mValueList.end());
}

template <typename VALUE >
inline bool LinkedList<VALUE>::is_invalid_position(const LISTPOS pos) const
{
    return (pos == mValueList.end());
}

template <typename VALUE >
inline bool LinkedList<VALUE>::is_valid_position(const LISTPOS pos) const
{
    return (pos != mValueList.end());
}

template <typename VALUE >
inline bool LinkedList<VALUE>::check_position(const LISTPOS pos) const
{
    auto it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template <typename VALUE >
inline void LinkedList<VALUE>::clear()
{
    mValueList.clear();
}

template <typename VALUE >
inline void LinkedList<VALUE>::free_extra()
{
    mValueList.clear();
}

template <typename VALUE >
inline void LinkedList<VALUE>::release()
{
    mValueList.clear();
}

template <typename VALUE >
inline const VALUE & LinkedList<VALUE>::first_entry() const
{
    ASSERT(mValueList.empty() == false);
    return mValueList.front();
}

template <typename VALUE >
inline VALUE& LinkedList<VALUE>::first_entry()
{
    ASSERT(mValueList.empty() == false);
    return mValueList.front();
}

template <typename VALUE >
inline const VALUE & LinkedList<VALUE>::last_entry() const	
{
    ASSERT(mValueList.empty() == false);
    return mValueList.back();
}

template <typename VALUE >
inline VALUE& LinkedList<VALUE>::last_entry()
{
    ASSERT(mValueList.empty() == false);
    return mValueList.back();
}

template <typename VALUE >
inline const VALUE & LinkedList<VALUE>::next(LISTPOS& nextPos) const
{
    return *nextPos++;
}

template <typename VALUE >
inline VALUE& LinkedList<VALUE>::next(LISTPOS& nextPos)
{
    LISTPOS pos = nextPos++;
    return *pos;
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::next_position(LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return ++atPosition;
}

template <typename VALUE >
inline const VALUE & LinkedList<VALUE>::prev(LISTPOS& prevPos) const
{
    ASSERT(prevPos != mValueList.end());
    LISTPOS pos = prevPos;
    prevPos = prevPos == _citer2pos(mValueList.begin()) ? _citer2pos(mValueList.end()) : --prevPos;
    return *pos;
}

template <typename VALUE >
inline VALUE & LinkedList<VALUE>::prev(LISTPOS& prevPos)
{
    ASSERT(prevPos != mValueList.end());
    LISTPOS pos = prevPos;
    prevPos = prevPos == mValueList.begin() ? mValueList.end() : --prevPos;
    return *pos;
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::prev_position(LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return _citer2pos(atPosition == mValueList.begin() ? invalid_position() : --atPosition);
}

template <typename VALUE >
inline const VALUE & LinkedList<VALUE>::value_at_position( const LISTPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE >
inline VALUE & LinkedList<VALUE>::value_at_position(LISTPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE >
inline const VALUE & LinkedList<VALUE>::at(uint32_t index) const
{
    LISTPOS pos = position(index);
    ASSERT(is_valid_position(pos));

    return *pos;
}

template <typename VALUE >
inline VALUE & LinkedList<VALUE>::at(uint32_t index)
{
    LISTPOS pos = position(index);
    ASSERT(is_valid_position(pos));

    return *pos;
}

template <typename VALUE >
inline const VALUE& LinkedList<VALUE>::at(const LISTPOS atPosition) const
{
    return value_at_position(atPosition);
}

template <typename VALUE >
inline VALUE& LinkedList<VALUE>::at(LISTPOS atPosition)
{
    return value_at_position(atPosition);
}

template <typename VALUE >
inline bool LinkedList<VALUE>::next_entry(LISTPOS & nextPos, VALUE & nextValue) const
{
    bool result = false;
    ASSERT(nextPos != mValueList.end());
    nextPos = next_position(nextPos);
    if (nextPos != mValueList.end())
    {
        nextValue = *nextPos;
        result = true;
    }

    return result;
}

template <typename VALUE >
inline bool LinkedList<VALUE>::prev_entry(LISTPOS& prevPos, VALUE & prevValue) const
{
    bool result = false;
    ASSERT(prevPos != mValueList.end());
    prevPos = prev_position(prevPos);
    if (prevPos != mValueList.end())
    {
        prevValue = *prevPos;
        result = true;
    }

    return result;
}

template <typename VALUE >
inline void LinkedList<VALUE>::remove_first()
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_front();
}

template <typename VALUE >
inline bool LinkedList<VALUE>::remove_first(VALUE & value)
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
inline void LinkedList<VALUE>::remove_last()
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_back();
}

template <typename VALUE >
inline bool LinkedList<VALUE>::remove_last(VALUE & value)
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
inline void LinkedList<VALUE>::push_first(const VALUE & newElement)
{
    mValueList.push_front(newElement);
}

template <typename VALUE >
inline void LinkedList<VALUE>::push_first( VALUE && newElement )
{
    mValueList.push_front(std::move(newElement));
}

template <typename VALUE >
inline bool LinkedList<VALUE>::push_first_unique(const VALUE& newElement, bool updateExisting /*= false*/ )
{
    bool result{ false };

    auto pos = std::find( mValueList.begin( ), mValueList.end( ), newElement );
    if ( pos == mValueList.end())
    {
        result = true;
        mValueList.push_front(newElement);
    }
    else if (updateExisting )
    {
        *pos = newElement;
    }

    return result;
}

template <typename VALUE >
inline bool LinkedList<VALUE>::push_first_unique(VALUE&& newElement, bool updateExisting /*= false*/ )
{
    bool result{ false };
    auto pos = std::find( mValueList.begin( ), mValueList.end( ), newElement );
    if ( pos == mValueList.end())
    {
        result = true;
        mValueList.push_front(std::move(newElement));
    }
    else if ( updateExisting )
    {
        *pos = newElement;
    }

    return result;
}

template <typename VALUE >
inline void LinkedList<VALUE>::push_last(const VALUE & newElement)
{
    mValueList.push_back(newElement);
}

template <typename VALUE >
inline void LinkedList<VALUE>::push_last(VALUE && newElement)
{
    mValueList.push_back(std::move(newElement));
}

template <typename VALUE >
inline bool LinkedList<VALUE>::push_last_unique(const VALUE& newElement, bool updateExisting /*= false*/ )
{
    bool result{ false };
    auto pos = std::find( mValueList.rbegin( ), mValueList.rend( ), newElement );
    if (pos == mValueList.rend())
    {
        result = true;
        mValueList.push_back(newElement);
    }
    else if ( updateExisting )
    {
        *pos = newElement;
    }

    return result;
}

template <typename VALUE >
inline bool LinkedList<VALUE>::push_last_unique(VALUE&& newElement, bool updateExisting /*= false*/ )
{
    bool result{ false };
    auto pos = std::find( mValueList.rbegin( ), mValueList.rend( ), newElement );
    if (pos == mValueList.rend())
    {
        result = true;
        mValueList.push_back(std::move(newElement));
    }
    else if ( updateExisting )
    {
        *pos = newElement;
    }

    return result;
}

template <typename VALUE >
inline VALUE LinkedList<VALUE>::pop_first()
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.front();
    mValueList.pop_front();
    return result;
}

template <typename VALUE >
inline VALUE LinkedList<VALUE>::pop_last()
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.back();
    mValueList.pop_back();
    return result;
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::insert_before(LISTPOS beforePosition, const VALUE & newElement)
{
    return mValueList.insert(beforePosition, std::move(newElement));
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::insert_before(LISTPOS beforePosition, VALUE && newElement)
{
    return mValueList.insert(beforePosition, std::move(newElement));
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::insert_after(LISTPOS afterPosition, const VALUE & newElement)
{
    if (afterPosition == mValueList.end())
    {
        mValueList.push_back(newElement);
        return last_position();
    }
    else
    {
        return mValueList.insert(++afterPosition, newElement);
    }
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::insert_after(LISTPOS afterPosition, VALUE && newElement)
{
    if (afterPosition == mValueList.end())
    {
        mValueList.push_back(newElement);
        return last_position();
    }
    else
    {
        auto cit(afterPosition);
        return mValueList.insert(++cit, std::move(newElement));
    }
}

template <typename VALUE >
inline void LinkedList<VALUE>::set_at(LISTPOS atPosition, const VALUE & newValue)
{
    ASSERT(atPosition != mValueList.end());
    *atPosition = newValue;
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::remove_at(LISTPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::remove_at(LISTPOS atPosition, VALUE& out_value)
{
    LISTPOS result = invalid_position();

    if (atPosition != result)
    {
        out_value = *atPosition;
        result = mValueList.erase(atPosition);
    }

    return result;
}

template <typename VALUE >
inline bool LinkedList<VALUE>::remove_entry( const VALUE & removeElement )
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
inline bool LinkedList<VALUE>::remove_entry(const VALUE & removeElement, LinkedList<VALUE>::LISTPOS searchAfter /*= nullptr*/)
{
    bool result = false;
    LISTPOS end = invalid_position();
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
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::find(const VALUE& searchValue) const
{
    return _citer2pos(std::find(mValueList.begin(), mValueList.end(), searchValue));
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::find(const VALUE & searchValue, LinkedList<VALUE>::LISTPOS searchAfter ) const
{
    LISTPOS end = invalid_position();
    return (searchAfter != end ? static_cast<LISTPOS>(std::find(++searchAfter, end, searchValue)) : end);
}

template <typename VALUE >
inline bool LinkedList<VALUE>::contains(const VALUE& elemSearch) const
{
    return contains(elemSearch, first_position());
}

template <typename VALUE >
inline bool LinkedList<VALUE>::contains(const VALUE& elemSearch, LISTPOS startAt) const
{
    return (startAt != mValueList.end() ? std::find(startAt, invalid_position(), elemSearch) != mValueList.end() : false);
}

template<typename VALUE>
inline const std::list<VALUE>& LinkedList<VALUE>::data() const
{
    return mValueList;
}

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::find_index(uint32_t index) const
{
    return Constless<std::list<VALUE>>::iter(mValueList, index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() + index : mValueList.end());
}

template <typename VALUE >
inline uint32_t LinkedList<VALUE>::make_index(LISTPOS atPosition) const
{
    uint32_t result{ 0 };
    LISTPOS pos = mValueList.begin();
    LISTPOS end = mValueList.end();
    for (; (pos != end) && (pos != atPosition); ++pos, ++result)
        ;

    return (pos != end ? result : static_cast<uint32_t>(NECommon::INVALID_INDEX));
}

template <typename VALUE >
inline void LinkedList<VALUE>::merge(LinkedList<VALUE>& source)
{
    mValueList.sort();
    source.mValueList.sort();
    mValueList.merge(source.mValueList);
}

template <typename VALUE >
inline void LinkedList<VALUE>::merge(LinkedList<VALUE>&& source)
{
    mValueList.sort();
    source.mValueList.sort();
    mValueList.merge(std::move(source.mValueList));
}

template<typename VALUE>
inline uint32_t LinkedList<VALUE>::elements(VALUE* list, uint32_t elemCount)
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

template <typename VALUE >
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::position(uint32_t index) const
{
    typename std::list<VALUE>::const_iterator pos = index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() : mValueList.end();
    for (uint32_t i = 1; i <= index; ++i)
    {
        ++pos;
    }

    return _citer2pos(pos);
}

template<typename VALUE>
template<class Compare>
inline LinkedList<VALUE>& LinkedList<VALUE>::sort(Compare comp)
{
    mValueList.sort(comp);
    return (*this);
}

template<typename VALUE>
inline typename LinkedList<VALUE>::LISTPOS LinkedList<VALUE>::_citer2pos(typename std::list<VALUE>::const_iterator cit) const
{
    return Constless<std::list<VALUE>>::iter(mValueList, cit);
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template <typename V>
inline const InStream & operator >> ( const InStream & stream, LinkedList<V> & input )
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

template <typename V>
inline OutStream & operator << ( OutStream & stream, const LinkedList<V> & output )
{
    uint32_t size = output.size();
    stream << size;

    for (const auto& elem : output.mValueList)
    {
        stream << elem;
    }

    return stream;
}

#endif  // AREG_BASE_LINKEDLIST_HPP
