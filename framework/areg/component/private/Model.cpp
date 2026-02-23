/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Model.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, classes of NCRegistry namespace.
 *
 ************************************************************************/
#include "areg/component/Model.hpp"
#include "areg/component/Component.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/CommonDefs.hpp"

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
AREG_API_IMPL const areg::ServiceEntry & areg::invalidServiceEntry()
{
    static const areg::ServiceEntry _invalidServiceEntry;
    return _invalidServiceEntry;
}

/**
 * \brief   Predefined invalid Service List.
 **/
AREG_API_IMPL const areg::ServiceList & areg::invalidServiceList()
{
    static const areg::ServiceList _invalidServiceList;
    return _invalidServiceList;
}

/**
 * \brief   Predefined invalid Worker Thread Entry.
 **/
AREG_API_IMPL const areg::WorkerThreadEntry & areg::invalidWorkerThreadEntry()
{
    static const areg::WorkerThreadEntry _invalidWorkerThreadEntry;
    return _invalidWorkerThreadEntry;
}

/**
 * \brief   Predefined invalid Worker Thread List.
 **/
AREG_API_IMPL const areg::WorkerThreadList & areg::invalidWorkerThreadList()
{
    static const areg::WorkerThreadList _invalidWorkerThreadList;
    return _invalidWorkerThreadList;
}

/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API_IMPL const areg::DependencyEntry & areg::invalidDepedencyEntry()
{
    static const areg::DependencyEntry _invalidDepedencyEntry;
    return _invalidDepedencyEntry;
}

/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API_IMPL const areg::DependencyList & areg::invalidDepedencyList()
{
    static const areg::DependencyList _invalidDepedencyList;
    return _invalidDepedencyList;
}

/**
 * \brief   Predefined Invalid Component Entry.
 **/
AREG_API_IMPL const areg::ComponentEntry & areg::invalidComponentEntry()
{
    static const areg::ComponentEntry _invalidComponentEntry;
    return _invalidComponentEntry;
}

/**
 * \brief   Predefined invalid Component List.
 **/
AREG_API_IMPL const areg::ComponentList & areg::invalidComponentList()
{
    static const areg::ComponentList _invalidComponentList;
    return _invalidComponentList;
}

/**
 * \brief   Predefined invalid Thread Entry.
 **/
AREG_API_IMPL const areg::ComponentThreadEntry & areg::invalidThreadEntry()
{
    static const areg::ComponentThreadEntry _invalidThreadEntry;
    return _invalidThreadEntry;
}

/**
 * \brief   Predefined invalid Thread List.
 **/
AREG_API_IMPL const areg::ComponentThreadList & areg::invalidThreadList()
{
    static const areg::ComponentThreadList _invalidThreadList;
    return _invalidThreadList;
}

/**
 * \brief   Predefined invalid Model.
 **/
AREG_API_IMPL const areg::Model & areg::invalidModel()
{
    static const areg::Model _invalidModel;
    return _invalidModel;
}
//////////////////////////////////////////////////////////////////////////
// class areg::ServiceEntry implementation
//////////////////////////////////////////////////////////////////////////

areg::ServiceEntry::ServiceEntry( const String & serviceName, uint32_t major, uint32_t minor, uint32_t patch )
    : mName     ( serviceName )
    , mVersion  ( major, minor, patch )
{
    ASSERT( mName.isEmpty( ) == false );
    ASSERT( mVersion.isValid( ) );
}

areg::ServiceEntry::ServiceEntry( const String & serviceName, const Version & version )
    : mName     (serviceName)
    , mVersion  (version)
{
    ASSERT( mName.isEmpty() == false );
}

bool areg::ServiceEntry::operator == ( const areg::ServiceEntry & other ) const
{
    return (this == &other) || ((mName == other.mName) && (mVersion == other.mVersion));
}

bool areg::ServiceEntry::operator != ( const areg::ServiceEntry & other ) const
{
    return (this != &other) && ((mName != other.mName) || (mVersion != other.mVersion));
}

bool areg::ServiceEntry::isValid() const
{
    return ( (mName.isEmpty() == false) && mVersion.isValid() );
}

