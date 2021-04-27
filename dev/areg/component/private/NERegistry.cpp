/************************************************************************
 * \file        areg/component/private/NERegistry.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, classes of NCRegistry namespace.
 *
 ************************************************************************/
#include "areg/component/NERegistry.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NECommon.hpp"

//////////////////////////////////////////////////////////////////////////
// NERegistry namespace implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NERegistry namespace Predefined constants
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Predefined invalid Service Entry. 
 **/
AREG_API const NERegistry::CEServiceEntry           NERegistry::INVALID_SERVICE_ENTRY;
/**
 * \brief   Predefined invalid Service List.
 **/
AREG_API const NERegistry::CEServiceList            NERegistry::INVALID_SERVICE_LIST;
/**
 * \brief   Predefined invalid Worker Thread Entry.
 **/
AREG_API const NERegistry::CEWorkerThreadEntry      NERegistry::INVALID_WORKER_THREAD_ENTRY;
/**
 * \brief   Predefined invalid Worker Thread List.
 **/
AREG_API const NERegistry::CEWorkerThreadList       NERegistry::INVALID_WORKER_THREAD_LIST;
/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API const NERegistry::CEDependencyEntry        NERegistry::INVALID_DEPENDENCY_ENTRY;
/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API const NERegistry::CEDependencyList         NERegistry::INVALID_DEPENDENCY_LIST;
/**
 * \brief   Predefined Invalid Component Entry.
 **/
AREG_API const NERegistry::CEComponentEntry         NERegistry::INVALID_COMPONENT_ENTRY;
/**
 * \brief   Predefined invalid Component List.
 **/
AREG_API const NERegistry::CEComponentList          NERegistry::INVALID_COMPONENT_LIST;
/**
 * \brief   Predefined invalid Thread Entry.
 **/
AREG_API const NERegistry::CEComponentThreadEntry   NERegistry::INVALID_THREAD_ENTRY;
/**
 * \brief   Predefined invalid Thread List.
 **/
AREG_API const NERegistry::CEComponentThreadList    NERegistry::INVALID_THREAD_LIST;
/**
 * \brief   Predefined invalid Model.
 **/
AREG_API const NERegistry::CEModel                  NERegistry::INVALID_MODEL;

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEServiceEntry implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEServiceEntry::CEServiceEntry( void )
    : mName     ( )
    , mVersion  ( 0, 0, 0)
{
    ; // do nothing
}

NERegistry::CEServiceEntry::CEServiceEntry( const char* serviceName, unsigned int major, unsigned int minor, unsigned int patch )
    : mName     ( serviceName )
    , mVersion  ( major, minor, patch )
{
    ASSERT( mName.IsEmpty( ) == false );
    ASSERT( mVersion.IsValid( ) );
}

NERegistry::CEServiceEntry::CEServiceEntry( const char* serviceName, const CEVersion& version )
    : mName     (serviceName)
    , mVersion  (version)
{
    ASSERT( mName.IsEmpty() == false );
    ASSERT( mVersion.IsValid() );
}

NERegistry::CEServiceEntry::CEServiceEntry( const NERegistry::CEServiceEntry & src )
    : mName     (src.mName)
    , mVersion  (src.mVersion)
{
    ; // do nothing
}

NERegistry::CEServiceEntry::~CEServiceEntry()
{
    ; // do nothing
}

const NERegistry::CEServiceEntry& NERegistry::CEServiceEntry::operator = ( const NERegistry::CEServiceEntry& src )
{
    if (static_cast<const CEServiceEntry *>(this) != &src)
    {
        mName   = src.mName;
        mVersion= src.mVersion;
    }
    return (*this);
}

bool NERegistry::CEServiceEntry::operator == ( const NERegistry::CEServiceEntry& other ) const
{
    return (this != &other ? mName == other.mName && mVersion == other.mVersion : true);
}

