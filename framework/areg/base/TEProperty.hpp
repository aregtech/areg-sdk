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
#include "areg/base/TEPair.hpp"

/**
 * \brief   Class template of properties. It is an extension of TEPair class.
 *          For more details has a look description of TEPair
 * \tparam  KEY         The type of property Key. Should be unique.
 * \tparam  VALUE       The type of property Value.
 * \tparam  KEY_TYPE    The type of Key. By default is same as KEY.
 * \tparam  VALUE_TYPE  By default same as VALUE.
 * \tparam  Implement   The name of class that implements methods to check equality of Key or Value.
 * \see     TEPair
 **/
template <typename KEY, typename VALUE, typename KEY_TYPE = KEY, typename VALUE_TYPE = VALUE, class Implement = TEPairImpl<KEY_TYPE, VALUE_TYPE>>
class TEProperty   : public TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>
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
     * \brief   Initializes key and value pairs
     * \param   Key     The initial property key to set.
     * \param   Value   The initial property value to set.
     **/
    TEProperty( KEY_TYPE Key, VALUE_TYPE Value );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TEProperty( const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src ) = default;

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    TEProperty( TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src )  noexcept = default;

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
     */
    TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & operator = (const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src) = default;

    /**
     * \brief   Move operator. Moves data from given source.
     */
    TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & operator = ( TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src ) noexcept = default;

    /**
     * \brief	Checks equality of 2 property objects, and returns true if they are equal.
     * \param	other	The property object to compare.
     **/
    inline bool operator == ( const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const;

    /**
     * \brief	Checks inequality of 2 property objects, and returns true if they are not equal.
     * \param	other	The property object to compare.
     **/
    inline bool operator != ( const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const;
};

//////////////////////////////////////////////////////////////////////////
// Function Implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE> */>
TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEProperty( KEY_TYPE Key, VALUE_TYPE Value )
    : TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> ( Key, Value )
{
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE> */>
inline bool TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator == ( const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const
{
    return (this == &other ? TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::isEqualKeys(this->mKey, other.mKey) : true);
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE> */>
inline bool TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator != ( const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const
{
    return (this != &other ? TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::isEqualKeys(this->mKey, other.mKey) == false: false);
}
