#ifndef AREG_BASE_ECONTAINERS_HPP
#define AREG_BASE_ECONTAINERS_HPP

/************************************************************************
 * \file        areg/src/base/EContainers.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Collections of containers.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/TEArrayList.hpp"
#include "areg/src/base/TEHashMap.hpp"
#include "areg/src/base/TELinkedList.hpp"
#include "areg/src/base/TERingStack.hpp"
#include "areg/src/base/TEStack.hpp"
#include "areg/src/base/TESortedLinkedList.hpp"

#include "areg/src/base/CEString.hpp"
#include "areg/src/base/NEMath.hpp"

/**
 * \brief       Collection of container classes like linked list,
 *              hash maps and arrays.
 * 
 * \details     These are collections of typed TEArrayList, TEHashMap,
 *              TELinkedList template classes. All classes are
 *              derived public. Override the template or methods if need
 *              customized check of key and value equality, or calculation of hash-key.
 *
 **/

//////////////////////////////////////////////////////////////////////////
// Hierarchy of classes.
//////////////////////////////////////////////////////////////////////////

// class TEArrayList
    class CEIntegerArray;
    class CEStringArray;
    class CEPointerArray;

// class TEHashMap
    template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEIntegerHashMapImpl */>
    class TEIntegerHashMap;
        class CEIntegerToIntegerMap;
        class CEIntegerToStringMap;
        class CEIntegerToPointergMap;

    template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEStringHashMapImpl */>
    class TEStringHashMap;
        class CEStringToIntegerMap;
        class CEStringToStringMap;
        class CEStringToPointergMap;

    template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement /* = TEPointerHashMap */>
    class TEPointerHashMap;
        class CEPointerToIntegerMap;
        class CEPointerToStringMap;
        class CEPointerToPointergMap;

// class TELinkedList;
    class CEIntegerList;
    class CEStringList;
    class CEPointerList;

// class TESortedLinkedList;
    class CESortedIntegerList;
    class CESortedStringList;

//////////////////////////////////////////////////////////////////////////
// TEIntegerHashMap class template declaration
//////////////////////////////////////////////////////////////////////////

template <typename VALUE_TYPE>
class TEIntegerHashMapImpl  : public TEHashMapImpl<unsigned int, VALUE_TYPE>
{
public:
    /**
     * \brief	Calculates and returns Hash Key value.
     * \param	Key	    The key object to get hash key value
     * \return	32-bit hash key value
     **/
    inline unsigned int ImplHashKey( unsigned int Key ) const
    {
        return Key;
    }
};

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
     * \brief   Default constructor. Creates Hash Map with integer keys, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEIntegerHashMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with integer keys.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEIntegerHashMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
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

template <typename VALUE_TYPE>
class TEIdHashMapImpl   : public TEHashMapImpl<ITEM_ID, VALUE_TYPE>
{
public:
    /**
     * \brief	Calculates and returns Hash Key value.
     * \param	Key	    The key object to get hash key value
     * \return	32-bit hash key value
     **/
    inline unsigned int ImplHashKey( ITEM_ID Key ) const
    {
        return static_cast<unsigned int>(Key);
    }
};

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
     * \brief   Default constructor. Creates Hash Map with integer keys, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEIdHashMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with integer keys.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEIdHashMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
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
template <typename VALUE_TYPE>
class TEStringHashMapImpl   : public TEHashMapImpl<const CEString &, VALUE_TYPE>
{
public:
    /**
     * \brief	Calculates Hash Key value of string.
     * \param	Key	    The key value of element in hash map.
     * \return	Calculated 32-bit Hash Key value of string.
     **/
    unsigned int ImplHashKey( const CEString & Key ) const
    {
        return static_cast<unsigned int>(Key);
    }
};

/**
 * \brief   Hash Map class template with string keys
 *          to calculate hash key value of string
 * \tparam  VALUE       The type of value to store in map
 * \tparam  VALUE_TYPE  The type when get or set value
 **/
template <typename VALUE, typename VALUE_TYPE = VALUE, class Implement = TEStringHashMapImpl<VALUE_TYPE>>
class TEStringHashMap : public TEHashMap<CEString, VALUE, const CEString &, VALUE_TYPE, Implement>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with string keys, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEStringHashMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with string keys.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEStringHashMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
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

template <typename VALUE_TYPE>
class TEPointerHashMapImpl: public TEHashMapImpl<const void *, VALUE_TYPE>
{
public:
    /**
     * \brief	Calculates and returns Hash Key of pointer.
     * \param	Key	    The key object to get hash key value
     * \return	32-bit hash key value
     **/
    inline unsigned int ImplHashKey( const void * Key ) const
    {
        return MACRO_PTR2COUNT(Key);
    }
};

