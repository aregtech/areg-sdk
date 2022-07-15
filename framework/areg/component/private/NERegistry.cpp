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

bool NERegistry::ServiceEntry::operator == ( const NERegistry::ServiceEntry & other ) const
{
    return (this == &other) || ((mName == other.mName) && (mVersion == other.mVersion));
}

bool NERegistry::ServiceEntry::operator != ( const NERegistry::ServiceEntry & other ) const
{
    return (this != &other) && ((mName != other.mName) || (mVersion != other.mVersion));
}

bool NERegistry::ServiceEntry::isValid( void ) const
{
    return ( (mName.isEmpty() == false) && mVersion.isValid() );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ServiceList implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::ServiceList::ServiceList( const NERegistry::ServiceEntry & entry )
    : mListServices ()
{
    if ( entry.isValid() )
        mListServices.add( entry );
}

const NERegistry::ServiceEntry & NERegistry::ServiceList::getService( const char* serviceName ) const
{
    int i = findService(serviceName);
    return (i == NECommon::INVALID_INDEX ? NERegistry::INVALID_SERVICE_ENTRY : mListServices.getAt(i));
}

int NERegistry::ServiceList::findService( const NERegistry::ServiceEntry & entry ) const
{
    return findService( entry.mName.getString() );
}

int NERegistry::ServiceList::findService( const char* serviceName ) const
{
    int result = NECommon::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListServices.getSize(); ++ i )
    {
        if (mListServices.getAt(i).mName == serviceName)
        {
            result = static_cast<int>(i);
            break;
        }
    }

    return result;
}

