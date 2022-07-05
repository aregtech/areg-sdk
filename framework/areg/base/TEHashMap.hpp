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
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEMemory.hpp"

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////
// TEHashMap<KEY, VALUE> class template declaration
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
 **/
template < typename KEY, typename VALUE>
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
    TEHashMap( const TEHashMap<KEY, VALUE> & src ) = default;

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    TEHashMap( TEHashMap<KEY, VALUE> && src ) noexcept = default;

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
    TEHashMap<KEY, VALUE>& operator = ( const TEHashMap<KEY, VALUE> & src ) = default;

    /**
     * \brief	Move operator. It moves all elements from source map
     **/
    TEHashMap<KEY, VALUE>& operator = ( TEHashMap<KEY, VALUE> && src ) noexcept = default;

    /**
     * \brief   Checks equality of 2 hash-map objects, and returns true if they are equal.
     *          There should be possible to compare KEY and VALUE types of hash map.
     * \param   other   The hash-map object to compare
     **/
    inline bool operator == ( const TEHashMap<KEY, VALUE> & other ) const;

    /**
     * \brief   Checks inequality of 2 hash-map objects, and returns true if they are not equal.
     *          There should be possible to compare KEY and VALUE types of hash map.
     * \param   other   The hash-map object to compare
     **/
    inline bool operator != ( const TEHashMap<KEY, VALUE> & other ) const;

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
    template < typename KEY, typename VALUE >
    friend const IEInStream & operator >> ( const IEInStream & stream, TEHashMap<KEY, VALUE> & input);
    /**
     * \brief   Writes to the stream Hash Map values.
     *          The Elements of Hash Map will be written to the stream starting from start position.
     *          There should be possibility to stream every Element of Hash Map and if KEY and VALUE 
     *          are not primitives, but objects, they should have implemented streaming operator.
     * \param   stream  The streaming object to write values
     * \param   input   The Hash Map object to read out values.
     **/
    template < typename KEY, typename VALUE >
    friend IEOutStream & operator << ( IEOutStream & stream, const TEHashMap<KEY, VALUE> & output );

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
     * \brief   Returns true if specified position pointing start of the hash map.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the hash map.
     **/
    inline bool isStartPosition(const MAPPOS pos) const;

    /**
     * \brief   Returns true if specified position pointing start of the stack.
     * \param   pos     The position to check.
     * \return  Returns true if specified position pointing start of the stack.
     **/
    inline bool isLastPosition(const MAPPOS pos) const;

    /**
     * \brief   Returns invalid position object.
     **/
    inline MAPPOS invalidPosition(void) const;

    /**
     * \brief   Returns true if specified position reached the end of the hash map.
     * \param   pos     The position to check.
     * \return  Returns true if specified position reached the end of the hash map.
     **/
    inline bool isInvalidPosition(const MAPPOS pos) const;

    /**
     * \brief   Returns true if specified position is not pointing the end of the hash map.
     *          NOTE:   This method does not ensure that the position object is pointing
     *                  to the valid entry in the hash map. To do so, use 'checkPos' method.
     * \param pos       The position to check.
     * \return Returns true if specified position is not pointing the end of the hash map.
     */
    inline bool isValidPosition(const MAPPOS pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the hash map.
     *          The duration of checkup depends on the location of the position in the map.
     * \param pos       The position to check.
     * \return  Returns true if specified position points to the valid entry in the hash map.
     */
    inline bool checkPosition(const MAPPOS pos) const;

/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief   Remove all entries of the hash map.
     **/
    inline void clear(void);

    /**
     * \brief   Delete extra entries in the hash map.
     **/
    inline void freeExtra(void);

    /**
     * \brief   Sets the size of the hash map to zero and deletes all capacity space.
     */
    inline void release(void);

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
    inline void setAt( const std::pair<KEY, VALUE> & newElement );
    inline void setAt( std::pair<KEY, VALUE> && newElement);

    /**
     * \brief   Extracts elements from the given source and inserts into the hash map.
     *          If there is an entry with the key equivalent to the key from source element,
     *          then that element is not extracted from the source and remains unchanged.
     * \param   source  The source of hash map to merge.
     */
    inline void merge( const TEHashMap<KEY, VALUE> & source );
    inline void merge( TEHashMap<KEY, VALUE> && source );

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
    inline std::pair<MAPPOS, bool> addIfNew(const KEY & newKey, const VALUE & newValue);
    inline std::pair<MAPPOS, bool> addIfNew(KEY && newKey, VALUE && newValue);

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
    inline MAPPOS nextPosition(typename MAPPOS atPosition, std::pair<KEY, VALUE> & out_Element ) const;
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
    inline void getAtPosition(typename MAPPOS atPosition, std::pair<KEY, VALUE> & out_Element) const;

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
};