/**
 * \brief   Hash Map class template with pointer keys.
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
     * \brief   Default constructor. 
     *          By default, it creates Hash Map with pointer keys, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEPointerHashMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with pointer keys.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEPointerHashMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TEPointerHashMap( const TEPointerHashMap<VALUE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Destructor
     **/
    ~TEPointerHashMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEIntegerArray class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEListImpl<unsigned int>  ImplIntegerArray;
/**
 * \brief   Array of integer elements
 **/
class AREG_API CEIntegerArray    : public TEArrayList<unsigned int, unsigned int, ImplIntegerArray>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates array of integer elements.
     * \param   capacity    Initial reserved space of array. By default the initial reserved space is zero.
     * \param   increase    The number to increase elements every time when need to reallocate new space
     *                      By default the increase value is CETemplateConstants::ARRAY_DEFAULT_INCREASE (-1),
     *                      which means that elements will grow by CETemplateConstants::ARRAY_DEFAULT_MIN_GROW (4)
     **/
    CEIntegerArray(int capacity = 0, int increase = -1);

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEIntegerArray( const CEIntegerArray & src );

    /**
     * \brief   Destructor
     **/
    ~CEIntegerArray( void );
};

//////////////////////////////////////////////////////////////////////////
// CEStringArray class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEListImpl<const CEString &>    ImplStringArray;
/**
 * \brief   Array of string elements
 **/
class AREG_API CEStringArray     : public TEArrayList<CEString, const CEString &, ImplStringArray>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates array of string elements.
     * \param   capacity    Initial reserved space of array. By default the initial reserved space is zero.
     * \param   increase    The number to increase elements every time when need to reallocate new space
     *                      By default the increase value is CETemplateConstants::ARRAY_DEFAULT_INCREASE (-1),
     *                      which means that elements will grow by CETemplateConstants::ARRAY_DEFAULT_MIN_GROW (4)
     **/
    CEStringArray( int capacity = 0, int increase = -1 );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEStringArray( const CEStringArray & src );

    /**
     * \brief   Destructor
     **/
    ~CEStringArray( void );
};

//////////////////////////////////////////////////////////////////////////
// CEPointerArray class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEListImpl<const void *>    ImplPointerArray;
/**
 * \brief   Array of pointer elements
 **/
class AREG_API CEPointerArray    : public TEArrayList<void *, void *, ImplPointerArray>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates array of pointer elements.
     * \param   capacity    Initial reserved space of array. By default the initial reserved space is zero.
     * \param   increase    The number to increase elements every time when need to reallocate new space
     *                      By default the increase value is CETemplateConstants::ARRAY_DEFAULT_INCREASE (-1),
     *                      which means that elements will grow by CETemplateConstants::ARRAY_DEFAULT_MIN_GROW (4)
     **/
    CEPointerArray( int capacity = 0, int increase = -1 );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEPointerArray( const CEPointerArray & src );

    /**
     * \brief   Destructor
     **/
    ~CEPointerArray( void );
};

//////////////////////////////////////////////////////////////////////////
// CEIntegerToIntegerMap class declaration
//////////////////////////////////////////////////////////////////////////

typedef TEIntegerHashMapImpl<unsigned int>    ImplIntegerToIntegerMap;

/**
 * \brief   Hash Map class with elements of integer keys and integer values
 **/
class AREG_API CEIntegerToIntegerMap : public TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with integer keys and integer values, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    CEIntegerToIntegerMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with integer keys and integer values.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    CEIntegerToIntegerMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEIntegerToIntegerMap( const CEIntegerToIntegerMap & src );

    /**
     * \brief   Destructor
     **/
    ~CEIntegerToIntegerMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEIntegerToStringMap class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEIntegerHashMapImpl<const CEString &>  ImplIntegerToStringMap;

/**
 * \brief   Hash Map class with elements of integer keys and string values
 **/
class AREG_API CEIntegerToStringMap : public TEIntegerHashMap<CEString, const CEString &, ImplIntegerToStringMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with integer keys and string values, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    CEIntegerToStringMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with integer keys and string values.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    CEIntegerToStringMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEIntegerToStringMap( const CEIntegerToStringMap & src );

    /**
     * \brief   Destructor
     **/
    ~CEIntegerToStringMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEIntegerToPointergMap class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEIntegerHashMapImpl<const void *>  ImplIntegerToPointergMap;
/**
 * \brief   Hash Map class with elements of integer keys and pointer values
 **/
