/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/NERegistry.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, classes of NCRegistry namespace.
 *
 ************************************************************************/
#include "areg/component/NERegistry.hpp"
#include "areg/component/Component.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NECommon.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// NERegistry namespace implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NERegistry namespace Predefined constants
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Predefined invalid Service Entry. 
 **/
AREG_API const NERegistry::ServiceEntry           NERegistry::INVALID_SERVICE_ENTRY;
/**
 * \brief   Predefined invalid Service List.
 **/
AREG_API const NERegistry::ServiceList            NERegistry::INVALID_SERVICE_LIST;
/**
 * \brief   Predefined invalid Worker Thread Entry.
 **/
AREG_API const NERegistry::WorkerThreadEntry      NERegistry::INVALID_WORKER_THREAD_ENTRY;
/**
 * \brief   Predefined invalid Worker Thread List.
 **/
AREG_API const NERegistry::WorkerThreadList       NERegistry::INVALID_WORKER_THREAD_LIST;
/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API const NERegistry::DependencyEntry        NERegistry::INVALID_DEPENDENCY_ENTRY;
/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API const NERegistry::DependencyList         NERegistry::INVALID_DEPENDENCY_LIST;
/**
 * \brief   Predefined Invalid Component Entry.
 **/
AREG_API const NERegistry::ComponentEntry         NERegistry::INVALID_COMPONENT_ENTRY;
/**
 * \brief   Predefined invalid Component List.
 **/
AREG_API const NERegistry::ComponentList          NERegistry::INVALID_COMPONENT_LIST;
/**
 * \brief   Predefined invalid Thread Entry.
 **/
AREG_API const NERegistry::ComponentThreadEntry   NERegistry::INVALID_THREAD_ENTRY;
/**
 * \brief   Predefined invalid Thread List.
 **/
AREG_API const NERegistry::ComponentThreadList    NERegistry::INVALID_THREAD_LIST;
/**
 * \brief   Predefined invalid Model.
 **/
AREG_API const NERegistry::Model                  NERegistry::INVALID_MODEL;

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ServiceEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::ServiceEntry::ServiceEntry( const char* serviceName, unsigned int major, unsigned int minor, unsigned int patch )
    : mName     ( serviceName )
    , mVersion  ( major, minor, patch )
{
    ASSERT( mName.isEmpty( ) == false );
    ASSERT( mVersion.isValid( ) );
}

NERegistry::ServiceEntry::ServiceEntry( const char* serviceName, const Version & version )
    : mName     (serviceName)
    , mVersion  (version)
{
    ASSERT( mName.isEmpty() == false );
}

NERegistry::ServiceEntry::ServiceEntry( const NERegistry::ServiceEntry & src )
    : mName     (src.mName)
    , mVersion  (src.mVersion)
{
}

NERegistry::ServiceEntry::ServiceEntry( NERegistry::ServiceEntry && src ) noexcept
    : mName     ( std::move(src.mName) )
    , mVersion  ( std::move(src.mVersion) )
{
}

NERegistry::ServiceEntry & NERegistry::ServiceEntry::operator = ( const NERegistry::ServiceEntry & src )
{
    mName   = src.mName;
    mVersion= src.mVersion;
    
    return (*this);
}

NERegistry::ServiceEntry & NERegistry::ServiceEntry::operator = ( NERegistry::ServiceEntry && src ) noexcept
{
    mName   = std::move(src.mName);
    mVersion= std::move(src.mVersion);

    return (*this);
}

bool NERegistry::ServiceEntry::operator == ( const NERegistry::ServiceEntry & other ) const
{
    return (this != &other ? mName == other.mName && mVersion == other.mVersion : true);
}

