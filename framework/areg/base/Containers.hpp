#ifndef AREG_BASE_CONTAINERS_HPP
#define AREG_BASE_CONTAINERS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/Containers.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Collections of containers.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/ArrayList.hpp"
#include "areg/base/HashMap.hpp"
#include "areg/base/LinkedList.hpp"
#include "areg/base/OrderedMap.hpp"
#include "areg/base/RingStack.hpp"
#include "areg/base/Stack.hpp"
#include "areg/base/SortedLinkedList.hpp"

#include "areg/base/String.hpp"
#include "areg/base/MathDefs.hpp"

/************************************************************************
 * \brief   Collection of container such as hash maps, arrays, linked 
 *          list and sorted list. For name of existing classes and
 *          hierarchy see details bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Hierarchy of classes.
//////////////////////////////////////////////////////////////////////////

/* class ArrayList */
    /* StringArray;*/
        class Tokenizer;

/* class HashMap */
    template <typename VALUE>
    class IntegerHashMap;
    template <typename VALUE>
    class StringHashMap;
    template <typename VALUE>
    class PtrHashMap;

/* class OrderedMap */
    template <typename VALUE>
    class IntegerMap;
    template <typename VALUE>
    class StringMap;
    template <typename VALUE>
    class PtrMap;

//////////////////////////////////////////////////////////////////////////
// IntegerHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hash Map class template with integer keys.
 * \tparam  VALUE       The type of value to store in map
 **/
