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
#include "areg/base/TEArrayList.hpp"
#include "areg/base/TEHashMap.hpp"
#include "areg/base/TELinkedList.hpp"
#include "areg/base/TEMap.hpp"
#include "areg/base/TERingStack.hpp"
#include "areg/base/TEStack.hpp"
#include "areg/base/TESortedLinkedList.hpp"

#include "areg/base/String.hpp"
#include "areg/base/NEMath.hpp"

/************************************************************************
 * \brief   Collection of container such as hash maps, arrays, linked 
 *          list and sorted list. For name of existing classes and
 *          hierarchy see details bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Hierarchy of classes.
//////////////////////////////////////////////////////////////////////////

/* class TEArrayList */
    /* StringArray;*/
        class Tokenizer;

/* class TEHashMap */
    template <typename VALUE>
    class TEIntegerHashMap;
    template <typename VALUE>
    class TEStringHashMap;
    template <typename VALUE>
    class TEPointerHashMap;

/* class TEMap */
    template <typename VALUE>
    class TEIntegerMap;
    template <typename VALUE>
    class TEStringMap;
    template <typename VALUE>
    class TEPointerMap;

//////////////////////////////////////////////////////////////////////////
// TEIntegerHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hash Map class template with integer keys.
 * \tparam  VALUE       The type of value to store in map
 **/
template <typename VALUE>
class TEIntegerHashMap  : public TEHashMap<unsigned int, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty Hash Map object where the keys are integers and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE
     **/
    TEIntegerHashMap( void ) = default;

    /**
     * \brief	Creates Hash Map object where the keys are integers.
     * \param	hashSize	The size of blocks in hash map to create at once.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE
     **/
    TEIntegerHashMap( uint32_t hashSize );

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    TEIntegerHashMap( const TEIntegerHashMap<VALUE> & src ) = default;

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    TEIntegerHashMap( TEIntegerHashMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEIntegerHashMap( void ) = default;
};

//////////////////////////////////////////////////////////////////////////
// TEIdHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   This hash map is used to store values associate with ID. This is mainly used to
 *          keep control of resources. So that, there will be no other implementation. Because
 *          resources are mainly pointers and they would need individual solutions.
 * \tparam  VALUE       The type of value to store in map
 **/
template <typename VALUE>
class TEIdHashMap: public TEHashMap<id_type, VALUE>
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
    TEIdHashMap(uint32_t hashSize);

    /**
     * \brief   Creates an empty Hash Map object where the keys are Item IDs and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE
     **/
    TEIdHashMap( void ) = default;

    /**
     * \brief   Copies hash map data from given source.
     * \param   src     The source to copy data.
     **/
    TEIdHashMap( const TEIdHashMap<VALUE> & src ) = default;

    /**
     * \brief   Moves hash map data from given source.
     * \param   src     The source to move data.
     **/
    TEIdHashMap( TEIdHashMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEIdHashMap( void ) = default;
};

//////////////////////////////////////////////////////////////////////////
// TEStringHashMap class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map class template where key are strings.
 * \tparam  VALUE       The type of value to store in the map.
 **/
template <typename VALUE>
class TEStringHashMap: public TEHashMap<String, VALUE>
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
    TEStringHashMap(uint32_t hashSize);

    /**
     * \brief   Creates an empty Hash Map where keys are strings and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE.
     **/
    TEStringHashMap( void ) = default;

    /**
     * \brief   Copies hash-map values from given source.
     * \param   src     The source to copy data.
     **/
    TEStringHashMap( const TEStringHashMap<VALUE> & src ) = default;

    /**
     * \brief   Moves hash-map values from given source.
     * \param   src     The source to move data.
     **/
    TEStringHashMap( TEStringHashMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEStringHashMap( void ) = default;
};

//////////////////////////////////////////////////////////////////////////
// TEPointerHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hash Map class template where keys are pointers.
 * \tparam  VALUE       The type of value to store in map
 **/
