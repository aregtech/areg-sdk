#ifndef AREG_BASE_FIXEDARRAY_HPP
#define AREG_BASE_FIXEDARRAY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/FixedArray.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Fixed Array class template.
 *              This class template is an Array of fixed size. 
 *              It cannot resized by adding new element. It resizes by 
 *              copying another array, i.e. by applying assigning operator.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/TemplateBase.hpp"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/IOStream.hpp"

#include <algorithm>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// FixedArray<VALUE> class template declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Fixed-size array container with pre-allocated capacity, supporting element access and
 *          copying but not insertion or removal operations. Not thread-safe.
 **/
template<typename VALUE>
class FixedArray
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a FixedArray with initial size. Size zero means no elements can be accessed;
     *          use assignment or move to change size.
     *
     * \param   elemCount       Initial element count (default 0).
     **/
    explicit FixedArray( uint32_t elemCount = 0);
    /**
     * \brief   Copies all elements from source array.
     *
     * \param   src     Source array to copy from.
     **/
    FixedArray( const FixedArray<VALUE> & src );
    /**
     * \brief   Moves all elements from source array.
     *
     * \param   src     Source array to move from.
     **/
    FixedArray( FixedArray<VALUE> && src ) noexcept;
    /**
     * \brief   Initializes array by copying from C-style array.
     *
     * \param   list        C-style array to copy from.
     * \param   count       Number of elements in array.
     **/
    FixedArray(const VALUE* list, uint32_t count);
    ~FixedArray();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// basic operators
/************************************************************************/

    /**
     * \brief   Returns reference to element at index for both read and write access.
     *
     * \param   index       Zero-based index.
     **/
    [[nodiscard]]
    inline VALUE& operator [] (uint32_t index);

    /**
     * \brief   Returns const reference to element at valid index for read-only access.
     *
     * \param   index       Zero-based index between 0 and size()-1.
     **/
    [[nodiscard]]
    inline const VALUE& operator [] (uint32_t index) const;

    /**
     * \brief   Assigns all values from source array, replacing existing contents.
     *
     * \param   src     Source FixedArray.
     **/
    inline FixedArray<VALUE> & operator = ( const FixedArray<VALUE> & src);

    /**
     * \brief   Move-assigns all values from source array, leaving source empty.
     *
     * \param   src     Source FixedArray to move from.
     **/
    inline FixedArray<VALUE> & operator = ( FixedArray<VALUE> && src ) noexcept;

    /**
     * \brief   Returns true if two arrays have equal size and all elements are equal.
     *
     * \param   other       Array to compare.
     **/
    [[nodiscard]]
    inline bool operator == (const FixedArray<VALUE> & other) const noexcept;

    /**
     * \brief   Returns true if two arrays differ in size or have any unequal elements.
     *
     * \param   other       Array to compare.
     **/
    [[nodiscard]]
    inline bool operator != (const FixedArray<VALUE> & other) const noexcept;

    /**
     * \brief   Returns const pointer to first element (read-only access to array data).
     **/
    [[nodiscard]]
    inline operator const VALUE * () const noexcept;

