#ifndef AREG_BASE_ECONTAINERS_HPP
#define AREG_BASE_ECONTAINERS_HPP

/************************************************************************
 * \file        areg/base/EContainers.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    class IntegerArray;
    class StringArray;
        class Tokenizer;
    class PointerArray;

/* class TEHashMap */
    template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEIntegerHashMapImpl */>
    class TEIntegerHashMap;
        class IntegerToIntegerMap;
        class IntegerToStringMap;
        class IntegerToPointergMap;
    template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEStringHashMapImpl */>
    class TEStringHashMap;
        class StringToIntegerMap;
        class StringToStringMap;
        class StringToPointergMap;
    template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPointerHashMap */>
    class TEPointerHashMap;
        class PointerToIntegerMap;
        class PointerToStringMap;
        class PointerToPointergMap;

/* class TELinkedList */
    class IntegerList;
    class StringList;
    class PointerList;

/* class TESortedLinkedList */
    class SortedIntegerList;
    class SortedStringList;

//////////////////////////////////////////////////////////////////////////
// Types and implemented container helper classes and hierarchies.
//////////////////////////////////////////////////////////////////////////

// TEHashMapImpl
    template <typename VALUE_TYPE> class TEIntegerHashMapImpl;
    // typedef TEIntegerHashMapImpl<unsigned int>      ImplIntegerToIntegerMap;
    // typedef TEIntegerHashMapImpl<const String &>    ImplIntegerToStringMap;
    // typedef TEIntegerHashMapImpl<const void *>      ImplIntegerToPointergMap
    template <typename VALUE_TYPE> class TEIdHashMapImpl;
    template <typename VALUE_TYPE> class TEStringHashMapImpl;
    // typedef TEStringHashMapImpl<unsigned int>       ImplStringToIntegerMap;
    // typedef TEStringHashMapImpl<const String &>     ImplStringToStringMap;
    // typedef TEStringHashMapImpl<const void *>       ImplStringToPointergMap
    template <typename VALUE_TYPE> class TEPointerHashMapImpl;
    // typedef TEPointerHashMapImpl<unsigned int>      ImplPointerToIntegerMap;
    // typedef TEPointerHashMapImpl<const String &>    ImplPointerToStringMap;
    // typedef TEPointerHashMapImpl<const void *>      ImplPointerToPointergMap;

// TEListImpl
    // typedef TEListImpl<unsigned int>                ImplIntegerList;
    // typedef TEListImpl<const String &>              ImplStringList;
    // typedef TEListImpl<const void *>                ImplPointerList;

// TESortImpl
    // typedef TESortImpl<unsigned int>                ImplIntegerSortList;
    class SortedStringListImpl;

//////////////////////////////////////////////////////////////////////////
// Declaration and implementation of helper classes for hash maps
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The class template that contains implementation of hash-key calculation of Integer hash-maps.
 **/
template <typename VALUE_TYPE>
class TEIntegerHashMapImpl  : public TEHashMapImpl<unsigned int, VALUE_TYPE>
{
public:
    /**
     * \brief	Calculates and returns Hash Key value.
     * \param	Key	    The key object to get hash key value
     * \return	32-bit hash key value
     **/
    inline unsigned int implHashKey( unsigned int Key ) const
    {
        return Key;
    }
};

/**
 * \brief   The class template that contains implementation of hash-key calculation of special Item ID hash-maps.
 **/
template <typename VALUE_TYPE>
class TEIdHashMapImpl   : public TEHashMapImpl<ITEM_ID, VALUE_TYPE>
{
public:
    /**
     * \brief	Calculates and returns Hash Key value.
     * \param	Key	    The key object to get hash key value
     * \return	32-bit hash key value
     **/
    inline unsigned int implHashKey( ITEM_ID Key ) const
    {
        return static_cast<unsigned int>(Key);
    }
};

/**
 * \brief   The class template that contains implementation of hash-key calculation of string hash-maps.
 **/
