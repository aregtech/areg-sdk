#ifndef AREG_BASE_TEARRAYLIST_HPP
#define AREG_BASE_TEARRAYLIST_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/TEArrayList.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
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
#include <vector>

#include "areg/base/NECommon.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/IEIOStream.hpp"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
// TEArrayList< VALUE > class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Array List has general functionalities to access, insert, move, find
 *          and copy elements. The elements of array are fast accessed for read
 *          and modification by using index. Use add() method if to add new element.
 *
 *          The type VALUE should have at least default constructor, applicable
 *          comparing and assigning operators. The TEArrayList object is not
 *          thread safe and data access should be synchronized manually.
 *
 *          For performance issue, it is recommended to pass capacity value
 *          in constructor to define initial reserved space for array.
 *          By default, the capacity is NECommon::ARRAY_DEFAULT_CAPACITY.
 *
 * \tparam  VALUE   The type of stored elements should be either primitive or have
 *                  default constructor, applicable comparing and assigning operators.
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
     * \brief   Constructs object and sets the initial capacity of the ArrayList.
     * \param   capacity    Initial reserved space of array, it must be not less than the 'resize'.
     * \param   resize      Initial size of array.
     **/
    TEArrayList( uint32_t capacity = NECommon::ARRAY_DEFAULT_CAPACITY, uint32_t resize = 0 );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    TEArrayList( const TEArrayList< VALUE > & src );
    TEArrayList( const std::vector< VALUE > & src );

    /**
     * \brief   Compiles entries from the given array of objects.
     * \param   list    The list of entries to copy.
     * \param   count   The number of entries in the array.
     **/
    TEArrayList( const VALUE * list, uint32_t count );

    /**
     * \brief   Moves entries from given source.
     * \param   src     The source to move data.
     **/
    TEArrayList( TEArrayList< VALUE > && src ) noexcept;
    TEArrayList( std::vector< VALUE > && src ) noexcept;

    /**
     * \brief   Destructor.
     **/
    ~TEArrayList( void );

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
    inline VALUE & operator [] ( uint32_t index );

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid zero-based index.
     *          The index should be valid number between 0 and (mSize -1).
     *          May be used on the right (r-value).
     **/
    inline const VALUE & operator [] (uint32_t index ) const;

    /**
     * \brief   Assigning operator. Copies all values from given source.
     *          If array previously had values, they will be removed and new values
     *          from source array will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of array of values.
     **/
    inline TEArrayList< VALUE > & operator = ( const TEArrayList< VALUE > & src );
    inline TEArrayList< VALUE > & operator = ( const std::vector< VALUE > & src );

    /**
     * \brief   Move operator. Moves all values from given source.
     *          If Array previously had values, they will be removed and new values
     *          from source Array will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of array of values.
     **/
    inline TEArrayList< VALUE > & operator = ( TEArrayList< VALUE > && src ) noexcept;
    inline TEArrayList< VALUE > & operator = ( std::vector< VALUE > && src ) noexcept;

    /**
     * \brief   Checks equality of 2 array objects, and returns true if they are equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The array object to compare.
     **/
    inline bool operator == ( const TEArrayList< VALUE > & other ) const;
    inline bool operator == ( const std::vector< VALUE > & other ) const;

    /**
     * \brief   Checks inequality of 2 array objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The array object to compare.
     **/
    inline bool operator != ( const TEArrayList< VALUE > & other ) const;
    inline bool operator != ( const std::vector< VALUE > & other ) const;

    /**
     * \brief   Returns pointer to the array values. The values cannot be modified
     **/
    inline operator const VALUE * ( void ) const;

