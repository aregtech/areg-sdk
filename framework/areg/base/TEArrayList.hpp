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

#include <utility>
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
 *                      possible to convert to type const VALUE &.
 * \tparam  Implement   The implementation of value equality function used by array list.
 **/
template<typename VALUE, class Implement = TEListImpl<const VALUE &>>
class TEArrayList
{
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
    inline VALUE & operator [] ( int index );

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid index.
     *          The index should be valid number between 0 and (mSize -1).
     *          May be used on either the right (r-value).
     **/
    inline const VALUE & operator [] ( int index ) const;

    /**
     * \brief   Assigning operator. Copies all values from given source.
     *          If Array previously had values, they will be removed and new values
     *          from source Array will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of list of values.
     **/
    inline TEArrayList<VALUE, Implement> & operator = ( const TEArrayList<VALUE, Implement> & src );

    /**
     * \brief   Move operator. Movesall values from given source.
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
    bool operator == ( const TEArrayList<VALUE, Implement> & other ) const;

    /**
     * \brief   Checks inequality of 2 hash-map objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The hash-map object to compare
     **/
    bool operator != ( const TEArrayList<VALUE, Implement> & other ) const;

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
    template <typename V, class Impl>
    friend const IEInStream & operator >> (const IEInStream & stream, TEArrayList<V, Impl> & input);