template <typename VALUE_TYPE>
class TEStringHashMapImpl   : public TEHashMapImpl<const String &, VALUE_TYPE>
{
public:
    /**
     * \brief	Calculates Hash Key value of string.
     * \param	Key	    The key value of element in hash map.
     * \return	Calculated 32-bit Hash Key value of string.
     **/
    unsigned int implHashKey( const String & Key ) const
    {
        return static_cast<unsigned int>(Key);
    }
};

/**
 * \brief   The class template that contains implementation of hash-key calculation of pointer hash-maps.
 **/
template <typename VALUE_TYPE>
class TEPointerHashMapImpl: public TEHashMapImpl<const void *, VALUE_TYPE>
{
public:
    /**
     * \brief	Calculates and returns Hash Key of pointer.
     * \param	Key	    The key object to get hash key value
     * \return	32-bit hash key value
     **/
    inline unsigned int implHashKey( const void * Key ) const
    {
        return MACRO_PTR2INT32(Key);
    }
};

/**
 * \brief   Type for hash map where keys and values are integers.
 **/
typedef TEIntegerHashMapImpl<unsigned int>      ImplIntegerToIntegerMap;
/**
 * \brief   Type for hash map where keys are integers and values are strings.
 **/
typedef TEIntegerHashMapImpl<const String &>    ImplIntegerToStringMap;
/**
 * \brief   Type for Hash Map where keys are integers and values are pointers.
 **/
typedef TEIntegerHashMapImpl<const void *>      ImplIntegerToPointergMap;
/**
 * \brief   Type for Hash Map where keys are strings and values are integers.
 **/
typedef TEStringHashMapImpl<unsigned int>       ImplStringToIntegerMap;
/**
 * \brief   Type for Hash Map where keys are strings and values are strings.
 **/
typedef TEStringHashMapImpl<const String &>     ImplStringToStringMap;
/**
 * \brief   Type for Hash Map where keys are strings and values are pointers.
 **/
typedef TEStringHashMapImpl<const void *>       ImplStringToPointergMap;
/**
 * \brief   Type for Hash Map where keys are pointers and values are integers.
 **/
typedef TEPointerHashMapImpl<unsigned int>      ImplPointerToIntegerMap;
/**
 * \brief   Hash Map where keys are pointers and values are strings.
 **/
typedef TEPointerHashMapImpl<const String &>    ImplPointerToStringMap;
/**
 * \brief   Type for Hash Map where keys are pointers and values are pointers.
 **/
typedef TEPointerHashMapImpl<const void *>      ImplPointerToPointergMap;

//////////////////////////////////////////////////////////////////////////
// Declaration and implementation of helper classes for lists and arrays
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Type for array and list of integers.
 **/
typedef TEListImpl<unsigned int>                ImplIntegerList;
/**
 * \brief   Type for list and array of strings.
 **/
typedef TEListImpl<const String &>              ImplStringList;
/**
 * \brief   Type for list and array of pointers
 **/
typedef TEListImpl<const void *>                ImplPointerList;

//////////////////////////////////////////////////////////////////////////
// Declaration and implementation of helper classes for sorted lists and arrays
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Type for sorted list and array of integers
 **/
typedef TESortImpl<unsigned int>                ImplIntegerSortList;
/**
 * \brief   Type for sorted list and array of Strings
 **/
class AREG_API SortedStringListImpl  : public TESortImpl<const String &>
{
public:
    /**
     * \brief   Compares 2 values, returns true if they are equal.
     * \param   Value1  The right-side value to compare.
     * \param   Value2  The left-side value to compare.
     * \return  Returns true if 2 values are equal.
     **/
    inline bool implEqualValues(const String & Value1, const String & Value2) const
    {
        return ( NEMath::CompEqual == static_cast<NEMath::eCompare>(NEString::compareFast<char, char>(Value1.getString(), Value2.getString())) );
    }