class AREG_API CEIntegerToPointergMap : public TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with integer keys and pointer values, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    CEIntegerToPointergMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with integer keys and pointer values.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    CEIntegerToPointergMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEIntegerToPointergMap( const CEIntegerToPointergMap & src );

    /**
     * \brief   Destructor
     **/
    ~CEIntegerToPointergMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEStringToIntegerMap class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEStringHashMapImpl<unsigned int>  ImplStringToIntegerMap;
/**
 * \brief   Hash Map class with elements of string keys and integer values
 **/
class AREG_API CEStringToIntegerMap  : public TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with string keys and integer values, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    CEStringToIntegerMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with string keys and integer values.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    CEStringToIntegerMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEStringToIntegerMap( const CEStringToIntegerMap & src );

    /**
     * \brief   Destructor
     **/
    ~CEStringToIntegerMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEStringToStringMap class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEStringHashMapImpl<const CEString &>  ImplStringToStringMap;
/**
 * \brief   Hash Map class with elements of string keys and string values
 **/
class AREG_API CEStringToStringMap   : public TEStringHashMap<CEString, const CEString &, ImplStringToStringMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with string keys and string values, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    CEStringToStringMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with string keys and string values.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    CEStringToStringMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEStringToStringMap( const CEStringToStringMap & src );

    /**
     * \brief   Destructor
     **/
    ~CEStringToStringMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEStringToPointergMap class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEStringHashMapImpl<const void *>  ImplStringToPointergMap;

/**
 * \brief   Hash Map class with elements of string keys and pointer values
 **/
class AREG_API CEStringToPointergMap : public TEStringHashMap<void *, const void *, ImplStringToPointergMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with string keys and pointer values, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    CEStringToPointergMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with string keys and pointer values.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    CEStringToPointergMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEStringToPointergMap( const CEStringToPointergMap & src );

    /**
     * \brief   Destructor
     **/
    ~CEStringToPointergMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEPointerToIntegerMap class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEPointerHashMapImpl<unsigned int>  ImplPointerToIntegerMap;

/**
 * \brief   Hash Map class with elements of pointer keys and integer values
 **/
class AREG_API CEPointerToIntegerMap : public TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with pointer keys and integer values, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    CEPointerToIntegerMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with pointer keys and integer values.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    CEPointerToIntegerMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEPointerToIntegerMap( const CEPointerToIntegerMap & src );

    /**
     * \brief   Destructor
     **/
    ~CEPointerToIntegerMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEPointerToStringMap class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEPointerHashMapImpl<const CEString &>  ImplPointerToStringMap;

/**
 * \brief   Hash Map class with elements of pointer keys and string values
 **/
class AREG_API CEPointerToStringMap : public TEPointerHashMap<CEString, const CEString &, ImplPointerToStringMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with pointer keys and string values, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    CEPointerToStringMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with pointer keys and string values.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    CEPointerToStringMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEPointerToStringMap( const CEPointerToStringMap & src );

    /**
     * \brief   Destructor
     **/
    ~CEPointerToStringMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEPointerToPointergMap class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEPointerHashMapImpl<const void *>  ImplPointerToPointergMap;

/**
 * \brief   Hash Map class with elements of pointer keys and pointer values
 **/
class AREG_API CEPointerToPointergMap : public TEPointerHashMap<void *, const void *, ImplPointerToPointergMap>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates Hash Map with pointer keys and pointer values, 
     *          which Block Size is MAP_DEFAULT_BLOCK_SIZE (48) and 
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    CEPointerToPointergMap( void );
    
    /**
     * \brief	Constructor, initialization. Creates Hash Map with pointer keys and pointer values.
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    CEPointerToPointergMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEPointerToPointergMap( const CEPointerToPointergMap & src );

    /**
     * \brief   Destructor
     **/
    ~CEPointerToPointergMap( void );
};

//////////////////////////////////////////////////////////////////////////
// CEIntegerList class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEListImpl<unsigned int>  ImplIntegerList;
/**
 * \brief   Linked List class of integer elements
 **/
class AREG_API CEIntegerList :   public TELinkedList<unsigned int, unsigned int, ImplIntegerList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty linked list of integer elements
     **/
    CEIntegerList( void );
    
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEIntegerList( const CEIntegerList & src );

    /**
     * \brief   Destructor
     **/
    ~CEIntegerList( void );
};

//////////////////////////////////////////////////////////////////////////
// CEStringList class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEListImpl<const CEString &>    ImplStringList;
/**
 * \brief   Linked List class of string elements
 **/
