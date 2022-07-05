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
 * \file        areg/base/TEArrayList.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Array List class template
 *              This class template defines array of elements.
 *              It allows to store elements in array, making general
 *              operations and having access by integer index value.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TETemplateBase.hpp"

#include "areg/base/NECommon.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/IEIOStream.hpp"

#include <vector>

//////////////////////////////////////////////////////////////////////////
// TEArrayList< VALUE > class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Array List has general functionalities to access, insert, move, find
 *          and copy elements.
 * 
 *          For performance issue, it is recommended to pass capacity value
 *          in constructor to define initial reserved space for array.
 *          This capacity value will also define the initial growing size
 *          array. By default, the minimum growing size of array is
 *          NECommon::ARRAY_DEFAULT_MIN_GROW and cannot be more
 *          than NECommon::ARRAY_DEFAULT_MAX_GROW.
 *          The VALUE types should have at least default constructor
 *          and valid public available assigning operator.
 *          The elements of array can be fast accessed  for read and change 
 *          by valid index value. If the index of array element to modify 
 *          is out of valid scope, use add() function to add new element 
 *          at the end.
 *          The ArryList object is not thread safe and data access should be 
 *          synchronized manually.
 *
 * \tparam  VALUE       The type of stored items. Either should be 
 *                      primitive or should have default constructor 
 *                      and valid assigning operator. Also, should be 
 *                      possible to convert to type 'const VALUE &'.
 * \tparam  Implement   The implementation of value equality function used by array list.
 **/
