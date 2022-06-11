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
// TEArrayList<VALUE, Implement> class template declaration
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
template<typename VALUE, class Implement = TEListImpl<const VALUE &>>
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
    TEArrayList( const TEArrayList<VALUE, Implement> & src );

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    TEArrayList( TEArrayList<VALUE, Implement> && src ) noexcept;

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
    inline TEArrayList<VALUE, Implement> & operator = ( const TEArrayList<VALUE, Implement> & src );

    /**
     * \brief   Move operator. Moves all values from given source.
     *          If Array previously had values, they will be removed and new values
     *          from source Array will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of list of values.
     **/
    inline TEArrayList<VALUE, Implement> & operator = ( TEArrayList<VALUE, Implement> && src ) noexcept;

    /**
     * \brief   Checks equality of 2 hash-map objects, and returns true if they are equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The array object to compare
     **/
    inline bool operator == ( const TEArrayList<VALUE, Implement> & other ) const;

    /**
     * \brief   Checks inequality of 2 hash-map objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The hash-map object to compare
     **/
    inline bool operator != ( const TEArrayList<VALUE, Implement> & other ) const;

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
    template <typename VALUE, class Implement>
    friend const IEInStream & operator >> (const IEInStream & stream, TEArrayList<VALUE, Implement> & input);

    /**
     * \brief   Writes to the stream Array values.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream every value of Array and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Array object to read out values.
     **/
    template <typename VALUE, class Implement>
    friend IEOutStream & operator << (IEOutStream & stream, const TEArrayList<VALUE, Implement> & output);

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

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Delete extra entries in array
     **/
    inline void freeExtra( void );

    /**
     * \brief   Returns true if the specified index is valid.
     **/
    inline bool isValidIndex( uint32_t index ) const;

    /**
     * \brief   Remove all entries of array
     **/
    inline void removeAll( void );
    
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
    inline void append( const TEArrayList<VALUE, Implement> & src );

    /**
     * \brief	Copies all entries from given source
     * \param	src	    The source of array elements
     **/
    inline void copy( const TEArrayList<VALUE, Implement> & src );

    /**
     * \brief	Moves all entries from given source. On output, the source is empty.
     * \param	src	    The source of array elements
     **/
    inline void move( TEArrayList<VALUE, Implement> && src ) noexcept;

    /**
     * \brief	Extracts entries from the given source and inserted into the array list.
     * \param	src	    The source of array elements to merge
     **/
    inline void merge(const TEArrayList<VALUE, Implement> & src);
    inline void merge(TEArrayList<VALUE, Implement> && src);

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
    inline void insertAt( uint32_t startAt, const TEArrayList<VALUE, Implement> & newArray );

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
     * \brief   Called when comparing 2 values of element.
     *          Create public method 'EqualValues' when need to change comparison.
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
    /**
     * \brief   Instance of object that compares values.
     **/
    Implement           mImplement;
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEArrayList<VALUE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
TEArrayList<VALUE, Implement>::TEArrayList(uint32_t capacity /*= 0*/)
    : Constless ( )
    , mValueList( )
{
    if (capacity != 0)
    {
        mValueList.reserve(capacity > NECommon::MAX_CONTAINER_SIZE ? NECommon::MAX_CONTAINER_SIZE : capacity);
    }
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
TEArrayList<VALUE, Implement>::TEArrayList( const TEArrayList<VALUE, Implement> & src )
    : Constless ( )
    , mValueList(src.mValueList)
{
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
TEArrayList<VALUE, Implement>::TEArrayList( TEArrayList<VALUE, Implement> && src ) noexcept
    : Constless ( )
    , mValueList(std::move(src.mValueList))
{
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::isEmpty( void ) const
{
    return mValueList.empty();
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline uint32_t TEArrayList<VALUE, Implement>::getSize( void ) const
{
    return static_cast<uint32_t>(mValueList.size());
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::freeExtra( void )
{
    mValueList.shrink_to_fit();
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::isValidIndex(uint32_t index ) const
{
    return ( index < static_cast<uint32_t>(mValueList.size()) );
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::removeAll( void )
{
    mValueList.clear();
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline const VALUE & TEArrayList<VALUE, Implement>::getAt(uint32_t index) const
{
    ASSERT(isValidIndex(index));
    return mValueList.at(index);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline VALUE& TEArrayList<VALUE, Implement>::getAt(uint32_t index)
{
    ASSERT(isValidIndex(index));
    return mValueList.at(index);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::setAt(uint32_t index, const VALUE & newElement)
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

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::validateIndex(uint32_t index)
{
    if ( (index < NECommon::MAX_CONTAINER_SIZE) && (index >= static_cast<uint32_t>(mValueList.size())) )
    {
        mValueList.resize(index + 1);
    }

    return (index < static_cast<uint32_t>(mValueList.size()));
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::resize( uint32_t newSize )
{
    mValueList.resize(newSize > NECommon::MAX_CONTAINER_SIZE ? NECommon::MAX_CONTAINER_SIZE : newSize);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::reserve( uint32_t newCapacity)
{
    mValueList.reserve(newCapacity > NECommon::MAX_CONTAINER_SIZE ? NECommon::MAX_CONTAINER_SIZE : newCapacity);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline const VALUE* TEArrayList<VALUE, Implement>::getValues( void ) const
{
    return static_cast<const VALUE *>(mValueList.data());
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::add(const VALUE & newElement)
{
    if (NECommon::MAX_CONTAINER_SIZE > static_cast<uint32_t>(mValueList.size()))
    {
        mValueList.push_back(newElement);
    }
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::addUnique(const VALUE & newElement)
{
    bool result = true;
    for (const VALUE& elem : mValueList)
    {
        if (isEqualValues(elem, newElement))
        {
            result = false;
            break;
        }
    }

    if (result)
    {
        if (NECommon::MAX_CONTAINER_SIZE > static_cast<uint32_t>(mValueList.size()))
        {
            mValueList.push_back(newElement);
        }
        else
        {
            result = false;
        }
    }

    return result;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::append(const TEArrayList<VALUE, Implement>& src)
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

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::copy(const TEArrayList<VALUE, Implement>& src)
{
    ASSERT(this != &src);
    mValueList = src.mValueList;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::move( TEArrayList<VALUE, Implement> && src ) noexcept
{
    ASSERT( this != &src );
    mValueList = std::move(src.mValueList);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::merge(const TEArrayList<VALUE, Implement>& src)
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

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::merge( TEArrayList<VALUE, Implement> && src)
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

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline VALUE& TEArrayList<VALUE, Implement>::operator [] (uint32_t index)
{
    ASSERT(isValidIndex(index));
    return mValueList[index];
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline const VALUE & TEArrayList<VALUE, Implement>::operator [] (uint32_t index) const
{
    ASSERT(isValidIndex(index));
    return mValueList[index];
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline TEArrayList<VALUE, Implement> & TEArrayList<VALUE, Implement>::operator = ( const TEArrayList<VALUE, Implement>& src )
{
    mValueList = src.mValueList;
    return (*this);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline TEArrayList<VALUE, Implement> & TEArrayList<VALUE, Implement>::operator = ( TEArrayList<VALUE, Implement> && src ) noexcept
{
    mValueList = std::move(src.mValueList);
    return (*this);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::operator == ( const TEArrayList<VALUE, Implement>& other ) const
{
    return mValueList == other.mValueList;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::operator != (const TEArrayList<VALUE, Implement>& other) const
{
    return mValueList != other.mValueList;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline TEArrayList<VALUE, Implement>::operator const VALUE * ( void ) const
{   
    return static_cast<const VALUE *>(mValueList.data());
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::removeAt(uint32_t index, uint32_t elemCount /*= 1*/)
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

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline VALUE TEArrayList<VALUE, Implement>::removePosition(uint32_t index)
{
    ASSERT(isValidIndex(index));
    ARRAYPOS first = getPosition(index);
    *(mValueList.erase(first));
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::insertAt(uint32_t startAt, const VALUE& newElement, uint32_t elemCount /*= 1*/)
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

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::insertAt(uint32_t startAt, const VALUE* newArray, uint32_t count)
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

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::insertAt(uint32_t startAt, const TEArrayList<VALUE, Implement>& newArray)
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

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline int TEArrayList<VALUE, Implement>::find( const VALUE & elemSearch, uint32_t startAt /*= 0*/ ) const
{
    uint32_t count = static_cast<int>(mValueList.size());
    startAt = startAt <= 0 ? 0 : MACRO_MIN(startAt, count);
    ARRAYPOS cit = getPosition(startAt);
    
    uint32_t result = startAt;
    for (; cit != mValueList.end(); ++cit, ++result)
    {
        if (isEqualValues(*cit, elemSearch))
        {
            break;
        }
    }

    return (result < count ? static_cast<int>(result) : NECommon::INVALID_INDEX);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::contains( const VALUE & elemSearch, uint32_t startAt /*= 0*/ ) const
{
    return (find(elemSearch, startAt) >= 0);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::removeElem( const VALUE & elemRemove, uint32_t searchAt /*= 0*/ )
{
    bool result = false;
    ARRAYPOS cit = getPosition(searchAt);
    for (; cit != mValueList.end(); ++cit)
    {
        if (isEqualValues(*cit, elemRemove))
        {
            mValueList.erase(cit);
            result = true;
            break;
        }
    }

    return result;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
void TEArrayList<VALUE, Implement>::shift(uint32_t startAt, int count)
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

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::setSize(uint32_t elemCount)
{
    mValueList.resize(elemCount);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::isEqualValues(const VALUE & value1, const VALUE & value2) const
{
    return mImplement.implEqualValues(value1, value2);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline const typename TEArrayList<VALUE, Implement>::ARRAYPOS TEArrayList<VALUE, Implement>::getPosition(uint32_t index) const
{
    std::vector<VALUE>::const_iterator result = mValueList.begin();
    for (; (index != 0) && (result != mValueList.end()); --index)
        ++ result;

    return Constless::iter(mValueList, result);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline typename TEArrayList<VALUE, Implement>::ARRAYPOS TEArrayList<VALUE, Implement>::getPosition(uint32_t index)
{
    std::vector<VALUE>::iterator result = mValueList.begin();
    for (; (index != 0) && (result != mValueList.end()); --index)
        ++result;

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE, class Implement>
const IEInStream & operator >> ( const IEInStream & stream, TEArrayList<VALUE, Implement> & input )
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

template<typename VALUE, class Implement>
IEOutStream & operator << ( IEOutStream& stream, const TEArrayList<VALUE, Implement>& output )
{
    stream << output.getSize();
    for (const VALUE & elem : output.mValueList)
    {
        stream << elem;
    }

    return stream;
}