bool NERegistry::ServiceEntry::isValid( void ) const
{
    return ( (mName.isEmpty() == false) && mVersion.isValid() );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ServiceList implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::ServiceList::ServiceList( const NERegistry::ServiceEntry & entry )
    : NERegistry::ServiceListBase( 0 )
{
    if ( entry.isValid() )
        add( entry );
}

const NERegistry::ServiceEntry & NERegistry::ServiceList::getService( const char* serviceName ) const
{
    int i = findService(serviceName);
    return (i == NECommon::INVALID_INDEX ? NERegistry::INVALID_SERVICE_ENTRY : getAt(i));
}

int NERegistry::ServiceList::findService( const NERegistry::ServiceEntry & entry ) const
{
    return findService( entry.mName.getString() );
}

int NERegistry::ServiceList::findService( const char* serviceName ) const
{
    int i = 0;
    for ( ; i < getSize(); ++ i )
    {
        if (getAt(i).mName == serviceName)
            break;
    }

    return (i < getSize() ? i : NECommon::INVALID_INDEX);
}

bool NERegistry::ServiceList::isValid( void ) const
{
    return (getSize() != 0 );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::WorkerThreadEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::WorkerThreadEntry::WorkerThreadEntry( const char * masterThreadName, const char* workerThreadName, const char * compRoleName, const char* compConsumerName )
    : mThreadName   (NEUtilities::createComponentItemName(masterThreadName, workerThreadName))
    , mConsumerName (NEUtilities::createComponentItemName(compRoleName, compConsumerName))
{
}

NERegistry::WorkerThreadEntry::WorkerThreadEntry( const NERegistry::WorkerThreadEntry &src )
    : mThreadName   (src.mThreadName)
    , mConsumerName (src.mConsumerName)
{
}

NERegistry::WorkerThreadEntry::WorkerThreadEntry( NERegistry::WorkerThreadEntry &&src ) noexcept
    : mThreadName   ( std::move(src.mThreadName) )
    , mConsumerName ( std::move(src.mConsumerName) )
{
}

NERegistry::WorkerThreadEntry & NERegistry::WorkerThreadEntry::operator = ( const NERegistry::WorkerThreadEntry & src )
{
    mThreadName     = src.mThreadName;
    mConsumerName   = src.mConsumerName;

    return (*this);
}

NERegistry::WorkerThreadEntry & NERegistry::WorkerThreadEntry::operator = ( NERegistry::WorkerThreadEntry && src ) noexcept
{
    mThreadName     = std::move(src.mThreadName);
    mConsumerName   = std::move(src.mConsumerName);

    return (*this);
}

bool NERegistry::WorkerThreadEntry::operator == ( const NERegistry::WorkerThreadEntry & other ) const
{
    return ( this != &other ? mThreadName == other.mThreadName && mConsumerName == other.mConsumerName : true );
}

bool NERegistry::WorkerThreadEntry::isValid( void ) const
{
    return ( (mThreadName.isEmpty() == false) && (mConsumerName.isEmpty() == false) );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::WorkerThreadList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::WorkerThreadList::WorkerThreadList( const NERegistry::WorkerThreadEntry & entry )
    : NERegistry::WorkerThreadListBase( 0 )
{
    if ( entry.isValid() )
        add( entry );
}

const NERegistry::WorkerThreadEntry & NERegistry::WorkerThreadList::getWorkerThread( const char* threadName ) const
{
    int index = findThread(threadName);
    return (index == NECommon::INVALID_INDEX ? NERegistry::INVALID_WORKER_THREAD_ENTRY : getAt(index));
}

int NERegistry::WorkerThreadList::findThread( const char* threadName ) const
{
    int i = 0;
    for ( ; i < getSize(); ++ i )
    {
        if (getAt(i).mThreadName == threadName)
            break;
    }
    return (i < getSize() ? i : NECommon::INVALID_INDEX);
}

int NERegistry::WorkerThreadList::findThread( const NERegistry::WorkerThreadEntry & entry ) const
{
    return findThread( static_cast<const char *>(entry.mThreadName) );
}

bool NERegistry::WorkerThreadList::isValid( void ) const
{
    return ( getSize() != 0  );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::DependencyEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::DependencyEntry::DependencyEntry( const char* roleName )
    : mRoleName(roleName)
{
}

NERegistry::DependencyEntry::DependencyEntry( const NERegistry::DependencyEntry& src )
    : mRoleName(src.mRoleName)
{
}

NERegistry::DependencyEntry::DependencyEntry( NERegistry::DependencyEntry && src ) noexcept
    : mRoleName( std::move(src.mRoleName) )
{
}

NERegistry::DependencyEntry & NERegistry::DependencyEntry::operator = ( const DependencyEntry & src )
{
    mRoleName = src.mRoleName;
    return (*this);
}

NERegistry::DependencyEntry & NERegistry::DependencyEntry::operator = ( DependencyEntry && src ) noexcept
{
    mRoleName = std::move(src.mRoleName);
    return (*this);
}

bool NERegistry::DependencyEntry::operator == ( const NERegistry::DependencyEntry & other ) const
{
    return (mRoleName == other.mRoleName);
}

bool NERegistry::DependencyEntry::isValid( void ) const
{
    return (mRoleName.isEmpty() == false);
}

void NERegistry::DependencyEntry::setDependentService( const char * roleName )
{
    mRoleName   = roleName;
}

const String & NERegistry::DependencyEntry::getDepdendentService( void ) const
{
    return mRoleName;
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::DependencyList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::DependencyList::DependencyList( const NERegistry::DependencyEntry & entry  )
    : NERegistry::DependencyListBase( 0 )
{
    if ( entry.isValid())
        add(entry);
}

const NERegistry::DependencyEntry & NERegistry::DependencyList::getDependency( const char* roleName ) const
{
    int index = findDependency(roleName);
    return (index == NECommon::INVALID_INDEX ? NERegistry::INVALID_DEPENDENCY_ENTRY : getAt(index) );
}

int NERegistry::DependencyList::findDependency( const NERegistry::DependencyEntry & entry ) const
{
    return findDependency( static_cast<const char *>(entry.mRoleName) );
}

int NERegistry::DependencyList::findDependency( const char* roleName ) const
{
    int i = 0;
    for ( ; i < getSize(); ++ i )
    {
        if (getAt(i).mRoleName == roleName)
            break;
    }
    return (i < getSize() ? i : NECommon::INVALID_INDEX);
}

bool NERegistry::DependencyList::isValid( void ) const
{
    return (getSize() != 0);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ComponentEntry implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::ComponentEntry::ComponentEntry( void )
    : mRoleName             ( )

    , mThreadName           ( )
    , mFuncCreate           (nullptr)
    , mFuncDelete           (nullptr)

    , mSupportedServices    ( )
    , mWorkerThreads        ( )
    , mDependencyServices   ( )

    , mComponentData        ( NEMemory::InvalidElement )
{
}

NERegistry::ComponentEntry::ComponentEntry( const char * masterThreadName, const char* roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete )
    : mRoleName             (roleName)

    , mThreadName           (masterThreadName != nullptr ? masterThreadName : "")
    , mFuncCreate           (funcCreate)
    , mFuncDelete           (funcDelete)

    , mSupportedServices    ( )
    , mWorkerThreads        ( )
    , mDependencyServices   ( )

    , mComponentData        ( NEMemory::InvalidElement )
{
}

NERegistry::ComponentEntry::ComponentEntry(   const char * masterThreadName
                                            , const char * roleName
                                            , FuncCreateComponent funcCreate
                                            , FuncDeleteComponent funcDelete
                                            , const NERegistry::ServiceList & serviceList
                                            , const NERegistry::DependencyList & dependencyList
                                            , const NERegistry::WorkerThreadList & workerList )
    : mRoleName             (roleName)

    , mThreadName           (masterThreadName != nullptr ? masterThreadName : "")
    , mFuncCreate           (funcCreate)
    , mFuncDelete           (funcDelete)

    , mSupportedServices    (serviceList)
    , mWorkerThreads        (workerList)
    , mDependencyServices   (dependencyList)

    , mComponentData        ( NEMemory::InvalidElement )
{
}

NERegistry::ComponentEntry::ComponentEntry(   const char * masterThreadName
                                            , const char * roleName
                                            , FuncCreateComponent funcCreate
                                            , FuncDeleteComponent funcDelete
                                            , const NERegistry::ServiceEntry & service
                                            , const NERegistry::DependencyEntry & dependency
                                            , const NERegistry::WorkerThreadEntry & worker )
    : mRoleName             (roleName)

    , mThreadName           (masterThreadName != nullptr ? masterThreadName : "")
    , mFuncCreate           (funcCreate)
    , mFuncDelete           (funcDelete)

    , mSupportedServices    (service)
    , mWorkerThreads        (worker)
    , mDependencyServices   (dependency)

    , mComponentData        ( NEMemory::InvalidElement )
{
}

NERegistry::ComponentEntry::ComponentEntry( const NERegistry::ComponentEntry & src )
    : mRoleName             (src.mRoleName)

    , mThreadName           (src.mThreadName)
    , mFuncCreate           (src.mFuncCreate)
    , mFuncDelete           (src.mFuncDelete)

    , mSupportedServices    (src.mSupportedServices)
    , mWorkerThreads        (src.mWorkerThreads)
    , mDependencyServices   (src.mDependencyServices)

    , mComponentData        ( src.mComponentData )
{
}

NERegistry::ComponentEntry::ComponentEntry( NERegistry::ComponentEntry && src ) noexcept
    : mRoleName             ( std::move(src.mRoleName) )

    , mThreadName           ( std::move(src.mThreadName) )
    , mFuncCreate           ( std::move(src.mFuncCreate) )
    , mFuncDelete           ( std::move(src.mFuncDelete) )

    , mSupportedServices    ( std::move(src.mSupportedServices) )
    , mWorkerThreads        ( std::move(src.mWorkerThreads) )
    , mDependencyServices   ( std::move(src.mDependencyServices) )

    , mComponentData        ( std::move(src.mComponentData) )
{
}

NERegistry::ComponentEntry & NERegistry::ComponentEntry::operator = ( const NERegistry::ComponentEntry & src )
{
    if (static_cast<const NERegistry::ComponentEntry *>(this) != &src)
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

NERegistry::ComponentEntry & NERegistry::ComponentEntry::operator = ( NERegistry::ComponentEntry && src ) noexcept
{
    mRoleName           = std::move(src.mRoleName);
    mThreadName         = std::move(src.mThreadName);
    mFuncCreate         = std::move(src.mFuncCreate);
    mFuncDelete         = std::move(src.mFuncDelete);
    mSupportedServices  = std::move(src.mSupportedServices);
    mWorkerThreads      = std::move(src.mWorkerThreads);
    mDependencyServices = std::move(src.mDependencyServices);
    mComponentData      = std::move(src.mComponentData);

    return (*this);
}

bool NERegistry::ComponentEntry::operator == ( const NERegistry::ComponentEntry & other ) const
{
    return (this != &other ? mRoleName == other.mRoleName && mThreadName == other.mThreadName && mFuncCreate == other.mFuncCreate && mFuncDelete == other.mFuncDelete : true);
}

void NERegistry::ComponentEntry::addSupportedService( const NERegistry::ServiceEntry & entry )
{
    if (findSupportedService(entry) < 0)
        mSupportedServices.add(entry);
}

void NERegistry::ComponentEntry::addSupportedService( const NERegistry::ServiceList & serviceList )
{
    for ( int i = 0; i < serviceList.getSize(); ++ i )
        addSupportedService(serviceList[i]);
}

NERegistry::ServiceEntry & NERegistry::ComponentEntry::addSupportedService(const char * serviceName, const Version & version)
{
    int index = findSupportedService(serviceName);
    if ( index == NECommon::INVALID_INDEX )
    {
        index = mSupportedServices.add(NERegistry::ServiceEntry(serviceName, version));
    }

    return mSupportedServices[index];
}

bool NERegistry::ComponentEntry::removeSupportedService( const char * serviceName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(serviceName) == false )
    {
        for ( int i = 0; (result == false) && (i < mSupportedServices.getSize( )); ++i )
        {
            if ( mSupportedServices.getAt(i).mName == serviceName )
            {
                mSupportedServices.removeAt(i, 1);
                result = true;
            }
        }
    }
    return result;
}

int NERegistry::ComponentEntry::findSupportedService( const NERegistry::ServiceEntry& entry ) const
{
    return mSupportedServices.findService(entry);
}

int NERegistry::ComponentEntry::findSupportedService( const char* serviceName ) const
{
    return mSupportedServices.findService(serviceName);
}

void NERegistry::ComponentEntry::addWorkerThread( const NERegistry::WorkerThreadEntry& entry )
{
    if (findWorkerThread(entry) < 0)
        mWorkerThreads.add(entry);
}

void NERegistry::ComponentEntry::addWorkerThread( const NERegistry::WorkerThreadList & workerList )
{
    for ( int i = 0; i < workerList.getSize(); ++ i )
        addWorkerThread(workerList[i]);
}

int NERegistry::ComponentEntry::findWorkerThread( const NERegistry::WorkerThreadEntry& entry ) const
{
    return mWorkerThreads.findThread(entry);
}

int NERegistry::ComponentEntry::findWorkerThread( const char* workerName ) const
{
    return mWorkerThreads.findThread(workerName);
}

bool NERegistry::ComponentEntry::removeWorkerThread( const char * workerName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(workerName) == false )
    {
        for ( int i = 0; (result == false) && (i < mWorkerThreads.getSize()); ++i )
        {
            if ( mWorkerThreads.getAt(i).mThreadName == workerName )
            {
                mWorkerThreads.removeAt(i);
                result = true;
            }
        }
    }
    return result;
}

void NERegistry::ComponentEntry::addDependencyService( const NERegistry::DependencyEntry& entry )
{
    if (findDependencyService(entry) < 0)
        mDependencyServices.add(entry);
}

void NERegistry::ComponentEntry::addDependencyService( const NERegistry::DependencyList & dependencyList )
{
    for ( int i = 0; i < dependencyList.getSize(); ++ i )
        addDependencyService(dependencyList[i]);
}

NERegistry::DependencyEntry & NERegistry::ComponentEntry::addDependencyService(const char * roleName)
{
    int index = findDependencyService(roleName);
    if ( index == NECommon::INVALID_INDEX )
    {
        index = mDependencyServices.add(NERegistry::DependencyEntry(roleName));
    }

    return mDependencyServices[index];
}

int NERegistry::ComponentEntry::findDependencyService( const NERegistry::DependencyEntry& entry ) const
{
    return mDependencyServices.findDependency(entry);
}

bool NERegistry::ComponentEntry::removeDependencyService( const char * roleName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(roleName) == false )
    {
        for (int i = 0; (result == false) && (i < mDependencyServices.getSize()); ++i )
        {
            if ( mDependencyServices.getAt(i).mRoleName == roleName )
            {
                mDependencyServices.removeAt(i);
                result = true;
            }
        }
    }
    return result;
}

int NERegistry::ComponentEntry::findDependencyService( const char* roleName ) const
{
    return mDependencyServices.findDependency(roleName);
}

const NERegistry::ServiceList & NERegistry::ComponentEntry::getSupportedServices( void ) const
{
    return mSupportedServices;
}

const NERegistry::WorkerThreadList & NERegistry::ComponentEntry::getWorkerThreads( void ) const
{
    return mWorkerThreads;
}

const NERegistry::DependencyList & NERegistry::ComponentEntry::getDependencyServices( void ) const
{
    return mDependencyServices;
}

void NERegistry::ComponentEntry::setInstanceMethods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete )
{
    mFuncCreate = fnCreate;
    mFuncDelete = fnDelete;
}

bool NERegistry::ComponentEntry::isValid( void ) const
{
    return ( (mRoleName.isEmpty() == false) && (mFuncCreate != nullptr) && (mFuncDelete != nullptr) );
}

void NERegistry::ComponentEntry::setComponentData( NEMemory::uAlign compData )
{
    mComponentData  = compData;
}

NEMemory::uAlign NERegistry::ComponentEntry::getComponentData( void ) const
{
    return mComponentData;
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ComponentList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::ComponentList::ComponentList(  const NERegistry::ComponentEntry & entry )
    : NERegistry::ComponentListBase( 0 )
{
    if ( entry.isValid() )
        add( entry );
}

const NERegistry::ComponentEntry & NERegistry::ComponentList::getComponent( const char* roleName ) const
{
    int index = findComponent(roleName);
    return (index != NECommon::INVALID_INDEX ? getAt(index) : NERegistry::INVALID_COMPONENT_ENTRY);
}

int NERegistry::ComponentList::findComponent(const char* roleName) const
{
    int i = 0;
    for ( ; i < getSize(); ++ i )
    {
        if (getAt(i).mRoleName == roleName)
            break;
    }
    return (i < getSize() ? i : NECommon::INVALID_INDEX);
}

int NERegistry::ComponentList::findComponent( const NERegistry::ComponentEntry& entry ) const
{
    return findComponent( static_cast<const char *>(entry.mRoleName) );
}

bool NERegistry::ComponentList::isValid( void ) const
{
    return (getSize() != 0);
}

bool NERegistry::ComponentList::setComponentData( const char * roleName, NEMemory::uAlign compData )
{
    bool result = false;
    for ( int i = 0; i < getSize(); ++ i )
    {
        NERegistry::ComponentEntry & entry = getAt(i);
        if ( entry.mRoleName == roleName )
        {
            entry.setComponentData(compData);
            result = true;
            break;
        }
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ComponentThreadEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::ComponentThreadEntry::ComponentThreadEntry( const char* threadName )
    : mThreadName   (threadName)
    , mComponents   ( )
{
}

NERegistry::ComponentThreadEntry::ComponentThreadEntry( const char* threadName, const NERegistry::ComponentList& supCompList )
    : mThreadName   (threadName)
    , mComponents   (supCompList)
{
}

NERegistry::ComponentThreadEntry::ComponentThreadEntry( const NERegistry::ComponentThreadEntry& src )
    : mThreadName   (src.mThreadName)
    , mComponents   (src.mComponents)
{
}

NERegistry::ComponentThreadEntry::ComponentThreadEntry( NERegistry::ComponentThreadEntry && src ) noexcept
    : mThreadName   ( std::move(src.mThreadName) )
    , mComponents   ( std::move(src.mComponents) )
{
}

NERegistry::ComponentThreadEntry & NERegistry::ComponentThreadEntry::operator = ( const NERegistry::ComponentThreadEntry & src )
{
    mThreadName = src.mThreadName;
    mComponents = src.mComponents;
    
    return (*this);
}

NERegistry::ComponentThreadEntry & NERegistry::ComponentThreadEntry::operator = ( NERegistry::ComponentThreadEntry && src ) noexcept
{
    mThreadName = std::move(src.mThreadName);
    mComponents = std::move(src.mComponents);
    
    return (*this);
}

bool NERegistry::ComponentThreadEntry::operator == ( const NERegistry::ComponentThreadEntry & other ) const
{
    return (this != &other ? mThreadName == other.mThreadName : true);
}

void NERegistry::ComponentThreadEntry::addComponent( const NERegistry::ComponentEntry & entry )
{
    if (findComponentEntry(entry) < 0)
        mComponents.add(entry);
}

void NERegistry::ComponentThreadEntry::addComponent( const NERegistry::ComponentList & componentList )
{
    for (int i = 0; i < componentList.getSize(); ++ i )
        addComponent(componentList[i]);
}

NERegistry::ComponentEntry & NERegistry::ComponentThreadEntry::addComponent(const char * roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete)
{
    int index = mComponents.findComponent(roleName);
    if ( index == NECommon::INVALID_INDEX )
    {
        index = mComponents.add( NERegistry::ComponentEntry(mThreadName.getString(), roleName, funcCreate, funcDelete));
    }

    return mComponents[index];
}

bool NERegistry::ComponentThreadEntry::removeComponent( const char * roleName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(roleName) == false )
    {
        for ( int i = 0; (result == false) && (i < mComponents.getSize()); ++i )
        {
            if ( mComponents.getAt(i).mRoleName == roleName )
            {
                mComponents.removeAt(i);
                result = true;
            }
        }
    }
    return result;
}

int NERegistry::ComponentThreadEntry::findComponentEntry( const NERegistry::ComponentEntry & entry ) const
{
    return mComponents.findComponent(entry);
}

int NERegistry::ComponentThreadEntry::findComponentEntry( const char* roleName ) const
{
    return mComponents.findComponent(roleName);
}

bool NERegistry::ComponentThreadEntry::isValid( void ) const
{
    return ( (mThreadName.isEmpty() == false) && (mComponents.isEmpty() == false) );
}

bool NERegistry::ComponentThreadEntry::setComponentData( const char * roleName, NEMemory::uAlign compData )
{
    return mComponents.setComponentData(roleName, compData);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ComponentThreadList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::ComponentThreadList::ComponentThreadList( const NERegistry::ComponentThreadEntry & entry )
    : NERegistry::ComponentThreadListBase( 0 )
{
    if ( entry.isValid() )
        add(entry);
}

const NERegistry::ComponentThreadEntry & NERegistry::ComponentThreadList::getThread( const char* threadName ) const
{
    int index = findThread(threadName);
    return (index != NECommon::INVALID_INDEX ? getAt(index) : NERegistry::INVALID_THREAD_ENTRY);
}

int NERegistry::ComponentThreadList::findThread( const NERegistry::ComponentThreadEntry& entry ) const
{
    return findThread( static_cast<const char *>(entry.mThreadName) );
}

int NERegistry::ComponentThreadList::findThread( const char* threadName ) const
{
    int i = 0;
    for ( ; i < getSize(); ++ i )
    {
        if (getAt(i).mThreadName == threadName)
            break;
    }

    return (i < getSize() ? i : NECommon::INVALID_INDEX);
}

bool NERegistry::ComponentThreadList::isValid( void ) const
{
    return (getSize() != 0);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::Model implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::Model::Model( void )
    : mModelName    ( )
    , mModelThreads ( )
    , mIsLoaded     ( false )
{
}

NERegistry::Model::Model( const char* modelName )
    : mModelName    (modelName)
    , mModelThreads ( )
    , mIsLoaded     ( false )
{
}

NERegistry::Model::Model( const char* modelName, const ComponentThreadList & threadList  )
    : mModelName    (modelName)
    , mModelThreads (threadList)
    , mIsLoaded     ( false )
{
}

NERegistry::Model::Model( const Model & src )
    : mModelName    ( src.mModelName )
    , mModelThreads ( src.mModelThreads )
    , mIsLoaded     ( src.mIsLoaded )
{
}

NERegistry::Model::Model( Model && src ) noexcept
    : mModelName    ( std::move(src.mModelName) )
    , mModelThreads ( std::move(src.mModelThreads) )
    , mIsLoaded     ( src.mIsLoaded )
{
}

NERegistry::Model & NERegistry::Model::operator = ( const NERegistry::Model & src )
{
    mModelName      = src.mModelName;
    mModelThreads   = src.mModelThreads;
    mIsLoaded       = src.mIsLoaded;

    return (*this);
}

NERegistry::Model & NERegistry::Model::operator = ( NERegistry::Model && src ) noexcept
{
    mModelName      = std::move(src.mModelName);
    mModelThreads   = std::move(src.mModelThreads);
    mIsLoaded       = src.mIsLoaded;

    return (*this);
}

bool NERegistry::Model::operator == ( const NERegistry::Model & other ) const
{
    return (this != &other ? mModelName == other.mModelName && mModelThreads == other.mModelThreads : true );
}

bool NERegistry::Model::isValid( void ) const
{
    return ( (mModelName.isEmpty() == false) && mModelThreads.isValid() );
}

int NERegistry::Model::findThread( const NERegistry::ComponentThreadEntry& entry ) const
{
    return mModelThreads.findThread(entry);
}

int NERegistry::Model::findThread( const char* threadName ) const
{
    return mModelThreads.findThread(threadName);
}

void NERegistry::Model::addThread( const NERegistry::ComponentThreadEntry& entry )
{
    if ( entry.isValid() && (findThread(entry) == NECommon::INVALID_INDEX) )
        mModelThreads.add(entry);
}

void NERegistry::Model::addThread( const NERegistry::ComponentThreadList& threadList )
{
    for (int i = 0; i < threadList.getSize(); ++ i )
        addThread(threadList[i]);
}

NERegistry::ComponentThreadEntry & NERegistry::Model::addThread(const char * threadName)
{
    int index = findThread(threadName);
    if (index == NECommon::INVALID_INDEX )
    {
        index = mModelThreads.add(NERegistry::ComponentThreadEntry(threadName));
    }

    return mModelThreads[index];
}

bool NERegistry::Model::removeThread( const char * threadName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(threadName) == false )
    {
        for ( int i = 0; (result == false) && (i < mModelThreads.getSize()); ++i )
        {
            if ( mModelThreads.getAt(i).mThreadName == threadName )
            {
                mModelThreads.removeAt(i);
                result = true;
            }
        }
    }
    return result;
}

const String & NERegistry::Model::getModelName( void ) const
{
    return mModelName;
}

bool NERegistry::Model::hasRegisteredComponent( const NERegistry::ComponentEntry & entry ) const
{
    int result = NECommon::INVALID_INDEX;
    for ( int i = 0; (result == NECommon::INVALID_INDEX) && (i < mModelThreads.getSize()); ++ i )
        result = mModelThreads.getAt(i).findComponentEntry(entry);
    return ( result < 0 ? false : true );
}

bool NERegistry::Model::hasRegisteredComponent( const char * roleName ) const
{
    int result = NECommon::INVALID_INDEX;
    for ( int i = 0; (result == NECommon::INVALID_INDEX) && (i < mModelThreads.getSize()); ++ i )
        result = mModelThreads.getAt(i).findComponentEntry(roleName);
    return ( result < 0 ? false : true );
}

bool NERegistry::Model::isModelLoaded( void ) const
{
    return mIsLoaded;
}

void NERegistry::Model::markModelLoaded( bool isLoaded /*= true */ )
{
    mIsLoaded = isLoaded;
}

const NERegistry::ComponentThreadList & NERegistry::Model::getThreadList( void ) const
{
    return mModelThreads;
}

bool NERegistry::Model::setComponentData( const char * roleName, NEMemory::uAlign compData )
{
    bool result = false;
    for ( int i = 0; i < mModelThreads.getSize(); ++ i )
    {
        NERegistry::ComponentThreadEntry & entry = mModelThreads.getAt(i);
        if ( entry.setComponentData(roleName, compData) )
        {
            result = true;
            break;
        }
    }
    return result;
}
