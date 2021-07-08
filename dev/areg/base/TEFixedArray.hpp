#ifndef AREG_BASE_TEFIXEDARRAY_HPP
#define AREG_BASE_TEFIXEDARRAY_HPP
/************************************************************************
 * \file        areg/base/TEFixedArray.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
#include "areg/base/ETemplateBase.hpp"

#include "areg/base/NEMemory.hpp"
#include "areg/base/IEIOStream.hpp"

//////////////////////////////////////////////////////////////////////////
// TEFixedArray<VALUE, VALUE_TYPE, Implement> class template declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief	    Fixed Array has general functionalities to access and copy
 *              elements by valid index. Fixed Array is similar to 
 *              TEArrayList<VALUE, VALUE_TYPE> except that it does not 
 *              support insert or remove element(s), it will not 
 *              change initial size until Array object is not assigned 
 *              from other source. In this case old value will be discarded 
 *              and new values will be set. The type VALUE should have
 *              at least default constructor and applicable assigning operator.
 *              By default VALUE_TYPE is equal to type VALUE.
 *              The index to access array element must be valid.
 *              The FixedArray object is not thread safe and data access 
 *              should be  synchronized manually.
 *              For use example, see example bellow.
 *
 * \tparam  VALUE       The type of stored items. Either should be 
 *                      primitive or should have default constructor 
 *                      and valid assigning operator. Also, should be 
 *                      possible to convert to type VALUE_TYPE.
 * \tparam  VALUE_TYPE  By default same as VALUE, but can be any other
 *                      type, which is converted from type VALUE.
 *
 * \tparam  Implement   The implementation of value equality function used by fixed array.
 *
 * \example     TEFixedArray use
 *
 *              Suppose that there is need to have a matrix with various
 *              length of columns. For example:
 *              1   2   3
 *              1   2
 *              1

 *              In this case, this can be defined in following way:
 *              typedef TEFixedArray<int, int>                 FixedArray;
 *              typedef TEFixedArray<FixedArray*, FixedArray*> FixedMatrix;
 *              FixedMatrix matrix(3);
 *              matrix[0] = new FixedArray(3);
 *              matrix[1] = new FixedArray(2);
 *              matrix[2] = new FixedArray(1);
 *              matrix[0][0] = 1; matrix[0][1] = 2; matrix[0][3] = 3;
 *              matrix[1][0] = 1; matrix[1][1] = 2;
 *              matrix[2][0] = 1;
 *              
 *              This might be useful in continues single buffer chunk 
 *              such matrix should be initialized. In particular, 
 *              it is used in class NEService::ProxyData for setting
 *              parameter states in response calls.
 *
 **/
template<typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEListImpl<VALUE_TYPE>>
class TEFixedArray  : protected Implement
                    , protected TemplateConstants
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor. Can be used as a default constructor.
     *          Creates Fixed Array with initial size. If the initial
     *          is zero, no element can be accessed. To change the size,
     *          use assigning operator to copy elements from source array.
     * \param	elemCount	The initial size of array.
     **/
    TEFixedArray( int elemCount = 0);
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TEFixedArray( const TEFixedArray<VALUE, VALUE_TYPE, Implement> & src );
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
     * \brief	Assigning operator. Assigns elements from source.
     *          The previous buffer will be freed and the size will be changed.
     * \param	src	    Source of Array to copy data.
     **/
    const TEFixedArray<VALUE, VALUE_TYPE, Implement> & operator = ( const TEFixedArray<VALUE, VALUE_TYPE, Implement> & src);
    /**
     * \brief   Checks equality of 2 hash-map objects, and returns true if they are equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The fixed array object to compare
     **/
    bool operator == (const TEFixedArray<VALUE, VALUE_TYPE, Implement> & other) const;
    /**
     * \brief   Checks inequality of 2 hash-map objects, and returns true if they are not equal.
     *          There should be possible to compare VALUE type entries of array.
     * \param   other   The fixed array object to compare
     **/
    bool operator != (const TEFixedArray<VALUE, VALUE_TYPE, Implement> & other) const;

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid index.
     *          The index should be valid  number between 0 and (mElemCount -1).
     *          May be used on the right (r-value) or the left (l-value) of an assignment statement.
     **/
    inline VALUE & operator [] ( int index );

    /**
     * \brief   Subscript operator. Returns reference to value of element by given valid index.
     *          The index should be valid number between 0 and (mElemCount -1).
     *          May be used on either the right (r-value).
     **/
    inline VALUE_TYPE operator [] ( int index ) const;

    /**
     * \brief   Returns pointer to the array value. The values cannot be modified.
     **/
    inline operator const VALUE * ( void ) const;

