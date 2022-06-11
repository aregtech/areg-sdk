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

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////
// TEHashMap<KEY, VALUE, Implement> class template declaration
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
 *          firstPosition()). Each next element is accessed by calling next position.
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
template < typename KEY, typename VALUE, class Implement = TEHashMapImpl<KEY, VALUE> >
class TEHashMap : protected Constless<std::unordered_map<KEY, VALUE>>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    //! Position in the hash map
    using MAPPOS    = typename std::unordered_map<KEY, VALUE>::iterator;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Constructor, initialization
     * \param	hashSize	The size of has map table. 
     *                      If it is negative, the size is MAP_DEFAULT_HASH_SIZE (64).
     *                      It cannot be more than MAP_MAX_TABLE_SIZE (1024)
     **/
    TEHashMap( uint32_t hashSize = NECommon::MAP_DEFAULT_HASH_SIZE);

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    TEHashMap( const TEHashMap<KEY, VALUE, Implement> & src );

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    TEHashMap( TEHashMap<KEY, VALUE, Implement> && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~TEHashMap( void ) = default;

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
    inline TEHashMap<KEY, VALUE, Implement>& operator = ( const TEHashMap<KEY, VALUE, Implement> & src );

    /**
     * \brief	Move operator. It moves all elements from source map
     **/
    inline TEHashMap<KEY, VALUE, Implement>& operator = ( TEHashMap<KEY, VALUE, Implement> && src ) noexcept;

    /**
     * \brief   Checks equality of 2 hash-map objects, and returns true if they are equal.
     *          There should be possible to compare KEY and VALUE types of hash map.
     * \param   other   The hash-map object to compare
     **/
    inline bool operator == ( const TEHashMap<KEY, VALUE, Implement> & other ) const;

    /**
     * \brief   Checks inequality of 2 hash-map objects, and returns true if they are not equal.
     *          There should be possible to compare KEY and VALUE types of hash map.
     * \param   other   The hash-map object to compare
     **/
    inline bool operator != ( const TEHashMap<KEY, VALUE, Implement> & other ) const;

    /**
     * \brief	Subscript operator. Returns reference to value of element by given key.
     *          If the key does not exist, inserts an element into Hash Map with specified key value. 
     *          Used on the left of an assignment statement.
     **/
    inline VALUE & operator [] ( const KEY & Key );

    /**
     * \brief	Subscript operator. Returns value of element by given key.
     *          If the key does not exist, inserts an element into Hash Map with specified key value.
     *          Modification of value is not possible.
     **/
    inline const VALUE & operator [] (const KEY& Key ) const;

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
    template < typename KEY, typename VALUE, class Implement >
    friend const IEInStream & operator >> ( const IEInStream & stream, TEHashMap<KEY, VALUE, Implement> & input);
    /**
     * \brief   Writes to the stream Hash Map values.
     *          The Elements of Hash Map will be written to the stream starting from start position.
     *          There should be possibility to stream every Element of Hash Map and if KEY and VALUE 
     *          are not primitives, but objects, they should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Hash Map object to read out values.
     **/
    template < typename KEY, typename VALUE, class Implement >
    friend IEOutStream & operator << ( IEOutStream & stream, const TEHashMap<KEY, VALUE, Implement> & output );

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
    inline uint32_t getSize( void ) const;

    /**
     * \brief	Returns the size of hash table
     **/
    inline uint32_t getTableSize( void ) const;

    /**
     * \brief	Returns the first position of the hash map, which is not invalid if
     *          the hash map is not empty. Otherwise, it is equal to the last position
     *          and not valid.
     **/
    inline MAPPOS firstPosition( void ) const;

    /**
     * \brief	Returns the last position of the hash map, which is always invalid.
     **/
    inline MAPPOS lastPosition(void) const;

    /**
     * \brief   Returns true if specified position reached the end of the hash map.
     * \param   pos     The position to check.
     * \return  Returns true if specified position reached the end of the hash map.
     **/
    inline bool isEndPosition(const MAPPOS & pos) const;

    /**
     * \brief   Returns true if specified position pointing start of the hash map.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the hash map.
     **/
    inline bool isStartPosition(const MAPPOS& pos) const;

    /**
     * \brief   Returns true if specified position is not pointing the end of the hash map.
     *          NOTE:   This method does not ensure that the position object is pointing
     *                  to the valid entry in the hash map. To do so, use 'checkPos' method.
     * \param pos       The position to check.
     * \return Returns true if specified position is not pointing the end of the hash map.
     */
    inline bool isValidPosition(const MAPPOS& pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the hash map.
     *          The duration of checkup depends on the location of the position in the map and may run long.
     * \param pos       The position to check.
     * \return  Returns true if specified position points to the valid entry in the hash map.
     */
    inline bool checkPosition(const MAPPOS& pos) const;

/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief	Searches element by given key. 
     *          If found element, return true and on exit returns the value of element
     * \param	Key	        The key to search.
     * \param	out_Value   On output, contains value of found element
     * \return	Returns true if finds element with specified key.
     **/
    inline bool find( const KEY & Key, VALUE & out_Value ) const;

    /**
     * \brief	Search an element by given key and returns position.
     *          If could not find element, returns end-of-map position.
     *          The values of position can be modified carefully that
     *          the hash-value of the key is not changed.
     * \param	Key	    The key to search.
     * \return	If finds, return position in Hash Map, otherwise returns end-of-map.
     **/
    inline MAPPOS find(const KEY& Key) const;

    /**
     * \brief   Returns true if specified key exists in the hash map.
     * \param   Key     The key to check.
     * \return  Returns true if key exists.
     */
    inline bool contains(const KEY& Key) const;

    /**
     * \brief	Update existing element value or inserts new element in the Hash Map.
     * \param	Key	                The key of element to search or to create
     * \param	newValue	        The value of element to set or insert.
     **/
    inline void setAt( const KEY & Key, const VALUE & newValue );
    inline void setAt( KEY && Key, const VALUE & newValue);
    /**
     * \brief	Update existing element value or inserts new element in the Hash Map.
     * \param	newElement	        The Key and Value pair of element to set or insert.
     **/
    inline void setAt( const TEPair<KEY, VALUE, Implement> & newElement );
    inline void setAt( TEPair<KEY, VALUE, Implement> && newElement);

    /**
     * \brief   Extracts elements from the given source and inserts into the hash map.
     *          If there is an entry with the key equivalent to the key from source element,
     *          then that element is not extracted from the source and remains unchanged.
     * \param   source  The source of hash map to merge.
     */
    inline void merge( const TEHashMap<KEY, VALUE, Implement> & source );
    inline void merge( TEHashMap<KEY, VALUE, Implement> && source );

    /**
     * \brief   Adds new entry with the specified key in the hash map if it is not existing.
     *          If the entry with specified key exists, the entry is not added.
     *          The method returns pair of value, where it indicates the position of the entry
     *          and the flag, indicating whether it added new entry or not.
     *          In 'std::pair<MAPPOS, bool>' the 'MAPPOS' indicates the position of the entry in
     *          the hash map, and 'bool' equal to 'true' indicates that the new entry is created.
     * \param   Key     The key of the entry in the hash map.
     * \param   Value   The value of the entry in the hash map.
     * \return  Returns a pair of 'MAPPOS' and 'bool' values, where
     *              -   'MAPPOS' indicates the position of the entry in the hash map.
     *              -   'bool' equal to 'true' indicates that new entry is created.
     *                  If this value is 'false' no new entry is created.
     */
    inline std::pair<MAPPOS, bool> addNew(const KEY & newKey, const VALUE & newValue);
    inline std::pair<MAPPOS, bool> addNew(KEY && newKey, VALUE && newValue);

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
    inline MAPPOS updateAt( const KEY & Key, const VALUE & newValue );

    /**
     * \brief	Remove existing key and if key exists, returns true.
     * \param	Key	    The Key to search and remove
     * \return	Return true if key successfully removed from hash map
     **/
    inline bool removeAt(const KEY& Key );

    /**
     * \brief	Remove existing key and if succeeded, on output contains value of removed element
     * \param	Key	            The key to remove from Hash Map
     * \param	out_Value       If removed with success, on output it contains value of removed element
     * \return	Return true if successfully removed key. If key does not exist, return false.
     **/
    inline bool removeAt( const KEY & Key, VALUE & out_Value );

    /**
     * \brief	Update value of an element by given position and return position to next element
     * \param	atPosition      Position of element to update value
     * \param	newValue	    New value to set for existing element.
     * \return  Returns position of next element. If given position was position of last valid element, it will return nullptr.
     **/
    inline MAPPOS setPosition(typename MAPPOS atPosition, const VALUE& newValue );

    /**
     * \brief	Removes element by given position, and on output key and value 
     *          parameters contain value of remove element.
     *          The function returns next position of element in hash map.
     * \param	curPos      This should contain valid position of hash map.
     * \param	out_Key     On output, this contains key of removed element
     * \param	out_Value   On output, this contains value of removed element.
     * \return  Next position in hash map or invalid position if reached the end.
     **/
    inline MAPPOS removePosition(typename MAPPOS curPos, KEY & out_Key, VALUE & out_Value );

    /**
     * \brief	Removes element by given valid position and returns value of removed element
     * \param	atPosition	Valid position of element in hash map
     * \return  Next position in hash map or invalid position if reached the end.
     **/
    inline MAPPOS removePosition(typename MAPPOS atPosition );

    /**
     * \brief   Removes the first entry in the hash map. On output, the
     *          'out_Key' and 'out_Value' contain the entries that are removed.
     * \param   out_Key     On output it contains the key of the fist element in hash-map, which was removed.
     * \param   out_Value   On output it contains the value of the fist element in hash-map, which was removed.
     * \return  Returns true if hash-map was not empty and first element is removed.
     *          Returns false if hash-map is empty, so that it could not remove first entry.
     **/
    inline bool removeFirst(KEY& out_Key, VALUE& out_Value);

    /**
     * \brief   Removes the first element in the hash map and returns true if removed with success.
     * \return  Returns true if could remove first element in the hash map.
     **/
    inline bool removeFirst( void );

    /**
     * \brief   Removes the last entry in the hash map. On output, the
     *          'out_Key' and 'out_Value' contain the entries that are removed.
     * \param   out_Key     On output it contains the key of the last element in hash-map, which was removed.
     * \param   out_Value   On output it contains the value of the last element in hash-map, which was removed.
     * \return  Returns true if hash-map was not empty and last element is removed.
     *          Returns false if hash-map is empty, so that it could not remove last entry.
     **/
    inline bool removeLast(KEY& out_Key, VALUE& out_Value);

    /**
     * \brief   Removes the first element in the hash map and returns true if removed with success.
     * \return  Returns true if could remove first element in the hash map.
     **/
    inline bool removeLast(void);

    /**
     * \brief   Removes all elements in hash map, sets size zero
     **/
    inline void removeAll( void );

    /**
     * \brief	By given position value, retrieves key and value of element, and returns next position
     * \param	atPosition  Starting Position to get next position of element, and retrieve value and key
     * \param	out_Key     On output, this contains key of given position
     * \param	out_Value   On output, this contains value of given position
     * \return	Next position of element or next if it is last element in hash map.
     **/
    inline MAPPOS nextPosition(typename MAPPOS  atPosition, KEY & out_Key, VALUE & out_Value ) const;
    /**
     * \brief	By given position value, retrieves key and value pair of element, and returns next position
     * \param	atPosition  Starting Position to get next position of element, and retrieve value and key pair
     * \param	out_Element On output, this element contains pair of Key and Value
     * \return	Next position of element or next if it is last element in hash map.
     **/
    inline MAPPOS nextPosition(typename MAPPOS atPosition, TEPair<KEY, VALUE, Implement> & out_Element ) const;
    /**
     * \brief	By given position value, returns next position
     * \param	atPosition  Starting Position to get next position of element
     * \return	Next position of element or next if it is last element in hash map.
     **/
    inline MAPPOS nextPosition(typename MAPPOS atPosition ) const;
    /**
     * \brief	Returns reference to value of element by given key.
     *          If the key does not exist, inserts an element into Hash Map with specified key value.
     **/
    inline VALUE & getAt(const KEY & Key);
    /**
     * \brief	Finds element by given Key and returns Value. The key should exist in hash map.
     *          If not element with give key found, assertion raises.
     * \param	Key	    The Key to search
     * \return	Value of element
     **/
    inline const VALUE & getAt( const KEY & Key ) const;

    /**
     * \brief	Retrieves key and value of element by given position
     * \param	atPosition	The position of element to retrieve key and value
     * \param	out_Key	    On output, contains key of element of given position
     * \param	out_Value   On output, contains value of element of given position
     **/
    inline void getAtPosition(typename MAPPOS atPosition, KEY & out_Key, VALUE & out_Value ) const;
    /**
     * \brief	Retrieves key and value pair of element by given position
     * \param	atPosition	The position of element to retrieve key and value
     * \param	out_Element On output, contains Key and Value pair of element of given position
     **/
    inline void getAtPosition(typename MAPPOS atPosition, TEPair<KEY, VALUE, Implement> & out_Element) const;

    /**
     * \brief   Returns the Key object value at the given position.
     *          The Key should be used only for reading. Avoid modifications of Key value.
     * \param   atPosition  The position to return Key object value.
     **/
    inline const KEY & keyAtPosition(typename MAPPOS atPosition ) const;

    /**
     * \brief   Returns the Key object value at the given position, which can be modified.
     *          Modification of the Key after return may harm hash-map if the update
     *          can change hash value. Use this method carefully.
     * \param   atPosition  The position to return Key object value.
     **/
    inline KEY& keyAtPosition(typename MAPPOS atPosition);

    /**
     * \brief   Returns the Value object value at the given position,
     *          which can be used only for reading data.
     * \param   atPosition  The position to return Value object value.
     **/
    inline const VALUE & valueAtPosition(typename MAPPOS atPosition ) const;

    /**
     * \brief   Returns the Value object value at the given position,
     *          which can be used to modify data.
     * \param   atPosition  The position to return Value object value.
     **/
    inline VALUE& valueAtPosition(typename MAPPOS atPosition);

    /**
     * \brief	Gets next element in hash map by given valid position and on output, 
     *          returns position, key and value of next element
     * \param	in_out_NextPosition	On input this should be valid position,
     *                              on output, this contain position to next element in hash map
     * \param	out_NextKey	        On output, this contains key of next element in hash map
     * \param	out_NextValue       On output, this contain value of next element in hash map
     * \return	Returns true, if next element was found and values on output are valid.
     **/
    inline bool nextEntry(typename MAPPOS & in_out_NextPosition, KEY & out_NextKey, VALUE & out_NextValue ) const;

//////////////////////////////////////////////////////////////////////////
// Member Variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Instance of map to store key-value pairs.
     **/
    std::unordered_map<KEY, VALUE>  mValueList;
    /**
     * \brief   Instance of object that compares keys and values.
     **/
    Implement                       mImplement;
};

//////////////////////////////////////////////////////////////////////////
// Function Implement
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEHashMap<KEY, VALUE, Implement> class template Implement
//////////////////////////////////////////////////////////////////////////

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, Implement>::TEHashMap(uint32_t hashSize /* = NECommon::MAP_DEFAULT_HASH_SIZE */)
    : Constless ( )
    , mImplement( )
    , mValueList(hashSize)
{
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, Implement>::TEHashMap(const TEHashMap<KEY, VALUE, Implement>& src)
    : Constless ( )
    , mImplement( )
    , mValueList( src.mValueList )
{
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
TEHashMap<KEY, VALUE, Implement>::TEHashMap( TEHashMap<KEY, VALUE, Implement> && src ) noexcept
    : Constless ( )
    , mImplement( )
    , mValueList(std::move(src.mValueList))
{
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline TEHashMap<KEY, VALUE, Implement>& TEHashMap<KEY, VALUE, Implement>::operator = ( const TEHashMap<KEY, VALUE, Implement> & src )
{
    mValueList = src.mValueList;
    return (*this);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline TEHashMap<KEY, VALUE, Implement>& TEHashMap<KEY, VALUE, Implement>::operator = ( TEHashMap<KEY, VALUE, Implement> && src ) noexcept
{
    mValueList = std::move(src.mValueList);
    return (*this);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
bool TEHashMap<KEY, VALUE, Implement>::operator == (const TEHashMap<KEY, VALUE, Implement>& other) const
{
    return (mValueList == other.mValueList);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
bool TEHashMap<KEY, VALUE, Implement>::operator != ( const TEHashMap<KEY, VALUE, Implement>& other ) const
{
    return (mValueList != other.mValueList);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline VALUE & TEHashMap<KEY, VALUE, Implement>::operator [] (const KEY& Key)
{
    return mValueList[Key];
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline const VALUE & TEHashMap<KEY, VALUE, Implement>::operator [] ( const KEY & Key ) const
{
    return mValueList[Key];
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::isEmpty(void) const
{
    return mValueList.empty();
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline uint32_t TEHashMap<KEY, VALUE, Implement>::getSize( void ) const
{
    return static_cast<uint32_t>(mValueList.size());
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline uint32_t TEHashMap<KEY, VALUE, Implement>::getTableSize( void ) const
{
    return static_cast<uint32_t>(mValueList.bucket_count());
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::firstPosition( void ) const
{
    typename std::unordered_map<KEY, VALUE>::const_iterator pos = mValueList.begin();
    return Constless::iter(mValueList, pos);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::lastPosition(void) const
{
    typename std::unordered_map<KEY, VALUE>::const_iterator pos = mValueList.end();
    return Constless::iter(mValueList, pos);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::isEndPosition(const MAPPOS& pos) const
{
    return (pos == mValueList.end());
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::isStartPosition(const MAPPOS& pos) const
{
    return (pos == mValueList.begin());
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::isValidPosition(const MAPPOS& pos) const
{
    return (pos != mValueList.end());
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::checkPosition(const MAPPOS& pos) const
{
    bool result = false;
    typename std::unordered_map<KEY, VALUE>::const_iterator it = mValueList.begin();

    for ( ; it != mValueList.end(); ++it)
    {
        if (it == pos)
        {
            result = true;
            break;
        }
    }

    return result;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::find( const KEY & Key, VALUE & out_Value ) const
{
    bool result = false;
    auto pos = mValueList.find(Key);
    if (pos != mValueList.end())
    {
        out_Value = pos->second;
        result = true;
    }
    
    return result;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::find(const KEY& Key) const
{
    typename std::unordered_map<KEY, VALUE>::const_iterator cit = mValueList.find(Key);
    return iter(mValueList, cit);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::contains(const KEY& Key) const
{
    return (mValueList.find(Key) != mValueList.end());
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, Implement>::setAt(const KEY & Key, const VALUE & newValue)
{
    mValueList[Key] = newValue;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, Implement>::setAt( KEY && Key, const VALUE & newValue)
{
    mValueList[Key] = newValue;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, Implement>::setAt(const TEPair<KEY, VALUE, Implement> &newItem)
{
    setAt(newItem.mKey, newItem.mValue);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, Implement>::setAt( TEPair<KEY, VALUE, Implement> && newItem)
{
    setAt(std::move(newItem.mKey), newItem.mValue);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, Implement>::merge(const TEHashMap<KEY, VALUE, Implement>& source)
{
    mValueList.merge(source);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, Implement>::merge(TEHashMap<KEY, VALUE, Implement> && source)
{
    mValueList.merge(source);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline std::pair<typename TEHashMap<KEY, VALUE, Implement>::MAPPOS, bool> TEHashMap<KEY, VALUE, Implement>::addNew(const KEY& newKey, const VALUE& newValue)
{
    return mValueList.insert({ newKey, newValue });
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline std::pair<typename TEHashMap<KEY, VALUE, Implement>::MAPPOS, bool> TEHashMap<KEY, VALUE, Implement>::addNew( KEY && newKey, VALUE && newValue)
{
    return mValueList.insert(std::make_pair(newKey, newValue));
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::updateAt(const KEY & Key, const VALUE & newValue)
{
    MAPPOS pos = mValueList.find(Key);
    if (pos != mValueList.end())
    {
        pos->second = newValue;
    }

    return pos;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
bool TEHashMap<KEY, VALUE, Implement>::removeAt(const KEY& Key)
{
    bool result = false;
    MAPPOS pos = mValueList.find(Key);
    if (pos != mValueList.end())
    {
        result = true;
        mValueList.erase(pos);
    }

    return result;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::removeAt(const KEY & Key, VALUE& out_Value)
{
    bool result = false;
    MAPPOS pos = mValueList.find(Key);
    if (pos != mValueList.end())
    {
        result = true;
        out_Value = pos->second;
        mValueList.erase(pos);
    }

    return result;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::setPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition, const VALUE & newValue)
{
    ASSERT( atPosition != mValueList.end() );
    atPosition->second = newValue;
    return (++atPosition);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::removePosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS curPos, KEY& out_Key, VALUE& out_Value)
{
    ASSERT( curPos != mValueList.end());
    MAPPOS result   = curPos + 1;
    out_Key         = curPos->first;
    out_Value       = curPos->second;

    return mValueList.erase(curPos);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::removePosition(MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::removeFirst(KEY& out_Key, VALUE& out_Value)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        auto pos    = mValueList.begin();
        ASSERT(pos != mValueList.end());
        out_Key = pos->first;
        out_Value = pos->second;
        
        mValueList.erase(pos);
        result      = true;
    }

    return result;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::removeFirst( void )
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.begin();
        ASSERT(pos != mValueList.end());
        mValueList.erase(pos);
        result = true;
    }

    return result;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::removeLast(KEY& out_Key, VALUE& out_Value)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.cbegin();
        ASSERT(pos != mValueList.end());
        out_Key = pos->first;
        out_Value = pos->second;

        mValueList.erase(pos);
        result = true;
    }

    return result;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::removeLast(void)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.cbegin();
        ASSERT(pos != mValueList.end());
        mValueList.erase(pos);
        result = true;
    }

    return result;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, Implement>::removeAll( void )
{
    mValueList.clear();
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::nextPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition, KEY& out_Key, VALUE& out_Value) const
{
    ASSERT(atPosition != mValueList.end());

    MAPPOS result   = atPosition + 1;
    out_Key		    = atPosition->first;
    out_Value	    = atPosition->second;

    return result;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::nextPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition, TEPair<KEY, VALUE, Implement> & out_Element) const
{
    return nextPosition(atPosition, out_Element.mKey, out_Element.mValue);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline typename TEHashMap<KEY, VALUE, Implement>::MAPPOS TEHashMap<KEY, VALUE, Implement>::nextPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return ++ atPosition;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline VALUE & TEHashMap<KEY, VALUE, Implement>::getAt( const KEY & Key )
{
    return mValueList.at(Key);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline const VALUE & TEHashMap<KEY, VALUE, Implement>::getAt(const KEY & Key) const
{
    return mValueList.at(Key);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, Implement>::getAtPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition, KEY & out_Key, VALUE & out_Value) const
{
    ASSERT(atPosition != mValueList.end());
    out_Key     = atPosition->first;
    out_Value   = atPosition->second;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline void TEHashMap<KEY, VALUE, Implement>::getAtPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition, TEPair<KEY, VALUE, Implement> & out_Element) const
{
    getAtPosition(atPosition, out_Element.mKey, out_Element.mValue);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline const KEY & TEHashMap<KEY, VALUE, Implement>::keyAtPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->first;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline KEY& TEHashMap<KEY, VALUE, Implement>::keyAtPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return const_cast<KEY &>(atPosition->first);
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline const VALUE & TEHashMap<KEY, VALUE, Implement>::valueAtPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline VALUE& TEHashMap<KEY, VALUE, Implement>::valueAtPosition(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE, class Implement /* = HashMapBase */ >
inline bool TEHashMap<KEY, VALUE, Implement>::nextEntry(typename TEHashMap<KEY, VALUE, Implement>::MAPPOS & in_out_NextPosition, KEY & out_NextKey, VALUE & out_NextValue) const
{
    ASSERT( in_out_NextPosition != mValueList.end() );
    bool result = false;
    if (++in_out_NextPosition != mValueList.end())
    {
        out_NextKey     = in_out_NextPosition->first;
        out_NextValue   = in_out_NextPosition->second;
        result = true;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// TEHashMap<KEY, VALUE, Implement> class friend methods
//////////////////////////////////////////////////////////////////////////

template < typename KEY, typename VALUE, class Implement >
const IEInStream & operator >> ( const IEInStream & stream, TEHashMap<KEY, VALUE, Implement> & input )
{
    uint32_t size = 0;
    stream >> size;
    
    input.mValueList.clear();
    input.mValueList.reserve(size);

    for (uint32_t i = 0; i < size; ++ i)
    {
        KEY key;
        VALUE value;
        stream >> key >> value;
        input.setAt(key, value);
    }

    return stream;
}

template < typename KEY, typename VALUE, class Implement >
IEOutStream & operator << ( IEOutStream & stream, const TEHashMap<KEY, VALUE, Implement> & output )
{
    uint32_t size = output.getSize();
    stream << size;
    if ( size != 0 )
    {
        for (const auto& elem : output.mValueList)
        {
            stream << elem.first;
            stream << elem.second;
        }
    }

    return stream;
}