/************************************************************************/
// Friend global operators to make Fixed Array streamable
/************************************************************************/

    /**
     * \brief   Deserializes array values from stream, replacing existing contents.
     *
     * \param   stream      Streaming object to read values from.
     * \param[out] input       FixedArray to receive deserialized values.
     **/
    template<typename V>
    friend const InStream & operator >> ( const InStream & stream, FixedArray<V> & input );
    /**
     * \brief   Serializes all array values to stream starting from first element.
     *
     * \param[out] stream      Streaming object to write values to.
     * \param   output      FixedArray containing values to serialize.
     **/
    template<typename V>
    friend OutStream & operator << ( OutStream & stream, const FixedArray<V> & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the array has no elements.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Returns the number of elements in the array.
     **/
    [[nodiscard]]
    inline uint32_t size() const noexcept;

    /**
     * \brief   Returns true if the index is within valid range [0, size()-1].
     *
     * \param   index       Index to validate.
     **/
    [[nodiscard]]
    inline bool is_valid_index(const uint32_t index) const noexcept;

    /**
     * \brief   Returns true if element exists in array starting from given index.
     *
     * \param   elemSearch      Element to search for.
     * \param   startAt         Index to start searching (default 0).
     * \return  Returns true if element found; false otherwise.
     **/
    [[nodiscard]]
    inline bool contains( const VALUE & elemSearch, uint32_t startAt = 0 ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Removes all elements from array, setting size to zero.
     **/
    inline void clear() noexcept;

    /**
     * \brief   Replaces element at valid index with new value (copy).
     *
     * \param   index           Valid zero-based index.
     * \param   newElement      New element value to set.
     **/
    inline void set_value_at(uint32_t index, const VALUE& newElement);
    /**
     * \brief   Replaces element at valid index with new value (move).
     *
     * \param   index           Valid zero-based index.
     * \param   newElement      New element value to move.
     * \note    Move overload. Takes ownership of newElement.
     **/
    inline void set_value_at(uint32_t index, VALUE && newElement);

    /**
     * \brief   Returns const reference to element at valid zero-based position.
     *
     * \param   atPosition      Valid zero-based position in array.
     **/
    [[nodiscard]]
    inline const VALUE& value_at( const uint32_t atPosition ) const noexcept;
    /**
     * \brief   Returns reference to element at valid zero-based position for read or write access.
     *
     * \param   atPosition      Valid zero-based position in array.
     **/
    [[nodiscard]]
    inline VALUE& value_at( uint32_t atPosition ) noexcept;

    /**
     * \brief   Returns const pointer to array data (read-only access).
     **/
    [[nodiscard]]
    inline const VALUE* values() const noexcept;

    /**
     * \brief   Copies all elements from source array, replacing existing contents.
     *
     * \param   src     Source array to copy from.
     **/
    void copy(const FixedArray< VALUE >& src);

    /**
     * \brief   Moves all elements from source array, leaving source empty.
     *
     * \param   src     Source array to move from.
     **/
    inline void move(FixedArray< VALUE >&& src) noexcept;

    /**
     * \brief   Searches for element and returns its index, or -1 if not found.
     *
     * \param   elemSearch      Element to search for.
     * \param   startAt         Index to start searching (default 0).
     * \return  Valid array index if found; otherwise -1 (INVALID_INDEX).
     **/
    [[nodiscard]]
    inline int32_t find(const VALUE& elemSearch, uint32_t startAt = 0) const noexcept;

    /**
     * \brief   Resizes the array to new length and preserves existing elements.
     *
     * \param   newLength       New array size.
     **/
    inline void resize(uint32_t newLength );

    /**
     * \brief   Returns const reference to first element. Array must not be empty.
     **/
    [[nodiscard]]
    inline const VALUE & first_entry() const noexcept;
    /**
     * \brief   Returns reference to first element for read or write. Array must not be empty.
     **/
    [[nodiscard]]
    inline VALUE & first_entry() noexcept;

    /**
     * \brief   Returns const reference to last element. Array must not be empty.
     **/
    [[nodiscard]]
    inline const VALUE & last_entry() const noexcept;
    /**
     * \brief   Returns reference to last element for read or write. Array must not be empty.
     **/
    [[nodiscard]]
    inline VALUE & last_entry() noexcept;

    /**
     * \brief   Sorts array in-place using provided comparator and returns self.
     *
     * \param   comp    Comparator function/functor (similar to std::greater).
     * \return  Returns reference to this sorted FixedArray.
     **/
    template <class Compare>
    inline FixedArray< VALUE >& sort(Compare comp);

    /**
     * \brief   Copies up to elemCount elements into pre-allocated buffer.
     *
     * \param[out] list            Pre-allocated buffer large enough for elemCount elements.
     * \param   elemCount       Maximum number of elements to copy (0 means no copy).
     * \return  Number of elements actually copied.
     **/
    [[nodiscard]]
    inline uint32_t elements(VALUE* list, uint32_t elemCount);

//////////////////////////////////////////////////////////////////////////
// Protected member variables
//////////////////////////////////////////////////////////////////////////
protected:
    VALUE *     mValueList; //!< Pointer to array of elements
    uint32_t    mElemCount; //!< The size of array
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// FixedArray<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template< typename VALUE >
FixedArray<VALUE>::FixedArray(uint32_t elemCount /*= 0*/ )
    : mValueList( elemCount != 0 ? DEBUG_NEW VALUE[elemCount] : nullptr )
    , mElemCount( mValueList != nullptr ? elemCount : 0 )
{
}

template< typename VALUE >
FixedArray<VALUE>::FixedArray( const FixedArray<VALUE>& src )
    : mValueList( src.mElemCount != 0 ? DEBUG_NEW VALUE[src.mElemCount] : nullptr )
    , mElemCount( mValueList != nullptr ? src.mElemCount : 0 )
{
    areg::copy_elems<VALUE>(mValueList, src.mValueList, mElemCount);
}

template< typename VALUE >
FixedArray<VALUE>::FixedArray( FixedArray<VALUE> && src ) noexcept
    : mValueList( src.mValueList )
    , mElemCount( src.mElemCount )
{
    src.mValueList  = nullptr;
    src.mElemCount  = 0;
}

template<typename VALUE>
FixedArray<VALUE>::FixedArray(const VALUE* list, uint32_t count)
    : mValueList(count ? DEBUG_NEW VALUE[count] : nullptr)
    , mElemCount(mValueList != nullptr ? count : 0)
{
    areg::copy_elems<VALUE>(mValueList, list, mElemCount);
}

template< typename VALUE >
FixedArray<VALUE>::~FixedArray()
{
    clear();
}

template< typename VALUE >
inline VALUE& FixedArray<VALUE>::operator [] (uint32_t index)
{
    ASSERT(is_valid_index(index));
    return mValueList[index];
}

template< typename VALUE >
inline const VALUE& FixedArray<VALUE>::operator [] (uint32_t index) const
{
    ASSERT(is_valid_index(index));
    return static_cast<const VALUE&>(mValueList[index]);
}

template< typename VALUE >
inline FixedArray<VALUE> & FixedArray<VALUE>::operator = ( const FixedArray<VALUE>& src )
{
    copy(src);
    return (*this);
}

template< typename VALUE >
inline FixedArray<VALUE> & FixedArray<VALUE>::operator = ( FixedArray<VALUE> && src ) noexcept
{
    move(std::move(src));
    return (*this);
}

template< typename VALUE >
inline bool FixedArray<VALUE>::operator == ( const FixedArray<VALUE>& other ) const noexcept
{
    return ((mElemCount == other.size()) && areg::equal_elements<VALUE>(mValueList, other.mValueList, mElemCount));
}

template< typename VALUE >
inline bool FixedArray<VALUE>::operator != (const FixedArray<VALUE>& other) const noexcept
{
    return ((mElemCount != other.size()) || !areg::equal_elements<VALUE>(mValueList, other.mValueList, mElemCount));
}

template< typename VALUE >
inline FixedArray<VALUE>::operator const VALUE * () const noexcept
{
    return mValueList;
}

template< typename VALUE >
inline bool FixedArray<VALUE>::is_empty() const noexcept
{
    return (mElemCount == 0);
}

template< typename VALUE >
inline uint32_t FixedArray<VALUE>::size() const noexcept
{
    return mElemCount;
}

template< typename VALUE >
inline bool FixedArray<VALUE>::is_valid_index(uint32_t whichIndex) const noexcept
{
    return (whichIndex < mElemCount);
}

template< typename VALUE >
inline bool FixedArray<VALUE>::contains(const VALUE& elemSearch, uint32_t startAt /*= 0*/) const noexcept
{
    return (find(elemSearch, startAt) != areg::INVALID_INDEX);
}

template< typename VALUE >
inline void FixedArray<VALUE>::clear() noexcept
{
    delete[] mValueList;
    mValueList = nullptr;
    mElemCount = 0;
}

template< typename VALUE >
inline void FixedArray<VALUE>::set_value_at(uint32_t index, const VALUE& newValue )
{
    ASSERT(is_valid_index(index));
    mValueList[index] = newValue;
}

template< typename VALUE >
inline void FixedArray<VALUE>::set_value_at(uint32_t index, VALUE && newValue)
{
    ASSERT(is_valid_index(index));
    mValueList[index] = std::move(newValue);
}

template<typename VALUE >
inline const VALUE & FixedArray< VALUE >::value_at( const uint32_t atPosition ) const noexcept
{
    ASSERT( is_valid_index( atPosition ) );
    return static_cast<const VALUE&>(mValueList[atPosition]);
}

template<typename VALUE >
inline VALUE& FixedArray< VALUE >::value_at( uint32_t atPosition ) noexcept
{
    ASSERT( is_valid_index( atPosition ) );
    return mValueList[atPosition];
}

template< typename VALUE >
inline const VALUE* FixedArray<VALUE>::values() const noexcept
{
    return  mValueList;
}

template< typename VALUE >
void FixedArray<VALUE>::copy(const FixedArray< VALUE >& src)
{
    if (static_cast<const FixedArray<VALUE> *>(this) != &src)
    {
        if (mElemCount != src.size())
        {
            clear();
            mValueList = src.size() > 0 ? DEBUG_NEW VALUE[src.size()] : nullptr;
            mElemCount = mValueList != nullptr ? src.size() : 0;
        }

        areg::copy_elems<VALUE>(mValueList, src.mValueList, mElemCount);
    }
}

template< typename VALUE >
inline void FixedArray<VALUE>::move(FixedArray< VALUE > && src) noexcept
{
    if (static_cast<const FixedArray<VALUE> *>(this) != &src)
    {
        std::swap(mValueList, src.mValueList);
        std::swap(mElemCount, src.mElemCount);
    }
}

template< typename VALUE >
inline int32_t FixedArray<VALUE>::find(const VALUE& elemSearch, uint32_t startAt /* = 0 */) const noexcept
{
    int32_t result = areg::INVALID_INDEX;
    for (uint32_t i = startAt; i < mElemCount; ++i)
    {
        if (elemSearch == mValueList[i])
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}


template< typename VALUE >
void FixedArray<VALUE>::resize(uint32_t newLength)
{
    VALUE * newList = newLength != 0 ? DEBUG_NEW VALUE[newLength] : nullptr;
    uint32_t count = std::min(newLength, mElemCount);
    areg::copy_elems<VALUE>(newList, mValueList, count);
    delete [] mValueList;
    mValueList = newList;
    mElemCount = newLength;
}

template<typename VALUE>
inline const VALUE & FixedArray<VALUE>::first_entry() const noexcept
{
    ASSERT( mElemCount != 0 );
    return mValueList[ 0 ];
}

template<typename VALUE>
inline VALUE & FixedArray<VALUE>::first_entry() noexcept
{
    ASSERT( mElemCount != 0 );
    return mValueList[ 0 ];
}

template<typename VALUE>
inline const VALUE & FixedArray<VALUE>::last_entry() const noexcept
{
    ASSERT( mElemCount != 0 );
    return mValueList[ mElemCount - 1 ];
}

template<typename VALUE>
inline VALUE & FixedArray<VALUE>::last_entry() noexcept
{
    ASSERT( mElemCount != 0 );
    return mValueList[ mElemCount - 1 ];
}

template<typename VALUE>
template<class Compare>
inline FixedArray<VALUE>& FixedArray<VALUE>::sort(Compare comp)
{
    if (mValueList != nullptr)
    {
        std::sort(mValueList, mValueList + mElemCount, comp);
    }

    return (*this);
}

template<typename VALUE>
inline uint32_t FixedArray<VALUE>::elements(VALUE* list, uint32_t elemCount)
{
    uint32_t result{ std::min(mElemCount, elemCount) };
    areg::copy_elems<VALUE>(list, mValueList, result);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template<typename V>
const InStream & operator >> ( const InStream & stream, FixedArray<V> & input )
{
    uint32_t size = 0;
    stream >> size;
    input.clear();
    input.resize(size);

    for (uint32_t i = 0; i < input.mElemCount; ++ i )
    {
        stream >> input.mValueList[i];
    }

    return stream;
}

template<typename V>
OutStream & operator << ( OutStream & stream, const FixedArray<V> & output )
{
    stream << output.mElemCount;
    for (uint32_t i = 0; i < output.mElemCount; ++ i )
    {
        stream << output.mValueList[i];
    }

    return stream;
}

template<typename VALUE>
struct required_size <areg::FixedArray<VALUE>>
{
    [[nodiscard]]
    inline uint32_t operator ()(const areg::FixedArray<VALUE>& list) const noexcept
    {
        uint32_t result{ static_cast<uint32_t>(sizeof(uint32_t)) };
        const VALUE* const arr  = list.values();
        const uint32_t     count= list.size();
        for (uint32_t i = 0; i < count; ++i)
        {
            result += required_size<VALUE>{}(arr[i]);
        }

        return result;
    }
};

} // namespace areg
#endif  // AREG_BASE_FIXEDARRAY_HPP
