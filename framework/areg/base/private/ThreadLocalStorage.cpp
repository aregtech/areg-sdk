/************************************************************************
 * \file        areg/base/private/ThreadLocalStorage.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread Local Storage class.
 *              Should be initialized when thread starts running.
 *
 ************************************************************************/
#include "areg/base/ThreadLocalStorage.hpp"
#include "areg/base/Thread.hpp"

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage::StorageItem class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage::StorageItem class constructors / destructor
//////////////////////////////////////////////////////////////////////////
ThreadLocalStorage::StorageItem::StorageItem( void )
    : TEPair<String, NEMemory::uAlign, const String&, NEMemory::uAlign, ImplStorageItem>   ( )
{
    ; // do nothing
}

ThreadLocalStorage::StorageItem::StorageItem( const String & Key, NEMemory::uAlign Value )
    : TEPair<String, NEMemory::uAlign, const String &, NEMemory::uAlign, ImplStorageItem> (Key, Value)
{
    ; // do nothing
}

ThreadLocalStorage::StorageItem::StorageItem( const ThreadLocalStorage::StorageItem & src )
    : TEPair<String, NEMemory::uAlign, const String &, NEMemory::uAlign, ImplStorageItem> ( static_cast< const TEPair<String, NEMemory::uAlign, const String&, NEMemory::uAlign, ImplStorageItem> &>(src) )
{
    ; // do nothing
}

ThreadLocalStorage::StorageItem::~StorageItem( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage::StorageItem class operator
//////////////////////////////////////////////////////////////////////////
const ThreadLocalStorage::StorageItem & ThreadLocalStorage::StorageItem::operator = ( const ThreadLocalStorage::StorageItem & src )
{
    static_cast< TEPair<String, NEMemory::uAlign, const String &, NEMemory::uAlign, ImplStorageItem> &>(*this) = static_cast<const TEPair<String, NEMemory::uAlign, const String &, NEMemory::uAlign, ImplStorageItem> &>(src);
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage::StorageList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage::StorageList class constructor / destructor
//////////////////////////////////////////////////////////////////////////
ThreadLocalStorage::StorageList::StorageList( void )
    : TELinkedList<ThreadLocalStorage::StorageItem, const ThreadLocalStorage::StorageItem &, ImplStorageList>( )
{
    ; // do nothing
}

ThreadLocalStorage::StorageList::~StorageList( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class constructor / destructor
//////////////////////////////////////////////////////////////////////////
ThreadLocalStorage::ThreadLocalStorage(Thread & owningThread)
    : mStorageList  ( )
    , mOwningThread (owningThread)
{
    ; // do nothing
}

ThreadLocalStorage::~ThreadLocalStorage( void )
{
    mStorageList.removeAll();
}

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class methods
//////////////////////////////////////////////////////////////////////////
NEMemory::uAlign ThreadLocalStorage::getStorageItem( const char * Key ) const
{
    NEMemory::uAlign result = NEMemory::InvalidElement;

    LISTPOS pos = mStorageList.firstPosition();
    for ( ; pos != NULL; pos = mStorageList.nextPosition(pos))
    {
        if (mStorageList.getAt(pos).mKey == Key)
        {
            result = mStorageList.getAt(pos).mValue;
            break;
        }
    }

    return result;
}

void ThreadLocalStorage::setStorageItem(const char* Key, NEMemory::uAlign Value)
{
    mStorageList.pushFirst(ThreadLocalStorage::StorageItem(Key, Value));
}

void ThreadLocalStorage::setStorageItem( const char* Key, const void* Value )
{
    NEMemory::uAlign aln;
    aln.alignPtr.mElement = (void *)(Value);
    setStorageItem(Key, aln);
}

void ThreadLocalStorage::setStorageItem( const char * Key, unsigned int Value )
{
    NEMemory::uAlign aln;
    aln.alignUInt.mElement = Value;
    setStorageItem(Key, aln);
}

void ThreadLocalStorage::setStorageItem( const char * Key, uint64_t Value )
{
    NEMemory::uAlign aln;
    aln.alignUInt64.mElement = Value;
    setStorageItem(Key, aln);
}

void ThreadLocalStorage::setStorageItem( const char * Key, double Value )
{
    NEMemory::uAlign aln;
    aln.alignDouble.mElement = Value;
    setStorageItem(Key, aln);
}

NEMemory::uAlign ThreadLocalStorage::removeStoragteItem( const char * Key )
{
    NEMemory::uAlign result = {0};
    LISTPOS pos = mStorageList.firstPosition();
    for ( ; pos != NULL; pos = mStorageList.nextPosition(pos))
    {
        if (mStorageList.getAt(pos).mKey == Key)
        {
            result = mStorageList.removeAt(pos).mValue;
            break;
        }
    }
    return result;
}

bool ThreadLocalStorage::existKey( const char* Key ) const
{
    LISTPOS pos = mStorageList.firstPosition();
    for ( ; pos != NULL; pos = mStorageList.nextPosition(pos))
    {
        if (mStorageList.getAt(pos).mKey == Key)
            break;
    }
    return (pos != static_cast<LISTPOS>(NULL));
}

const char* ThreadLocalStorage::getName( void ) const
{
    return mOwningThread.getName().getString();
}
