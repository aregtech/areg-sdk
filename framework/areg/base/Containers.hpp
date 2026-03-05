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
namespace areg {

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
 * \brief   Hash map template with integer keys.
 **/
template <typename VALUE>
class IntegerHashMap  : public HashMap<uint32_t, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    IntegerHashMap() = default;

    /**
     * \brief   Creates a hash map with the specified table size.
     *
     * \param   hashSize    The initial size of the hash table.
     **/
    IntegerHashMap( uint32_t hashSize );

    /**
     * \brief
     *
     * \param   src     The source to copy.
     **/
    IntegerHashMap( const IntegerHashMap<VALUE> & src ) = default;

    /**
     * \brief
     *
     * \param   src     The source to move.
     **/
    IntegerHashMap( IntegerHashMap<VALUE> && src ) noexcept = default;

    ~IntegerHashMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// IdHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hash map template with uint32_t ID keys for resource tracking and management.
 **/
template <typename VALUE>
class IdHashMap: public HashMap<id_type, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a hash map with the specified table size (max MAP_MAX_TABLE_SIZE = 1024).
     *
     * \param   hashSize    The size of the hash map table.
     **/
    IdHashMap(uint32_t hashSize);

    IdHashMap() = default;

    /**
     * \brief
     *
     * \param   src     The source to copy.
     **/
    IdHashMap( const IdHashMap<VALUE> & src ) = default;

    /**
     * \brief
     *
     * \param   src     The source to move.
     **/
    IdHashMap( IdHashMap<VALUE> && src ) noexcept = default;

    ~IdHashMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// StringHashMap class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash map template with string keys.
 **/
template <typename VALUE>
class StringHashMap: public HashMap<String, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a hash map with the specified table size.
     *
     * \param   hashSize    The size of the hash map table (max MAP_MAX_TABLE_SIZE).
     **/
    StringHashMap(uint32_t hashSize);

    StringHashMap() = default;

    /**
     * \brief
     *
     * \param   src     The source to copy.
     **/
    StringHashMap( const StringHashMap<VALUE> & src ) = default;

    /**
     * \brief
     *
     * \param   src     The source to move.
     **/
    StringHashMap( StringHashMap<VALUE> && src ) noexcept = default;

    ~StringHashMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// PtrHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hash map template with pointer keys.
 **/
template <typename VALUE>
class PtrHashMap: public HashMap<void *, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a hash map with the specified table size (max MAP_MAX_TABLE_SIZE = 1024).
     *
     * \param   hashSize    The size of the hash map table.
     **/
    PtrHashMap(uint32_t hashSize);

    PtrHashMap() = default;

    /**
     * \brief
     *
     * \param   src     The source to copy.
     **/
    PtrHashMap( const PtrHashMap<VALUE> & src ) = default;

    /**
     * \brief
     *
     * \param   src     The source to move.
     **/
    PtrHashMap( PtrHashMap<VALUE> && src ) noexcept = default;

    ~PtrHashMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// IntegerHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Sorted map template with integer keys.
 **/
template <typename VALUE>
class IntegerMap : public OrderedMap<uint32_t, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    IntegerMap() = default;

    /**
     * \brief
     *
     * \param   src     The source map to copy.
     **/
    IntegerMap(const IntegerMap<VALUE>& src) = default;

    /**
     * \brief
     *
     * \param   src     The source map to move.
     * \note    Move constructor.
     **/
    IntegerMap(IntegerMap<VALUE>&& src) noexcept = default;

    ~IntegerMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// IdMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Sorted map template for storing values associated with Item IDs, used for resource
 *          management.
 **/
template <typename VALUE>
class IdMap : public OrderedMap<id_type, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    IdMap() = default;

    /**
     * \brief
     *
     * \param   src     The source map to copy.
     **/
    IdMap(const IdMap<VALUE>& src) = default;

    /**
     * \brief
     *
     * \param   src     The source map to move.
     * \note    Move constructor.
     **/
    IdMap(IdMap<VALUE>&& src) noexcept = default;

