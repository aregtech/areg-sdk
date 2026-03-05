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

    /**
     * \brief
     * \note    Move constructor. Takes ownership of the source list.
     **/
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
    inline bool operator == (const SortedLinkedList<VALUE>& other) const;

    /**
     * \brief   Returns true if the lists differ in elements or order.
     *
     * \param   other       The list to compare with.
     **/
    inline bool operator != (const SortedLinkedList<VALUE>& other) const;


    /**
     * \brief   Returns the element at the given zero-based index. Index must be valid.
     *
     * \param   atIndex     Valid zero-based index of element to access.
     * \return  Returns const reference to the element; value should not be modified.
     **/
    inline const VALUE& operator [] (uint32_t atIndex) const;

    /**
     * \brief   Returns the element at the given list position.
     *
     * \param   atPosition      Valid position in the list.
     * \return  Returns const reference to the element.
     **/
    inline const VALUE& operator [] (const LISTPOS atPosition) const;

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
     * \brief   Returns true if the list contains no elements.
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
     * \brief   Returns the position of the first element, or an invalid position if the list is
     *          empty.
     **/
    inline LISTPOS first_position() const;

    /**
     * \brief   Returns an always-invalid sentinel position marking the end of the list.
     **/
    inline LISTPOS last_position() const;

    /**
     * \brief   Returns true if the position points to the first element.
     *
     * \param   pos     The position to check.
     **/
    [[nodiscard]]
    inline bool is_first_position(const LISTPOS pos) const;

    /**
     * \brief   Returns true if the position points to the last element.
     *
     * \param   pos     The position to check.
     **/
    [[nodiscard]]
    inline bool is_last_position(const LISTPOS pos) const;

    /**
     * \brief   Returns an invalid position value representing no valid element.
     **/
    inline LISTPOS invalid_position() const;

    /**
     * \brief   Returns true if the position points to a valid element (not the end).
     *
     * \param   pos     The position to check.
     **/
    [[nodiscard]]
    inline bool is_valid_position(const LISTPOS pos) const;

    /**
     * \brief   Returns true if the position points to a valid entry in the list; may be slower due
     *          to traversal.
     *
     * \param   pos     The position to verify.
     **/
    [[nodiscard]]
    inline bool check_position(const LISTPOS pos) const;

    /**
     * \brief   Returns true if the element exists in the list.
     *
     * \param   elemSearch      The element to search for.
     **/
    inline bool contains(const VALUE& elemSearch) const;

    /**
     * \brief   Returns true if the element exists starting from the given position.
     *
     * \param   elemSearch      The element to search for.
     * \param   startAt         The position to start searching.
     **/
    inline bool contains(const VALUE& elemSearch, LISTPOS startAt) const;

    /**
     * \brief   Returns const reference to the underlying std::list container.
     **/
    inline const std::list<VALUE>& data() const;

