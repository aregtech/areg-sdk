#ifndef AREG_BASE_HASHMAP_HPP
#define AREG_BASE_HASHMAP_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/HashMap.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Hash Map class template.
 *              Hash Map object to store elements by its hash value.
 *              Every element is accessed by unique Key value.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TemplateBase.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/MemoryDefs.hpp"

#include <algorithm>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////
// HashMap<KEY, VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The Hash Map binds Value with its Key. The Key element is unique.
 *          Values are accessed by Key. Key and Value can be of different types.
 *
 *          To access first element in Hash Map, get the first Position (call function
 *          first_position()). Each next element is accessed by calling next position.
 *          The type KEY should be possible to convert to uint32_t type required to
 *          calculate the Hash. If KEY type is an object it should have implemented
 *          hasher std::hash<KEY>, KEY comparing operator or KEY comparing function
 *          std::equal_to<KEY>. In addition, the KEY and VALUE types should have
 *          at least default constructor and valid assigning operator.
 *
 *          For example:
 *          class MyClass {
 *              int32_t     mData;
 *          }
 *
 *          namespace std {
 *
 *              // Hasher
 *              template<> struct hash<MyClass> {
 *                  uint32_t operator()(const MyClass& my) const
 *                  {   return mData; }
 *              }
 *
 *              // Check equality
 *              template<> struct equal_to<MyClass> {
 *                  bool operator()(const MyClass& lhs, const MyClass& rhs) const
 *                  {   return (lhs.mData == rhs.mData); }
 *              }
 *          }
 *
 *          The HashMap object is not thread safe and data should be  synchronized manually.
 *
 * \tparam  KEY     The type of Key to identify entries in the hash map. Either should
 *                  be primitive or should have at least default and copy constructors,
 *                  and assigning operator. There should be as well possibility to convert
 *                  KEY type to the hash by calling std::hash() and compare keys by calling
 *                  comparing operator or by calling std::equal_to().
 * \tparam  VALUE   The type of stored items. Either should be primitive or should have
 *                  default constructor and valid assigning operator. Also, should be
 *                  possible to convert to type 'const VALUE&'.
 **/
template < typename KEY, typename VALUE>
/**
 * \brief   Hash map binding values to unique keys, with key-value pair access by key. Keys are
 *          hashed for O(1) lookup. KEY type must be hashable via std::hash<KEY> and comparable via
 *          std::equal_to<KEY>; VALUE type can differ. Both KEY and VALUE require default
 *          constructor and assignment operator. Not thread-safe; synchronize manually.
 **/
class HashMap : protected Constless<std::unordered_map<KEY, VALUE>>
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
     * \brief   Constructs empty hash-map with hash table size 'hashSize'.
     *
     * \param   hashSize    The size of hash map table. By default, MAP_DEFAULT_HASH_SIZE (63).
     **/
    HashMap( uint32_t hashSize = NECommon::MAP_DEFAULT_HASH_SIZE);

    /**
     * \brief   Copy constructor.
     *
     * \param   src     The source to copy data.
     **/
    HashMap( const HashMap<KEY, VALUE> & src ) = default;

    /**
     * \brief   Move constructor.
     *
     * \param   src     The source to move data.
     **/
    HashMap( HashMap<KEY, VALUE> && src ) noexcept = default;

    /**
     * \brief   Constructs hash-map from parallel arrays of keys and values. Duplicate keys result
     *          in the last value being retained; final size equals count only if all keys are
     *          unique.
     *
     * \param   keys        The list of keys to copy.
     * \param   values      The list of values to pair with keys.
     * \param   count       The number of entries in the key and value entries.
     **/
    HashMap(const KEY* keys, const VALUE * values, uint32_t count);

    /**
     * \brief   Destructor.
     **/
    ~HashMap();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Subscript operator. Returns reference to value of element by given key. May be used
     *          on either the right (r-value) or the left (l-value) of an assignment statement.
     **/
    inline VALUE& operator [] (const KEY& Key);

    /**
     * \brief   Subscript operator. Returns reference to value of element by given key. May be used
     *          on the right (r-value).
     **/
    inline const VALUE& operator [] (const KEY& Key) const;

    /**
     * \brief   Copy assignment operator. Replaces all entries with a copy of source entries.
     *
     * \param   src     The source of hash-map.
     **/
    inline HashMap<KEY, VALUE>& operator = ( const HashMap<KEY, VALUE> & src ) = default;

    /**
     * \brief   Move assignment operator. Replaces all entries by moving source entries.
     *
     * \param   src     The source of hash-map.
     **/
    inline HashMap<KEY, VALUE>& operator = ( HashMap<KEY, VALUE> && src ) noexcept = default;

    /**
     * \brief   Returns true if both hash-maps contain equal keys and values.
     *
     * \param   other       The hash-map object to compare.
     **/
    inline bool operator == ( const HashMap<KEY, VALUE> & other ) const;

    /**
     * \brief   Returns true if the hash-maps differ in keys or values.
     *
     * \param   other       The hash-map object to compare.
     **/
    inline bool operator != ( const HashMap<KEY, VALUE> & other ) const;

