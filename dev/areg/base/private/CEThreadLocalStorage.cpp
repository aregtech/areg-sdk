/************************************************************************
 * \file        areg/base/private/CEThreadLocalStorage.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread Local Storage class.
 *              Should be initialized when thread starts running.
 *
 ************************************************************************/
#include "areg/base/CEThreadLocalStorage.hpp"
#include "areg/base/CEThread.hpp"

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage::CEStorageItem class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage::CEStorageItem class constructors / destructor
//////////////////////////////////////////////////////////////////////////
CEThreadLocalStorage::CEStorageItem::CEStorageItem( void )
    : TEPair<CEString, NEMemory::uAlign, const CEString&, NEMemory::uAlign, ImplStorageItem>   ( )
{
    ; // do nothing
}

CEThreadLocalStorage::CEStorageItem::CEStorageItem( const CEString & Key, NEMemory::uAlign Value )
    : TEPair<CEString, NEMemory::uAlign, const CEString &, NEMemory::uAlign, ImplStorageItem> (Key, Value)
{
    ; // do nothing
}

CEThreadLocalStorage::CEStorageItem::CEStorageItem( const CEThreadLocalStorage::CEStorageItem & src )
    : TEPair<CEString, NEMemory::uAlign, const CEString &, NEMemory::uAlign, ImplStorageItem> ( static_cast< const TEPair<CEString, NEMemory::uAlign, const CEString&, NEMemory::uAlign, ImplStorageItem> &>(src) )
{
    ; // do nothing
}

CEThreadLocalStorage::CEStorageItem::~CEStorageItem( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage::CEStorageItem class operator
//////////////////////////////////////////////////////////////////////////
const CEThreadLocalStorage::CEStorageItem & CEThreadLocalStorage::CEStorageItem::operator = ( const CEThreadLocalStorage::CEStorageItem & src )
{
    static_cast< TEPair<CEString, NEMemory::uAlign, const CEString &, NEMemory::uAlign, ImplStorageItem> &>(*this) = static_cast<const TEPair<CEString, NEMemory::uAlign, const CEString &, NEMemory::uAlign, ImplStorageItem> &>(src);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage::CEStorageList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage::CEStorageList class constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEThreadLocalStorage::CEStorageList::CEStorageList( void )
    : TELinkedList<CEThreadLocalStorage::CEStorageItem, const CEThreadLocalStorage::CEStorageItem &, ImplStorageList>( )
{
    ; // do nothing
}

CEThreadLocalStorage::CEStorageList::~CEStorageList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage class constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEThreadLocalStorage::CEThreadLocalStorage(CEThread & owningThread)
    : mStorageList  ( )
    , mOwningThread (owningThread)
{
    ; // do nothing
}

CEThreadLocalStorage::~CEThreadLocalStorage( void )
{
    mStorageList.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage class methods
//////////////////////////////////////////////////////////////////////////
NEMemory::uAlign CEThreadLocalStorage::GetStorageItem( const char * Key ) const
{
    NEMemory::uAlign result = NEMemory::InvalidElement;

    LISTPOS pos = mStorageList.GetHeadPosition();
    for ( ; pos != NULL; pos = mStorageList.GetNextPosition(pos))
    {
        if (mStorageList.GetAt(pos).mKey == Key)
        {
            result = mStorageList.GetAt(pos).mValue;
            break;
        }
    }

    return result;
}

void CEThreadLocalStorage::SetStorageItem(const char* Key, NEMemory::uAlign Value)
{
    mStorageList.AddHead(CEThreadLocalStorage::CEStorageItem(Key, Value));
}

void CEThreadLocalStorage::SetStorageItem( const char* Key, const void* Value )
{
    NEMemory::uAlign aln;
    aln.alignPtr.mElement = (void *)(Value);
    SetStorageItem(Key, aln);
}

void CEThreadLocalStorage::SetStorageItem( const char * Key, unsigned int Value )
{
    NEMemory::uAlign aln;
    aln.alignUInt.mElement = Value;
    SetStorageItem(Key, aln);
}

void CEThreadLocalStorage::SetStorageItem( const char * Key, uint64_t Value )
{
    NEMemory::uAlign aln;
    aln.alignUInt64.mElement = Value;
    SetStorageItem(Key, aln);
}

void CEThreadLocalStorage::SetStorageItem( const char * Key, double Value )
{
    NEMemory::uAlign aln;
    aln.alignDouble.mElement = Value;
    SetStorageItem(Key, aln);
}

NEMemory::uAlign CEThreadLocalStorage::RemoveStoragteItem( const char * Key )
{
    NEMemory::uAlign result = {0};
    LISTPOS pos = mStorageList.GetHeadPosition();
    for ( ; pos != NULL; pos = mStorageList.GetNextPosition(pos))
    {
        if (mStorageList.GetAt(pos).mKey == Key)
        {
            result = mStorageList.RemoveAt(pos).mValue;
            break;
        }
    }
    return result;
}

bool CEThreadLocalStorage::KeyExist( const char* Key ) const
{
    LISTPOS pos = mStorageList.GetHeadPosition();
    for ( ; pos != NULL; pos = mStorageList.GetNextPosition(pos))
    {
        if (mStorageList.GetAt(pos).mKey == Key)
            break;
    }
    return (pos != static_cast<LISTPOS>(NULL));
}

const char* CEThreadLocalStorage::GetName( void ) const
{
    return mOwningThread.GetThreadName().GetBuffer();
}