//////////////////////////////////////////////////////////////////////////
// class areg::ServiceList implementation
//////////////////////////////////////////////////////////////////////////
areg::ServiceList::ServiceList( const areg::ServiceEntry & entry )
    : mListServices ()
{
    if (entry.isValid())
    {
        mListServices.add(entry);
    }
}

const areg::ServiceEntry & areg::ServiceList::getService( const String & serviceName ) const
{
    int32_t index = findService(serviceName);
    return (index == areg::INVALID_INDEX ? areg::invalidServiceEntry() : mListServices.getAt(static_cast<uint32_t>(index)));
}

int32_t areg::ServiceList::findService( const areg::ServiceEntry & entry ) const
{
    return findService( entry.mName );
}

int32_t areg::ServiceList::findService( const String & serviceName ) const
{
    int32_t result = areg::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListServices.getSize(); ++ i )
    {
        if (mListServices[i].mName == serviceName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

bool areg::ServiceList::isValid() const
{
    return (mListServices.getSize() != 0 );
}

//////////////////////////////////////////////////////////////////////////
// class areg::WorkerThreadEntry implementation
//////////////////////////////////////////////////////////////////////////

areg::WorkerThreadEntry::WorkerThreadEntry()
    : mThreadName       ()
    , mConsumerName     ()
    , mWatchdogTimeout  (areg::WATCHDOG_IGNORE)
    , mStackSizeKb      (areg::STACK_SIZE_DEFAULT)
    , mMaxQueue         (areg::IGNORE_VALUE)
{
}

areg::WorkerThreadEntry::WorkerThreadEntry( const String & masterThreadName
                                                , const String & workerThreadName
                                                , const String & compRoleName
                                                , const String & compConsumerName
                                                , const uint32_t watchdogTimeout /* = areg::WATCHDOG_IGNORE     */
                                                , const uint32_t stackSizeKb     /* = areg::STACK_SIZE_DEFAULT  */
                                                , const uint32_t maxQueue        /* = areg::IGNORE_VALUE        */ )
    : mThreadName       (areg::createComponentItemName(masterThreadName, workerThreadName))
    , mConsumerName     (areg::createComponentItemName(compRoleName, compConsumerName))
    , mWatchdogTimeout  (watchdogTimeout)
    , mStackSizeKb      (stackSizeKb)
    , mMaxQueue         (maxQueue)
{
}

bool areg::WorkerThreadEntry::operator == ( const areg::WorkerThreadEntry & other ) const
{
    return ( (this == &other) || ((mThreadName == other.mThreadName) && (mConsumerName == other.mConsumerName)));
}

bool areg::WorkerThreadEntry::operator != ( const areg::WorkerThreadEntry & other ) const
{
    return ((this != &other) && ((mThreadName != other.mThreadName) || (mConsumerName != other.mConsumerName)));
}

bool areg::WorkerThreadEntry::isValid() const
{
    return ( (mThreadName.isEmpty() == false) && (mConsumerName.isEmpty() == false) );
}

//////////////////////////////////////////////////////////////////////////
// class areg::WorkerThreadList implementation
//////////////////////////////////////////////////////////////////////////

areg::WorkerThreadList::WorkerThreadList( const areg::WorkerThreadEntry & entry )
    : mListWorkers  ()
{
    if (entry.isValid())
    {
        mListWorkers.add(entry);
    }
}

const areg::WorkerThreadEntry & areg::WorkerThreadList::getWorkerThread( const String & threadName ) const
{
    int32_t index = findThread(threadName);
    return (index == areg::INVALID_INDEX ? areg::invalidWorkerThreadEntry() : mListWorkers.getAt(static_cast<uint32_t>(index)));
}

int32_t areg::WorkerThreadList::findThread( const String & threadName ) const
{
    int32_t result = areg::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListWorkers.getSize(); ++ i )
    {
        if (mListWorkers[i].mThreadName == threadName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

int32_t areg::WorkerThreadList::findThread( const areg::WorkerThreadEntry & entry ) const
{
    return findThread( entry.mThreadName );
}

bool areg::WorkerThreadList::isValid() const
{
    return (mListWorkers.getSize() != 0  );
}

//////////////////////////////////////////////////////////////////////////
// class areg::DependencyEntry implementation
//////////////////////////////////////////////////////////////////////////

areg::DependencyEntry::DependencyEntry( const String & roleName )
    : mRoleName(roleName)
{
}

bool areg::DependencyEntry::operator == ( const areg::DependencyEntry & other ) const
{
    return (mRoleName == other.mRoleName);
}

bool areg::DependencyEntry::operator != ( const areg::DependencyEntry & other ) const
{
    return (mRoleName != other.mRoleName);
}

bool areg::DependencyEntry::isValid() const
{
    return (mRoleName.isEmpty() == false);
}

void areg::DependencyEntry::setDependentService( const String & roleName )
{
    mRoleName   = roleName;
}

const String & areg::DependencyEntry::getDepdendentService() const
{
    return mRoleName;
}

//////////////////////////////////////////////////////////////////////////
// class areg::DependencyList implementation
//////////////////////////////////////////////////////////////////////////

areg::DependencyList::DependencyList( const areg::DependencyEntry & entry  )
    : mListDependencies ( )
{
    if (entry.isValid())
    {
        mListDependencies.add(entry);
    }
}

const areg::DependencyEntry & areg::DependencyList::getDependency( const String & roleName ) const
{
    int32_t index = findDependency(roleName);
    return (index == areg::INVALID_INDEX ? areg::invalidDepedencyEntry() : mListDependencies.getAt(static_cast<uint32_t>(index)) );
}

int32_t areg::DependencyList::findDependency( const areg::DependencyEntry & entry ) const
{
    return findDependency( entry.mRoleName );
}

int32_t areg::DependencyList::findDependency( const String & roleName ) const
{
    int32_t result = areg::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListDependencies.getSize(); ++ i )
    {
        if (mListDependencies.getAt(i).mRoleName == roleName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

bool areg::DependencyList::isValid() const
{
    return (mListDependencies.getSize() != 0);
}

//////////////////////////////////////////////////////////////////////////
// class areg::ComponentEntry implementation
//////////////////////////////////////////////////////////////////////////
areg::ComponentEntry::ComponentEntry()
    : mRoleName             ( )

    , mThreadName           ( )
    , mSupportedServices    ( )
    , mWorkerThreads        ( )
    , mDependencyServices   ( )

    , mFuncCreate           ( )
    , mFuncDelete           ( )
    , mComponentData        ( )
{
}

areg::ComponentEntry::ComponentEntry( const String & masterThreadName, const String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete )
    : mRoleName             (roleName)

    , mThreadName           (masterThreadName)
    , mSupportedServices    ( )
    , mWorkerThreads        ( )
    , mDependencyServices   ( )

    , mFuncCreate           (funcCreate)
    , mFuncDelete           (funcDelete)
    , mComponentData        ( )
{
}

areg::ComponentEntry::ComponentEntry(   const String & masterThreadName
                                            , const String & roleName
                                            , FuncCreateComponent funcCreate
                                            , FuncDeleteComponent funcDelete
                                            , const areg::ServiceList & serviceList
                                            , const areg::DependencyList & dependencyList
                                            , const areg::WorkerThreadList & workerList )
    : mRoleName             (roleName)

    , mThreadName           (masterThreadName)
    , mSupportedServices    (serviceList)
    , mWorkerThreads        (workerList)
    , mDependencyServices   (dependencyList)

    , mFuncCreate           (funcCreate)
    , mFuncDelete           (funcDelete)
    , mComponentData        ( )
{
}

areg::ComponentEntry::ComponentEntry(   const String & masterThreadName
                                            , const String & roleName
                                            , FuncCreateComponent funcCreate
                                            , FuncDeleteComponent funcDelete
                                            , const areg::ServiceEntry & service
                                            , const areg::DependencyEntry & dependency
                                            , const areg::WorkerThreadEntry & worker )
    : mRoleName             (roleName)

    , mThreadName           (masterThreadName)
    , mSupportedServices    (service)
    , mWorkerThreads        (worker)
    , mDependencyServices   (dependency)

    , mFuncCreate           (funcCreate)
    , mFuncDelete           (funcDelete)
    , mComponentData        ( )
{
}

areg::ComponentEntry::ComponentEntry( const areg::ComponentEntry & src )
    : mRoleName             (src.mRoleName)

    , mThreadName           (src.mThreadName)
    , mSupportedServices    (src.mSupportedServices)
    , mWorkerThreads        (src.mWorkerThreads)
    , mDependencyServices   (src.mDependencyServices)

    , mFuncCreate           (src.mFuncCreate)
    , mFuncDelete           (src.mFuncDelete)
    , mComponentData        ( src.mComponentData )
{
}

areg::ComponentEntry::ComponentEntry( areg::ComponentEntry && src ) noexcept
    : mRoleName             ( std::move(src.mRoleName) )

    , mThreadName           ( std::move(src.mThreadName) )
    , mSupportedServices    ( std::move(src.mSupportedServices) )
    , mWorkerThreads        ( std::move(src.mWorkerThreads) )
    , mDependencyServices   ( std::move(src.mDependencyServices) )

    , mFuncCreate           ( std::move(src.mFuncCreate) )
    , mFuncDelete           ( std::move(src.mFuncDelete) )
    , mComponentData        ( std::move(src.mComponentData) )
{
}

areg::ComponentEntry & areg::ComponentEntry::operator = ( const areg::ComponentEntry & src )
{
    if (static_cast<const areg::ComponentEntry *>(this) != &src)
    {
        mRoleName           = src.mRoleName;

        mThreadName         = src.mThreadName;
        mSupportedServices  = src.mSupportedServices;
        mWorkerThreads      = src.mWorkerThreads;
        mDependencyServices = src.mDependencyServices;

        mFuncCreate         = src.mFuncCreate;
        mFuncDelete         = src.mFuncDelete;
        mComponentData      = src.mComponentData;
    }

    return (*this);
}

areg::ComponentEntry & areg::ComponentEntry::operator = ( areg::ComponentEntry && src ) noexcept
{
    mRoleName           = std::move(src.mRoleName);
    mThreadName         = std::move(src.mThreadName);
    mSupportedServices  = std::move(src.mSupportedServices);
    mWorkerThreads      = std::move(src.mWorkerThreads);
    mDependencyServices = std::move(src.mDependencyServices);
    mFuncCreate         = std::move(src.mFuncCreate);
    mFuncDelete         = std::move(src.mFuncDelete);
    mComponentData      = std::move(src.mComponentData);

    return (*this);
}

bool areg::ComponentEntry::operator == ( const areg::ComponentEntry & other ) const
{
    return (this != &other ? (mRoleName   == other.mRoleName)     &&
                             (mThreadName == other.mThreadName)
                           : true);
}

void areg::ComponentEntry::addSupportedService( const areg::ServiceEntry & entry )
{
    if (findSupportedService(entry) < 0)
    {
        mSupportedServices.mListServices.add(entry);
    }
}

void areg::ComponentEntry::addSupportedService( const areg::ServiceList & serviceList )
{
    for (uint32_t i = 0; i < serviceList.mListServices.getSize(); ++i)
    {
        addSupportedService(serviceList.mListServices[i]);
    }
}

areg::ServiceEntry & areg::ComponentEntry::addSupportedService(const String & serviceName, const Version & version)
{
    int32_t index = findSupportedService(serviceName);
    if ( index == areg::INVALID_INDEX )
    {
        index = static_cast<int32_t>(mSupportedServices.mListServices.getSize());
        mSupportedServices.mListServices.add(areg::ServiceEntry(serviceName, version));
    }

    return mSupportedServices.mListServices.getAt(static_cast<uint32_t>(index));
}

bool areg::ComponentEntry::removeSupportedService( const String & serviceName )
{
    bool result = false;
    if ( serviceName.isEmpty() == false)
    {
        for ( uint32_t i = 0; (result == false) && (i < mSupportedServices.mListServices.getSize( )); ++i )
        {
            if ( mSupportedServices.mListServices[i].mName == serviceName)
            {
                mSupportedServices.mListServices.removeAt(i, 1);
                result = true;
            }
        }
    }

    return result;
}

int32_t areg::ComponentEntry::findSupportedService( const areg::ServiceEntry& entry ) const
{
    return mSupportedServices.findService(entry);
}

int32_t areg::ComponentEntry::findSupportedService( const String & serviceName ) const
{
    return mSupportedServices.findService(serviceName);
}

void areg::ComponentEntry::addWorkerThread( const areg::WorkerThreadEntry& entry )
{
    if (findWorkerThread(entry) < 0)
    {
        mWorkerThreads.mListWorkers.add(entry);
    }
}

void areg::ComponentEntry::addWorkerThread( const areg::WorkerThreadList & workerList )
{
    for (uint32_t i = 0; i < workerList.mListWorkers.getSize(); ++i)
    {
        addWorkerThread(workerList.mListWorkers[i]);
    }
}

int32_t areg::ComponentEntry::findWorkerThread( const areg::WorkerThreadEntry& entry ) const
{
    return mWorkerThreads.findThread(entry);
}

int32_t areg::ComponentEntry::findWorkerThread( const String & workerName ) const
{
    return mWorkerThreads.findThread(workerName);
}

bool areg::ComponentEntry::removeWorkerThread( const String & workerName )
{
    bool result = false;
    if ( workerName.isEmpty() == false)
    {
        for ( uint32_t i = 0; i < mWorkerThreads.mListWorkers.getSize(); ++i )
        {
            if ( mWorkerThreads.mListWorkers[i].mThreadName == workerName )
            {
                mWorkerThreads.mListWorkers.removeAt(i);
                result = true;
                break;
            }
        }
    }

    return result;
}

void areg::ComponentEntry::addDependencyService( const areg::DependencyEntry& entry )
{
    mDependencyServices.mListDependencies.add(entry);
}

void areg::ComponentEntry::addDependencyService( const areg::DependencyList & dependencyList )
{
    for (uint32_t i = 0; i < dependencyList.mListDependencies.getSize(); ++i)
    {
        addDependencyService(dependencyList.mListDependencies[i]);
    }
}

areg::DependencyEntry & areg::ComponentEntry::addDependencyService(const String & roleName)
{
    int32_t index = static_cast<int32_t>(mDependencyServices.mListDependencies.getSize());
    mDependencyServices.mListDependencies.add(areg::DependencyEntry(roleName));
    return mDependencyServices.mListDependencies.getAt(static_cast<uint32_t>(index));
}

int32_t areg::ComponentEntry::findDependencyService( const areg::DependencyEntry& entry ) const
{
    return mDependencyServices.findDependency(entry);
}

bool areg::ComponentEntry::removeDependencyService( const String & roleName )
{
    bool result = false;
    if ( roleName.isEmpty() == false)
    {
        for (uint32_t i = 0; i < mDependencyServices.mListDependencies.getSize(); ++i )
        {
            if ( mDependencyServices.mListDependencies[i].mRoleName == roleName )
            {
                mDependencyServices.mListDependencies.removeAt(i);
                result = true;
                break;
            }
        }
    }
    return result;
}

int32_t areg::ComponentEntry::findDependencyService( const String & roleName ) const
{
    return mDependencyServices.findDependency(roleName);
}

const areg::ServiceList & areg::ComponentEntry::getSupportedServices() const
{
    return mSupportedServices;
}

const areg::WorkerThreadList & areg::ComponentEntry::getWorkerThreads() const
{
    return mWorkerThreads;
}

const areg::DependencyList & areg::ComponentEntry::getDependencyServices() const
{
    return mDependencyServices;
}

void areg::ComponentEntry::setInstanceMethods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete )
{
    mFuncCreate = fnCreate;
    mFuncDelete = fnDelete;
}

bool areg::ComponentEntry::isValid() const
{
    return ( (mRoleName.isEmpty() == false) && (mFuncCreate != nullptr) && (mFuncDelete != nullptr) );
}

void areg::ComponentEntry::setData( std::any compData )
{
    mComponentData  = std::move(compData);
}

void areg::ComponentEntry::resetData()
{
    mComponentData.reset();
}

std::any areg::ComponentEntry::getData() const
{
    return mComponentData;
}

bool areg::ComponentEntry::hasData() const
{
    return mComponentData.has_value();
}

//////////////////////////////////////////////////////////////////////////
// class areg::ComponentList implementation
//////////////////////////////////////////////////////////////////////////

areg::ComponentList::ComponentList(  const areg::ComponentEntry & entry )
    : mListComponents   ( )
{
    if (entry.isValid())
    {
        mListComponents.add(entry);
    }
}

const areg::ComponentEntry & areg::ComponentList::getComponent( const String & roleName ) const
{
    int32_t index = findComponent(roleName);
    return (index != areg::INVALID_INDEX ? mListComponents.getAt(static_cast<uint32_t>(index)) : areg::invalidComponentEntry());
}

int32_t areg::ComponentList::findComponent(const String & roleName) const
{
    int32_t result = areg::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListComponents.getSize(); ++ i )
    {
        if (mListComponents[i].mRoleName == roleName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

int32_t areg::ComponentList::findComponent( const areg::ComponentEntry& entry ) const
{
    return findComponent( entry.mRoleName );
}

bool areg::ComponentList::isValid() const
{
    return (mListComponents.getSize() != 0);
}

bool areg::ComponentList::setComponentData( const String & roleName, std::any compData )
{
    bool result = false;
    for (uint32_t i = 0; i < mListComponents.getSize(); ++ i )
    {
        areg::ComponentEntry & entry = mListComponents[i];
        if ( entry.mRoleName == roleName )
        {
            entry.setData(compData);
            result = true;
            break;
        }
    }
    return result;
}

bool areg::ComponentList::resetComponentData(const String& roleName)
{
    bool result = false;
    for (uint32_t i = 0; i < mListComponents.getSize(); ++i)
    {
        areg::ComponentEntry& entry = mListComponents[i];
        if (entry.mRoleName == roleName)
        {
            entry.resetData();
            result = true;
            break;
        }
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
// class areg::ComponentThreadEntry implementation
//////////////////////////////////////////////////////////////////////////

areg::ComponentThreadEntry::ComponentThreadEntry()
    : mThreadName       ( )
    , mComponents       ( )
    , mWatchdogTimeout  (areg::WATCHDOG_IGNORE)
    , mStackSizeKB      (areg::STACK_SIZE_DEFAULT)
    , mMaxQueue         (areg::IGNORE_VALUE)
{
}

areg::ComponentThreadEntry::ComponentThreadEntry( const String & threadName
                                                      , const uint32_t watchdogTimeout  /* = areg::WATCHDOG_IGNORE      */
                                                      , const uint32_t stackSizeKb      /* = areg::STACK_SIZE_DEFAULT   */ 
                                                      , const uint32_t maxQueue         /* = areg::IGNORE_VALUE         */)
    : mThreadName       (threadName)
    , mComponents       ( )
    , mWatchdogTimeout  (watchdogTimeout)
    , mStackSizeKB      (stackSizeKb)
    , mMaxQueue         (maxQueue)
{
}

areg::ComponentThreadEntry::ComponentThreadEntry( const String & threadName
                                                      , const areg::ComponentList& supCompList
                                                      , const uint32_t watchdogTimeout  /* = areg::WATCHDOG_IGNORE      */
                                                      , const uint32_t stackSizeKb      /* = areg::STACK_SIZE_DEFAULT   */ 
                                                      , const uint32_t maxQueue         /* = areg::IGNORE_VALUE         */)
    : mThreadName       (threadName)
    , mComponents       (supCompList)
    , mWatchdogTimeout  (watchdogTimeout)
    , mStackSizeKB      (stackSizeKb)
    , mMaxQueue         (maxQueue)
{
}

bool areg::ComponentThreadEntry::operator == ( const areg::ComponentThreadEntry & other ) const
{
    return ((this == &other) || (mThreadName == other.mThreadName));
}

bool areg::ComponentThreadEntry::operator != ( const areg::ComponentThreadEntry & other ) const
{
    return ((this != &other) && (mThreadName != other.mThreadName));
}

void areg::ComponentThreadEntry::addComponent( const areg::ComponentEntry & entry )
{
    if (findComponentEntry(entry) < 0)
    {
        mComponents.mListComponents.add(entry);
    }
}

void areg::ComponentThreadEntry::addComponent( const areg::ComponentList & componentList )
{
    for (uint32_t i = 0; i < componentList.mListComponents.getSize(); ++i)
    {
        addComponent(componentList.mListComponents[i]);
    }
}

areg::ComponentEntry & areg::ComponentThreadEntry::addComponent(const String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete)
{
    int32_t index = mComponents.findComponent(roleName);
    if ( index == areg::INVALID_INDEX )
    {
        index = static_cast<int32_t>(mComponents.mListComponents.getSize());
        mComponents.mListComponents.add( areg::ComponentEntry(mThreadName.getString(), roleName, funcCreate, funcDelete));
    }

    return mComponents.mListComponents.getAt(static_cast<uint32_t>(index));
}

bool areg::ComponentThreadEntry::removeComponent( const String & roleName )
{
    bool result = false;
    if ( roleName.isEmpty() == false)
    {
        for ( uint32_t i = 0; i < mComponents.mListComponents.getSize(); ++i )
        {
            if ( mComponents.mListComponents[i].mRoleName == roleName )
            {
                mComponents.mListComponents.removeAt(i);
                result = true;
                break;
            }
        }
    }

    return result;
}

int32_t areg::ComponentThreadEntry::findComponentEntry( const areg::ComponentEntry & entry ) const
{
    return mComponents.findComponent(entry);
}

int32_t areg::ComponentThreadEntry::findComponentEntry( const String & roleName ) const
{
    return mComponents.findComponent(roleName);
}

bool areg::ComponentThreadEntry::isValid() const
{
    return ( (mThreadName.isEmpty() == false) && (mComponents.mListComponents.isEmpty() == false) );
}

bool areg::ComponentThreadEntry::setComponentData( const String & roleName, std::any compData )
{
    return mComponents.setComponentData(roleName, compData);
}

bool areg::ComponentThreadEntry::resetComponentData(const String& roleName)
{
    return mComponents.resetComponentData(roleName);
}

//////////////////////////////////////////////////////////////////////////
// class areg::ComponentThreadList implementation
//////////////////////////////////////////////////////////////////////////

areg::ComponentThreadList::ComponentThreadList( const areg::ComponentThreadEntry & entry )
    : mListThreads  ( )
{
    if (entry.isValid())
    {
        mListThreads.add(entry);
    }
}

const areg::ComponentThreadEntry & areg::ComponentThreadList::getThread( const String & threadName ) const
{
    int32_t index = findThread(threadName);
    return (index != areg::INVALID_INDEX ? mListThreads.getAt(static_cast<uint32_t>(index)) : areg::invalidThreadEntry());
}

int32_t areg::ComponentThreadList::findThread( const areg::ComponentThreadEntry& entry ) const
{
    return findThread( entry.mThreadName );
}

int32_t areg::ComponentThreadList::findThread( const String & threadName ) const
{
    int32_t result = areg::INVALID_INDEX;
    for (uint32_t i = 0; i < mListThreads.getSize(); ++ i )
    {
        if (mListThreads[i].mThreadName == threadName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

bool areg::ComponentThreadList::isValid() const
{
    return (mListThreads.getSize() != 0);
}

//////////////////////////////////////////////////////////////////////////
// class areg::Model implementation
//////////////////////////////////////////////////////////////////////////

areg::Model::Model()
    : mModelName    ( )
    , mModelThreads ( )
    , mLoadState    ( Model::ModelState::Initialized )
    , mAliveDuration( )
{
}

areg::Model::Model( const String & modelName )
    : mModelName    (modelName)
    , mModelThreads ( )
    , mLoadState    ( Model::ModelState::Initialized )
    , mAliveDuration( )
{
}

areg::Model::Model( const String & modelName, const ComponentThreadList & threadList  )
    : mModelName    (modelName)
    , mModelThreads (threadList)
    , mLoadState    ( Model::ModelState::Initialized )
    , mAliveDuration( )
{
}

bool areg::Model::operator == ( const areg::Model & other ) const
{
    return (this == &other) || ((mModelName == other.mModelName) && (mModelThreads.mListThreads == other.mModelThreads.mListThreads));
}

bool areg::Model::operator != ( const areg::Model & other ) const
{
    return (this != &other) && ((mModelName != other.mModelName) || (mModelThreads.mListThreads != other.mModelThreads.mListThreads));
}

bool areg::Model::isValid() const
{
    return ( (mModelName.isEmpty() == false) && mModelThreads.isValid() );
}

int32_t areg::Model::findThread( const areg::ComponentThreadEntry& entry ) const
{
    return mModelThreads.findThread(entry);
}

int32_t areg::Model::findThread( const String & threadName ) const
{
    return mModelThreads.findThread(threadName);
}

void areg::Model::addThread( const areg::ComponentThreadEntry& entry )
{
    if (entry.isValid() && (findThread(entry) == areg::INVALID_INDEX))
    {
        mModelThreads.mListThreads.add(entry);
    }
}

void areg::Model::addThread( const areg::ComponentThreadList& threadList )
{
    for (uint32_t i = 0; i < threadList.mListThreads.getSize(); ++i)
    {
        addThread(threadList.mListThreads[i]);
    }
}

areg::ComponentThreadEntry & areg::Model::addThread(const String & threadName)
{
    int32_t index = findThread(threadName);
    if (index == areg::INVALID_INDEX )
    {
        index = static_cast<int32_t>(mModelThreads.mListThreads.getSize());
        mModelThreads.mListThreads.add(areg::ComponentThreadEntry(threadName));
    }

    return mModelThreads.mListThreads.getAt(static_cast<uint32_t>(index));
}

bool areg::Model::removeThread( const String & threadName )
{
    bool result = false;
    if ( threadName.isEmpty() == false)
    {
        for ( uint32_t i = 0; i < mModelThreads.mListThreads.getSize(); ++i )
        {
            if ( mModelThreads.mListThreads[i].mThreadName == threadName )
            {
                mModelThreads.mListThreads.removeAt(i);
                result = true;
                break;
            }
        }
    }

    return result;
}

const String & areg::Model::getModelName() const
{
    return mModelName;
}

bool areg::Model::hasRegisteredComponent( const areg::ComponentEntry & entry ) const
{
    int32_t result = areg::INVALID_INDEX;
    for (uint32_t i = 0; (result == areg::INVALID_INDEX) && (i < mModelThreads.mListThreads.getSize()); ++i)
    {
        result = mModelThreads.mListThreads[i].findComponentEntry(entry);
    }

    return ( result >= 0 );
}

bool areg::Model::hasRegisteredComponent( const String & roleName ) const
{
    int32_t result = areg::INVALID_INDEX;
    for (uint32_t i = 0; (result == areg::INVALID_INDEX) && (i < mModelThreads.mListThreads.getSize()); ++i)
    {
        result = mModelThreads.mListThreads[i].findComponentEntry(roleName);
    }

    return ( result >= 0 );
}

bool areg::Model::isModelLoaded() const
{
    return (mLoadState == areg::Model::ModelState::Loaded);
}

void areg::Model::markModelLoaded( bool isLoaded /*= true */ )
{
    mLoadState = isLoaded ? Model::ModelState::Loaded : Model::ModelState::Unloaded;
}

void areg::Model::markModelAlive( bool isAlive )
{
    if ( isAlive )
    {
        mAliveDuration.start( );
    }
    else
    {
        mAliveDuration.stop( );
    }
}

const areg::ComponentThreadList & areg::Model::getThreadList() const
{
    return mModelThreads;
}

bool areg::Model::setComponentData( const String & roleName, std::any compData )
{
    bool result = false;
    for ( uint32_t i = 0; i < mModelThreads.mListThreads.getSize(); ++ i )
    {
        areg::ComponentThreadEntry & entry = mModelThreads.mListThreads.getAt(i);
        if ( entry.setComponentData(roleName, compData) )
        {
            result = true;
            break;
        }
    }

    return result;
}

bool areg::Model::resetComponentData(const String& roleName)
{
    bool result = false;
    for (uint32_t i = 0; i < mModelThreads.mListThreads.getSize(); ++i)
    {
        areg::ComponentThreadEntry& entry = mModelThreads.mListThreads.getAt(i);
        if (entry.resetComponentData(roleName))
        {
            result = true;
            break;
        }
    }

    return result;
}
