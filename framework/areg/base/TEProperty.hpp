#ifndef AREG_BASE_TEPROPERTY_HPP
#define AREG_BASE_TEPROPERTY_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
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
 * \brief   The TEProperty class template is an implementation of a pair of data,
 *          where one member plays role of a Key and other is a Value. It is used
 *          in the Property List where keys are unique entries.
 * \tparam  KEY     The type of property Key.
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
     * \brief   Constructors
     **/
    TEProperty( void ) = default;
    TEProperty(const TEProperty<KEY, VALUE>& src) = default;
    TEProperty(TEProperty<KEY, VALUE>&& src)  noexcept = default;

    /**
     * \brief   Initializes key and value pairs.
     **/
    TEProperty( const KEY & Key, const VALUE & Value );
    TEProperty(KEY && Key, VALUE && Value);

    /**
     * \brief   Destructor
     **/
    ~TEProperty( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assignment operators.
     **/
    inline TEProperty<KEY, VALUE> & operator = (const TEProperty<KEY, VALUE> & src);
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
// Member variable
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The pair object.Declared public for the fast access because
     *          this object does not contain any special logic.
     **/
    std::pair<KEY, VALUE>   mValue;
};

//////////////////////////////////////////////////////////////////////////
// Function Implementation
//////////////////////////////////////////////////////////////////////////

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
    return ((this == &other) || (mValue.first == other.mValue.first));
}

template <typename KEY, typename VALUE>
inline bool TEProperty<KEY, VALUE>::operator != ( const TEProperty<KEY, VALUE> & other ) const
{
    return ((this != &other) && (mValue.first != other.mValue.first));
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

#endif  // AREG_BASE_TEPROPERTY_HPP