/************************************************************************/
// Friend global operators to make Fixed Array streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream Fixed Array values.
     *          If Fixed Array previously had values, they will be lost 
     *          and Fixed Array will be re-initialized again.
     *          The values in the Fixed Array will be initialized in the same sequence
     *          as they were written.
     *          There should be possibility to initialize values from streaming object and
     *          if VALUE is not a primitive, but an object, it should have
     *          implemented streaming operator.
     * \param   stream  The streaming object for reading values
     * \param   input   The Fixed Array object to save initialized values.
     **/
    template<typename V, typename VT, class Impl>
    friend const IEInStream & operator >> ( const IEInStream & stream, TEFixedArray<V, VT, Impl> & input );
    /**
     * \brief   Writes to the stream Fixed Array values.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream every value of Fixed Array and if VALUE 
     *          is not a primitive, but an object, it should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Fixed Array object to read out values.
     **/
    template<typename V, typename VT, class Impl>
    friend IEOutStream & operator << ( IEOutStream & stream, const TEFixedArray<V, VT, Impl> & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if array is empty
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns the size of array.
     **/
    inline int getSize( void ) const;

    /**
     * \brief   Returns element value by valid index. 
     *          If index is not valid, assertion is raised.
     **/
    inline VALUE_TYPE getAt( int index ) const;

    /**
     * \brief   Returns instance of element at given valid index.
     *          If index is not valid, assertion is raised.
     **/
    inline VALUE & getAt( int index );

    /**
     * \brief   Sets element new value at the given valid index
     *          The index value must be number between 0 and(mElemCount - 1)
     * \param   index       Valid index, between 0 and (mElemCount - 1)
     * \param   newValue    New value to set at given index
     **/
    inline void setAt( int index, VALUE_TYPE newValue );

    /**
     * \brief   Returns they pointer to array of values.
     **/
    inline VALUE * getValues( void ) const;

    /**
     * \brief   Returns true if the given index is valid.
     **/
    inline bool isValidIndex( int whichIndex ) const;

    /**
     * \brief   Resize the array, set new length and copy existing data.
     * \param   newLength   The new length of array to set.
     **/
    void resize( int newLength );

    /**
     * \brief   Clears all elements of array
     **/
    inline void removeAll( void );

//////////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Called when comparing 2 values of element.
     *          Overwrite method when need to change comparison.
     * \param   value1  Value on left side to compare.
     * \param   value2  Value on right side to compare.
     * \return  If function returns true, 2 values are equal.
     *          Otherwise, they are not equal.
     **/
    inline bool isEqualValues( VALUE_TYPE value1, VALUE_TYPE value2) const;

//////////////////////////////////////////////////////////////////////////
// Protected member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Pointer to array of elements
     **/
    VALUE * mValueList;
    /**
     * \brief   The size of array
     **/
    int     mElemCount;

};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEFixedArray<VALUE, VALUE_TYPE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
TEFixedArray<VALUE, VALUE_TYPE, Implement>::TEFixedArray( int elemCount /*= 0*/ )
    : Implement         ( )
    , TemplateConstants ( )

    , mValueList( elemCount > 0 ? DEBUG_NEW VALUE[static_cast<unsigned int>(elemCount)] : NULL )
    , mElemCount( mValueList != NULL ? elemCount : 0 )
{
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
TEFixedArray<VALUE, VALUE_TYPE, Implement>::TEFixedArray( const TEFixedArray<VALUE, VALUE_TYPE, Implement>& src )
    : Implement         ( )
    , TemplateConstants ( )

    , mValueList( src.mElemCount > 0 ? DEBUG_NEW VALUE[static_cast<unsigned int>(src.mElemCount)] : NULL )
    , mElemCount( mValueList != NULL ? src.mElemCount : 0 )
{
    NEMemory::copyElems<VALUE, VALUE>(mValueList, src.mValueList, mElemCount);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
TEFixedArray<VALUE, VALUE_TYPE, Implement>::~TEFixedArray( void )
{
    removeAll();
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
const TEFixedArray<VALUE, VALUE_TYPE, Implement> & TEFixedArray<VALUE, VALUE_TYPE, Implement>::operator = ( const TEFixedArray<VALUE, VALUE_TYPE, Implement>& src )
{
    if (static_cast<const TEFixedArray<VALUE, VALUE_TYPE, Implement> *>(this) != &src)
    {
        if (mElemCount != src.getSize())
        {
            removeAll();
            mValueList    = src.getSize() > 0 ? DEBUG_NEW VALUE[static_cast<unsigned int>(src.getSize())] : NULL;
            mElemCount    = mValueList != NULL ? src.getSize() : 0;
        }

        for (int i = 0; i < mElemCount; ++ i)
            mValueList[i] = src.getAt(i);
    }

    return (*this);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
bool TEFixedArray<VALUE, VALUE_TYPE, Implement>::operator == ( const TEFixedArray<VALUE, VALUE_TYPE, Implement>& other ) const
{
    bool result = true;

    if (this != &other)
    {
        result = false;
        if (mElemCount == other.getSize())
        {
            result = true;
            for (int i = 0; result && (i < mElemCount); ++ i)
                result = isEqualValues(mValueList[i], other.mValueList[i]);
        }
    }

    return result;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
bool TEFixedArray<VALUE, VALUE_TYPE, Implement>::operator != (const TEFixedArray<VALUE, VALUE_TYPE, Implement>& other) const
{
    bool result = false;
    
    if ( this != &other )
    {
        result = true;
        if ( mElemCount == other.getSize( ) )
        {
            result = false;
            for ( int i = 0; (result == false) && (i < mElemCount); ++ i )
                result = isEqualValues( mValueList[i], other.mValueList[i] ) == false;
        }
    }
    
    return result;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline VALUE& TEFixedArray<VALUE, VALUE_TYPE, Implement>::operator [] ( int index )
{
    return getAt(index);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline VALUE_TYPE TEFixedArray<VALUE, VALUE_TYPE, Implement>::operator [] ( int index ) const
{
    return getAt(index);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline TEFixedArray<VALUE, VALUE_TYPE, Implement>::operator const VALUE * ( void ) const
{
    return mValueList;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline bool TEFixedArray<VALUE, VALUE_TYPE, Implement>::isEmpty( void ) const
{
    return (mElemCount == 0);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline int TEFixedArray<VALUE, VALUE_TYPE, Implement>::getSize( void ) const
{
    return mElemCount;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline VALUE_TYPE TEFixedArray<VALUE, VALUE_TYPE, Implement>::getAt( int index ) const
{
    ASSERT(isValidIndex(index));
    return static_cast<VALUE_TYPE>(mValueList[index]);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline VALUE & TEFixedArray<VALUE, VALUE_TYPE, Implement>::getAt( int index )
{
    ASSERT(isValidIndex(index));
    return mValueList[index];
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline void TEFixedArray<VALUE, VALUE_TYPE, Implement>::setAt( int index, VALUE_TYPE newValue )
{
    ASSERT(isValidIndex(index));
    mValueList[index] = newValue;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline VALUE* TEFixedArray<VALUE, VALUE_TYPE, Implement>::getValues( void ) const
{
    return  mValueList;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline bool TEFixedArray<VALUE, VALUE_TYPE, Implement>::isValidIndex( int whichIndex ) const
{
    return ((whichIndex >= 0) && (whichIndex < mElemCount));
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline bool TEFixedArray<VALUE, VALUE_TYPE, Implement>::isEqualValues(VALUE_TYPE value1, VALUE_TYPE value2) const
{
    return Implement::implEqualValues(value1, value2);
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEListImpl<VALUE_TYPE> */>
inline void TEFixedArray<VALUE, VALUE_TYPE, Implement>::removeAll( void )
{
    if (mValueList != NULL)
        delete [] mValueList;

    mValueList  = NULL;
    mElemCount  = 0;
}

template<typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /*= TEListImpl<VALUE_TYPE>*/>
void TEFixedArray<VALUE, VALUE_TYPE, Implement>::resize(int newLength)
{
    VALUE * newList = newLength != 0 ? DEBUG_NEW VALUE[static_cast<unsigned int>(newLength)] : NULL;
    int count = MACRO_MIN(newLength, mElemCount);
    for (int i = 0; i < count; ++ i)
        newList[i] = mValueList[i];
    
    if (mValueList != NULL)
        delete [] mValueList;

    mValueList  = newList;
    mElemCount  = newLength;
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template<typename V, typename VT /*= V*/, class Impl /* = TEListImpl<VT> */>
const IEInStream & operator >> ( const IEInStream & stream, TEFixedArray<V, VT, Impl> & input )
{
    int size = 0;
    stream >> size;
    input.removeAll();
    input.mValueList= size != 0 ? DEBUG_NEW V[size] : NULL;
    input.mElemCount     = input.mValueList != NULL ? size : 0;

    for ( int i = 0; i < input.mElemCount; ++ i )
    {
        stream >> input.mValueList[i];
    }

    return stream;
}

template<typename V, typename VT /*= V*/, class Impl /* = TEListImpl<VT> */>
IEOutStream & operator << ( IEOutStream & stream, const TEFixedArray<V, VT, Impl> & output )
{
    stream << output.mElemCount;
    for ( int i = 0; i < output.mElemCount; ++ i )
    {
        stream << output.mValueList[i];
    }

    return stream;
}

#endif  // AREG_BASE_TEFIXEDARRAY_HPP
