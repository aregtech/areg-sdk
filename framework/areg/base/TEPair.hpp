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
 * \file        areg/base/TEPair.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Pair class template.
 *              Pair is binding 2 parameter, which can be of
 *              different types.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TETemplateBase.hpp"
#include "areg/base/IEIOStream.hpp"

//////////////////////////////////////////////////////////////////////////
// TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Pair is a class contains 2 parameters, which are binded.
 *              One value symbolically is called Key, another -- Value.
 *              The typical use of Pair is in hash-map, where a values
 *              are stored and accessed by keys.
 *
 * \tparam  KEY         The type of Key to identify values as pair.
 *                      Can be primitive or if an object, should have at least
 *                      default constructor, copy constructor and assignment operator.
 *                      Should be possible to convert from type KEY to KEY_TYPE.
 * \tparam  VALUE       The type of value as stored items. Can be  primitive or an object,
 *                      which has at least default constructor, copy constructor and 
 *                      assignment operator. Should be possible to convert to type VALUE_TYPE
 * \tparam  KEY_TYPE    The type of Key. By default is same as KEY.
 *                      If different, should be possible to convert 
 *                      and assign to KEY type.
 * \tparam  VALUE_TYPE  By default same as VALUE, but can be any other
 *                      type, which is converted and assign to VALUE type.
 * \tparam  Implement   The name of class that contains comparing functions, which are called
 *                      to compare keys or values.
 **/
template <typename KEY, typename VALUE, typename KEY_TYPE = KEY, typename VALUE_TYPE = VALUE, class Implement = TEPairImpl<KEY_TYPE, VALUE_TYPE>>
class TEPair
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    TEPair( void );

    /**
     * \brief	Initialization constructor. Initializes Key and Value parameters
     *          of Pair object passed in constructor.
     * \param	key	    Key parameter to initialize
     * \param	value	Value parameter to initialize
     **/
    TEPair( KEY_TYPE Key, VALUE_TYPE Value );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TEPair(const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src);

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    TEPair( TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TEPair( void );

//////////////////////////////////////////////////////////////////////////
// Operators. Can be overwritten for certain pairs
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief	Assigning operator. Assigns values from source, copy Key and Value.
     * \param	src	    Source object to get information
     * \return	Returns reference to result Pair object.
     **/
    inline TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & operator = (const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src);

    /**
     * \brief	Assigning operator. Assigns values from source, copy Key and Value.
     * \param	src	    Source object to get information
     * \return	Returns reference to result Pair object.
     **/
    inline TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & operator = ( TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src ) noexcept;

    /**
     * \brief	Checks equality of 2 pair objects, and returns true if they are equal.
     * \param	other	The pair object to compare.
     **/
    inline bool operator == ( const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const;

    /**
     * \brief	Checks inequality of 2 pair objects, and returns true if they are not equal.
     * \param	other	The pair object to compare.
     **/
    inline bool operator != ( const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const;

/************************************************************************/
// Friend global operators to make Pair streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream Key and Value pair.
     *          There should be possibility to initialize Key and Value elements from streaming object and
     *          if KEY and VALUE are not primitives, they should have implemented streaming operator.
     * \param   stream  The streaming object to read values
     * \param   input   The Pair object to save values.
     **/
    template <typename K, typename V, typename KT, typename VT, class Impl>
    friend const IEInStream & operator >> (const IEInStream & stream, TEPair<K, V, KT, VT, Impl> & input);
    /**
     * \brief   Writes to the stream Key and Value of Pair.
     *          If KEY and VALUE are not primitives, they should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   output  The Pair object, which contains key and value pair.
     **/
    template <typename K, typename V, typename KT, typename VT, class Impl>
    friend IEOutStream & operator << (IEOutStream & stream, const TEPair<K, V, KT, VT, Impl> & output);

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Called when comparing 2 keys.
     *          Overwrite method when need to change comparison.
     * \param   key1    Key on left side to compare
     * \param   key2    Key on right side to compare
     * \return  If function returns true, 2 keys are equal.
     *          Otherwise, they are not equal.
     **/
    inline bool isEqualKeys( KEY_TYPE key1, KEY_TYPE key2 ) const;

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
// Member variables
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Key of Pair object
     **/
    KEY     mKey;
    /**
     * \brief   Value of Pair object
     **/
    VALUE   mValue;
    /**
     * \brief   Instance of object that copares keys and values.
     **/
    Implement   mImplement;
};

//////////////////////////////////////////////////////////////////////////
// Function Implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEPair( void )
    : mKey      ( )
    , mValue    ( )
    , mImplement( )
{
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEPair( KEY_TYPE Key, VALUE_TYPE Value )
    : mKey      (Key)
    , mValue    (Value)
    , mImplement( )
{
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEPair( const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& src )
    : mKey      (src.mKey)
    , mValue    (src.mValue)
    , mImplement( )
{
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEPair( TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src ) noexcept
    : mKey      ( src.mKey )
    , mValue    ( src.mValue )
    , mImplement( )
{
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::~TEPair( void )
{
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
inline TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator = ( const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& src )
{
    if (static_cast<const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> *>(this) != &src)
    {
        this->mKey      = src.mKey;
        this->mValue    = src.mValue;
    }

    return (*this);
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
inline TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator = ( TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src ) noexcept
{
    if (static_cast<const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> *>(this) != &src)
    {
        this->mKey      = src.mKey;
        this->mValue    = src.mValue;
    }

    return (*this);
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
inline bool TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator == ( const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& other ) const
{
    return (this != &other ? isEqualKeys(mKey, other.mKey) && isEqualValues(mValue, other.mValue) : true);
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
inline bool TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator!=( const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& other ) const
{
    return (this != &other ? !isEqualKeys(mKey, other.mKey) || !isEqualValues(mValue, other.mValue) : false);
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
inline bool TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::isEqualValues(VALUE_TYPE value1, VALUE_TYPE value2) const
{
    return mImplement.implEqualValues(value1, value2);
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE>*/ >
inline bool TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::isEqualKeys(KEY_TYPE key1, KEY_TYPE key2) const
{
    return mImplement.implEqualKeys(key1, key2);
}

//////////////////////////////////////////////////////////////////////////
// Friend function implementation
//////////////////////////////////////////////////////////////////////////

template <typename K, typename V, typename KT, typename VT, class Impl>
const IEInStream & operator >> ( const IEInStream& stream,  TEPair<K, V, KT, VT, Impl> & input )
{
    stream >> input.mKey;
    stream >> input.mValue;
    return stream;
}

template <typename K, typename V, typename KT, typename VT, class Impl>
IEOutStream & operator << ( IEOutStream& stream, const  TEPair<K, V, KT, VT, Impl> & output )
{
    stream << output.mKey;
    stream << output.mValue;
    return stream;
}
