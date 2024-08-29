#ifndef AREG_BASE_TESORTEDLINKEDLIST_HPP
#define AREG_BASE_TESORTEDLINKEDLIST_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/TESortedLinkedList.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Sorted Linked List class template.
 *              The linked list contains list of values that can be
 *              accessed either by Position or by valid index.
 *              The size of Linked List is dynamic, Elements contain
 *              information of previous and next Block.
 *              When element is inserted, the class guaranties, that values
 *              are sorted either ascending or descending.
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
// TESortedLinkedList<VALUE, VALUE_TYPE, Sorter> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Bi-directional linked list, which elements are sorted ascending or descending.
 *          Decision whether the elements of linked list are ascending or descending is
 *          done when initializing object. Depending on this flag, elements are sorted when
 *          inserted into the linked list.
 *
 *          The Sorted Linked List does not allow modification of element by position and
 *          it does not give possibility to insert Head or Tail position. Accessing elements
 *          by linked list position and index are read-only. The Sorted LinkedList object
 *          is not thread safe and data access should be  synchronized manually.
 *
 * \tparam  VALUE       The type of stored items. Either should be primitive or should have
 *                      default constructor and valid assigning operator. Also, should be
 *                      possible to convert to type VALUE_TYPE.
 **/
template <typename VALUE>
class TESortedLinkedList : private Constless<std::list<VALUE>>
{
//////////////////////////////////////////////////////////////////////////
// Internal objects and types declaration
//////////////////////////////////////////////////////////////////////////
public:
    using LISTPOS = typename std::list<VALUE>::iterator;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty linked list
     **/
    explicit TESortedLinkedList(bool sortAscending = true);

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TESortedLinkedList(const TESortedLinkedList<VALUE>& src) = default;

    /**
     * \brief   Move constructor.
     * \param   src     The source to copy data.
     **/
    TESortedLinkedList(TESortedLinkedList<VALUE>&& src) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TESortedLinkedList(void) = default;

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
    inline TESortedLinkedList<VALUE>& operator = (const TESortedLinkedList<VALUE>& src);

    /**
     * \brief	Move operator, moves elements of src to the Linked List
     * \param	src	Source Linked List to get elements.
     **/
    inline TESortedLinkedList<VALUE>& operator = (TESortedLinkedList<VALUE>&& src) noexcept;

    /**
     * \brief   Checks equality of 2 linked-list objects, and returns true if they are equal.
     *          There should be possible to compare VALUE types entries of linked list.
     * \param   other   The linked-list object to compare
     **/
    inline bool operator == (const TESortedLinkedList<VALUE>& other) const;

    /**
     * \brief   Checks inequality of 2 linked-list objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE types entries of linked list.
     * \param   other   The linked-list object to compare
     **/
    inline bool operator != (const TESortedLinkedList<VALUE>& other) const;


    /**
     * \brief   Subscript operator. Returns value of element by given
     *          valid index. The index should be valid, otherwise
     *          assertion raised. May be used only on the right.
     * \param   atIndex     The valid zero-based index.
     * \return  Returns Value of Element at given index, the value should not be modified
     **/
    inline const VALUE& operator [] (uint32_t atIndex) const;

    /**
     * \brief   Subscript operator. Returns value of element by given
     *          valid Position. May be used only on the right.
     * \param   atPosition  The valid Linked List position.
     * \return  Returns Value of Element at given Linked List position, the value can be modified
     **/
    inline const VALUE& operator [] (const LISTPOS atPosition) const;

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
    template<typename V>
    friend const IEInStream& operator >> (const IEInStream& stream, TESortedLinkedList<V>& input);
    /**
     * \brief   Writes to the stream Linked List values.
     *          The values will be written to the stream starting from head position.
     *          There should be possibility to stream every value of Linked List and if VALUE
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   output  The Linked List object to read out values.
     **/
    template<typename V>
    friend IEOutStream& operator << (IEOutStream& stream, const TESortedLinkedList<V>& output);

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
    inline bool isEmpty(void) const;

    /**
     * \brief   Returns the size of Linked List container
     **/
    inline uint32_t getSize(void) const;

    /**
     * \brief   Returns true if the list is sorting ascending
     **/
    inline bool isAscending(void) const;

