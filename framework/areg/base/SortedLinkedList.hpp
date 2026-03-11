#ifndef AREG_BASE_SORTEDLINKEDLIST_HPP
#define AREG_BASE_SORTEDLINKEDLIST_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SortedLinkedList.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Sorted Linked List class template.
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
#include "areg/base/areg_global.h"
#include "areg/base/TemplateBase.hpp"
#include "areg/base/IOStream.hpp"

#include <algorithm>
#include <list>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// SortedLinkedList<VALUE, VALUE_TYPE, Sorter> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Bi-directional linked list with elements automatically sorted in ascending or descending
 *          order during insertion. Not thread-safe; synchronization must be handled externally.
 **/
template <typename VALUE>
class SortedLinkedList : private Constless<std::list<VALUE>>
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
     * \brief   Initializes an empty sorted linked list with the specified sort order.
     **/
    explicit SortedLinkedList(bool sortAscending = true);

    SortedLinkedList(const SortedLinkedList<VALUE>& src) = default;

    SortedLinkedList(SortedLinkedList<VALUE>&& src) noexcept = default;

    ~SortedLinkedList() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Assigns elements from src to this list, replacing existing content.
     *
     * \param   src     Source list to copy elements from.
     **/
    inline SortedLinkedList<VALUE>& operator = (const SortedLinkedList<VALUE>& src);

    /**
     * \brief   Moves elements from src to this list, replacing existing content.
     *
     * \param   src     Source list to move elements from.
     **/
    inline SortedLinkedList<VALUE>& operator = (SortedLinkedList<VALUE>&& src) noexcept;

    /**
     * \brief   Returns true if both lists have identical elements in identical order.
     *
     * \param   other       The list to compare with.
     **/
    [[nodiscard]]
    inline bool operator == (const SortedLinkedList<VALUE>& other) const noexcept;

    /**
     * \brief   Returns true if the lists differ in elements or order.
     *
     * \param   other       The list to compare with.
     **/
    [[nodiscard]]
    inline bool operator != (const SortedLinkedList<VALUE>& other) const noexcept;


    /**
     * \brief   Returns the element at the given zero-based index. Index must be valid.
     *
     * \param   atIndex     Valid zero-based index of element to access.
     * \return  Returns const reference to the element; value should not be modified.
     **/
    [[nodiscard]]
    inline const VALUE& operator [] (uint32_t atIndex) const noexcept;

    /**
     * \brief   Returns the element at the given list position.
     *
     * \param   atPosition      Valid position in the list.
     * \return  Returns const reference to the element.
     **/
    [[nodiscard]]
    inline const VALUE& operator [] (const LISTPOS atPosition) const noexcept;