template<typename VALUE>
class TEArrayList   : private Constless<std::vector<VALUE>>
{
protected:
    using ARRAYPOS  = typename std::vector<VALUE>::iterator;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     * \param   capacity    Initial reserved space of array. By default the initial reserved space is zero.
     **/
    TEArrayList( uint32_t capacity = 0 );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TEArrayList( const TEArrayList< VALUE > & src );

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    TEArrayList( TEArrayList< VALUE > && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TEArrayList( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// basic operators
/************************************************************************/

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid index.
     *          The index should be valid number between 0 and (mSize -1).
     *          May be used on either the right (r-value) or the left (l-value) of an assignment statement.
     **/
    inline VALUE & operator [] ( uint32_t index );

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid index.
     *          The index should be valid number between 0 and (mSize -1).
     *          May be used on either the right (r-value).
     **/
    inline const VALUE & operator [] (uint32_t index ) const;

    /**
     * \brief   Assigning operator. Copies all values from given source.
     *          If Array previously had values, they will be removed and new values
     *          from source Array will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of list of values.
     **/
    inline TEArrayList< VALUE > & operator = ( const TEArrayList< VALUE > & src );

    /**
     * \brief   Move operator. Moves all values from given source.
     *          If Array previously had values, they will be removed and new values
     *          from source Array will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of list of values.
     **/
    inline TEArrayList< VALUE > & operator = ( TEArrayList< VALUE > && src ) noexcept;

    /**
     * \brief   Checks equality of 2 hash-map objects, and returns true if they are equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The array object to compare
     **/
    inline bool operator == ( const TEArrayList< VALUE > & other ) const;

    /**
     * \brief   Checks inequality of 2 hash-map objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The hash-map object to compare
     **/
    inline bool operator != ( const TEArrayList< VALUE > & other ) const;

    /**
     * \brief   Returns pointer to the array value. The values cannot be modified
     **/
    inline operator const VALUE * ( void ) const;

/************************************************************************/
// Friend global operators to make Array streamable
/************************************************************************/
    /**
     * \brief   Reads out from the stream Array values.
     *          If Array previously had values, they will be lost.
     *          The values in the Array will be initialized in the same sequence
     *          as they were written.
     *          There should be possibility to initialize values from streaming object and
     *          if VALUE is not a primitive, but an object, it should have
     *          implemented streaming operator.
     * \param   stream  The streaming object for reading values
     * \param   input   The Array object to save initialized values.
     **/
    template <typename VALUE>
    friend const IEInStream & operator >> (const IEInStream & stream, TEArrayList< VALUE > & input);

    /**
     * \brief   Writes to the stream Array values.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream every value of Array and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Array object to read out values.
     **/
    template <typename VALUE>
    friend IEOutStream & operator << (IEOutStream & stream, const TEArrayList< VALUE > & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the array list is empty and has no element
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief	Returns the current size of array
     **/
    inline uint32_t getSize( void ) const;

    /**
     * \brief   Returns true if the specified index is valid.
     **/
    inline bool isValidIndex(const uint32_t index) const;

    /**
     * \brief   Returns true if specified position pointing start of the array.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the array.
     **/
    inline bool isStartPosition(const LISTPOS pos) const;

    /**
     * \brief   Returns true if specified position pointing start of the array.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the array.
     **/
    inline bool isLastPosition(const LISTPOS pos) const;

    /**
     * \brief   Returns the invalid position of the array list.
     */
    ARRAYPOS invalidPosition(void) const;

    /**
     * \brief   Returns true if specified position is invalid, i.e. points the end of the array.
     **/
    bool isInvalidPosition(const ARRAYPOS pos) const;

    /**
     * \brief   Returns true if the given position is valid, i.e. is not pointing the end of array.
     *          Note, it does not check whether there is a such position in the array,
     *          The method ensures that the position is not pointing to the invalid value.
     **/
    inline bool isValidPosition(const ARRAYPOS pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the array.
     *          The duration of checkup depends on the location of the position in the array.
     * \param pos       The position to check.
     * \return  Returns true if specified position points to the valid entry in the array.
     */
    inline bool checkPosition(const ARRAYPOS pos) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Remove all entries of array
     **/
    inline void clear(void);

    /**
     * \brief   Delete extra entries in array.
     **/
    inline void freeExtra( void );

    /**
     * \brief   Sets the size of array to zero and deletes all capacity space.
     **/
    inline void release(void);

    /**
     * \brief   Returns element value by valid index. 
     *          If index is not valid, assertion is raised.
     **/
    inline const VALUE & getAt( uint32_t index ) const;

    /**
     * \brief   Returns instance of element at given valid index.
     *          If index is not valid, assertion is raised.
     **/
    inline VALUE & getAt( uint32_t index );

    /**
     * \brief   Sets new element at given valid index.
     *          If index is not valid, assertion is raised.
     **/
    inline void setAt( uint32_t index, const VALUE & newElement );

    /**
     * \brief   Returns array of elements.
     **/
    inline const VALUE * getValues( void ) const;

    /**
     * \brief   Adds new element at the end of array and returns index value of new inserted element
     * \param   newElement  New element to add at the end of array.
     **/
    inline void add( const VALUE & newElement );

    /**
     * \brief   Adds new element at the end of the array only if the element does not exist.
     *          The function searches given parameter in the list starting from beginning, 
     *          if does not find any entry, it adds given parameter to the end and returns true.
     * \param   newElement  New element to add at the end of array.
     * \return  Returns true if succeeded to add element at the end of array. If specified element
     *          exists in array, it ignores operation and returns false.
     **/
    inline bool addUnique(const VALUE & newElement);

    /**
     * \brief	Appends entries taken from source at the end of array.
     * \param	src	    The source of new entries.
     **/
    inline void append( const TEArrayList< VALUE > & src );

    /**
     * \brief	Copies all entries from given source
     * \param	src	    The source of array elements
     **/
    inline void copy( const TEArrayList< VALUE > & src );

    /**
     * \brief	Moves all entries from given source. On output, the source is empty.
     * \param	src	    The source of array elements
     **/
    inline void move( TEArrayList< VALUE > && src ) noexcept;

    /**
     * \brief	Extracts entries from the given source and inserted into the array list.
     * \param	src	    The source of array elements to merge
     **/
    inline void merge(const TEArrayList< VALUE > & src);
    inline void merge(TEArrayList< VALUE > && src);

    /**
     * \brief   Inserts element at the specified zero-based position. If position is valid, it shifts
     *          elements and inserts at specified position. If position is equal or more than number
     *          of elements in the array, it inserts element at end of the array without shifting elements.
     *          Nothing happens if index is negative.
     * \param   startAt     The index position to insert. If valid position, shifts the elements to insert
     *                      new element. If more than element count, inserts element at the end of array.
     * \param   newElement  Value of new element to insert.
     * \param   elemCount   If not one, it will repeat operation.
     **/
    inline void insertAt( uint32_t startAt, const VALUE & newElement, uint32_t elemCount = 1 );

    /**
     * \brief	Inserts new entries from given array source
     * \param	startAt	    Starting index position to insert entries.
     * \param	newArray	Sources of array elements to insert.
     * \param   count       Number of elements in the array.
     **/
    inline void insertAt(uint32_t startAt, const VALUE * newArray, uint32_t count );

    /**
     * \brief	Inserts new entries from given array source
     * \param	startAt	    Starting index position to insert entries.
     * \param	newArray	Sources of array elements
     **/
    inline void insertAt( uint32_t startAt, const TEArrayList< VALUE > & newArray );

    /**
     * \brief	Removes elemCount element starting at given index position.
     *          The index should be valid
     * \param	index	    The index to start removing elements
     * \param	elemCount	Amount of elements to remove.
     **/
    inline void removeAt(uint32_t index, uint32_t elemCount = 1);

    /**
     * \brief   Remove the element at specified index and returns the removed element.
     * \param   index   The index of element to remove.
     * \return  The actual returned element.
     */
    inline VALUE removePosition( uint32_t index );

    /**
     * \brief	Search element starting from given position and removes by first match
     * \param	elemRemove	Element to search and remove.
     * \param	searchAt	Start searching at given position
     * \return	Returns true if removed element.
     **/
    inline bool removeElem(const VALUE& elemRemove, uint32_t searchAt = 0);

    /**
     * \brief	Search element entry in the array and returns index.
     *          If element is not found, returns -1.
     * \param	elemSearch	Element to search, should have comparing operator
     * \param	startAt	    The index to start searching. If negative value,
     *                      search from begging.
     * \return	If found, returns index of element in array. Otherwise return -1.
     **/
    inline int find( const VALUE & elemSearch, uint32_t startAt = 0 ) const;

    /**
     * \brief	Checks whether given element exist in array or not.
     *          return true if found. Starts searching at given position.
     * \param	elemSearch	Element to search, should have comparing operator
     * \param	startAt	    The index to start searching. If negative,
     *          search from begging.
     * \return	Returns true if could find element starting at given position.
     **/
    inline bool contains( const VALUE & elemSearch, uint32_t startAt = 0 ) const;

    /**
     * \brief   Ensures validity of index. If index is more that element count in array,
     *          it will increase number of elements to (index + 1) that the value 'index'
     *          is valid. Otherwise, it does nothing.
     * \note    If isValidIndex() function only checks the validity of index, this function
     *          ensures that the index is valid, i.e. if needed, it changes the size of array.
     * \param   index   The index to ensure. Cannot be negative value
     * \return  Returns true if index is validation succeeded.
     **/
    inline bool validateIndex(uint32_t index );

    /**
     * \brief	Sets new size of array. If size is zero, all elements are removed.
     *          Otherwise, the size of array is set and if was not empty, 
     *          existing elements are copied or truncated, depending
     *          whether new size is more or less than the existing size of array.
     * \param	newSize	    New size to set. If zero, array is emptied.
     * \param	increaseBy	The number of entries to increase array when reserving space
     * \return  Returns the number of element created by resizing array. The number of created elements
     *          might be less or equal to maximum size, but cannot be more.
     **/
    inline void resize( uint32_t newSize );

    /**
     * \brief   Reserves the elements in the array without changing the size of the array.
     *          If the newCapacity is greater than the existing capacity of the array, it allocates new
     *          space. Otherwise, it does nothing.
     * \param   newCapacity The new capacity of the array.
     **/
    inline void reserve( uint32_t newCapacity );

    /**
     * \brief	Shifts array elements starting at given index position. Reserves 'count' entries in new array
     *          or removes them depending whether the 'count' value is positive or negative.
     * \param	startAt	    The starting index position to start shifting
     * \param	count       The space to reserve. If the value is positive, the elements are stretched out, 
     *                      so that there are 'count' elements can be inserted. If the value is negative,
     *                      the elements are narrowed, so that there are 'count' elements are removed.
     **/
    void shift( uint32_t startAt, int count);

//////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief	Sets new size of array. 
     *          If size is zero, all elements will be removed.
     *          Otherwise the size of array is set and if was not empty, 
     *          existing elements are copied or truncated, depending
     *          whether new size is more or less than the existing size of array.
     * \param	elemCount   New size to set. If zero, array is emptied.
     **/
    inline void setSize( uint32_t elemCount );

    /**
     * \brief   Returns the position of the element at the given index.
     *          The value of returned position cannot be modified.
     * \param   index   The index of the element to return position.
     * \return  Returns the position of the element at the given index.
     *          The value of returned position cannot be modified.
     **/
    inline const ARRAYPOS getPosition( uint32_t index ) const;

    /**
     * \brief   Returns the position of the element at the given index.
     * \param   index   The index of the element to return position.
     * \return  Returns the position of the element at the given index.
     **/
    inline ARRAYPOS getPosition(uint32_t index);

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
// TEArrayList< VALUE > class template implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE >
TEArrayList< VALUE >::TEArrayList(uint32_t capacity /*= 0*/)
    : Constless ( )
    , mValueList( )
{
    if (capacity != 0)
    {
        mValueList.reserve(capacity > NECommon::MAX_CONTAINER_SIZE ? NECommon::MAX_CONTAINER_SIZE : capacity);
    }
}

template<typename VALUE >
TEArrayList< VALUE >::TEArrayList( const TEArrayList< VALUE > & src )
    : Constless ( )
    , mValueList(src.mValueList)
{
}

template<typename VALUE >
TEArrayList< VALUE >::TEArrayList( TEArrayList< VALUE > && src ) noexcept
    : Constless ( )
    , mValueList(std::move(src.mValueList))
{
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::isEmpty( void ) const
{
    return mValueList.empty();
}

template<typename VALUE >
inline uint32_t TEArrayList< VALUE >::getSize( void ) const
{
    return static_cast<uint32_t>(mValueList.size());
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::isValidIndex(uint32_t index) const
{
    return (index < static_cast<uint32_t>(mValueList.size()));
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::isStartPosition(const LISTPOS pos) const
{
    return (pos == mValueList.begin());
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::isLastPosition(const LISTPOS pos) const
{
    return ((mValueList.empty() == false) && (pos == --mValueList.end()));
}

template<typename VALUE >
inline typename TEArrayList< VALUE >::ARRAYPOS TEArrayList< VALUE >::invalidPosition(void) const
{
    return Constless::iter(mValueList, mValueList.end());
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::isInvalidPosition(const ARRAYPOS pos) const
{
    return (pos == mValueList.end());
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::isValidPosition(const ARRAYPOS pos) const
{
    return (pos != mValueList.end());
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::checkPosition(const ARRAYPOS pos) const
{
    std::vector<VALUE>::const_iterator it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template<typename VALUE >
inline void TEArrayList< VALUE >::clear(void)
{
    mValueList.clear();
}

template<typename VALUE >
inline void TEArrayList< VALUE >::freeExtra( void )
{
    mValueList.shrink_to_fit();
}

template<typename VALUE >
inline void TEArrayList< VALUE >::release(void)
{
    mValudList.clear();
    mValueList.shrink_to_fit();
}

template<typename VALUE >
inline const VALUE & TEArrayList< VALUE >::getAt(uint32_t index) const
{
    ASSERT(isValidIndex(index));
    return mValueList.at(index);
}

template<typename VALUE >
inline VALUE& TEArrayList< VALUE >::getAt(uint32_t index)
{
    ASSERT(isValidIndex(index));
    return mValueList.at(index);
}

template<typename VALUE >
inline void TEArrayList< VALUE >::setAt(uint32_t index, const VALUE & newElement)
{
    if (isValidIndex(index))
    {
        mValueList[index] = newElement;
    }
    else if ( NECommon::MAX_CONTAINER_SIZE > static_cast<uint32_t>(mValueList.size()) )
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
inline bool TEArrayList< VALUE >::validateIndex(uint32_t index)
{
    if ( (index < NECommon::MAX_CONTAINER_SIZE) && (index >= static_cast<uint32_t>(mValueList.size())) )
    {
        mValueList.resize(index + 1);
    }

    return (index < static_cast<uint32_t>(mValueList.size()));
}

template<typename VALUE >
inline void TEArrayList< VALUE >::resize( uint32_t newSize )
{
    mValueList.resize(newSize > NECommon::MAX_CONTAINER_SIZE ? NECommon::MAX_CONTAINER_SIZE : newSize);
}

template<typename VALUE >
inline void TEArrayList< VALUE >::reserve( uint32_t newCapacity)
{
    mValueList.reserve(newCapacity > NECommon::MAX_CONTAINER_SIZE ? NECommon::MAX_CONTAINER_SIZE : newCapacity);
}

template<typename VALUE >
inline const VALUE* TEArrayList< VALUE >::getValues( void ) const
{
    return static_cast<const VALUE *>(mValueList.data());
}

template<typename VALUE >
inline void TEArrayList< VALUE >::add(const VALUE & newElement)
{
    if (NECommon::MAX_CONTAINER_SIZE > static_cast<uint32_t>(mValueList.size()))
    {
        mValueList.push_back(newElement);
    }
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::addUnique(const VALUE & newElement)
{
    bool result = false;

    if (std::find(mValueList.begin(), mValueList.end(), newElement) == mValueList.end())
    {
        mValueList.push_back(newElement);
        result = true;
    }

    return result;
}

template<typename VALUE >
inline void TEArrayList< VALUE >::append(const TEArrayList< VALUE >& src)
{
    ASSERT(this != &src);

    if (NECommon::MAX_CONTAINER_SIZE >= (getSize() + src.getSize()))
    {
        for (const VALUE& elem : src.mValueList)
        {
            mValueList.push_back(elem);
        }
    }
    else
    {
        uint32_t count = NECommon::MAX_CONTAINER_SIZE - getSize();
        for (uint32_t i = 0; i < count; ++i)
        {
            mValueList.push_back(src[i]);
        }
    }
}

template<typename VALUE >
inline void TEArrayList< VALUE >::copy(const TEArrayList< VALUE >& src)
{
    ASSERT(this != &src);
    mValueList = src.mValueList;
}

template<typename VALUE >
inline void TEArrayList< VALUE >::move( TEArrayList< VALUE > && src ) noexcept
{
    ASSERT( this != &src );
    mValueList = std::move(src.mValueList);
}

template<typename VALUE >
inline void TEArrayList< VALUE >::merge(const TEArrayList< VALUE >& src)
{
    ASSERT(this != &src);
    uint32_t size   = mValueList.size();
    uint32_t remain = src.mValueList.size();
    if ((size + remain) > NECommon::MAX_CONTAINER_SIZE)
        remain = NECommon::MAX_CONTAINER_SIZE - mValueList.size();

    mValueList.reserve(size + remain);
    for (uint32_t i = 0; i < remain; ++i)
    {
        mValueList.push_back(src.mValueList[i]);
    }
}

template<typename VALUE >
inline void TEArrayList< VALUE >::merge( TEArrayList< VALUE > && src)
{
    ASSERT(this != &src);
    uint32_t size = mValueList.size();
    uint32_t remain = src.mValueList.size();
    if ((size + remain) > NECommon::MAX_CONTAINER_SIZE)
        remain = NECommon::MAX_CONTAINER_SIZE - mValueList.size();

    mValueList.reserve(size + remain);
    for (uint32_t i = 0; i < remain; ++i)
    {
        mValueList.push_back( std::move(src.mValueList[i]) );
    }
}

template<typename VALUE >
inline VALUE& TEArrayList< VALUE >::operator [] (uint32_t index)
{
    ASSERT(isValidIndex(index));
    return mValueList[index];
}

template<typename VALUE >
inline const VALUE & TEArrayList< VALUE >::operator [] (uint32_t index) const
{
    ASSERT(isValidIndex(index));
    return mValueList[index];
}

template<typename VALUE >
inline TEArrayList< VALUE > & TEArrayList< VALUE >::operator = ( const TEArrayList< VALUE >& src )
{
    mValueList = src.mValueList;
    return (*this);
}

template<typename VALUE >
inline TEArrayList< VALUE > & TEArrayList< VALUE >::operator = ( TEArrayList< VALUE > && src ) noexcept
{
    mValueList = std::move(src.mValueList);
    return (*this);
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::operator == ( const TEArrayList< VALUE >& other ) const
{
    return mValueList == other.mValueList;
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::operator != (const TEArrayList< VALUE >& other) const
{
    return mValueList != other.mValueList;
}

template<typename VALUE >
inline TEArrayList< VALUE >::operator const VALUE * ( void ) const
{   
    return static_cast<const VALUE *>(mValueList.data());
}

template<typename VALUE >
inline void TEArrayList< VALUE >::removeAt(uint32_t index, uint32_t elemCount /*= 1*/)
{
    if (elemCount != 0)
    {
        elemCount -= 1;
        ASSERT(isValidIndex(index) && isValidIndex(index + elemCount));
        ARRAYPOS first = getPosition(index);
        if (elemCount == 0)
        {
            mValueList.erase(first);
        }
        else
        {
            ARRAYPOS last = first + elemCount;
            mValueList.erase(first, last);
        }
    }
}

template<typename VALUE >
inline VALUE TEArrayList< VALUE >::removePosition(uint32_t index)
{
    ASSERT(isValidIndex(index));
    ARRAYPOS first = getPosition(index);
    *(mValueList.erase(first));
}

template<typename VALUE >
inline void TEArrayList< VALUE >::insertAt(uint32_t startAt, const VALUE& newElement, uint32_t elemCount /*= 1*/)
{
    if (elemCount != 0)
    {
        if ((getSize() + elemCount) > NECommon::MAX_CONTAINER_SIZE)
        {
            elemCount = NECommon::MAX_CONTAINER_SIZE - getSize();
        }

        ARRAYPOS cit = getPosition(startAt);
        if (cit != mValueList.end())
        {
            mValueList.insert(cit, elemCount, newElement);
        }
        else
        {
            for (uint32_t i = 0; i < elemCount; ++i)
            {
                mValueList.push_back(newElement);
            }
        }
    }
}

template<typename VALUE >
inline void TEArrayList< VALUE >::insertAt(uint32_t startAt, const VALUE* newArray, uint32_t count)
{
    ASSERT(isValidIndex(startAt));
    if ((newArray != nullptr) && (count != 0))
    {
        if ((getSize() + count) > NECommon::MAX_CONTAINER_SIZE)
        {
            count = NECommon::MAX_CONTAINER_SIZE - getSize();
        }

        shift(startAt, static_cast<int>(count));
        for (uint32_t i = 0; i < count; ++i)
        {
            mValueList[startAt ++] = newArray[i];
        }
    }
}

template<typename VALUE >
inline void TEArrayList< VALUE >::insertAt(uint32_t startAt, const TEArrayList< VALUE >& newArray)
{
    ASSERT(isValidIndex(startAt));

    if (newArray.mValueList.empty() == false)
    {
        uint32_t limit = 0;
        if ((getSize() + newArray.getSize()) > NECommon::MAX_CONTAINER_SIZE)
        {
            limit = NECommon::MAX_CONTAINER_SIZE - (getSize() + newArray.getSize());
        }

        ARRAYPOS cit = getPosition(startAt);
        mValueList.insert(cit, newArray.mValueList.begin(), newArray.mValueList.end() - limit);
    }
}

template<typename VALUE >
inline int TEArrayList< VALUE >::find( const VALUE & elemSearch, uint32_t startAt /*= 0*/ ) const
{
    int result = NECommon::INVALID_INDEX;
    if (startAt < static_cast<uint32_t>(mValueList.size()))
    {
        result = static_cast<int>(startAt) - 1;
        auto it = std::find_if( mValueList.begin() + startAt, mValueList.end()
                              , [&](const auto& elem) { ++result; return (elemSearch == elem);});

        if (it == mValueList.end())
        {
            result = NECommon::INVALID_INDEX;
        }
    }

    return result;
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::contains( const VALUE & elemSearch, uint32_t startAt /*= 0*/ ) const
{
    return (startAt < static_cast<uint32_t>(mValueList.size()) ? std::find(mValueList.begin() + startAt, mValueList.end(), elemSearch) != mValueList.end() : false);
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::removeElem( const VALUE & elemRemove, uint32_t searchAt /*= 0*/ )
{
    bool result = false;
    if (searchAt < static_cast<uint32_t>(mValueList.size()))
    {
        auto it = std::find(mValueList.begin() + searchAt, mValueList.end(), elemRemove);
        if (it != mValueList.end())
        {
            mValueList.erase(it);
            result = true;
        }
    }

    return result;
}

template<typename VALUE >
void TEArrayList< VALUE >::shift(uint32_t startAt, int count)
{
    if ((mValueList.size() != 0) && (startAt < mValueList.size()) && (count != 0))
    {
        if (count > 0)
        {
            if ((getSize() + count) > NECommon::MAX_CONTAINER_SIZE)
            {
                count = static_cast<int>(NECommon::MAX_CONTAINER_SIZE - getSize());
            }

            uint32_t posFirst   = static_cast<uint32_t>(mValueList.size()) - 1;
            uint32_t posLast    = posFirst + count;
            mValueList.resize(mValueList.size() + count);
            while (posFirst >= startAt)
            {
                mValueList[posLast --] = mValueList[posFirst --];
            }
        }
        else
        {
            count *= -1;
            uint32_t start  = static_cast<uint32_t>(count) > (startAt + 1) ? 0 : startAt - count;
            uint32_t last   = static_cast<uint32_t>(mValueList.size()) - 1;
            uint32_t length = start + (last - startAt) + 1;
            for (uint32_t i = startAt; i <= last; ++i)
            {
                mValueList[start++] = mValueList[i];
            }

            mValueList.resize(length);
        }
    }
}

template<typename VALUE >
inline void TEArrayList< VALUE >::setSize(uint32_t elemCount)
{
    mValueList.resize(elemCount);
}

template<typename VALUE >
inline const typename TEArrayList< VALUE >::ARRAYPOS TEArrayList< VALUE >::getPosition(uint32_t index) const
{
    return Constless::iter(mValueList, index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() + index : mValueList.end());
}

template<typename VALUE >
inline typename TEArrayList< VALUE >::ARRAYPOS TEArrayList< VALUE >::getPosition(uint32_t index)
{
    return Constless::iter(mValueList, index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() + index : mValueList.end());
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE>
const IEInStream & operator >> ( const IEInStream & stream, TEArrayList< VALUE > & input )
{
    uint32_t size = 0;
    stream >> size;
    input.setSize( size );
 
    for (VALUE & elem : input.mValueList)
    {
        stream >> elem;
    }

    return stream;
}

template<typename VALUE>
IEOutStream & operator << ( IEOutStream& stream, const TEArrayList< VALUE >& output )
{
    stream << output.getSize();
    for (const VALUE & elem : output.mValueList)
    {
        stream << elem;
    }

    return stream;
}
