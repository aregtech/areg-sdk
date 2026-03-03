#ifndef AREG_BASE_ARRAYLIST_HPP
#define AREG_BASE_ARRAYLIST_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/ArrayList.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Array List class template
 *              This class template defines array of elements.
 *              It allows to store elements in array, making general
 *              operations and having access by integer index value.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/TemplateBase.hpp"
#include <algorithm>
#include <vector>

#include "areg/base/CommonDefs.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/IOStream.hpp"

#include <algorithm>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ArrayList< VALUE > class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Array List has general functionalities to access, insert, move, find
 *          and copy elements. The elements of array are fast accessed for read
 *          and modification by using index. Use add() method if to add new element.
 *
 *          The type VALUE should have at least default constructor, applicable
 *          comparing and assigning operators. The ArrayList object is not
 *          thread safe and data access should be synchronized manually.
 *
 *          For performance issue, it is recommended to pass capacity value
 *          in constructor to define initial reserved space for array.
 *          By default, the capacity is areg::ARRAY_DEFAULT_CAPACITY.
 *
 * \tparam  VALUE   The type of stored elements should be either primitive or have
 *                  default constructor, applicable comparing and assigning operators.
 **/
template<typename VALUE>
class ArrayList   : private Constless<std::vector<VALUE>>
{
protected:
    using ARRAYPOS  = typename std::vector<VALUE>::iterator;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs object and sets the initial capacity of the ArrayList.
     *
     * \param   capacity    Initial reserved space of array, it must be not less than the 'resize'.
     * \param   resize      Initial size of array.
     **/
    ArrayList( uint32_t capacity = areg::ARRAY_DEFAULT_CAPACITY, uint32_t resize = 0 );

    /**
     * \brief   Copies entries from given source.
     *
     * \param   src     The source to copy data.
     **/
    ArrayList( const ArrayList< VALUE > & src );
    /**
     * \brief   Copies entries from given source.
     *
     * \param   src     The source to copy data.
     **/
    ArrayList( const std::vector< VALUE > & src );

    /**
     * \brief   Constructs array from given array of objects.
     *
     * \param   list        The list of entries to copy.
     * \param   count       The number of entries in the array.
     **/
    ArrayList( const VALUE * list, uint32_t count );

    /**
     * \brief   Moves entries from given source.
     *
     * \param   src     The source to move data.
     **/
    ArrayList( ArrayList< VALUE > && src ) noexcept;
    /**
     * \brief   Moves entries from given source.
     *
     * \param   src     The source to move data.
     **/
    ArrayList( std::vector< VALUE > && src ) noexcept;

    /**
     * \brief   Destructor.
     **/
    ~ArrayList();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// basic operators
/************************************************************************/

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid zero-based
     *          index. May be used on either the right (r-value) or the left (l-value) of an
     *          assignment statement.
     **/
    inline VALUE & operator [] ( uint32_t index );

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid zero-based
     *          index. The index should be valid number between 0 and (mSize -1). May be used on the
     *          right (r-value).
     **/
    inline const VALUE & operator [] (uint32_t index ) const;

    /**
     * \brief   Assigning operator. Copies all values from given source. If array previously had
     *          values, they will be removed and new values from source array will be set in the
     *          same sequence as they are present in the source.
     *
     * \param   src     The source of array of values.
     **/
    inline ArrayList< VALUE > & operator = ( const ArrayList< VALUE > & src );
    /**
     * \brief   Assigning operator. Copies all values from given source. If array previously had
     *          values, they will be removed and new values from source array will be set in the
     *          same sequence as they are present in the source.
     *
     * \param   src     The source of array of values.
     **/
    inline ArrayList< VALUE > & operator = ( const std::vector< VALUE > & src );

    /**
     * \brief   Move operator. Moves all values from given source. If Array previously had values,
     *          they will be removed and new values from source Array will be set in the same
     *          sequence as they are present in the source.
     *
     * \param   src     The source of array of values.
     **/
    inline ArrayList< VALUE > & operator = ( ArrayList< VALUE > && src ) noexcept;
    /**
     * \brief   Move operator. Moves all values from given source. If Array previously had values,
     *          they will be removed and new values from source Array will be set in the same
     *          sequence as they are present in the source.
     *
     * \param   src     The source of array of values.
     **/
    inline ArrayList< VALUE > & operator = ( std::vector< VALUE > && src ) noexcept;

    /**
     * \brief   Returns true if the arrays are equal. The VALUE type must support comparison.
     *
     * \param   other       The array object to compare.
     **/
    inline bool operator == ( const ArrayList< VALUE > & other ) const;
    /**
     * \brief   Returns true if the arrays are equal. The VALUE type must support comparison.
     *
     * \param   other       The array object to compare.
     **/
    inline bool operator == ( const std::vector< VALUE > & other ) const;