/************************************************************************/
// Friend global operators to make Hash Map streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream hash-map key and value pairs.
     *          If hash-map previously had values, they will be removed and new values
     *          from the stream will be set in the same sequence as they are present
     *          in the stream. There should be possibility to initialize values from
     *          streaming object and if KEY or VALUE are not primitives, but an object,
     *          they should have implemented streaming operator.
     * \param   stream  The streaming object to read values.
     * \param   input   The hash-map object to save initialized values.
     **/
    template < typename K, typename V >
    /**
     * \brief   Deserializes hash-map key-value pairs from stream, replacing existing entries.
     *
     * \param   stream      The streaming object to read values.
     * \param   input       The hash-map object to save initialized values.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, HashMap<K, V> & input);

    /**
     * \brief   Writes to the stream the key and value pairs of hash-map.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream key and value pairs and if KEY or VALUE
     *          are not primitives, but an object, they should have implemented streaming operator.
     * \param   stream  The stream to write values.
     * \param   output  The hash-map object containing value to stream.
     **/
    template < typename K, typename V >
    /**
     * \brief   Serializes hash-map key-value pairs to stream.
     *
     * \param   stream      The stream to write values.
     * \param   output      The hash-map object containing value to stream.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const HashMap<K, V> & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the hash-map is empty and has no elements.
     **/
    inline bool is_empty() const;

    /**
     * \brief   Returns the current size of the hash-map.
     **/
    inline uint32_t size() const;

    /**
     * \brief   Returns the position of the first key-value entry in the hash-map, or invalid
     *          position if empty.
     **/
    inline MAPPOS first_position() const;

    /**
     * \brief   Returns true if specified position points the first entry in the hash-map.
     *
     * \param   pos     The position to check.
     **/
    inline bool is_first_position(const MAPPOS pos) const;

    /**
     * \brief   Returns the invalid position sentinel value.
     **/
    inline MAPPOS invalid_position() const;

    /**
     * \brief   Returns true if specified position is invalid, i.e. points the end of the hash-map.
     **/
    inline bool is_invalid_position(const MAPPOS pos) const;

    /**
     * \brief   Returns true if the given position is not pointing the end of the hash-map. Note, it
     *          does not check whether there is a such position existing in the hash-map.
     **/
    inline bool is_valid_position(const MAPPOS pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the
     *          hash-map. The duration of checkup depends on the location of the position in the
     *          hash-map.
     *
     * \param   pos     The position to check.
     **/
    inline bool check_position(const MAPPOS pos) const;

    /**
     * \brief   Returns true if the given element exists in the hash-map.
     *
     * \param   Key     The key of value to search.
     **/
    inline bool contains(const KEY& Key) const;

    /**
     * \brief   Returns the underlying unordered_map where data are stored.
     **/
    inline const std::unordered_map<KEY, VALUE>& data() const;

/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief   Remove all entries of the hash map.
     **/
    inline void clear();

    /**
     * \brief   Clears all entries and releases allocated capacity.
     **/
    inline void release();

    /**
     * \brief   Searches for element by key. Returns true if found; outputs the value.
     *
     * \param   Key         The key to search.
     * \param[out] Value       On output, contains value of found element
     * \return  Returns true if there is an entry with the specified key.
     **/
    bool find( const KEY & Key, VALUE & Value ) const;

    /**
     * \brief   Returns the position of the entry with the given key, or invalid position if not
     *          found.
     *
     * \param   Key     The key to search.
     * \return  Returns valid hash-map position if found an entry by the give key. Otherwise,
     *          returns invalid position (end of map position).
     **/
    inline MAPPOS find(const KEY& Key) const;

    /**
     * \brief   Returns reference to the value of the element by given existing key, which can be on
     *          either the right (r-value) or the left (l-value) of an assignment statement.
     **/
    inline VALUE& at(const KEY& Key);
    /**
     * \brief   Returns reference to the value of the element by given existing key, which can be on
     *          the right (r-value) of an assignment statement.
     **/
    inline const VALUE& at(const KEY& Key) const;

    /**
     * \brief   Sets or creates entry by key with given value.
     *
     * \param   Key         The key of element to search or create new entry.
     * \param   newValue    The value of element to set.
     **/
    inline void set_at( const KEY & Key, const VALUE & newValue );
    /**
     * \brief   Sets or creates entry by key with given value.
     *
     * \param   Key         The key of element to search or create new entry.
     * \param   newValue    The value of element to set.
     * \note    Move overload. Takes ownership of key and value.
     **/
    inline void set_at( KEY && Key, VALUE && newValue);
    /**
     * \brief   Sets or creates entry from key-value pair.
     *
     * \param   element     The Key and Value pair of element to set or insert.
     **/
    inline void set_at( const std::pair<KEY, VALUE> & element);
    /**
     * \brief   Sets or creates entry from key-value pair.
     *
     * \param   element     The Key and Value pair of element to set or insert.
     * \note    Move overload. Takes ownership of the pair.
     **/
    inline void set_at( std::pair<KEY, VALUE> && element);

    /**
     * \brief   Extracts entries from source and inserts into this map. Entries with duplicate keys
     *          are left in source.
     *
     * \param[in,out] source      The source of hash map to merge. On output, contains entries with
     *                            keys already present in this map.
     **/
    inline void merge( HashMap<KEY, VALUE> & source );
    /**
     * \brief   Extracts entries from source and inserts into this map. Entries with duplicate keys
     *          are left in source.
     *
     * \param[in,out] source      The source of hash map to merge. On output, contains entries with
     *                            keys already present in this map.
     * \note    Move overload.
     **/
    inline void merge( HashMap<KEY, VALUE> && source );

    /**
     * \brief   Adds entry only if key does not exist, or updates existing entry if updateExisting
     *          is true. Returns position and success flag.
     *
     * \param   newKey              The key of the entry in the hash map.
     * \param   newValue            The value of the entry in the hash map.
     * \param   updateExisting      If true, updates the value of an existing key instead of
     *                              skipping it.
     * \return  Returns a pair of 'MAPPOS' and 'bool' values: 'MAPPOS' indicates the position of the
     *          entry in the hash map. 'bool' equal to 'true' indicates that new entry is created.
     *          If this value is 'false' no new entry is created. When new entry is created, the
     *          existing position values can be invalidated.
     **/
    inline std::pair<MAPPOS, bool> add_if_unique(const KEY & newKey, const VALUE & newValue, bool updateExisting = false );
    /**
     * \brief   Adds entry only if key does not exist, or updates existing entry if updateExisting
     *          is true. Returns position and success flag.
     *
     * \param   newKey              The key of the entry in the hash map.
     * \param   newValue            The value of the entry in the hash map.
     * \param   updateExisting      If true, updates the value of an existing key instead of
     *                              skipping it.
     * \return  Returns a pair of 'MAPPOS' and 'bool' values: 'MAPPOS' indicates the position of the
     *          entry in the hash map. 'bool' equal to 'true' indicates that new entry is created.
     *          If this value is 'false' no new entry is created. When new entry is created, the
     *          existing position values can be invalidated.
     * \note    Move overload. Takes ownership of key and value.
     **/
    inline std::pair<MAPPOS, bool> add_if_unique(KEY && newKey, VALUE && newValue, bool updateExisting = false );

    /**
     * \brief   Updates value of existing entry. Returns its position, or invalid position if key
     *          not found.
     *
     * \param   Key         The key of an element in the hash-map to update.
     * \param   newValue    New value to set on existing entry.
     * \return  Returns valid position if the existing element is updated. Otherwise, returns
     *          invalid position.
     **/
    inline MAPPOS update_at( const KEY & Key, const VALUE & newValue );

    /**
     * \brief   Removes entry by key. Returns true if found and removed, false otherwise.
     *
     * \param   Key     The Key of the entry to search and remove.
     **/
    inline bool remove_at(const KEY& Key );

    /**
     * \brief   Removes entry by key. Returns true if found and removed, outputting the removed
     *          value.
     *
     * \param   Key         The Key of the entry to search and remove.
     * \param[out] Value       If succeeded to remove, on output it contains the value of the
     *                         removed element.
     **/
    inline bool remove_at( const KEY & Key, VALUE & Value );

    /**
     * \brief   Updates value at position. Returns position of next entry, or invalid if at end.
     *
     * \param   atPosition      The valid position of the element to update value.
     * \param   newValue        New value to set for existing element.
     * \return  Returns valid position of the next element or invalid position if it updated the
     *          last entry in the hash-map.
     **/
    inline MAPPOS set_position(MAPPOS atPosition, const VALUE& newValue );

    /**
     * \brief   Removes entry at position. Returns next position, or invalid if at end.
     *
     * \param   atPosition      The valid position of the element in the hash-map to remove.
     * \return  Returns valid position of the next entry in the hash-map or returns invalid position
     *          if removed last element in the map.
     **/
    inline MAPPOS remove_position(MAPPOS atPosition);

    /**
     * \brief   Removes entry at position, outputting key and value. Returns next position, or
     *          invalid if at end.
     *
     * \param[out] Key         On output, this contains the key of the removed element
     * \param[out] Value       On output, this contains the value of the removed element.
     * \return  Returns valid position of the next entry in the hash-map or returns invalid position
     *          if removed last element in the map.
     **/
    inline MAPPOS remove_position(MAPPOS atPosition, KEY & Key, VALUE & Value );

    /**
     * \brief   Removes the first entry in the hash map.
     **/
    inline void remove_first();

    /**
     * \brief   Removes the first entry, outputting its key and value. Returns true if map was not
     *          empty.
     *
     * \param[out] Key         On output it contains the key of the removed element in the hash-map.
     * \param[out] Value       On output it contains the value of the removed element in the
     *                         hash-map.
     * \return  Returns true if hash-map was not empty and first entry is removed. Otherwise,
     *          returns false.
     **/
    inline bool remove_first(KEY& Key, VALUE& Value);

    /**
     * \brief   Removes the last entry in the hash map.
     **/
    inline void remove_last();

    /**
     * \brief   Removes the last entry, outputting its key and value. Returns true if map was not
     *          empty.
     *
     * \param[out] Key         On output it contains the key of the removed element in the hash-map.
     * \param[out] Value       On output it contains the value of the removed element in the
     *                         hash-map.
     * \return  Returns true if hash-map was not empty and last entry is removed. Otherwise, returns
     *          false.
     **/
    inline bool remove_last(KEY& Key, VALUE& Value);

    /**
     * \brief   Returns position of the next entry following the given position.
     *
     * \param   atPosition      The position of the entry to get next and extract values.
     * \return  Next valid position in the hash-map or invalid position if reached end of hash-map.
     **/
    inline MAPPOS next_position(MAPPOS atPosition) const;

    /**
     * \brief   Returns position of the next entry following the given position, outputting key and
     *          value.
     *
     * \param   atPosition      The position of the entry to get next and extract values.
     * \param[out] Key             On output, this contains key of given position.
     * \param[out] Value           On output, this contains value of given position.
     * \return  Next valid position in the hash-map or invalid position if reached end of hash-map.
     **/
    inline MAPPOS next_position(MAPPOS atPosition, KEY & Key, VALUE & Value ) const;

    /**
     * \brief   Returns position of the next entry following the given position, outputting
     *          key-value pair.
     *
     * \param   atPosition      The position of the entry to get next and extract values.
     * \param[out] Element         On output, this element contains pair of Key and Value specified
     *                             by given position.
     * \return  Next valid position in the hash-map or invalid position if reached end of hash-map.
     **/
    inline MAPPOS next_position(MAPPOS atPosition, std::pair<KEY, VALUE> & Element ) const;

    /**
     * \brief   Extracts key and value at the given position.
     *
     * \param   atPosition      The position of the element to extract key and value.
     * \param[out] Key             On output, contains key of the element at given position.
     * \param[out] Value           On output, contains value of the element at given position.
     **/
    inline void at_position(MAPPOS atPosition, KEY & Key, VALUE & Value ) const;

    /**
     * \brief   Extracts key-value pair at the given position.
     *
     * \param   atPosition      The position of the element to extract key and value.
     * \param[out] Element         On output, contains the Key and Value pair of the element at
     *                             given position
     **/
    inline void at_position(MAPPOS atPosition, std::pair<KEY, VALUE> & Element) const;

    /**
     * \brief   Returns the Key of the entry at the given position.
     *
     * \param   atPosition      The position of the element.
     **/
    inline const KEY & key_at_position(const MAPPOS atPosition ) const;
    /**
     * \brief   Returns the Key of the entry at the given position.
     *
     * \param   atPosition      The position of the element.
     **/
    inline KEY& key_at_position(MAPPOS atPosition);

    /**
     * \brief   Returns the Value of the entry at the given position.
     *
     * \param   atPosition      The position of the element.
     **/
    inline const VALUE & value_at_position(const MAPPOS atPosition ) const;
    /**
     * \brief   Returns the Value of the entry at the given position.
     *
     * \param   atPosition      The position of the element.
     **/
    inline VALUE& value_at_position(MAPPOS atPosition);

    /**
     * \brief   Advances position and outputs next entry's key and value. Returns true if next entry
     *          exists.
     *
     * \param[in,out] nextPos         On input this indicates the valid position of the entry in the
     *                                hash map. On output, this parameter points either next valid
     *                                entry in the hash-map or invalid entry if no more entry is
     *                                following.
     * \param[out] nextKey         On output, this contains key of the next entry in hash map.
     * \param[out] nextValue       On output, this contain value of the next entry in hash map.
     * \return  Returns true, if there is a next element and the output values are valid.
     **/
    inline bool next_entry(MAPPOS & nextPos, KEY & nextKey, VALUE & nextValue ) const;

    /**
     * \brief   Copies up to elemCount entries into provided pre-allocated buffers. Returns number
     *          of entries copied.
     *
     * \param[in,out] keys            A pre-allocated buffer where the keys of the hash-map elements
     *                                will be copied. Must be large enough to hold at least
     *                                `elemCount` elements.
     * \param[in,out] values          A pre-allocated buffer where the values of the hash-map
     *                                elements will be copied. Must be large enough to hold at least
     *                                `elemCount` elements.
     * \param   elemCount       The maximum number of elements to copy into the keys and values
     *                          buffer. If set to 0, no elements are copied.
     * \return  The number of elements successfully copied.
     **/
    inline uint32_t elements(KEY * keys, VALUE * values, uint32_t elemCount);

//////////////////////////////////////////////////////////////////////////
//Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Converts unordered_map const iterator to MAPPOS.
     *
     * \param   cit     The constant iterator of the unsorted map.
     * \return  Returns converted MAPPOS type.
     **/
    inline MAPPOS _citer2pos(typename std::unordered_map<KEY, VALUE>::const_iterator cit) const;

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
// HashMap<KEY, VALUE> class template Implement
//////////////////////////////////////////////////////////////////////////

template < typename KEY, typename VALUE >
HashMap<KEY, VALUE>::HashMap(uint32_t hashSize /* = NECommon::MAP_DEFAULT_HASH_SIZE */)
    : Constless<std::unordered_map<KEY, VALUE>>( )
    , mValueList(hashSize)
{
}

template<typename KEY, typename VALUE>
HashMap<KEY, VALUE>::HashMap(const KEY* keys, const VALUE* values, uint32_t count)
    : Constless<std::unordered_map<KEY, VALUE>>()
    , mValueList()
{
    mValueList.reserve(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        mValueList[keys[i]] = values[i];
    }
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::operator == (const HashMap<KEY, VALUE>& other) const
{
    return (mValueList == other.mValueList);
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::operator != ( const HashMap<KEY, VALUE>& other ) const
{
    return (mValueList != other.mValueList);
}

template<typename KEY, typename VALUE>
inline HashMap<KEY, VALUE>::~HashMap()
{
    mValueList.clear();
}

template < typename KEY, typename VALUE >
inline VALUE & HashMap<KEY, VALUE>::operator [] (const KEY& Key)
{
    return mValueList[Key];
}

template < typename KEY, typename VALUE >
inline const VALUE & HashMap<KEY, VALUE>::operator [] ( const KEY & Key ) const
{
    return mValueList[Key];
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::is_empty() const
{
    return mValueList.empty();
}

template < typename KEY, typename VALUE >
inline uint32_t HashMap<KEY, VALUE>::size() const
{
    return static_cast<uint32_t>(mValueList.size());
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::first_position() const
{
    return _citer2pos(mValueList.begin());
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::is_first_position(const MAPPOS pos) const
{
    return (mValueList.empty() == false) && (pos == mValueList.begin());
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::invalid_position() const
{
	return _citer2pos(mValueList.end());
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::is_invalid_position(const MAPPOS pos) const
{
    return (pos == mValueList.end());
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::is_valid_position(const MAPPOS pos) const
{
    return (pos != mValueList.end());
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::check_position(const MAPPOS pos) const
{
    auto it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
    {
        ++it;
    }

    return (it != mValueList.end());
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::contains(const KEY& Key) const
{
    return (mValueList.find(Key) != mValueList.end());
}

template<typename KEY, typename VALUE>
inline const std::unordered_map<KEY, VALUE>& HashMap<KEY, VALUE>::data() const
{
    return mValueList;
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::clear()
{
    mValueList.clear();
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::release()
{
    mValueList.clear();
}

template < typename KEY, typename VALUE >
bool HashMap<KEY, VALUE>::find( const KEY & Key, VALUE & Value ) const
{
    bool result{ false };
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.find(Key);
        if (pos != mValueList.end())
        {
            Value = pos->second;
            result = true;
        }
    }

    return result;
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::find(const KEY& Key) const
{
    return _citer2pos(mValueList.empty() ? mValueList.end() : mValueList.find(Key));
}

template < typename KEY, typename VALUE >
inline VALUE & HashMap<KEY, VALUE>::at( const KEY & Key )
{
    return mValueList.at(Key);
}

template < typename KEY, typename VALUE >
inline const VALUE & HashMap<KEY, VALUE>::at(const KEY & Key) const
{
    return mValueList.at(Key);
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::set_at(const KEY & Key, const VALUE & newValue)
{
    mValueList[Key] = newValue;
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::set_at( KEY && Key, VALUE && newValue)
{
    mValueList[Key] = std::move(newValue);
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::set_at(const std::pair<KEY, VALUE>& element)
{
    set_at(element.first, element.second);
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::set_at( std::pair<KEY, VALUE> && element)
{
    set_at(std::move(element.first), std::move(element.second));
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::merge(HashMap<KEY, VALUE>& source)
{
    mValueList.merge(source.mValueList);
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::merge(HashMap<KEY, VALUE> && source)
{
    mValueList.merge(std::move(source.mValueList));
}

template < typename KEY, typename VALUE >
inline std::pair<typename HashMap<KEY, VALUE>::MAPPOS, bool> HashMap<KEY, VALUE>::add_if_unique(const KEY& newKey, const VALUE& newValue, bool updateExisting /*= false*/ )
{
    std::pair<MAPPOS, bool> result = mValueList.insert({ newKey, newValue });
    if ( updateExisting && (result.second == false) )
    {
        ASSERT( result.first != mValueList.end( ) );
        result.first->second = newValue;
    }

    return result;
}

template < typename KEY, typename VALUE >
inline std::pair<typename HashMap<KEY, VALUE>::MAPPOS, bool> HashMap<KEY, VALUE>::add_if_unique( KEY && newKey, VALUE && newValue, bool updateExisting /*= false*/ )
{
    std::pair<MAPPOS, bool> result = mValueList.insert( std::make_pair( newKey, newValue ) );
    if ( updateExisting && (result.second == false) )
    {
        ASSERT( result.first != mValueList.end( ) );
        result.first->second = std::move(newValue);
    }

    return result;
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::update_at(const KEY & Key, const VALUE & newValue)
{
    MAPPOS pos = mValueList.empty() ? invalid_position() : mValueList.find(Key);
    if (pos != mValueList.end())
    {
        pos->second = newValue;
    }

    return pos;
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::remove_at(const KEY& Key)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        MAPPOS pos = mValueList.find(Key);
        if (pos != mValueList.end())
        {
            result = true;
            mValueList.erase(pos);
        }
    }

    return result;
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::remove_at(const KEY & Key, VALUE& Value)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        MAPPOS pos = mValueList.find(Key);
        if (pos != mValueList.end())
        {
            result = true;
            Value = pos->second;
            mValueList.erase(pos);
        }
    }

    return result;
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::set_position(typename HashMap<KEY, VALUE>::MAPPOS atPosition, const VALUE & newValue)
{
    ASSERT( atPosition != mValueList.end() );
    atPosition->second = newValue;
    return (++atPosition);
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::remove_position(typename HashMap<KEY, VALUE>::MAPPOS curPos, KEY& Key, VALUE& Value)
{
    ASSERT( curPos != mValueList.end());
    Key         = curPos->first;
    Value       = curPos->second;

    return mValueList.erase(curPos);
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::remove_position(MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::remove_first(KEY& Key, VALUE& Value)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        auto pos    = mValueList.begin();
        ASSERT(pos != mValueList.end());
        Key = pos->first;
        Value = pos->second;

        mValueList.erase(pos);
        result      = true;
    }

    return result;
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::remove_first()
{
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.begin();
        ASSERT(pos != mValueList.end());
        mValueList.erase(pos);
    }
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::remove_last(KEY& Key, VALUE& Value)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.rbegin();
        ASSERT(pos != mValueList.end());
        Key = pos->first;
        Value = pos->second;

        mValueList.erase(pos);
        result = true;
    }

    return result;
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::remove_last()
{
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.rbegin();
        ASSERT(pos != mValueList.end());
        mValueList.erase(pos);
    }
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::next_position(HashMap<KEY, VALUE>::MAPPOS atPosition, KEY& Key, VALUE& Value) const
{
    ASSERT(atPosition != mValueList.end());

    Key		    = atPosition->first;
    Value	    = atPosition->second;

    return (++ atPosition);
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS
HashMap<KEY, VALUE>::next_position( HashMap<KEY, VALUE>::MAPPOS atPosition, std::pair<KEY, VALUE> & Element) const
{
    return next_position(atPosition, Element.first, Element.second);
}

template < typename KEY, typename VALUE >
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::next_position(HashMap<KEY, VALUE>::MAPPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return (++ atPosition);
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::at_position(HashMap<KEY, VALUE>::MAPPOS atPosition, KEY & Key, VALUE & Value) const
{
    ASSERT(atPosition != mValueList.end());
    Key     = atPosition->first;
    Value   = atPosition->second;
}

template < typename KEY, typename VALUE >
inline void HashMap<KEY, VALUE>::at_position(HashMap<KEY, VALUE>::MAPPOS atPosition, std::pair<KEY, VALUE> & Element) const
{
    at_position(atPosition, Element.first, Element.second);
}

template < typename KEY, typename VALUE >
inline const KEY & HashMap<KEY, VALUE>::key_at_position(const HashMap<KEY, VALUE>::MAPPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->first;
}

template < typename KEY, typename VALUE >
inline KEY& HashMap<KEY, VALUE>::key_at_position(HashMap<KEY, VALUE>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return const_cast<KEY &>(atPosition->first);
}

template < typename KEY, typename VALUE >
inline const VALUE & HashMap<KEY, VALUE>::value_at_position(const HashMap<KEY, VALUE>::MAPPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE >
inline VALUE& HashMap<KEY, VALUE>::value_at_position(HashMap<KEY, VALUE>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE >
inline bool HashMap<KEY, VALUE>::next_entry(HashMap<KEY, VALUE>::MAPPOS & nextPos, KEY & nextKey, VALUE & nextValue) const
{
    ASSERT( nextPos != mValueList.end() );
    bool result = false;
    if (++nextPos != mValueList.end())
    {
        nextKey     = nextPos->first;
        nextValue   = nextPos->second;
        result = true;
    }

    return result;
}

template<typename KEY, typename VALUE>
inline uint32_t HashMap<KEY, VALUE>::elements(KEY* keys, VALUE* values, uint32_t elemCount)
{
    uint32_t result{ std::min(static_cast<uint32_t>(mValueList.size()), elemCount)};
    if (result > 0)
    {
        uint32_t i = 0;
        for (const auto & elem : mValueList)
        {
            keys[i]     = elem.first;
            values[i]   = elem.second;

            if (++i == result)
            {
                break;
            }
        }
    }

    return result;
}

template<typename KEY, typename VALUE>
inline typename HashMap<KEY, VALUE>::MAPPOS HashMap<KEY, VALUE>::_citer2pos(typename std::unordered_map<KEY, VALUE>::const_iterator cit) const
{
    return Constless<std::unordered_map<KEY, VALUE>>::iter(mValueList, cit);
}

//////////////////////////////////////////////////////////////////////////
// HashMap<KEY, VALUE> class friend methods
//////////////////////////////////////////////////////////////////////////

template < typename K, typename V >
inline const InStream & operator >> ( const InStream & stream, HashMap<K, V> & input )
{
    uint32_t size = 0;
    stream >> size;

    input.mValueList.clear();
    input.mValueList.reserve(size);

    for (uint32_t i = 0; i < size; ++ i)
    {
        K key;
        V value;
        stream >> key >> value;
        input.set_at(key, value);
    }

    return stream;
}

template < typename K, typename V >
inline OutStream & operator << ( OutStream & stream, const HashMap<K, V> & output )
{
    uint32_t size = output.size();
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

#endif  // AREG_BASE_HASHMAP_HPP
