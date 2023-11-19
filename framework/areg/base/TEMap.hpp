#ifndef AREG_BASE_TEMAP_HPP
#define AREG_BASE_TEMAP_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/TEMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Map class template.
 *              In the map object every element is accessed by the unique key.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TETemplateBase.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEMemory.hpp"

#include <map>

//////////////////////////////////////////////////////////////////////////
// TEMap<KEY, VALUE> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The Map is a sorted associative container that contains key-value
 *          pairs with unique keys. Keys are sorted by comparison. The values 
 *          are accessed by Key. Key and Value can be of different types.
 *
 *          To access first element in the Map, get the first Position (call function
 *          firstPosition()). Each next element is accessed by calling next position.
 *          The type KEY should be possible to compare. If KEY type is an object 
 *          it should have implemented less ( < ) operator or std::less<Key>.
 *          In addition, the KEY and VALUE types should have at least default
 *          constructor and valid assigning operator.
 *
 *          For example:
 *          class MyClass {
 *              int     mData;
 *          }
 *
 *          namespace std {
 *
 *              // comparison
 *              template<> struct less<MyClass> {
 *                  bool operator()( const MyClass& lhs, const MyClass& rhs ) const
 *                  {   return lhs.mData < rhs.mData; }
 *              }
 *          }
 *
 *          The Map object is not thread safe and data should be synchronized manually.
 *
 * \tparam  KEY     The type of Key to identify entries in the sorted map. Either should
 *                  be primitive or should have at least default and copy constructors,
 *                  and assigning operator. There should be as well possibility to compare
 *                  KEY type when calling std::less<KEY>().
 * \tparam  VALUE   The type of stored items. Either should be primitive or should have
 *                  default constructor and valid assigning operator. Also, should be
 *                  possible to convert to type 'const VALUE&'.
 **/
template < typename KEY, typename VALUE>
class TEMap : protected Constless<std::map<KEY, VALUE>>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    //! Position in the sorted map
    using MAPPOS    = typename std::map<KEY, VALUE>::iterator;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Creates empty map.
     **/
    TEMap( void ) = default;

    /**
     * \brief   Copies entries from given source.
     * \param   src     The source to copy data.
     **/
    TEMap( const TEMap<KEY, VALUE> & src ) = default;

    /**
     * \brief   Moves entries from given source.
     * \param   src     The source to move data.
     **/
    TEMap( TEMap<KEY, VALUE> && src ) noexcept = default;

    /**
     * \brief   Destructor.
     **/
    ~TEMap( void ) = default;

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
     *          If sorted map previously had values, they will be removed and new values
     *          from source will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of map.
     **/
    inline TEMap<KEY, VALUE>& operator = ( const TEMap<KEY, VALUE> & src ) = default;

    /**
     * \brief   Move operator. Moves all values from given source.
     *          If sorted map previously had values, they will be removed and new values
     *          from source will be set in the same sequence as they are
     *          present in the source.
     * \param   src     The source of map.
     **/
    inline TEMap<KEY, VALUE>& operator = ( TEMap<KEY, VALUE> && src ) noexcept = default;

    /**
     * \brief   Checks equality of 2 map objects, and returns true if they are equal.
     *          There should be possible to compare KEY and VALUE type entries of sorted map.
     * \param   other   The map object to compare.
     **/
    inline bool operator == ( const TEMap<KEY, VALUE> & other ) const;

    /**
     * \brief   Checks inequality of 2 map objects, and returns true if they are not equal.
     *          There should be possible to compare KEY and VALUE type entries of sorted map.
     * \param   other   The map object to compare.
     **/
    inline bool operator != ( const TEMap<KEY, VALUE> & other ) const;