    ~IdMap() = default;
};

//////////////////////////////////////////////////////////////////////////
// StringMap class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Sorted map template with string keys.
 **/
template <typename VALUE>
class StringMap : public OrderedMap<String, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    StringMap() = default;

    /**
     * \brief
     *
     * \param   src     The source map to copy.
     **/
    StringMap(const StringMap<VALUE>& src) = default;

    /**
     * \brief
     *
     * \param   src     The source map to move.
     * \note    Move constructor.
     **/
    StringMap(StringMap<VALUE>&& src) noexcept = default;

    ~StringMap() = default;
};

/**
 * \brief   Sorted map template with pointer keys.
 **/
template <typename VALUE>
class PtrMap: public OrderedMap<void *, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    PtrMap() = default;

    /**
     * \brief
     *
     * \param   src     The source map to copy.
     **/
    PtrMap( const PtrMap<VALUE> & src ) = default;

    /**
     * \brief
     *
     * \param   src     The source map to move.
     * \note    Move constructor.
     **/
    PtrMap( PtrMap<VALUE> && src ) noexcept = default;

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
using StringArray   = ArrayList<String>;

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
using IntegerToStringHashMap    = IntegerHashMap<String>;

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
using StringToStringHashMap     = StringHashMap<String>;

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
using PointerToStringHashMap    = PtrHashMap<String>;

/**
 * \brief   Sorted Map where keys are values are integers.
 **/
using IntegerToIntegerMap = IntegerMap<uint32_t>;

/**
 * \brief   Sorted Map where keys are integers and values are strings
 **/
using IntegerToStringMap = IntegerMap<String>;

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
using StringToStringMap = StringMap<String>;

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
using PointerToStringMap = PtrMap<String>;

/**
 * \brief   Linked List where values are integers.
 **/
using IntegerList   = LinkedList<uint32_t>;

/**
 * \brief   Linked List where values are strings.
 **/
using StringList    = LinkedList<String>;

/**
 * \brief   Linked List where values are pointers
 **/
using PointerList   = LinkedList<void *>;

/**
* \brief   Sorted List class where values are integers.
 **/
using SortedIntegerList = SortedLinkedList<uint32_t>;

/**
 * \brief   Sorted List class where values are strings.
 **/
using SortedStringList  = SortedLinkedList<String>;

//////////////////////////////////////////////////////////////////////////
// Tokenizer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Tokenizes a string by splitting it on delimiter characters.
 **/
class AREG_API Tokenizer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    Tokenizer() = default;
    
    /**
     * \brief   Tokenizes the string using the specified delimiters.
     *
     * \param   str             The string to tokenize.
     * \param   delimiters      One or more delimiter characters that define token boundaries.
     * \param   keepEmpty       If true, consecutive delimiters create empty tokens; if false, only
     *                          non-empty tokens are included.
     **/
    Tokenizer( const String & str, const String & delimiters, bool keepEmpty = true);

    Tokenizer( const Tokenizer & src );

    Tokenizer( Tokenizer && src ) noexcept;

    ~Tokenizer() = default;
//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Tokenizes the string and populates the array with the resulting tokens.
     *
     * \param   str             The string to tokenize.
     * \param   delimiters      One or more delimiter characters.
     * \param   keepEmpty       If true, consecutive delimiters create empty tokens; if false, only
     *                          non-empty tokens are included.
     * \return  Returns the array of resulting tokens.
     **/
    const StringArray & tokenize( const String & str, const String & delimiters, bool keepEmpty = true);

    /**
     * \brief   Returns the array of tokens from the most recent tokenization.
     **/
    [[nodiscard]]
    inline const StringArray& list() const noexcept;

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
    : HashMap<String, VALUE>   (hashSize)
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

inline const StringArray& Tokenizer::list() const noexcept
{
    return mTokens;
}

} // namespace areg
#endif  // AREG_BASE_CONTAINERS_HPP