/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief   Removes all elements from the list.
     **/
    inline void clear();

    /**
     * \brief   Deallocates unused capacity in the list.
     **/
    inline void free_extra();

    /**
     * \brief   Clears all elements and deallocates all memory.
     **/
    inline void release();

    /**
     * \brief   Returns const reference to the first element. List must not be empty.
     **/
    inline const VALUE& first_entry() const;

    /**
     * \brief   Returns const reference to the last element. List must not be empty.
     **/
    inline const VALUE& last_entry() const;

    /**
     * \brief   Returns the element at the current position and advances the position to the next
     *          element.
     *
     * \param[in,out] nextPos     On input, a valid position. On output, the position of the next
     *                            element or invalid position if at the end.
     **/
    inline const VALUE & next(LISTPOS & nextPos) const;

    /**
     * \brief   Returns the position of the next element, or invalid position if at the end.
     *
     * \param   atPosition      The current position.
     **/
    inline LISTPOS next_position(LISTPOS atPosition) const;

    /**
     * \brief   Returns the element at the current position and moves the position backward to the
     *          previous element.
     *
     * \param[in,out] prevPos     On input, a valid position. On output, the position of the
     *                            previous element or invalid position if at the beginning.
     **/
    inline const VALUE & prev(LISTPOS & prevPos) const;

    /**
     * \brief   Returns the position of the previous element, or invalid position if at the
     *          beginning.
     *
     * \param   atPosition      The current position.
     **/
    inline LISTPOS prev_position(const LISTPOS atPosition) const;

    /**
     * \brief   Returns const reference to the element at the given position.
     *
     * \param   atPosition      Valid position in the list.
     **/
    inline const VALUE& value_at(const LISTPOS atPosition) const;

    /**
     * \brief   Returns const reference to the element at the given zero-based index.
     *
     * \param   index       Valid zero-based index, must be less than size().
     **/
    inline const VALUE& value_at(uint32_t index) const;

    /**
     * \brief   Returns true if a next element exists; on success, nextPos and nextValue are
     *          updated.
     *
     * \param[in,out] nextPos         On input, a valid position. On output, the position of the
     *                                next element or invalid position if at the end.
     * \param[out] nextValue       If the function returns true, contains the next element value.
     *                             Otherwise unchanged.
     **/
    bool next_entry(LISTPOS& nextPos, VALUE& nextValue) const;

    /**
     * \brief   Returns true if a previous element exists; on success, prevPos and prevValue are
     *          updated.
     *
     * \param[in,out] prevPos         On input, a valid position. On output, the position of the
     *                                previous element or invalid position if at the beginning.
     * \param[out] prevValue       If the function returns true, contains the previous element
     *                             value. Otherwise unchanged.
     **/
    bool prev_entry(LISTPOS& prevPos, VALUE& prevValue) const;

    /**
     * \brief   Removes the first element from the list.
     **/
    inline void remove_first();

    /**
     * \brief   Removes and returns the first element. Returns false if the list was empty.
     *
     * \param[out] value       On success, contains the removed element.
     **/
    bool remove_first(VALUE& value);

    /**
     * \brief   Removes the last element from the list.
     **/
    inline void remove_last();

    /**
     * \brief   Removes and returns the last element. Returns false if the list was empty.
     *
     * \param[out] value       On success, contains the removed element.
     **/
    bool remove_last(VALUE& value);

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
     * \brief   Removes and returns the first element. List must not be empty.
     **/
    inline VALUE pop_first();

    /**
     * \brief   Removes and returns the last element. List must not be empty.
     **/
    inline VALUE pop_last();

    /**
     * \brief   Replaces the element at the given position with a new value.
     *
     * \param   atPosition      Valid position of element to replace.
     * \param   newValue        The new value to store.
     **/
    inline void set_value_at(LISTPOS atPosition, const VALUE& newValue);

    /**
     * \brief   Removes element at the given position and returns the position of the next element.
     *
     * \param   atPosition      Valid position of element to remove.
     **/
    inline LISTPOS remove_at(LISTPOS atPosition);

    /**
     * \brief   Removes element at the given position and returns the position of the next element.
     *
     * \param   atPosition      Valid position of element to remove.
     * \param[out] Value           On return, contains the removed element value.
     **/
    inline LISTPOS remove_at(LISTPOS atPosition, VALUE& Value);

    /**
     * \brief   Searches for and removes the first matching element, starting from the head.
     *
     * \param   removeElement       The element to search for and remove.
     **/
    bool remove_entry(const VALUE& removeElement);

    /**
     * \brief   Searches for and removes the first matching element, starting from the given
     *          position.
     *
     * \param   removeElement       The element to search for and remove.
     * \param   searchAfter         The position to start searching (moves to next elements during
     *                              search).
     **/
    bool remove_entry(const VALUE& removeElement, LISTPOS searchAfter);

    /**
     * \brief   Returns the position of the first element equal to the search value, or invalid
     *          position if not found.
     *
     * \param   searchValue     The element to search for.
     **/
    inline LISTPOS find(const VALUE& searchValue) const;

    /**
     * \brief   Returns the position of the first matching element starting from the given position,
     *          or invalid position if not found.
     *
     * \param   searchValue     The element to search for.
     * \param   searchAfter     If valid, search begins at the next element. If invalid, search
     *                          begins at the head.
     **/
    inline LISTPOS find(const VALUE& searchValue, LISTPOS searchAfter) const;

    /**
     * \brief   Returns the position of the element at the given zero-based index, or invalid
     *          position if index is out of bounds.
     *
     * \param   index       Zero-based index of element.
     **/
    inline LISTPOS find_index(uint32_t index) const;

    /**
     * \brief   Returns the zero-based index of the given position, or 0xFFFFFFFF if position is
     *          invalid.
     *
     * \param   atPosition      Valid position in the list.
     **/
    uint32_t make_index(LISTPOS atPosition) const;

    /**
     * \brief   Re-sorts all elements according to the current sort order.
     **/
    void resort();

    /**
     * \brief   Merges elements from source into this list without copying; source becomes empty
     *          afterward.
     *
     * \param[in,out] source      The source list to merge; becomes empty after the call.
     **/
    inline void merge(SortedLinkedList<VALUE> & source);
    /**
     * \brief   Merges elements from source into this list without copying; source becomes empty
     *          afterward.
     *
     * \param   source      The source list to merge; becomes empty after the call.
     * \note    Move overload. Takes ownership of elements.
     **/
    inline void merge(SortedLinkedList<VALUE> && source);

    /**
     * \brief   Copies up to elemCount elements into the provided buffer and returns the number of
     *          elements copied.
     *
     * \param[in,out] list            Pre-allocated buffer to store copied elements; must be large
     *                                enough for elemCount elements.
     * \param   elemCount       Maximum number of elements to copy. If zero, no elements are copied.
     **/
    inline uint32_t elements(VALUE* list, uint32_t elemCount);

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns the position of the element at the given zero-based index.
     *
     * \param   index       Zero-based index of element.
     **/
    inline LISTPOS position(uint32_t index) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Converts a std::list const_iterator to a LISTPOS position.
     *
     * \param   cit     A const_iterator from the underlying list.
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
inline bool SortedLinkedList<VALUE>::operator == (const SortedLinkedList<VALUE>& other) const
{
    return (mValueList == other.mValueList);
}