/************************************************************************/
// Friend global operators to make Linked List streamable
/************************************************************************/

    /**
     * \brief   Reads list elements from the input stream, clearing any existing elements first.
     *
     * \param   stream      The input stream to read from.
     * \param[in,out] input       The list to store deserialized elements. Existing elements are
     *                            cleared.
     **/
    template<typename V>
    friend const InStream& operator >> (const InStream& stream, SortedLinkedList<V>& input);
    /**
     * \brief   Writes all list elements to the output stream in head-to-tail order.
     *
     * \param[in,out] stream      The output stream to write to.
     * \param   output      The list whose elements will be serialized.
     **/
    template<typename V>
    friend OutStream& operator << (OutStream& stream, const SortedLinkedList<V>& output);

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Attributes
/************************************************************************/

    /**
     * \brief   Returns true if empty.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Returns the number of elements in the list.
     **/
    [[nodiscard]]
    inline uint32_t size() const noexcept;

    /**
     * \brief   Returns true if elements are sorted in ascending order.
     **/
    [[nodiscard]]
    inline bool is_ascending() const noexcept;

    /**
     * \brief   Returns true if elements are sorted in descending order.
     **/
    [[nodiscard]]
    inline bool is_descending() const noexcept;

    /**
     * \brief   Returns the position of the first element, or an invalid position if the list is empty.
     **/
    [[nodiscard]]
    inline LISTPOS first_position() const noexcept;

    /**
     * \brief   Returns the position of the last value entry in the list, which is not
     *          invalid if the linked list is not empty. Otherwise, returns invalid position.
     **/
    [[nodiscard]]
    inline LISTPOS last_position() const noexcept;

    /**
     * \brief   Returns true if the position points to the first element.
     *
     * \param   pos     The position to check.
     **/
    [[nodiscard]]
    inline bool is_first_position(const LISTPOS pos) const noexcept;

    /**
     * \brief   Returns true if the position points to the last element.
     *
     * \param   pos     The position to check.
     **/
    [[nodiscard]]
    inline bool is_last_position(const LISTPOS pos) const noexcept;

    /**
     * \brief   Returns an invalid position value representing no valid element.
     **/
    [[nodiscard]]
    inline LISTPOS invalid_position() const noexcept;

    /**
     * \brief   Returns true if the position points to a valid element (not the end).
     *
     * \param   pos     The position to check.
     **/
    [[nodiscard]]
    inline bool is_valid_position(const LISTPOS pos) const noexcept;

    /**
     * \brief   Returns true if the position points to a valid entry in the list; may be slower due
     *          to traversal.
     *
     * \param   pos     The position to verify.
     **/
    [[nodiscard]]
    inline bool check_position(const LISTPOS pos) const noexcept;

    /**
     * \brief   Returns true if the element exists in the list.
     *
     * \param   elemSearch      The element to search for.
     **/
    [[nodiscard]]
    inline bool contains(const VALUE& elemSearch) const noexcept;

    /**
     * \brief   Returns true if the element exists starting from the given position.
     *
     * \param   elemSearch      The element to search for.
     * \param   startAt         The position to start searching.
     **/
    [[nodiscard]]
    inline bool contains(const VALUE& elemSearch, LISTPOS startAt) const noexcept;

    /**
     * \brief   Returns std::list container.
     **/
    [[nodiscard]]
    inline const std::list<VALUE>& data() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Removes all elements from the list.
     **/
    inline void clear() noexcept;

    /**
     * \brief   Deallocates unused capacity in the list.
     **/
    inline void free_extra() noexcept;

    /**
     * \brief   Clears all elements and deallocates all memory.
     **/
    inline void release() noexcept;

    /**
     * \brief   Returns the value of the head element in the linked list. The linked list must not be empty.
     **/
    [[nodiscard]]
    inline const VALUE& first_entry() const noexcept;

    /**
     * \brief   Returns the value of the tail element in the linked list. The linked list must not be empty.
     **/
    [[nodiscard]]
    inline const VALUE& last_entry() const noexcept;

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
    inline const VALUE& next( LISTPOS& nextPos ) const noexcept;

    /**
     * \brief   Returns either valid position of the next entry in the Linked List or invalid
     *          position if reached end of the linked list.
     *
     * \param   atPosition      Actual position to get the next position. Should be valid position.
     **/
    [[nodiscard]]
    inline LISTPOS next_position( LISTPOS atPosition ) const noexcept;

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
    inline const VALUE& prev( LISTPOS & prevPos ) const noexcept;

    /**
     * \brief   Returns either valid position of the previous entry in the Linked List or invalid
     *          position if it indicates the first entry (no previous entry) of the linked list.
     *
     * \param   atPosition      Actual position to get the previous position. Should be valid
     *                          position.
     **/
    [[nodiscard]]
    inline LISTPOS prev_position( LISTPOS atPosition ) const noexcept;

    /**
     * \brief   Returns the value of entry at the given valid position. The position must be valid.
     *          The returned value can be used by right (r-value) operation.
     *
     * \param   atPosition      The valid position in Linked List
     * \return  Returns the value of entry at the given valid position.
     **/
    [[nodiscard]]
    inline const VALUE & value_at( const LISTPOS atPosition ) const noexcept;

    /**
     * \brief   Returns element value by valid zero-based index, which can be used by right
     *          operation (r-value). The index should be valid.
     **/
    [[nodiscard]]
    inline const VALUE& value_at(uint32_t index ) const noexcept;

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
    inline bool next_entry( LISTPOS& nextPos, VALUE& nextValue ) const;

    /**
     * \brief   Extracts the previous position and value of the element preceding the given position.
     *
     * \param[in,out] prevPos         On input this indicates the valid position of the entry in the
     *                                linked list. On output, this parameter points either previous
     *                                valid entry in the linked list or invalid entry if has no
     *                                previous entry, i.e. indicates first entry.
     * \param[out] prevValue       On output, this contains value of the previous entry in the
     *                             linked list.
     * \return  Returns true if there is a previous element and the output value is valid.
     **/
    inline bool prev_entry( LISTPOS& prevPos, VALUE& prevValue ) const;

    /**
     * \brief   Removes the head element from the linked list.
     **/
    inline void remove_first() noexcept;

    /**
     * \brief   Removes the head element from the linked list. On output, the 'value' contains data
     *          of removed element. Function returns true if linked list was not empty and the
     *          'value' contains the data of removed element.
     *
     * \param[out] value       On output, this contains the data of the removed element.
     * \return  Returns true if linked list was not empty and the 'value' contains the data of
     *          removed element.
     **/
    inline bool remove_first( VALUE& value );

    /**
     * \brief   Removes the tail element from the linked list.
     **/
    inline void remove_last() noexcept;

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
     * \brief   Inserts element at its sorted position and returns the position where inserted.
     *
     * \param   newElement      Element to insert.
     **/
    LISTPOS add(const VALUE& newElement);
    /**
     * \brief   Inserts element at its sorted position and returns the position where inserted.
     *
     * \param   newElement      Element to move and insert.
     * \note    Move overload. Takes ownership of the element.
     **/
    LISTPOS add(VALUE&& newElement);

    /**
     * \brief   Inserts element only if not already in the list; returns position and insertion
     *          success flag.
     *
     * \param   newElement          Element to conditionally insert.
     * \param   updateExisting      If true and element exists, the existing element is updated with
     *                              the new value.
     **/
    std::pair<LISTPOS, bool> add_if_unique(const VALUE& newElement, bool updateExisting = false );

    /**
     * \brief   Inserts element only if not already in the list; returns position and insertion
     *          success flag.
     *
     * \param   newElement          Element to move and conditionally insert.
     * \param   updateExisting      If true and element exists, the existing element is updated with
     *                              the new value.
     * \note    Move overload. Takes ownership of the element.
     **/
    std::pair<LISTPOS, bool> add_if_unique(VALUE&& newElement, bool updateExisting = false );

    /**
     * \brief   Pops and returns the element at the head of linked list. The linked list must not be empty.
     **/
    inline VALUE pop_first() noexcept;

    /**
     * \brief   Pops and returns the element at the tail of linked list. The linked list must not be empty.
     **/
    inline VALUE pop_last() noexcept;

    /**
     * \brief   Removes element at given position and returns position of the next entry in the
     *          linked list. Returns invalid position if tail entry is removed.
     *
     * \param   atPosition      Position of the element to remove from Linked List.
     * \return  Returns the position of the next entry in the linked-list. If the tail entry is
     *          removed, returns invalid position.
     **/
    inline LISTPOS remove_at( LISTPOS atPosition ) noexcept;

    /**
     * \brief   Removes element at given position and returns position of the next entry in the
     *          linked list. Returns invalid position if tail entry is removed.
     *
     * \param   atPosition      Position of the element to remove from Linked List.
     * \param[out] Value           On output, it contains value of removed element
     * \return  Returns the position of the next entry in the linked-list. If the tail entry is
     *          removed, returns invalid position.
     **/
    inline LISTPOS remove_at( LISTPOS atPosition, VALUE& Value );

    /**
     * \brief   Searches and removes first match of entry, which value is equal to the given
     *          element. Returns true if found and removed entry with success.
     *
     * \param   removeElement       Element to search and remove from Linked List
     * \return  Returns true if found and removed entry.
     **/
    inline bool remove_entry( const VALUE& removeElement ) noexcept;

    /**
     * \brief   Searches and removes first match of entry, which value is equal to the given
     *          element, starting from the given position. Returns true if found and removed entry
     *          with success.
     *
     * \param   removeElement       Element to search and remove from Linked List
     * \param   searchAfter         The valid position in the linked list to start searching.
     * \return  Returns true if found and removed entry.
     **/
    inline bool remove_entry(const VALUE& removeElement, LISTPOS searchAfter) noexcept;

    /**
     * \brief   Searches position of the entry by given value and returns valid position if found an
     *          entry. Otherwise, it returns invalid position.
     *
     * \param   searchValue     Value of element to search.
     * \return  If found, returns valid position of the entry in the linked-list. Otherwise, returns
     *          invalid position.
     **/
    [[nodiscard]]
    inline LISTPOS find(const VALUE& searchValue) const noexcept;

    /**
     * \brief   Searches position of the entry by given value starting from the given position and
     *          returns valid position if found an entry. Otherwise, it returns invalid position.
     *
     * \param   searchValue     Value of element to search.
     * \param   searchAfter     The valid position in the linked list to start searching.
     * \return  If found, returns valid position of the entry in the linked-list. Otherwise, returns
     *          invalid position.
     **/
    [[nodiscard]]
    inline LISTPOS find( const VALUE& searchValue, LISTPOS searchAfter ) const noexcept;

    /**
     * \brief   Returns valid position of the element by given zero-based index. Returns invalid
     *          position if the index is invalid.
     *
     * \param   index       The index of element in Linked List to get position.
     **/
    [[nodiscard]]
    inline LISTPOS find_index( uint32_t index ) const noexcept;

    /**
     * \brief   Returns valid zero-based index of the element by given valid position. Returns
     *          invalid index (0xFFFFFFFF) if the position is invalid.
     *
     * \param   atPosition      The valid position in the linked list to return index.
     **/
    [[nodiscard]]
    inline uint32_t make_index( LISTPOS atPosition ) const noexcept;

    /**
     * \brief   Merges entries from source into this list. Both lists should be sorted. The source
     *          list becomes empty after merge.
     *
     * \param   source      The source linked list to merge.
     * \note    For equivalent elements in the two lists, elements from this list precede elements
     *          from the source, preserving the sort order.
     **/
    inline void merge(SortedLinkedList<VALUE>& source);

    /**
     * \brief   Merges entries from source into this list. Both lists should be sorted. The source
     *          list becomes empty after merge.
     *
     * \param   source      The source linked list to merge.
     * \note    Move overload. Takes ownership of the source list.
     * \note    For equivalent elements in the two lists, elements from this list precede elements
     *          from the source, preserving the sort order.
     **/
    inline void merge(SortedLinkedList<VALUE>&& source);

    /**
     * \brief   Re-sorts all elements according to the current sort order.
     **/
    void resort();

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
    [[nodiscard]]
    inline LISTPOS position(uint32_t index) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Converts the constant iterator of the list into the LISTPOS type.
     * \param   cit     The constant iterator of the list.
     **/
    [[nodiscard]]
    inline constexpr LISTPOS _citer2pos(typename std::list<VALUE>::const_iterator cit) const noexcept;

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
    areg::SortOrder mSorting;
};