bool NERegistry::CEServiceEntry::IsValid( void ) const
{
    return ( (mName.IsEmpty() == false) && mVersion.IsValid() );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEServiceList implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEServiceList::CEServiceList( void )
    : TEArrayList<NERegistry::CEServiceEntry, const NERegistry::CEServiceEntry &>( 0 )
{
    ; // do nothing
}

NERegistry::CEServiceList::CEServiceList( const NERegistry::CEServiceEntry & entry )
    : TEArrayList<NERegistry::CEServiceEntry, const NERegistry::CEServiceEntry &>( 0 )
{
    if ( entry.IsValid() )
        Add( entry );
}

NERegistry::CEServiceList::CEServiceList( const NERegistry::CEServiceList &src )
    :  TEArrayList<NERegistry::CEServiceEntry, const NERegistry::CEServiceEntry &>(static_cast<const  TEArrayList<NERegistry::CEServiceEntry, const NERegistry::CEServiceEntry &> &>(src))
{
    ; // do nothing
}

NERegistry::CEServiceList::~CEServiceList( void )
{
    ; // do nothing
}

const NERegistry::CEServiceList & NERegistry::CEServiceList::operator = ( const NERegistry::CEServiceList & src )
{
    if ( static_cast<const NERegistry::CEServiceList *>(this) != &src )
    {
        RemoveAll();
        for ( int i = 0; i < src.GetSize(); ++ i )
            Add( src.GetAt(i) );
    }
    return static_cast<const NERegistry::CEServiceList &>(*this);
}

const NERegistry::CEServiceEntry & NERegistry::CEServiceList::GetService( const char* serviceName ) const
{
    int i = SearchService(serviceName);
    return (i == NECommon::InvalidIndex ? NERegistry::INVALID_SERVICE_ENTRY : GetAt(i));
}

int NERegistry::CEServiceList::SearchService( const NERegistry::CEServiceEntry & entry ) const
{
    return SearchService( static_cast<const char *>(entry.mName) );
}

int NERegistry::CEServiceList::SearchService( const char* serviceName ) const
{
    int i = 0;
    for ( ; i < GetSize(); ++ i )
    {
        if (GetAt(i).mName == serviceName)
            break;
    }
    return (i < GetSize() ? i : NECommon::InvalidIndex);
}

bool NERegistry::CEServiceList::IsValid( void ) const
{
    return (GetSize() != 0 );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEWorkerThreadEntry implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEWorkerThreadEntry::CEWorkerThreadEntry( void )
    : mThreadName   ( )
    , mConsumerName ( )
{
    ;   // do nothing
}

NERegistry::CEWorkerThreadEntry::CEWorkerThreadEntry( const char * masterThreadName, const char* workerThreadName, const char * compRoleName, const char* compConsumerName )
    : mThreadName   (NEUtilities::CreateComponentItemName(masterThreadName, workerThreadName))
    , mConsumerName (NEUtilities::CreateComponentItemName(compRoleName, compConsumerName))
{
    ;   // do nothing
}

NERegistry::CEWorkerThreadEntry::CEWorkerThreadEntry( const NERegistry::CEWorkerThreadEntry &src )
    : mThreadName   (src.mThreadName)
    , mConsumerName (src.mConsumerName)
{
    ;   // do nothing
}

NERegistry::CEWorkerThreadEntry::~CEWorkerThreadEntry( void )
{
    ;   // do nothing
}

const NERegistry::CEWorkerThreadEntry & NERegistry::CEWorkerThreadEntry::operator = ( const NERegistry::CEWorkerThreadEntry & src )
{
    if ( static_cast<const NERegistry::CEWorkerThreadEntry *>(this) != &src )
    {
        mThreadName     = src.mThreadName; 
        mConsumerName   = src.mConsumerName;
    }
    return (*this);
}

bool NERegistry::CEWorkerThreadEntry::operator == ( const NERegistry::CEWorkerThreadEntry & other ) const
{
    return ( this != &other ? mThreadName == other.mThreadName && mConsumerName == other.mConsumerName : true );
}

bool NERegistry::CEWorkerThreadEntry::IsValid( void ) const
{
    return ( (mThreadName.IsEmpty() == false) && (mConsumerName.IsEmpty() == false) );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEWorkerThreadList implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEWorkerThreadList::CEWorkerThreadList( void  )
    : TEArrayList<NERegistry::CEWorkerThreadEntry, const NERegistry::CEWorkerThreadEntry&>( 0 )
{
    ; // do nothing
}

NERegistry::CEWorkerThreadList::CEWorkerThreadList( const NERegistry::CEWorkerThreadEntry & entry )
    : TEArrayList<NERegistry::CEWorkerThreadEntry, const NERegistry::CEWorkerThreadEntry&>( 0 )
{
    if ( entry.IsValid() )
        Add( entry );
}

NERegistry::CEWorkerThreadList::CEWorkerThreadList( const NERegistry::CEWorkerThreadList& src )
    : TEArrayList<NERegistry::CEWorkerThreadEntry, const NERegistry::CEWorkerThreadEntry&>(static_cast<const TEArrayList<NERegistry::CEWorkerThreadEntry, const NERegistry::CEWorkerThreadEntry&> &>(src))
{
    ; // do nothing
}

NERegistry::CEWorkerThreadList::~CEWorkerThreadList( void )
{
    ; // do nothing
}

const NERegistry::CEWorkerThreadList & NERegistry::CEWorkerThreadList::operator = ( const NERegistry::CEWorkerThreadList & src )
{
    if ( static_cast<const NERegistry::CEWorkerThreadList *>(this) != &src )
    {
        RemoveAll();
        for ( int i = 0; i < src.GetSize(); ++ i )
            Add( src.GetAt(i) );
    }
    return static_cast<const NERegistry::CEWorkerThreadList &>(*this);
}

const NERegistry::CEWorkerThreadEntry & NERegistry::CEWorkerThreadList::GetWorkerThread( const char* threadName ) const
{
    int index = SearchThread(threadName);
    return (index == NECommon::InvalidIndex ? NERegistry::INVALID_WORKER_THREAD_ENTRY : GetAt(index));
}

int NERegistry::CEWorkerThreadList::SearchThread( const char* threadName ) const
{
    int i = 0;
    for ( ; i < GetSize(); ++ i )
    {
        if (GetAt(i).mThreadName == threadName)
            break;
    }
    return (i < GetSize() ? i : NECommon::InvalidIndex);
}

int NERegistry::CEWorkerThreadList::SearchThread( const NERegistry::CEWorkerThreadEntry& entry ) const
{
    return SearchThread( static_cast<const char *>(entry.mThreadName) );
}

bool NERegistry::CEWorkerThreadList::IsValid( void ) const
{
    return ( GetSize() != 0  );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEDependencyEntry implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEDependencyEntry::CEDependencyEntry( void )
    : mRoleName( )
{
    ; // do nothing
}

NERegistry::CEDependencyEntry::CEDependencyEntry( const char* roleName )
    : mRoleName(roleName)
{
    ; // do nothing
}

NERegistry::CEDependencyEntry::CEDependencyEntry( const NERegistry::CEDependencyEntry& src )
    : mRoleName(src.mRoleName)
{
    ; // do nothing
}

NERegistry::CEDependencyEntry::~CEDependencyEntry( void )
{
    ; // do nothing
}

const NERegistry::CEDependencyEntry & NERegistry::CEDependencyEntry::operator = ( const CEDependencyEntry & src )
{
    mRoleName = src.mRoleName;
    return (*this);
}

bool NERegistry::CEDependencyEntry::operator == ( const NERegistry::CEDependencyEntry & other ) const
{
    return (mRoleName == other.mRoleName);
}

bool NERegistry::CEDependencyEntry::IsValid( void ) const
{
    return (mRoleName.IsEmpty() == false);
}

void NERegistry::CEDependencyEntry::SetDependentService( const char * roleName )
{
    mRoleName   = roleName;
}

const CEString & NERegistry::CEDependencyEntry::GetDepdendentService( void ) const
{
    return mRoleName;
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEDependencyList implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEDependencyList::CEDependencyList( void  )
    : TEArrayList<NERegistry::CEDependencyEntry, const NERegistry::CEDependencyEntry&>( 0 )
{
    ; // do nothing
}

NERegistry::CEDependencyList::CEDependencyList( const NERegistry::CEDependencyEntry & entry  )
    : TEArrayList<NERegistry::CEDependencyEntry, const NERegistry::CEDependencyEntry&>( 0 )
{
    if ( entry.IsValid())
        Add(entry);
}

NERegistry::CEDependencyList::CEDependencyList( const NERegistry::CEDependencyList& src )
    : TEArrayList<NERegistry::CEDependencyEntry, const NERegistry::CEDependencyEntry&>(static_cast<const TEArrayList<NERegistry::CEDependencyEntry, const NERegistry::CEDependencyEntry&> &>(src))
{
    ; // do nothing
}

NERegistry::CEDependencyList::~CEDependencyList( void )
{
    ; // do nothing
}

const NERegistry::CEDependencyList & NERegistry::CEDependencyList::operator = ( const NERegistry::CEDependencyList & src )
{
    if ( static_cast<const NERegistry::CEDependencyList *>(this) != &src )
    {
        RemoveAll();
        for ( int i = 0; i < src.GetSize(); ++ i )
            Add( src.GetAt(i) );
    }
    return static_cast<const NERegistry::CEDependencyList &>(*this);
}

const NERegistry::CEDependencyEntry & NERegistry::CEDependencyList::GetDependency( const char* roleName ) const
{
    int index = SearchDependency(roleName);
    return (index == NECommon::InvalidIndex ? NERegistry::INVALID_DEPENDENCY_ENTRY : GetAt(index) );
}

int NERegistry::CEDependencyList::SearchDependency( const NERegistry::CEDependencyEntry & entry ) const
{
    return SearchDependency( static_cast<const char *>(entry.mRoleName) );
}

int NERegistry::CEDependencyList::SearchDependency( const char* roleName ) const
{
    int i = 0;
    for ( ; i < GetSize(); ++ i )
    {
        if (GetAt(i).mRoleName == roleName)
            break;
    }
    return (i < GetSize() ? i : NECommon::InvalidIndex);
}

bool NERegistry::CEDependencyList::IsValid( void ) const
{
    return (GetSize() != 0);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEComponentEntry implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEComponentEntry::CEComponentEntry( void )
    : mRoleName             ( )

    , mThreadName           ( )
    , mFuncCreate           (NULL)
    , mFuncDelete           (NULL)

    , mSupportedServices    ( )
    , mWorkerThreads        ( )
    , mDependencyServices   ( )

    , mComponentData        ( NEMemory::InvalidElement )
{
    ; // do nothing
}

NERegistry::CEComponentEntry::CEComponentEntry( const char * masterThreadName, const char* roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete )
    : mRoleName             (roleName)

    , mThreadName           (masterThreadName != NULL ? masterThreadName : "")
    , mFuncCreate           (funcCreate)
    , mFuncDelete           (funcDelete)

    , mSupportedServices    ( )
    , mWorkerThreads        ( )
    , mDependencyServices   ( )

    , mComponentData        ( NEMemory::InvalidElement )
{
    ; // do nothing
}

NERegistry::CEComponentEntry::CEComponentEntry( const char * masterThreadName
                                              , const char * roleName
                                              , FuncCreateComponent funcCreate
                                              , FuncDeleteComponent funcDelete
                                              , const NERegistry::CEServiceList & serviceList
                                              , const NERegistry::CEDependencyList & dependencyList
                                              , const NERegistry::CEWorkerThreadList & workerList )
    : mRoleName             (roleName)

    , mThreadName           (masterThreadName != NULL ? masterThreadName : "")
    , mFuncCreate           (funcCreate)
    , mFuncDelete           (funcDelete)

    , mSupportedServices    (serviceList)
    , mWorkerThreads        (workerList)
    , mDependencyServices   (dependencyList)

    , mComponentData        ( NEMemory::InvalidElement )
{
    ; // do nothing
}

NERegistry::CEComponentEntry::CEComponentEntry( const char * masterThreadName
                                              , const char * roleName
                                              , FuncCreateComponent funcCreate
                                              , FuncDeleteComponent funcDelete
                                              , const NERegistry::CEServiceEntry & service
                                              , const NERegistry::CEDependencyEntry & dependency
                                              , const NERegistry::CEWorkerThreadEntry & worker )
    : mRoleName             (roleName)

    , mThreadName           (masterThreadName != NULL ? masterThreadName : "")
    , mFuncCreate           (funcCreate)
    , mFuncDelete           (funcDelete)

    , mSupportedServices    (service)
    , mWorkerThreads        (worker)
    , mDependencyServices   (dependency)

    , mComponentData        ( NEMemory::InvalidElement )
{
    ; // do nothing
}

NERegistry::CEComponentEntry::CEComponentEntry( const NERegistry::CEComponentEntry& src )
    : mRoleName             (src.mRoleName)

    , mThreadName           (src.mThreadName)
    , mFuncCreate           (src.mFuncCreate)
    , mFuncDelete           (src.mFuncDelete)

    , mSupportedServices    (src.mSupportedServices)
    , mWorkerThreads        (src.mWorkerThreads)
    , mDependencyServices   (src.mDependencyServices)

    , mComponentData        ( NEMemory::InvalidElement )
{
    ; // do nothing
}

NERegistry::CEComponentEntry::~CEComponentEntry( void )
{
    mComponentData  = NEMemory::InvalidElement;
}

const NERegistry::CEComponentEntry & NERegistry::CEComponentEntry::operator = ( const NERegistry::CEComponentEntry& src )
{
    if (static_cast<const NERegistry::CEComponentEntry *>(this) != &src)
    {
        mRoleName           = src.mRoleName;

        mThreadName         = src.mThreadName;
        mFuncCreate         = src.mFuncCreate;
        mFuncDelete         = src.mFuncDelete;

        mSupportedServices  = src.mSupportedServices;
        mWorkerThreads      = src.mWorkerThreads;
        mDependencyServices = src.mDependencyServices;

        mComponentData      = src.mComponentData;
    }
    return (*this);
}

bool NERegistry::CEComponentEntry::operator == ( const NERegistry::CEComponentEntry& other ) const
{
    return (this != &other ? mRoleName == other.mRoleName && mThreadName == other.mThreadName && mFuncCreate == other.mFuncCreate && mFuncDelete == other.mFuncDelete : true);
}

void NERegistry::CEComponentEntry::AddSupportedService( const NERegistry::CEServiceEntry& entry )
{
    if (SearchSupportedService(entry) < 0)
        mSupportedServices.Add(entry);
}

void NERegistry::CEComponentEntry::AddSupportedService( const NERegistry::CEServiceList & serviceList )
{
    for ( int i = 0; i < serviceList.GetSize(); ++ i )
        AddSupportedService(serviceList[i]);
}

bool NERegistry::CEComponentEntry::RemoveSupportedService( const char * serviceName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(serviceName) == false )
    {
        for ( int i = 0; (result == false) && (i < mSupportedServices.GetSize( )); ++i )
        {
            if ( mSupportedServices.GetAt(i).mName == serviceName )
            {
                mSupportedServices.RemoveAt(i, 1);
                result = true;
            }
        }
    }
    return result;
}

int NERegistry::CEComponentEntry::SearchSupportedService( const NERegistry::CEServiceEntry& entry ) const
{
    return mSupportedServices.SearchService(entry);
}

int NERegistry::CEComponentEntry::SearchSupportedService( const char* serviceName ) const
{
    return mSupportedServices.SearchService(serviceName);
}

void NERegistry::CEComponentEntry::AddWorkerThread( const NERegistry::CEWorkerThreadEntry& entry )
{
    if (SearchWorkerThread(entry) < 0)
        mWorkerThreads.Add(entry);
}

void NERegistry::CEComponentEntry::AddWorkerThread( const NERegistry::CEWorkerThreadList & workerList )
{
    for ( int i = 0; i < workerList.GetSize(); ++ i )
        AddWorkerThread(workerList[i]);
}

int NERegistry::CEComponentEntry::SearchWorkerThread( const NERegistry::CEWorkerThreadEntry& entry ) const
{
    return mWorkerThreads.SearchThread(entry);
}

int NERegistry::CEComponentEntry::SearchWorkerThread( const char* workerName ) const
{
    return mWorkerThreads.SearchThread(workerName);
}

bool NERegistry::CEComponentEntry::RemoveWorkerThread( const char * workerName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(workerName) == false )
    {
        for ( int i = 0; (result == false) && (i < mWorkerThreads.GetSize()); ++i )
        {
            if ( mWorkerThreads.GetAt(i).mThreadName == workerName )
            {
                mWorkerThreads.RemoveAt(i);
                result = true;
            }
        }
    }
    return result;
}

void NERegistry::CEComponentEntry::AddDependencyService( const NERegistry::CEDependencyEntry& entry )
{
    if (SearchDependencyService(entry) < 0)
        mDependencyServices.Add(entry);
}

void NERegistry::CEComponentEntry::AddDependencyService( const NERegistry::CEDependencyList & dependencyList )
{
    for ( int i = 0; i < dependencyList.GetSize(); ++ i )
        AddDependencyService(dependencyList[i]);
}

int NERegistry::CEComponentEntry::SearchDependencyService( const NERegistry::CEDependencyEntry& entry ) const
{
    return mDependencyServices.SearchDependency(entry);
}

bool NERegistry::CEComponentEntry::RemoveDependencyService( const char * roleName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(roleName) == false )
    {
        for (int i = 0; (result == false) && (i < mDependencyServices.GetSize()); ++i )
        {
            if ( mDependencyServices.GetAt(i).mRoleName == roleName )
            {
                mDependencyServices.RemoveAt(i);
                result = true;
            }
        }
    }
    return result;
}

int NERegistry::CEComponentEntry::SearchDependencyService( const char* roleName ) const
{
    return mDependencyServices.SearchDependency(roleName);
}

const NERegistry::CEServiceList & NERegistry::CEComponentEntry::GetSupportedServices( void ) const
{
    return mSupportedServices;
}

const NERegistry::CEWorkerThreadList & NERegistry::CEComponentEntry::GetWorkerThreads( void ) const
{
    return mWorkerThreads;
}

const NERegistry::CEDependencyList & NERegistry::CEComponentEntry::GetDependencyServices( void ) const
{
    return mDependencyServices;
}

void NERegistry::CEComponentEntry::SetInstanceMethods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete )
{
    mFuncCreate = fnCreate;
    mFuncDelete = fnDelete;
}

bool NERegistry::CEComponentEntry::IsValid( void ) const
{
    return ( (mRoleName.IsEmpty() == false) && (mFuncCreate != NULL) && (mFuncDelete != NULL) );
}

void NERegistry::CEComponentEntry::SetComponentData( NEMemory::uAlign compData )
{
    mComponentData  = compData;
}

NEMemory::uAlign NERegistry::CEComponentEntry::GetComponentData( void ) const
{
    return mComponentData;
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEComponentList implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEComponentList::CEComponentList( void )
    : TEArrayList<NERegistry::CEComponentEntry, const NERegistry::CEComponentEntry&>( 0 )
{
    ; // do nothing
}

NERegistry::CEComponentList::CEComponentList(  const NERegistry::CEComponentEntry & entry )
    : TEArrayList<NERegistry::CEComponentEntry, const NERegistry::CEComponentEntry&>( 0 )
{
    if ( entry.IsValid() )
        Add( entry );
}

NERegistry::CEComponentList::CEComponentList( const NERegistry::CEComponentList& src )
    : TEArrayList<NERegistry::CEComponentEntry, const NERegistry::CEComponentEntry&>(static_cast<const TEArrayList<NERegistry::CEComponentEntry, const NERegistry::CEComponentEntry&> &>(src))
{
    ; // do nothing
}

NERegistry::CEComponentList::~CEComponentList( void )
{
    ; // do nothing
}

const NERegistry::CEComponentList & NERegistry::CEComponentList::operator = ( const NERegistry::CEComponentList & src )
{
    if ( static_cast<const NERegistry::CEComponentList *>(this) != &src )
    {
        RemoveAll();
        for ( int i = 0; i < src.GetSize(); ++ i )
            Add( src.GetAt(i) );
    }
    return static_cast<const NERegistry::CEComponentList &>(*this);
}

const NERegistry::CEComponentEntry & NERegistry::CEComponentList::GetComponent( const char* roleName ) const
{
    int index = SearchComponent(roleName);
    return (index != NECommon::InvalidIndex ? GetAt(index) : NERegistry::INVALID_COMPONENT_ENTRY);
}

int NERegistry::CEComponentList::SearchComponent(const char* roleName) const
{
    int i = 0;
    for ( ; i < GetSize(); ++ i )
    {
        if (GetAt(i).mRoleName == roleName)
            break;
    }
    return (i < GetSize() ? i : NECommon::InvalidIndex);
}

int NERegistry::CEComponentList::SearchComponent( const NERegistry::CEComponentEntry& entry ) const
{
    return SearchComponent( static_cast<const char *>(entry.mRoleName) );
}

bool NERegistry::CEComponentList::IsValid( void ) const
{
    return (GetSize() != 0);
}

bool NERegistry::CEComponentList::SetComponentData( const char * roleName, NEMemory::uAlign compData )
{
    bool result = false;
    for ( int i = 0; i < GetSize(); ++ i )
    {
        NERegistry::CEComponentEntry & entry = GetAt(i);
        if ( entry.mRoleName == roleName )
        {
            entry.SetComponentData(compData);
            result = true;
            break;
        }
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEComponentThreadEntry implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEComponentThreadEntry::CEComponentThreadEntry( void )
    : mThreadName   ( )
    , mComponents   ( )
{
    ; // do nothing
}

NERegistry::CEComponentThreadEntry::CEComponentThreadEntry( const char* threadName )
    : mThreadName   (threadName)
    , mComponents   ( )
{
    ; // do nothing
}

NERegistry::CEComponentThreadEntry::CEComponentThreadEntry( const char* threadName, const NERegistry::CEComponentList& supCompList )
    : mThreadName   (threadName)
    , mComponents   (supCompList)
{
    ; // do nothing
}

NERegistry::CEComponentThreadEntry::CEComponentThreadEntry( const NERegistry::CEComponentThreadEntry& src )
    : mThreadName   (src.mThreadName)
    , mComponents   (src.mComponents)
{
    ; // do nothing
}


NERegistry::CEComponentThreadEntry::~CEComponentThreadEntry( void )
{
    ; // do nothing
}

void NERegistry::CEComponentThreadEntry::AddComponent( const NERegistry::CEComponentEntry & entry )
{
    if (SearchComponentEntry(entry) < 0)
        mComponents.Add(entry);
}

void NERegistry::CEComponentThreadEntry::AddComponent( const NERegistry::CEComponentList & componentList )
{
    for (int i = 0; i < componentList.GetSize(); ++ i )
        AddComponent(componentList[i]);
}

bool NERegistry::CEComponentThreadEntry::RemoveComponent( const char * roleName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(roleName) == false )
    {
        for ( int i = 0; (result == false) && (i < mComponents.GetSize()); ++i )
        {
            if ( mComponents.GetAt(i).mRoleName == roleName )
            {
                mComponents.RemoveAt(i);
                result = true;
            }
        }
    }
    return result;
}

int NERegistry::CEComponentThreadEntry::SearchComponentEntry( const NERegistry::CEComponentEntry & entry ) const
{
    return mComponents.SearchComponent(entry);
}

int NERegistry::CEComponentThreadEntry::SearchComponentEntry( const char* roleName ) const
{
    return mComponents.SearchComponent(roleName);
}

const NERegistry::CEComponentThreadEntry & NERegistry::CEComponentThreadEntry::operator = ( const NERegistry::CEComponentThreadEntry & src )
{
    if (static_cast<const NERegistry::CEComponentThreadEntry *>(this) != &src)
    {
        mThreadName = src.mThreadName;
        mComponents = src.mComponents;
    }
    return (*this);
}

bool NERegistry::CEComponentThreadEntry::operator == ( const NERegistry::CEComponentThreadEntry & other ) const
{
    return (this != &other ? mThreadName == other.mThreadName : true);
}

bool NERegistry::CEComponentThreadEntry::IsValid( void ) const
{
    return ( (mThreadName.IsEmpty() == false) && (mComponents.IsEmpty() == false) );
}

bool NERegistry::CEComponentThreadEntry::SetComponentData( const char * roleName, NEMemory::uAlign compData )
{
    return mComponents.SetComponentData(roleName, compData);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEComponentThreadList implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::CEComponentThreadList::CEComponentThreadList( void )
    : TEArrayList<NERegistry::CEComponentThreadEntry, const NERegistry::CEComponentThreadEntry&>( 0 )
{
    ; // do nothing
}

NERegistry::CEComponentThreadList::CEComponentThreadList( const NERegistry::CEComponentThreadEntry & entry )
    : TEArrayList<NERegistry::CEComponentThreadEntry, const NERegistry::CEComponentThreadEntry&>( 0 )
{
    if ( entry.IsValid() )
        Add(entry);
}

NERegistry::CEComponentThreadList::CEComponentThreadList( const NERegistry::CEComponentThreadList & src )
    : TEArrayList<NERegistry::CEComponentThreadEntry, const NERegistry::CEComponentThreadEntry&>(static_cast<const TEArrayList<NERegistry::CEComponentThreadEntry, const NERegistry::CEComponentThreadEntry&> &>(src))
{
    ; // do nothing
}

NERegistry::CEComponentThreadList::~CEComponentThreadList( void )
{
    ; // do nothing
}

const NERegistry::CEComponentThreadList & NERegistry::CEComponentThreadList::operator = ( const NERegistry::CEComponentThreadList & src )
{
    if ( static_cast<const NERegistry::CEComponentThreadList *>(this) != &src )
    {
        RemoveAll();
        for ( int i = 0; i < src.GetSize(); ++ i )
            Add( src.GetAt(i) );
    }
    return static_cast<const NERegistry::CEComponentThreadList &>(*this);
}

const NERegistry::CEComponentThreadEntry & NERegistry::CEComponentThreadList::GetThread( const char* threadName ) const
{
    int index = SearchThread(threadName);
    return (index != NECommon::InvalidIndex ? GetAt(index) : NERegistry::INVALID_THREAD_ENTRY);
}

int NERegistry::CEComponentThreadList::SearchThread( const NERegistry::CEComponentThreadEntry& entry ) const
{
    return SearchThread( static_cast<const char *>(entry.mThreadName) );
    int i = 0;
    for ( ; i < GetSize(); ++ i )
    {
        if (GetAt(i) == entry)
            break;
    }
    return (i < GetSize() ? i : NECommon::InvalidIndex);
}

int NERegistry::CEComponentThreadList::SearchThread( const char* threadName ) const
{
    int i = 0;
    for ( ; i < GetSize(); ++ i )
    {
        if (GetAt(i).mThreadName == threadName)
            break;
    }
    return (i < GetSize() ? i : NECommon::InvalidIndex);
}

bool NERegistry::CEComponentThreadList::IsValid( void ) const
{
    return (GetSize() != 0);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::CEModel implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::CEModel::CEModel( void )
    : mModelName    ( )
    , mModelThreads ( )
    , mIsLoaded     ( false )
{
    ; // do nothing
}

NERegistry::CEModel::CEModel( const char* modelName )
    : mModelName    (modelName)
    , mModelThreads ( )
    , mIsLoaded     ( false )
{
    ; // do nothing
}

NERegistry::CEModel::CEModel( const char* modelName, const CEComponentThreadList & threadList  )
    : mModelName    (modelName)
    , mModelThreads (threadList)
    , mIsLoaded     ( false )
{
    ; // do nothing
}

NERegistry::CEModel::CEModel( const CEModel & src )
    : mModelName    ( src.mModelName )
    , mModelThreads ( src.mModelThreads )
    , mIsLoaded     ( src.mIsLoaded )
{
    ; // do nothing
}

NERegistry::CEModel::~CEModel()
{
    ; // do nothing
}

const NERegistry::CEModel & NERegistry::CEModel::operator = ( const NERegistry::CEModel & src )
{
    if ( static_cast<const NERegistry::CEModel *>(this) != &src )
    {
        mModelName      = src.mModelName;
        mModelThreads   = src.mModelThreads;
        mIsLoaded       = src.mIsLoaded;
    }
    return static_cast<const NERegistry::CEModel &>(*this);
}

bool NERegistry::CEModel::operator == ( const NERegistry::CEModel & other ) const
{
    return (this != &other ? mModelName == other.mModelName && mModelThreads == other.mModelThreads : true );
}

bool NERegistry::CEModel::IsValid( void ) const
{
    return ( (mModelName.IsEmpty() == false) && mModelThreads.IsValid() );
}

int NERegistry::CEModel::SearchThread( const NERegistry::CEComponentThreadEntry& entry ) const
{
    return mModelThreads.SearchThread(entry);
}

int NERegistry::CEModel::SearchThread( const char* threadName ) const
{
    return mModelThreads.SearchThread(threadName);
}

void NERegistry::CEModel::AddThread( const NERegistry::CEComponentThreadEntry& entry )
{
    if ( entry.IsValid() && (SearchThread(entry) == NECommon::InvalidIndex) )
        mModelThreads.Add(entry);
}

void NERegistry::CEModel::AddThread( const NERegistry::CEComponentThreadList& threadList )
{
    for (int i = 0; i < threadList.GetSize(); ++ i )
        AddThread(threadList[i]);
}

bool NERegistry::CEModel::RemoveThread( const char * threadName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(threadName) == false )
    {
        for ( int i = 0; (result == false) && (i < mModelThreads.GetSize()); ++i )
        {
            if ( mModelThreads.GetAt(i).mThreadName == threadName )
            {
                mModelThreads.RemoveAt(i);
                result = true;
            }
        }
    }
    return result;
}

const CEString & NERegistry::CEModel::GetModelName( void ) const
{
    return mModelName;
}

bool NERegistry::CEModel::HasRegisteredComponent( const NERegistry::CEComponentEntry & entry ) const
{
    int result = NECommon::InvalidIndex;
    for ( int i = 0; (result == NECommon::InvalidIndex) && (i < mModelThreads.GetSize()); ++ i )
        result = mModelThreads.GetAt(i).SearchComponentEntry(entry);
    return ( result < 0 ? false : true );
}

bool NERegistry::CEModel::HasRegisteredComponent( const char * roleName ) const
{
    int result = NECommon::InvalidIndex;
    for ( int i = 0; (result == NECommon::InvalidIndex) && (i < mModelThreads.GetSize()); ++ i )
        result = mModelThreads.GetAt(i).SearchComponentEntry(roleName);
    return ( result < 0 ? false : true );
}

bool NERegistry::CEModel::IsModelLoaded( void ) const
{
    return mIsLoaded;
}

void NERegistry::CEModel::SetModelLoaded( bool isLoaded /*= true */ )
{
    mIsLoaded = isLoaded;
}

const NERegistry::CEComponentThreadList & NERegistry::CEModel::GetThreadList( void ) const
{
    return mModelThreads;
}

bool NERegistry::CEModel::SetComponentData( const char * roleName, NEMemory::uAlign compData )
{
    bool result = false;
    for ( int i = 0; i < mModelThreads.GetSize(); ++ i )
    {
        NERegistry::CEComponentThreadEntry & entry = mModelThreads.GetAt(i);
        if ( entry.SetComponentData(roleName, compData) )
        {
            result = true;
            break;
        }
    }
    return result;
}