template <typename VALUE >
bool SortedLinkedList<VALUE>::operator != (const SortedLinkedList<VALUE>& other) const
{
    return (mValueList != other.mValueList);
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::operator [](uint32_t atIndex) const
{
    return value_at(atIndex);
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::operator [] (const SortedLinkedList<VALUE>::LISTPOS atPosition) const
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
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::first_position() const
{
    return _citer2pos(mValueList.begin());
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::last_position() const
{
	return _citer2pos(mValueList.empty() == false ? --mValueList.end() : mValueList.end());
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::is_first_position(const SortedLinkedList<VALUE>::LISTPOS pos) const
{
    return (pos == mValueList.begin());
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::is_last_position(const SortedLinkedList<VALUE>::LISTPOS pos) const
{
    return ((mValueList.empty() == false) && (pos == --mValueList.end()));
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::invalid_position() const
{
    return _citer2pos(mValueList.end());
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::is_valid_position(const SortedLinkedList<VALUE>::LISTPOS pos) const
{
    return (pos != mValueList.end());
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::check_position(const SortedLinkedList<VALUE>::LISTPOS pos) const
{
	auto it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::clear()
{
    mValueList.clear();
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::free_extra()
{
    mValueList.clear();
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::release()
{
    mValueList.clear();
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::first_entry() const
{
    ASSERT(mValueList.empty() == false);
    return mValueList.front();
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::last_entry() const
{
    ASSERT(mValueList.empty() == false);
    return mValueList.back();
}

template<typename VALUE>
inline const VALUE& SortedLinkedList<VALUE>::next(LISTPOS& nextPos) const
{
    return *nextPos++;
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::next_position(SortedLinkedList<VALUE>::LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return ++atPosition;
}

template <typename VALUE >
inline const VALUE & SortedLinkedList<VALUE>::prev(LISTPOS & prevPos) const
{
    ASSERT(prevPos != mValueList.end());
    LISTPOS pos = prevPos;
    LISTPOS beginPos = _citer2pos(mValueList.begin());
    LISTPOS endPos = _citer2pos(mValueList.end());
    prevPos = prevPos == beginPos ? endPos : --prevPos;
    return *pos;
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::prev_position(SortedLinkedList<VALUE>::LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    typename std::list<VALUE>::iterator it = static_cast<typename std::list<VALUE>::iterator>(atPosition);
    return (atPosition == mValueList.begin() ? invalid_position() : static_cast<SortedLinkedList<VALUE>::LISTPOS >(--it));
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::value_at(const SortedLinkedList<VALUE>::LISTPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return *atPosition;
}

template <typename VALUE >
inline const VALUE& SortedLinkedList<VALUE>::value_at(uint32_t index) const
{
    LISTPOS pos = position(index);
    ASSERT(is_valid_position(pos));

    return *pos;
}

template <typename VALUE >
bool SortedLinkedList<VALUE>::next_entry(SortedLinkedList<VALUE>::LISTPOS& nextPos, VALUE& nextValue) const
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
bool SortedLinkedList<VALUE>::prev_entry(SortedLinkedList<VALUE>::LISTPOS& prevPos, VALUE& prevValue) const
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
inline void SortedLinkedList<VALUE>::remove_first()
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_front();
}

template <typename VALUE >
bool SortedLinkedList<VALUE>::remove_first(VALUE& value)
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
inline void SortedLinkedList<VALUE>::remove_last()
{
    ASSERT(mValueList.empty() == false);
    mValueList.pop_back();
}

template <typename VALUE >
bool SortedLinkedList<VALUE>::remove_last(VALUE& value)
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
inline VALUE SortedLinkedList<VALUE>::pop_first()
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.front();
    mValueList.pop_front();
    return result;
}

template <typename VALUE >
inline VALUE SortedLinkedList<VALUE>::pop_last()
{
    ASSERT(mValueList.empty() == false);
    VALUE result = mValueList.back();
    mValueList.pop_back();
    return result;
}

template <typename VALUE >
inline void SortedLinkedList<VALUE>::set_value_at(LISTPOS atPosition, const VALUE& newValue)
{
    ASSERT(atPosition != mValueList.end());
    *atPosition = newValue;
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::remove_at(LISTPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::remove_at(LISTPOS atPosition, VALUE& out_value)
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
bool SortedLinkedList<VALUE>::remove_entry(const VALUE& removeElement)
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
bool SortedLinkedList<VALUE>::remove_entry(const VALUE& removeElement, SortedLinkedList<VALUE>::LISTPOS searchAfter /*= nullptr*/)
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
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::find(const VALUE& searchValue) const
{
    return _citer2pos(std::find(mValueList.begin(), mValueList.end(), searchValue));
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::find(const VALUE& searchValue, SortedLinkedList<VALUE>::LISTPOS searchAfter) const
{
    LISTPOS end = invalid_position();
    return (searchAfter != end ? static_cast<LISTPOS>(std::find(++searchAfter, end, searchValue)) : end);
}

template <typename VALUE >
inline bool SortedLinkedList<VALUE>::contains(const VALUE& elemSearch) const
{
    return contains(elemSearch, first_position());
}

template<typename VALUE>
inline bool SortedLinkedList<VALUE>::contains(const VALUE& elemSearch, LISTPOS startAt) const
{
    return (startAt != mValueList.end() ? std::find(startAt, invalid_position(), elemSearch) != mValueList.end() : false);
}

template<typename VALUE>
inline const std::list<VALUE>& SortedLinkedList<VALUE>::data() const
{
    return mValueList;
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::find_index(uint32_t index) const
{
    return _citer2pos(index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() + index : mValueList.end());
}

template <typename VALUE >
uint32_t SortedLinkedList<VALUE>::make_index(LISTPOS atPosition) const
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
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::position(uint32_t index) const
{
    auto pos = index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() : mValueList.end();
    for (uint32_t i = 1; i <= index; ++i)
    {
        ++pos;
    }

    return _citer2pos(pos);
}

template <typename VALUE >
inline typename SortedLinkedList<VALUE>::LISTPOS SortedLinkedList<VALUE>::_citer2pos(typename std::list<VALUE>::const_iterator cit) const
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
