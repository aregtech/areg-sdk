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
 * \file        areg/base/Containers.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Collections of containers.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/TEArrayList.hpp"
#include "areg/base/TEHashMap.hpp"
#include "areg/base/TELinkedList.hpp"
#include "areg/base/TERingStack.hpp"
#include "areg/base/TEStack.hpp"
#include "areg/base/TESortedLinkedList.hpp"

#include "areg/base/String.hpp"
#include "areg/base/NEMath.hpp"

/**
 * \brief       Collection of container such as hash maps, arrays, linked list and sorted list.
 *              For name of existing classes and hierarchy see details bellow.
 **/

//////////////////////////////////////////////////////////////////////////
// Hierarchy of classes.
//////////////////////////////////////////////////////////////////////////

/* class TEArrayList */
    /* StringArray;*/
        class Tokenizer;

/* class TEHashMap */
    template <typename VALUE, class Implement>
    class TEIntegerHashMap;
    template <typename VALUE, class Implement>
    class TEStringHashMap;
    template <typename VALUE, class Implement>
    class TEPointerHashMap;

//////////////////////////////////////////////////////////////////////////
// Declaration and implementation of helper classes for sorted lists and arrays
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Type for sorted list and array of Strings
 **/
class ImplSortedStringList: public TESortImpl<const String &>
{
public:
    /**
     * \brief   Compares 2 values, returns true if they are equal.
     * \param   Value1  The right-side value to compare.
     * \param   Value2  The left-side value to compare.
     * \return  Returns true if 2 values are equal.
     **/
    inline bool implEqualValues( const String & Value1, const String & Value2 ) const
    {
        return (Value1 == Value2);
    }

    /**
     * \brief   Compares 2 values and returns:
     *              - NEMath::Smaller   if Value1 is smaller than Value2;
     *              - NEMath::Equal     if Value1 and Value2 are equal;
     *              - NEMath::Bigger   if Value1 is greater than Value2.
     *          Make own implementation if comparing has special functionality.
     * \param   Value1  The left-side value to compare.
     * \param   Value2  The right-side value to compare.
     * \return  Returns one of following values:
     *              - NEMath::Smaller   if Value1 is smaller than Value2;
     *              - NEMath::Equal     if Value1 and Value2 are equal;
     *              - NEMath::Bigger   if Value1 is greater than Value2.
     **/
    inline NEMath::eCompare implCompareValues( const String & Value1, const String & Value2 ) const
    {
        int len1 = Value1.getLength( );
        int len2 = Value2.getLength( );
        if ( len1 == len2 )
        {
            return static_cast<NEMath::eCompare>(NEString::compareFast<char>( Value1.getString( ), Value2.getString( ), len1 ));
        }
        else if ( len1 > len2 )
        {
            return NEMath::eCompare::Bigger;
        }
        else
        {
            return NEMath::eCompare::Smaller;
        }
    }
};

//////////////////////////////////////////////////////////////////////////
// TEIntegerHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hash Map class template with integer keys.
 * \tparam  VALUE       The type of value to store in map
 * \tparam  VALUE_TYPE  The type when get or set value
 **/
template <typename VALUE, class Implement = TEHashMapImpl<unsigned int, VALUE>>
class TEIntegerHashMap  : public TEHashMap<unsigned int, VALUE, Implement>
{
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map object where the keys are integers, the Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEIntegerHashMap( void ) = default;

    /**
     * \brief	Creates Hash Map object where the keys are integers.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     **/
    TEIntegerHashMap( uint32_t hashSize );

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    TEIntegerHashMap( const TEIntegerHashMap<VALUE, Implement> & src ) = default;

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    TEIntegerHashMap( TEIntegerHashMap<VALUE, Implement> && src ) noexcept = default;

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
 **/
template <typename VALUE, class Implement = TEHashMapImpl<id_type, VALUE>>
class TEIdHashMap: public TEHashMap<id_type, VALUE, Implement>
{
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates Hash Map where keys are Item IDs.
     * \param	hashSize	The size of has map table.
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEIdHashMap(uint32_t hashSize);

    /**
     * \brief   Creates Hash Map object where the keys are Item IDs, the Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEIdHashMap( void ) = default;

    /**
     * \brief   Copies hash map data from given source.
     * \param   src     The source to copy data.
     **/
    TEIdHashMap( const TEIdHashMap<VALUE, Implement> & src ) = default;

