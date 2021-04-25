#ifndef AREG_BASE_TEARRAYLIST_HPP
#define AREG_BASE_TEARRAYLIST_HPP
/************************************************************************
 * \file        areg/src/base/TEArrayList.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Array List class template
 *              This class template defines array of elements.
 *              It allows to store elements in array, making general
 *              operations and having access by integer index value.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/ETemplateBase.hpp"

#include "areg/src/base/NEMemory.hpp"
#include "areg/src/base/IEIOStream.hpp"

//////////////////////////////////////////////////////////////////////////
// TEArrayList<VALUE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief       Array List class template declaration for fast insert and access 
 *              to elements by given valid index.
 *
 * \details     Array List has general functionalities to access, insert, move, find
 *              and copy elements. By default, the VALUE_TYPE is equal to VALUE.
 *              Derives private class CETemplateConstants to have static members.
 *              For performance issue, it is recommended to pass capacity value
 *              in constructor to define initial reserved space for array.
 *              This capacity value will also define the initial growing size
 *              of array. By default, the minimum growing size of array is
 *              CETemplateConstants::ARRAY_DEFAULT_MIN_GROW and cannot be more
 *              than CETemplateConstants::ARRAY_DEFAULT_MAX_GROW.
 *              The VALUE types should have at least default constructor
 *              and valid public available assigning operator.
 *              The elements of array can be fast accessed  for read and change 
 *              by valid index value. If the index of array element to modify 
 *              is out of valid scope, use Add() function to add new element 
 *              at the end.
 *              The ArryList object is not thread safe and data access should be 
 *              synchronized manually.
 *
 * \tparam  VALUE       the type of stored items. Either should be 
 *                      primitive or should have default constructor 
 *                      and valid assigning operator. Also, should be 
 *                      possible to convert to type VALUE_TYPE.
 * \tparam  VALUE_TYPE  By default same as VALUE, but can be any other
 *                      type, which is converted from type VALUE.
 *
 * \tparam  Implement   The implementation of value equality function used by array list.
 **/

