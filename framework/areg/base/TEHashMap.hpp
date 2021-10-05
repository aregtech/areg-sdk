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
 * \file        areg/base/TEHashMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Hash Map class template.
 *              Hash Map object to store elements by its hash value.
 *              Every element is accessed by unique Key value.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TETemplateBase.hpp"
#include "areg/base/TEPair.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEMemory.hpp"

//////////////////////////////////////////////////////////////////////////
// TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The Hash Map binds Value with its Key. The Key element is unique. 
 *          Values are accessed by Key. Key and Value can be of different types.
 *
 *          Every element is stored in Block containing the Key, Value, Hash and 
 *          the pointer to the next Block within the Hash Table. The Hash value 
 *          is calculated on Key. If large number of element is going to be saved 
 *          in Hash Map, it is recommended to increase the size of Hash Table.
 *          By default, the size of the Hash Table is NECommon::MAP_DEFAULT_HASH_SIZE.
 *          The maximum size of the Hash Table is NECommon::MAP_MAX_TABLE_SIZE.
 *
 *          To access first element in Hash Map, get the first Position (call function 
 *          firstPosition()). Each next element is accessed by calling nex position.
 *          The type KEY should be possible to convert to unsigned int type required to 
 *          calculate the Hash. KEY and VALUE types should have at least default constructor
 *          and valid assigning operator.
 * 
 *          The HashMap object is not thread safe and data should be  synchronized manually.
 *
 * \tparam  KEY         The type of Key to identify values in hash map.
 *                      Either should be primitive or should have at least
 *                      default constructor, initialization or copy 
 *                      constructor depending KEY and KEY_TYPE types.
 *                      Should be possible to convert to unsigned int
 *                      type to calculate hash value.
 * \tparam  VALUE       The type of stored items. Either should be 
 *                      primitive or should have default constructor 
 *                      and valid assigning operator. Also, should be 
 *                      possible to convert to type VALUE_TYPE.
 * \tparam  KEY_TYPE    The type of Key. By default is same as KEY.
 *                      If different, should be possible to convert 
 *                      and assign to KEY type.
 * \tparam  VALUE_TYPE  By default same as VALUE, but can be any other
 *                      type, which is converted and assign to VALUE type.
 * \tparam  Implement   The class that contains methods to get hash key value,
 *                      compare keys and values of the map. Pass own implementation
 *                      if default methods needs to be changed.
 **/
template < typename KEY, typename VALUE, typename KEY_TYPE = KEY, typename VALUE_TYPE = VALUE, class Implement = TEHashMapImpl<KEY_TYPE, VALUE_TYPE> >
class TEHashMap
{
//////////////////////////////////////////////////////////////////////////
// Internal objects and types declaration
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Hidden / Protected class declaration.
     *          The Block class contains information
     *          of element in Hash Map -- value, pointers to next element,
     *          key value and hash value of Block object.
     **/
    //////////////////////////////////////////////////////////////////////////
    // TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::Block class declaration
    //////////////////////////////////////////////////////////////////////////
    class Block   : public TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>
    {
    //////////////////////////////////////////////////////////////////////////
    // constructor / destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor
         **/
        Block( void );
        /**
         * \brief   Initialization constructor.
         **/
        Block(unsigned int hash, KEY_TYPE key, VALUE_TYPE value);
        /**
         * \brief   Moves data from given source.
         **/
        Block( Block && src ) noexcept;
        /**
         * \brief   Destructor.
         **/
        ~Block();

    //////////////////////////////////////////////////////////////////////////
    // member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Pointer to next block in hash map
         **/
        Block *       mNext;
        /**
         * \brief   Hash value of element, 
         *          it cannot be different for every element key, as well as it identifies
         *          the index in Hash Table, which value cannot be more than the Maximum Hash Size
         *          of Hash Table. The maximum size is no more than MAP_MAX_TABLE_SIZE (1024)
         **/
        unsigned int    mHash;
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. 
     *          By default, the block size is MAP_DEFAULT_BLOCK_SIZE (48),
     *          Hash Table size is MAP_DEFAULT_HASH_SIZE (40)
     **/
    TEHashMap( void );

    /**
     * \brief	Constructor, initialization
     * \param	blockSize	The size of blocks in hash map to create at once.
     *                      If this is negative value, it creates MAP_DEFAULT_BLOCK_SIZE blocks.
     *                      It cannot be more than MAP_MAX_BLOCK_SIZE (1024)
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEHashMap( int blockSize, int hashSize );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TEHashMap( const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src );

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    TEHashMap( TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TEHashMap( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief	Assigning operator. It copies all elements from source map
     **/
    TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& operator = ( const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src );

    /**
     * \brief	Move operator. It moves all elements from source map
     **/
    TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& operator = ( TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src ) noexcept;