    /**
     * \brief   Moves hash map data from given source.
     * \param   src     The source to move data.
     **/
    TEIdHashMap( TEIdHashMap<VALUE, Implement> && src ) noexcept = default;

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
 * \tparam  VALUE       The type of value to store in map
 * \tparam  VALUE_TYPE  The type when get or set value
 **/
template <typename VALUE, class Implement = TEHashMapImpl<const String &, VALUE>>
class TEStringHashMap: public TEHashMap<String, VALUE, Implement>
{
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates Hash Map where keys are strings.
     * \param	hashSize	The size of has map table.
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEStringHashMap(uint32_t hashSize);

    /**
     * \brief   Creates Hash Map where keys are strings, Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEStringHashMap( void ) = default;

    /**
     * \brief   Copies hash-map values from given source.
     * \param   src     The source to copy data.
     **/
    TEStringHashMap( const TEStringHashMap<VALUE, Implement> & src ) = default;

    /**
     * \brief   Moves hash-map values from given source.
     * \param   src     The source to move data.
     **/
    TEStringHashMap( TEStringHashMap<VALUE, Implement> && src ) noexcept = default;

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
 * \tparam  VALUE_TYPE  The type when get or set value
 **/
template <typename VALUE, class Implement = TEPointerHashMapImpl<void *, VALUE>>
class TEPointerHashMap: public TEHashMap<void *, VALUE, Implement>
{
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates Hash Map where keys are pointers.
     * \param	hashSize	The size of has map table.
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEPointerHashMap(uint32_t hashSize);

    /**
     * \brief   Creates Hash Map where keys are pointers Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEPointerHashMap( void ) = default;

    /**
     * \brief   Copies hash map entries from given source.
     * \param   src     The source to copy data.
     **/
    TEPointerHashMap( const TEPointerHashMap<VALUE, Implement> & src ) = default;

    /**
     * \brief   Moves hash map entries from given source.
     * \param   src     The source to move data.
     **/
    TEPointerHashMap( TEPointerHashMap<VALUE, Implement> && src ) noexcept = default;

