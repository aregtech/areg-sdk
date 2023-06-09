#ifndef AREG_BASE_TEHASHMAP_HPP
#define AREG_BASE_TEHASHMAP_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
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
 *          To access first element in Hash Map, get the first Position (call function
 *          firstPosition()). Each next element is accessed by calling next position.
 *          The type KEY should be possible to convert to unsigned int type required to
 *          calculate the Hash. If KEY type is an object it should have implemented
 *          hasher std::hash<KEY>, KEY comparing operator or KEY comparing function
 *          std::equal_to<KEY>. In addition, the KEY and VALUE types should have
 *          at least default constructor and valid assigning operator.
 *
 *          For example:
 *          class MyClass {
 *              int     mData;
 *          }
 *
 *          namespace std {
 *
 *              // Hasher
 *              template<> struct hash<MyClass> {
 *                  unsigned int operator()(const MyClass& my) const
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
     * \brief	Constructs empty hash-map with hash table size 'hashSize'.
     * \param	hashSize	The size of has map table. By default, MAP_DEFAULT_HASH_SIZE (64).
     **/
    TEHashMap( uint32_t hashSize = NECommon::MAP_DEFAULT_HASH_SIZE);

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    TEHashMap( const TEHashMap<KEY, VALUE> & src ) = default;

    /**
     * \brief   Moves entries from given source.
     * \param   src     The source to move data.
     **/
    TEHashMap( TEHashMap<KEY, VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor.
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
     * \brief   Subscript operator. Returns reference to value of element by given key.
     *          May be used on either the right (r-value) or the left (l-value) of an assignment statement.
     **/
    inline VALUE& operator [] (const KEY& Key);

    /**
     * \brief   Subscript operator. Returns reference to value of element by given key.
     *          May be used on the right (r-value).
     **/
    inline const VALUE& operator [] (const KEY& Key) const;

    /**
     * \brief   Assigning operator. Copies all values from given source.
     *          If hash-map previously had values, they will be removed and new values
     *          from source will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of hash-map.
     **/
    inline TEHashMap<KEY, VALUE>& operator = ( const TEHashMap<KEY, VALUE> & src ) = default;

    /**
     * \brief   Move operator. Moves all values from given source.
     *          If hash-map previously had values, they will be removed and new values
     *          from source will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of hash-map.
     **/
    inline TEHashMap<KEY, VALUE>& operator = ( TEHashMap<KEY, VALUE> && src ) noexcept = default;

    /**
     * \brief   Checks equality of 2 hash-map objects, and returns true if they are equal.
     *          There should be possible to compare KEY and VALUE type entries of hash-map.
     * \param   other   The hash-map object to compare.
     **/
    inline bool operator == ( const TEHashMap<KEY, VALUE> & other ) const;

    /**
     * \brief   Checks inequality of 2 hash-map objects, and returns true if they are not equal.
     *          There should be possible to compare KEY and VALUE type entries of hash-map.
     * \param   other   The hash-map object to compare.
     **/
    inline bool operator != ( const TEHashMap<KEY, VALUE> & other ) const;

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
    friend inline const IEInStream & operator >> ( const IEInStream & stream, TEHashMap<K, V> & input);

    /**
     * \brief   Writes to the stream the key and value pairs of hash-map.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream key and value pairs and if KEY or VALUE
     *          are not primitives, but an object, they should have implemented streaming operator.
     * \param   stream  The stream to write values.
     * \param   input   The hash-map object containing value to stream.
     **/
    template < typename K, typename V >
    friend inline IEOutStream & operator << ( IEOutStream & stream, const TEHashMap<K, V> & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the hash-map is empty and has no elements.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief	Returns the current size of the hash-map.
     **/
    inline uint32_t getSize( void ) const;

    /**
     * \brief	Returns the position of the first key and value entry in the hash-map, which is
     *          not invalid if the hash-map is not empty. Otherwise, returns invalid position.
     **/
    inline MAPPOS firstPosition(void) const;

    /**
     * \brief   Returns true if specified position points the first entry in the hash-map.
     * \param   pos     The position to check.
     **/
    inline bool isStartPosition(const MAPPOS pos) const;

    /**
     * \brief   Returns the invalid position of the hash-map.
     **/
    inline MAPPOS invalidPosition(void) const;

    /**
     * \brief   Returns true if specified position is invalid, i.e. points the end of the hash-map.
     **/
    inline bool isInvalidPosition(const MAPPOS pos) const;

    /**
     * \brief   Returns true if the given position is not pointing the end of the hash-map.
     *          Note, it does not check whether there is a such position existing in the hash-map.
     **/
    inline bool isValidPosition(const MAPPOS pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the hash-map.
     *          The duration of checkup depends on the location of the position in the hash-map.
     * \param   pos     The position to check.
     */
    inline bool checkPosition(const MAPPOS pos) const;

    /**
     * \brief	Checks and returns true if the given element exist in the hash-map or not.
     * \param	key	    The key of value to search.
     */
    inline bool contains(const KEY& Key) const;

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
     * \brief   Sets the size of the hash-map to zero and deletes all capacity space.
     */
    inline void release(void);

    /**
     * \brief	Searches an element entry by the given key.
     *          If found element, return true and on exit returns the value of element
     * \param	Key	        The key to search.
     * \param	out_Value   On output, contains value of found element
     * \return	Returns true if there is an entry with the specified key.
     **/
    inline bool find( const KEY & Key, VALUE & OUT out_Value ) const;

    /**
     * \brief	Search an element entry by the given key and returns the position in hash-map.
     * \param	Key	    The key to search.
     * \return	Returns valid hash-map position if found an entry by the give key.
     *          Otherwise, returns invalid position (end of map position).
     **/
    inline MAPPOS find(const KEY& Key) const;

    /**
     * \brief	Returns reference to the value of the element by given existing key, which can be
     *          on either the right (r-value) or the left (l-value) of an assignment statement.
     **/
    inline VALUE& getAt(const KEY& Key);
    /**
     * \brief	Returns reference to the value of the element by given existing key, which can be
     *          on the right (r-value) of an assignment statement.
     **/
    inline const VALUE& getAt(const KEY& Key) const;

    /**
     * \brief	Update the value of the existing element in the hash-map.
     *          The existence of the entry is checked by the given key.
     *          Creates and inserts new entry if no element with the specified key exists.
     * \param	Key	        The key of element to search or create new entry.
     * \param	newValue	The value of element to set.
     **/
    inline void setAt( const KEY & Key, const VALUE & newValue );
    inline void setAt( KEY && Key, VALUE && newValue);
    /**
     * \brief	Update existing element value or inserts new element in the Hash Map.
     * \param	element     The Key and Value pair of element to set or insert.
     **/
    inline void setAt( const std::pair<KEY, VALUE> & element);
    inline void setAt( std::pair<KEY, VALUE> && element);

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
     *          If the entry with specified key exists, neither new entry is added, nor the existing is updated.
     *          The method returns pair of value, where it indicates the position of the entry
     *          and the flag, indicating whether it added new entry or not.
     * \param   Key     The key of the entry in the hash map.
     * \param   Value   The value of the entry in the hash map.
     * \return  Returns a pair of 'MAPPOS' and 'bool' values, where
     *              -   'MAPPOS' indicates the position of the entry in the hash map.
     *              -   'bool' equal to 'true' indicates that new entry is created.
     *                  If this value is 'false' no new entry is created. When new entry is created, the existing
     *                  position values can be invalidated.
     **/
    inline std::pair<MAPPOS, bool> addIfUnique(const KEY & newKey, const VALUE & newValue);
    inline std::pair<MAPPOS, bool> addIfUnique(KEY && newKey, VALUE && newValue);

    /**
     * \brief   Updates existing element specified by the Key and returns the position in the map.
     *          If Key does not exit, no new entry is created and function returns invalid position.
     * \param   Key         The key of an element in the hash-map to update.
     * \param   newValue    New value to set on existing entry.
     * \return  Returns valid position if the existing element is updated. Otherwise, returns invalid position.
     **/
    inline MAPPOS updateAt( const KEY & Key, const VALUE & newValue );

    /**
     * \brief	Remove existing entry specified by the key and returns true if operation succeeded.
     *          Otherwise, returns false.
     * \param	Key	        The Key of the entry to search and remove.
     * \param	out_Value   If succeeded to remove, on output it contains the value of the removed element.
     **/
    inline bool removeAt(const KEY& Key );
    inline bool removeAt( const KEY & Key, VALUE & out_Value );

    /**
     * \brief	Update value of an element at the given position and return position of the next entry.
     * \param	atPosition      The valid position of the element to update value.
     * \param	newValue	    New value to set for existing element.
     * \return  Returns valid position of the next element or invalid position if it updated the last 
     *          entry in the hash-map.
     **/
    inline MAPPOS setPosition(MAPPOS atPosition, const VALUE& newValue );

    /**
     * \brief	Removes an element at the given position. The function returns next position of an entry in the hash map.
     *          or invalid position if removed last element in the map.
     * \param	curPos      The valid position of the element in the hash-map to remove.
     * \param	out_Key     On output, this contains the key of the removed element
     * \param	out_Value   On output, this contains the value of the removed element.
     * \return  Returns valid position of the next entry in the hash-map or returns invalid position if
     *          removed last element in the map.
     **/
    inline MAPPOS removePosition(MAPPOS atPosition);
    inline MAPPOS removePosition(MAPPOS IN curPos, KEY & OUT out_Key, VALUE & OUT out_Value );

    /**
     * \brief   Removes the first entry in the hash map.
     *
     * \param   out_Key     On output it contains the key of the removed element in the hash-map.
     * \param   out_Value   On output it contains the value of the removed element in the hash-map.
     * \return  Returns true if hash-map was not empty and first entry is removed. Otherwise, returns false.
     **/
    inline void removeFirst(void);
    inline bool removeFirst(KEY& OUT out_Key, VALUE& OUT out_Value);

    /**
     * \brief   Removes the last entry in the hash map.
     *
     * \param   out_Key     On output it contains the key of the removed element in the hash-map.
     * \param   out_Value   On output it contains the value of the removed element in the hash-map.
     * \return  Returns true if hash-map was not empty and last entry is removed. Otherwise, returns false.
     **/
    inline void removeLast(void);
    inline bool removeLast(KEY& OUT out_Key, VALUE& OUT out_Value);

    /**
     * \brief	Returns position of the next entry in the hash-map followed the given position.
     *
     * \param	atPosition  The position of the entry to get next and extract values.
     * \param	out_Key     On output, this contains key of given position.
     * \param	out_Value   On output, this contains value of given position.
     * \param	out_Element On output, this element contains pair of Key and Value specified by given position.
     * \return	Next valid position in the hash-map or invalid position if reached end of hash-map.
     **/
    inline MAPPOS nextPosition(MAPPOS IN atPosition) const;
    inline MAPPOS nextPosition(MAPPOS IN atPosition, KEY & OUT out_Key, VALUE & OUT out_Value ) const;
    inline MAPPOS nextPosition(MAPPOS IN atPosition, std::pair<KEY, VALUE> & OUT out_Element ) const;

    /**
     * \brief	Extract data of the key and value of the entry by given position.
     * \param	atPosition	The position of the element to extract key and value.
     * \param	out_Key	    On output, contains key of the element at given position.
     * \param	out_Value   On output, contains value of the element at given position.
     * \param	out_Element On output, contains the Key and Value pair of the element at given position
     **/
    inline void getAtPosition(MAPPOS IN atPosition, KEY & OUT out_Key, VALUE & OUT out_Value ) const;
    inline void getAtPosition(MAPPOS IN atPosition, std::pair<KEY, VALUE> & OUT out_Element) const;

    /**
     * \brief   Returns the Key of the entry at the given position.
     * \param   atPosition  The position of the element.
     **/
    inline const KEY & keyAtPosition(const MAPPOS atPosition ) const;
    inline KEY& keyAtPosition(MAPPOS atPosition);

    /**
     * \brief   Returns the Value of the entry at the given position.
     * \param   atPosition  The position of the element.
     **/
    inline const VALUE & valueAtPosition(const MAPPOS atPosition ) const;
    inline VALUE& valueAtPosition(MAPPOS atPosition);

    /**
     * \brief	Extracts next position, key and value of the element in the hash-map followed position.
     *
     * \param	in_out_NextPosition	On input this indicates the valid position of the entry in the hash map.
     *                              On output, this parameter points either next valid entry in the hash-map
     *                              or invalid entry if no more entry is following.
     * \param	out_NextKey	        On output, this contains key of the next entry in hash map.
     * \param	out_NextValue       On output, this contain value of the next entry in hash map.
     * \return	Returns true, if there is a next element and the output values are valid.
     **/
    inline bool nextEntry(MAPPOS & IN OUT in_out_NextPosition, KEY & OUT out_NextKey, VALUE & OUT out_NextValue ) const;

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
    : Constless<std::unordered_map<KEY, VALUE>>( )
    , mValueList(hashSize)
{
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::operator == (const TEHashMap<KEY, VALUE>& other) const
{
    return (mValueList == other.mValueList);
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::operator != ( const TEHashMap<KEY, VALUE>& other ) const
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
    auto pos = mValueList.begin();
    return Constless<std::unordered_map<KEY, VALUE>>::iter(mValueList, pos);
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::isStartPosition(const MAPPOS pos) const
{
    return (pos == mValueList.begin());
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::invalidPosition(void) const
{
	auto end = mValueList.end();
    return Constless<std::unordered_map<KEY, VALUE>>::iter(mValueList, end);
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
    auto it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
    {
        ++it;
    }

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
inline bool TEHashMap<KEY, VALUE>::find( const KEY & Key, VALUE & OUT out_Value ) const
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.find(Key);
        if (pos != mValueList.end())
        {
            out_Value = pos->second;
            result = true;
        }
    }

    return result;
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::find(const KEY& Key) const
{
    auto cit = mValueList.empty() ? mValueList.end() : mValueList.find(Key);
    return Constless<std::unordered_map<KEY, VALUE>>::iter(mValueList, cit);
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
inline void TEHashMap<KEY, VALUE>::setAt( KEY && Key, VALUE && newValue)
{
    mValueList[Key] = std::move(newValue);
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::setAt(const std::pair<KEY, VALUE>& element)
{
    setAt(element.first, element.second);
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::setAt( std::pair<KEY, VALUE> && element)
{
    setAt(std::move(element.first), std::move(element.second));
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
inline std::pair<typename TEHashMap<KEY, VALUE>::MAPPOS, bool> TEHashMap<KEY, VALUE>::addIfUnique(const KEY& newKey, const VALUE& newValue)
{
    return mValueList.insert({ newKey, newValue });
}

template < typename KEY, typename VALUE >
inline std::pair<typename TEHashMap<KEY, VALUE>::MAPPOS, bool> TEHashMap<KEY, VALUE>::addIfUnique( KEY && newKey, VALUE && newValue)
{
    return mValueList.insert(std::make_pair(newKey, newValue));
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::updateAt(const KEY & Key, const VALUE & newValue)
{
    MAPPOS pos = mValueList.empty() ? invalidPosition() : mValueList.find(Key);
    if (pos != mValueList.end())
    {
        pos->second = newValue;
    }

    return pos;
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::removeAt(const KEY& Key)
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
inline bool TEHashMap<KEY, VALUE>::removeAt(const KEY & Key, VALUE& out_Value)
{
    bool result = false;
    if (mValueList.empty() == false)
    {
        MAPPOS pos = mValueList.find(Key);
        if (pos != mValueList.end())
        {
            result = true;
            out_Value = pos->second;
            mValueList.erase(pos);
        }
    }

    return result;
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::setPosition(typename TEHashMap<KEY, VALUE>::MAPPOS atPosition, const VALUE & newValue)
{
    ASSERT( atPosition != mValueList.end() );
    atPosition->second = newValue;
    return (++atPosition);
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::removePosition(typename TEHashMap<KEY, VALUE>::MAPPOS IN curPos, KEY& OUT out_Key, VALUE& OUT out_Value)
{
    ASSERT( curPos != mValueList.end());
    out_Key         = curPos->first;
    out_Value       = curPos->second;

    return mValueList.erase(curPos);
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::removePosition(MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::removeFirst(KEY& OUT out_Key, VALUE& OUT out_Value)
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
inline void TEHashMap<KEY, VALUE>::removeFirst( void )
{
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.begin();
        ASSERT(pos != mValueList.end());
        mValueList.erase(pos);
    }
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::removeLast(KEY& OUT out_Key, VALUE& OUT out_Value)
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
inline void TEHashMap<KEY, VALUE>::removeLast(void)
{
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.rbegin();
        ASSERT(pos != mValueList.end());
        mValueList.erase(pos);
    }
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::nextPosition(TEHashMap<KEY, VALUE>::MAPPOS IN atPosition, KEY& OUT out_Key, VALUE& OUT out_Value) const
{
    ASSERT(atPosition != mValueList.end());

    out_Key		    = atPosition->first;
    out_Value	    = atPosition->second;

    return (++ atPosition);
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS
TEHashMap<KEY, VALUE>::nextPosition( TEHashMap<KEY, VALUE>::MAPPOS IN atPosition, std::pair<KEY, VALUE> & OUT out_Element) const
{
    return nextPosition(atPosition, out_Element.mKey, out_Element.mValue);
}

template < typename KEY, typename VALUE >
inline typename TEHashMap<KEY, VALUE>::MAPPOS TEHashMap<KEY, VALUE>::nextPosition(TEHashMap<KEY, VALUE>::MAPPOS IN atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return (++ atPosition);
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
inline void TEHashMap<KEY, VALUE>::getAtPosition(TEHashMap<KEY, VALUE>::MAPPOS IN atPosition, KEY & OUT out_Key, VALUE & OUT out_Value) const
{
    ASSERT(atPosition != mValueList.end());
    out_Key     = atPosition->first;
    out_Value   = atPosition->second;
}

template < typename KEY, typename VALUE >
inline void TEHashMap<KEY, VALUE>::getAtPosition(TEHashMap<KEY, VALUE>::MAPPOS IN atPosition, std::pair<KEY, VALUE> & OUT out_Element) const
{
    getAtPosition(atPosition, out_Element.mKey, out_Element.mValue);
}

template < typename KEY, typename VALUE >
inline const KEY & TEHashMap<KEY, VALUE>::keyAtPosition(const TEHashMap<KEY, VALUE>::MAPPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->first;
}

template < typename KEY, typename VALUE >
inline KEY& TEHashMap<KEY, VALUE>::keyAtPosition(TEHashMap<KEY, VALUE>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return const_cast<KEY &>(atPosition->first);
}

template < typename KEY, typename VALUE >
inline const VALUE & TEHashMap<KEY, VALUE>::valueAtPosition(const TEHashMap<KEY, VALUE>::MAPPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE >
inline VALUE& TEHashMap<KEY, VALUE>::valueAtPosition(TEHashMap<KEY, VALUE>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE >
inline bool TEHashMap<KEY, VALUE>::nextEntry(TEHashMap<KEY, VALUE>::MAPPOS & IN OUT in_out_NextPosition, KEY & OUT out_NextKey, VALUE & OUT out_NextValue) const
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

template < typename K, typename V >
inline const IEInStream & operator >> ( const IEInStream & stream, TEHashMap<K, V> & input )
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
        input.setAt(key, value);
    }

    return stream;
}

template < typename K, typename V >
inline IEOutStream & operator << ( IEOutStream & stream, const TEHashMap<K, V> & output )
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

#endif  // AREG_BASE_TEHASHMAP_HPP