    /**
     * \brief   Compares 2 values and returns:
     *              - NEMath::CompSmaller   if Value1 is smaller than Value2;
     *              - NEMath::CompEqual     if Value1 and Value2 are equal;
     *              - NEMath::CompGreater   if Value1 is greater than Value2.
     *          Make own implementation if comparing has special functionality.
     * \param   Value1  The left-side value to compare.
     * \param   Value2  The right-side value to compare.
     * \return  Returns one of following values:
     *              - NEMath::CompSmaller   if Value1 is smaller than Value2;
     *              - NEMath::CompEqual     if Value1 and Value2 are equal;
     *              - NEMath::CompGreater   if Value1 is greater than Value2.
     **/
    inline NEMath::eCompare implCompareValues(const String & Value1, const String & Value2) const
    {
        return static_cast<NEMath::eCompare>(NEString::compareFast<char, char>(Value1.getString(), Value2.getString()));
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
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEIntegerHashMapImpl<VALUE_TYPE>>
class TEIntegerHashMap : public TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map object where the keys are integers, the Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEIntegerHashMap( void );
    
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
     * \brief   Destructor
     **/
    ~TEIntegerHashMap( void );
};

//////////////////////////////////////////////////////////////////////////
// TEIdHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   This hash map is used to store values associate with ID. This is mainly used to 
 *          keep control of resources. So that, there will be no other implementation. Because
 *          resources are mainly pointers and they would need individual solutions.
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEIdHashMapImpl<VALUE_TYPE>>
class TEIdHashMap   : public TEHashMap<ITEM_ID, VALUE, ITEM_ID, VALUE_TYPE, Implement>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map object where the keys are Item IDs, the Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEIdHashMap( void );
    
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
     * \brief   Destructor
     **/
    ~TEIdHashMap( void );
};

//////////////////////////////////////////////////////////////////////////
// TEStringHashMap class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map class template where key are strings.
 * \tparam  VALUE       The type of value to store in map
 * \tparam  VALUE_TYPE  The type when get or set value
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEStringHashMapImpl<VALUE_TYPE>>
class TEStringHashMap : public TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are strings, Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEStringHashMap( void );
    
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
     * \brief   Copy hash-map values from given source.
     * \param   src     The source to copy data.
     **/
    TEStringHashMap( const TEStringHashMap<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Destructor
     **/
    ~TEStringHashMap( void );
};

//////////////////////////////////////////////////////////////////////////
// TEPointerHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hash Map class template where keys are pointers.
 * \tparam  VALUE       The type of value to store in map
 * \tparam  VALUE_TYPE  The type when get or set value
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEPointerHashMapImpl<VALUE_TYPE>>
class TEPointerHashMap : public TEHashMap<void *, VALUE, const void *, VALUE_TYPE, Implement>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are pointers Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEPointerHashMap( void );
    
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
     * \brief   Destructor
     **/
    ~TEPointerHashMap( void );
};

//////////////////////////////////////////////////////////////////////////
// IntegerArray class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Array of integer elements
 **/
class AREG_API IntegerArray    : public TEArrayList<unsigned int, unsigned int, ImplIntegerList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates array of integer elements.
     * \param   capacity    Initial reserved space of array. By default the initial reserved space is zero.
     * \param   increase    The number to increase elements every time when need to reallocate new space
     *                      By default the increase value is TemplateConstants::ARRAY_DEFAULT_INCREASE (-1),
     *                      which means that elements will grow by TemplateConstants::ARRAY_DEFAULT_MIN_GROW (4)
     **/
    IntegerArray(int capacity = 0, int increase = -1);

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    IntegerArray( const IntegerArray & src );

    /**
     * \brief   Destructor
     **/
    ~IntegerArray( void );
};

//////////////////////////////////////////////////////////////////////////
// StringArray class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Array of string elements
 **/
class AREG_API StringArray     : public TEArrayList<String, const String &, ImplStringList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates array of string elements.
     * \param   capacity    Initial reserved space of array. By default the initial reserved space is zero.
     * \param   increase    The number to increase elements every time when need to reallocate new space
     *                      By default the increase value is TemplateConstants::ARRAY_DEFAULT_INCREASE (-1),
     *                      which means that elements will grow by TemplateConstants::ARRAY_DEFAULT_MIN_GROW (4)
     **/
    StringArray( int capacity = 0, int increase = -1 );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    StringArray( const StringArray & src );

    /**
     * \brief   Destructor.
     **/
    ~StringArray( void );
};