//////////////////////////////////////////////////////////////////////////
// Function Implement
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEHashMap<KEY, VALUE> class template Implement
//////////////////////////////////////////////////////////////////////////

template < typename KEY, typename VALUE >
TEHashMap<KEY, VALUE>::TEHashMap(uint32_t hashSize /* = NECommon::MAP_DEFAULT_HASH_SIZE */)
    : Constless ( )
    , mValueList(hashSize)
{
}

template < typename KEY, typename VALUE >
bool TEHashMap<KEY, VALUE>::operator == (const TEHashMap<KEY, VALUE>& other) const
{
    return (mValueList == other.mValueList);
}

template < typename KEY, typename VALUE >
bool TEHashMap<KEY, VALUE>::operator != ( const TEHashMap<KEY, VALUE>& other ) const
{
    return (mValueList != other.mValueList);
}

template < typename KEY, typename VALUE >
inline VALUE & TEHashMap<KEY, VALUE>::operator [] (const KEY& Key)
{
    return mValueList[Key];
}

template < typename KEY, typename VALUE >
inline const VALUE & TEHashMap<KEY, VALUE>::operator [] ( const KEY & Key ) const
{
    return mValueList[Key];
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::isEmpty(void) const
{
    return mValueList.empty();
}

template < typename KEY, typename VALUE >
inline uint32_t TEHashMap<KEY, VALUE>::getSize( void ) const
{
    return static_cast<uint32_t>(mValueList.size());
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::firstPosition( void ) const
{
    typename std::unordered_map<KEY, VALUE>::const_iterator pos = mValueList.begin();
    return Constless::iter(mValueList, pos);
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::lastPosition(void) const
{
    typename std::unordered_map<KEY, VALUE>::const_iterator pos = mValueList.empty() == false ? -- mValueList.end() : mValueList.end();
    return Constless::iter(mValueList, pos);
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::isStartPosition(const MAPPOS pos) const
{
    return (pos == mValueList.begin());
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::isLastPosition(const MAPPOS pos) const
{
    return (mValueList.empty() == false) && (pos == --mValueList.end());
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::invalidPosition(void) const
{
    return iter(mValueList.end());
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::isInvalidPosition(const MAPPOS pos) const
{
    return (pos == mValueList.end());
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::isValidPosition(const MAPPOS pos) const
{
    return (pos != mValueList.end());
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::checkPosition(const MAPPOS pos) const
{
    typename std::unordered_map<KEY, VALUE>::const_iterator it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
        ++it;

    return (it != mValueList.end());
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::clear(void)
{
    mValueList.clear();
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::freeExtra(void)
{
    mValueList.clear();
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::release(void)
{
    mValueList.clear();
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::find( const KEY & Key, VALUE & out_Value ) const
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

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::find(const KEY& Key) const
{
    typename std::unordered_map<KEY, VALUE>::const_iterator cit = mValueList.find(Key);
    return iter(mValueList, cit);
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::contains(const KEY& Key) const
{
    return (mValueList.find(Key) != mValueList.end());
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::setAt(const KEY & Key, const VALUE & newValue)
{
    mValueList[Key] = newValue;
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::setAt( KEY && Key, const VALUE & newValue)
{
    mValueList[Key] = newValue;
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::setAt(const std::pair<KEY, VALUE>& newItem)
{
    setAt(newItem.first, newItem.second);
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::setAt( std::pair<KEY, VALUE> && newItem)
{
    setAt(std::move(newItem.first), newItem.second);
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::merge(const TEHashMap<KEY, VALUE>& source)
{
    mValueList.merge(source.mValueList);
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::merge(TEHashMap<KEY, VALUE> && source)
{
    mValueList.merge(std::move(source.mValueList));
}

template < typename KEY, typename VALUE >
inline std::pair<typename TEHashMap<KEY, VALUE>::MAPPOS, bool> TEHashMap<KEY, VALUE>::addIfNew(const KEY& newKey, const VALUE& newValue)
{
    return mValueList.insert({ newKey, newValue });
}

template < typename KEY, typename VALUE >
inline std::pair<typename TEHashMap<KEY, VALUE>::MAPPOS, bool> TEHashMap<KEY, VALUE>::addIfNew( KEY && newKey, VALUE && newValue)
{
    return mValueList.insert(std::make_pair(newKey, newValue));
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::updateAt(const KEY & Key, const VALUE & newValue)
{
    MAPPOS pos = mValueList.find(Key);
    if (pos != mValueList.end())
    {
        pos->second = newValue;
    }

    return pos;
}

template < typename KEY, typename VALUE >
bool TEHashMap<KEY, VALUE>::removeAt(const KEY& Key)
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

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::removeAt(const KEY & Key, VALUE& out_Value)
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

template < typename KEY, typename VALUE >
typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::setPosition(typename TEHashMap<KEY, VALUE>::MAPPOS atPosition, const VALUE & newValue)
{
    ASSERT( atPosition != mValueList.end() );
    atPosition->second = newValue;
    return (++atPosition);
}

template < typename KEY, typename VALUE >
typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::removePosition(typename TEHashMap<KEY, VALUE>::MAPPOS curPos, KEY& out_Key, VALUE& out_Value)
{
    ASSERT( curPos != mValueList.end());
    MAPPOS result   = curPos + 1;
    out_Key         = curPos->first;
    out_Value       = curPos->second;

    return mValueList.erase(curPos);
}

template < typename KEY, typename VALUE >
typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::removePosition(MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::removeFirst(KEY& out_Key, VALUE& out_Value)
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

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::removeFirst( void )
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

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::removeLast(KEY& out_Key, VALUE& out_Value)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.rbegin();
        ASSERT(pos != mValueList.end());
        out_Key = pos->first;
        out_Value = pos->second;

        mValueList.erase(pos);
        result = true;
    }

    return result;
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::removeLast(void)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.rbegin();
        ASSERT(pos != mValueList.end());
        mValueList.erase(pos);
        result = true;
    }

    return result;
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::nextPosition(typename TEHashMap<KEY, VALUE>::MAPPOS atPosition, KEY& out_Key, VALUE& out_Value) const
{
    ASSERT(atPosition != mValueList.end());

    MAPPOS result   = atPosition + 1;
    out_Key		    = atPosition->first;
    out_Value	    = atPosition->second;

    return result;
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS 
TEHashMap<KEY, VALUE>::nextPosition( typename TEHashMap<KEY, VALUE>::MAPPOS atPosition
                                              , std::pair<KEY, VALUE> & out_Element) const
{
    return nextPosition(atPosition, out_Element.mKey, out_Element.mValue);
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::nextPosition(typename TEHashMap<KEY, VALUE>::MAPPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return ++ atPosition;
}

template < typename KEY, typename VALUE >
inline VALUE & TEHashMap<KEY, VALUE>::getAt( const KEY & Key )
{
    return mValueList.at(Key);
}

template < typename KEY, typename VALUE >
inline const VALUE & TEHashMap<KEY, VALUE>::getAt(const KEY & Key) const
{
    return mValueList.at(Key);
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::getAtPosition(typename TEHashMap<KEY, VALUE>::MAPPOS atPosition, KEY & out_Key, VALUE & out_Value) const
{
    ASSERT(atPosition != mValueList.end());
    out_Key     = atPosition->first;
    out_Value   = atPosition->second;
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::getAtPosition( typename TEHashMap<KEY, VALUE>::MAPPOS atPosition
                                                           , std::pair<KEY, VALUE> & out_Element) const
{
    getAtPosition(atPosition, out_Element.mKey, out_Element.mValue);
}

template < typename KEY, typename VALUE >
inline const KEY & TEHashMap<KEY, VALUE>::keyAtPosition(typename TEHashMap<KEY, VALUE>::MAPPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->first;
}

template < typename KEY, typename VALUE >
inline KEY& TEHashMap<KEY, VALUE>::keyAtPosition(typename TEHashMap<KEY, VALUE>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return const_cast<KEY &>(atPosition->first);
}

template < typename KEY, typename VALUE >
inline const VALUE & TEHashMap<KEY, VALUE>::valueAtPosition(typename TEHashMap<KEY, VALUE>::MAPPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE >
inline VALUE& TEHashMap<KEY, VALUE>::valueAtPosition(typename TEHashMap<KEY, VALUE>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::nextEntry(typename TEHashMap<KEY, VALUE>::MAPPOS & in_out_NextPosition, KEY & out_NextKey, VALUE & out_NextValue) const
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
// TEHashMap<KEY, VALUE> class friend methods
//////////////////////////////////////////////////////////////////////////

template < typename KEY, typename VALUE >
const IEInStream & operator >> ( const IEInStream & stream, TEHashMap<KEY, VALUE> & input )
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

template < typename KEY, typename VALUE >
IEOutStream & operator << ( IEOutStream & stream, const TEHashMap<KEY, VALUE> & output )
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