    /**
     * \brief   Checks equality of 2 hash-map objects, and returns true if they are equal.
     *          There should be possible to compare KEY and VALUE types of hash map.
     * \param   other   The hash-map object to compare
     **/
    bool operator == ( const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const;

    /**
     * \brief   Checks inequality of 2 hash-map objects, and returns true if they are not equal.
     *          There should be possible to compare KEY and VALUE types of hash map.
     * \param   other   The hash-map object to compare
     **/
    bool operator != ( const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & other ) const;

    /**
     * \brief	Subscript operator. Returns reference to value of element by given key.
     *          If the key does not exist, inserts an element into Hash Map with specified key value. 
     *          Used on the left of an assignment statement.
     **/
    VALUE & operator [] ( KEY_TYPE Key );

    /**
     * \brief	Subscript operator. Returns value of element by given key.
     *          If the key does not exist, inserts an element into Hash Map with specified key value.
     *          Modification of value is not possible.
     **/
    inline VALUE_TYPE operator [] ( KEY_TYPE Key ) const;

/************************************************************************/
// Friend global operators to make Hash Map streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream Hash Map Key and Value pair Elements and saves in Hash Map.
     *          If Hash Map previously had Elements, they will be lost.
     *          The Elements in the Hash Map will be initialized in the same sequence
     *          as they were written.
     *          There should be possibility to initialize Value and Key pairs from streaming object and
     *          if KEY and VALUE are not primitives, but objects, they should have implemented streaming operator.
     * \param   stream  The streaming object for reading values
     * \param   input   The Hash Map object to save initialized values.
     **/
     template <typename K, typename V, typename KT, typename VT, class Impl>
    friend const IEInStream & operator >> ( const IEInStream & stream, TEHashMap<K, V, KT, VT, Impl> & input);
    /**
     * \brief   Writes to the stream Hash Map values.
     *          The Elements of Hash Map will be written to the stream starting from start position.
     *          There should be possibility to stream every Element of Hash Map and if KEY and VALUE 
     *          are not primitives, but objects, they should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Hash Map object to read out values.
     **/
     template <typename K, typename V, typename KT, typename VT, class Impl>
    friend IEOutStream & operator << ( IEOutStream & stream, const TEHashMap<K, V, KT, VT, Impl> & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Attributes
/************************************************************************/

    /**
     * \brief	Returns true if Hash Map is empty
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief	Returns the size of Hash Map
     **/
    inline int getSize( void ) const;

    /**
     * \brief	Returns the size of hash table
     **/
    inline int getTableSize( void ) const;

    /**
     * \brief	If Hash Map is not empty, returns the starting position.
     *          Otherwise, returns nullptr.
     **/
    inline MAPPOS firstPosition( void ) const;


/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief	Searches element by given key. 
     *          If found element, return true and on exit returns the value of element
     * \param	Key	        Key to search.
     * \param	out_Value   On output, contains value of found element
     * \return	Returns true if finds element with specified key.
     **/
    inline bool find( KEY_TYPE Key, VALUE & out_Value ) const;

    /**
     * \brief	Search an element by given key and returns position.
     *          If could not find element, returns nullptr (INVALID_POSITION).
     * \param	Key	    The key to search.
     * \return	If finds, return position in Hash Map, otherwise returns nullptr (INVALID_POSITION)
     **/
    inline MAPPOS find( KEY_TYPE Key ) const;

    /**
     * \brief	Update existing element value or inserts new element in the Hash Map.
     *          If searchBeforeInsert is true, it will search for element first, 
     *          and if finds, it will update existing. Otherwise, it inserts new element.
     *          if searchBeforeInsert is false, it will insert new element without searching
     *          and no uniqueness of key values in hash map can be guarantied.
     * \param	Key	                The key of element to search or to create
     * \param	newValue	        The value of element to set or insert
     * \param	searchBeforeInsert	If true, it will search before creating new element and if finds
     *                              update value of existing element. Otherwise create new element
     *                              without checking uniqueness of keys in hash map
     * \return  Returns position of updated or new inserted element.
     **/
    MAPPOS setAt( KEY_TYPE Key, VALUE_TYPE newValue, bool searchBeforeInsert = true );
    /**
     * \brief	Update existing element value or inserts new element in the Hash Map.
     *          If searchBeforeInsert is true, it will search for element first, 
     *          and if finds, it will update existing. Otherwise, it inserts new element.
     *          if searchBeforeInsert is false, it will insert new element without searching
     *          and no uniqueness of key values in hash map can be guarantied.
     * \param	newElement	        The Key and Value pair of element to set or insert
     * \param	searchBeforeInsert	If true, it will search before creating new element and if finds
     *                              update value of existing element. Otherwise create new element
     *                              without checking uniqueness of keys in hash map
     * \return  Returns position of updated or new inserted element.
     **/
    inline MAPPOS setAt( const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & newElement, bool searchBeforeInsert = true );

    /**
     * \brief   Updates Existing Key and returns the position in the map.
     *          If Key does not exit, no new key will be set and function
     *          will return nullptr.
     * \param   Key         The key of map to update. The function will update
     *                      only existing key. 
     * \param   newValue    New value to set on existing key.
     * \return  Returns non-nullptr position value of existing key.
     *          And returns nullptr if key does not exit.
     **/
    MAPPOS updateAt( KEY_TYPE Key, VALUE_TYPE newValue );

    /**
     * \brief	Remove existing key and if key exists, returns true.
     * \param	Key	    The Key to search and remove
     * \return	Return true if key successfully removed from hash map
     **/
    bool removeAt( KEY_TYPE Key );

    /**
     * \brief	Remove existing key and if succeeded, on output contains value of removed element
     * \param	Key	            The key to remove from Hash Map
     * \param	out_Value       If removed with success, on output it contains value of removed element
     * \return	Return true if successfully removed key. If key does not exist, return false.
     **/
    bool removeAt( KEY_TYPE Key, VALUE & out_Value );

    /**
     * \brief	Update value of an element by given position and return position to next element
     * \param	atPosition      Position of element to update value
     * \param	newValue	    New value to set for existing element.
     * \return  Returns position of next element. If given position was position of last valid element, it will return nullptr.
     **/
    MAPPOS setPosition( MAPPOS atPosition, VALUE_TYPE newValue );

    /**
     * \brief	Removes element by given position, and on output key and value 
     *          parameters contain value of remove element.
     *          The function returns next position of element in hash map.
     * \param	curPos      This should contain valid position of hash map.
     * \param	out_Key     On output, this contains key of removed element
     * \param	out_Value   On output, this contains value of removed element.
     * \return  Next position in hash map or nullptr if reached the end.
     **/
    MAPPOS removePosition( MAPPOS curPos, KEY & out_Key, VALUE & out_Value );

    /**
     * \brief	Removes element by given valid position and returns value of removed element
     * \param	atPosition	Valid position of element in hash map
     * \return	Value of removed element.
     **/
    VALUE removePosition( MAPPOS atPosition );

    /**
     * \brief   Removes all elements in hash map, sets size zero
     **/
    void removeAll( void );

    /**
     * \brief	By given position value, retrieves key and value of element, and returns next position
     * \param	atPosition  Starting Position to get next position of element, and retrieve value and key
     * \param	out_Key     On output, this contains key of given position
     * \param	out_Value   On output, this contains value of given position
     * \return	Next position of element or next if it is last element in hash map.
     **/
    MAPPOS nextPosition( MAPPOS atPosition, KEY & out_Key, VALUE & out_Value ) const;
    /**
     * \brief	By given position value, retrieves key and value pair of element, and returns next position
     * \param	atPosition  Starting Position to get next position of element, and retrieve value and key pair
     * \param	out_Element On output, this element contains pair of Key and Value
     * \return	Next position of element or next if it is last element in hash map.
     **/
    inline MAPPOS nextPosition( MAPPOS atPosition, TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & out_Element ) const;
    /**
     * \brief	By given position value, returns next position
     * \param	atPosition  Starting Position to get next position of element
     * \return	Next position of element or next if it is last element in hash map.
     **/
    inline MAPPOS nextPosition( MAPPOS atPosition ) const;
    /**
     * \brief   Returns reverence of element value at given valid position.
     *          If index is not valid, assertion is raised.
     **/
    inline VALUE & getPosition( const MAPPOS atPosition );
    /**
     * \brief   Returns element value at given valid position.
     *          If index is not valid, assertion is raised.
     **/
    inline VALUE_TYPE getPosition( const MAPPOS atPosition ) const;
    /**
     * \brief	Returns reference to value of element by given key.
     *          If the key does not exist, inserts an element into Hash Map with specified key value.
     **/
    VALUE & getAt(KEY_TYPE Key);
    /**
     * \brief	Finds element by given Key and returns Value. The key should exist in hash map.
     *          If not element with give key found, assertion raises.
     * \param	Key	    The Key to search
     * \return	Value of element
     **/
    inline VALUE_TYPE getAt( KEY_TYPE Key ) const;

    /**
     * \brief	Retrieves key and value of element by given position
     * \param	atPosition	The position of element to retrieve key and value
     * \param	out_Key	    On output, contains key of element of given position
     * \param	out_Value   On output, contains value of element of given position
     **/
    inline void getAtPosition( MAPPOS atPosition, KEY & out_Key, VALUE & out_Value ) const;
    /**
     * \brief	Retrieves key and value pair of element by given position
     * \param	atPosition	The position of element to retrieve key and value
     * \param	out_Element On output, contains Key and Value pair of element of given position
     **/
    inline void getAtPosition( MAPPOS atPosition, TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & out_Element) const;

    /**
     * \brief   Returns the Key object value at the given position.
     *          The Key should be used only for reading. Avoid modifications of Key value.
     * \param   atPosition  The position to return Key object value.
     **/
    inline KEY_TYPE keyAtPosition( MAPPOS atPosition ) const;

    /**
     * \brief   Returns the Value object value at the given position.
     * \param   atPosition  The position to return Value object value.
     **/
    inline VALUE_TYPE valueAtPosition( MAPPOS atPosition ) const;

    /**
     * \brief	Gets next element in hash map by given valid position and on output, 
     *          returns position, key and value of next element
     * \param	in_out_NextPosition	On input this should be valid position,
     *                              on output, this contain position to next element in hash map
     * \param	out_NextKey	        On output, this contains key of next element in hash map
     * \param	out_NextValue       On output, this contain value of next element in hash map
     * \return	Returns true, if next element was found and values on output are valid.
     **/
    bool nextEntry( MAPPOS & in_out_NextPosition, KEY & out_NextKey, VALUE & out_NextValue ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief	Calculates 32-bit and return the Hash Key value
     *          of the given Key object. The Hash Key value differs
     *          from Hash value stored in every element.
     *          The hash key value is used to calculate hash index
     *          of element in Hash Table.
     * \param	Key	    The key object to get hash key value
     * \return	Returns 32-bit hash key value
     **/
    inline unsigned int getHashKey( KEY_TYPE Key ) const;

    /**
     * \brief   Called when comparing 2 keys.
     *          Overwrite method when need to change comparison.
     * \param   key1    Key on left side to compare
     * \param   key2    Key on right side to compare
     * \return  If function returns true, 2 keys are equal.
     *          Otherwise, they are not equal.
     **/
    inline bool isEqualKeys( KEY_TYPE key1, KEY_TYPE key2 ) const;

    /**
     * \brief   Called when comparing 2 values of element.
     *          Overwrite method when need to change comparison.
     * \param   value1  Value on left side to compare.
     * \param   value2  Value on right side to compare.
     * \return  If function returns true, 2 values are equal.
     *          Otherwise, they are not equal.
     **/
    inline bool isEqualValues( VALUE_TYPE value1, VALUE_TYPE value2) const;

//////////////////////////////////////////////////////////////////////////
// Protected / internal operations
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief	Initialize the Hash Table object.
     * \param	sizeHashTable	The initial size of hash table. By default it is MAP_DEFAULT_HASH_SIZE (48)
     **/
    void initHashTable( int sizeHashTable = NECommon::MAP_DEFAULT_HASH_SIZE );

    /**
     * \brief   Creates block list (mBlockSize elements contained in Block List)
     *          and initialize entries of elements in Block List
     **/
    void createBlockList( void );

    /**
     * \brief   Delete Block List entries
     **/
    void deleteBlockList( void );

    /**
     * \brief   Free given block and place in Free List
     * \param block The block to free and link with Free List
     **/
    void freeBlock( Block * block );

    /**
     * \brief	Finds and returns pointer to block object by given Key if element by key exists.
     *          Otherwise, it returns nullptr.
     *          On output, 'outHash' contains hash value, which is defining index in Hash Table.
     * \param	Key	        The Key to search
     * \param	out_Hash    The Hash value of entry within Hash Table
     * \return	If found Key, returns pointer to Block element, otherwise returns nullptr.
     **/
    inline Block * blockAt( KEY_TYPE Key, unsigned int & OUT out_Hash ) const
    {
        Block * result = nullptr;
        out_Hash = getHashKey(Key);

        if ( mHashTable != nullptr )
        {
            unsigned int size = static_cast<unsigned int>(mHashTableSize);
            for ( result  = mHashTable[out_Hash % size]; result != nullptr; result = result->mNext)
            {
                if ( out_Hash == result->mHash && isEqualKeys(result->mKey, Key) )
                    break;
            }
        }

        return result;
    }

    /**
     * \brief	Finds and returns pointer to pointer of block object by given Key if element by key exists.
     *          Otherwise it returns nullptr.
     * \param	Key     The Key to search
     **/
    inline Block ** blockAtRef( KEY_TYPE Key ) const
    {
        Block ** result = nullptr;

        if ( mHashTable != nullptr )
        {
            unsigned int hash = getHashKey(Key);
            unsigned int size = static_cast<unsigned int>(mHashTableSize);
            for ( result = &(mHashTable[hash % size]); (*result) != nullptr; result = &((*result)->mNext))
            {
                if ( hash == (*result)->mHash && isEqualKeys((*result)->mKey, Key) )
                    break;
            }
        }

        return result;
    }

    /**
     * \brief   Returns Block object at the given position.
     *          If the position object is a first position, it will return first valid block.
     * \param atPosition    The position of Block
     **/
    inline Block * blockAt( MAPPOS atPosition ) const
    {
        ASSERT(atPosition != nullptr );
        return (atPosition == NECommon::START_POSITION ? firstValidBlock() : static_cast<Block *>(atPosition));
    }

    /**
     * \brief   Finds and returns reference (pointer to pointer)
     *          of given block within hash map that changes on Block
     *          have direct impact on Hash Table elements.
     *          This speeds up remove procedure.
     * \param   block   Block to find reference.
     * \return  Address of pointer of block within hash map space.
     **/
    inline Block ** blockReference( const Block & block )
    {

        Block ** result  = mHashTable != nullptr ? &mHashTable[block.mHash % mHashTableSize] : nullptr;

        if (result != nullptr )
        {
            for ( ; *result != nullptr; result = &(*result)->mNext )
            {
                if ( block.mHash == (*result)->mHash && isEqualKeys( (*result)->mKey, block.mKey ) )
                    break;
            }
        }

        return result;
    }

    /**
     * \brief   Gets the very first valid Block within Hash Table.
     *          Called if function was called with position parameter
     *          equal to START_POSITION (0xFFFFFFFF)
     **/
    inline Block * firstValidBlock( void ) const
    {
        Block* result = nullptr;
        for ( int idx = 0; (result == nullptr) && (idx < mHashTableSize); ++ idx )
            result = mHashTable[idx];

        // must find something
        ASSERT(result != nullptr);
        return result;
    }

    /**
     * \brief   Gets the next valid Block within Hash Table bay given valid pointer to block object.
     * \param startAt   The pointer to Block object to start searching next entry. It must not be nullptr,
     *                  otherwise assertion raised
     **/
    inline Block * nextValidBlock( const Block * startAt ) const
    {
        ASSERT(startAt != nullptr);
        ASSERT(mElemCount != 0);
        Block* result = startAt->mNext;
        if (result == nullptr)
        {
            unsigned int size = static_cast<unsigned int>(mHashTableSize);
            for ( unsigned int idx = startAt->mHash % size + 1; (result == nullptr) && (idx < size); ++ idx )
                result = mHashTable[idx];
        }
        
        return result;
    }

    /**
     * \brief	Initialize new Block entry taken from Free List
     * \return	Returns pointer to new Block entry
     **/
    inline Block * initNewBlock( void )
    {
        if (mFreeList == nullptr)
            createBlockList();

        ASSERT(mFreeList != nullptr);
        Block* block= mFreeList;
        mFreeList   = mFreeList->mNext;
        mElemCount ++;
        ASSERT(mElemCount > 0);
        new(&block->mKey)	KEY;
        new(&block->mValue)VALUE;

        return block;
    }

    /**
     * \brief   Removes block in Hash Map
     * \param   block   Address of pointer of block
     *                  within Hash Map. Value returned
     *                  by GetBlockReference() function.
     **/
    inline void removeBlock( Block ** block)
    {
        Block* nextBlock = (*block)->mNext;
        freeBlock(*block);
        *block = nextBlock;
        if (mElemCount == 0)
            deleteAllBlocks();
    }

    /**
     * \brief   Deletes all blocks, reset all data.
     **/
    inline void deleteAllBlocks( void )
    {
        deleteBlockList();

        NEMemory::zeroElements<Block *>(mHashTable, mHashTableSize);
        mElemCount	= 0;
        mFreeList	= nullptr;
        mBlockList	= nullptr;
    }


//////////////////////////////////////////////////////////////////////////
// Member Variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Hash Table object. 
     *          By default the Hash Table size is MAP_DEFAULT_HASH_SIZE (64)
     *          and cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    Block **    mHashTable;
    /**
     * \brief   Size of Hash Map Table. By default it is MAP_DEFAULT_HASH_SIZE (64)
     *          and cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    int         mHashTableSize;

    /**
     * \brief   Current Block List. 
     *          By default the size of every block list is MAP_DEFAULT_BLOCK_SIZE (48)
     *          and cannot be more than MAP_MAX_BLOCK_SIZE (1024).
     *          Block List is created every time when List of Free Blocks is empty
     **/
    Block *   mBlockList;
    /**
     * \brief   The size of single Block List. By default it is MAP_DEFAULT_BLOCK_SIZE (48)
     *          and cannot be more than MAP_MAX_BLOCK_SIZE (1024).
     **/
    int         mBlockSize;

    /**
     * \brief   Number of elements in Hash Map
     **/
    int         mElemCount;

    /**
     * \brief   List of Free Blocks
     **/
    Block *     mFreeList;
    /**
     * \brief   Instance of object that copares keys and values.
     **/
    Implement   mImplement;
};

//////////////////////////////////////////////////////////////////////////
// Function Implement
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::Block class Implement
//////////////////////////////////////////////////////////////////////////

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::Block::Block( void )
    : TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>( )
    , mNext ( nullptr )
    , mHash (0)
{
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::Block::Block(unsigned int hash, KEY_TYPE key, VALUE_TYPE value)
    : TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>(key, value)
    , mNext ( nullptr )
    , mHash (hash)
{
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::Block::Block( TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::Block && src ) noexcept
    : TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>( static_cast<TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> &&>(src) )
    , mNext ( src.mNext )
    , mHash ( src.mHash )
{
    src.mNext   = nullptr;
    src.mHash   = NECommon::MAP_INVALID_HASH;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::Block::~Block( void )
{
}

//////////////////////////////////////////////////////////////////////////
// TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> class template Implement
//////////////////////////////////////////////////////////////////////////
template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEHashMap( void )
    : mHashTable    ( nullptr )
    , mHashTableSize(0)
    , mBlockList    ( nullptr )
    , mBlockSize    ( NECommon::MAP_DEFAULT_BLOCK_SIZE)
    , mElemCount    (0)
    , mFreeList     ( nullptr )
    , mImplement    ( )
{
    initHashTable( NECommon::MAP_DEFAULT_HASH_SIZE);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEHashMap(int blockSize, int hashSize)
    : mHashTable    ( nullptr )
    , mHashTableSize(0)
    , mBlockList    ( nullptr )
    , mBlockSize    (blockSize > 0 && blockSize <= NECommon::MAP_MAX_BLOCK_SIZE ? blockSize : (blockSize < 0 ? NECommon::MAP_DEFAULT_BLOCK_SIZE : NECommon::MAP_MAX_BLOCK_SIZE))
    , mElemCount    (0)
    , mFreeList     ( nullptr )
    , mImplement    ( )
{
    initHashTable(hashSize > 0 && hashSize <= NECommon::MAP_MAX_TABLE_SIZE ? hashSize : hashSize < 0 ? NECommon::MAP_DEFAULT_HASH_SIZE : NECommon::MAP_MAX_TABLE_SIZE);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEHashMap(const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& src)
    : mHashTable    ( nullptr )
    , mHashTableSize(0)
    , mBlockList    ( nullptr )
    , mBlockSize    (src.mBlockSize)
    , mElemCount    (0)
    , mFreeList     ( nullptr )
    , mImplement    ( )
{
    initHashTable(src.mHashTableSize);
    MAPPOS pos = src.firstPosition();
    KEY key;
    VALUE value;
    while (pos != NECommon::INVALID_POSITION)
    {
        pos = src.nextPosition(pos, key, value);
        setAt(key, value, false);
    }
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::TEHashMap( TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src ) noexcept
    : mHashTable    ( src.mHashTable )
    , mHashTableSize( src.mHashTableSize )
    , mBlockList    ( src.mBlockList )
    , mBlockSize    ( src.mBlockSize )
    , mElemCount    ( src.mElemCount )
    , mFreeList     ( src.mFreeList  )
    , mImplement    ( )
{
    src.mBlockList      = nullptr;
    src.mBlockSize      = 0;
    src.mElemCount      = 0;
    src.mFreeList       = nullptr;

    src.mHashTable	    = DEBUG_NEW Block *[src.mHashTableSize];
    NEMemory::zeroElements<Block *>( mHashTable, mHashTableSize );
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::~TEHashMap( void )
{
    removeAll();
    if (mHashTable != nullptr )
        delete [] mHashTable;

    mHashTable      = nullptr;
    mHashTableSize  = 0;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator = ( const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & src )
{
    if (static_cast<const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> *>(this) != static_cast<const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> *>(&src))
    {
        removeAll();
        mBlockSize  = src.mBlockSize;
        initHashTable(src.mHashTableSize);

        MAPPOS pos = src.firstPosition();
        KEY key;
        VALUE value;

        while (pos != NECommon::INVALID_POSITION)
        {
            pos = src.nextPosition(pos, key, value);
            setAt(key, value, false);
        }
    }
    return (*this);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator = ( TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> && src ) noexcept
{
    if ( static_cast<const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> *>(this) != static_cast<const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> *>(&src) )
    {
        removeAll( );
        if ( mHashTable != nullptr )
            delete [] mHashTable;

        mHashTable    = src.mHashTable;
        mHashTableSize= src.mHashTableSize;
        mBlockList    = src.mBlockList;
        mBlockSize    = src.mBlockSize;
        mElemCount    = src.mElemCount;
        mFreeList     = src.mFreeList;

        src.mHashTable      = nullptr;
        src.mHashTableSize  = 0;
        src.mBlockList      = nullptr;
        src.mBlockSize      = 0;
        src.mElemCount      = 0;
        src.mFreeList       = nullptr;
    }

    return (*this);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
bool TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator == (const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& other) const
{
    bool result = true;
    if ( this != &other )
    {
        result = false;
        if ( other.mElemCount == mElemCount )
        {
            result = true;
            KEY otherKey;
            VALUE otherValue, thisValue;

            for ( MAPPOS otherPos = other.firstPosition( ); result && (otherPos != NECommon::INVALID_POSITION); )
            {
                otherPos = other.nextPosition( otherPos, otherKey, otherValue );
                result = find( otherKey, thisValue ) ? isEqualValues( otherValue, thisValue ) : false;
            }
        }
    }
    return result;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
bool TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator != ( const TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>& other ) const
{
    bool result = false;
    if (this != &other )
    {
        result = true;
        if (other.mElemCount == mElemCount)
        {
            KEY otherKey;
            VALUE otherValue, thisValue;

            for ( MAPPOS otherPos = other.firstPosition(); result && (otherPos != NECommon::INVALID_POSITION); )
            {
                otherPos = other.nextPosition(otherPos, otherKey, otherValue);
                result = find(otherKey, thisValue) ? isEqualValues( otherValue, thisValue ) == false : true;
            }
        }
    }

    return result;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
VALUE & TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator [] (KEY_TYPE Key)
{
    return getAt(Key);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline VALUE_TYPE TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::operator [] ( KEY_TYPE Key ) const
{
    return getAt(Key);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline int TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::getSize( void ) const
{
    return mElemCount;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline int TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::getTableSize( void ) const
{
    return mHashTableSize;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::isEmpty( void ) const
{
    return (mElemCount == 0);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::firstPosition( void ) const
{
    return (mElemCount != 0 ? NECommon::START_POSITION : nullptr);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::find( KEY_TYPE Key, VALUE & out_Value ) const
{
    bool result     = false;
    Block **block = blockAtRef(Key);
    if ( (block != nullptr) && (*block != nullptr) )
    {
        out_Value   = (*block)->mValue;
        result      = true;
    }
    
    return result;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::find(KEY_TYPE Key) const
{
    Block** result = blockAtRef(Key);
    return (result != nullptr ? static_cast<MAPPOS>(*result) : nullptr);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::setAt(KEY_TYPE Key, VALUE_TYPE newValue, bool searchBeforeInsert /*= true*/)
{
    ASSERT(mHashTable != nullptr);

    unsigned int hash = NECommon::MAP_INVALID_HASH;
    Block* block = nullptr; 
    
    if ( searchBeforeInsert )
    {
        block = blockAt(Key, hash);
    }
    else
    {
        hash = getHashKey(Key);
    }

    if (block == nullptr)
    {
        // it doesn't exist, add a new Block
        unsigned int size = static_cast<unsigned int>(mHashTableSize);
        unsigned int idx  = hash % size;
        block       = initNewBlock();
        block->mHash= hash;
        block->mKey	= Key;
        // 'block->mValue' is a constructed object, nothing more put into hash table
        block->mNext= mHashTable[idx];
        mHashTable[idx]= block;
    }

    block->mValue = newValue;
    return static_cast<MAPPOS>(block);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::setAt(const TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> &newItem, bool searchBeforeInsert /*= true*/)
{
    return setAt(newItem.mKey, newItem.mValue, searchBeforeInsert);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::updateAt(KEY_TYPE Key, VALUE_TYPE newValue)
{
    ASSERT(mHashTable != nullptr );

    unsigned int hash = NECommon::MAP_INVALID_HASH;
    Block* block    = blockAt(Key, hash);
    if (block != nullptr)
    {
        // Block exists, update existing key value.
        block->mValue = newValue;
    }

    return static_cast<MAPPOS>(block);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
bool TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::removeAt(KEY_TYPE Key)
{
    bool result = false;
    Block** block = blockAtRef(Key);
    if ((block != nullptr) && (*block != nullptr))
    {
        result = true;
        removeBlock(block);
    }

    return result;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
bool TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::removeAt(KEY_TYPE Key, VALUE& out_Value)
{
    bool result = false;
    Block** block = blockAtRef(Key);
    if ((block != nullptr) && (*block != nullptr))
    {
        result      = true;
        out_Value   = (*block)->mValue;
        removeBlock(block);
    }

    return result;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::setPosition(MAPPOS atPosition, VALUE_TYPE newValue)
{
    ASSERT( mHashTable != nullptr );
    ASSERT( atPosition != nullptr );

    Block* block      = blockAt(atPosition);
    Block* nextBlock  = nextValidBlock(block);
    block->mValue     = newValue;
    return static_cast<MAPPOS>(nextBlock);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::removePosition(MAPPOS curPos, KEY& out_Key, VALUE& out_Value)
{
    ASSERT( mHashTable != nullptr );
    ASSERT( curPos     != nullptr );
    MAPPOS   result = nullptr;
    Block* block    = blockAt(curPos);
    Block* nextBlock= nextValidBlock(block);
    result      = static_cast<MAPPOS>(nextBlock);
    out_Key		= block->mKey;
    out_Value	= block->mValue;

    ASSERT(block != nullptr);
    removeBlock(blockReference(*block));

    return result;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
VALUE TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::removePosition(MAPPOS atPosition)
{
    ASSERT(mHashTable != nullptr);
    ASSERT(atPosition != nullptr);

    VALUE result;
    Block* block = blockAt(atPosition);
    result = block->mValue;
    
    ASSERT(block != nullptr);
    removeBlock(blockReference(*block));
    return result;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
void TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::removeAll( void )
{
    Block	*block = nullptr;
    for (int idx = 0; idx < mHashTableSize; ++ idx)
    {
        for (block = mHashTable[idx]; block != nullptr; block = block->mNext)
        {
            (&block->mKey)->~KEY();
            (&block->mValue)->~VALUE();
        }
    }

    deleteAllBlocks();
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::nextPosition(MAPPOS atPosition, KEY& out_Key, VALUE& out_Value) const
{
    ASSERT(mHashTable != nullptr);
    ASSERT(atPosition != nullptr);

    Block* block      = blockAt(atPosition);
    Block* nextBlock  = nextValidBlock(block);
    out_Key		      = block->mKey;
    out_Value	      = block->mValue;

    return static_cast<MAPPOS>(nextBlock);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::nextPosition(MAPPOS atPosition, TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & out_Element) const
{
    return nextPosition(atPosition, out_Element.mKey, out_Element.mValue);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline MAPPOS TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::nextPosition( MAPPOS atPosition ) const
{
    ASSERT(mHashTable != nullptr);
    ASSERT(atPosition != nullptr);
    Block* block      = blockAt(atPosition);
    return static_cast<MAPPOS>( nextValidBlock(block) );
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline VALUE& TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::getPosition( const MAPPOS atPosition )
{
    ASSERT( mHashTable != nullptr );
    ASSERT( atPosition != nullptr );

    Block* block = blockAt( atPosition );
    return block->mValue;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline VALUE_TYPE TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::getPosition( const MAPPOS atPosition ) const
{
    ASSERT( mHashTable != nullptr );
    ASSERT( atPosition != nullptr );

    Block* block = blockAt( atPosition );
    return block->mValue;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
VALUE& TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::getAt( KEY_TYPE Key )
{
    ASSERT( mHashTable != nullptr );

    unsigned int hash   = NECommon::MAP_INVALID_HASH;
    Block *list = blockAt( Key, hash );
    if ( list == nullptr )
    {
        // add new list
        int idx     = static_cast<int>(hash % mHashTableSize);
        list        = initNewBlock( );
        list->mHash = hash;
        list->mKey	= Key;
        list->mNext	= mHashTable[idx];
        mHashTable[idx] = list;
    }

    return list->mValue;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline VALUE_TYPE TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::getAt(KEY_TYPE Key) const
{
    Block ** block  = blockAtRef(Key);
    ASSERT((block != nullptr) && (*block != nullptr));
    return (*block)->mValue;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::getAtPosition(MAPPOS atPosition, KEY & out_Key, VALUE & out_Value) const
{
    ASSERT(mHashTable != nullptr);
    ASSERT(atPosition != nullptr);

    Block* block    = blockAt(atPosition);
    out_Key   = block->mKey;
    out_Value= block->mValue;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::getAtPosition(MAPPOS atPosition, TEPair<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> & out_Element) const
{
    getAtPosition(atPosition, out_Element.mKey, out_Element.mValue);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline VALUE_TYPE TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::valueAtPosition( MAPPOS atPosition ) const
{
    ASSERT( mHashTable != nullptr );
    ASSERT( atPosition != nullptr );

    Block* block    = blockAt(atPosition);
    return block->mValue;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline KEY_TYPE TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::keyAtPosition( MAPPOS atPosition ) const
{
    ASSERT( mHashTable != nullptr );
    ASSERT( atPosition != nullptr );

    Block* block    = blockAt(atPosition);
    return block->mKey;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
bool TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::nextEntry(MAPPOS & in_out_NextPosition, KEY & out_NextKey, VALUE & out_NextValue) const
{
    ASSERT( mHashTable != nullptr );
    ASSERT( in_out_NextPosition != nullptr );

    bool result     = false;
    Block* block    = blockAt(in_out_NextPosition);
    Block* nextBlock= nextValidBlock(block);
    in_out_NextPosition = static_cast<MAPPOS>(nextBlock);
    if ( nextBlock != nullptr )
    {
        out_NextKey     = nextBlock->mKey;
        out_NextValue   = nextBlock->mValue;
        result          = true;
    }

    return result;
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
void TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::initHashTable(int sizeHashTable /*= NECommon::MAP_DEFAULT_HASH_SIZE*/)
{
    ASSERT(mElemCount   == 0);
    ASSERT(sizeHashTable >  0 && sizeHashTable <= NECommon::MAP_MAX_TABLE_SIZE);
    if (mHashTable != nullptr)
        delete [] mHashTable;

    mHashTable	    = DEBUG_NEW Block *[static_cast<unsigned int>(sizeHashTable)];
    mHashTableSize	= mHashTable != nullptr ? sizeHashTable : 0;
    NEMemory::zeroElements<Block *>( mHashTable, mHashTableSize);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
void TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::createBlockList( void )
{
    // add another block
    // chain them into free list
    unsigned int length     = static_cast<unsigned int>(mBlockSize * sizeof(Block) + sizeof(Block *));
    unsigned char* newBlock = DEBUG_NEW unsigned char[length];
    if (newBlock != nullptr)
    {
        // link blocks, copy the address of last block into
        // beginning of new block (and skip address part)
        NEMemory::memCopy(newBlock, static_cast<int>(length), &mBlockList, static_cast<int>(sizeof(Block *)));
        mBlockList   = reinterpret_cast<Block *>(newBlock);
        newBlock    += sizeof(Block *);
        Block* block = reinterpret_cast<Block *>(newBlock);
        block       += mBlockSize - 1;
        for (int i = mBlockSize; i > 0; -- i, -- block )
        {
            block->mNext= mFreeList;
            mFreeList   = block;
        }
    }
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
void TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::deleteBlockList( void )
{
    unsigned char* blocks = reinterpret_cast<unsigned char *>(mBlockList);
    while (blocks != nullptr)
    {
        // copy address of next block, placed during creating blocks.
        NEMemory::memCopy(&mBlockList, sizeof(Block *), blocks, sizeof(Block *));
        delete [] blocks;
        blocks = reinterpret_cast<unsigned char *>(mBlockList);
    }
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
void TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::freeBlock(Block* block)
{
    (&block->mKey)->~KEY();
    (&block->mValue)->~VALUE();
    block->mHash = 0;
    block->mNext = mFreeList;
    mFreeList = block;
    mElemCount --;
    ASSERT(mElemCount >= 0);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */ >
inline unsigned int TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::getHashKey( KEY_TYPE Key ) const
{
    return mImplement.implHashKey(Key);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */>
inline bool TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::isEqualKeys(KEY_TYPE key1, KEY_TYPE key2) const
{
    return mImplement.implEqualKeys(key1, key2);
}

template < typename KEY, typename VALUE, typename KEY_TYPE /*= KEY*/, typename VALUE_TYPE /*= VALUE */, class Implement /* = HashMapBase */>
inline bool TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement>::isEqualValues(VALUE_TYPE value1, VALUE_TYPE value2) const
{
    return mImplement.implEqualValues(value1, value2);
}

//////////////////////////////////////////////////////////////////////////
// TEHashMap<KEY, VALUE, KEY_TYPE, VALUE_TYPE, Implement> class friend methods
//////////////////////////////////////////////////////////////////////////

template < typename K, typename V, typename KT, typename VT, class Impl >
const IEInStream & operator >> ( const IEInStream & stream, TEHashMap<K, V, KT, VT, Impl> & input )
{
    int size = 0;
    stream >> size;
    for (int i = 0; i < size; ++ i)
    {
        TEPair<K, V, KT, VT, Impl> mapItem;
        stream >> mapItem;
        input.setAt(mapItem, false);
    }
    return stream;
}

template < typename K, typename V, typename KT, typename VT, class Impl>
IEOutStream & operator << ( IEOutStream & stream, const TEHashMap<K, V, KT, VT, Impl> & output )
{
    int size = output.getSize();
    stream << size;
    if ( size != 0 )
    {
        const typename TEHashMap<K, V, KT, VT, Impl>::Block * block = output.firstValidBlock();
        for ( ; block != nullptr; block = output.nextValidBlock(block))
            stream << static_cast<const TEPair<K, V, KT, VT, Impl> &>(*block);
    }

    return stream;
}