/************************************************************************/
// Friend global operators to make Sorted Map streamable
/************************************************************************/

    /**
     * \brief   Reads out from the stream the key and value pairs of the map.
     *          If map previously had values, they will be removed and new values
     *          from the stream will be set in the same sequence as they are present
     *          in the stream. There should be possibility to initialize values from
     *          streaming object and if KEY or VALUE are not primitives, but an object,
     *          they should have implemented streaming operator.
     * \param   stream  The streaming object to read values.
     * \param   input   The sorted map object to save initialized values.
     **/
    template < typename K, typename V >
    friend inline const IEInStream & operator >> ( const IEInStream & stream, TEMap<K, V> & input);

    /**
     * \brief   Writes to the stream the key and value pairs of the map.
     *          The values will be written to the stream starting from firs entry.
     *          There should be possibility to stream key and value pairs and if KEY or VALUE
     *          are not primitives, but an object, they should have implemented streaming operator.
     * \param   stream  The stream to write values.
     * \param   input   The sorted map object containing value to stream.
     **/
    template < typename K, typename V >
    friend inline IEOutStream & operator << ( IEOutStream & stream, const TEMap<K, V> & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the sorted map is empty and has no elements.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief	Returns the current size of the map.
     **/
    inline uint32_t getSize( void ) const;

    /**
     * \brief	Returns the position of the first key and value entry in the sorted map, which is
     *          not invalid if the map is not empty. Otherwise, returns invalid position.
     **/
    inline MAPPOS firstPosition(void) const;

    /**
     * \brief   Returns true if specified position points the first entry in the sorted map.
     * \param   pos     The position to check.
     **/
    inline bool isStartPosition(const MAPPOS pos) const;

    /**
     * \brief   Returns the invalid position of the sorted map.
     **/
    inline MAPPOS invalidPosition(void) const;

    /**
     * \brief   Returns true if specified position is invalid, i.e. points the end of the sorted map.
     **/
    inline bool isInvalidPosition(const MAPPOS pos) const;

    /**
     * \brief   Returns true if the given position is not pointing the end of the sorted map.
     *          Note, it does not check whether there is a such position existing in the map.
     **/
    inline bool isValidPosition(const MAPPOS pos) const;

    /**
     * \brief   Checks and ensures that specified position is pointing the valid entry in the sorted map.
     *          The duration of checkup depends on the location of the position in the sorted map.
     * \param   pos     The position to check.
     */
    inline bool checkPosition(const MAPPOS pos) const;

    /**
     * \brief	Checks and returns true if the given element exist in the sorted map or not.
     * \param	key	    The key of value to search.
     */
    inline bool contains(const KEY& Key) const;

    /**
     * \brief   Returns the vector object where the data are stored.
     **/
    inline const std::map<KEY, VALUE>& getData(void) const;

/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief   Remove all entries of the map.
     **/
    inline void clear(void);

    /**
     * \brief   Delete extra entries in the map.
     **/
    inline void freeExtra(void);

    /**
     * \brief   Sets the size of the map to zero and deletes all capacity space.
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
     * \brief	Search an element entry by the given key and returns the position in sorted map.
     * \param	Key	    The key to search.
     * \return	Returns valid sorted map position if found an entry by the give key.
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
     * \brief	Update the value of the existing element in the sorted map.
     *          The existence of the entry is checked by the given key.
     *          Creates and inserts new entry if no element with the specified key exists.
     * \param	Key	        The key of element to search or create new entry.
     * \param	newValue	The value of element to set.
     **/
    inline void setAt( const KEY & Key, const VALUE & newValue );
    inline void setAt( KEY && Key, VALUE && newValue);
    /**
     * \brief	Update existing element value or inserts new element in the sorted map.
     * \param	element     The Key and Value pair of element to set or insert.
     **/
    inline void setAt( const std::pair<KEY, VALUE> & element);
    inline void setAt( std::pair<KEY, VALUE> && element);

    /**
     * \brief   Extracts elements from the given source and inserts into the sorted map.
     *          If there is an entry with the key equivalent to the key from source element,
     *          then that element is not extracted from the source and remains unchanged.
     * \param   source  The source of map to merge.
     */
    inline void merge( const TEMap<KEY, VALUE> & source );
    inline void merge( TEMap<KEY, VALUE> && source );

    /**
     * \brief   Adds new entry with the specified key in the sorted map if it is not existing.
     *          If 'updateExisting' parameter is true, it updates the existing key and value.
     *          The method returns pair of value, where it indicates the position of the entry
     *          and the flag, indicating whether it added new entry or not.
     * \param   Key             The key of the entry in the sorted map.
     * \param   Value           The value of the entry in the sorted map.
     * \param   updateExisting  The flag, indicating whether should update the entry with the existing key.
     *                          If true, updates the existing key and value.
     *                          If, for example, 2 objects are compared by the name and not by
     *                          absolute values, setting this parameter true updates the value of the existing entry.
     *                          the existing Key and Value entries.
     * \return  Returns a pair of 'MAPPOS' and 'bool' values, where
     *              -   'MAPPOS' indicates the position of the entry in the sorted map.
     *              -   'bool' equal to 'true' indicates that new entry is created.
     *                  If this value is 'false' no new entry is created. When new entry is created, the existing
     *                  position values can be invalidated.
     **/
    inline std::pair<MAPPOS, bool> addIfUnique(const KEY & newKey, const VALUE & newValue, bool updateExisting = false );
    inline std::pair<MAPPOS, bool> addIfUnique(KEY && newKey, VALUE && newValue, bool updateExisting = false );

    /**
     * \brief   Updates existing element specified by the Key and returns the position in the map.
     *          If Key does not exit, no new entry is created and function returns invalid position.
     * \param   Key         The key of an element in the sorted map to update.
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
     *          entry in the sorted map.
     **/
    inline MAPPOS setPosition(MAPPOS atPosition, const VALUE& newValue );

    /**
     * \brief	Removes an element at the given position. The function returns next position of an entry in the sorted map
     *          or invalid position if removed last element in the map.
     * \param	curPos      The valid position of the element in the sorted map to remove.
     * \param	out_Key     On output, this contains the key of the removed element
     * \param	out_Value   On output, this contains the value of the removed element.
     * \return  Returns valid position of the next entry in the sorted map  or returns invalid position if
     *          removed last element in the map.
     **/
    inline MAPPOS removePosition(MAPPOS atPosition);
    inline MAPPOS removePosition(MAPPOS IN curPos, KEY & OUT out_Key, VALUE & OUT out_Value );

    /**
     * \brief   Removes the first entry in the sorted map.
     * 
     * \param   out_Key     On output it contains the key of the removed element in the sorted map.
     * \param   out_Value   On output it contains the value of the removed element in the sorted map.
     * \return  Returns true if sorted map was not empty and first entry is removed. Otherwise, returns false.
     **/
    inline void removeFirst(void);
    inline bool removeFirst(KEY& OUT out_Key, VALUE& OUT out_Value);

    /**
     * \brief   Removes the last entry in the sorted map.
     *
     * \param   out_Key     On output it contains the key of the removed element in the sorted map.
     * \param   out_Value   On output it contains the value of the removed element in the sorted map.
     * \return  Returns true if sorted map was not empty and last entry is removed. Otherwise, returns false.
     **/
    inline void removeLast(void);
    inline bool removeLast(KEY& OUT out_Key, VALUE& OUT out_Value);

    /**
     * \brief	Returns position of the next entry in the sorted map followed the given position.
     *
     * \param	atPosition  The position of the entry to get next and extract values.
     * \param	out_Key     On output, this contains key of given position.
     * \param	out_Value   On output, this contains value of given position.
     * \param	out_Element On output, this element contains pair of Key and Value specified by given position.
     * \return	Next valid position in the sorted map or invalid position if reached end of sorted map.
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
     * \brief	Extracts next position, key and value of the element in the sorted map followed position.
     *
     * \param	in_out_NextPosition	On input this indicates the valid position of the entry in the sorted map.
     *                              On output, this parameter points either next valid entry in the sorted map
     *                              or invalid entry if no more entry is following.
     * \param	out_NextKey	        On output, this contains key of the next entry in sorted map.
     * \param	out_NextValue       On output, this contain value of the next entry in sorted map.
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
    std::map<KEY, VALUE>  mValueList;
};

//////////////////////////////////////////////////////////////////////////
// Function Implement
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEMap<KEY, VALUE> class template Implement
//////////////////////////////////////////////////////////////////////////

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::operator == (const TEMap<KEY, VALUE>& other) const
{
    return (mValueList == other.mValueList);
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::operator != ( const TEMap<KEY, VALUE>& other ) const
{
    return (mValueList != other.mValueList);
}

template < typename KEY, typename VALUE >
inline VALUE & TEMap<KEY, VALUE>::operator [] (const KEY& Key)
{
    return mValueList[Key];
}

template < typename KEY, typename VALUE >
inline const VALUE & TEMap<KEY, VALUE>::operator [] ( const KEY & Key ) const
{
    return mValueList[Key];
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::isEmpty(void) const
{
    return mValueList.empty();
}

template < typename KEY, typename VALUE >
inline uint32_t TEMap<KEY, VALUE>::getSize( void ) const
{
    return static_cast<uint32_t>(mValueList.size());
}

template < typename KEY, typename VALUE >
inline typename TEMap<KEY, VALUE>::MAPPOS TEMap<KEY, VALUE>::firstPosition( void ) const
{
    auto pos = mValueList.begin();
    return Constless<std::map<KEY, VALUE>>::iter(mValueList, pos);
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::isStartPosition(const MAPPOS pos) const
{
    return (pos == mValueList.begin());
}

template < typename KEY, typename VALUE >
inline typename TEMap<KEY, VALUE>::MAPPOS TEMap<KEY, VALUE>::invalidPosition(void) const
{
	auto end = mValueList.end();
    return Constless<std::map<KEY, VALUE>>::iter(mValueList, end);
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::isInvalidPosition(const MAPPOS pos) const
{
    return (pos == mValueList.end());
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::isValidPosition(const MAPPOS pos) const
{
    return (pos != mValueList.end());
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::checkPosition(const MAPPOS pos) const
{
    auto it = mValueList.begin();
    while ((it != mValueList.end()) && (it != pos))
    {
        ++it;
    }

    return (it != mValueList.end());
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::clear(void)
{
    mValueList.clear();
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::freeExtra(void)
{
    mValueList.clear();
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::release(void)
{
    mValueList.clear();
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::find( const KEY & Key, VALUE & OUT out_Value ) const
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
inline typename TEMap<KEY, VALUE>::MAPPOS TEMap<KEY, VALUE>::find(const KEY& Key) const
{
    auto cit = mValueList.empty() ? mValueList.end() : mValueList.find(Key);
    return Constless<std::map<KEY, VALUE>>::iter(mValueList, cit);
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::contains(const KEY& Key) const
{
    return (mValueList.find(Key) != mValueList.end());
}

template<typename KEY, typename VALUE>
inline const std::map<KEY, VALUE>& TEMap<KEY, VALUE>::getData(void) const
{
    return mValueList;
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::setAt(const KEY & Key, const VALUE & newValue)
{
    mValueList[Key] = newValue;
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::setAt( KEY && Key, VALUE && newValue)
{
    mValueList[Key] = std::move(newValue);
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::setAt(const std::pair<KEY, VALUE>& element)
{
    setAt(element.first, element.second);
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::setAt( std::pair<KEY, VALUE> && element)
{
    setAt(std::move(element.first), std::move(element.second));
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::merge(const TEMap<KEY, VALUE>& source)
{
    mValueList.merge(source.mValueList);
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::merge(TEMap<KEY, VALUE> && source)
{
    mValueList.merge(std::move(source.mValueList));
}

template < typename KEY, typename VALUE >
inline std::pair<typename TEMap<KEY, VALUE>::MAPPOS, bool> TEMap<KEY, VALUE>::addIfUnique(const KEY& newKey, const VALUE& newValue, bool updateExisting /*= false*/ )
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
inline std::pair<typename TEMap<KEY, VALUE>::MAPPOS, bool> TEMap<KEY, VALUE>::addIfUnique( KEY && newKey, VALUE && newValue, bool updateExisting /*= false*/ )
{
    std::pair<MAPPOS, bool> result = mValueList.insert( std::make_pair( newKey, newValue ) );
    if ( updateExisting && (result.second == false) )
    {
        ASSERT( result.first != mValueList.end( ) );
        result.first->second = newValue;
    }

    return result;
}