template<typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEListImpl<VALUE_TYPE>>
class TEArrayList   : protected Implement
                    , private   CETemplateConstants
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     * \param   capacity    Initial reserved space of array. By default the initial reserved space is zero.
     * \param   increase    The number to increase elements every time when need to reallocate new space
     *                      By default the increase value is CETemplateConstants::ARRAY_DEFAULT_INCREASE (-1),
     *                      which means that elements will grow by CETemplateConstants::ARRAY_DEFAULT_MIN_GROW (4)
     **/
    TEArrayList( int capacity = 0, int increase = CETemplateConstants::ARRAY_DEFAULT_INCREASE );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TEArrayList( const TEArrayList<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Destructor
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
    inline VALUE_TYPE operator [] ( int index ) const;

    /**
     * \brief   Assigning operator. Copies all values from given source.
     *          If Array previously had values, they will be removed and new values
     *          from source Array will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of list of values.
     **/
    inline TEArrayList<VALUE, VALUE_TYPE, Implement> & operator = ( const TEArrayList<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Checks equality of 2 hash-map objects, and returns true if they are equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The array object to compare
     **/
    bool operator == ( const TEArrayList<VALUE, VALUE_TYPE, Implement> & other ) const;

    /**
     * \brief   Checks inequality of 2 hash-map objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The hash-map object to compare
     **/
    bool operator != ( const TEArrayList<VALUE, VALUE_TYPE, Implement> & other ) const;

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
    template <typename V, typename VT, class Impl>
    friend const IEInStream & operator >> (const IEInStream & stream, TEArrayList<V, VT, Impl> & input);

    /**
     * \brief   Writes to the stream Array values.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream every value of Array and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Array object to read out values.
     **/
    template <typename V, typename VT, class Impl>
    friend IEOutStream & operator << (IEOutStream & stream, const TEArrayList<V, VT, Impl> & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the array list is empty and has no element
     **/
    inline bool IsEmpty( void ) const;

    /**
     * \brief	Returns the current size of array
     **/
    inline int GetSize( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Delete extra entries in array
     **/
    void FreeExtra( void );

    /**
     * \brief   Returns true if the specified index is valid.
     **/
    inline bool IsValidIndex( int index ) const;

    /**
     * \brief   Remove all entries of array
     **/
    inline void RemoveAll( void );
    
    /**
     * \brief   Returns element value by valid index. 
     *          If index is not valid, assertion is raised.
     **/
    inline VALUE_TYPE GetAt( int index ) const;

    /**
     * \brief   Returns instance of element at given valid index.
     *          If index is not valid, assertion is raised.
     **/
    inline VALUE & GetAt( int index );

    /**
     * \brief   Sets new element at given valid index.
     *          If index is not valid, assertion is raised.
     **/
    inline void SetAt( int index, VALUE_TYPE newElement );

    /**
     * \brief   Returns array of elements.
     **/
    inline const VALUE * GetValues( void ) const;

    /**
     * \brief   Adds new element at the end of array and returns index value of new inserted element
     **/
    inline int Add( VALUE_TYPE newElement );

    /**
     * \brief	Appends entries taken from source at the end of array.
     * \param	src	    The source of new entries.
     * \return	Returns new size of array. 
     **/
    inline int Append( const TEArrayList<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief	Copies all entries from given source
     * \param	src	    The source of array elements
     * \return	Returns actual size of array.
     **/
    int Copy( const TEArrayList<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   If needed, shifts existing entries of array and
     *          inserts new element to the position specified by index.
     *          If elemCount is more than one, it will repeat operation starting by 
     *          specified index.
     * \param   index       Index to insert element
     * \param   newElement  Value of new element to insert
     * \param   elemCount   If not one, it will repeat operation
     **/
    inline void InsertAt( int index, VALUE_TYPE newElement, int elemCount = 1 );

    /**
     * \brief	Removes elemCount element starting at given index position.
     *          The index should be valid
     * \param	index	    The index to start removing elements
     * \param	elemCount	Amount of elements to remove.
     **/
    inline void RemoveAt( int index, int elemCount = 1 );

    /**
     * \brief	Inserts new entries from given array source
     * \param	startAt	    Starting index position to insert entries.
     * \param	newArray	Sources of array elements
     **/
    inline void InsertAt( int startAt, const TEArrayList<VALUE, VALUE_TYPE, Implement> & newArray );

    /**
     * \brief	Search element entry in the array and returns index.
     *          If element is not found, returns -1.
     * \param	elemSearch	Element to search, should have comparing operator
     * \param	startAt	    The index to start searching. If negative value,
     *                      search from begging.
     * \return	If found, returns index of element in array. Otherwise return -1.
     **/
    inline int FindElement( VALUE_TYPE elemSearch, int startAt = 0 ) const;

    /**
     * \brief	Checks whether given element exist in array or not.
     *          return true if found. Starts searching at given position.
     * \param	elemSearch	Element to search, should have comparing operator
     * \param	startAt	    The index to start searching. If negative,
     *          search from begging.
     * \return	Returns true if could find element starting at given position.
     **/
    inline bool ElementExist( VALUE_TYPE elemSearch, int startAt = 0 ) const;

    /**
     * \brief	Search element starting from given position and removes by first match
     * \param	elemRemove	Element to search and remove.
     * \param	searchAt	Start searching at given position
     * \return	Returns true if removed element.
     **/
    inline bool RemoveElement( VALUE_TYPE elemRemove, int searchAt = 0 );

    /**
     * \brief   Ensures validity of index. If index is more that element count in array,
     *          it will increase number of elements to (index + 1) that the value 'index'
     *          is valid. Otherwise, it does nothing.
     * \param   index   The index to ensure. Cannot be negative value
     * \return  Returns true if index is validation succeeded.
     **/
    inline bool ValidateIndex( int index );

    /**
     * \brief	Sets new size of array. 
     *          If size is zero, all elements will be removed.
     *          Otherwise the size of array is set and if was not empty, 
     *          existing elements are copied or truncated, depending
     *          whether new size is more or less than the existing size of array.
     * \param	newSize	    New size to set. If zero, array is emptied.
     * \param	increaseBy	The number of entries to increase array when reserving space
     * \return  Returns the number of element created by resizing array. The number of created elements
     *          might be less or equal to maximum size, but cannot be more.
     **/
    inline int ReserveElements( int newSize, int increaseBy = CETemplateConstants::ARRAY_DEFAULT_INCREASE );

//////////////////////////////////////////////////////////////////////////
// Protected operations
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief	Shifts array elements starting at given index position and reserves placeCount entries in new array
     * \param	startAt	    The starting index position to start shifting
     * \param	placeCount	The space to reserve
     **/
    void Shift( int startAt, int placeCount );

    /**
     * \brief	Sets new size of array. 
     *          If size is zero, all elements will be removed.
     *          Otherwise the size of array is set and if was not empty, 
     *          existing elements are copied or truncated, depending
     *          whether new size is more or less than the existing size of array.
     * \param	elemCount   New size to set. If zero, array is emptied.
     * \param	increaseBy	The number of entries to increase array when reserving space
     * \return  Returns the number of element created by resizing array. The number of created elements
     *          might be less or equal to maximum size, but cannot be more.
     **/
    int SetSize( int elemCount, int increaseBy = CETemplateConstants::ARRAY_DEFAULT_INCREASE );

    /**
     * \brief   Called when comparing 2 values of element.
     *          Create public method 'EqualValues' when need to change comparison.
     * \param   value1  Value on left side to compare.
     * \param   value2  Value on right side to compare.
     * \return  If function returns true, 2 values are equal.
     *          Otherwise, they are not equal.
     **/
    inline bool EqualValues( VALUE_TYPE value1, VALUE_TYPE value2) const;

    /**
     * \brief	Constructs element entries in the given list
     * \param	valueList	The list to construct elements
     * \param	elemCount	The number of element entries in list
     **/
    inline void Construct( VALUE * valueList, int elemCount );

    /**
     * \brief	Destructs element entries in the given list
     * \param	valueList	The list to destroy elements
     * \param	elemCount	The number of element entries in list
     **/
    inline void Destruct( VALUE * valueList, int elemCount );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Number of entries to reserve by increasing procedure.
     **/
    int     mIncreaseBy;
    /**
     * \brief   Maximum size of array
     **/
    int     mMaxElems;
    /**
     * \brief   Number of valid entries in array
     **/
    int     mElemCount;
    /**
     * \brief   The list elements / array
     **/
    VALUE * mValueList;
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEArrayList<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
TEArrayList<VALUE, VALUE_TYPE, Implement>::TEArrayList(int capacity /*= 0*/, int increase /*= CETemplateConstants::ARRAY_DEFAULT_INCREASE*/)
    : Implement                             ( )
    , CETemplateConstants                   ( )

    , mIncreaseBy   ( CETemplateConstants::ARRAY_DEFAULT_MIN_GROW )
    , mMaxElems     ( 0 )
    , mElemCount    ( 0 )
    , mValueList    ( NULL )
{
    SetSize(capacity, increase);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
TEArrayList<VALUE, VALUE_TYPE, Implement>::TEArrayList( const TEArrayList<VALUE, VALUE_TYPE, Implement> & src )
    : Implement                         ( )
    , CETemplateConstants               ( )

    , mIncreaseBy   ( CETemplateConstants::ARRAY_DEFAULT_MIN_GROW )
    , mMaxElems     ( 0 )
    , mElemCount    ( 0 )
    , mValueList    ( NULL )
{
    Copy(src);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
TEArrayList<VALUE, VALUE_TYPE, Implement>::~TEArrayList( void )
{
    RemoveAll( );

    mValueList  = NULL;
    mMaxElems   = 0;
    mElemCount  = 0;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline bool TEArrayList<VALUE, VALUE_TYPE, Implement>::IsEmpty( void ) const
{   return (mElemCount == 0);   }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline int TEArrayList<VALUE, VALUE_TYPE, Implement>::GetSize( void ) const
{   return mElemCount;          }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
void TEArrayList<VALUE, VALUE_TYPE, Implement>::FreeExtra( void )
{
    if (mElemCount != mMaxElems)
    {
        VALUE* newValues = NULL;
        if (mElemCount != 0)
        {
            newValues = reinterpret_cast<VALUE *>( new unsigned char[mElemCount * sizeof(VALUE)]);
            NEMemory::CopyElems<VALUE, VALUE>(newValues, mValueList, mElemCount);
        }

        Destruct(mValueList, mElemCount);
        delete [] reinterpret_cast<unsigned char *>(mValueList);
        mValueList= newValues;
        mMaxElems = mElemCount;
    }
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline bool TEArrayList<VALUE, VALUE_TYPE, Implement>::IsValidIndex( int index ) const
{   return ( (index >= 0) && (index < mElemCount) );                                                }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline void TEArrayList<VALUE, VALUE_TYPE, Implement>::RemoveAll( void )
{   SetSize(0, -1);                                                                                 }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline VALUE_TYPE TEArrayList<VALUE, VALUE_TYPE, Implement>::GetAt(int index) const
{   ASSERT(IsValidIndex(index)); return static_cast<VALUE_TYPE>(mValueList[index]); }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline VALUE& TEArrayList<VALUE, VALUE_TYPE, Implement>::GetAt(int index)
{   ASSERT(IsValidIndex(index)); return mValueList[index];                          }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline void TEArrayList<VALUE, VALUE_TYPE, Implement>::SetAt(int index, VALUE_TYPE newElement)
{
    if (IsValidIndex(index))
        mValueList[index] = (VALUE)(newElement);
    else if (index >= mElemCount )
        Add(newElement);
#ifdef _DEBUG
    else
        ASSERT(false);
#endif // _DEBUG
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline bool TEArrayList<VALUE, VALUE_TYPE, Implement>::ValidateIndex(int index)
{
    ASSERT(index >= 0);
    if (index >= mElemCount)
        mElemCount = SetSize(index + 1);
    return (index >= 0 && mElemCount >= index + 1);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline int TEArrayList<VALUE, VALUE_TYPE, Implement>::ReserveElements( int newSize, int increaseBy /*= CETemplateConstants::ARRAY_DEFAULT_INCREASE */ )
{   mElemCount = SetSize(newSize, increaseBy); return mElemCount;   }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline const VALUE* TEArrayList<VALUE, VALUE_TYPE, Implement>::GetValues( void ) const
{   return mValueList;                                              }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline int TEArrayList<VALUE, VALUE_TYPE, Implement>::Add(VALUE_TYPE newElement)
{
    SetSize(mElemCount + 1, -1);
    mValueList[mElemCount] = newElement;
    return mElemCount ++;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline int TEArrayList<VALUE, VALUE_TYPE, Implement>::Append(const TEArrayList<VALUE, VALUE_TYPE, Implement>& src)
{
    ASSERT(this != &src);
    SetSize(mElemCount + src.mElemCount, -1);
    NEMemory::CopyElems<VALUE, VALUE>(mValueList + mElemCount, src.mValueList, src.mElemCount);
    mElemCount += src.mElemCount;
    return mElemCount;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
int TEArrayList<VALUE, VALUE_TYPE, Implement>::Copy(const TEArrayList<VALUE, VALUE_TYPE, Implement>& src)
{
    ASSERT(this != &src);
    if (mValueList != NULL)
    {
        Destruct(mValueList, mElemCount);
        delete [] reinterpret_cast<unsigned char *>(mValueList);
        mValueList= NULL;
        mElemCount= 0;
        mMaxElems = 0;
    }

    SetSize(src.mElemCount, -1);
    NEMemory::CopyElems<VALUE, VALUE>(mValueList, src.mValueList, src.mElemCount);
    mElemCount = src.mElemCount;
    return mElemCount;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline VALUE& TEArrayList<VALUE, VALUE_TYPE, Implement>::operator [] (int index)
{   return GetAt(index);    }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline VALUE_TYPE TEArrayList<VALUE, VALUE_TYPE, Implement>::operator [] (int index) const
{   return GetAt(index);    }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
TEArrayList<VALUE, VALUE_TYPE, Implement>& TEArrayList<VALUE, VALUE_TYPE, Implement>::operator = ( const TEArrayList<VALUE, VALUE_TYPE, Implement>& src )
{
    if (static_cast<const TEArrayList<VALUE, VALUE_TYPE, Implement> *>(this) != &src)
    {
        RemoveAll();
        mIncreaseBy   = src.mIncreaseBy;
        Copy(src);
    }
    return (*this);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
bool TEArrayList<VALUE, VALUE_TYPE, Implement>::operator == ( const TEArrayList<VALUE, VALUE_TYPE, Implement>& other ) const
{
    bool result = true;
    if (this != &other)
    {
        result = false;
        if (mElemCount == other.mElemCount)
        {
            result = true;
            for (int i = 0; result && (i < mElemCount); ++ i)
                result = EqualValues( mValueList[i], other.mValueList[i] );
        }
    }
    return result;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
bool TEArrayList<VALUE, VALUE_TYPE, Implement>::operator != (const TEArrayList<VALUE, VALUE_TYPE, Implement>& other) const
{
    bool result = false;
    if ( this != &other )
    {
        result = true;
        if ( mElemCount == other.mElemCount )
        {
            result = false;
            for ( int i = 0; (result == false) && (i < mElemCount); ++ i )
                result = EqualValues( mValueList[i], other.mValueList[i] ) == false;
        }
    }
    return result;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline TEArrayList<VALUE, VALUE_TYPE, Implement>::operator const VALUE * ( void ) const
{   
    return mValueList;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline void TEArrayList<VALUE, VALUE_TYPE, Implement>::InsertAt(int index, VALUE_TYPE newElement, int elemCount /*= 1*/)
{   Shift(index, elemCount); NEMemory::SetMemory<VALUE, VALUE_TYPE>(mValueList + index, newElement, elemCount);    }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline void TEArrayList<VALUE, VALUE_TYPE, Implement>::RemoveAt(int index, int elemCount /*= 1*/)
{
    ASSERT(index >= 0 && elemCount >= 0 && index + elemCount <= mElemCount);
    int moveElems = mElemCount - (index + elemCount);
    if (moveElems > 0)
    {
        NEMemory::MoveElems<VALUE>(mValueList + index, mValueList + index + elemCount, moveElems);
    }

    Destruct(mValueList + index + moveElems, elemCount);
    mElemCount -= elemCount;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline void TEArrayList<VALUE, VALUE_TYPE, Implement>::InsertAt(int startAt, const TEArrayList<VALUE, VALUE_TYPE, Implement>& newArray)
{
    ASSERT(startAt >= 0);
    if (newArray.mElemCount > 0)
    {
        Shift(startAt, newArray.mElemCount);
        NEMemory::CopyElems<VALUE, VALUE>(mValueList + startAt, newArray.mValueList, newArray.mElemCount);
    }
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline int TEArrayList<VALUE, VALUE_TYPE, Implement>::FindElement( VALUE_TYPE elemSearch, int startAt /*= 0*/ ) const
{
    int result = startAt <= 0 ? 0 : MACRO_MIN(startAt, mElemCount);
    for ( ; result < mElemCount; ++ result)
    {
        if ( EqualValues(mValueList[result], elemSearch) )
            break;
    }
    return (result < mElemCount ? result : -1);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline bool TEArrayList<VALUE, VALUE_TYPE, Implement>::ElementExist( VALUE_TYPE elemSearch, int startAt /*= 0*/ ) const
{   return (FindElement(elemSearch, startAt) >= 0);  }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline bool TEArrayList<VALUE, VALUE_TYPE, Implement>::RemoveElement( VALUE_TYPE elemRemove, int searchAt /*= 0*/ )
{
    int pos = FindElement(elemRemove, searchAt);
    if (pos >= 0)
    {
        RemoveAt(pos, 1);
    }

    return (pos >= 0);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
void TEArrayList<VALUE, VALUE_TYPE, Implement>::Shift(int startAt, int placeCount)
{
    ASSERT(startAt >= 0 && placeCount > 0);
    int oldSize = mElemCount;
    if (startAt >= mElemCount)
    {
        SetSize(startAt + placeCount, -1);
        mElemCount = startAt + placeCount;
    }
    else
    {
        SetSize(mElemCount + placeCount, -1);
        mElemCount += placeCount;
        NEMemory::MoveElems<VALUE>(mValueList + startAt + placeCount, mValueList + startAt, oldSize - startAt);
    }

    ASSERT(startAt + placeCount <= mElemCount);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
int TEArrayList<VALUE, VALUE_TYPE, Implement>::SetSize(int elemCount, int increaseBy /*= CETemplateConstants::ARRAY_DEFAULT_INCREASE*/)
{
    int result = -1;
    mIncreaseBy = increaseBy != CETemplateConstants::ARRAY_DEFAULT_INCREASE && increaseBy > 0 ? MACRO_MAX(CETemplateConstants::ARRAY_DEFAULT_MIN_GROW, increaseBy) : mIncreaseBy;
    mIncreaseBy = MACRO_MIN(CETemplateConstants::ARRAY_DEFAULT_MAX_GROW, mIncreaseBy);

    if (elemCount == 0)
    {
        if (mValueList != NULL)
        {
            Destruct(mValueList, mElemCount);
            delete [] reinterpret_cast<unsigned char *>(mValueList);
        }
        mValueList  = static_cast<VALUE *>(NULL);
        mElemCount = mMaxElems = 0;
        result = 0;
    }
    else if (elemCount <= mMaxElems)
    {
        if (elemCount > mElemCount)
            Construct(mValueList + mElemCount, elemCount - mElemCount);
        else if (elemCount < mElemCount)
            Destruct(mValueList + elemCount, mElemCount - elemCount);
        mElemCount = MACRO_MIN(elemCount, mElemCount);
        result = elemCount;
    }
    else
    {
        ASSERT(mValueList != static_cast<VALUE *>(NULL) || mElemCount == 0);
        int grow = mIncreaseBy == 0 ? mElemCount / 8 : mIncreaseBy;
        grow = MACRO_MAX(grow, CETemplateConstants::ARRAY_DEFAULT_MIN_GROW);
        grow = MACRO_MIN(grow, CETemplateConstants::ARRAY_DEFAULT_MAX_GROW);
        int newMax   = MACRO_MAX(elemCount, mMaxElems + grow);
        ASSERT(newMax >= mMaxElems && elemCount > mElemCount);

        VALUE* newValues = reinterpret_cast<VALUE *>( DEBUG_NEW unsigned char[newMax * sizeof(VALUE)] );
        if (newValues != NULL)
        {
            ASSERT(elemCount <= newMax);
            Construct(newValues, elemCount);
            NEMemory::CopyElems<VALUE, VALUE>(newValues, mValueList, mElemCount);
            if (mValueList != NULL)
            {
                Destruct(mValueList, mElemCount);
                delete [] reinterpret_cast<unsigned char *>(mValueList);
            }
            mValueList	= newValues;
            mMaxElems	= newMax;
            result      = elemCount;
        }
        else
        {
            result      = mElemCount; // do nothing, there was not much space
        }
    }
    return result;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline void TEArrayList<VALUE, VALUE_TYPE, Implement>::Construct(VALUE *valueList, int elemCount)
{   NEMemory::ConstructElems<VALUE>(static_cast<void *>(valueList), elemCount); }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline void TEArrayList<VALUE, VALUE_TYPE, Implement>::Destruct(VALUE *valueList, int elemCount)
{   NEMemory::DestroyElems<VALUE>(valueList, elemCount);                        }

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline bool TEArrayList<VALUE, VALUE_TYPE, Implement>::EqualValues(VALUE_TYPE value1, VALUE_TYPE value2) const
{
    return Implement::ImplEqualValues(value1, value2);
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template <typename V, typename VT, class Impl>
const IEInStream & operator >> ( const IEInStream & stream, TEArrayList<V, VT, Impl> & input )
{
    int size = 0;
    input.SetSize( size );
    stream >> size;

    for ( int i = 0; i < size; ++ i )
    {
        stream >> input.mValueList[i];
    }

    return stream;
}

template <typename V, typename VT, class Impl>
IEOutStream & operator << ( IEOutStream& stream, const TEArrayList<V, VT, Impl>& output )
{
    stream << output.mElemCount;
    for ( int i = 0; i < output.mElemCount; ++ i )
    {
        stream << output.mValueList[i];
    }

    return stream;
}

#endif  // AREG_BASE_TEARRAYLIST_HPP