    /**
     * \brief   Returns true if the arrays are not equal. The VALUE type must support comparison.
     *
     * \param   other       The array object to compare.
     **/
    inline bool operator != ( const ArrayList< VALUE > & other ) const;
    /**
     * \brief   Returns true if the arrays are not equal. The VALUE type must support comparison.
     *
     * \param   other       The array object to compare.
     **/
    inline bool operator != ( const std::vector< VALUE > & other ) const;

    /**
     * \brief   Returns pointer to the array values. The values cannot be modified.
     **/
    inline operator const VALUE * () const;

/************************************************************************/
// Friend global operators to make Array streamable
/************************************************************************/
    /**
     * \brief   Reads array values from the stream. If array previously had values, they will be
     *          removed and new values from the stream will be set in the same sequence as they are
     *          present in the stream. The VALUE type must support initialization from streaming and
     *          have a streaming operator.
     *
     * \param   stream      The streaming object to read values.
     * \param   input       The array object to save initialized values.
     **/
    template <typename V>
    friend const InStream & operator >> (const InStream & stream, ArrayList< V > & input);

    /**
     * \brief   Writes array values to the stream starting from the first entry. The VALUE type must
     *          support streaming and have a streaming operator.
     *
     * \param   stream      The stream to write values.
     * \param   output      The array object containing values to stream.
     **/
    template <typename V>
    friend OutStream & operator << (OutStream & stream, const ArrayList< V > & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the array list is empty and has no elements.
     **/
    inline bool is_empty() const;

    /**
     * \brief   Returns the current size of the array.
     **/
    inline uint32_t size() const;

    /**
     * \brief   Returns true if the specified index is valid.
     **/
    inline bool is_valid_index(const uint32_t index) const;

    /**
     * \brief   Returns true if the given element exists in the array. The VALUE type must support
     *          comparison operators.
     *
     * \param   elemSearch      The element to search.
     * \param   startAt         The index to start searching.
     **/
    inline bool contains( const VALUE & elemSearch, uint32_t startAt = 0 ) const;

    /**
     * \brief   Returns the vector object where the data are stored.
     **/
    inline const std::vector<VALUE>& data() const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Removes all entries of the array.
     **/
    inline void clear();

    /**
     * \brief   Deletes extra unused capacity in the array.
     **/
    inline void free_extra();

    /**
     * \brief   Clears the array and deallocates all reserved space.
     **/
    inline void release();

    /**
     * \brief   Returns element value by valid zero-based index, which can be used by right
     *          operation (r-value). The index should be valid.
     **/
    inline const VALUE & at( uint32_t index ) const;

    /**
     * \brief   Returns element value by valid zero-based index, which can be used by left (l-value)
     *          and right operation (r-value). The index should be valid.
     **/
    inline VALUE & at( uint32_t index );

    /**
     * \brief   Sets new element at given valid index. The index should be valid.
     *
     * \param   index           The index position to set the element.
     * \param   newElement      The new element to set at the index.
     **/
    inline void set_at( uint32_t index, const VALUE & newElement );
    /**
     * \brief   Sets new element at given valid index. The index should be valid.
     *
     * \param   index           The index position to set the element.
     * \param   newElement      The new element to set at the index.
     * \note    Move overload.
     **/
    inline void set_at( uint32_t index, VALUE && newElement );

    /**
     * \brief   Returns element value by valid zero-based index.
     *
     * \param   atPosition      Zero-based valid position in array.
     **/
    inline const VALUE& value_at_position(const uint32_t atPosition) const;
    /**
     * \brief   Returns element value by valid zero-based index.
     *
     * \param   atPosition      Zero-based valid position in array.
     **/
    inline VALUE& value_at_position(uint32_t atPosition);

    /**
     * \brief   Returns pointer to array of values, which cannot be modified.
     **/
    inline const VALUE * values() const;

    /**
     * \brief   Adds new element at the end of the array.
     *
     * \param   newElement      New element to add at the end of the array.
     **/
    inline void add( const VALUE & newElement );

    /**
     * \brief   Adds element at the end if it is not already in the array. If updateExisting is
     *          true, updates existing element instead. The VALUE type must support comparison
     *          operators.
     *
     * \param   newElement          New element to add at the end of array.
     * \param   updateExisting      If true, updates the existing element instead of skipping the
     *                              add. Useful when two objects are compared by a field rather than
     *                              all values.
     * \return  Returns true if new element was added. Otherwise, returns false.
     **/
    inline bool add_if_unique(const VALUE & newElement, bool updateExisting = false );

    /**
     * \brief   Appends entries from given source at the end of the array.
     *
     * \param   src     The source of new entries.
     * \return  Returns reference to this array.
     **/
    inline ArrayList< VALUE >& append( const ArrayList< VALUE > & src );
    /**
     * \brief   Appends entries from given source at the end of the array.
     *
     * \param   src     The source of new entries.
     * \return  Returns reference to this array.
     **/
    inline ArrayList< VALUE >& append( const std::vector< VALUE > & src );
    /**
     * \brief   Appends entries from given source at the end of the array.
     *
     * \param   src     The source of new entries.
     * \return  Returns reference to this array.
     * \note    Move overload.
     **/
    inline ArrayList< VALUE >& append( ArrayList< VALUE > && src) noexcept;
    /**
     * \brief   Appends entries from given source at the end of the array.
     *
     * \param   src     The source of new entries.
     * \return  Returns reference to this array.
     * \note    Move overload.
     **/
    ArrayList< VALUE >& append( std::vector< VALUE > && src) noexcept;

    /**
     * \brief   Copies all entries from given source. If array previously had values, they will be
     *          removed and new values from source array will be set in the same sequence as they
     *          present in the source.
     *
     * \param   src     The source of array elements.
     **/
    inline void copy( const ArrayList< VALUE > & src );
    /**
     * \brief   Copies all entries from given source. If array previously had values, they will be
     *          removed and new values from source array will be set in the same sequence as they
     *          present in the source.
     *
     * \param   src     The source of array elements.
     **/
    inline void copy( const std::vector< VALUE > & src );

    /**
     * \brief   Moves all entries from given source. On output, the source array is empty.
     *
     * \param   src     The source of array elements.
     * \note    Move semantics.
     **/
    inline void move( ArrayList< VALUE > && src ) noexcept;
    /**
     * \brief   Moves all entries from given source. On output, the source array is empty.
     *
     * \param   src     The source of array elements.
     * \note    Move semantics.
     **/
    inline void move( std::vector< VALUE > && src ) noexcept;

    /**
     * \brief   If position is valid, shifts elements and inserts new element at specified position.
     *          Otherwise, appends element at the end of the array.
     *
     * \param   startAt         The index of position to insert element.
     * \param   newElement      Value of new element to insert.
     * \param   elemCount       Number of times to repeat the insertion.
     **/
    void insert_at( uint32_t startAt, const VALUE & newElement, uint32_t elemCount = 1 );

    /**
     * \brief   If position is valid, shifts elements and inserts new elements from the given source
     *          at specified position. Otherwise, appends elements at the end of the array.
     *
     * \param   startAt     Starting index position to insert entries.
     * \param   newArray    Source array of elements to insert.
     * \param   count       Number of elements in the array.
     **/
    void insert_at(uint32_t startAt, const VALUE * newArray, uint32_t count );

    /**
     * \brief   If position is valid, shifts elements and inserts new elements from the given source
     *          at specified position. Otherwise, appends elements at the end of the array.
     *
     * \param   startAt     Starting index position to insert entries.
     * \param   newArray    Source array of elements.
     **/
    inline void insert_at( uint32_t startAt, const ArrayList< VALUE > & newArray );
    /**
     * \brief   If position is valid, shifts elements and inserts new elements from the given source
     *          at specified position. Otherwise, appends elements at the end of the array.
     *
     * \param   startAt     Starting index position to insert entries.
     * \param   newArray    Source array of elements.
     **/
    void insert_at( uint32_t startAt, const std::vector< VALUE > & newArray );

    /**
     * \brief   Removes elements starting at given valid index position.
     *
     * \param   index           The index to start removing elements.
     * \param   elemCount       Amount of elements to remove.
     **/
    void remove_at(uint32_t index, uint32_t elemCount = 1);

    /**
     * \brief   Removes the element at specified index and returns the removed element.
     *
     * \param   index       The index of element to remove.
     * \return  The removed element.
     **/
    inline VALUE remove_position( uint32_t index );

    /**
     * \brief   Searches for element starting at given position and removes it on first match. The
     *          VALUE type must support comparison operators.
     *
     * \param   elemRemove      The element to search and remove.
     * \param   searchAt        The position to start searching.
     * \return  Returns true if element was found and removed.
     **/
    bool remove_elem(const VALUE & elemRemove, uint32_t searchAt = 0);

    /**
     * \brief   Searches for element in the array and returns its index. Returns -1 if not found.
     *          The VALUE type must support comparison operators.
     *
     * \param   elemSearch      The element to search.
     * \param   startAt         The index to start searching.
     * \return  If found, returns valid index of element in array. Otherwise, returns -1.
     **/
    int32_t find( const VALUE & elemSearch, uint32_t startAt = 0 ) const;

    /**
     * \brief   Sets new size of array. If needed, either increases or truncates elements. The VALUE
     *          type must have a default constructor to create and initialize new elements.
     *
     * \param   newSize     New size to set. If zero, array is emptied.
     **/
    inline void resize( uint32_t newSize );

    /**
     * \brief   Reserves capacity for elements without changing the array size. If newCapacity
     *          exceeds current capacity, allocates new space. Otherwise, does nothing.
     *
     * \param   newCapacity     The new capacity of the array.
     **/
    inline void reserve( uint32_t newCapacity );

    /**
     * \brief   Returns the capacity of the array.
     **/
    inline uint32_t capacity() const;

    /**
     * \brief   Shifts array elements starting at given valid index position. Positive count
     *          stretches elements to make room; negative count removes elements.
     *
     * \param   startAt     The starting index position to start shifting.
     * \param   count       Number of elements to shift. Positive value inserts space; negative
     *                      value removes space.
     **/
    void shift( uint32_t startAt, int32_t count);

    /**
     * \brief   Returns the first entry in the array. The array must not be empty.
     **/
    inline const VALUE & first_entry() const;
    /**
     * \brief   Returns the first entry in the array. The array must not be empty.
     **/
    inline VALUE & first_entry();

    /**
     * \brief   Returns the last entry in the array. The array must not be empty.
     **/
    inline const VALUE & last_entry() const;
    /**
     * \brief   Returns the last entry in the array. The array must not be empty.
     **/
    inline VALUE & last_entry();

    /**
     * \brief   Sorts the array using the given comparison function and returns reference to this
     *          array.
     *
     * \param   comp    The comparison function, similar to std::greater().
     * \return  Returns reference to the sorted array.
     **/
    template <class Compare>
    inline ArrayList< VALUE >& sort(Compare comp);

    /**
     * \brief   Copies elements from the array into the provided pre-allocated buffer. If
     *          `elemCount` is less than the number of elements in the array, only the first
     *          `elemCount` elements are copied. Otherwise, all elements in the array are copied. No
     *          elements are copied if `elemCount` is 0.
     *
     * \param[in,out] list            A pre-allocated buffer where the array elements will be
     *                                copied. Must be large enough to hold at least `elemCount`
     *                                elements.
     * \param   elemCount       The maximum number of elements to copy into the `list` buffer. If
     *                          set to 0, no elements are copied.
     * \return  The number of elements successfully copied into the `list` buffer.
     **/
    inline uint32_t elements(VALUE* list, uint32_t elemCount);

//////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets new size of array. If zero, all elements are removed. Otherwise, existing
     *          elements are copied or truncated based on whether new size is larger or smaller.
     *
     * \param   elemCount       New size to set. If zero, array is emptied.
     **/
    inline void set_size( uint32_t elemCount );

    /**
     * \brief   Returns the position of the element at the given index. The returned position cannot
     *          be modified.
     *
     * \param   index       The index of the element to return position.
     * \return  Returns the position of the element at the given index.
     **/
    inline ARRAYPOS position( uint32_t index ) const;

//////////////////////////////////////////////////////////////////////////
//Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Converts a constant iterator of the vector to ARRAYPOS type.
     *
     * \param   cit     The constant iterator of the vector.
     * \return  Returns converted ARRAYPOS type.
     **/
    inline ARRAYPOS _citer2pos(typename std::vector<VALUE>::const_iterator cit) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The array of elements.
     **/
    std::vector<VALUE>  mValueList;
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ArrayList< VALUE > class template implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE >
ArrayList< VALUE >::ArrayList( uint32_t capacity /*= areg::ARRAY_DEFAULT_CAPACITY*/, uint32_t resize /*= 0*/ )
    : Constless<std::vector<VALUE>>( )
    , mValueList( )
{
    capacity = std::max( resize, capacity );
    if (capacity != 0)
    {
        mValueList.reserve(capacity > areg::MAX_CONTAINER_SIZE ? areg::MAX_CONTAINER_SIZE : capacity);
    }

    if ( resize > 0 )
    {
        mValueList.resize( resize );
    }
}

template<typename VALUE>
ArrayList<VALUE>::ArrayList( const ArrayList<VALUE> & src )
    : Constless<std::vector<VALUE>>( )
    , mValueList( src.mValueList )
{
}

template<typename VALUE>
ArrayList<VALUE>::ArrayList( const std::vector<VALUE> & src )
    : Constless<std::vector<VALUE>>( )
    , mValueList( src )
{
}

template<typename VALUE>
ArrayList<VALUE>::ArrayList(const VALUE* list, uint32_t count)
    : Constless<std::vector<VALUE>>( )
    , mValueList( list != nullptr ? count : 0)
{
    if (areg::ARRAY_DEFAULT_CAPACITY > static_cast<uint32_t>(mValueList.capacity()))
    {
        mValueList.reserve(areg::ARRAY_DEFAULT_CAPACITY);
    }

    if (list != nullptr)
    {
        for (uint32_t i = 0; i < count; ++i)
        {
            mValueList.at(i) = list[i];
        }
    }
}

template<typename VALUE>
ArrayList<VALUE>::ArrayList( ArrayList<VALUE> && src ) noexcept
    : Constless<std::vector<VALUE>>( )
    , mValueList( std::move(src.mValueList) )
{
}

template<typename VALUE>
ArrayList<VALUE>::ArrayList( std::vector<VALUE> && src ) noexcept
    : Constless<std::vector<VALUE>>( )
    , mValueList( std::move( src ) )
{
}

template<typename VALUE>
ArrayList<VALUE>::~ArrayList()
{
}

template<typename VALUE >
inline VALUE& ArrayList< VALUE >::operator [] (uint32_t index)
{
    ASSERT(is_valid_index(index));
    return mValueList[index];
}

template<typename VALUE >
inline const VALUE & ArrayList< VALUE >::operator [] (uint32_t index) const
{
    ASSERT(is_valid_index(index));
    return mValueList[index];
}

template<typename VALUE >
inline ArrayList< VALUE > & ArrayList< VALUE >::operator = ( const ArrayList< VALUE >& src )
{
    return this->operator = (src.mValueList);
}

template<typename VALUE >
inline ArrayList< VALUE > & ArrayList< VALUE >::operator = ( const std::vector< VALUE >& src )
{
    mValueList = src;
    return (*this);
}

template<typename VALUE >
inline ArrayList< VALUE > & ArrayList< VALUE >::operator = ( ArrayList< VALUE > && src ) noexcept
{
    return this->operator = (std::move(src.mValueList));
}

template<typename VALUE >
inline ArrayList< VALUE > & ArrayList< VALUE >::operator = ( std::vector< VALUE > && src ) noexcept
{
    mValueList = std::move(src);
    return (*this);
}

template<typename VALUE >
inline bool ArrayList< VALUE >::operator == ( const ArrayList< VALUE >& other ) const
{
    return mValueList == other.mValueList;
}

template<typename VALUE >
inline bool ArrayList< VALUE >::operator == ( const std::vector< VALUE >& other ) const
{
    return mValueList == other;
}

template<typename VALUE >
inline bool ArrayList< VALUE >::operator != ( const ArrayList< VALUE >& other ) const
{
    return this->operator != (other.mValueList);
}

template<typename VALUE >
inline bool ArrayList< VALUE >::operator != ( const std::vector< VALUE >& other ) const
{
    return mValueList != other;
}

template<typename VALUE >
inline ArrayList< VALUE >::operator const VALUE * () const
{   
    return static_cast<const VALUE *>(mValueList.data());
}

template<typename VALUE >
inline bool ArrayList< VALUE >::is_empty() const
{
    return mValueList.empty();
}

template<typename VALUE >
inline uint32_t ArrayList< VALUE >::size() const
{
    return static_cast<uint32_t>(mValueList.size());
}

template<typename VALUE >
inline bool ArrayList< VALUE >::is_valid_index(uint32_t index) const
{
    return (index < static_cast<uint32_t>(mValueList.size()));
}

template<typename VALUE >
inline bool ArrayList< VALUE >::contains( const VALUE & elemSearch, uint32_t startAt /*= 0*/ ) const
{
    return (startAt < static_cast<uint32_t>(mValueList.size()) ? std::find(mValueList.begin() + static_cast<int32_t>(startAt), mValueList.end(), elemSearch) != mValueList.end() : false);
}

template<typename VALUE>
inline const std::vector<VALUE>& ArrayList<VALUE>::data() const
{
    return mValueList;
}

template<typename VALUE >
inline void ArrayList< VALUE >::clear()
{
    mValueList.clear();
}

template<typename VALUE >
inline void ArrayList< VALUE >::free_extra()
{
    mValueList.shrink_to_fit();
}

template<typename VALUE >
inline void ArrayList< VALUE >::release()
{
	mValueList.clear();
    mValueList.shrink_to_fit();
}

template<typename VALUE >
inline const VALUE & ArrayList< VALUE >::at(uint32_t index) const
{
    ASSERT(is_valid_index(index));
    return mValueList.at(index);
}

template<typename VALUE >
inline VALUE& ArrayList< VALUE >::at(uint32_t index)
{
    ASSERT(is_valid_index(index));
    return mValueList.at(index);
}

template<typename VALUE >
inline void ArrayList< VALUE >::set_at(uint32_t index, const VALUE & newElement)
{
    if (is_valid_index(index))
    {
        mValueList[index] = newElement;
    }
    else if ( areg::MAX_CONTAINER_SIZE > static_cast<uint32_t>(mValueList.size()) )
    {
        mValueList.push_back(newElement);
    }
#ifdef DEBUG
    else
    {
        ASSERT(false);
    }
#endif // DEBUG

}

template<typename VALUE >
inline void ArrayList< VALUE >::set_at(uint32_t index, VALUE && newElement)
{
    if (is_valid_index(index))
    {
        mValueList[index] = std::move(newElement);
    }
    else if (areg::MAX_CONTAINER_SIZE > static_cast<uint32_t>(mValueList.size()))
    {
        mValueList.push_back(std::move(newElement));
    }
#ifdef DEBUG
    else
    {
        ASSERT(false);
    }
#endif // DEBUG

}

template<typename VALUE >
inline const VALUE & ArrayList< VALUE >::value_at_position( const uint32_t atPosition ) const
{
    ASSERT( is_valid_index( atPosition ) );
    return mValueList.at( atPosition );
}

template<typename VALUE >
inline VALUE& ArrayList< VALUE >::value_at_position( uint32_t atPosition )
{
    ASSERT( is_valid_index( atPosition ) );
    return mValueList.at( atPosition );
}

template<typename VALUE >
inline const VALUE* ArrayList< VALUE >::values() const
{
    return static_cast<const VALUE *>(mValueList.data());
}

template<typename VALUE >
inline void ArrayList< VALUE >::add(const VALUE & newElement)
{
    if (areg::MAX_CONTAINER_SIZE > static_cast<uint32_t>(mValueList.size()))
    {
        mValueList.push_back(newElement);
    }
}

template<typename VALUE >
inline bool ArrayList< VALUE >::add_if_unique(const VALUE & newElement, bool updateExisting /*= false*/ )
{
    bool result{ false };

    ARRAYPOS pos = std::find( mValueList.begin( ), mValueList.end( ), newElement );
    if ( pos == mValueList.end() )
    {
        mValueList.push_back(newElement);
        result = true;
    }
    else if ( updateExisting )
    {
        *pos = newElement;
    }

    return result;
}

template<typename VALUE >
inline ArrayList< VALUE >& ArrayList< VALUE >::append(const ArrayList< VALUE >& src)
{
    return append(src.mValueList);
}

template<typename VALUE >
inline ArrayList< VALUE >& ArrayList< VALUE >::append(const std::vector< VALUE >& src)
{
    ASSERT(&mValueList != &src);

    uint32_t size = static_cast<uint32_t>(mValueList.size());
    uint32_t remain = static_cast<uint32_t>(src.size());
    if ((size + remain) > areg::MAX_CONTAINER_SIZE)
    {
        remain = areg::MAX_CONTAINER_SIZE - static_cast<uint32_t>(mValueList.size());
    }

    mValueList.reserve(size + remain);
    for (uint32_t i = 0; i < remain; ++i)
    {
        mValueList.push_back(src[i]);
    }

    return (*this);
}

template<typename VALUE >
inline ArrayList< VALUE >& ArrayList< VALUE >::append(ArrayList< VALUE >&& src) noexcept
{
    return append(std::move(src.mValueList));
}

template<typename VALUE >
ArrayList< VALUE >& ArrayList< VALUE >::append(std::vector< VALUE > && src) noexcept
{
    ASSERT(&mValueList != &src);

    uint32_t size = static_cast<uint32_t>(mValueList.size());
    uint32_t remain = static_cast<uint32_t>(src.size());
    if ((size + remain) > areg::MAX_CONTAINER_SIZE)
    {
        remain = areg::MAX_CONTAINER_SIZE - static_cast<uint32_t>(mValueList.size());
    }

    mValueList.reserve(size + remain);
    for (uint32_t i = 0; i < remain; ++i)
    {
        mValueList.push_back(std::move(src[i]));
    }

    return (*this);
}

template<typename VALUE >
inline void ArrayList< VALUE >::copy(const ArrayList< VALUE >& src)
{
    copy(src.data());
}

template<typename VALUE >
inline void ArrayList< VALUE >::copy(const std::vector< VALUE >& src)
{
    ASSERT(&mValueList != &src);
    mValueList = src;
}

template<typename VALUE >
inline void ArrayList< VALUE >::move( ArrayList< VALUE > && src ) noexcept
{
    move(std::move(src.mValueList));
}

template<typename VALUE >
inline void ArrayList< VALUE >::move(std::vector< VALUE > && src) noexcept
{
    ASSERT(&mValueList != &src);
    mValueList = std::move(src);
}

template<typename VALUE >
void ArrayList< VALUE >::insert_at(uint32_t startAt, const VALUE& newElement, uint32_t elemCount /*= 1*/)
{
    if (elemCount != 0)
    {
        uint32_t count = size();
        if ((count + elemCount) > areg::MAX_CONTAINER_SIZE)
        {
            elemCount = areg::MAX_CONTAINER_SIZE - count;
        }

        ARRAYPOS cit = position(startAt);
        if (cit != mValueList.end())
        {
            mValueList.insert(cit, elemCount, newElement);
        }
        else
        {
            mValueList.resize(count + elemCount, newElement);
        }
    }
}

template<typename VALUE >
void ArrayList< VALUE >::insert_at(uint32_t startAt, const VALUE* newArray, uint32_t count)
{
    if ((newArray != nullptr) && (count != 0))
    {
        if ((size() + count) > areg::MAX_CONTAINER_SIZE)
        {
            count = areg::MAX_CONTAINER_SIZE - size();
        }

        if (mValueList.size() == startAt)
        {
            resize(static_cast<uint32_t>(mValueList.size() + count));
        }
        else
        {
            ASSERT(is_valid_index(startAt));
            shift(startAt, static_cast<int32_t>(count));
        }

        for (uint32_t i = 0; i < count; ++i)
        {
            mValueList[startAt ++] = newArray[i];
        }
    }
}

template<typename VALUE >
inline void ArrayList< VALUE >::insert_at(uint32_t startAt, const ArrayList< VALUE >& newArray)
{
    insert_at(startAt, newArray.mValueList);
}

template<typename VALUE >
void ArrayList< VALUE >::insert_at(uint32_t startAt, const std::vector< VALUE >& newArray)
{
    ASSERT(is_valid_index(startAt));

    if (newArray.empty() == false)
    {
        int32_t limit = 0;
        if ((size() + newArray.size()) > areg::MAX_CONTAINER_SIZE)
        {
            limit = static_cast<int32_t>(areg::MAX_CONTAINER_SIZE - (size() + static_cast<uint32_t>(newArray.size())));
        }

        ARRAYPOS cit = position(startAt);
        mValueList.insert(cit, newArray.begin(), newArray.end() - limit);
    }
}

template<typename VALUE >
void ArrayList< VALUE >::remove_at(uint32_t index, uint32_t elemCount /*= 1*/)
{
    if (elemCount != 0)
    {
        uint32_t remain = static_cast<uint32_t>(mValueList.size()) - index;
        elemCount = std::min(elemCount, remain);
        ASSERT(is_valid_index(index));
        ASSERT(is_valid_index(index + elemCount - 1));

        ARRAYPOS first = position(index);
        if (elemCount == 1)
        {
            mValueList.erase(first);
        }
        else
        {
            ARRAYPOS last = first + static_cast<int32_t>(elemCount);
            mValueList.erase(first, last);
        }
    }
}

template<typename VALUE >
inline VALUE ArrayList< VALUE >::remove_position(uint32_t index)
{
    ASSERT(is_valid_index(index));
    ARRAYPOS first = position(index);
    VALUE result = *first;
    mValueList.erase(first);
    return result;
}

template<typename VALUE >
bool ArrayList< VALUE >::remove_elem( const VALUE & elemRemove, uint32_t searchAt /*= 0*/ )
{
    bool result = false;
    if (searchAt < static_cast<uint32_t>(mValueList.size()))
    {
        auto it = std::find(mValueList.begin() + static_cast<int32_t>(searchAt), mValueList.end(), elemRemove);
        if (it != mValueList.end())
        {
            mValueList.erase(it);
            result = true;
        }
    }

    return result;
}

template<typename VALUE >
int32_t ArrayList< VALUE >::find( const VALUE & elemSearch, uint32_t startAt /*= 0*/ ) const
{
    int32_t result = areg::INVALID_INDEX;
    if (startAt < static_cast<uint32_t>(mValueList.size()))
    {
        result = static_cast<int32_t>(startAt) - 1;
        auto it = std::find_if( mValueList.begin() + static_cast<int32_t>(startAt), mValueList.end()
                              , [&](const auto& elem) { ++result; return (elemSearch == elem);});

        if (it == mValueList.end())
        {
            result = areg::INVALID_INDEX;
        }
    }

    return result;
}

template<typename VALUE >
inline void ArrayList< VALUE >::resize( uint32_t newSize )
{
    mValueList.resize(newSize > areg::MAX_CONTAINER_SIZE ? areg::MAX_CONTAINER_SIZE : newSize);
}

template<typename VALUE >
inline void ArrayList< VALUE >::reserve( uint32_t newCapacity)
{
    mValueList.reserve(newCapacity > areg::MAX_CONTAINER_SIZE ? areg::MAX_CONTAINER_SIZE : newCapacity);
}

template<typename VALUE >
inline uint32_t ArrayList< VALUE >::capacity() const
{
    return static_cast<uint32_t>(mValueList.capacity());
}

template<typename VALUE >
void ArrayList< VALUE >::shift(uint32_t startAt, int32_t  count)
{
    if ((mValueList.size() == 0u) || (startAt >= mValueList.size()) || (count == 0))
        return;

    if (count > 0)
    {
        if (static_cast<uint32_t>(static_cast<int32_t>(size()) + count) > areg::MAX_CONTAINER_SIZE)
        {
            count = static_cast<int32_t>(areg::MAX_CONTAINER_SIZE - size());
        }

        VALUE* values = mValueList.data();
        uint32_t size = static_cast<uint32_t>(mValueList.size());
        mValueList.resize(static_cast<uint32_t>(static_cast<int32_t>(size) + count));
        areg::move_elems<VALUE>(values + startAt + count, values + startAt, size - startAt);
    }
    else if (startAt != 0)
    {
        VALUE* values = mValueList.data();
        uint32_t size = static_cast<uint32_t>(mValueList.size());

        count *= -1;
        if (startAt < static_cast<uint32_t>(count))
        {
            count = static_cast<int32_t>(startAt);
        }

        areg::move_elems<VALUE>(values + startAt - count, values + startAt, size - startAt);
        mValueList.resize(static_cast<uint32_t>(static_cast<int32_t>(size) - count));
    }
}

template<typename VALUE>
inline const VALUE & ArrayList<VALUE>::first_entry() const
{
    ASSERT( mValueList.size( ) != 0 );
    return mValueList[ 0 ];
}

template<typename VALUE>
inline VALUE & ArrayList<VALUE>::first_entry()
{
    ASSERT( mValueList.size( ) != 0 );
    return mValueList[ 0 ];
}

template<typename VALUE>
inline const VALUE & ArrayList<VALUE>::last_entry() const
{
    ASSERT( mValueList.size( ) != 0 );
    return mValueList[ mValueList.size( ) - 1 ];
}

template<typename VALUE>
inline VALUE & ArrayList<VALUE>::last_entry()
{
    ASSERT( mValueList.size( ) != 0 );
    return mValueList[ mValueList.size( ) - 1 ];
}

template<typename VALUE>
template<class Compare>
inline ArrayList<VALUE>& ArrayList<VALUE>::sort(Compare comp)
{
    std::sort(mValueList.begin(), mValueList.end(), comp);
    return (*this);
}

template<typename VALUE>
inline uint32_t ArrayList<VALUE>::elements(VALUE* list, uint32_t elemCount)
{
    uint32_t result{ std::min(static_cast<uint32_t>(mValueList.size()), elemCount) };
    for (uint32_t i = 0; i < result; ++i)
    {
        list[i] = mValueList[i];
    }

    return result;
}

template<typename VALUE >
inline void ArrayList< VALUE >::set_size(uint32_t elemCount)
{
    mValueList.resize(elemCount);
}

template<typename VALUE >
inline typename ArrayList< VALUE >::ARRAYPOS ArrayList< VALUE >::position(uint32_t index) const
{
    using shift_type = typename std::vector<VALUE>::difference_type;
    auto len = mValueList.size();
    return _citer2pos(index < static_cast<uint32_t>(len) ? mValueList.begin() + static_cast<shift_type>(index) : mValueList.end());
}

template<typename VALUE >
inline typename ArrayList< VALUE >::ARRAYPOS ArrayList< VALUE >::_citer2pos(typename std::vector<VALUE>::const_iterator cit) const
{
    return Constless<std::vector<VALUE>>::iter(mValueList, cit);
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template<typename V>
inline const areg::InStream & operator >> ( const areg::InStream & stream, areg::ArrayList< V > & input )
{
    input.clear();
    return (stream >> input.mValueList);
}

template<typename V>
inline areg::OutStream & operator << (areg::OutStream& stream, const areg::ArrayList< V >& output )
{
    return (stream << output.mValueList);
}

} // namespace areg
#endif  // AREG_BASE_ARRAYLIST_HPP
