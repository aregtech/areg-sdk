/************************************************************************
 * \file        areg/base/private/EContainers.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Containers
 *
 ************************************************************************/

#include "areg/base/EContainers.hpp"

//////////////////////////////////////////////////////////////////////////
// CEIntegerArray class implementation
//////////////////////////////////////////////////////////////////////////
CEIntegerArray::CEIntegerArray( int capacity /*= 0*/, int increase /*= -1*/ )
    : TEArrayList<unsigned int, unsigned int>  (capacity, increase)
{
    ; // do nothing
}

CEIntegerArray::CEIntegerArray( const CEIntegerArray & src )
    : TEArrayList<unsigned int, unsigned int>  (static_cast<const TEArrayList<unsigned int, unsigned int> &>(src))
{
    ; // do nothing
}

CEIntegerArray::~CEIntegerArray( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEStringArray class implementation
//////////////////////////////////////////////////////////////////////////
CEStringArray::CEStringArray( int capacity /*= 0*/, int increase /*= -1*/ )
    : TEArrayList<CEString, const CEString &>  (capacity, increase)
{
    ; // do nothing
}

CEStringArray::CEStringArray( const CEStringArray & src )
    : TEArrayList<CEString, const CEString &>  (static_cast<const TEArrayList<CEString, const CEString &> &>(src))
{
    ; // do nothing
}

CEStringArray::~CEStringArray( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEPointerArray class implementation
//////////////////////////////////////////////////////////////////////////
CEPointerArray::CEPointerArray( int capacity /*= 0*/, int increase /*= -1*/ )
    : TEArrayList<void *, void *, ImplPointerArray>  (capacity, increase)
{
    ; // do nothing
}

CEPointerArray::CEPointerArray( const CEPointerArray & src )
    : TEArrayList<void *, void *, ImplPointerArray>  (static_cast<const TEArrayList<void *, void *, ImplPointerArray> &>(src))
{
    ; // do nothing
}

CEPointerArray::~CEPointerArray( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEIntegerToIntegerMap class declaration
//////////////////////////////////////////////////////////////////////////

CEIntegerToIntegerMap::CEIntegerToIntegerMap( void )
    : TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap> ( )
{
    ; // do nothing
}

CEIntegerToIntegerMap::CEIntegerToIntegerMap( int blockSize, int hashSize )
    : TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap> ( blockSize, hashSize )
{
    ; // do nothing
}

CEIntegerToIntegerMap::CEIntegerToIntegerMap( const CEIntegerToIntegerMap & src )
    : TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap> ( static_cast<const TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap> &>(src) )
{
    ; // do nothing
}

CEIntegerToIntegerMap::~CEIntegerToIntegerMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEIntegerToStringMap class implementation
//////////////////////////////////////////////////////////////////////////

CEIntegerToStringMap::CEIntegerToStringMap( void )
    : TEIntegerHashMap<CEString, const CEString &, ImplIntegerToStringMap> ( )
{
    ; // do nothing
}

CEIntegerToStringMap::CEIntegerToStringMap( int blockSize, int hashSize )
    : TEIntegerHashMap<CEString, const CEString &, ImplIntegerToStringMap> ( blockSize, hashSize )
{
    ; // do nothing
}

CEIntegerToStringMap::CEIntegerToStringMap( const CEIntegerToStringMap & src )
    : TEIntegerHashMap<CEString, const CEString &, ImplIntegerToStringMap> ( static_cast<const TEIntegerHashMap<CEString, const CEString &, ImplIntegerToStringMap> &>(src) )
{
    ; // do nothing
}

CEIntegerToStringMap::~CEIntegerToStringMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEIntegerToPointergMap class implementation
//////////////////////////////////////////////////////////////////////////

CEIntegerToPointergMap::CEIntegerToPointergMap( void )
    : TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap>   ( )
{
    ; // do nothing
}

CEIntegerToPointergMap::CEIntegerToPointergMap( int blockSize, int hashSize )
    : TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap>   ( blockSize, hashSize )
{
    ; // do nothing
}

CEIntegerToPointergMap::CEIntegerToPointergMap( const CEIntegerToPointergMap & src )
    : TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap>   ( static_cast<const TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap> &>(src) )
{
    ; // do nothing
}

CEIntegerToPointergMap::~CEIntegerToPointergMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEStringToIntegerMap class implementation
//////////////////////////////////////////////////////////////////////////

CEStringToIntegerMap::CEStringToIntegerMap( void )
    : TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap>  ( )
{
    ; // do nothing
}

CEStringToIntegerMap::CEStringToIntegerMap( int blockSize, int hashSize )
    : TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap>  ( blockSize, hashSize )
{
    ; // do nothing
}

CEStringToIntegerMap::CEStringToIntegerMap( const CEStringToIntegerMap & src )
    : TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap>  ( static_cast<const TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap> &>(src) )
{
    ; // do nothing
}

CEStringToIntegerMap::~CEStringToIntegerMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEStringToStringMap class implementation
//////////////////////////////////////////////////////////////////////////

CEStringToStringMap::CEStringToStringMap( void )
    : TEStringHashMap<CEString, const CEString &, ImplStringToStringMap>  ( )
{
    ; // do nothing
}

CEStringToStringMap::CEStringToStringMap( int blockSize, int hashSize )
    : TEStringHashMap<CEString, const CEString &, ImplStringToStringMap>  ( blockSize, hashSize )
{
    ; // do nothing
}

CEStringToStringMap::CEStringToStringMap( const CEStringToStringMap & src )
    : TEStringHashMap<CEString, const CEString &, ImplStringToStringMap>  ( static_cast<const TEStringHashMap<CEString, const CEString &, ImplStringToStringMap> &>(src) )
{
    ; // do nothing
}

CEStringToStringMap::~CEStringToStringMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEStringToPointergMap class implementation
//////////////////////////////////////////////////////////////////////////

CEStringToPointergMap::CEStringToPointergMap( void )
    : TEStringHashMap<void *, const void *, ImplStringToPointergMap>    ( )
{
    ; // do nothing
}

CEStringToPointergMap::CEStringToPointergMap( int blockSize, int hashSize )
    : TEStringHashMap<void *, const void *, ImplStringToPointergMap>    ( blockSize, hashSize )
{
    ; // do nothing
}

CEStringToPointergMap::CEStringToPointergMap( const CEStringToPointergMap & src )
    : TEStringHashMap<void *, const void *, ImplStringToPointergMap>    ( static_cast<const TEStringHashMap<void *, const void *, ImplStringToPointergMap> &>(src) )
{
    ; // do nothing
}

CEStringToPointergMap::~CEStringToPointergMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEPointerToIntegerMap class implementation
//////////////////////////////////////////////////////////////////////////

CEPointerToIntegerMap::CEPointerToIntegerMap( void )
    : TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap> ( )
{
    ; // do nothing
}

CEPointerToIntegerMap::CEPointerToIntegerMap( int blockSize, int hashSize )
    : TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap> ( blockSize, hashSize )
{
    ; // do nothing
}

CEPointerToIntegerMap::CEPointerToIntegerMap( const CEPointerToIntegerMap & src )
    : TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap> ( static_cast<const TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap> &>(src) )
{
    ; // do nothing
}

CEPointerToIntegerMap::~CEPointerToIntegerMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEPointerToStringMap class implementation
//////////////////////////////////////////////////////////////////////////

CEPointerToStringMap::CEPointerToStringMap( void )
    : TEPointerHashMap<CEString, const CEString &, ImplPointerToStringMap> ( )
{
    ; // do nothing
}

CEPointerToStringMap::CEPointerToStringMap( int blockSize, int hashSize )
    : TEPointerHashMap<CEString, const CEString &, ImplPointerToStringMap> ( blockSize, hashSize )
{
    ; // do nothing
}

CEPointerToStringMap::CEPointerToStringMap( const CEPointerToStringMap & src )
    : TEPointerHashMap<CEString, const CEString &, ImplPointerToStringMap> ( static_cast<const TEPointerHashMap<CEString, const CEString &, ImplPointerToStringMap> &>(src) )
{
    ; // do nothing
}

CEPointerToStringMap::~CEPointerToStringMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEPointerToPointergMap class implementation
//////////////////////////////////////////////////////////////////////////

CEPointerToPointergMap::CEPointerToPointergMap( void )
    : TEPointerHashMap<void *, const void *, ImplPointerToPointergMap>   ( )
{
    ; // do nothing
}

CEPointerToPointergMap::CEPointerToPointergMap( int blockSize, int hashSize )
    : TEPointerHashMap<void *, const void *, ImplPointerToPointergMap>   ( blockSize, hashSize )
{
    ; // do nothing
}

CEPointerToPointergMap::CEPointerToPointergMap( const CEPointerToPointergMap & src )
    : TEPointerHashMap<void *, const void *, ImplPointerToPointergMap>   ( static_cast<const TEPointerHashMap<void *, const void *, ImplPointerToPointergMap> &>(src) )
{
    ; // do nothing
}

CEPointerToPointergMap::~CEPointerToPointergMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEIntegerList class implementation
//////////////////////////////////////////////////////////////////////////

CEIntegerList::CEIntegerList( void )
    : TELinkedList<unsigned int, unsigned int, ImplIntegerList> ( )
{
    ; // do nothing
}

CEIntegerList::CEIntegerList( const CEIntegerList & src )
    : TELinkedList<unsigned int, unsigned int, ImplIntegerList> ( static_cast<const TELinkedList<unsigned int, unsigned int, ImplIntegerList> &>(src) )
{
    ; // do nothing
}

CEIntegerList::~CEIntegerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEStringList class implementation
//////////////////////////////////////////////////////////////////////////

CEStringList::CEStringList( void )
    : TELinkedList<CEString, const CEString &, ImplStringList> ( )
{
    ; // do nothing
}

CEStringList::CEStringList( const CEStringList & src )
    : TELinkedList<CEString, const CEString &, ImplStringList> ( static_cast<const TELinkedList<CEString, const CEString &, ImplStringList> &>(src) )
{
    ; // do nothing
}

CEStringList::~CEStringList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEPointerList class implementation
//////////////////////////////////////////////////////////////////////////

CEPointerList::CEPointerList( void )
    : TELinkedList<void *, const void *, ImplPointerList>   ( )
{
    ; // do nothing
}

CEPointerList::CEPointerList( const CEPointerList & src )
    : TELinkedList<void *, const void *, ImplPointerList>   ( static_cast<const TELinkedList<void *, const void *, ImplPointerList> &>(src) )
{
    ; // do nothing
}

CEPointerList::~CEPointerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CESortedIntegerList class declaration
//////////////////////////////////////////////////////////////////////////

CESortedIntegerList::CESortedIntegerList( bool sortAcceding /*= true */ )
    : TESortedLinkedList<unsigned int, unsigned int, TESortImpl<unsigned int>>   ( sortAcceding )
{
    ; // do nothing
}

CESortedIntegerList::CESortedIntegerList( const CESortedIntegerList & src )
    : TESortedLinkedList<unsigned int, unsigned int, TESortImpl<unsigned int>>   ( static_cast<const TESortedLinkedList<unsigned int, unsigned int, TESortImpl<unsigned int>> &>(src) )
{
    ; // do nothing
}

CESortedIntegerList::~CESortedIntegerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CESortedStringList class declaration
//////////////////////////////////////////////////////////////////////////

CESortedStringList::CESortedStringList( bool sortAcceding /*= true*/ )
    : TESortedLinkedList<CEString, const CEString &, CESortedStringListImpl>   ( sortAcceding )
{
    ; // do nothing
}

CESortedStringList::CESortedStringList(const CESortedStringList & src)
    : TESortedLinkedList<CEString, const CEString &, CESortedStringListImpl>   ( static_cast<const TESortedLinkedList<CEString, const CEString &, CESortedStringListImpl> &>(src) )
{
    ; // do nothing
}

CESortedStringList::~CESortedStringList( void )
{
    ; // do nothing
}


//////////////////////////////////////////////////////////////////////////
// CETokenizer class implementation
//////////////////////////////////////////////////////////////////////////

CETokenizer::CETokenizer( void )
{
    // do nothing
}

CETokenizer::CETokenizer( const CEString & str, const CEString & delimiters, bool keepEmpty/*=true*/)
{
    Tokenize(str, delimiters, keepEmpty);
}

CETokenizer::CETokenizer( const CETokenizer & src )
    : CEStringArray(src)
{
    // do nothing
}


CETokenizer::~CETokenizer( void )
{
    // do nothing
}


void CETokenizer::Tokenize( const CEString & str, const CEString & delimiters, bool keepEmpty/*=true*/)
{
    unsigned int lastPos= 0;
    unsigned int length = str.GetLength();
    // empty self
    RemoveAll();
    while (lastPos <= length)
    {
        unsigned int pos = str.FindOneOf(delimiters, lastPos);
        if (pos == NEString::InvalidPos)
           pos = length;
        if (pos != lastPos || keepEmpty)
            Add(str.Substr(lastPos, pos - lastPos));
        lastPos = pos + 1;
    }
}

