/************************************************************************
 * \file        areg/base/private/EContainers.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Containers
 *
 ************************************************************************/

#include "areg/base/EContainers.hpp"

//////////////////////////////////////////////////////////////////////////
// IntegerArray class implementation
//////////////////////////////////////////////////////////////////////////
IntegerArray::IntegerArray( int capacity /*= 0*/, int increase /*= -1*/ )
    : TEArrayList<unsigned int, unsigned int>  (capacity, increase)
{
    ; // do nothing
}

IntegerArray::IntegerArray( const IntegerArray & src )
    : TEArrayList<unsigned int, unsigned int>  (static_cast<const TEArrayList<unsigned int, unsigned int> &>(src))
{
    ; // do nothing
}

IntegerArray::~IntegerArray( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// StringArray class implementation
//////////////////////////////////////////////////////////////////////////
StringArray::StringArray( int capacity /*= 0*/, int increase /*= -1*/ )
    : TEArrayList<String, const String &>  (capacity, increase)
{
    ; // do nothing
}

StringArray::StringArray( const StringArray & src )
    : TEArrayList<String, const String &>  (static_cast<const TEArrayList<String, const String &> &>(src))
{
    ; // do nothing
}

StringArray::~StringArray( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PointerArray class implementation
//////////////////////////////////////////////////////////////////////////
PointerArray::PointerArray( int capacity /*= 0*/, int increase /*= -1*/ )
    : TEArrayList<void *, void *, ImplPointerList>  (capacity, increase)
{
    ; // do nothing
}

PointerArray::PointerArray( const PointerArray & src )
    : TEArrayList<void *, void *, ImplPointerList>  (static_cast<const TEArrayList<void *, void *, ImplPointerList> &>(src))
{
    ; // do nothing
}

PointerArray::~PointerArray( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// IntegerToIntegerMap class declaration
//////////////////////////////////////////////////////////////////////////

IntegerToIntegerMap::IntegerToIntegerMap( void )
    : TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap> ( )
{
    ; // do nothing
}

IntegerToIntegerMap::IntegerToIntegerMap( int blockSize, int hashSize )
    : TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap> ( blockSize, hashSize )
{
    ; // do nothing
}

IntegerToIntegerMap::IntegerToIntegerMap( const IntegerToIntegerMap & src )
    : TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap> ( static_cast<const TEIntegerHashMap<unsigned int, unsigned int, ImplIntegerToIntegerMap> &>(src) )
{
    ; // do nothing
}

IntegerToIntegerMap::~IntegerToIntegerMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// IntegerToStringMap class implementation
//////////////////////////////////////////////////////////////////////////

IntegerToStringMap::IntegerToStringMap( void )
    : TEIntegerHashMap<String, const String &, ImplIntegerToStringMap> ( )
{
    ; // do nothing
}

IntegerToStringMap::IntegerToStringMap( int blockSize, int hashSize )
    : TEIntegerHashMap<String, const String &, ImplIntegerToStringMap> ( blockSize, hashSize )
{
    ; // do nothing
}

IntegerToStringMap::IntegerToStringMap( const IntegerToStringMap & src )
    : TEIntegerHashMap<String, const String &, ImplIntegerToStringMap> ( static_cast<const TEIntegerHashMap<String, const String &, ImplIntegerToStringMap> &>(src) )
{
    ; // do nothing
}

IntegerToStringMap::~IntegerToStringMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// IntegerToPointergMap class implementation
//////////////////////////////////////////////////////////////////////////

IntegerToPointergMap::IntegerToPointergMap( void )
    : TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap>   ( )
{
    ; // do nothing
}

IntegerToPointergMap::IntegerToPointergMap( int blockSize, int hashSize )
    : TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap>   ( blockSize, hashSize )
{
    ; // do nothing
}

IntegerToPointergMap::IntegerToPointergMap( const IntegerToPointergMap & src )
    : TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap>   ( static_cast<const TEIntegerHashMap<void *, const void *, ImplIntegerToPointergMap> &>(src) )
{
    ; // do nothing
}

IntegerToPointergMap::~IntegerToPointergMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// StringToIntegerMap class implementation
//////////////////////////////////////////////////////////////////////////

StringToIntegerMap::StringToIntegerMap( void )
    : TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap>  ( )
{
    ; // do nothing
}

StringToIntegerMap::StringToIntegerMap( int blockSize, int hashSize )
    : TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap>  ( blockSize, hashSize )
{
    ; // do nothing
}

StringToIntegerMap::StringToIntegerMap( const StringToIntegerMap & src )
    : TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap>  ( static_cast<const TEStringHashMap<unsigned int, unsigned int, ImplStringToIntegerMap> &>(src) )
{
    ; // do nothing
}

StringToIntegerMap::~StringToIntegerMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// StringToStringMap class implementation
//////////////////////////////////////////////////////////////////////////

StringToStringMap::StringToStringMap( void )
    : TEStringHashMap<String, const String &, ImplStringToStringMap>  ( )
{
    ; // do nothing
}

StringToStringMap::StringToStringMap( int blockSize, int hashSize )
    : TEStringHashMap<String, const String &, ImplStringToStringMap>  ( blockSize, hashSize )
{
    ; // do nothing
}

StringToStringMap::StringToStringMap( const StringToStringMap & src )
    : TEStringHashMap<String, const String &, ImplStringToStringMap>  ( static_cast<const TEStringHashMap<String, const String &, ImplStringToStringMap> &>(src) )
{
    ; // do nothing
}

StringToStringMap::~StringToStringMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// StringToPointergMap class implementation
//////////////////////////////////////////////////////////////////////////

StringToPointergMap::StringToPointergMap( void )
    : TEStringHashMap<void *, const void *, ImplStringToPointergMap>    ( )
{
    ; // do nothing
}

StringToPointergMap::StringToPointergMap( int blockSize, int hashSize )
    : TEStringHashMap<void *, const void *, ImplStringToPointergMap>    ( blockSize, hashSize )
{
    ; // do nothing
}

StringToPointergMap::StringToPointergMap( const StringToPointergMap & src )
    : TEStringHashMap<void *, const void *, ImplStringToPointergMap>    ( static_cast<const TEStringHashMap<void *, const void *, ImplStringToPointergMap> &>(src) )
{
    ; // do nothing
}

StringToPointergMap::~StringToPointergMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PointerToIntegerMap class implementation
//////////////////////////////////////////////////////////////////////////

PointerToIntegerMap::PointerToIntegerMap( void )
    : TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap> ( )
{
    ; // do nothing
}

PointerToIntegerMap::PointerToIntegerMap( int blockSize, int hashSize )
    : TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap> ( blockSize, hashSize )
{
    ; // do nothing
}

PointerToIntegerMap::PointerToIntegerMap( const PointerToIntegerMap & src )
    : TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap> ( static_cast<const TEPointerHashMap<unsigned int, unsigned int, ImplPointerToIntegerMap> &>(src) )
{
    ; // do nothing
}

PointerToIntegerMap::~PointerToIntegerMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PointerToStringMap class implementation
//////////////////////////////////////////////////////////////////////////

PointerToStringMap::PointerToStringMap( void )
    : TEPointerHashMap<String, const String &, ImplPointerToStringMap> ( )
{
    ; // do nothing
}

PointerToStringMap::PointerToStringMap( int blockSize, int hashSize )
    : TEPointerHashMap<String, const String &, ImplPointerToStringMap> ( blockSize, hashSize )
{
    ; // do nothing
}

PointerToStringMap::PointerToStringMap( const PointerToStringMap & src )
    : TEPointerHashMap<String, const String &, ImplPointerToStringMap> ( static_cast<const TEPointerHashMap<String, const String &, ImplPointerToStringMap> &>(src) )
{
    ; // do nothing
}

PointerToStringMap::~PointerToStringMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PointerToPointergMap class implementation
//////////////////////////////////////////////////////////////////////////

PointerToPointergMap::PointerToPointergMap( void )
    : TEPointerHashMap<void *, const void *, ImplPointerToPointergMap>   ( )
{
    ; // do nothing
}

PointerToPointergMap::PointerToPointergMap( int blockSize, int hashSize )
    : TEPointerHashMap<void *, const void *, ImplPointerToPointergMap>   ( blockSize, hashSize )
{
    ; // do nothing
}

PointerToPointergMap::PointerToPointergMap( const PointerToPointergMap & src )
    : TEPointerHashMap<void *, const void *, ImplPointerToPointergMap>   ( static_cast<const TEPointerHashMap<void *, const void *, ImplPointerToPointergMap> &>(src) )
{
    ; // do nothing
}

PointerToPointergMap::~PointerToPointergMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// IntegerList class implementation
//////////////////////////////////////////////////////////////////////////

IntegerList::IntegerList( void )
    : TELinkedList<unsigned int, unsigned int, ImplIntegerList> ( )
{
    ; // do nothing
}

IntegerList::IntegerList( const IntegerList & src )
    : TELinkedList<unsigned int, unsigned int, ImplIntegerList> ( static_cast<const TELinkedList<unsigned int, unsigned int, ImplIntegerList> &>(src) )
{
    ; // do nothing
}

IntegerList::~IntegerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// StringList class implementation
//////////////////////////////////////////////////////////////////////////

StringList::StringList( void )
    : TELinkedList<String, const String &, ImplStringList> ( )
{
    ; // do nothing
}

StringList::StringList( const StringList & src )
    : TELinkedList<String, const String &, ImplStringList> ( static_cast<const TELinkedList<String, const String &, ImplStringList> &>(src) )
{
    ; // do nothing
}

StringList::~StringList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PointerList class implementation
//////////////////////////////////////////////////////////////////////////

PointerList::PointerList( void )
    : TELinkedList<void *, const void *, ImplPointerList>   ( )
{
    ; // do nothing
}

PointerList::PointerList( const PointerList & src )
    : TELinkedList<void *, const void *, ImplPointerList>   ( static_cast<const TELinkedList<void *, const void *, ImplPointerList> &>(src) )
{
    ; // do nothing
}

PointerList::~PointerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SortedIntegerList class declaration
//////////////////////////////////////////////////////////////////////////

SortedIntegerList::SortedIntegerList( bool sortAcceding /*= true */ )
    : TESortedLinkedList<unsigned int, unsigned int, TESortImpl<unsigned int>>   ( sortAcceding )
{
    ; // do nothing
}

SortedIntegerList::SortedIntegerList( const SortedIntegerList & src )
    : TESortedLinkedList<unsigned int, unsigned int, TESortImpl<unsigned int>>   ( static_cast<const TESortedLinkedList<unsigned int, unsigned int, TESortImpl<unsigned int>> &>(src) )
{
    ; // do nothing
}

SortedIntegerList::~SortedIntegerList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SortedStringList class declaration
//////////////////////////////////////////////////////////////////////////

SortedStringList::SortedStringList( bool sortAcceding /*= true*/ )
    : TESortedLinkedList<String, const String &, SortedStringListImpl>   ( sortAcceding )
{
    ; // do nothing
}

SortedStringList::SortedStringList(const SortedStringList & src)
    : TESortedLinkedList<String, const String &, SortedStringListImpl>   ( static_cast<const TESortedLinkedList<String, const String &, SortedStringListImpl> &>(src) )
{
    ; // do nothing
}

SortedStringList::~SortedStringList( void )
{
    ; // do nothing
}


//////////////////////////////////////////////////////////////////////////
// Tokenizer class implementation
//////////////////////////////////////////////////////////////////////////

Tokenizer::Tokenizer( void )
{
    // do nothing
}

Tokenizer::Tokenizer( const String & str, const String & delimiters, bool keepEmpty/*=true*/)
{
    Tokenize(str, delimiters, keepEmpty);
}

Tokenizer::Tokenizer( const Tokenizer & src )
    : StringArray(src)
{
    // do nothing
}


Tokenizer::~Tokenizer( void )
{
    // do nothing
}


void Tokenizer::Tokenize( const String & str, const String & delimiters, bool keepEmpty/*=true*/)
{
    NEString::CharPos lastPos   = 0;
    NEString::CharCount length  = str.getLength();
    // empty self
    removeAll();
    while (lastPos <= length)
    {
        NEString::CharPos pos = str.findOneOf(delimiters, lastPos);
        if (pos == NEString::InvalidPos)
           pos = length;

        if (pos != lastPos || keepEmpty)
            add(str.substring(lastPos, pos - lastPos));
        
        lastPos = pos + 1;
    }
}