    /**
     * \brief   Returns true if the list is sorting descending
     **/
    inline bool isDescending(void) const;

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
     * \brief   Returns true if specified position pointing start of the linked list.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the linked list.
     **/
    inline bool isFirstPosition(const LISTPOS pos) const;

    /**
     * \brief   Returns true if specified position pointing start of the linked list.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the linked list.
     **/
    inline bool isLastPosition(const LISTPOS pos) const;

    /**
     * \brief   Returns invalid position object.
     **/
    inline LISTPOS invalidPosition(void) const;

    /**
     * \brief   Returns true if specified position reached the end of the hash map.
     * \param   pos     The position to check.
     * \return  Returns true if specified position reached the end of the hash map.
     **/
    inline bool isInvalidPosition(const LISTPOS pos) const;

    /**
     * \brief   Returns true if specified position is not pointing the end of the hash map.
     *          NOTE:   This method does not ensure that the position object is pointing
     *                  to the valid entry in the hash map. To do so, use 'checkPos' method.
     * \param pos       The position to check.
     * \return Returns true if specified position is not pointing the end of the hash map.
     */
    inline bool isValidPosition(const LISTPOS pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the linked list.
     *          The duration of checkup depends on the location of the position in the linked list.
     * \param pos       The position to check.
     * \return  Returns true if specified position points to the valid entry in the linked list.
     */
    inline bool checkPosition(const LISTPOS pos) const;

    /**
     * \brief	Checks whether given element exist in the linked list or not. The elements of type
     *          VALUE should have comparing operators.
     * \param	elemSearch  The element to search.
     * \return	Returns true if could find element starting at given position.
     **/
    inline bool contains(const VALUE& elemSearch) const;

    /**
     * \brief	Checks whether given element exist in the linked list or not. The elements of type
     *          VALUE should have comparing operators.
     * \param	elemSearch  The element to search.
     * \param	startAt	    The position to start searching.
     * \return	Returns true if could find element starting at given position.
     **/
    inline bool contains(const VALUE& elemSearch, LISTPOS startAt) const;

    /**
     * \brief   Returns the vector object where the data are stored.
     **/
    inline const std::list<VALUE>& getData(void) const;

/************************************************************************/
// Operations
/************************************************************************/

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
    inline const VALUE& firstEntry(void) const;

    /**
     * \brief   Returns value of tail element in Linked List container.
     *          On call the Linked List should not be empty, otherwise assertion is raised.
     **/
    inline const VALUE& lastEntry(void) const;

    /**
     * \brief	Returns given position value and on exit position of next element in Linked List container.
     *          On input, the value of 'in_out_NextPosition' should be valid (for example, should be equal to head position).
     *          Otherwise assertion is raised.
     * \param	in_out_NextPosition On input, this should be valid position of element in Linked List container.
     *                              On output, this contains position of next element in Linked List container
     *                              or nullptr (INVALID_POSITION) if passed position on input is position of tail element
     * \return	Returns value of element at the given position.
     **/
    inline const VALUE & getNext(LISTPOS & IN OUT in_out_NextPosition) const;

    /**
     * \brief	Returns position of next element in Linked List
     * \param	atPosition	Actual position to get the next position
     * \return	If element at given position has next element, returns position of next element in Linked List.
     *          Otherwise it returns nullptr
     **/
    inline LISTPOS nextPosition(LISTPOS atPosition) const;

    /**
     * \brief	Returns given position value and on exit position of previous element in Linked List container.
     *          On input, the value of 'in_out_PrevPosition' should be valid (for example, should be equal to tail position).
     *          Otherwise assertion is raised.
     * \param	in_out_PrevPosition On input, this should be valid position of element in Linked List container.
     *                              On output, this contains position of previous element in Linked List container
     *                              or nullptr (INVALID_POSITION) if passed position on input is position of head element
     * \return	Returns value of element at the given position.
     **/
    inline const VALUE & getPrev(LISTPOS & IN OUT in_out_PrevPosition) const;

    /**
     * \brief	Returns position of previous element in Linked List
     * \param	atPosition	Actual position to get the previous position
     * \return	If element at given position has previous element, returns position of previous element in Linked List.
     *          Otherwise it returns nullptr
     **/
    inline LISTPOS prevPosition(const LISTPOS atPosition) const;

    /**
     * \brief   Returns value of element at the give position.
     * \param   atPosition  The valid position in Linked List
     * \return  Returns Value at given position, which should not be modified.
     **/
    inline const VALUE& valueAtPosition(const LISTPOS atPosition) const;

    /**
     * \brief	Returns the value of element by give index. The index of head element is 0.
     *          The index must not be more than GetSize() - 1.
     * \param	index	The index of element to get value
     * \return	The value of element by give index.
     **/
    inline const VALUE& getAt(uint32_t index) const;

    /**
     * \brief   Returns value of element at the give position.
     * \param   atPosition  The valid position in Linked List
     * \return  Returns Value at given position, which should not be modified.
     **/
    inline const VALUE& getAt(const TESortedLinkedList<VALUE>::LISTPOS atPosition) const;

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
    bool nextEntry(LISTPOS& in_out_NextPosition, VALUE& out_NextValue) const;

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
    bool prevEntry(LISTPOS& in_out_PrevPosition, VALUE& out_PrevValue) const;

    /**
     * \brief   Removes head element from Sorted Linked List.
     **/
    inline void removeFirst(void);

    /**
     * \brief   Removes the head element from the linked list. On output, the 'value' contains
     *          data of removed element. Function returns true if linked list was not empty,
     *          succeeded to remove head and the 'value' contains the data of removed element.
     * \param   value [out]     If succeeded to remove head element, on output it contains
     *                          the data of remove element.
     * \return  Returns true if succeeded to remove head. It as well indicates that the 'value'
     *          contains data of removed element.
     */
    bool removeFirst(VALUE& value);

    /**
     * \brief   Removes tail element from Sorted Linked List.
     **/
    inline void removeLast(void);

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
     * \brief   Add element to the sorted Linked List and returns position of the added element.
     *          Before adding, it will search the right position, depending on sorting algorithm.
     * \param   newElement   New element to add in Linked List
     * \return  Returns position of the element in Linked List.
     **/
    LISTPOS add(const VALUE& newElement);
    LISTPOS add(VALUE&& newElement);

    /**
     * \brief   Add uniquely element to the sorted Linked List and returns position of the element.
     *          Before adding, it will check whether the element is unique in the list and search the right
     *          position, depending on sorting algorithm. The element should have all comparing operators.
     *          If parameter 'updateExisting' is true, it updates the existing entry.
     * \param   newElement   New element to add in Linked List
     * \param   updateExisting  If true, updates the existing element.
     *                          If, for example, 2 objects are compared by the name and not by
     *                          absolute values, setting this parameter true updates the existing entry.
     * \return  Returns a pair where first element indicates the position of the element in the Sorted Linked List
     *          and the second entry indicates whether the new entry added or not.
     **/
    std::pair<LISTPOS, bool> addIfUnique(const VALUE& newElement, bool updateExisting = false );
    std::pair<LISTPOS, bool> addIfUnique(VALUE&& newElement, bool updateExisting = false );

    /**
     * \brief   Pops the element at the head of linked list and returns the stored value.
     *          The linked list must not be empty.
     * \return  The popped out element from the header of the linked list.
     */
    inline VALUE popFirst(void);

    /**
     * \brief   Pops the element at the tails of linked list and returns the stored value.
     *          The linked list must not be empty.
     * \return  The popped out element from the tail of the linked list.
     */
    inline VALUE popLast(void);

    /**
     * \brief	Sets new value at give position and returns old value of same element.
     * \param	atPosition	The Linked List element position to change value
     * \param	newValue	New Value to change
     **/
    inline void setAt(LISTPOS atPosition, const VALUE& newValue);

    /**
     * \brief	Removes element at given position and returns value of removed element.
     * \param	atPosition  Position of element to remove from Linked List.
     *                      It should be valid position, otherwise assertion is raised.
     * \return  Returns position following nest to removed position or invalid position
     *          if last entry was removed.
     **/
    inline LISTPOS removeAt(LISTPOS atPosition);

    /**
     * \brief	Removes element at given position and on output returns value of removed element.
     * \param	atPosition  On input, position of element to remove from Linked List.
     *                      It should be valid position, otherwise assertion is raised.
     * \param   out_Value   On output, it contains value of removed element
     * \return  Returns position following nest to removed position or invalid position
     *          if last entry was removed.
     **/
    inline LISTPOS removeAt(LISTPOS atPosition, VALUE& out_Value);

    /**
     * \brief	Searches and removes first match of given element from Linked List.
     *          If 'searchAfter' is nullptr, it will start searching element from head.
     *          And returns true if element was found and successfully removed from Linked List.
     * \param	removeElement	Element to search and remove from Linked List
     * \return	Returns true if element was found and successfully removed from linked list.
     **/
    bool removeEntry(const VALUE& removeElement);

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
     * \return	Returns position of the element in the linked list.
     **/
    inline LISTPOS find(const VALUE& searchValue) const;

    /**
     * \brief	Searches position of element by given value. If searchAfter is valid, the searching will be started
     *          from given position and will move to next element.
     * \param	searchValue	Value of element to search
     * \param	searchAfter	If valid position, the searching starts from next element specified by position.
     *                      If invalid position, the searching starts from the beginning of the linked list.
     * \return	Returns position of the element in the linked list.
     **/
    inline LISTPOS find(const VALUE& searchValue, LISTPOS searchAfter) const;

    /**
     * \brief	Returns position of element by given index. The index of head element is 0.
     *          The index of last element (tail element) is GetSize() - 1.
     *          If invalid index is passed, function returns nullptr
     * \param	index	The index of element in Linked List to get position
     * \return	Returns position of element if index is valid, i.e. more or equal to 0 and less than GetSize().
     *          Otherwise returns nullptr (INVALID_POSITION)
     **/
    inline LISTPOS findIndex(uint32_t index) const;

    /**
     * \brief	Returns valid index of element by given valid position.
     *          Otherwise returns 0xFFFFFFFF.
     * \param	atPosition  The position of element, which index should be returned.
     * \return	The index of element by given position
     **/
    uint32_t makeIndex(LISTPOS atPosition) const;

    /**
     * \brief   Call to resort the linked list.
     **/
    void resort();

    /**
     * \brief   Merges entries from the `source` into *this.
     *          No elements are copied, and the source container becomes empty after the merge.
     *          The elements from `*this` always precede the elements from `source`, 
     *          and the order of elements do not change, which means that the order 
     *          of elements in the result list remain either ascending or descending.
     * \param   source  The source of linked list to merge.
     **/
    inline void merge(TESortedLinkedList<VALUE> & source);
    inline void merge(TESortedLinkedList<VALUE> && source);

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns the position of the element at the given index.
     *          The value of returned position cannot be modified.
     * \param   index   The index of the element to return position.
     * \return  Returns the position of the element at the given index.
     *          The value of returned position cannot be modified.
     **/
    inline LISTPOS getPosition(uint32_t index) const;

    /**
     * \brief   Returns the position of the element at the given index.
     * \param   index   The index of the element to return position.
     * \return  Returns the position of the element at the given index.
     **/
    inline LISTPOS getPosition(uint32_t index);

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Converts the constant iterator of the list into the LISPOS type.
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
    /**
     * \brief   The sorting criteria of linked list.
     **/
    NECommon::eSort     mSorting;
};

//////////////////////////////////////////////////////////////////////////
// TESortedLinkedList<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE >
TESortedLinkedList<VALUE>::TESortedLinkedList(bool sortAscending /*= true*/)
    : Constless<std::list<VALUE>> ( )
    , mValueList( )
    , mSorting  (sortAscending ? NECommon::eSort::SortAscending : NECommon::eSort::SortDescending )
{
}

template <typename VALUE >
inline TESortedLinkedList<VALUE>& TESortedLinkedList<VALUE>::operator = (const TESortedLinkedList<VALUE>& src)
{
    mValueList  = src.mValueList;
    mSorting    = src.mSorting;
    return (*this);
}

template <typename VALUE >
inline TESortedLinkedList<VALUE>& TESortedLinkedList<VALUE>::operator = (TESortedLinkedList<VALUE>&& src) noexcept
{
    mValueList  = std::move(src.mValueList);
    mSorting    = src.mSorting;
    return (*this);
}

template <typename VALUE >
inline bool TESortedLinkedList<VALUE>::operator == (const TESortedLinkedList<VALUE>& other) const
{
    return (mValueList == other.mValueList);
}

template <typename VALUE >
bool TESortedLinkedList<VALUE>::operator != (const TESortedLinkedList<VALUE>& other) const
{
    return (mValueList != other.mValueList);
}

template <typename VALUE >
inline const VALUE& TESortedLinkedList<VALUE>::operator [](uint32_t atIndex) const
{
    return getAt(atIndex);
}

template <typename VALUE >
inline const VALUE& TESortedLinkedList<VALUE>::operator [] (const TESortedLinkedList<VALUE>::LISTPOS atPosition) const
{
    return valueAtPosition(atPosition);
}

template <typename VALUE >
inline bool TESortedLinkedList<VALUE>::isEmpty(void) const
{
    return mValueList.empty();
}

template <typename VALUE >
inline uint32_t TESortedLinkedList<VALUE>::getSize(void) const
{
    return static_cast<uint32_t>(mValueList.size());
}

template<typename VALUE>
inline bool TESortedLinkedList<VALUE>::isAscending( void ) const
{
    return (mSorting == NECommon::eSort::SortAscending);
}

template<typename VALUE>
inline bool TESortedLinkedList<VALUE>::isDescending(void) const
{
    return (mSorting == NECommon::eSort::SortDescending);
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::firstPosition(void) const
{
    return _citer2pos(mValueList.begin());
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::lastPosition(void) const
{
	return _citer2pos(mValueList.empty() == false ? --mValueList.end() : mValueList.end());
}

template <typename VALUE >
inline bool TESortedLinkedList<VALUE>::isFirstPosition(const TESortedLinkedList<VALUE>::LISTPOS pos) const
{
    return (pos == mValueList.begin());
}

template <typename VALUE >
inline bool TESortedLinkedList<VALUE>::isLastPosition(const TESortedLinkedList<VALUE>::LISTPOS pos) const
{
    return ((mValueList.empty() == false) && (pos == --mValueList.end()));
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::invalidPosition(void) const
{
    return _citer2pos(mValueList.end());
}

template <typename VALUE >
inline bool TESortedLinkedList<VALUE>::isInvalidPosition(const TESortedLinkedList<VALUE>::LISTPOS pos) const
{
    return (pos == mValueList.end());
}

template <typename VALUE >
inline bool TESortedLinkedList<VALUE>::isValidPosition(const TESortedLinkedList<VALUE>::LISTPOS pos) const
{
    return (pos != mValueList.end());
}

template <typename VALUE >
inline bool TESortedLinkedList<VALUE>::checkPosition(const TESortedLinkedList<VALUE>::LISTPOS pos) const
{
	auto it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template <typename VALUE >
inline void TESortedLinkedList<VALUE>::clear(void)
{
    mValueList.clear();
}

template <typename VALUE >
inline void TESortedLinkedList<VALUE>::freeExtra(void)
{
    mValueList.clear();
}

template <typename VALUE >
inline void TESortedLinkedList<VALUE>::release(void)
{
    mValueList.clear();
}

template <typename VALUE >
inline const VALUE& TESortedLinkedList<VALUE>::firstEntry(void) const
{
    ASSERT(mValueList.empty() == false);
    return mValueList.front();
}

template <typename VALUE >
inline const VALUE& TESortedLinkedList<VALUE>::lastEntry(void) const
{
    ASSERT(mValueList.empty() == false);
    return mValueList.back();
}

template<typename VALUE>
inline const VALUE& TESortedLinkedList<VALUE>::getNext(LISTPOS& IN OUT in_out_NextPosition) const
{
    return *in_out_NextPosition++;
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::nextPosition(TESortedLinkedList<VALUE>::LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return ++atPosition;
}

template <typename VALUE >
inline const VALUE & TESortedLinkedList<VALUE>::getPrev(LISTPOS & IN OUT in_out_PrevPosition) const
{
    ASSERT(in_out_PrevPosition != mValueList.end());
    LISTPOS pos = in_out_PrevPosition;
    LISTPOS beginPos = _citer2pos(mValueList.begin());
    LISTPOS endPos = _citer2pos(mValueList.end());
    in_out_PrevPosition = in_out_PrevPosition == beginPos ? endPos : --in_out_PrevPosition;
    return *pos;
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::prevPosition(TESortedLinkedList<VALUE>::LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return (atPosition == mValueList.begin() ? invalidPosition() : --atPosition);
}

template <typename VALUE >
inline const VALUE& TESortedLinkedList<VALUE>::valueAtPosition(const TESortedLinkedList<VALUE>::LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE >
inline const VALUE& TESortedLinkedList<VALUE>::getAt(uint32_t index) const
{
    LISTPOS pos = getPosition(index);
    ASSERT(isValidPosition(pos));

    return *pos;
}

template <typename VALUE >
inline const VALUE& TESortedLinkedList<VALUE>::getAt(const TESortedLinkedList<VALUE>::LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE >
bool TESortedLinkedList<VALUE>::nextEntry(TESortedLinkedList<VALUE>::LISTPOS& in_out_NextPosition, VALUE& out_NextValue) const
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
bool TESortedLinkedList<VALUE>::prevEntry(TESortedLinkedList<VALUE>::LISTPOS& in_out_PrevPosition, VALUE& out_PrevValue) const
{
    bool result = false;
    ASSERT(in_out_PrevPosition != mValueList.end());
    in_out_PrevPosition = prevPosition(in_out_PrevPosition);
    if (in_out_PrevPosition != mValueList.end())
    {
        out_PrevValue = *in_out_PrevPosition;
        result = true;
    }

    return result;
}

template <typename VALUE >
inline void TESortedLinkedList<VALUE>::removeFirst(void)
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_front();
}

template <typename VALUE >
bool TESortedLinkedList<VALUE>::removeFirst(VALUE& value)
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
inline void TESortedLinkedList<VALUE>::removeLast(void)
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_back();
}

template <typename VALUE >
bool TESortedLinkedList<VALUE>::removeLast(VALUE& value)
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
typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::add(const VALUE& newElement)
{
	auto it = mValueList.begin();
    if ( mSorting == NECommon::eSort::SortAscending )
    {
        for (; it != mValueList.end(); ++it)
        {
            if (*it > newElement)
                break;
        }
    }
    else
    {
        for (; it != mValueList.end(); ++it)
        {
            if (*it < newElement)
                break;
        }
    }

    return mValueList.insert(it, newElement);
}

template <typename VALUE >
typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::add(VALUE&& newElement)
{
	auto it = mValueList.begin();
    if (mSorting == NECommon::eSort::SortAscending)
    {
        for (; it != mValueList.end(); ++it)
        {
            if (*it > newElement)
                break;
        }
    }
    else
    {
        for (; it != mValueList.end(); ++it)
        {
            if (*it < newElement)
                break;
        }
    }

    return mValueList.insert(it, std::move(newElement));
}

template <typename VALUE >
std::pair<typename TESortedLinkedList<VALUE>::LISTPOS, bool> TESortedLinkedList<VALUE>::addIfUnique(const VALUE& newElement, bool updateExisting /*= false*/ )
{
    auto it = mValueList.begin();
    if (mSorting == NECommon::eSort::SortAscending)
    {
        while ((it != mValueList.end()) && (*it < newElement))
        {
            ++it;
        }
    }
    else
    {
        while ((it != mValueList.end()) && (*it > newElement))
        {
            ++it;
        }
    }

    std::pair<LISTPOS, bool> result( std::make_pair(it, false) );
    if ( (it == mValueList.end()) || (*it != newElement) )
    {
        result.first = mValueList.insert( it, newElement );
        result.second = true;
    }
    else if ( updateExisting )
    {
        *it = newElement;
    }

    return result;
}

template <typename VALUE >
std::pair<typename TESortedLinkedList<VALUE>::LISTPOS, bool> TESortedLinkedList<VALUE>::addIfUnique(VALUE&& newElement, bool updateExisting /*= false*/ )
{
    auto it = mValueList.begin();
    if (mSorting == NECommon::eSort::SortAscending)
    {
        while ((it != mValueList.end()) && (*it < newElement))
        {
            ++it;
        }
    }
    else
    {
        while ((it != mValueList.end()) && (*it > newElement))
        {
            ++it;
        }
    }

    std::pair<LISTPOS, bool> result( std::make_pair( it, false ) );
    if ( (it == mValueList.end( )) || (*it != newElement) )
    {
        result.first = mValueList.insert( it, newElement );
        result.second = true;
    }
    else if ( updateExisting )
    {
        *it = newElement;
    }

    return result;
}

template <typename VALUE >
inline VALUE TESortedLinkedList<VALUE>::popFirst(void)
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.front();
    mValueList.pop_front();
    return result;
}

template <typename VALUE >
inline VALUE TESortedLinkedList<VALUE>::popLast(void)
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.back();
    mValueList.pop_back();
    return result;
}

template <typename VALUE >
inline void TESortedLinkedList<VALUE>::setAt(LISTPOS atPosition, const VALUE& newValue)
{
    ASSERT(atPosition != mValueList.end());
    *atPosition = newValue;
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::removeAt(LISTPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::removeAt(LISTPOS atPosition, VALUE& out_value)
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
bool TESortedLinkedList<VALUE>::removeEntry(const VALUE& removeElement)
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
bool TESortedLinkedList<VALUE>::removeEntry(const VALUE& removeElement, TESortedLinkedList<VALUE>::LISTPOS searchAfter /*= nullptr*/)
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
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::find(const VALUE& searchValue) const
{
    return _citer2pos(std::find(mValueList.begin(), mValueList.end(), searchValue));
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::find(const VALUE& searchValue, TESortedLinkedList<VALUE>::LISTPOS searchAfter) const
{
    LISTPOS end = invalidPosition();
    return (searchAfter != end ? static_cast<LISTPOS>(std::find(++searchAfter, end, searchValue)) : end);
}

template <typename VALUE >
inline bool TESortedLinkedList<VALUE>::contains(const VALUE& elemSearch) const
{
    return contains(elemSearch, firstPosition());
}

template<typename VALUE>
inline bool TESortedLinkedList<VALUE>::contains(const VALUE& elemSearch, LISTPOS startAt) const
{
    return (startAt != mValueList.end() ? std::find(startAt, invalidPosition(), elemSearch) != mValueList.end() : false);
}

template<typename VALUE>
inline const std::list<VALUE>& TESortedLinkedList<VALUE>::getData(void) const
{
    return mValueList;
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::findIndex(uint32_t index) const
{
    return _citer2pos(index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() + index : mValueList.end());
}

template <typename VALUE >
uint32_t TESortedLinkedList<VALUE>::makeIndex(LISTPOS atPosition) const
{
    uint32_t result{ 0 };
    LISTPOS pos = mValueList.begin();
    LISTPOS end = mValueList.end();
    for (; (pos != end) && (pos != atPosition); ++pos, ++result)
        ;

    return (pos != end ? result : static_cast<uint32_t>(NECommon::INVALID_INDEX));
}

template <typename VALUE >
void TESortedLinkedList<VALUE>::resort()
{
    if (isAscending())
    {
        mValueList.sort();
    }
    else
    {
        mValueList.sort(std::greater());
    }
}

template <typename VALUE >
inline void TESortedLinkedList<VALUE>::merge(TESortedLinkedList<VALUE>& source)
{
    mValueList.sort();
    source.mValueList.sort();
    mValueList.merge(source.mValueList);
    resort();
}

template <typename VALUE >
inline void TESortedLinkedList<VALUE>::merge(TESortedLinkedList<VALUE>&& source)
{
    mValueList.sort();
    source.mValueList.sort();
    mValueList.merge(std::move(source.mValueList));
    resort();
    if (source.isEmpty() == false)
    {
        source.mSorting = mSorting;
    }
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::getPosition(uint32_t index) const
{
    auto pos = index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() : mValueList.end();
    uint32_t count = index + 1;
    for (uint32_t i = 1; i < count; ++i)
        ++pos;

    return _citer2pos(pos);
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::getPosition(uint32_t index)
{
    return static_cast<const TESortedLinkedList<VALUE> *>(this)->getPosition(index);
}

template <typename VALUE >
inline typename TESortedLinkedList<VALUE>::LISTPOS TESortedLinkedList<VALUE>::_citer2pos(typename std::list<VALUE>::const_iterator cit) const
{
    return Constless<std::list<VALUE>>::iter(mValueList, cit);
}


//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template <typename V>
const IEInStream& operator >> (const IEInStream& stream, TESortedLinkedList<V>& input)
{
    input.clear();

    uint32_t size = 0;
    uint8_t sort = 0u;
    stream >> size;
    stream >> sort;
    input.mValueList.resize(size);
    input.mSorting = static_cast<NECommon::eSort>(sort);

    for (auto& elem : input.mValueList)
    {
        stream >> elem;
    }

    return stream;
}

template <typename V>
IEOutStream& operator << (IEOutStream& stream, const TESortedLinkedList<V>& output)
{
    uint32_t size = output.getSize();
    uint8_t sort = static_cast<uint8_t>(output.mSorting);

    stream << size;
    stream << sort;

    for (const auto& elem : output.mValueList)
    {
        stream << elem;
    }

    return stream;
}

#endif  // AREG_BASE_TESORTEDLINKEDLIST_HPP