template <typename VALUE>
class TEPointerHashMap: public TEHashMap<void *, VALUE>
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
    TEPointerHashMap(uint32_t hashSize);

    /**
     * \brief   Creates Hash Map where keys are pointers.
     **/
    TEPointerHashMap( void ) = default;

    /**
     * \brief   Copies hash map entries from given source.
     * \param   src     The source to copy data.
     **/
    TEPointerHashMap( const TEPointerHashMap<VALUE> & src ) = default;

    /**
     * \brief   Moves hash map entries from given source.
     * \param   src     The source to move data.
     **/
    TEPointerHashMap( TEPointerHashMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEPointerHashMap( void ) = default;
};

//////////////////////////////////////////////////////////////////////////
// TEIntegerHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Sorted Map class template with integer keys.
 * \tparam  VALUE       The type of value to store in map
 **/
template <typename VALUE>
class TEIntegerMap : public TEMap<unsigned int, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty.
     **/
    TEIntegerMap(void) = default;

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    TEIntegerMap(const TEIntegerMap<VALUE>& src) = default;

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    TEIntegerMap(TEIntegerMap<VALUE>&& src) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEIntegerMap(void) = default;
};

//////////////////////////////////////////////////////////////////////////
// TEIdMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   This sorted map is used to store values associate with ID. This is mainly used to
 *          keep control of resources. So that, there will be no other implementation. Because
 *          resources are mainly pointers and they would need individual solutions.
  * \tparam  VALUE       The type of value to store in map
**/
template <typename VALUE>
class TEIdMap : public TEMap<id_type, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty sorted map where the keys are Item IDs.
     **/
    TEIdMap(void) = default;

    /**
     * \brief   Copies hash map data from given source.
     * \param   src     The source to copy data.
     **/
    TEIdMap(const TEIdMap<VALUE>& src) = default;

    /**
     * \brief   Moves hash map data from given source.
     * \param   src     The source to move data.
     **/
    TEIdMap(TEIdMap<VALUE>&& src) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEIdMap(void) = default;
};

//////////////////////////////////////////////////////////////////////////
// TEStringMap class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Sorted Map class template where key are strings.
 * \tparam  VALUE       The type of value to store in the map.
 **/
template <typename VALUE>
class TEStringMap : public TEMap<String, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty sorted map where keys are strings.
     **/
    TEStringMap(void) = default;

    /**
     * \brief   Copies map values from given source.
     * \param   src     The source to copy data.
     **/
    TEStringMap(const TEStringMap<VALUE>& src) = default;

    /**
     * \brief   Moves map values from given source.
     * \param   src     The source to move data.
     **/
    TEStringMap(TEStringMap<VALUE>&& src) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEStringMap(void) = default;
};

/**
 * \brief   Sorted Map class template where keys are pointers.
 * \tparam  VALUE       The type of value to store in map.
 **/
template <typename VALUE>
class TEPointerMap: public TEMap<void *, VALUE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates an empty Sorted Map where keys are pointers.
     **/
    TEPointerMap( void ) = default;

    /**
     * \brief   Copies map entries from given source.
     * \param   src     The source to copy data.
     **/
    TEPointerMap( const TEPointerMap<VALUE> & src ) = default;

    /**
     * \brief   Moves map entries from given source.
     * \param   src     The source to move data.
     **/
    TEPointerMap( TEPointerMap<VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEPointerMap( void ) = default;
};

//////////////////////////////////////////////////////////////////////////
// definition of container class aliases
//////////////////////////////////////////////////////////////////////////

/**
  * \brief   Array of integer elements
 **/
using IntegerArray  = TEArrayList<unsigned int>;

/**
 * \brief   Array of string elements
 **/
using StringArray   = TEArrayList<String>;

/**
 * \brief   Array of pointer elements
 **/
using PointerArray  = TEArrayList<void *>;

/**
 * \brief   Hash Map where keys are values are integers.
 **/
using IntegerToIntegerHashMap   = TEIntegerHashMap<unsigned int>;

/**
 * \brief   Hash Map where keys are integers and values are strings
 **/
using IntegerToStringHashMap    = TEIntegerHashMap<String>;

/**
 * \brief   Hash Map where keys are integers and values are pointers.
 **/
using IntegerToPointergHashMap  = TEIntegerHashMap<void *>;

