#ifndef AREG_BASE_TEPROPERTY_HPP
#define AREG_BASE_TEPROPERTY_HPP
/************************************************************************
 * \file        areg/base/TEProperty.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    TEProperty( void );

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
    TEProperty( const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Destructor
     **/
    ~TEProperty( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assignment operator. Copies data from given source.
     */
    const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & operator = (const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src);

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
TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEProperty( void )
    : TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> ( )
{   ;   }

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE> */>
TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEProperty( KEY_TYPE Key, VALUE_TYPE Value )
    : TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> ( Key, Value )
{   ;   }

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE> */>
TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEProperty( const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src )
    : TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> ( static_cast<const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> &>(src) )
{   ;   }

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE> */>
TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::~TEProperty( void )
{   ;   }

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE> */>
const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator = (const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src)
{
    static_cast<TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> &>(*this) = static_cast<const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> &>(src);
    return (*this);
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE> */>
inline bool TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator == ( const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const
{
    return (this == &other ? TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::isEqualKeys(this->mKey, other.mKey) : true);
}

template <typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPairImpl<KEY_TYPE, VALUE_TYPE> */>
inline bool TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator != ( const TEProperty<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const
{
    return (this != &other ? !TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::isEqualKeys(this->mKey, other.mKey) : false);
}

#endif  // AREG_BASE_TEPROPERTY_HPP
