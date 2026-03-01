#ifndef AREG_BASE_KEYVALUEPAIR_HPP
#define AREG_BASE_KEYVALUEPAIR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/KeyValuePair.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Property object to hold Key-Value pairs.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/IOStream.hpp"
namespace areg {

/**
 * \brief   A template class holding a key-value pair; used in property lists where keys are unique.
 **/
template <typename KEY, typename VALUE>
class KeyValuePair
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief
     * \note    Default constructor.
     **/
    KeyValuePair() = default;
    /**
     * \brief
     * \note    Copy constructor.
     **/
    KeyValuePair(const KeyValuePair<KEY, VALUE>& src) = default;
    /**
     * \brief
     * \note    Move constructor.
     **/
    KeyValuePair(KeyValuePair<KEY, VALUE>&& src)  noexcept = default;

    /**
     * \brief   Initializes the pair with key and value.
     *
     * \param   Key         The key of the pair.
     * \param   Value       The value of the pair.
     **/
    KeyValuePair( const KEY & Key, const VALUE & Value );
    /**
     * \brief   Initializes the pair by moving key and value.
     *
     * \param   Key         The key of the pair to move.
     * \param   Value       The value of the pair to move.
     **/
    KeyValuePair(KEY && Key, VALUE && Value);

    /**
     * \brief   Destructor
     **/
    ~KeyValuePair() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigns key and value from the source pair.
     *
     * \param   src     The source pair to copy from.
     * \return  Reference to this pair.
     **/
    inline KeyValuePair<KEY, VALUE> & operator = (const KeyValuePair<KEY, VALUE> & src);
    /**
     * \brief   Moves key and value from the source pair.
     *
     * \param   src     The source pair to move from.
     * \return  Reference to this pair.
     **/
    inline KeyValuePair<KEY, VALUE> & operator = ( KeyValuePair<KEY, VALUE> && src ) noexcept;

    /**
     * \brief   Returns true if two pairs are equal.
     *
     * \param   other       The pair to compare.
     * \return  Returns true if both pairs are equal; false otherwise.
     **/
    inline bool operator == ( const KeyValuePair<KEY, VALUE> & other ) const;

    /**
     * \brief   Returns true if two pairs are not equal.
     *
     * \param   other       The pair to compare.
     * \return  Returns true if the pairs are not equal; false otherwise.
     **/
    inline bool operator != ( const KeyValuePair<KEY, VALUE> & other ) const;

/************************************************************************/
// Friend global operators to make property streamable
/************************************************************************/

    /**
     * \brief   Reads key and value from a stream.
     *
     * \param   stream      The input stream to read from.
     * \param[out] input       The pair to write the values into.
     * \return  Reference to the input stream.
     **/
    template < typename K, typename V >
    friend inline const InStream & operator >> ( const InStream & stream, KeyValuePair<K, V> & input);

