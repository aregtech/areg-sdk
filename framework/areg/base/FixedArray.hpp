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
#include "areg/base/GEGlobal.h"
#include "areg/base/TemplateBase.hpp"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/IOStream.hpp"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
// FixedArray<VALUE> class template declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Fixed Array has general functionalities to access and copy elements 
 *          by valid index. Fixed Array is similar to FixedArray<VALUE> except
 *          that it does not support insert or remove element(s) and does not
 *          change the initial size, unless it is not assigned or moved from 
 *          another source.
 *
 *          The type VALUE should have at least default constructor, applicable
 *          comparing and assigning operators. The FixedArray object is not
 *          thread safe and data access should be synchronized manually.
 *
 * \tparam  VALUE   The type of stored elements should be either primitive or have
 *                  default constructor, applicable comparing and assigning operators.
 *
 * \example     FixedArray use
 *
 *              Suppose that there is a need to have a matrix with various length of columns. 
 *              For example:
 *              1   2   3
 *              1   2
 *              1

 *              In this case, this can be defined in following way:
 *
 *              typedef FixedArray<int32_t>           FixedArray;
 *              typedef FixedArray<FixedArray*>   FixedMatrix;
 *              FixedMatrix matrix(3);
 *              matrix[0] = new FixedArray(3);
 *              matrix[1] = new FixedArray(2);
 *              matrix[2] = new FixedArray(1);
 *              matrix[0][0] = 1; matrix[0][1] = 2; matrix[0][3] = 3;
 *              matrix[1][0] = 1; matrix[1][1] = 2;
 *              matrix[2][0] = 1;
 *
 **/
template<typename VALUE>
class FixedArray
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates FixedArray with initial size. If the initial
     *          size is zero, no element can be accessed. To change the size,
     *          assigning or move operators should be used.
     * \param	elemCount	The initial size of array.
     **/
    explicit FixedArray( uint32_t elemCount = 0);
    /**
     * \brief   Copy elements of array from the given source.
     * \param   src     The source to copy data.
     **/
    FixedArray( const FixedArray<VALUE> & src );
    /**
     * \brief   Moves elements of array from the given source.
     * \param   src     The source to move data.
     **/
    FixedArray( FixedArray<VALUE> && src ) noexcept;
    /**
     * \brief   Compiles entries from the given array of objects.
     * \param   list    The list of entries to copy.
     * \param   count   The number of entries in the array.
     **/
    FixedArray(const VALUE* list, uint32_t count);
    /**
     * \brief   Destructor.
     **/
    ~FixedArray();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// basic operators
/************************************************************************/

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid zero-based index.
     *          May be used on either the right (r-value) or the left (l-value) of an assignment statement.
     **/
    inline VALUE& operator [] (uint32_t index);

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid zero-based index.
     *          The index should be valid number between 0 and (mSize -1).
     *          May be used on the right (r-value).
     **/
    inline const VALUE& operator [] (uint32_t index) const;

    /**
     * \brief   Assigning operator. Copies all values from given source.
     *          If array previously had values, they will be removed and new values
     *          from source array will be set in the same sequence as they are
     *          present in the source. The size of FixedArray may differ.
     * \param   src     The source of fixed array of values.
     **/
    inline FixedArray<VALUE> & operator = ( const FixedArray<VALUE> & src);

    /**
     * \brief   Move operator. Moves all values from given source.
     *          If Array previously had values, they will be removed and new values
     *          from source Array will be set in the same sequence as they are
     *          present in the source. The size of FixedArray may differ.
     * \param   src     The source of fixed array of values.
     **/
    inline FixedArray<VALUE> & operator = ( FixedArray<VALUE> && src ) noexcept;

    /**
     * \brief   Checks equality of 2 array objects, and returns true if they are equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The fixed array object to compare.
     **/
    inline bool operator == (const FixedArray<VALUE> & other) const;
    /**
     * \brief   Checks inequality of 2 array objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The fixed array object to compare.
     **/
    inline bool operator != (const FixedArray<VALUE> & other) const;

    /**
     * \brief   Returns pointer to the fixed array values. The values cannot be modified
     **/
    inline operator const VALUE * () const;