class AREG_API CEStringList :   public TELinkedList<CEString, const CEString &, ImplStringList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty linked list of string elements
     **/
    CEStringList( void );
    
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEStringList( const CEStringList & src );

    /**
     * \brief   Destructor
     **/
    ~CEStringList( void );
};

//////////////////////////////////////////////////////////////////////////
// CEPointerList class declaration
//////////////////////////////////////////////////////////////////////////
typedef TEListImpl<const void *>        ImplPointerList;
/**
 * \brief   Linked List class of pointer elements
 **/
class AREG_API CEPointerList :   public TELinkedList<void *, const void *, ImplPointerList>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty linked list of pointer elements
     **/
    CEPointerList( void );
    
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEPointerList( const CEPointerList & src );

    /**
     * \brief   Destructor
     **/
    ~CEPointerList( void );
};

//////////////////////////////////////////////////////////////////////////
// CESortedIntegerList class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Linked List class of integer elements
 **/
class AREG_API CESortedIntegerList  :   public TESortedLinkedList<unsigned int, unsigned int, TESortImpl<unsigned int>>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty linked list of integer elements
     **/
    CESortedIntegerList( bool sortAcceding = true );
    
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CESortedIntegerList( const CESortedIntegerList & src );

    /**
     * \brief   Destructor
     **/
    ~CESortedIntegerList( void );
};

//////////////////////////////////////////////////////////////////////////
// CESortedStringList class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CESortedStringListImpl    : public TESortImpl<const CEString &>
{
public:
    /**
     * \brief   Compares 2 values, returns true if they are equal.
     * \param   Value1  The right-side value to compare.
     * \param   Value2  The left-side value to compare.
     * \return  Returns true if 2 values are equal.
     **/
    inline bool ImplEqualValues(const CEString & Value1, const CEString & Value2) const
    {
        return ( NEMath::CompEqual == static_cast<NEMath::eCompare>(NEString::compareFast<char, char>(Value1.GetBuffer(), Value2.GetBuffer())) );
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
    inline NEMath::eCompare ImplCompareValues(const CEString & Value1, const CEString & Value2) const
    {
        return static_cast<NEMath::eCompare>(NEString::compareFast<char, char>(Value1.GetBuffer(), Value2.GetBuffer()));
    }
};

/**
 * \brief   Linked List class of string elements
 **/
class AREG_API CESortedStringList   :   public TESortedLinkedList<CEString, const CEString &, CESortedStringListImpl>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty linked list of string elements
     **/
    CESortedStringList( bool sortAcceding = true );
    
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CESortedStringList( const CESortedStringList & src );

    /**
     * \brief   Destructor
     **/
    ~CESortedStringList( void );
};


//////////////////////////////////////////////////////////////////////////
// CETokenizer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Class for tokenizing a String into String tokens
 **/
class AREG_API CETokenizer   :   public CEStringArray
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Creates empty array
     **/
    CETokenizer( void );
    
    /**
     * \brief   Constructor, taking a String to tokenize
     * \param   str         the String to tokenize
     * \param   delimiters  one or more delimiter chars that define token boundaries
     * \param   keepEmpty   if two delimiters next to each other specify an empty token
     *                      if false, the result will only contain nonempty tokens
     **/
    CETokenizer( const CEString & str, const CEString & delimiters, bool keepEmpty = true);

    
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CETokenizer( const CETokenizer & src );

    /**
     * \brief   Destructor
     **/
    ~CETokenizer( void );
//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Tokenize string. After operation, this is an array of all tokens
     * \param   str         the String to tokenize
     * \param   delimiters  one or more delimiter chars that define token boundaries
     * \param   keepEmpty   if two delimiters next to each other specify an empty token
     *                      if false, the result will only contain nonempty tokens
     **/
    void Tokenize( const CEString & str, const CEString & delimiters, bool keepEmpty = true);
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
    : TEHashMap<CEString, VALUE, const CEString &, VALUE_TYPE, Implement>   ( )
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEStringHashMap<VALUE, VALUE_TYPE, Implement>::TEStringHashMap( int blockSize, int hashSize )
    : TEHashMap<CEString, VALUE, const CEString &, VALUE_TYPE, Implement>   (blockSize, hashSize)
{
    ; // do nothing
}

template <typename VALUE, typename VALUE_TYPE /*= VALUE*/, class Implement>
TEStringHashMap<VALUE, VALUE_TYPE, Implement>::TEStringHashMap( const TEStringHashMap<VALUE, VALUE_TYPE, Implement> & src )
    : TEHashMap<CEString, VALUE, const CEString &, VALUE_TYPE, Implement>   ( static_cast<const TEHashMap<CEString, VALUE, const CEString &, VALUE_TYPE, Implement> &>(src) )
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