    /**
     * \brief   Writes key and value to a stream.
     *
     * \param   stream      The output stream to write to.
     * \param   output      The pair to read the values from.
     * \return  Reference to the output stream.
     **/
    template < typename K, typename V >
    friend inline OutStream & operator << ( OutStream & stream, const KeyValuePair<K, V> & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets both key and value.
     *
     * \param   Key         The key to set.
     * \param   Value       The value to set.
     **/
    inline void set_data(const KEY & Key, const VALUE & Value);

    /**
     * \brief   Moves key and value into the pair.
     *
     * \param   Key         The key to move.
     * \param   Value       The value to move.
     **/
    inline void set_data(KEY && Key, VALUE && Value);

    /**
     * \brief   Sets the key.
     *
     * \param   Key     The key to set.
     **/
    inline void set_key(const KEY & Key);
    /**
     * \brief   Moves the key into the pair.
     *
     * \param   Key     The key to move.
     **/
    inline void set_key(KEY && Key);

    /**
     * \brief   Returns the key.
     **/
    inline const KEY & key() const;
    /**
     * \brief   Returns the key.
     **/
    inline KEY & key();

    /**
     * \brief   Sets the value.
     *
     * \param   Value       The value to set.
     **/
    inline void set_value(const VALUE & Value);
    /**
     * \brief   Moves the value into the pair.
     *
     * \param   Value       The value to move.
     **/
    inline void set_value(VALUE && Value);

    /**
     * \brief   Returns the value.
     **/
    inline const VALUE& value() const;
    /**
     * \brief   Returns the value.
     **/
    inline VALUE& value();

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
// KeyValuePair<KEY, VALUE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename KEY, typename VALUE>
KeyValuePair<KEY, VALUE>::KeyValuePair( const KEY & Key, const VALUE & Value )
    : mValue( Key, Value )
{
}

template <typename KEY, typename VALUE>
KeyValuePair<KEY, VALUE>::KeyValuePair(KEY && Key, VALUE && Value)
    : mValue( std::move(Key), std::move(Value) )
{
}

template <typename KEY, typename VALUE>
inline KeyValuePair<KEY, VALUE>& KeyValuePair<KEY, VALUE>::operator = (const KeyValuePair<KEY, VALUE>& src)
{
    mValue = src.mValue;
    return (*this);
}

template <typename KEY, typename VALUE>
inline KeyValuePair<KEY, VALUE>& KeyValuePair<KEY, VALUE>::operator = (KeyValuePair<KEY, VALUE> && src) noexcept
{
    mValue = std::move(src.mValue);
    return (*this);
}

template <typename KEY, typename VALUE>
inline bool KeyValuePair<KEY, VALUE>::operator == ( const KeyValuePair<KEY, VALUE> & other ) const
{
    return ((this == &other) || (mValue.first == other.mValue.first));
}

template <typename KEY, typename VALUE>
inline bool KeyValuePair<KEY, VALUE>::operator != ( const KeyValuePair<KEY, VALUE> & other ) const
{
    return ((this != &other) && (mValue.first != other.mValue.first));
}

template <typename KEY, typename VALUE>
inline void KeyValuePair<KEY, VALUE>::set_data(const KEY& Key, const VALUE& Value)
{
    mValue.first    = Key;
    mValue.second   = Value;
}

template <typename KEY, typename VALUE>
inline void KeyValuePair<KEY, VALUE>::set_data(KEY && Key, VALUE && Value)
{
    mValue.first    = std::move(Key);
    mValue.second   = std::move(Value);
}

template <typename KEY, typename VALUE>
inline void KeyValuePair<KEY, VALUE>::set_key(const KEY& Key)
{
    mValue.first = Key;
}

template <typename KEY, typename VALUE>
inline void KeyValuePair<KEY, VALUE>::set_key(KEY && Key)
{
    mValue.first = std::move(Key);
}

template <typename KEY, typename VALUE>
inline const KEY& KeyValuePair<KEY, VALUE>::key() const
{
    return mValue.first;
}

template <typename KEY, typename VALUE>
inline KEY& KeyValuePair<KEY, VALUE>::key()
{
    return mValue.first;
}

template <typename KEY, typename VALUE>
inline void KeyValuePair<KEY, VALUE>::set_value(const VALUE& Value)
{
    mValue.second = Value;
}

template <typename KEY, typename VALUE>
inline void KeyValuePair<KEY, VALUE>::set_value(VALUE && Value)
{
    mValue.second = std::move(Value);
}

template <typename KEY, typename VALUE>
inline const VALUE& KeyValuePair<KEY, VALUE>::value() const
{
    return mValue.second;
}

template <typename KEY, typename VALUE>
inline VALUE& KeyValuePair<KEY, VALUE>::value()
{
    return mValue.second;
}

//////////////////////////////////////////////////////////////////////////
// KeyValuePair<KEY, VALUE> class friend methods
//////////////////////////////////////////////////////////////////////////

template < typename K, typename V >
inline const InStream& operator >> (const InStream& stream, KeyValuePair<K, V>& input)
{
    K key;
    V value;
    stream >> key >> value;
    input.set_data(std::move(key), std::move(value));
    return stream;
}

template < typename K, typename V >
inline OutStream& operator << (OutStream& stream, const KeyValuePair<K, V>& output)
{
    stream << output.mValue.first;
    stream << output.mValue.second;
    return stream;
}

} // namespace areg
#endif  // AREG_BASE_KEYVALUEPAIR_HPP