    /**
     * \brief   Destructor
     **/
    ~TEPointerHashMap( void ) = default;
};

//////////////////////////////////////////////////////////////////////////
// definition of container class aliases
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Type for sorted list and array of integers
 **/
using ImplIntegerSortList       = TESortImpl<unsigned int>;
/**
 * \brief   Type for hash map where keys and values are integers.
 **/
using ImplIntegerToIntegerMap   = TEHashMapImpl<unsigned int, unsigned int>;
/**
 * \brief   Type for hash map where keys are integers and values are strings.
 **/
using ImplIntegerToStringMap    = TEHashMapImpl<unsigned int, const String &>;
/**
 * \brief   Type for Hash Map where keys are integers and values are pointers.
 **/
using ImplIntegerToPointergMap  = TEHashMapImpl<unsigned int, void *>;
/**
 * \brief   Type for Hash Map where keys are strings and values are integers.
 **/
using ImplStringToIntegerMap    = TEHashMapImpl<const String &, unsigned int>;
/**
 * \brief   Type for Hash Map where keys are strings and values are strings.
 **/
using ImplStringToStringMap     = TEHashMapImpl<const String &, const String &>;
/**
 * \brief   Type for Hash Map where keys are strings and values are pointers.
 **/
using ImplStringToPointergMap   = TEHashMapImpl<const String &, void *>;
/**
 * \brief   Type for Hash Map where keys are pointers and values are integers.
 **/
using ImplPointerToIntegerMap   = TEPointerHashMapImpl<void *, unsigned int>;
/**
 * \brief   Hash Map where keys are pointers and values are strings.
 **/
using ImplPointerToStringMap    = TEPointerHashMapImpl<void *, const String &>;
/**
 * \brief   Type for Hash Map where keys are pointers and values are pointers.
 **/
using ImplPointerToPointergMap  = TEPointerHashMapImpl<void *, void *>;
/**
 * \brief   Type for array and list of integers.
 **/
using ImplIntegerList           = TEListImpl<unsigned int>;
/**
 * \brief   Type for list and array of strings.
 **/
using ImplStringList            = TEListImpl<const String &>;
/**
 * \brief   Type for list and array of pointers
 **/
using ImplPointerList           = TEListImpl<void *>;

//////////////////////////////////////////////////////////////////////////
// Declaration of container class aliases
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Array of integer elements
 **/
using IntegerArray  = TEArrayList<unsigned int, ImplIntegerList>;

/**
 * \brief   Array of string elements
 **/
using StringArray   = TEArrayList<String, ImplStringList>;

/**
 * \brief   Array of pointer elements
 **/
using PointerArray  = TEArrayList<void *, ImplPointerList>;

/**
 * \brief   Hash Map where keys are values are integers.
 **/
using IntegerToIntegerMap   = TEIntegerHashMap<unsigned int, ImplIntegerToIntegerMap>;

/**
 * \brief   Hash Map where keys are integers and values are strings
 **/
using IntegerToStringMap    = TEIntegerHashMap<String, ImplIntegerToStringMap>;

/**
 * \brief   Hash Map where keys are integers and values are pointers.
 **/
using IntegerToPointergMap  = TEIntegerHashMap<void *, ImplIntegerToPointergMap>;

/**
 * \brief   Hash Map where keys are strings and values are integers.
 **/
using StringToIntegerMap    = TEStringHashMap<unsigned int, ImplStringToIntegerMap>;

/**
 * \brief   Hash Map where keys are strings and values are strings.
 **/
using StringToStringMap     = TEStringHashMap<String, ImplStringToStringMap>;

/**
 * \brief   Hash Map where keys are strings and values are pointers.
 **/
using StringToPointergMap   = TEStringHashMap<void *, ImplStringToPointergMap>;

/**
 * \brief   Hash Map where keys are pointers and values are integers.
 **/
using PointerToIntegerMap   = TEPointerHashMap<unsigned int, ImplPointerToIntegerMap>;

/**
 * \brief   Hash Map where keys are pointers and values are strings.
 **/
using PointerToStringMap    = TEPointerHashMap<String, ImplPointerToStringMap>;

/**
 * \brief   Hash Map where keys are pointers and values are pointers.
 **/
using PointerToPointergMap  = TEPointerHashMap<void *, ImplPointerToPointergMap>;

/**
 * \brief   Linked List where values are integers.
 **/
using IntegerList   = TELinkedList<unsigned int, ImplIntegerList>;

/**
 * \brief   Linked List where values are strings.
 **/
using StringList    = TELinkedList<String, ImplStringList>;

/**
 * \brief   Linked List where values are pointers
 **/
using PointerList   = TELinkedList<void *, ImplPointerList>;

/**
 * \brief   Sorted List class where values are integers.
 **/
using SortedIntegerList = TESortedLinkedList<unsigned int, unsigned int, ImplIntegerSortList>;

/**
 * \brief   Sorted List class where values are strings.
 **/
using SortedStringList  = TESortedLinkedList<String, const String &, ImplSortedStringList>;

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
     * \param   keepEmpty   if two delimiters next to each other specify an empty token
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
     * \param   keepEmpty   if two delimiters next to each other specify an empty token
     *                      if false, the result will only contain nonempty tokens
     **/
    void tokenize( const String & str, const String & delimiters, bool keepEmpty = true);

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

template <typename VALUE, class Implement>
TEIntegerHashMap<VALUE, Implement>::TEIntegerHashMap(uint32_t hashSize)
    : TEHashMap<unsigned int, VALUE, Implement> (hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// TEIdHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, class Implement>
TEIdHashMap<VALUE, Implement>::TEIdHashMap(uint32_t hashSize)
    : TEHashMap<id_type, VALUE, Implement> (hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// TEStringHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, class Implement>
TEStringHashMap<VALUE, Implement>::TEStringHashMap( uint32_t hashSize )
    : TEHashMap<String, VALUE, Implement>   (hashSize)
{
}

//////////////////////////////////////////////////////////////////////////
// TEPointerHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, class Implement>
TEPointerHashMap<VALUE, Implement>::TEPointerHashMap( uint32_t hashSize )
    : TEHashMap<void *, VALUE, Implement>(hashSize)
{
}