/**
 * \brief   Hash Map where keys are strings and values are integers.
 **/
using StringToIntegerHashMap    = TEStringHashMap<unsigned int>;

/**
 * \brief   Hash Map where keys are strings and values are strings.
 **/
using StringToStringHashMap     = TEStringHashMap<String>;

/**
 * \brief   Hash Map where keys are strings and values are pointers.
 **/
using StringToPointergHashMap   = TEStringHashMap<void *>;

/**
 * \brief   Hash Map where keys are pointers and values are integers.
 **/
using PointerToIntegerHashMap   = TEPointerHashMap<unsigned int>;

/**
 * \brief   Hash Map where keys are pointers and values are strings.
 **/
using PointerToStringHashMap    = TEPointerHashMap<String>;

/**
 * \brief   Sorted Map where keys are values are integers.
 **/
using IntegerToIntegerMap = TEIntegerMap<unsigned int>;

/**
 * \brief   Sorted Map where keys are integers and values are strings
 **/
using IntegerToStringMap = TEIntegerMap<String>;

/**
 * \brief   Sorted Map where keys are integers and values are pointers.
 **/
using IntegerToPointergMap = TEIntegerMap<void*>;

/**
 * \brief   Sorted Map where keys are strings and values are integers.
 **/
using StringToIntegerMap = TEStringMap<unsigned int>;

/**
 * \brief   Sorted Map where keys are strings and values are strings.
 **/
using StringToStringMap = TEStringMap<String>;

/**
 * \brief   Sorted Map where keys are strings and values are pointers.
 **/
using StringToPointergMap = TEStringMap<void*>;

/**
 * \brief   Sorted Map where keys are pointers and values are integers.
 **/
using PointerToIntegerMap = TEPointerMap<unsigned int>;

/**
 * \brief   Sorted Map where keys are pointers and values are strings.
 **/
using PointerToStringMap = TEPointerMap<String>;

/**
 * \brief   Linked List where values are integers.
 **/
using IntegerList   = TELinkedList<unsigned int>;

/**
 * \brief   Linked List where values are strings.
 **/
using StringList    = TELinkedList<String>;

/**
 * \brief   Linked List where values are pointers
 **/
using PointerList   = TELinkedList<void *>;

/**
* \brief   Sorted List class where values are integers.
 **/
using SortedIntegerList = TESortedLinkedList<unsigned int>;

/**
 * \brief   Sorted List class where values are strings.
 **/
using SortedStringList  = TESortedLinkedList<String>;

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
    Tokenizer( void ) = default;
    
    /**
     * \brief   Gets the string and tokenize according delimiters.
     * \param   str         the String to tokenize
     * \param   delimiters  one or more delimiter chars that define token boundaries
     * \param   keepEmpty   if true, two delimiters next to each other specify an empty token
     *                      if false, the result will only contain nonempty tokens
     **/
    Tokenizer( const String & str, const String & delimiters, bool keepEmpty = true);

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
    ~Tokenizer( void ) = default;
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
    const StringArray & tokenize( const String & str, const String & delimiters, bool keepEmpty = true);

    /**
     * \brief   Returns list of string split by matching delimiter.
     **/
    inline const StringArray& getList(void) const;

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
// TEIntegerHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
TEIntegerHashMap<VALUE>::TEIntegerHashMap(uint32_t hashSize)
    : TEHashMap<unsigned int, VALUE> (hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// TEIdHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
TEIdHashMap<VALUE>::TEIdHashMap(uint32_t hashSize)
    : TEHashMap<id_type, VALUE> (hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// TEStringHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
TEStringHashMap<VALUE>::TEStringHashMap( uint32_t hashSize )
    : TEHashMap<String, VALUE>   (hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// TEPointerHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE>
TEPointerHashMap<VALUE>::TEPointerHashMap( uint32_t hashSize )
    : TEHashMap<void *, VALUE>(hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// Tokenizer class inline implementation
//////////////////////////////////////////////////////////////////////////

inline const StringArray& Tokenizer::getList(void) const
{
    return mTokens;
}

#endif  // AREG_BASE_CONTAINERS_HPP