//////////////////////////////////////////////////////////////////////////
// PointerArray class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Array of pointer elements
 **/
class AREG_API PointerArray    : public TEArrayList<void *, void *, ImplPointerList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates array of pointer elements.
     * \param   capacity    Initial reserved space of array. By default the initial reserved space is zero.
     * \param   increase    The number to increase elements every time when need to reallocate new space
     *                      By default the increase value is TemplateConstants::ARRAY_DEFAULT_INCREASE (-1),
     *                      which means that elements will grow by TemplateConstants::ARRAY_DEFAULT_MIN_GROW (4)
     **/
    PointerArray( int capacity = 0, int increase = -1 );

    /**
     * \brief   Copies elements from given source.
     * \param   src     The source to copy data.
     **/
    PointerArray( const PointerArray & src );

    /**
     * \brief   Destructor
     **/
    ~PointerArray( void );
};

//////////////////////////////////////////////////////////////////////////
// IntegerToIntegerMap class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Hash Map where keys are values are integers.
 **/
class AREG_API IntegerToIntegerMap : public TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys and values are integers, Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    IntegerToIntegerMap( void );
    
    /**
     * \brief	Creates Hash Map where keys and values are integers.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    IntegerToIntegerMap( int blockSize, int hashSize );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    IntegerToIntegerMap( const IntegerToIntegerMap & src );

    /**
     * \brief   Destructor.
     **/
    ~IntegerToIntegerMap( void );
};

//////////////////////////////////////////////////////////////////////////
// IntegerToStringMap class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map where keys are integers and values are strings
 **/
class AREG_API IntegerToStringMap : public TEIntegerHashMap<String, const String &, ImplIntegerToStringMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are integers and values are strings, Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    IntegerToStringMap( void );
    
    /**
     * \brief	Creates Hash Map where keys are integers and values are strings.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    IntegerToStringMap( int blockSize, int hashSize );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    IntegerToStringMap( const IntegerToStringMap & src );

    /**
     * \brief   Destructor.
     **/
    ~IntegerToStringMap( void );
};

//////////////////////////////////////////////////////////////////////////
// IntegerToPointergMap class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map where keys are integers and values are pointers.
 **/
class AREG_API IntegerToPointergMap : public TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are integers and values are pointers, Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40).
     **/
    IntegerToPointergMap( void );
    
    /**
     * \brief	Creates Hash Map where keys are integers and values are pointers.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    IntegerToPointergMap( int blockSize, int hashSize );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    IntegerToPointergMap( const IntegerToPointergMap & src );

    /**
     * \brief   Destructor.
     **/
    ~IntegerToPointergMap( void );
};

//////////////////////////////////////////////////////////////////////////
// StringToIntegerMap class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map where keys are strings and values are integers.
 **/
class AREG_API StringToIntegerMap  : public TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are strings and values are integers, Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40).
     **/
    StringToIntegerMap( void );
    
    /**
     * \brief	Creates Hash Map where keys are strings and values are integers.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    StringToIntegerMap( int blockSize, int hashSize );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    StringToIntegerMap( const StringToIntegerMap & src );

    /**
     * \brief   Destructor.
     **/
    ~StringToIntegerMap( void );
};

//////////////////////////////////////////////////////////////////////////
// StringToStringMap class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map where keys are strings and values are strings.
 **/
class AREG_API StringToStringMap   : public TEStringHashMap<String, const String &, ImplStringToStringMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are strings and values are strings, Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40).
     **/
    StringToStringMap( void );
    
    /**
     * \brief	Creates Hash Map where keys are strings and values are strings.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    StringToStringMap( int blockSize, int hashSize );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    StringToStringMap( const StringToStringMap & src );

    /**
     * \brief   Destructor.
     **/
    ~StringToStringMap( void );
};

//////////////////////////////////////////////////////////////////////////
// StringToPointergMap class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map where keys are strings and values are pointers.
 **/
class AREG_API StringToPointergMap : public TEStringHashMap<void *, const void *, ImplStringToPointergMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are strings and values are pointers Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40).
     **/
    StringToPointergMap( void );
    
    /**
     * \brief	Creates Hash Map where keys are strings and values are pointers.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    StringToPointergMap( int blockSize, int hashSize );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    StringToPointergMap( const StringToPointergMap & src );

    /**
     * \brief   Destructor.
     **/
    ~StringToPointergMap( void );
};

