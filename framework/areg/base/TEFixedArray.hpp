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
 * \file        areg/base/TEFixedArray.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Fixed Array class template.
 *              This class template is an Array of fixed size. 
 *              It cannot resized by adding new element. It resizes by 
 *              copying another array, i.e. by applying assigning operator.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TETemplateBase.hpp"

#include "areg/base/NEMemory.hpp"
#include "areg/base/IEIOStream.hpp"

//////////////////////////////////////////////////////////////////////////
// TEFixedArray<VALUE> class template declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Fixed Array has general functionalities to access and copy elements 
 *          by valid index. Fixed Array is similar to TEFixedArray<VALUE> except
 *          that it does not support insert or remove element(s) and does not
 *          change the initial size, unless it is not assigned or moved from 
 *          another source.
 *
 *          The type VALUE should have at least default constructor, applicable
 *          comparing and assigning operators. The TEFixedArray object is not
 *          thread safe and data access should be synchronized manually.
 *
 * \tparam  VALUE   The type of stored elements should be either primitive or have
 *                  default constructor, applicable comparing and assigning operators.
 *
 * \example     TEFixedArray use
 *
 *              Suppose that there is a need to have a matrix with various length of columns. 
 *              For example:
 *              1   2   3
 *              1   2
 *              1

 *              In this case, this can be defined in following way:
 *
 *              typedef TEFixedArray<int>           FixedArray;
 *              typedef TEFixedArray<FixedArray*>   FixedMatrix;
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
class TEFixedArray
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates FixedArray with initial size. If the initial
     *          size is zero, no element can be accessed. To change the size,
     *          use assigning or move operators should be used.
     * \param	elemCount	The initial size of array.
     **/
    explicit TEFixedArray( uint32_t elemCount = 0);
    /**
     * \brief   Copy elements of array from the given source.
     * \param   src     The source to copy data.
     **/
    TEFixedArray( const TEFixedArray<VALUE> & src );
    /**
     * \brief   Moves elements of array from the given source.
     * \param   src     The source to move data.
     **/
    TEFixedArray( TEFixedArray<VALUE> && src ) noexcept;
    /**
     * \brief   Destructor.
     **/
    ~TEFixedArray( void );

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
    inline TEFixedArray<VALUE> & operator = ( const TEFixedArray<VALUE> & src);

    /**
     * \brief   Move operator. Moves all values from given source.
     *          If Array previously had values, they will be removed and new values
     *          from source Array will be set in the same sequence as they are
     *          present in the source. The size of FixedArray may differ.
     * \param   src     The source of fixed array of values.
     **/
    inline TEFixedArray<VALUE> & operator = ( TEFixedArray<VALUE> && src ) noexcept;

    /**
     * \brief   Checks equality of 2 array objects, and returns true if they are equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The fixed array object to compare.
     **/
    inline bool operator == (const TEFixedArray<VALUE> & other) const;
    /**
     * \brief   Checks inequality of 2 array objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The fixed array object to compare.
     **/
    inline bool operator != (const TEFixedArray<VALUE> & other) const;

    /**
     * \brief   Returns pointer to the fixed array values. The values cannot be modified
     **/
    inline operator const VALUE * ( void ) const;

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
    template<typename VALUE>
    friend const IEInStream & operator >> ( const IEInStream & stream, TEFixedArray<VALUE> & input );
    /**
     * \brief   Writes to the stream the values of fixed array.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream values and if VALUE is not a
     *          primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The stream to write values.
     * \param   input   The fixed array object containing value to stream.
     **/
    template<typename VALUE>
    friend IEOutStream & operator << ( IEOutStream & stream, const TEFixedArray<VALUE> & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the fixed array is empty and has no elements.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief	Returns the size of the fixed array.
     **/
    inline uint32_t getSize( void ) const;

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
    inline void clear(void);

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
     * \brief   Returns array of values, which cannot be modified.
     **/
    inline const VALUE* getValues(void) const;

    /**
     * \brief	Copies all entries from given source. If array previously had values,
     *          they will be removed and new values from source array will be set
     *          in the same sequence as they present in the source.
     * \param	src	    The source of array elements.
     **/
    inline void copy(const TEFixedArray< VALUE >& src);

    /**
     * \brief	Moves all entries from given source. On output, the source of array is empty.
     * \param	src	    The source of array elements
     **/
    inline void move(TEFixedArray< VALUE >&& src) noexcept;

    /**
     * \brief	Search element entry in the array and returns the index.
     *          If element is not found, returns -1. The elements of type VALUE
     *          should have comparing operators.
     * \param	elemSearch	The element to search.
     * \param	startAt	    The index to start searching.
     * \return	If found, returns valid index of element in array. Otherwise, returns -1.
     **/
    inline int find(const VALUE& elemSearch, uint32_t startAt = 0) const;

    /**
     * \brief   Resize the array, set new length and copy existing data.
     * \param   newLength   The new length of array to set.
     **/
    inline void resize(uint32_t newLength );

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
// TEFixedArray<VALUE> class template implementation
//////////////////////////////////////////////////////////////////////////

template< typename VALUE >
TEFixedArray<VALUE>::TEFixedArray(uint32_t elemCount /*= 0*/ )
    : mValueList( elemCount != 0 ? DEBUG_NEW VALUE[elemCount] : nullptr )
    , mElemCount( mValueList != nullptr ? elemCount : 0 )
{
}

template< typename VALUE >
TEFixedArray<VALUE>::TEFixedArray( const TEFixedArray<VALUE>& src )
    : mValueList( src.mElemCount != 0 ? DEBUG_NEW VALUE[src.mElemCount] : nullptr )
    , mElemCount( mValueList != nullptr ? src.mElemCount : 0 )
{
    NEMemory::copyElems<VALUE>(mValueList, src.mValueList, mElemCount);
}

template< typename VALUE >
TEFixedArray<VALUE>::TEFixedArray( TEFixedArray<VALUE> && src ) noexcept
    : mValueList( src.mValueList )
    , mElemCount( src.mElemCount )
{
    src.mValueList  = nullptr;
    src.mElemCount  = 0;
}

template< typename VALUE >
TEFixedArray<VALUE>::~TEFixedArray( void )
{
    clear();
}

template< typename VALUE >
inline VALUE& TEFixedArray<VALUE>::operator [] (uint32_t index)
{
    ASSERT(isValidIndex(index));
    return mValueList[index];
}

template< typename VALUE >
inline const VALUE& TEFixedArray<VALUE>::operator [] (uint32_t index) const
{
    ASSERT(isValidIndex(index));
    return static_cast<const VALUE&>(mValueList[index]);
}

template< typename VALUE >
inline TEFixedArray<VALUE> & TEFixedArray<VALUE>::operator = ( const TEFixedArray<VALUE>& src )
{
    copy(src);
    return (*this);
}

template< typename VALUE >
inline TEFixedArray<VALUE> & TEFixedArray<VALUE>::operator = ( TEFixedArray<VALUE> && src ) noexcept
{
    move(std::move(src));
    return (*this);
}

template< typename VALUE >
inline bool TEFixedArray<VALUE>::operator == ( const TEFixedArray<VALUE>& other ) const
{
    return ((mElemCount == other.getSize()) && NEMemory::equalElements<VALUE>(mValueList, other.mValueList, mElemCount));
}

template< typename VALUE >
inline bool TEFixedArray<VALUE>::operator != (const TEFixedArray<VALUE>& other) const
{
    return ((mElemCount != other.getSize()) || !NEMemory::equalElements<VALUE>(mValueList, other.mValueList, mElemCount));
}

template< typename VALUE >
inline TEFixedArray<VALUE>::operator const VALUE * ( void ) const
{
    return mValueList;
}

template< typename VALUE >
inline bool TEFixedArray<VALUE>::isEmpty( void ) const
{
    return (mElemCount == 0);
}

template< typename VALUE >
inline uint32_t TEFixedArray<VALUE>::getSize( void ) const
{
    return mElemCount;
}

template< typename VALUE >
inline bool TEFixedArray<VALUE>::isValidIndex(uint32_t whichIndex) const
{
    return (whichIndex < mElemCount);
}

template< typename VALUE >
inline bool TEFixedArray<VALUE>::contains(const VALUE& elemSearch, uint32_t startAt /*= 0*/) const
{
    bool result = false;
    for (uint32_t i = startAt; i < mElemCount; ++i)
    {
        if (mValueList[i] == elemSearch)
        {
            result = true;
            break;
        }
    }

    return result;
}

template< typename VALUE >
inline void TEFixedArray<VALUE>::clear(void)
{
    delete[] mValueList;
    mValueList = nullptr;
    mElemCount = 0;
}

template< typename VALUE >
inline const VALUE& TEFixedArray<VALUE>::getAt(uint32_t index ) const
{
    ASSERT(isValidIndex(index));
    return static_cast<const VALUE&>(mValueList[index]);
}

template< typename VALUE >
inline VALUE & TEFixedArray<VALUE>::getAt(uint32_t index )
{
    ASSERT(isValidIndex(index));
    return mValueList[index];
}

template< typename VALUE >
inline void TEFixedArray<VALUE>::setAt(uint32_t index, const VALUE& newValue )
{
    ASSERT(isValidIndex(index));
    mValueList[index] = newValue;
}

template< typename VALUE >
inline void TEFixedArray<VALUE>::setAt(uint32_t index, VALUE && newValue)
{
    ASSERT(isValidIndex(index));
    mValueList[index] = std::move(newValue);
}

template< typename VALUE >
inline const VALUE* TEFixedArray<VALUE>::getValues( void ) const
{
    return  mValueList;
}

template< typename VALUE >
inline void TEFixedArray<VALUE>::copy(const TEFixedArray< VALUE >& src)
{
    if (static_cast<const TEFixedArray<VALUE> *>(this) != &src)
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
inline void TEFixedArray<VALUE>::move(TEFixedArray< VALUE > && src) noexcept
{
    if (static_cast<const TEFixedArray<VALUE> *>(this) != &src)
    {
        delete[] mValueList;

        mValueList = src.mValueList;
        mElemCount = src.mElemCount;
        src.mValueList = nullptr;
        src.mElemCount = 0;
    }
}

template< typename VALUE >
inline int TEFixedArray<VALUE>::find(const VALUE& elemSearch, uint32_t startAt /* = 0 */) const
{
    int result = NECommon::INVALID_INDEX;
    for (uint32_t i = 0; i < mElemCount; ++i)
    {
        if (elemSearch == mValueList[i])
        {
            result = static_cast<int>(i);
            break;
        }
    }

    return result;
}


template< typename VALUE >
inline void TEFixedArray<VALUE>::resize(uint32_t newLength)
{
    VALUE * newList = newLength != 0 ? DEBUG_NEW VALUE[newLength] : nullptr;
    int count = MACRO_MIN(newLength, mElemCount);
    for (int i = 0; i < count; ++ i)
        newList[i] = mValueList[i];
    
    delete [] mValueList;

    mValueList  = newList;
    mElemCount  = newLength;
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE>
const IEInStream & operator >> ( const IEInStream & stream, TEFixedArray<VALUE> & input )
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

template<typename VALUE>
IEOutStream & operator << ( IEOutStream & stream, const TEFixedArray<VALUE> & output )
{
    stream << output.mElemCount;
    for (uint32_t i = 0; i < output.mElemCount; ++ i )
    {
        stream << output.mValueList[i];
    }

    return stream;
}