//////////////////////////////////////////////////////////////////////////
// SortedLinkedList<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE >
SortedLinkedList<VALUE>::SortedLinkedList(bool sortAscending /*= true*/)
    : Constless<std::list<VALUE>> ( )
    , mValueList( )
    , mSorting  (sortAscending ? areg::SortOrder::Ascending : areg::SortOrder::Descending )
{
}

template <typename VALUE >
inline SortedLinkedList<VALUE>& SortedLinkedList<VALUE>::operator = (const SortedLinkedList<VALUE>& src)
{
    mValueList  = src.mValueList;
    mSorting    = src.mSorting;
    return (*this);
}

template <typename VALUE >
inline SortedLinkedList<VALUE>& SortedLinkedList<VALUE>::operator = (SortedLinkedList<VALUE>&& src) noexcept
{
    mValueList  = std::move(src.mValueList);
    mSorting    = src.mSorting;
    return (*this);
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::operator == (const SortedLinkedList<VALUE>& other) const noexcept
{
    return (mValueList == other.mValueList);
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::operator != (const SortedLinkedList<VALUE>& other) const noexcept
{
    return (mValueList != other.mValueList);
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::operator [](uint32_t atIndex) const noexcept
{
    return value_at(atIndex);
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::operator [] (const SortedLinkedList<VALUE>::LISTPOS atPosition) const noexcept
{
    return value_at(atPosition);
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::is_empty() const noexcept
{
    return mValueList.empty();
}

template <typename VALUE >
inline uint32_t SortedLinkedList<VALUE>::size() const noexcept
{
    return static_cast<uint32_t>(mValueList.size());
}

template<typename VALUE>
inline bool SortedLinkedList<VALUE>::is_ascending() const noexcept
{
    return (mSorting == areg::SortOrder::Ascending);
}

template<typename VALUE>
inline bool SortedLinkedList<VALUE>::is_descending() const noexcept
{
    return (mSorting == areg::SortOrder::Descending);
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::first_position() const noexcept
{
    return _citer2pos(mValueList.begin());
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::last_position() const noexcept
{
    return _citer2pos(mValueList.empty() == false ? --mValueList.end() : mValueList.end());
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::is_first_position(const SortedLinkedList<VALUE>::LISTPOS pos) const noexcept
{
    return (pos == mValueList.begin());
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::is_last_position(const SortedLinkedList<VALUE>::LISTPOS pos) const noexcept
{
    return ((mValueList.empty() == false) && (pos == --mValueList.end()));
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::invalid_position() const noexcept
{
    return _citer2pos(mValueList.end());
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::is_valid_position(const SortedLinkedList<VALUE>::LISTPOS pos) const noexcept
{
    return (pos != mValueList.end());
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::check_position(const SortedLinkedList<VALUE>::LISTPOS pos) const noexcept
{
    auto it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::clear() noexcept
{
    mValueList.clear();
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::free_extra() noexcept
{
    mValueList.clear();
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::release() noexcept
{
    mValueList.clear();
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::first_entry() const noexcept
{
    ASSERT(mValueList.empty() == false);
    return mValueList.front();
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::last_entry() const noexcept
{
    ASSERT(mValueList.empty() == false);
    return mValueList.back();
}

template<typename VALUE>
inline const VALUE& SortedLinkedList<VALUE>::next(LISTPOS& nextPos) const noexcept
{
    return *nextPos++;
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::next_position(SortedLinkedList<VALUE>::LISTPOS atPosition) const noexcept
{
    ASSERT(atPosition != mValueList.end());
    return ++atPosition;
}

template <typename VALUE >
inline const VALUE & SortedLinkedList<VALUE>::prev(LISTPOS & prevPos) const noexcept
{
    ASSERT(prevPos != mValueList.end());
    LISTPOS pos = prevPos;
    prevPos = (prevPos == mValueList.begin()) ? mValueList.end() : --prevPos;
    return *pos;
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::prev_position(SortedLinkedList<VALUE>::LISTPOS atPosition) const noexcept
{
    ASSERT(atPosition != mValueList.end());
    return (atPosition == mValueList.begin() ? mValueList.end() : --atPosition);
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::value_at(const SortedLinkedList<VALUE>::LISTPOS atPosition) const noexcept
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::value_at(uint32_t index) const noexcept
{
    LISTPOS pos = position(index);
    ASSERT(is_valid_position(pos));

    return *pos;
}

template <typename VALUE >
bool SortedLinkedList<VALUE>::next_entry(SortedLinkedList<VALUE>::LISTPOS& nextPos, VALUE& nextValue) const
{
    ASSERT(nextPos != mValueList.end());
    nextPos = next_position(nextPos);
    if (nextPos != mValueList.end())
    {
        nextValue = *nextPos;
        return true;
    }

    return false;
}

template <typename VALUE >
bool SortedLinkedList<VALUE>::prev_entry(SortedLinkedList<VALUE>::LISTPOS& prevPos, VALUE& prevValue) const
{
    ASSERT(prevPos != mValueList.end());
    prevPos = prev_position(prevPos);
    if (prevPos != mValueList.end())
    {
        prevValue = *prevPos;
        return true;
    }

    return false;
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::remove_first() noexcept
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_front();
}

template <typename VALUE >
bool SortedLinkedList<VALUE>::remove_first(VALUE& value)
{
    if (mValueList.empty())
        return false;

    value = std::move(mValueList.front());
    mValueList.pop_front();
    return true;
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::remove_last() noexcept
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_back();
}

template <typename VALUE >
bool SortedLinkedList<VALUE>::remove_last(VALUE& value)
{
    if (mValueList.empty())
        return false;

    value = std::move(mValueList.back());
    mValueList.pop_back();
    return true;
}

template <typename VALUE >
typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::add(const VALUE& newElement)
{
	auto it = mValueList.begin();
    if ( mSorting == areg::SortOrder::Ascending )
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
typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::add(VALUE&& newElement)
{
	auto it = mValueList.begin();
    if (mSorting == areg::SortOrder::Ascending)
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
std::pair<typename SortedLinkedList<VALUE>::LISTPOS, bool> SortedLinkedList<VALUE>::add_if_unique(const VALUE& newElement, bool updateExisting /*= false*/ )
{
    auto it = mValueList.begin();
    if (mSorting == areg::SortOrder::Ascending)
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
std::pair<typename SortedLinkedList<VALUE>::LISTPOS, bool> SortedLinkedList<VALUE>::add_if_unique(VALUE&& newElement, bool updateExisting /*= false*/ )
{
    auto it = mValueList.begin();
    if (mSorting == areg::SortOrder::Ascending)
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
        result.first = mValueList.insert( it, std::move(newElement) );
        result.second = true;
    }
    else if ( updateExisting )
    {
        *it = std::move(newElement);
    }

    return result;
}

template <typename VALUE >
inline VALUE SortedLinkedList<VALUE>::pop_first() noexcept
{
    ASSERT(mValueList.empty() == false);
    VALUE result = std::move(mValueList.front());
    mValueList.pop_front();
    return result;
}

template <typename VALUE >
inline VALUE SortedLinkedList<VALUE>::pop_last() noexcept
{
    ASSERT(mValueList.empty() == false);
    VALUE result = std::move(mValueList.back());
    mValueList.pop_back();
    return result;
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::remove_at(LISTPOS atPosition) noexcept
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::remove_at(LISTPOS atPosition, VALUE& out_value)
{
    if (atPosition == mValueList.end())
        return mValueList.end();

    out_value = std::move(*atPosition);
    return mValueList.erase(atPosition);
}

template <typename VALUE >
bool SortedLinkedList<VALUE>::remove_entry(const VALUE& removeElement) noexcept
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
bool SortedLinkedList<VALUE>::remove_entry(const VALUE& removeElement, SortedLinkedList<VALUE>::LISTPOS searchAfter /*= nullptr*/) noexcept
{
    if (searchAfter == mValueList.end())
        return false;

    auto it = std::find(++searchAfter, mValueList.end(), removeElement);
    if (it == mValueList.end())
        return false;

    mValueList.erase(it);
    return true;
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::find(const VALUE& searchValue) const noexcept
{
    return _citer2pos(std::find(mValueList.begin(), mValueList.end(), searchValue));
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::find(const VALUE& searchValue, SortedLinkedList<VALUE>::LISTPOS searchAfter) const noexcept
{
    if (searchAfter == mValueList.end())
        return mValueList.end();

    return static_cast<LISTPOS>(std::find(++searchAfter, mValueList.end(), searchValue));
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::contains(const VALUE& elemSearch) const noexcept
{
    return contains(elemSearch, first_position());
}

template<typename VALUE>
inline bool SortedLinkedList<VALUE>::contains(const VALUE& elemSearch, LISTPOS startAt) const noexcept
{
    return (startAt != mValueList.end() ? std::find(startAt, mValueList.end(), elemSearch) != mValueList.end() : false);
}

template<typename VALUE>
inline const std::list<VALUE>& SortedLinkedList<VALUE>::data() const noexcept
{
    return mValueList;
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::find_index(uint32_t index) const noexcept
{
    if (index >= static_cast<uint32_t>(mValueList.size()))
        return _citer2pos(mValueList.end());

    auto it = mValueList.begin();
    std::advance(it, static_cast<std::ptrdiff_t>(index));
    return _citer2pos(it);
}

template <typename VALUE >
uint32_t SortedLinkedList<VALUE>::make_index(LISTPOS atPosition) const noexcept
{
    uint32_t result{ 0 };
    LISTPOS pos = mValueList.begin();
    LISTPOS end = mValueList.end();
    for (; (pos != end) && (pos != atPosition); ++pos, ++result)
        ;

    return (pos != end ? result : static_cast<uint32_t>(areg::INVALID_INDEX));
}

template <typename VALUE >
void SortedLinkedList<VALUE>::resort()
{
    if (is_ascending())
    {
        mValueList.sort();
    }
    else
    {
        mValueList.sort(std::greater<VALUE>());
    }
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::merge(SortedLinkedList<VALUE>& source)
{
    mValueList.sort();
    source.mValueList.sort();
    mValueList.merge(source.mValueList);
    resort();
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::merge(SortedLinkedList<VALUE>&& source)
{
    mValueList.sort();
    source.mValueList.sort();
    mValueList.merge(std::move(source.mValueList));
    resort();
    if (source.is_empty() == false)
    {
        source.mSorting = mSorting;
    }
}

template<typename VALUE>
inline uint32_t SortedLinkedList<VALUE>::elements(VALUE* list, uint32_t elemCount)
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
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::position(uint32_t index) const noexcept
{
    auto pos = index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() : mValueList.end();
    for (uint32_t i = 1; i <= index; ++i)
    {
        ++pos;
    }

    return _citer2pos(pos);
}

template <typename VALUE >
inline constexpr typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::_citer2pos(typename std::list<VALUE>::const_iterator cit) const noexcept
{
    return Constless<std::list<VALUE>>::iter(mValueList, cit);
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template <typename V>
const areg::InStream& operator >> (const areg::InStream& stream, areg::SortedLinkedList<V>& input)
{
    input.clear();

    uint32_t size{ 0u };
    uint8_t sort{ 0u };
    stream >> size;
    stream >> sort;
    input.mValueList.resize(size);
    input.mSorting = static_cast<areg::SortOrder>(sort);

    for (auto& elem : input.mValueList)
    {
        stream >> elem;
    }

    return stream;
}

template <typename V>
areg::OutStream& operator << (areg::OutStream& stream, const areg::SortedLinkedList<V>& output)
{
    uint32_t size = output.size();
    uint8_t sort = static_cast<uint8_t>(output.mSorting);

    stream << size;
    stream << sort;

    for (const auto& elem : output.mValueList)
    {
        stream << elem;
    }

    return stream;
}

} // namespace areg

#endif  // AREG_BASE_SORTEDLINKEDLIST_HPP