//////////////////////////////////////////////////////////////////////////
// PointerToIntegerMap class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map where keys are pointers and values are integers.
 **/
class AREG_API PointerToIntegerMap : public TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are pointers and values are integers Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40).
     **/
    PointerToIntegerMap( void );
    
    /**
     * \brief	Creates Hash Map where keys are pointers and values are integers.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    PointerToIntegerMap( int blockSize, int hashSize );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    PointerToIntegerMap( const PointerToIntegerMap & src );

    /**
     * \brief   Destructor.
     **/
    ~PointerToIntegerMap( void );
};

//////////////////////////////////////////////////////////////////////////
// PointerToStringMap class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map where keys are pointers and values are strings.
 **/
class AREG_API PointerToStringMap : public TEPointerHashMap<String, const String &, ImplPointerToStringMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are pointers and values are strings Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40).
     **/
    PointerToStringMap( void );
    
    /**
     * \brief	Creates Hash Map where keys are pointers and values are strings.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    PointerToStringMap( int blockSize, int hashSize );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    PointerToStringMap( const PointerToStringMap & src );

    /**
     * \brief   Destructor.
     **/
    ~PointerToStringMap( void );
};

//////////////////////////////////////////////////////////////////////////
// PointerToPointergMap class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Hash Map where keys are pointers and values are pointers.
 **/
class AREG_API PointerToPointergMap : public TEPointerHashMap<void *, const void *, ImplPointerToPointergMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates Hash Map where keys are pointers and values are pointers Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40).
     **/
    PointerToPointergMap( void );
    
    /**
     * \brief	Creates Hash Map where keys are pointers and values are pointers.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    PointerToPointergMap( int blockSize, int hashSize );

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    PointerToPointergMap( const PointerToPointergMap & src );

    /**
     * \brief   Destructor.
     **/
    ~PointerToPointergMap( void );
};

//////////////////////////////////////////////////////////////////////////
// IntegerList class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Linked List where values are integers.
 **/
class AREG_API IntegerList :   public TELinkedList<unsigned int, unsigned int, ImplIntegerList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty Linked List where values are integers.
     **/
    IntegerList( void );
    
    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    IntegerList( const IntegerList & src );

    /**
     * \brief   Destructor
     **/
    ~IntegerList( void );
};

//////////////////////////////////////////////////////////////////////////
// StringList class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Linked List where values are strings.
 **/
class AREG_API StringList :   public TELinkedList<String, const String &, ImplStringList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty Linked List where values are strings.
     **/
    StringList( void );
    
    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    StringList( const StringList & src );

    /**
     * \brief   Destructor.
     **/
    ~StringList( void );
};

//////////////////////////////////////////////////////////////////////////
// PointerList class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Linked List where values are pointers
 **/
class AREG_API PointerList :   public TELinkedList<void *, const void *, ImplPointerList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty Linked List where values are pointers.
     **/
    PointerList( void );
    
    /**
     * \brief   Copies entries from given pointers.
     * \param   src     The source to copy data.
     **/
    PointerList( const PointerList & src );

    /**
     * \brief   Destructor.
     **/
    ~PointerList( void );
};

//////////////////////////////////////////////////////////////////////////
// SortedIntegerList class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Sorted List class where values are integers.
 **/
class AREG_API SortedIntegerList    :   public TESortedLinkedList<unsigned int, unsigned int, ImplIntegerSortList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty Sorted List class where values are integers.
     **/
    SortedIntegerList( bool sortAcceding = true );
    
    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    SortedIntegerList( const SortedIntegerList & src );

    /**
     * \brief   Destructor.
     **/
    ~SortedIntegerList( void );
};

//////////////////////////////////////////////////////////////////////////
// SortedStringList class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Sorted List class where values are strings.
 **/