bool NERegistry::ServiceList::isValid( void ) const
{
    return (mListServices.getSize() != 0 );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::WorkerThreadEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::WorkerThreadEntry::WorkerThreadEntry( const char * masterThreadName, const char* workerThreadName, const char * compRoleName, const char* compConsumerName )
    : mThreadName   (NEUtilities::createComponentItemName(masterThreadName, workerThreadName))
    , mConsumerName (NEUtilities::createComponentItemName(compRoleName, compConsumerName))
{
}

bool NERegistry::WorkerThreadEntry::operator == ( const NERegistry::WorkerThreadEntry & other ) const
{
    return ( (this == &other) || ((mThreadName == other.mThreadName) && (mConsumerName == other.mConsumerName)));
}

bool NERegistry::WorkerThreadEntry::operator != ( const NERegistry::WorkerThreadEntry & other ) const
{
    return ((this != &other) && ((mThreadName != other.mThreadName) || (mConsumerName != other.mConsumerName)));
}

bool NERegistry::WorkerThreadEntry::isValid( void ) const
{
    return ( (mThreadName.isEmpty() == false) && (mConsumerName.isEmpty() == false) );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::WorkerThreadList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::WorkerThreadList::WorkerThreadList( const NERegistry::WorkerThreadEntry & entry )
    : mListWorkers  ()
{
    if ( entry.isValid() )
        mListWorkers.add( entry );
}

const NERegistry::WorkerThreadEntry & NERegistry::WorkerThreadList::getWorkerThread( const char* threadName ) const
{
    int index = findThread(threadName);
    return (index == NECommon::INVALID_INDEX ? NERegistry::INVALID_WORKER_THREAD_ENTRY : mListWorkers.getAt(index));
}

int NERegistry::WorkerThreadList::findThread( const char* threadName ) const
{
    int result = NECommon::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListWorkers.getSize(); ++ i )
    {
        if (mListWorkers.getAt(i).mThreadName == threadName)
        {
            result = static_cast<int>(i);
            break;
        }
    }

    return result;
}

int NERegistry::WorkerThreadList::findThread( const NERegistry::WorkerThreadEntry & entry ) const
{
    return findThread( static_cast<const char *>(entry.mThreadName) );
}

bool NERegistry::WorkerThreadList::isValid( void ) const
{
    return (mListWorkers.getSize() != 0  );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::DependencyEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::DependencyEntry::DependencyEntry( const char* roleName )
    : mRoleName(roleName)
{
}

bool NERegistry::DependencyEntry::operator == ( const NERegistry::DependencyEntry & other ) const
{
    return (mRoleName == other.mRoleName);
}

bool NERegistry::DependencyEntry::operator != ( const NERegistry::DependencyEntry & other ) const
{
    return (mRoleName != other.mRoleName);
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
    : mListDependencies ( )
{
    if ( entry.isValid())
        mListDependencies.add(entry);
}

const NERegistry::DependencyEntry & NERegistry::DependencyList::getDependency( const char* roleName ) const
{
    int index = findDependency(roleName);
    return (index == NECommon::INVALID_INDEX ? NERegistry::INVALID_DEPENDENCY_ENTRY : mListDependencies.getAt(index) );
}

int NERegistry::DependencyList::findDependency( const NERegistry::DependencyEntry & entry ) const
{
    return findDependency( static_cast<const char *>(entry.mRoleName) );
}

int NERegistry::DependencyList::findDependency( const char* roleName ) const
{
    int result = NECommon::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListDependencies.getSize(); ++ i )
    {
        if (mListDependencies.getAt(i).mRoleName == roleName)
        {
            result = static_cast<int>(i);
            break;
        }
    }

    return result;
}

bool NERegistry::DependencyList::isValid( void ) const
{
    return (mListDependencies.getSize() != 0);
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
        mSupportedServices.mListServices.add(entry);
}

void NERegistry::ComponentEntry::addSupportedService( const NERegistry::ServiceList & serviceList )
{
    for (uint32_t i = 0; i < serviceList.mListServices.getSize(); ++ i )
        addSupportedService(serviceList.mListServices[i]);
}

NERegistry::ServiceEntry & NERegistry::ComponentEntry::addSupportedService(const char * serviceName, const Version & version)
{
    uint32_t index = static_cast<uint32_t>(findSupportedService(serviceName));
    if ( index == static_cast<uint32_t>(NECommon::INVALID_INDEX) )
    {
        index = mSupportedServices.mListServices.getSize();
        mSupportedServices.mListServices.add(NERegistry::ServiceEntry(serviceName, version));
    }

    return mSupportedServices.mListServices[index];
}

bool NERegistry::ComponentEntry::removeSupportedService( const char * serviceName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(serviceName) == false )
    {
        for ( uint32_t i = 0; (result == false) && (i < mSupportedServices.mListServices.getSize( )); ++i )
        {
            if ( mSupportedServices.mListServices.getAt(i).mName == serviceName )
            {
                mSupportedServices.mListServices.removeAt(i, 1);
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
        mWorkerThreads.mListWorkers.add(entry);
}

void NERegistry::ComponentEntry::addWorkerThread( const NERegistry::WorkerThreadList & workerList )
{
    for ( uint32_t i = 0; i < workerList.mListWorkers.getSize(); ++ i )
        addWorkerThread(workerList.mListWorkers[i]);
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
        for ( uint32_t i = 0; (result == false) && (i < mWorkerThreads.mListWorkers.getSize()); ++i )
        {
            if ( mWorkerThreads.mListWorkers.getAt(i).mThreadName == workerName )
            {
                mWorkerThreads.mListWorkers.removeAt(i);
                result = true;
            }
        }
    }
    return result;
}

void NERegistry::ComponentEntry::addDependencyService( const NERegistry::DependencyEntry& entry )
{
    if (findDependencyService(entry) < 0)
        mDependencyServices.mListDependencies.add(entry);
}

void NERegistry::ComponentEntry::addDependencyService( const NERegistry::DependencyList & dependencyList )
{
    for ( uint32_t i = 0; i < dependencyList.mListDependencies.getSize(); ++ i )
        addDependencyService(dependencyList.mListDependencies[i]);
}

NERegistry::DependencyEntry & NERegistry::ComponentEntry::addDependencyService(const char * roleName)
{
    uint32_t index = static_cast<uint32_t>(findDependencyService(roleName));
    if ( index == static_cast<uint32_t>(NECommon::INVALID_INDEX) )
    {
        index = static_cast<int>(mDependencyServices.mListDependencies.getSize());
        mDependencyServices.mListDependencies.add(NERegistry::DependencyEntry(roleName));
    }

    return mDependencyServices.mListDependencies[index];
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
        for (uint32_t i = 0; (result == false) && (i < mDependencyServices.mListDependencies.getSize()); ++i )
        {
            if ( mDependencyServices.mListDependencies.getAt(i).mRoleName == roleName )
            {
                mDependencyServices.mListDependencies.removeAt(i);
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

void NERegistry::ComponentEntry::setComponentData( const NEMemory::uAlign & compData )
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
    : mListComponents   ( )
{
    if ( entry.isValid() )
        mListComponents.add( entry );
}

const NERegistry::ComponentEntry & NERegistry::ComponentList::getComponent( const char* roleName ) const
{
    int index = findComponent(roleName);
    return (index != NECommon::INVALID_INDEX ? mListComponents.getAt(index) : NERegistry::INVALID_COMPONENT_ENTRY);
}

int NERegistry::ComponentList::findComponent(const char* roleName) const
{
    int result = NECommon::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListComponents.getSize(); ++ i )
    {
        if (mListComponents.getAt(i).mRoleName == roleName)
        {
            result = static_cast<int>(i);
            break;
        }
    }

    return result;
}

int NERegistry::ComponentList::findComponent( const NERegistry::ComponentEntry& entry ) const
{
    return findComponent( static_cast<const char *>(entry.mRoleName) );
}

bool NERegistry::ComponentList::isValid( void ) const
{
    return (mListComponents.getSize() != 0);
}

bool NERegistry::ComponentList::setComponentData( const char * roleName, const NEMemory::uAlign & compData )
{
    bool result = false;
    for (uint32_t i = 0; i < mListComponents.getSize(); ++ i )
    {
        NERegistry::ComponentEntry & entry = mListComponents.getAt(i);
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

bool NERegistry::ComponentThreadEntry::operator == ( const NERegistry::ComponentThreadEntry & other ) const
{
    return ((this == &other) || (mThreadName == other.mThreadName));
}

bool NERegistry::ComponentThreadEntry::operator != ( const NERegistry::ComponentThreadEntry & other ) const
{
    return ((this != &other) && (mThreadName != other.mThreadName));
}

void NERegistry::ComponentThreadEntry::addComponent( const NERegistry::ComponentEntry & entry )
{
    if (findComponentEntry(entry) < 0)
        mComponents.mListComponents.add(entry);
}

void NERegistry::ComponentThreadEntry::addComponent( const NERegistry::ComponentList & componentList )
{
    for (uint32_t i = 0; i < componentList.mListComponents.getSize(); ++ i )
        addComponent(componentList.mListComponents[i]);
}

NERegistry::ComponentEntry & NERegistry::ComponentThreadEntry::addComponent(const char * roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete)
{
    uint32_t index = static_cast<uint32_t>(mComponents.findComponent(roleName));
    if ( index == static_cast<uint32_t>(NECommon::INVALID_INDEX) )
    {
        index = static_cast<int>(mComponents.mListComponents.getSize());
        mComponents.mListComponents.add( NERegistry::ComponentEntry(mThreadName.getString(), roleName, funcCreate, funcDelete));
    }

    return mComponents.mListComponents[index];
}

bool NERegistry::ComponentThreadEntry::removeComponent( const char * roleName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(roleName) == false )
    {
        for ( uint32_t i = 0; (result == false) && (i < mComponents.mListComponents.getSize()); ++i )
        {
            if ( mComponents.mListComponents.getAt(i).mRoleName == roleName )
            {
                mComponents.mListComponents.removeAt(i);
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
    return ( (mThreadName.isEmpty() == false) && (mComponents.mListComponents.isEmpty() == false) );
}

bool NERegistry::ComponentThreadEntry::setComponentData( const char * roleName, const NEMemory::uAlign & compData )
{
    return mComponents.setComponentData(roleName, compData);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ComponentThreadList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::ComponentThreadList::ComponentThreadList( const NERegistry::ComponentThreadEntry & entry )
    : mListThreads  ( )
{
    if ( entry.isValid() )
        mListThreads.add(entry);
}

const NERegistry::ComponentThreadEntry & NERegistry::ComponentThreadList::getThread( const char* threadName ) const
{
    int index = findThread(threadName);
    return (index != NECommon::INVALID_INDEX ? mListThreads.getAt(index) : NERegistry::INVALID_THREAD_ENTRY);
}

int NERegistry::ComponentThreadList::findThread( const NERegistry::ComponentThreadEntry& entry ) const
{
    return findThread( static_cast<const char *>(entry.mThreadName) );
}

int NERegistry::ComponentThreadList::findThread( const char* threadName ) const
{
    int result = NECommon::INVALID_INDEX;
    for (uint32_t i = 0; i < mListThreads.getSize(); ++ i )
    {
        if (mListThreads.getAt(i).mThreadName == threadName)
        {
            result = static_cast<int>(i);
            break;
        }
    }

    return result;
}

bool NERegistry::ComponentThreadList::isValid( void ) const
{
    return (mListThreads.getSize() != 0);
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

bool NERegistry::Model::operator == ( const NERegistry::Model & other ) const
{
    return (this == &other) || ((mModelName == other.mModelName) && (mModelThreads.mListThreads == other.mModelThreads.mListThreads));
}

bool NERegistry::Model::operator != ( const NERegistry::Model & other ) const
{
    return (this != &other) && ((mModelName != other.mModelName) || (mModelThreads.mListThreads != other.mModelThreads.mListThreads));
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
        mModelThreads.mListThreads.add(entry);
}

void NERegistry::Model::addThread( const NERegistry::ComponentThreadList& threadList )
{
    for (uint32_t i = 0; i < threadList.mListThreads.getSize(); ++ i )
        addThread(threadList.mListThreads[i]);
}

NERegistry::ComponentThreadEntry & NERegistry::Model::addThread(const char * threadName)
{
    uint32_t index = static_cast<uint32_t>(findThread(threadName));
    if (index == static_cast<uint32_t>(NECommon::INVALID_INDEX) )
    {
        index = static_cast<int>(mModelThreads.mListThreads.getSize());
        mModelThreads.mListThreads.add(NERegistry::ComponentThreadEntry(threadName));
    }

    return mModelThreads.mListThreads[index];
}

bool NERegistry::Model::removeThread( const char * threadName )
{
    bool result = false;
    if ( NEString::isEmpty<char>(threadName) == false )
    {
        for ( uint32_t i = 0; (result == false) && (i < mModelThreads.mListThreads.getSize()); ++i )
        {
            if ( mModelThreads.mListThreads.getAt(i).mThreadName == threadName )
            {
                mModelThreads.mListThreads.removeAt(i);
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
    for (uint32_t i = 0; (result == NECommon::INVALID_INDEX) && (i < mModelThreads.mListThreads.getSize()); ++i)
    {
        result = mModelThreads.mListThreads.getAt(i).findComponentEntry(entry);
    }

    return ( result >= 0 );
}

bool NERegistry::Model::hasRegisteredComponent( const char * roleName ) const
{
    int result = NECommon::INVALID_INDEX;
    for (uint32_t i = 0; (result == NECommon::INVALID_INDEX) && (i < mModelThreads.mListThreads.getSize()); ++i)
    {
        result = mModelThreads.mListThreads.getAt(i).findComponentEntry(roleName);
    }

    return ( result >= 0 );
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

bool NERegistry::Model::setComponentData( const char * roleName, const NEMemory::uAlign & compData )
{
    bool result = false;
    for ( uint32_t i = 0; i < mModelThreads.mListThreads.getSize(); ++ i )
    {
        NERegistry::ComponentThreadEntry & entry = mModelThreads.mListThreads.getAt(i);
        if ( entry.setComponentData(roleName, compData) )
        {
            result = true;
            break;
        }
    }

    return result;
}