template <typename VALUE>
class IntegerHashMap  : public HashMap<uint32_t, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty Hash Map object where the keys are integers and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE
     **/
    IntegerHashMap() = default;

    /**
     * \brief	Creates Hash Map object where the keys are integers.
     * \param	hashSize	The size of blocks in hash map to create at once.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE
     **/
    IntegerHashMap( uint32_t hashSize );

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    IntegerHashMap( const IntegerHashMap<VALUE> & src ) = default;

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    IntegerHashMap( IntegerHashMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~IntegerHashMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// IdHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   This hash map is used to store values associate with ID. This is mainly used to
 *          keep control of resources. So that, there will be no other implementation. Because
 *          resources are mainly pointers and they would need individual solutions.
 * \tparam  VALUE       The type of value to store in map
 **/
template <typename VALUE>
class IdHashMap: public HashMap<id_type, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates Hash Map where keys are Item IDs.
     * \param	hashSize	The size of has map table.
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    IdHashMap(uint32_t hashSize);

    /**
     * \brief   Creates an empty Hash Map object where the keys are Item IDs and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE
     **/
    IdHashMap() = default;

    /**
     * \brief   Copies hash map data from given source.
     * \param   src     The source to copy data.
     **/
    IdHashMap( const IdHashMap<VALUE> & src ) = default;

    /**
     * \brief   Moves hash map data from given source.
     * \param   src     The source to move data.
     **/
    IdHashMap( IdHashMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~IdHashMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// StringHashMap class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map class template where key are strings.
 * \tparam  VALUE       The type of value to store in the map.
 **/
template <typename VALUE>
class StringHashMap: public HashMap<areg::String, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates Hash Map where keys are strings.
     * \param	hashSize	The size of has map table.
     *                      It cannot be more than MAP_MAX_TABLE_SIZE.
     **/
    StringHashMap(uint32_t hashSize);

    /**
     * \brief   Creates an empty Hash Map where keys are strings and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE.
     **/
    StringHashMap() = default;

    /**
     * \brief   Copies hash-map values from given source.
     * \param   src     The source to copy data.
     **/
    StringHashMap( const StringHashMap<VALUE> & src ) = default;

    /**
     * \brief   Moves hash-map values from given source.
     * \param   src     The source to move data.
     **/
    StringHashMap( StringHashMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~StringHashMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// PtrHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hash Map class template where keys are pointers.
 * \tparam  VALUE       The type of value to store in map
 **/
template <typename VALUE>
class PtrHashMap: public HashMap<void *, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates Hash Map where keys are pointers.
     * \param	hashSize	The size of has map table.
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    PtrHashMap(uint32_t hashSize);

    /**
     * \brief   Creates Hash Map where keys are pointers.
     **/
    PtrHashMap() = default;

    /**
     * \brief   Copies hash map entries from given source.
     * \param   src     The source to copy data.
     **/
    PtrHashMap( const PtrHashMap<VALUE> & src ) = default;

    /**
     * \brief   Moves hash map entries from given source.
     * \param   src     The source to move data.
     **/
    PtrHashMap( PtrHashMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~PtrHashMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// IntegerHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Sorted Map class template with integer keys.
 * \tparam  VALUE       The type of value to store in map
 **/
template <typename VALUE>
class IntegerMap : public areg::OrderedMap<uint32_t, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty.
     **/
    IntegerMap() = default;

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    IntegerMap(const IntegerMap<VALUE>& src) = default;

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    IntegerMap(IntegerMap<VALUE>&& src) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~IntegerMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// IdMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   This sorted map is used to store values associate with ID. This is mainly used to
 *          keep control of resources. So that, there will be no other implementation. Because
 *          resources are mainly pointers and they would need individual solutions.
  * \tparam  VALUE       The type of value to store in map
**/
template <typename VALUE>
class IdMap : public areg::OrderedMap<id_type, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty sorted map where the keys are Item IDs.
     **/
    IdMap() = default;

    /**
     * \brief   Copies hash map data from given source.
     * \param   src     The source to copy data.
     **/
    IdMap(const IdMap<VALUE>& src) = default;

    /**
     * \brief   Moves hash map data from given source.
     * \param   src     The source to move data.
     **/
    IdMap(IdMap<VALUE>&& src) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~IdMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// StringMap class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Sorted Map class template where key are strings.
 * \tparam  VALUE       The type of value to store in the map.
 **/
template <typename VALUE>
class StringMap : public areg::OrderedMap<areg::String, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty sorted map where keys are strings.
     **/
    StringMap() = default;

    /**
     * \brief   Copies map values from given source.
     * \param   src     The source to copy data.
     **/
    StringMap(const StringMap<VALUE>& src) = default;

    /**
     * \brief   Moves map values from given source.
     * \param   src     The source to move data.
     **/
    StringMap(StringMap<VALUE>&& src) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~StringMap() = default;
};

/**
 * \brief   Sorted Map class template where keys are pointers.
 * \tparam  VALUE       The type of value to store in map.
 **/
template <typename VALUE>
class PtrMap: public areg::OrderedMap<void *, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates an empty Sorted Map where keys are pointers.
     **/
    PtrMap() = default;

    /**
     * \brief   Copies map entries from given source.
     * \param   src     The source to copy data.
     **/
    PtrMap( const PtrMap<VALUE> & src ) = default;

    /**
     * \brief   Moves map entries from given source.
     * \param   src     The source to move data.
     **/
    PtrMap( PtrMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~PtrMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// definition of container class aliases
//////////////////////////////////////////////////////////////////////////

/**
  * \brief   Array of integer elements
 **/
using IntegerArray  = ArrayList<uint32_t>;

/**
 * \brief   Array of string elements
 **/
using StringArray   = ArrayList<areg::String>;

/**
 * \brief   Array of pointer elements
 **/
using PointerArray  = ArrayList<void *>;

/**
 * \brief   Hash Map where keys are values are integers.
 **/
using IntegerToIntegerHashMap   = IntegerHashMap<uint32_t>;

/**
 * \brief   Hash Map where keys are integers and values are strings
 **/
using IntegerToStringHashMap    = IntegerHashMap<areg::String>;

/**
 * \brief   Hash Map where keys are integers and values are pointers.
 **/
using IntegerToPointergHashMap  = IntegerHashMap<void *>;

/**
 * \brief   Hash Map where keys are strings and values are integers.
 **/
using StringToIntegerHashMap    = StringHashMap<uint32_t>;

/**
 * \brief   Hash Map where keys are strings and values are strings.
 **/
using StringToStringHashMap     = StringHashMap<areg::String>;

/**
 * \brief   Hash Map where keys are strings and values are pointers.
 **/
using StringToPointergHashMap   = StringHashMap<void *>;

/**
 * \brief   Hash Map where keys are pointers and values are integers.
 **/
using PointerToIntegerHashMap   = PtrHashMap<uint32_t>;

/**
 * \brief   Hash Map where keys are pointers and values are strings.
 **/
using PointerToStringHashMap    = PtrHashMap<areg::String>;

/**
 * \brief   Sorted Map where keys are values are integers.
 **/
using IntegerToIntegerMap = IntegerMap<uint32_t>;

/**
 * \brief   Sorted Map where keys are integers and values are strings
 **/
using IntegerToStringMap = IntegerMap<areg::String>;

/**
 * \brief   Sorted Map where keys are integers and values are pointers.
 **/
using IntegerToPointergMap = IntegerMap<void*>;

/**
 * \brief   Sorted Map where keys are strings and values are integers.
 **/
using StringToIntegerMap = StringMap<uint32_t>;

/**
 * \brief   Sorted Map where keys are strings and values are strings.
 **/
using StringToStringMap = StringMap<areg::String>;

/**
 * \brief   Sorted Map where keys are strings and values are pointers.
 **/
using StringToPointergMap = StringMap<void*>;

/**
 * \brief   Sorted Map where keys are pointers and values are integers.
 **/
using PointerToIntegerMap = PtrMap<uint32_t>;

/**
 * \brief   Sorted Map where keys are pointers and values are strings.
 **/
using PointerToStringMap = PtrMap<areg::String>;

/**
 * \brief   Linked List where values are integers.
 **/
using IntegerList   = LinkedList<uint32_t>;

/**
 * \brief   Linked List where values are strings.
 **/
using StringList    = LinkedList<areg::String>;

/**
 * \brief   Linked List where values are pointers
 **/
using PointerList   = LinkedList<void *>;

/**
* \brief   Sorted List class where values are integers.
 **/
using SortedIntegerList = areg::SortedLinkedList<uint32_t>;

/**
 * \brief   Sorted List class where values are strings.
 **/
using SortedStringList  = areg::SortedLinkedList<areg::String>;

//////////////////////////////////////////////////////////////////////////
// Tokenizer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Class to tokenize a String.
 **/
class AREG_API Tokenizer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty array of string tokens.
     **/
    Tokenizer() = default;
    
    /**
     * \brief   Gets the string and tokenize according delimiters.
     * \param   str         the String to tokenize
     * \param   delimiters  one or more delimiter chars that define token boundaries
     * \param   keepEmpty   if true, two delimiters next to each other specify an empty token
     *                      if false, the result will only contain nonempty tokens
     **/
    Tokenizer( const areg::String & str, const areg::String & delimiters, bool keepEmpty = true);

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    Tokenizer( const Tokenizer & src );

    /**
     * \brief   Moves entries from given source.
     * \param   src     The source to move data.
     **/
    Tokenizer( Tokenizer && src ) noexcept;

    /**
     * \brief   Destructor.
     **/
    ~Tokenizer() = default;
//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Tokenize string. After operation, the array is filled by string separated by delimiters.
     * \param   str         the String to tokenize
     * \param   delimiters  one or more delimiter chars that define token boundaries
     * \param   keepEmpty   If true, two delimiters next to each other specify an empty token.
     *                      If false, the result will only contain nonempty tokens.
     * \return  Returns list of string split by matching delimiter
     **/
    const StringArray & tokenize( const areg::String & str, const areg::String & delimiters, bool keepEmpty = true);

    /**
     * \brief   Returns list of string split by matching delimiter.
     **/
    inline const StringArray& getList() const;

private:

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The list of tokenized string.
     */
    StringArray mTokens;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
};

/************************************************************************
 * class template implementation
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// IntegerHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
IntegerHashMap<VALUE>::IntegerHashMap(uint32_t hashSize)
    : HashMap<uint32_t, VALUE> (hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// IdHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
IdHashMap<VALUE>::IdHashMap(uint32_t hashSize)
    : HashMap<id_type, VALUE> (hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// StringHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
StringHashMap<VALUE>::StringHashMap( uint32_t hashSize )
    : HashMap<areg::String, VALUE>   (hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// PtrHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
PtrHashMap<VALUE>::PtrHashMap( uint32_t hashSize )
    : HashMap<void *, VALUE>(hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// Tokenizer class inline implementation
//////////////////////////////////////////////////////////////////////////

inline const StringArray& Tokenizer::getList() const
{
    return mTokens;
}

#endif  // AREG_BASE_CONTAINERS_HPP