/************************************************************************/
// Friend global operators to make Array streamable
/************************************************************************/
    /**
     * \brief   Reads out from the stream array values.
     *          If array previously had values, they will be removed and new values
     *          from the stream will be set in the same sequence as they are present 
     *          in the stream. There should be possibility to initialize values from 
     *          streaming object and if VALUE is not a primitive, but an object, it 
     *          should have implemented streaming operator.
     * \param   stream  The streaming object to read values.
     * \param   input   The array object to save initialized values.
     **/
    template <typename V>
    friend const IEInStream & operator >> (const IEInStream & stream, TEArrayList< V > & input);

    /**
     * \brief   Writes to the stream the values of array.
     *          The values will be written to the stream starting from firs entry. 
     *          There should be possibility to stream values and if VALUE is not a 
     *          primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The stream to write values.
     * \param   output   The array object containing value to stream.
     **/
    template <typename V>
    friend IEOutStream & operator << (IEOutStream & stream, const TEArrayList< V > & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the array list is empty and has no elements.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief	Returns the current size of the array.
     **/
    inline uint32_t getSize( void ) const;

    /**
     * \brief   Returns true if the specified index is valid.
     **/
    inline bool isValidIndex(const uint32_t index) const;

    /**
     * \brief	Checks whether given element exist in the array or not. The elements of type
     *          VALUE should have comparing operators.
     * \param	elemSearch	The element to search.
     * \param	startAt	    The index to start searching.
     * \return	Returns true if could find element starting at given position.
     **/
    inline bool contains( const VALUE & elemSearch, uint32_t startAt = 0 ) const;

    /**
     * \brief   Returns the vector object where the data are stored.
     **/
    inline const std::vector<VALUE>& getData(void) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Remove all entries of the array.
     **/
    inline void clear(void);

    /**
     * \brief   Delete extra unused entries of the array.
     **/
    inline void freeExtra( void );

    /**
     * \brief   Sets the size of the array to zero and deletes all capacity space.
     **/
    inline void release(void);

    /**
     * \brief   Returns element value by valid zero-based index, which can be used by right operation (r-value). 
     *          The index should be valid. 
     **/
    inline const VALUE & getAt( uint32_t index ) const;

    /**
     * \brief   Returns element value by valid zero-based index, which can be used by left (l-value) and 
     *          right operation (r-value). The index should be valid.
     **/
    inline VALUE & getAt( uint32_t index );

    /**
     * \brief   Sets new element at given valid index. The index should be valid.
     **/
    inline void setAt( uint32_t index, const VALUE & newElement );
    inline void setAt( uint32_t index, VALUE && newElement );

    /**
     * \brief   Returns element value by valid zero-based index.
     * \param   atPosition  Zero-based valid position in array.
     **/
    inline const VALUE& valueAtPosition(const uint32_t atPosition) const;
    inline VALUE& valueAtPosition(uint32_t atPosition);

    /**
     * \brief   Returns array of values, which cannot be modified.
     **/
    inline const VALUE * getValues( void ) const;

    /**
     * \brief   Adds new element at the end of the array.
     * \param   newElement  New element to add at the end of the array.
     **/
    inline void add( const VALUE & newElement );

    /**
     * \brief   The function searches given parameter in the list starting from beginning, 
     *          if does not find any entry, it adds given parameter at the end and returns true.
     *          If parameter 'updateExisting' is true, it updates the existing entry.
     *          The VALUE type should have valid comparing operator.
     * \param   newElement      New element to add at the end of array.
     * \param   updateExisting  If true, updates the existing element.
     *                          If, for example, 2 objects are compared by the name and not by
     *                          absolute values, setting this parameter true updates the existing entry.
     * \return  Returns true, if new element added. Otherwise, returns false.
     **/
    inline bool addIfUnique(const VALUE & newElement, bool updateExisting = false );

    /**
     * \brief	Appends entries taken from the given source at the end of the array.
     * \param	src	    The source of new entries.
     **/
    inline TEArrayList< VALUE >& append( const TEArrayList< VALUE > & src );
    inline TEArrayList< VALUE >& append( const std::vector< VALUE > & src );
    inline TEArrayList< VALUE >& append( TEArrayList< VALUE > && src) noexcept;
    inline TEArrayList< VALUE >& append( std::vector< VALUE > && src) noexcept;

    /**
     * \brief	Copies all entries from given source. If array previously had values,
     *          they will be removed and new values from source array will be set
     *          in the same sequence as they present in the source.
     * \param	src	    The source of array elements.
     **/
    inline void copy( const TEArrayList< VALUE > & src );
    inline void copy( const std::vector< VALUE > & src );

    /**
     * \brief	Moves all entries from given source. On output, the source of array is empty.
     * \param	src	    The source of array elements
     **/
    inline void move( TEArrayList< VALUE > && src ) noexcept;
    inline void move( std::vector< VALUE > && src ) noexcept;

    /**
     * \brief   If position is valid, it shifts elements and inserts new element at specified position.
     *          Otherwise, it appends element at the of the array.
     * \param   startAt     The index of position to insert element.
     * \param   newElement  Value of new element to insert.
     * \param   elemCount   If not one, it will repeat operation.
     **/
    inline void insertAt( uint32_t startAt, const VALUE & newElement, uint32_t elemCount = 1 );

    /**
     * \brief   If position is valid, it shifts elements and inserts new elements from the
     *          given source at specified position. Otherwise, it appends elements at the of the array.
     * \param	startAt	    Starting index position to insert entries.
     * \param	newArray	Sources of array elements to insert.
     * \param   count       Number of elements in the array.
     **/
    inline void insertAt(uint32_t startAt, const VALUE * newArray, uint32_t count );

    /**
     * \brief   If position is valid, it shifts elements and inserts new elements from the
     *          given source at specified position. Otherwise, it appends elements at the of the array.
     * \param	startAt	    Starting index position to insert entries.
     * \param	newArray	Sources of array elements.
     **/
    inline void insertAt( uint32_t startAt, const TEArrayList< VALUE > & newArray );
    inline void insertAt( uint32_t startAt, const std::vector< VALUE > & newArray );

    /**
     * \brief	Removes elements starting at given valid index position.
     * \param	index	    The index to start removing elements
     * \param	elemCount	Amount of elements to remove.
     **/
    inline void removeAt(uint32_t index, uint32_t elemCount = 1);

    /**
     * \brief   Remove the element at specified index and returns the removed element.
     * \param   index   The index of element to remove.
     * \return  The actual removed element.
     **/
    inline VALUE removePosition( uint32_t index );

    /**
     * \brief	Search element starting at given position and removes by first match.
     *          The element of type VALUE should have comparing operator.
     * \param	elemRemove	The element to search and remove.
     * \param	searchAt	The position to start searching.
     * \return	Returns true if found and removed the element.
     **/
    inline bool removeElem(const VALUE & elemRemove, uint32_t searchAt = 0);

    /**
     * \brief	Search element entry in the array and returns the index.
     *          If element is not found, returns -1. The elements of type VALUE
     *          should have comparing operators.
     * \param	elemSearch	The element to search.
     * \param	startAt	    The index to start searching.
     * \return	If found, returns valid index of element in array. Otherwise, returns -1.
     **/
    inline int find( const VALUE & elemSearch, uint32_t startAt = 0 ) const;

    /**
     * \brief	Sets new size of array. If needed, either increases or truncates
     *          elements in the array. The elements of type VALUE should have default
     *          constructor create and initialize elements.
     * \param	newSize	    New size to set. If zero, array is emptied.
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
     * \brief   Returns the capacity of the array
     **/
    inline uint32_t getCapacity(void) const;

    /**
     * \brief	Shifts array elements starting at given valid index position. Reserves .
     * \param	startAt	    The starting index position to start shifting
     * \param	count       The space to reserve. If the value is positive, the elements are stretched out, 
     *                      so that there are 'count' elements can be inserted. If the value is negative,
     *                      the elements are narrowed, so that there are 'count' elements are removed.
     **/
    void shift( uint32_t startAt, int count);

    /**
     * \brief   Return the fist entry in the array. The array must not be empty.
     *          Otherwise, it fails with the assertion.
     **/
    inline const VALUE & firstEntry( void ) const;
    inline VALUE & firstEntry( void );

    /**
     * \brief   Return the last entry in the array. The array must not be empty.
     *          Otherwise, it fails with the assertion.
     **/
    inline const VALUE & lastEntry( void ) const;
    inline VALUE & lastEntry( void );

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
    inline ARRAYPOS getPosition( uint32_t index ) const;

//////////////////////////////////////////////////////////////////////////
//Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ARRAYPOS _citer2pos(typename std::vector<VALUE>::const_iterator cit) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
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
TEArrayList< VALUE >::TEArrayList( uint32_t capacity /*= NECommon::ARRAY_DEFAULT_CAPACITY*/, uint32_t resize /*= 0*/ )
    : Constless<std::vector<VALUE>>( )
    , mValueList( )
{
    capacity = MACRO_MAX( resize, capacity );
    if (capacity != 0)
    {
        mValueList.reserve(capacity > NECommon::MAX_CONTAINER_SIZE ? NECommon::MAX_CONTAINER_SIZE : capacity);
    }

    if ( resize > 0 )
    {
        mValueList.resize( resize );
    }
}

template<typename VALUE>
inline TEArrayList<VALUE>::TEArrayList( const TEArrayList<VALUE> & src )
    : Constless<std::vector<VALUE>>( )
    , mValueList( src.mValueList )
{
}

template<typename VALUE>
inline TEArrayList<VALUE>::TEArrayList( const std::vector<VALUE> & src )
    : Constless<std::vector<VALUE>>( )
    , mValueList( src )
{
}

template<typename VALUE>
inline TEArrayList<VALUE>::TEArrayList(const VALUE* list, uint32_t count)
    : Constless<std::vector<VALUE>>( )
    , mValueList( list != nullptr ? count : 0)
{
    if (NECommon::ARRAY_DEFAULT_CAPACITY > static_cast<uint32_t>(mValueList.capacity()))
    {
        mValueList.reserve(NECommon::ARRAY_DEFAULT_CAPACITY);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        mValueList.at(i) = list[i];
    }
}

template<typename VALUE>
inline TEArrayList<VALUE>::TEArrayList( TEArrayList<VALUE> && src ) noexcept
    : Constless<std::vector<VALUE>>( )
    , mValueList( std::move(src.mValueList) )
{
}

template<typename VALUE>
inline TEArrayList<VALUE>::TEArrayList( std::vector<VALUE> && src ) noexcept
    : Constless<std::vector<VALUE>>( )
    , mValueList( std::move( src ) )
{
}

template<typename VALUE>
inline TEArrayList<VALUE>::~TEArrayList( void )
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
	mValueList.clear();
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
inline void TEArrayList< VALUE >::setAt(uint32_t index, VALUE && newElement)
{
    if (isValidIndex(index))
    {
        mValueList[index] = std::move(newElement);
    }
    else if (NECommon::MAX_CONTAINER_SIZE > static_cast<uint32_t>(mValueList.size()))
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
inline const VALUE & TEArrayList< VALUE >::valueAtPosition( const uint32_t atPosition ) const
{
    ASSERT( isValidIndex( atPosition ) );
    return mValueList.at( atPosition );
}

template<typename VALUE >
inline VALUE& TEArrayList< VALUE >::valueAtPosition( uint32_t atPosition )
{
    ASSERT( isValidIndex( atPosition ) );
    return mValueList.at( atPosition );
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
inline uint32_t TEArrayList< VALUE >::getCapacity(void) const
{
    return static_cast<uint32_t>(mValueList.capacity());
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
inline bool TEArrayList< VALUE >::addIfUnique(const VALUE & newElement, bool updateExisting /*= false*/ )
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
inline TEArrayList< VALUE >& TEArrayList< VALUE >::append(const TEArrayList< VALUE >& src)
{
    return append(src.mValueList);
}

template<typename VALUE >
inline TEArrayList< VALUE >& TEArrayList< VALUE >::append(const std::vector< VALUE >& src)
{
    ASSERT(&mValueList != &src);

    uint32_t size = static_cast<uint32_t>(mValueList.size());
    uint32_t remain = static_cast<uint32_t>(src.size());
    if ((size + remain) > NECommon::MAX_CONTAINER_SIZE)
    {
        remain = NECommon::MAX_CONTAINER_SIZE - static_cast<uint32_t>(mValueList.size());
    }

    mValueList.reserve(size + remain);
    for (uint32_t i = 0; i < remain; ++i)
    {
        mValueList.push_back(src[i]);
    }

    return (*this);
}

template<typename VALUE >
inline TEArrayList< VALUE >& TEArrayList< VALUE >::append(TEArrayList< VALUE >&& src) noexcept
{
    return append(std::move(src.mValueList));
}

template<typename VALUE >
inline TEArrayList< VALUE >& TEArrayList< VALUE >::append(std::vector< VALUE > && src) noexcept
{
    ASSERT(&mValueList != &src);

    uint32_t size = static_cast<uint32_t>(mValueList.size());
    uint32_t remain = static_cast<uint32_t>(src.size());
    if ((size + remain) > NECommon::MAX_CONTAINER_SIZE)
    {
        remain = NECommon::MAX_CONTAINER_SIZE - static_cast<uint32_t>(mValueList.size());
    }

    mValueList.reserve(size + remain);
    for (uint32_t i = 0; i < remain; ++i)
    {
        mValueList.push_back(std::move(src[i]));
    }

    return (*this);
}

template<typename VALUE >
inline void TEArrayList< VALUE >::copy(const TEArrayList< VALUE >& src)
{
    copy(src.getData());
}

template<typename VALUE >
inline void TEArrayList< VALUE >::copy(const std::vector< VALUE >& src)
{
    ASSERT(&mValueList != &src);
    mValueList = src;
}

template<typename VALUE >
inline void TEArrayList< VALUE >::move( TEArrayList< VALUE > && src ) noexcept
{
    move(std::move(src.mValueList));
}

template<typename VALUE >
inline void TEArrayList< VALUE >::move(std::vector< VALUE > && src) noexcept
{
    ASSERT(&mValueList != &src);
    mValueList = std::move(src);
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
    return this->operator = (src.mValueList);
}

template<typename VALUE >
inline TEArrayList< VALUE > & TEArrayList< VALUE >::operator = ( const std::vector< VALUE >& src )
{
    mValueList = src;
    return (*this);
}

template<typename VALUE >
inline TEArrayList< VALUE > & TEArrayList< VALUE >::operator = ( TEArrayList< VALUE > && src ) noexcept
{
    return this->operator = (std::move(src.mValueList));
}

template<typename VALUE >
inline TEArrayList< VALUE > & TEArrayList< VALUE >::operator = ( std::vector< VALUE > && src ) noexcept
{
    mValueList = std::move(src);
    return (*this);
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::operator == ( const TEArrayList< VALUE >& other ) const
{
    return mValueList == other.mValueList;
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::operator == ( const std::vector< VALUE >& other ) const
{
    return mValueList == other;
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::operator != ( const TEArrayList< VALUE >& other ) const
{
    return this->operator != (other.mValueList);
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::operator != ( const std::vector< VALUE >& other ) const
{
    return mValueList != other;
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
        uint32_t remain = static_cast<uint32_t>(mValueList.size()) - index;
        elemCount = MACRO_MIN(elemCount, remain);
        ASSERT(isValidIndex(index));
        ASSERT(isValidIndex(index + elemCount - 1));

        ARRAYPOS first = getPosition(index);
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
inline VALUE TEArrayList< VALUE >::removePosition(uint32_t index)
{
    ASSERT(isValidIndex(index));
    ARRAYPOS first = getPosition(index);
    VALUE result = *first;
    mValueList.erase(first);
    return result;
}

template<typename VALUE >
inline void TEArrayList< VALUE >::insertAt(uint32_t startAt, const VALUE& newElement, uint32_t elemCount /*= 1*/)
{
    if (elemCount != 0)
    {
        uint32_t size = getSize();
        if ((size + elemCount) > NECommon::MAX_CONTAINER_SIZE)
        {
            elemCount = NECommon::MAX_CONTAINER_SIZE - size;
        }

        ARRAYPOS cit = getPosition(startAt);
        if (cit != mValueList.end())
        {
            mValueList.insert(cit, elemCount, newElement);
        }
        else
        {
            mValueList.resize(size + elemCount, newElement);
        }
    }
}

template<typename VALUE >
inline void TEArrayList< VALUE >::insertAt(uint32_t startAt, const VALUE* newArray, uint32_t count)
{
    if ((newArray != nullptr) && (count != 0))
    {
        if ((getSize() + count) > NECommon::MAX_CONTAINER_SIZE)
        {
            count = NECommon::MAX_CONTAINER_SIZE - getSize();
        }

        if (mValueList.size() == startAt)
        {
            resize(static_cast<uint32_t>(mValueList.size() + count));
        }
        else
        {
            ASSERT(isValidIndex(startAt));
            shift(startAt, static_cast<int>(count));
        }

        for (uint32_t i = 0; i < count; ++i)
        {
            mValueList[startAt ++] = newArray[i];
        }
    }
}

template<typename VALUE >
inline void TEArrayList< VALUE >::insertAt(uint32_t startAt, const TEArrayList< VALUE >& newArray)
{
    insertAt(startAt, newArray.mValueList);
}

template<typename VALUE >
inline void TEArrayList< VALUE >::insertAt(uint32_t startAt, const std::vector< VALUE >& newArray)
{
    ASSERT(isValidIndex(startAt));

    if (newArray.empty() == false)
    {
        int32_t limit = 0;
        if ((getSize() + newArray.size()) > NECommon::MAX_CONTAINER_SIZE)
        {
            limit = static_cast<int>(NECommon::MAX_CONTAINER_SIZE - (getSize() + static_cast<uint32_t>(newArray.size())));
        }

        ARRAYPOS cit = getPosition(startAt);
        mValueList.insert(cit, newArray.begin(), newArray.end() - limit);
    }
}

template<typename VALUE >
inline int TEArrayList< VALUE >::find( const VALUE & elemSearch, uint32_t startAt /*= 0*/ ) const
{
    int result = NECommon::INVALID_INDEX;
    if (startAt < static_cast<uint32_t>(mValueList.size()))
    {
        result = static_cast<int>(startAt) - 1;
        auto it = std::find_if( mValueList.begin() + static_cast<int32_t>(startAt), mValueList.end()
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
    return (startAt < static_cast<uint32_t>(mValueList.size()) ? std::find(mValueList.begin() + static_cast<int32_t>(startAt), mValueList.end(), elemSearch) != mValueList.end() : false);
}

template<typename VALUE>
inline const std::vector<VALUE>& TEArrayList<VALUE>::getData(void) const
{
    return mValueList;
}

template<typename VALUE >
inline bool TEArrayList< VALUE >::removeElem( const VALUE & elemRemove, uint32_t searchAt /*= 0*/ )
{
    bool result = false;
    if (searchAt < static_cast<uint32_t>(mValueList.size()))
    {
        auto it = std::find(mValueList.begin() + static_cast<int>(searchAt), mValueList.end(), elemRemove);
        if (it != mValueList.end())
        {
            mValueList.erase(it);
            result = true;
        }
    }

    return result;
}

template<typename VALUE >
void TEArrayList< VALUE >::shift(uint32_t startAt, int  count)
{
    if ((mValueList.size() != 0) && (startAt < mValueList.size()) && (count != 0))
    {
        if (count > 0)
        {
            if (static_cast<uint32_t>(static_cast<int>(getSize()) + count) > NECommon::MAX_CONTAINER_SIZE)
            {
                count = static_cast<int>(NECommon::MAX_CONTAINER_SIZE - getSize());
            }

            VALUE* values = mValueList.data();
            uint32_t size = static_cast<uint32_t>(mValueList.size());
            mValueList.resize(static_cast<uint32_t>(static_cast<int>(size) + count));
            NEMemory::moveElems<VALUE>(values + startAt + count, values + startAt, size - startAt);
        }
        else if (startAt != 0)
        {
            VALUE* values = mValueList.data();
            uint32_t size = static_cast<uint32_t>(mValueList.size());

            count *= -1;
            if (startAt < static_cast<uint32_t>(count))
            {
                count = static_cast<int>(startAt);
            }

            NEMemory::moveElems<VALUE>(values + startAt - count, values + startAt, size - startAt);
            mValueList.resize(static_cast<uint32_t>(static_cast<int>(size) - count));
        }
    }
}

template<typename VALUE>
inline const VALUE & TEArrayList<VALUE>::firstEntry( void ) const
{
    ASSERT( mValueList.size( ) != 0 );
    return mValueList[ 0 ];
}

template<typename VALUE>
inline VALUE & TEArrayList<VALUE>::firstEntry( void )
{
    ASSERT( mValueList.size( ) != 0 );
    return mValueList[ 0 ];
}

template<typename VALUE>
inline const VALUE & TEArrayList<VALUE>::lastEntry( void ) const
{
    ASSERT( mValueList.size( ) != 0 );
    return mValueList[ mValueList.size( ) - 1 ];
}

template<typename VALUE>
inline VALUE & TEArrayList<VALUE>::lastEntry( void )
{
    ASSERT( mValueList.size( ) != 0 );
    return mValueList[ mValueList.size( ) - 1 ];
}

template<typename VALUE >
inline void TEArrayList< VALUE >::setSize(uint32_t elemCount)
{
    mValueList.resize(elemCount);
}

template<typename VALUE >
inline typename TEArrayList< VALUE >::ARRAYPOS TEArrayList< VALUE >::getPosition(uint32_t index) const
{
    return _citer2pos(index < static_cast<uint32_t>(mValueList.size()) ? mValueList.begin() + index : mValueList.end());
}

template<typename VALUE >
inline typename TEArrayList< VALUE >::ARRAYPOS TEArrayList< VALUE >::_citer2pos(typename std::vector<VALUE>::const_iterator cit) const
{
    return Constless<std::vector<VALUE>>::iter(mValueList, cit);
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template<typename V>
const IEInStream & operator >> ( const IEInStream & stream, TEArrayList< V > & input )
{
    uint32_t size = 0;
    stream >> size;
    input.setSize( size );
 
    for (auto & elem : input.mValueList)
    {
        stream >> elem;
    }

    return stream;
}

template<typename V>
IEOutStream & operator << ( IEOutStream& stream, const TEArrayList< V >& output )
{
    stream << output.getSize();
    for (const auto & elem : output.mValueList)
    {
        stream << elem;
    }

    return stream;
}

#endif  // AREG_BASE_TEARRAYLIST_HPP