template < typename KEY, typename VALUE >
inline typename TEMap<KEY, VALUE>::MAPPOS TEMap<KEY, VALUE>::updateAt(const KEY & Key, const VALUE & newValue)
{
    MAPPOS pos = mValueList.empty() ? invalidPosition() : mValueList.find(Key);
    if (pos != mValueList.end())
    {
        pos->second = newValue;
    }

    return pos;
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::removeAt(const KEY& Key)
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
inline bool TEMap<KEY, VALUE>::removeAt(const KEY & Key, VALUE& out_Value)
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
inline typename TEMap<KEY, VALUE>::MAPPOS TEMap<KEY, VALUE>::setPosition(typename TEMap<KEY, VALUE>::MAPPOS atPosition, const VALUE & newValue)
{
    ASSERT( atPosition != mValueList.end() );
    atPosition->second = newValue;
    return (++atPosition);
}

template < typename KEY, typename VALUE >
inline typename TEMap<KEY, VALUE>::MAPPOS TEMap<KEY, VALUE>::removePosition(typename TEMap<KEY, VALUE>::MAPPOS IN curPos, KEY& OUT out_Key, VALUE& OUT out_Value)
{
    ASSERT( curPos != mValueList.end());
    out_Key         = curPos->first;
    out_Value       = curPos->second;

    return mValueList.erase(++curPos);
}

template < typename KEY, typename VALUE >
inline typename TEMap<KEY, VALUE>::MAPPOS TEMap<KEY, VALUE>::removePosition(MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return mValueList.erase(atPosition);
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::removeFirst(KEY& OUT out_Key, VALUE& OUT out_Value)
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
inline void TEMap<KEY, VALUE>::removeFirst( void )
{
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.begin();
        ASSERT(pos != mValueList.end());
        mValueList.erase(pos);
    }
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::removeLast(KEY& OUT out_Key, VALUE& OUT out_Value)
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
inline void TEMap<KEY, VALUE>::removeLast(void)
{
    if (mValueList.empty() == false)
    {
        auto pos = mValueList.rbegin();
        ASSERT(pos != mValueList.end());
        mValueList.erase(pos);
    }
}

template < typename KEY, typename VALUE >
inline typename TEMap<KEY, VALUE>::MAPPOS TEMap<KEY, VALUE>::nextPosition(TEMap<KEY, VALUE>::MAPPOS IN atPosition, KEY& OUT out_Key, VALUE& OUT out_Value) const
{
    ASSERT(atPosition != mValueList.end());

    out_Key		    = atPosition->first;
    out_Value	    = atPosition->second;

    return (++ atPosition);
}

template < typename KEY, typename VALUE >
inline typename TEMap<KEY, VALUE>::MAPPOS
TEMap<KEY, VALUE>::nextPosition( TEMap<KEY, VALUE>::MAPPOS IN atPosition, std::pair<KEY, VALUE> & OUT out_Element) const
{
    return nextPosition(atPosition, out_Element.first, out_Element.second);
}

template < typename KEY, typename VALUE >
inline typename TEMap<KEY, VALUE>::MAPPOS TEMap<KEY, VALUE>::nextPosition(TEMap<KEY, VALUE>::MAPPOS IN atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return (++ atPosition);
}

template < typename KEY, typename VALUE >
inline VALUE & TEMap<KEY, VALUE>::getAt( const KEY & Key )
{
    return mValueList.at(Key);
}

template < typename KEY, typename VALUE >
inline const VALUE & TEMap<KEY, VALUE>::getAt(const KEY & Key) const
{
    return mValueList.at(Key);
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::getAtPosition(TEMap<KEY, VALUE>::MAPPOS IN atPosition, KEY & OUT out_Key, VALUE & OUT out_Value) const
{
    ASSERT(atPosition != mValueList.end());
    out_Key     = atPosition->first;
    out_Value   = atPosition->second;
}

template < typename KEY, typename VALUE >
inline void TEMap<KEY, VALUE>::getAtPosition(TEMap<KEY, VALUE>::MAPPOS IN atPosition, std::pair<KEY, VALUE> & OUT out_Element) const
{
    getAtPosition(atPosition, out_Element.first, out_Element.second);
}

template < typename KEY, typename VALUE >
inline const KEY & TEMap<KEY, VALUE>::keyAtPosition(const TEMap<KEY, VALUE>::MAPPOS atPosition) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->first;
}

template < typename KEY, typename VALUE >
inline KEY& TEMap<KEY, VALUE>::keyAtPosition(TEMap<KEY, VALUE>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return const_cast<KEY &>(atPosition->first);
}

template < typename KEY, typename VALUE >
inline const VALUE & TEMap<KEY, VALUE>::valueAtPosition(const TEMap<KEY, VALUE>::MAPPOS atPosition ) const
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE >
inline VALUE& TEMap<KEY, VALUE>::valueAtPosition(TEMap<KEY, VALUE>::MAPPOS atPosition)
{
    ASSERT(atPosition != mValueList.end());
    return atPosition->second;
}

template < typename KEY, typename VALUE >
inline bool TEMap<KEY, VALUE>::nextEntry(TEMap<KEY, VALUE>::MAPPOS & IN OUT in_out_NextPosition, KEY & OUT out_NextKey, VALUE & OUT out_NextValue) const
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
// TEMap<KEY, VALUE> class friend methods
//////////////////////////////////////////////////////////////////////////

template < typename K, typename V >
inline const IEInStream & operator >> ( const IEInStream & stream, TEMap<K, V> & input )
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
inline IEOutStream & operator << ( IEOutStream & stream, const TEMap<K, V> & output )
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

#endif  // AREG_BASE_TEMAP_HPP
