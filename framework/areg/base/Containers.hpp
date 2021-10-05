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
    template <typename VALUE, typename VALUE_TYPE, class Implement>
    class TEIntegerHashMap;
    template <typename VALUE, typename VALUE_TYPE, class Implement>
    class TEStringHashMap;
    template <typename VALUE, typename VALUE_TYPE, class Implement>
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
        int len = Value1.getLength( );
        if ( len == Value2.getLength( ) )
        {
            return (NEString::compareFast<char>( Value1.getString( ), Value2.getString( ), len ) == static_cast<int>(NEMath::eCompare::Equal));
        }
        else
        {
            return false;
        }
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
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEHashMapImpl<unsigned int, VALUE_TYPE>>
class TEIntegerHashMap  : public TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement>
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
     * \param	hashSize	The size of has map table.
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEIntegerHashMap( int blockSize, int hashSize );

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    TEIntegerHashMap( const TEIntegerHashMap<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Copies hash-map data from given sources.
     * \param   src     The source to copy data.
     **/
    TEIntegerHashMap( TEIntegerHashMap<VALUE, VALUE_TYPE, Implement> && src ) noexcept;

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
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEHashMapImpl<id_type, VALUE_TYPE>>
class TEIdHashMap: public TEHashMap<id_type, VALUE, id_type, VALUE_TYPE, Implement>
{
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map object where the keys are Item IDs, the Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEIdHashMap( void ) = default;

    /**
     * \brief	Creates Hash Map where keys are Item IDs.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table.
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEIdHashMap( int blockSize, int hashSize );

    /**
     * \brief   Copies hash map data from given source.
     * \param   src     The source to copy data.
     **/
    TEIdHashMap( const TEIdHashMap<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Moves hash map data from given source.
     * \param   src     The source to move data.
     **/
    TEIdHashMap( TEIdHashMap<VALUE, VALUE_TYPE, Implement> && src ) noexcept;

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
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEHashMapImpl<const String &, VALUE_TYPE>>
class TEStringHashMap: public TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement>
{
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are strings, Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEStringHashMap( void ) = default;

    /**
     * \brief	Creates Hash Map where keys are strings.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table.
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEStringHashMap( int blockSize, int hashSize );

    /**
     * \brief   Copies hash-map values from given source.
     * \param   src     The source to copy data.
     **/
    TEStringHashMap( const TEStringHashMap<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Moves hash-map values from given source.
     * \param   src     The source to move data.
     **/
    TEStringHashMap( TEStringHashMap<VALUE, VALUE_TYPE, Implement> && src ) noexcept;

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
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEPointerHashMapImpl<void *, VALUE_TYPE>>
class TEPointerHashMap: public TEHashMap<void *, VALUE, void *, VALUE_TYPE, Implement>
{
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are pointers Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEPointerHashMap( void ) = default;

    /**
     * \brief	Creates Hash Map where keys are pointers.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table.
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEPointerHashMap( int blockSize, int hashSize );

    /**
     * \brief   Copies hash map entries from given source.
     * \param   src     The source to copy data.
     **/
    TEPointerHashMap( const TEPointerHashMap<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Moves hash map entries from given source.
     * \param   src     The source to move data.
     **/
    TEPointerHashMap( TEPointerHashMap<VALUE, VALUE_TYPE, Implement> && src ) noexcept;

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
using IntegerArray  = TEArrayList<unsigned int, unsigned int, ImplIntegerList>;

/**
 * \brief   Array of string elements
 **/
using StringArray   = TEArrayList<String, const String &, ImplStringList>;

/**
 * \brief   Array of pointer elements
 **/
using PointerArray  = TEArrayList<void *, void *, ImplPointerList>;

/**
 * \brief   Hash Map where keys are values are integers.
 **/
using IntegerToIntegerMap   = TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap>;

/**
 * \brief   Hash Map where keys are integers and values are strings
 **/
using IntegerToStringMap    = TEIntegerHashMap<String, const String &, ImplIntegerToStringMap>;

/**
 * \brief   Hash Map where keys are integers and values are pointers.
 **/
using IntegerToPointergMap  = TEIntegerHashMap<void *, void *, ImplIntegerToPointergMap>;

/**
 * \brief   Hash Map where keys are strings and values are integers.
 **/
using StringToIntegerMap    = TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap>;

/**
 * \brief   Hash Map where keys are strings and values are strings.
 **/
using StringToStringMap     = TEStringHashMap<String, const String &, ImplStringToStringMap>;

/**
 * \brief   Hash Map where keys are strings and values are pointers.
 **/
using StringToPointergMap   = TEStringHashMap<void *, void *, ImplStringToPointergMap>;

/**
 * \brief   Hash Map where keys are pointers and values are integers.
 **/
using PointerToIntegerMap   = TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap>;

/**
 * \brief   Hash Map where keys are pointers and values are strings.
 **/
using PointerToStringMap    = TEPointerHashMap<String, const String &, ImplPointerToStringMap>;

/**
 * \brief   Hash Map where keys are pointers and values are pointers.
 **/
using PointerToPointergMap  = TEPointerHashMap<void *, void *, ImplPointerToPointergMap>;

/**
 * \brief   Linked List where values are integers.
 **/
using IntegerList   = TELinkedList<unsigned int, unsigned int, ImplIntegerList>;

/**
 * \brief   Linked List where values are strings.
 **/
using StringList    = TELinkedList<String, const String &, ImplStringList>;

/**
 * \brief   Linked List where values are pointers
 **/
using PointerList   = TELinkedList<void *, void *, ImplPointerList>;

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
 * \brief   Class for tokenizing a String.
 **/
class AREG_API Tokenizer   :   public StringArray
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
    void Tokenize( const String & str, const String & delimiters, bool keepEmpty = true);
};

/************************************************************************
 * class template implementation
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TEIntegerHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/,class Implement>
TEIntegerHashMap<VALUE, VALUE_TYPE, Implement>::TEIntegerHashMap(int blockSize, int hashSize)
    : TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement> (blockSize, hashSize)
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIntegerHashMap<VALUE, VALUE_TYPE, Implement>::TEIntegerHashMap( const TEIntegerHashMap<VALUE, VALUE_TYPE, Implement> & src )
    : TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement> ( static_cast< const TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement> &>(src) )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIntegerHashMap<VALUE, VALUE_TYPE, Implement>::TEIntegerHashMap( TEIntegerHashMap<VALUE, VALUE_TYPE, Implement> && src ) noexcept
    : TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement> ( static_cast<TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement> &&>(src) )
{
}

//////////////////////////////////////////////////////////////////////////
// TEIdHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/,class Implement>
TEIdHashMap<VALUE, VALUE_TYPE, Implement>::TEIdHashMap(int blockSize, int hashSize)
    : TEHashMap<id_type, VALUE, id_type, VALUE_TYPE, Implement> (blockSize, hashSize)
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIdHashMap<VALUE, VALUE_TYPE, Implement>::TEIdHashMap( const TEIdHashMap<VALUE, VALUE_TYPE, Implement> & src )
    : TEHashMap<id_type, VALUE, id_type, VALUE_TYPE, Implement> ( static_cast< const TEHashMap<id_type, VALUE, id_type, VALUE_TYPE, Implement> &>(src) )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIdHashMap<VALUE, VALUE_TYPE, Implement>::TEIdHashMap( TEIdHashMap<VALUE, VALUE_TYPE, Implement> && src ) noexcept
    : TEHashMap<id_type, VALUE, id_type, VALUE_TYPE, Implement> ( static_cast<TEHashMap<id_type, VALUE, id_type, VALUE_TYPE, Implement> &&>(src) )
{
}

//////////////////////////////////////////////////////////////////////////
// TEStringHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEStringHashMap<VALUE, VALUE_TYPE, Implement>::TEStringHashMap( int blockSize, int hashSize )
    : TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement>   (blockSize, hashSize)
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEStringHashMap<VALUE, VALUE_TYPE, Implement>::TEStringHashMap( const TEStringHashMap<VALUE, VALUE_TYPE, Implement> & src )
    : TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement>   ( static_cast<const TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement> &>(src) )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEStringHashMap<VALUE, VALUE_TYPE, Implement>::TEStringHashMap( TEStringHashMap<VALUE, VALUE_TYPE, Implement> && src ) noexcept
    : TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement>   ( static_cast<TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement> &&>(src) )
{
}

//////////////////////////////////////////////////////////////////////////
// TEPointerHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEPointerHashMap<VALUE, VALUE_TYPE, Implement>::TEPointerHashMap( int blockSize, int hashSize )
    : TEHashMap<void *, VALUE, void *, VALUE_TYPE, Implement>   (blockSize, hashSize)
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEPointerHashMap<VALUE, VALUE_TYPE, Implement>::TEPointerHashMap( const TEPointerHashMap<VALUE, VALUE_TYPE, Implement> & src )
    : TEHashMap<void *, VALUE, void *, VALUE_TYPE, Implement>   ( static_cast< const TEHashMap<void *, VALUE, void *, VALUE_TYPE, Implement> &>(src) )
{
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEPointerHashMap<VALUE, VALUE_TYPE, Implement>::TEPointerHashMap( TEPointerHashMap<VALUE, VALUE_TYPE, Implement> && src ) noexcept
    : TEHashMap<void *, VALUE, void *, VALUE_TYPE, Implement>   ( static_cast<TEHashMap<void *, VALUE, void *, VALUE_TYPE, Implement> &&>(src) )
{
}