class AREG_API SortedStringList     :   public TESortedLinkedList<String, const String &, SortedStringListImpl>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty Sorted List class where values are strings.
     **/
    SortedStringList( bool sortAcceding = true );
    
    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    SortedStringList( const SortedStringList & src );

    /**
     * \brief   Destructor.
     **/
    ~SortedStringList( void );
};


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
    Tokenizer( void );
    
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
     * \brief   Destructor.
     **/
    ~Tokenizer( void );
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
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIntegerHashMap<VALUE, VALUE_TYPE, Implement>::TEIntegerHashMap( void )
    : TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement> ( )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/,class Implement>
TEIntegerHashMap<VALUE, VALUE_TYPE, Implement>::TEIntegerHashMap(int blockSize, int hashSize)
    : TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement> (blockSize, hashSize)
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIntegerHashMap<VALUE, VALUE_TYPE, Implement>::TEIntegerHashMap( const TEIntegerHashMap<VALUE, VALUE_TYPE, Implement> & src )
    : TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement> ( static_cast< const TEHashMap<unsigned int, VALUE, unsigned int, VALUE_TYPE, Implement> &>(src) )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIntegerHashMap<VALUE, VALUE_TYPE, Implement>::~TEIntegerHashMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TEIdHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIdHashMap<VALUE, VALUE_TYPE, Implement>::TEIdHashMap( void )
    : TEHashMap<ITEM_ID , VALUE, ITEM_ID, VALUE_TYPE, Implement> ( )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/,class Implement>
TEIdHashMap<VALUE, VALUE_TYPE, Implement>::TEIdHashMap(int blockSize, int hashSize)
    : TEHashMap<ITEM_ID, VALUE, ITEM_ID, VALUE_TYPE, Implement> (blockSize, hashSize)
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIdHashMap<VALUE, VALUE_TYPE, Implement>::TEIdHashMap( const TEIdHashMap<VALUE, VALUE_TYPE, Implement> & src )
    : TEHashMap<ITEM_ID, VALUE, ITEM_ID, VALUE_TYPE, Implement> ( static_cast< const TEHashMap<ITEM_ID, VALUE, ITEM_ID, VALUE_TYPE, Implement> &>(src) )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEIdHashMap<VALUE, VALUE_TYPE, Implement>::~TEIdHashMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TEStringHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEStringHashMap<VALUE, VALUE_TYPE, Implement>::TEStringHashMap( void )
    : TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement>   ( )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEStringHashMap<VALUE, VALUE_TYPE, Implement>::TEStringHashMap( int blockSize, int hashSize )
    : TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement>   (blockSize, hashSize)
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEStringHashMap<VALUE, VALUE_TYPE, Implement>::TEStringHashMap( const TEStringHashMap<VALUE, VALUE_TYPE, Implement> & src )
    : TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement>   ( static_cast<const TEHashMap<String, VALUE, const String &, VALUE_TYPE, Implement> &>(src) )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEStringHashMap<VALUE, VALUE_TYPE, Implement>::~TEStringHashMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TEPointerHashMap<VALUE, VALUE_TYPE> class implementation
//////////////////////////////////////////////////////////////////////////
template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEPointerHashMap<VALUE, VALUE_TYPE, Implement>::TEPointerHashMap( void )
    : TEHashMap<void *, VALUE, const void *, VALUE_TYPE, Implement>   ( )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEPointerHashMap<VALUE, VALUE_TYPE, Implement>::TEPointerHashMap( int blockSize, int hashSize )
    : TEHashMap<void *, VALUE, const void *, VALUE_TYPE, Implement>   (blockSize, hashSize)
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEPointerHashMap<VALUE, VALUE_TYPE, Implement>::TEPointerHashMap( const TEPointerHashMap<VALUE, VALUE_TYPE, Implement> & src )
    : TEHashMap<void *, VALUE, const void *, VALUE_TYPE, Implement>   ( static_cast< const TEHashMap<void *, VALUE, const void *, VALUE_TYPE, Implement> &>(src) )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEPointerHashMap<VALUE, VALUE_TYPE, Implement>::~TEPointerHashMap( void )
{
    ; // do nothing
}

#endif  // AREG_BASE_ECONTAINERS_HPP