    /**
     * \brief   Writes to the stream Array values.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream every value of Array and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Array object to read out values.
     **/
    template <typename V, class Impl>
    friend IEOutStream & operator << (IEOutStream & stream, const TEArrayList<V, Impl> & output);

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
    inline int getSize( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Delete extra entries in array
     **/
    void freeExtra( void );

    /**
     * \brief   Returns true if the specified index is valid.
     **/
    inline bool isValidIndex( int index ) const;

    /**
     * \brief   Remove all entries of array
     **/
    inline void removeAll( void );
    
    /**
     * \brief   Returns element value by valid index. 
     *          If index is not valid, assertion is raised.
     **/
    inline const VALUE & getAt( int index ) const;

    /**
     * \brief   Returns instance of element at given valid index.
     *          If index is not valid, assertion is raised.
     **/
    inline VALUE & getAt( int index );

    /**
     * \brief   Sets new element at given valid index.
     *          If index is not valid, assertion is raised.
     **/
    inline void setAt( int index, const VALUE & newElement );

    /**
     * \brief   Returns array of elements.
     **/
    inline const VALUE * getValues( void ) const;

    /**
     * \brief   Adds new element at the end of array and returns index value of new inserted element
     * \param   newElement  New element to add at the end of array.
     * \return  Returns the index of new element in the array.
     **/
    inline int add( const VALUE & newElement );

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
     * \return	Returns new size of array. 
     **/
    inline int append( const TEArrayList<VALUE, Implement> & src );

    /**
     * \brief	Copies all entries from given source
     * \param	src	    The source of array elements
     * \return	Returns actual size of array.
     **/
    int copy( const TEArrayList<VALUE, Implement> & src );

    /**
     * \brief	Moves all entries from given source. On output, the source is empty.
     * \param	src	    The source of array elements
     * \return	Returns actual size of array.
     **/
    int move( TEArrayList<VALUE, Implement> && src ) noexcept;

    /**
     * \brief   Inserts element at the specified zero-based position. If position is valid, it shifts
     *          elements and inserts at specified position. If position is equal or more than number
     *          of elements in the array, it inserts element at end of the array without shifting elements.
     *          Nothing happens if index is negative.
     * \param   index       The index position to insert. If valid position, shifts the elements to insert
     *                      new element. If more than element count, inserts element at the end of array.
     * \param   newElement  Value of new element to insert.
     * \param   elemCount   If not one, it will repeat operation.
     * \return  Returns the zero-based position of last element that was inserted. If 'elemCount' is one,
     *          it returns the zero-based position where the element is inserted. If 'elemCount' is more than one,
     *          it returns the zero-based position of last element inserted in the list.
     **/
    inline int insertAt( int index, const VALUE & newElement, uint32_t elemCount = 1 );

    /**
     * \brief	Inserts new entries from given array source
     * \param	startAt	    Starting index position to insert entries.
     * \param	newArray	Sources of array elements
     **/
    inline int insertAt( int startAt, const TEArrayList<VALUE, Implement> & newArray );

    /**
     * \brief	Removes elemCount element starting at given index position.
     *          The index should be valid
     * \param	index	    The index to start removing elements
     * \param	elemCount	Amount of elements to remove.
     **/
    inline void removeAt( int index, uint32_t elemCount = 1);

    /**
     * \brief   Remove the element at specified index and returns the removed element.
     * \param   index   The index of element to remove.
     * \return  The actual returned element.
     */
    inline VALUE removePos( uint32_t index );

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
    inline int find( const VALUE & elemSearch, int startAt = 0 ) const;

    /**
     * \brief	Checks whether given element exist in array or not.
     *          return true if found. Starts searching at given position.
     * \param	elemSearch	Element to search, should have comparing operator
     * \param	startAt	    The index to start searching. If negative,
     *          search from begging.
     * \return	Returns true if could find element starting at given position.
     **/
    inline bool exist( const VALUE & elemSearch, int startAt = 0 ) const;

    /**
     * \brief   Ensures validity of index. If index is more that element count in array,
     *          it will increase number of elements to (index + 1) that the value 'index'
     *          is valid. Otherwise, it does nothing.
     * \note    If isValidIndex() function only checks the validity of index, this function
     *          ensures that the index is valid, i.e. if needed, it changes the size of array.
     * \param   index   The index to ensure. Cannot be negative value
     * \return  Returns true if index is validation succeeded.
     **/
    inline bool validateIndex( int index );

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
     */
    inline void reserve( uint32_t newCapacity );

//////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief	Shifts array elements starting at given index position and reserves placeCount entries in new array
     * \param	startAt	    The starting index position to start shifting
     * \param	placeCount	The space to reserve
     **/
    // void shift( int startAt, int placeCount );

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
     * \brief	Constructs element entries in the given list
     * \param	valueList	The list to construct elements
     * \param	elemCount	The number of element entries in list
     **/
    // inline void construct( VALUE * valueList, int elemCount );

    /**
     * \brief	Destructs element entries in the given list
     * \param	valueList	The list to destroy elements
     * \param	elemCount	The number of element entries in list
     **/
    // inline void destruct( VALUE * valueList, int elemCount );

    inline typename std::vector<VALUE>::const_iterator getPosition( uint32_t index ) const;
    inline typename std::vector<VALUE>::iterator getPosition(uint32_t index);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The array of elements.
     */
    std::vector<VALUE>  mElements;
    /**
     * \brief   Instance of object that compares values.
     **/
    Implement   mImplement;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEArrayList<VALUE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
TEArrayList<VALUE, Implement>::TEArrayList(uint32_t capacity /*= 0*/)
    : mElements ( capacity )
{
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
TEArrayList<VALUE, Implement>::TEArrayList( const TEArrayList<VALUE, Implement> & src )
    : mElements (src.mElements)
{
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
TEArrayList<VALUE, Implement>::TEArrayList( TEArrayList<VALUE, Implement> && src ) noexcept
    : mElements (static_cast<std::vector<VALUE> &&>(src.mElements))
{
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::isEmpty( void ) const
{
    return mElements.empty();
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline int TEArrayList<VALUE, Implement>::getSize( void ) const
{
    return static_cast<int>(mElements.size());
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
void TEArrayList<VALUE, Implement>::freeExtra( void )
{
    mElements.shrink_to_fit();
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::isValidIndex( int index ) const
{
    return ( (index >= 0) && (index < static_cast<int>(mElements.size())) );
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::removeAll( void )
{
    mElements.clear();
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline const VALUE & TEArrayList<VALUE, Implement>::getAt(int index) const
{
    ASSERT(isValidIndex(index));
    return mElements.at(index);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline VALUE& TEArrayList<VALUE, Implement>::getAt(int index)
{
    ASSERT(isValidIndex(index));
    return mElements.at(index);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::setAt(int index, const VALUE & newElement)
{
    if (isValidIndex(index))
    {
        mElements[index] = newElement;
    }
    else if (index >= 0)
    {
        mElements.push_back(newElement);
    }
#ifdef _DEBUG
    else
    {
        ASSERT(false);
    }
#endif // _DEBUG
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::validateIndex(int index)
{
    ASSERT(index >= 0);
    if (index >= static_cast<int>(mElements.size()))
    {
        reserve(index + 1);
    }

    return ((index >= 0) && (static_cast<int>(mElements.size()) >= index + 1));
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::resize( uint32_t newSize )
{
    mElements.resize(newSize);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::reserve( uint32_t newCapacity)
{
    mElements.reserve(newCapacity);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline const VALUE* TEArrayList<VALUE, Implement>::getValues( void ) const
{
    return static_cast<const VALUE *>(mElements.data());
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline int TEArrayList<VALUE, Implement>::add(const VALUE & newElement)
{
    mElements.push_back(newElement);
    return static_cast<int>(mElements.size()) - 1;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::addUnique(const VALUE & newElement)
{
    bool isUnique = true;
    for (const VALUE& elem : mElements)
    {
        if (isEqualValues(elem, newElement))
        {
            isUnique = false;
            break;
        }
    }

    if (isUnique)
    {
        mElements.push_back(newElement);
    }

    return isUnique;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline int TEArrayList<VALUE, Implement>::append(const TEArrayList<VALUE, Implement>& src)
{
    ASSERT(this != &src);

    int curPos  = static_cast<int>(mElements.size());
    mElements.resize(curPos + src.getSize());
    for (const VALUE& elem : src.mElements)
    {
        mElements[curPos ++] = elem;
    }

    return curPos;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
int TEArrayList<VALUE, Implement>::copy(const TEArrayList<VALUE, Implement>& src)
{
    ASSERT(this != &src);

    mElements = src.mElements;
    return static_cast<int>(mElements.size());
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
int TEArrayList<VALUE, Implement>::move( TEArrayList<VALUE, Implement> && src ) noexcept
{
    ASSERT( this != &src );

    mElements = std::move(src.mElements);
    return static_cast<int>(mElements.size());
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline VALUE& TEArrayList<VALUE, Implement>::operator [] (int index)
{
    return mElements[index];
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline const VALUE & TEArrayList<VALUE, Implement>::operator [] (int index) const
{
    return mElements[index];
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
TEArrayList<VALUE, Implement> & TEArrayList<VALUE, Implement>::operator = ( const TEArrayList<VALUE, Implement>& src )
{
    mElements = src.mElements;
    return (*this);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
TEArrayList<VALUE, Implement> & TEArrayList<VALUE, Implement>::operator = ( TEArrayList<VALUE, Implement> && src ) noexcept
{
    mElements = std::move(src.mElements);
    return (*this);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
bool TEArrayList<VALUE, Implement>::operator == ( const TEArrayList<VALUE, Implement>& other ) const
{
    return mElements == other.mElements;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
bool TEArrayList<VALUE, Implement>::operator != (const TEArrayList<VALUE, Implement>& other) const
{
    return mElements != other.mElements;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline TEArrayList<VALUE, Implement>::operator const VALUE * ( void ) const
{   
    return static_cast<const VALUE *>(mElements.data());
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline int TEArrayList<VALUE, Implement>::insertAt(int index, const VALUE & newElement, uint32_t elemCount /*= 1*/)
{
    std::vector<VALUE>::const_iterator cit = getPosition(index);
    if (elemCount > 0)
    {
        if (cit != mElements.end())
        {
            mElements.insert(cit, elemCount, newElement);
        }
        else
        {
            for (uint32_t i = 0; i < elemCount; ++i)
            {
                mElements.push_back(newElement);
            }

            index = static_cast<int>(mElements.size() - 1);
        }
    }

    return index;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::removeAt(int index, uint32_t elemCount /*= 1*/)
{
    if (elemCount != 0)
    {
        elemCount -= 1;
        ASSERT(isValidIndex(index) && isValidIndex(index + elemCount));
        std::vector<VALUE>::const_iterator first = getPosition(index);
        if (elemCount == 0)
        {
            mElements.erase(first);
        }
        else
        {
            std::vector<VALUE>::const_iterator last = first + elemCount;
            mElements.erase(first, last);
        }
    }
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline VALUE TEArrayList<VALUE, Implement>::removePos(uint32_t index)
{
    ASSERT(isValidIndex(index));
    std::vector<VALUE>::const_iterator first = getPosition(index);
    *(mElements.erase(first));
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline int TEArrayList<VALUE, Implement>::insertAt(int startAt, const TEArrayList<VALUE, Implement>& newArray)
{
    ASSERT(startAt >= 0);
    int result = startAt < static_cast<int>(mElements.size()) ? startAt : static_cast<int>(mElements.size() - 1);
    if (newArray.mElements.empty() == false)
    {
        std::vector<VALUE>::const_iterator cit = getPosition(startAt);
        mElements.insert(cit, newArray.mElements.begin(), newArray.mElements.end());
    }

    return (result + newArray.getSize());
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline int TEArrayList<VALUE, Implement>::find( const VALUE & elemSearch, int startAt /*= 0*/ ) const
{
    int count = static_cast<int>(mElements.size());
    startAt = startAt <= 0 ? 0 : MACRO_MIN(startAt, count);
    std::vector<VALUE>::const_iterator cit = getPosition(startAt);
    
    int result = startAt;
    for (; cit != mElements.end(); ++cit, ++result)
    {
        if (isEqualValues(*cit, elemSearch))
        {
            break;
        }
    }

    return (result < count ? result : -1);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::exist( const VALUE & elemSearch, int startAt /*= 0*/ ) const
{
    return (find(elemSearch, startAt) >= 0);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::removeElem( const VALUE & elemRemove, uint32_t searchAt /*= 0*/ )
{
    bool result = false;
    std::vector<VALUE>::const_iterator cit = getPosition(searchAt);
    for (; cit != mElements.end(); ++cit)
    {
        if (isEqualValues(*cit, elemRemove))
        {
            mElements.erase(cit);
            result = true;
            break;
        }
    }

    return result;
}

#if 0
template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
void TEArrayList<VALUE, Implement>::shift(int startAt, int placeCount)
{
    ASSERT(startAt >= 0 && placeCount > 0);
    int oldSize = mElemCount;
    if (startAt >= mElemCount)
    {
        setSize(startAt + placeCount, -1);
        mElemCount = startAt + placeCount;
    }
    else
    {
        setSize(mElemCount + placeCount, -1);
        mElemCount += placeCount;
        NEMemory::moveElems<VALUE, Implement>(mValueList + startAt + placeCount, mValueList + startAt, oldSize - startAt);
    }

    ASSERT(startAt + placeCount <= mElemCount);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::construct(VALUE* valueList, int elemCount)
{
    NEMemory::constructElems<VALUE, Implement>(static_cast<void*>(valueList), elemCount);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::destruct(VALUE* valueList, int elemCount)
{
    NEMemory::destroyElems<VALUE, Implement>(valueList, elemCount);
}

#endif

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline void TEArrayList<VALUE, Implement>::setSize(uint32_t elemCount)
{
    mElements.resize(elemCount);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline bool TEArrayList<VALUE, Implement>::isEqualValues(const VALUE & value1, const VALUE & value2) const
{
    // return mImplement.implEqualValues(value1, value2);
    return (value1 == value2);
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline typename std::vector<VALUE>::const_iterator TEArrayList<VALUE, Implement>::getPosition(uint32_t index) const
{
    std::vector<VALUE>::const_iterator result = mElements.begin();
    for (; (index != 0) && (result != mElements.end()); --index)
        ++ result;

    return result;
}

template<typename VALUE, class Implement /*= TEListImpl<const VALUE &>*/>
inline typename std::vector<VALUE>::iterator TEArrayList<VALUE, Implement>::getPosition(uint32_t index)
{
    std::vector<VALUE>::iterator result = mElements.begin();
    for (; (index != 0) && (result != mElements.end()); --index)
        ++result;

    return result;
}


//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template <typename V, class Impl>
const IEInStream & operator >> ( const IEInStream & stream, TEArrayList<V, Impl> & input )
{
    int size = 0;
    stream >> size;
    input.removeAll();
    input.setSize( size );
 
    for (V& elem : input.mElements)
    {
        stream >> elem;
    }

    return stream;
}

template <typename V, class Impl>
IEOutStream & operator << ( IEOutStream& stream, const TEArrayList<V, Impl>& output )
{
    stream << output.getSize();
    for (const V& elem : output.mElements)
    {
        stream << elem;
    }

    return stream;
}