/************************************************************************/
// Friend global operators to make Fixed Array streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream fixed array values.
     *          If fixed array previously had values, they will be removed and new values
     *          from the stream will be set in the same size and sequence as they are present
     *          in the stream. There should be possibility to initialize values from
     *          streaming object and if VALUE is not a primitive, but an object, it
     *          should have implemented streaming operator.
     * \param   stream  The streaming object to read values.
     * \param   input   The fixed array object to save initialized values.
     **/
    template<typename V>
    friend const areg::InStream & operator >> ( const areg::InStream & stream, FixedArray<V> & input );
    /**
     * \brief   Writes to the stream the values of fixed array.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream values and if VALUE is not a
     *          primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The stream to write values.
     * \param   output  The fixed array object containing value to stream.
     **/
    template<typename V>
    friend areg::OutStream & operator << ( areg::OutStream & stream, const FixedArray<V> & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the fixed array is empty and has no elements.
     **/
    inline bool isEmpty() const;

    /**
     * \brief	Returns the size of the fixed array.
     **/
    inline uint32_t getSize() const;

    /**
     * \brief   Returns true if the specified index is valid.
     **/
    inline bool isValidIndex(const uint32_t index) const;

    /**
     * \brief	Checks whether given element exist in fixed array or not. The elements of type
     *          VALUE should have comparing operators.
     * \param	elemSearch	The element to search.
     * \param	startAt	    The index to start searching.
     * \return	Returns true if could find element starting at given position.
     **/
    inline bool contains( const VALUE & elemSearch, uint32_t startAt = 0 ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Clears all elements of array
     **/
    inline void clear();

    /**
     * \brief   Returns element value by valid index, which can be used by right operation (r-value).
     *          The index should be valid.
     **/
    inline const VALUE& getAt(uint32_t index) const;

    /**
     * \brief   Returns element value by valid index, which can be used by left (l-value) and right operation (r-value).
     *          The index should be valid.
     **/
    inline VALUE& getAt(uint32_t index);

    /**
     * \brief   Sets new element at given valid index. The index should be valid.
     **/
    inline void setAt(uint32_t index, const VALUE& newElement);
    inline void setAt(uint32_t index, VALUE && newElement);

    /**
     * \brief   Returns element value by valid zero-based index.
     * \param   atPosition  Zero-based valid position in array.
     **/
    inline const VALUE& valueAtPosition( const uint32_t atPosition ) const;
    inline VALUE& valueAtPosition( uint32_t atPosition );

    /**
     * \brief   Returns array of values, which cannot be modified.
     **/
    inline const VALUE* getValues() const;

    /**
     * \brief	Copies all entries from given source. If array previously had values,
     *          they will be removed and new values from source array will be set
     *          in the same sequence as they present in the source.
     * \param	src	    The source of array elements.
     **/
    void copy(const FixedArray< VALUE >& src);

    /**
     * \brief	Moves all entries from given source. On output, the source of array is empty.
     * \param	src	    The source of array elements
     **/
    inline void move(FixedArray< VALUE >&& src) noexcept;

    /**
     * \brief	Search element entry in the array and returns the index.
     *          If element is not found, returns -1. The elements of type VALUE
     *          should have comparing operators.
     * \param	elemSearch	The element to search.
     * \param	startAt	    The index to start searching.
     * \return	If found, returns valid index of element in array. Otherwise, returns INVALID_INDEX aka -1.
     **/
    inline int32_t find(const VALUE& elemSearch, uint32_t startAt = 0) const;

    /**
     * \brief   Resize the array, set new length and copy existing data.
     * \param   newLength   The new length of array to set.
     **/
    inline void resize(uint32_t newLength );

    /**
     * \brief   Return the fist entry in the array. The array must not be empty.
     *          Otherwise, it fails with the assertion.
     **/
    inline const VALUE & firstEntry() const;
    inline VALUE & firstEntry();

    /**
     * \brief   Return the last entry in the array. The array must not be empty.
     *          Otherwise, it fails with the assertion.
     **/
    inline const VALUE & lastEntry() const;
    inline VALUE & lastEntry();

    /**
     * \brief   Sorts the array, compares the elements by given Compare functionality.
     * \param   comp    The comparing method, similar to the method  std::greater()
     * \return  Sorts and returns the fixed array object.
     **/
    template <class Compare>
    inline FixedArray< VALUE >& sort(Compare comp);

    /**
     * \brief   Copies elements from the array into the provided pre-allocated buffer.
     *          If `elemCount` is less than the number of elements in the array,
     *          only the first `elemCount` elements are copied. Otherwise, all elements
     *          in the array are copied. No elements are copied if `elemCount` is 0
     * \param   list [in, out]  A pre-allocated buffer where the array elements
     *                          will be copied. Must be large enough to hold at least
     *                          `elemCount` elements.
     * \param   elemCount [in]  The maximum number of elements to copy into the `list` buffer.
     *                          If set to 0, no elements are copied.
     * \return  The number of elements successfully copied into the `list` buffer.
     **/
    inline uint32_t getElements(VALUE* list, uint32_t elemCount);

//////////////////////////////////////////////////////////////////////////
// Protected member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Pointer to array of elements
     **/
    VALUE *     mValueList;
    /**
     * \brief   The size of array
     **/
    uint32_t    mElemCount;
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
    areg::copyElems<VALUE>(mValueList, src.mValueList, mElemCount);
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
    areg::copyElems<VALUE>(mValueList, list, mElemCount);
}

template< typename VALUE >
FixedArray<VALUE>::~FixedArray()
{
    clear();
}

template< typename VALUE >
inline VALUE& FixedArray<VALUE>::operator [] (uint32_t index)
{
    ASSERT(isValidIndex(index));
    return mValueList[index];
}

template< typename VALUE >
inline const VALUE& FixedArray<VALUE>::operator [] (uint32_t index) const
{
    ASSERT(isValidIndex(index));
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
inline bool FixedArray<VALUE>::operator == ( const FixedArray<VALUE>& other ) const
{
    return ((mElemCount == other.getSize()) && areg::equalElements<VALUE>(mValueList, other.mValueList, mElemCount));
}

template< typename VALUE >
inline bool FixedArray<VALUE>::operator != (const FixedArray<VALUE>& other) const
{
    return ((mElemCount != other.getSize()) || !areg::equalElements<VALUE>(mValueList, other.mValueList, mElemCount));
}

template< typename VALUE >
inline FixedArray<VALUE>::operator const VALUE * () const
{
    return mValueList;
}

template< typename VALUE >
inline bool FixedArray<VALUE>::isEmpty() const
{
    return (mElemCount == 0);
}

template< typename VALUE >
inline uint32_t FixedArray<VALUE>::getSize() const
{
    return mElemCount;
}

template< typename VALUE >
inline bool FixedArray<VALUE>::isValidIndex(uint32_t whichIndex) const
{
    return (whichIndex < mElemCount);
}

template< typename VALUE >
inline bool FixedArray<VALUE>::contains(const VALUE& elemSearch, uint32_t startAt /*= 0*/) const
{
    return (find(elemSearch, startAt) != areg::INVALID_INDEX);
}

template< typename VALUE >
inline void FixedArray<VALUE>::clear()
{
    delete[] mValueList;
    mValueList = nullptr;
    mElemCount = 0;
}

template< typename VALUE >
inline const VALUE& FixedArray<VALUE>::getAt(uint32_t index ) const
{
    ASSERT(isValidIndex(index));
    return static_cast<const VALUE&>(mValueList[index]);
}

template< typename VALUE >
inline VALUE & FixedArray<VALUE>::getAt(uint32_t index )
{
    ASSERT(isValidIndex(index));
    return mValueList[index];
}

template< typename VALUE >
inline void FixedArray<VALUE>::setAt(uint32_t index, const VALUE& newValue )
{
    ASSERT(isValidIndex(index));
    mValueList[index] = newValue;
}

template< typename VALUE >
inline void FixedArray<VALUE>::setAt(uint32_t index, VALUE && newValue)
{
    ASSERT(isValidIndex(index));
    mValueList[index] = std::move(newValue);
}

template<typename VALUE >
inline const VALUE & FixedArray< VALUE >::valueAtPosition( const uint32_t atPosition ) const
{
    ASSERT( isValidIndex( atPosition ) );
    return static_cast<const VALUE&>(mValueList[atPosition]);
}

template<typename VALUE >
inline VALUE& FixedArray< VALUE >::valueAtPosition( uint32_t atPosition )
{
    ASSERT( isValidIndex( atPosition ) );
    return mValueList[atPosition];
}

template< typename VALUE >
inline const VALUE* FixedArray<VALUE>::getValues() const
{
    return  mValueList;
}

template< typename VALUE >
void FixedArray<VALUE>::copy(const FixedArray< VALUE >& src)
{
    if (static_cast<const FixedArray<VALUE> *>(this) != &src)
    {
        if (mElemCount != src.getSize())
        {
            clear();
            mValueList = src.getSize() > 0 ? DEBUG_NEW VALUE[src.getSize()] : nullptr;
            mElemCount = mValueList != nullptr ? src.getSize() : 0;
        }

        for (uint32_t i = 0; i < mElemCount; ++i)
            mValueList[i] = src.mValueList[i];
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
inline int32_t FixedArray<VALUE>::find(const VALUE& elemSearch, uint32_t startAt /* = 0 */) const
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
    for (uint32_t i = 0; i < count; ++ i)
    {
        newList[i] = mValueList[i];
    }
    
    delete [] mValueList;

    mValueList  = newList;
    mElemCount  = newLength;
}

template<typename VALUE>
inline const VALUE & FixedArray<VALUE>::firstEntry() const
{
    ASSERT( mElemCount != 0 );
    return mValueList[ 0 ];
}

template<typename VALUE>
inline VALUE & FixedArray<VALUE>::firstEntry()
{
    ASSERT( mElemCount != 0 );
    return mValueList[ 0 ];
}

template<typename VALUE>
inline const VALUE & FixedArray<VALUE>::lastEntry() const
{
    ASSERT( mElemCount != 0 );
    return mValueList[ mElemCount - 1 ];
}

template<typename VALUE>
inline VALUE & FixedArray<VALUE>::lastEntry()
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
inline uint32_t FixedArray<VALUE>::getElements(VALUE* list, uint32_t elemCount)
{
    uint32_t result{ std::min(mElemCount, elemCount) };
    for (uint32_t i = 0; i < result; ++i)
    {
        list[i] = mValueList[i];
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template<typename V>
const areg::InStream & operator >> ( const areg::InStream & stream, FixedArray<V> & input )
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
areg::OutStream & operator << ( areg::OutStream & stream, const FixedArray<V> & output )
{
    stream << output.mElemCount;
    for (uint32_t i = 0; i < output.mElemCount; ++ i )
    {
        stream << output.mValueList[i];
    }

    return stream;
}

#endif  // AREG_BASE_FIXEDARRAY_HPP
