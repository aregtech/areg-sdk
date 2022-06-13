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
 * \file        areg/base/TEProperty.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, FIFO Stack base class templates.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/**
 * \brief   Class template of properties. It contains a pair of data, where
 *          the member 'first' is used as a Key and the member 'second' is
 *          used as a Value. The Key is used in property list and
 *          should be unique.
 * \tparam  KEY     The type of property Key. Should be unique.
 * \tparam  VALUE   The type of property Value.
 **/
template <typename KEY, typename VALUE>
class TEProperty
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    TEProperty( void ) = default;

    /**
     * \brief   Initializes key and value pairs.
     * \param   Key     The initial property key to set.
     * \param   Value   The initial property value to set.
     **/
    TEProperty( const KEY & Key, const VALUE & Value );

    /**
     * \brief   Initializes key and value pairs.
     * \param   Key     The initial property key to set.
     * \param   Value   The initial property value to set.
     **/
    TEProperty(KEY && Key, VALUE && Value);

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TEProperty( const TEProperty<KEY, VALUE> & src ) = default;

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    TEProperty( TEProperty<KEY, VALUE> && src )  noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEProperty( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assignment operator. Copies data from given source.
     **/
    inline TEProperty<KEY, VALUE> & operator = (const TEProperty<KEY, VALUE> & src);

    /**
     * \brief   Move operator. Moves data from given source.
     **/
    inline TEProperty<KEY, VALUE> & operator = ( TEProperty<KEY, VALUE> && src ) noexcept;

    /**
     * \brief	Checks equality of 2 property objects, and returns true if they are equal.
     * \param	other	The property object to compare.
     **/
    inline bool operator == ( const TEProperty<KEY, VALUE> & other ) const;

    /**
     * \brief	Checks inequality of 2 property objects, and returns true if they are not equal.
     * \param	other	The property object to compare.
     **/
    inline bool operator != ( const TEProperty<KEY, VALUE> & other ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Copies the key and value data of the pair from the given source.
     * \param   Key     The key data to set in the pair.
     * \param   Value   The value data to set in the pair.
     **/
    inline void setData(const KEY & Key, const VALUE & Value);

    /**
     * \brief   Moves the key and value data of the pair from the given source.
     * \param   Key     The key data to set in the pair.
     * \param   Value   The value data to set in the pair.
     **/
    inline void setData(KEY && Key, VALUE && Value);

    /**
     * \brief   Sets the key data of the pair from the given source.
     * \param   Key     The key data to set in the pair.
     **/
    inline void setKey(const KEY & Key);
    inline void setKey(KEY && Key);

    /**
     * \brief   Returns the Key data of the pair object.
     **/
    inline const KEY & getKey( void ) const;
    inline KEY & getKey( void );

    /**
     * \brief   Sets the Value data of the pair from the given source.
     * \param   Value   The key data to set in the pair.
     **/
    inline void setValue(const VALUE & Value);
    inline void setValue(VALUE && Value);

    /**
     * \brief   Returns the Value data of the pair object.
     **/
    inline const VALUE& getValue(void) const;
    inline VALUE& getValue(void);

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
    inline bool isEqualKeys( const KEY & key1, const KEY & key2 ) const;

    /**
     * \brief   Called when comparing 2 values of element.
     *          Overwrite method when need to change comparison.
     * \param   value1  Value on left side to compare.
     * \param   value2  Value on right side to compare.
     * \return  If function returns true, 2 values are equal.
     *          Otherwise, they are not equal.
     **/
    inline bool isEqualValues( const VALUE & value1, const VALUE & value2) const;

//////////////////////////////////////////////////////////////////////////
// Member variable
//////////////////////////////////////////////////////////////////////////
public:
    //! The pair object. Declared public for the fast access because
    //! this object does not contain any special logic, except comparing
    //! 2 object, where only keys are compared and values are ignored.
    std::pair<KEY, VALUE>   mValue;
};

//////////////////////////////////////////////////////////////////////////
// Function Implementation
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Reads out from the stream Key and Value pair.
 *          There should be possibility to initialize Key and Value elements from streaming object and
 *          if KEY and VALUE are not primitives, they should have implemented streaming operator.
 * \param   stream  The streaming object to read values
 * \param   input   The Pair object to save values.
 **/
template <typename KEY, typename VALUE>
const IEInStream& operator >> (const IEInStream& stream, std::pair<KEY, VALUE>& input)
{
    stream >> input.first;
    stream >> input.second;
    return stream;
}

/**
 * \brief   Writes to the stream Key and Value of Pair.
 *          If KEY and VALUE are not primitives, they should have implemented streaming operator.
 * \param   stream  The streaming object to write values
 * \param   output  The Pair object, which contains key and value pair.
 **/
template <typename KEY, typename VALUE>
IEOutStream& operator << (IEOutStream& stream, const std::pair<KEY, VALUE>& output)
{
    stream << output.first;
    stream << output.second;
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// TEProperty<KEY, VALUE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename KEY, typename VALUE>
TEProperty<KEY, VALUE>::TEProperty( const KEY & Key, const VALUE & Value )
    : mValue( Key, Value )
{
}

template <typename KEY, typename VALUE>
TEProperty<KEY, VALUE>::TEProperty(KEY && Key, VALUE && Value)
    : mValue( std::move(Key), std::move(Value) )
{
}

template <typename KEY, typename VALUE>
inline TEProperty<KEY, VALUE>& TEProperty<KEY, VALUE>::operator = (const TEProperty<KEY, VALUE>& src)
{
    mValue = src.mValue;
    return (*this);
}

template <typename KEY, typename VALUE>
inline TEProperty<KEY, VALUE>& TEProperty<KEY, VALUE>::operator = (TEProperty<KEY, VALUE> && src) noexcept
{
    mValue = std::move(src.mValue);
    return (*this);
}

template <typename KEY, typename VALUE>
inline bool TEProperty<KEY, VALUE>::operator == ( const TEProperty<KEY, VALUE> & other ) const
{
    return ((this == &other) || isEqualKeys(mValue.first, other.mValue.first));
}

template <typename KEY, typename VALUE>
inline bool TEProperty<KEY, VALUE>::operator != ( const TEProperty<KEY, VALUE> & other ) const
{
    return ((this != &other) && !isEqualKeys(mValue.first, other.mValue.first));
}

template <typename KEY, typename VALUE>
inline void TEProperty<KEY, VALUE>::setData(const KEY& Key, const VALUE& Value)
{
    mValue.first    = Key;
    mValue.second   = Value;
}

template <typename KEY, typename VALUE>
inline void TEProperty<KEY, VALUE>::setData(KEY && Key, VALUE && Value)
{
    mValue.first    = std::move(Key);
    mValue.second   = std::move(Value);
}

template <typename KEY, typename VALUE>
inline void TEProperty<KEY, VALUE>::setKey(const KEY& Key)
{
    mValue.first = Key;
}

template <typename KEY, typename VALUE>
inline void TEProperty<KEY, VALUE>::setKey(KEY && Key)
{
    mValue.first = std::move(Key);
}

template <typename KEY, typename VALUE>
inline const KEY& TEProperty<KEY, VALUE>::getKey(void) const
{
    return mValue.first;
}

template <typename KEY, typename VALUE>
inline KEY& TEProperty<KEY, VALUE>::getKey(void)
{
    return mValue.first;
}

template <typename KEY, typename VALUE>
inline void TEProperty<KEY, VALUE>::setValue(const VALUE& Value)
{
    mValue.second = Value;
}

template <typename KEY, typename VALUE>
inline void TEProperty<KEY, VALUE>::setValue(VALUE && Value)
{
    mValue.second = std::move(Value);
}

template <typename KEY, typename VALUE>
inline const VALUE& TEProperty<KEY, VALUE>::getValue(void) const
{
    return mValue.second;
}

template <typename KEY, typename VALUE>
inline VALUE& TEProperty<KEY, VALUE>::getValue(void)
{
    return mValue.second;
}

template <typename KEY, typename VALUE>
inline bool TEProperty<KEY, VALUE>::isEqualKeys(const KEY& key1, const KEY& key2) const
{
    return (key1 == key2);
}

template <typename KEY, typename VALUE>
inline bool TEProperty<KEY, VALUE>::isEqualValues(const VALUE& value1, const VALUE& value2) const
{
    return (value1 == value2);
}
