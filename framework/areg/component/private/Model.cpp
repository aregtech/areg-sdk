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
AREG_API_IMPL const areg::ServiceEntry & areg::invalid_service_entry() noexcept
{
    static const areg::ServiceEntry _invalidServiceEntry;
    return _invalidServiceEntry;
}

/**
 * \brief   Predefined invalid Service List.
 **/
AREG_API_IMPL const areg::ServiceList & areg::invalid_service_list() noexcept
{
    static const areg::ServiceList _invalidServiceList;
    return _invalidServiceList;
}

/**
 * \brief   Predefined invalid Worker Thread Entry.
 **/
AREG_API_IMPL const areg::WorkerThreadEntry & areg::invalid_worker_entry() noexcept
{
    static const areg::WorkerThreadEntry _invalidWorkerThreadEntry;
    return _invalidWorkerThreadEntry;
}

/**
 * \brief   Predefined invalid Worker Thread List.
 **/
AREG_API_IMPL const areg::WorkerThreadList & areg::invalid_worker_list() noexcept
{
    static const areg::WorkerThreadList _invalidWorkerThreadList;
    return _invalidWorkerThreadList;
}

/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API_IMPL const areg::DependencyEntry & areg::invalid_depedency_entry() noexcept
{
    static const areg::DependencyEntry _invalidDepedencyEntry;
    return _invalidDepedencyEntry;
}

/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API_IMPL const areg::DependencyList & areg::invalid_depedency_list() noexcept
{
    static const areg::DependencyList _invalidDepedencyList;
    return _invalidDepedencyList;
}

/**
 * \brief   Predefined Invalid Component Entry.
 **/
AREG_API_IMPL const areg::ComponentEntry & areg::invalid_component_entry() noexcept
{
    static const areg::ComponentEntry _invalidComponentEntry;
    return _invalidComponentEntry;
}

/**
 * \brief   Predefined invalid Component List.
 **/
AREG_API_IMPL const areg::ComponentList & areg::invalid_component_list() noexcept
{
    static const areg::ComponentList _invalidComponentList;
    return _invalidComponentList;
}

/**
 * \brief   Predefined invalid Thread Entry.
 **/
AREG_API_IMPL const areg::ComponentThreadEntry & areg::invalid_thread_entry() noexcept
{
    static const areg::ComponentThreadEntry _invalidThreadEntry;
    return _invalidThreadEntry;
}

/**
 * \brief   Predefined invalid Thread List.
 **/
AREG_API_IMPL const areg::ComponentThreadList & areg::invalid_thread_list() noexcept
{
    static const areg::ComponentThreadList _invalidThreadList;
    return _invalidThreadList;
}

/**
 * \brief   Predefined invalid Model.
 **/
AREG_API_IMPL const areg::Model & areg::invalid_model() noexcept
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
    ASSERT( mName.is_empty( ) == false );
    ASSERT( mVersion.is_valid( ) );
}

areg::ServiceEntry::ServiceEntry( const String & serviceName, const Version & version )
    : mName     (serviceName)
    , mVersion  (version)
{
    ASSERT( mName.is_empty() == false );
}

bool areg::ServiceEntry::operator == ( const areg::ServiceEntry & other ) const noexcept
{
    return (this == &other) || ((mName == other.mName) && (mVersion == other.mVersion));
}

bool areg::ServiceEntry::operator != ( const areg::ServiceEntry & other ) const noexcept
{
    return (this != &other) && ((mName != other.mName) || (mVersion != other.mVersion));
}

bool areg::ServiceEntry::is_valid() const noexcept
{
    return ( (mName.is_empty() == false) && mVersion.is_valid() );
}

//////////////////////////////////////////////////////////////////////////
// class areg::ServiceList implementation
//////////////////////////////////////////////////////////////////////////
areg::ServiceList::ServiceList( const areg::ServiceEntry & entry )
    : mListServices ()
{
    if (entry.is_valid())
    {
        mListServices.add(entry);
    }
}

const areg::ServiceEntry & areg::ServiceList::service( const String & serviceName ) const noexcept
{
    int32_t index = find_service(serviceName);
    return (index == areg::INVALID_INDEX ? areg::invalid_service_entry() : mListServices.value_at(static_cast<uint32_t>(index)));
}

int32_t areg::ServiceList::find_service( const areg::ServiceEntry & entry ) const noexcept
{
    return find_service( entry.mName );
}

int32_t areg::ServiceList::find_service( const String & serviceName ) const noexcept
{
    int32_t result = areg::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListServices.size(); ++ i )
    {
        if (mListServices[i].mName == serviceName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

bool areg::ServiceList::is_valid() const noexcept
{
    return (mListServices.size() != 0 );
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
    : mThreadName       (areg::create_component_item_name(masterThreadName, workerThreadName))
    , mConsumerName     (areg::create_component_item_name(compRoleName, compConsumerName))
    , mWatchdogTimeout  (watchdogTimeout)
    , mStackSizeKb      (stackSizeKb)
    , mMaxQueue         (maxQueue)
{
}

bool areg::WorkerThreadEntry::operator == ( const areg::WorkerThreadEntry & other ) const noexcept
{
    return ( (this == &other) || ((mThreadName == other.mThreadName) && (mConsumerName == other.mConsumerName)));
}

bool areg::WorkerThreadEntry::operator != ( const areg::WorkerThreadEntry & other ) const noexcept
{
    return ((this != &other) && ((mThreadName != other.mThreadName) || (mConsumerName != other.mConsumerName)));
}

bool areg::WorkerThreadEntry::is_valid() const noexcept
{
    return ( (mThreadName.is_empty() == false) && (mConsumerName.is_empty() == false) );
}

//////////////////////////////////////////////////////////////////////////
// class areg::WorkerThreadList implementation
//////////////////////////////////////////////////////////////////////////

areg::WorkerThreadList::WorkerThreadList( const areg::WorkerThreadEntry & entry )
    : mListWorkers  ()
{
    if (entry.is_valid())
    {
        mListWorkers.add(entry);
    }
}

const areg::WorkerThreadEntry & areg::WorkerThreadList::worker_thread( const String & threadName ) const noexcept
{
    int32_t index = find_thread(threadName);
    return (index == areg::INVALID_INDEX ? areg::invalid_worker_entry() : mListWorkers.value_at(static_cast<uint32_t>(index)));
}

int32_t areg::WorkerThreadList::find_thread( const String & threadName ) const noexcept
{
    int32_t result = areg::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListWorkers.size(); ++ i )
    {
        if (mListWorkers[i].mThreadName == threadName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

int32_t areg::WorkerThreadList::find_thread( const areg::WorkerThreadEntry & entry ) const noexcept
{
    return find_thread( entry.mThreadName );
}

bool areg::WorkerThreadList::is_valid() const noexcept
{
    return (mListWorkers.size() != 0  );
}

//////////////////////////////////////////////////////////////////////////
// class areg::DependencyEntry implementation
//////////////////////////////////////////////////////////////////////////

areg::DependencyEntry::DependencyEntry( const String & roleName )
    : mRoleName(roleName)
{
}

bool areg::DependencyEntry::operator == ( const areg::DependencyEntry & other ) const noexcept
{
    return (mRoleName == other.mRoleName);
}

bool areg::DependencyEntry::operator != ( const areg::DependencyEntry & other ) const noexcept
{
    return (mRoleName != other.mRoleName);
}

bool areg::DependencyEntry::is_valid() const noexcept
{
    return (mRoleName.is_empty() == false);
}

void areg::DependencyEntry::set_dependent_service( const String & roleName )
{
    mRoleName   = roleName;
}

const areg::String & areg::DependencyEntry::depdendent_service() const noexcept
{
    return mRoleName;
}

//////////////////////////////////////////////////////////////////////////
// class areg::DependencyList implementation
//////////////////////////////////////////////////////////////////////////

areg::DependencyList::DependencyList( const areg::DependencyEntry & entry  )
    : mListDependencies ( )
{
    if (entry.is_valid())
    {
        mListDependencies.add(entry);
    }
}

const areg::DependencyEntry & areg::DependencyList::dependency( const String & roleName ) const noexcept
{
    int32_t index = find_dependency(roleName);
    return (index == areg::INVALID_INDEX ? areg::invalid_depedency_entry() : mListDependencies.value_at(static_cast<uint32_t>(index)) );
}

int32_t areg::DependencyList::find_dependency( const areg::DependencyEntry & entry ) const noexcept
{
    return find_dependency( entry.mRoleName );
}

int32_t areg::DependencyList::find_dependency( const String & roleName ) const noexcept
{
    int32_t result = areg::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListDependencies.size(); ++ i )
    {
        if (mListDependencies.value_at(i).mRoleName == roleName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

bool areg::DependencyList::is_valid() const noexcept
{
    return (mListDependencies.size() != 0);
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

areg::ComponentEntry::ComponentEntry( const String & masterThreadName
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

areg::ComponentEntry::ComponentEntry( const String & masterThreadName
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

bool areg::ComponentEntry::operator == ( const areg::ComponentEntry & other ) const noexcept
{
    return (this != &other ? (mRoleName   == other.mRoleName)     &&
                             (mThreadName == other.mThreadName)
                           : true);
}

void areg::ComponentEntry::add_supported_service( const areg::ServiceEntry & entry )
{
    if (find_supported_service(entry) < 0)
    {
        mSupportedServices.mListServices.add(entry);
    }
}

void areg::ComponentEntry::add_supported_service( const areg::ServiceList & serviceList )
{
    for (uint32_t i = 0; i < serviceList.mListServices.size(); ++i)
    {
        add_supported_service(serviceList.mListServices[i]);
    }
}

areg::ServiceEntry & areg::ComponentEntry::add_supported_service(const String & serviceName, const Version & version)
{
    int32_t index = find_supported_service(serviceName);
    if ( index == areg::INVALID_INDEX )
    {
        index = static_cast<int32_t>(mSupportedServices.mListServices.size());
        mSupportedServices.mListServices.add(areg::ServiceEntry(serviceName, version));
    }

    return mSupportedServices.mListServices.value_at(static_cast<uint32_t>(index));
}

bool areg::ComponentEntry::remove_supported_service( const String & serviceName ) noexcept
{
    if (serviceName.is_empty())
        return false;

    for (uint32_t i = 0; i < mSupportedServices.mListServices.size(); ++i)
    {
        if (mSupportedServices.mListServices[i].mName == serviceName)
        {
            mSupportedServices.mListServices.remove_at(i, 1);
            return true;
        }
    }

    return false;
}

int32_t areg::ComponentEntry::find_supported_service( const areg::ServiceEntry& entry ) const noexcept
{
    return mSupportedServices.find_service(entry);
}

int32_t areg::ComponentEntry::find_supported_service( const String & serviceName ) const noexcept
{
    return mSupportedServices.find_service(serviceName);
}

void areg::ComponentEntry::add_worker_thread( const areg::WorkerThreadEntry& entry )
{
    if (find_worker_thread(entry) < 0)
    {
        mWorkerThreads.mListWorkers.add(entry);
    }
}

void areg::ComponentEntry::add_worker_thread( const areg::WorkerThreadList & workerList )
{
    for (uint32_t i = 0; i < workerList.mListWorkers.size(); ++i)
    {
        add_worker_thread(workerList.mListWorkers[i]);
    }
}

int32_t areg::ComponentEntry::find_worker_thread( const areg::WorkerThreadEntry& entry ) const noexcept
{
    return mWorkerThreads.find_thread(entry);
}

int32_t areg::ComponentEntry::find_worker_thread( const String & workerName ) const noexcept
{
    return mWorkerThreads.find_thread(workerName);
}

bool areg::ComponentEntry::remove_worker_thread( const String & workerName ) noexcept
{
    if (workerName.is_empty())
        return false;

    for (uint32_t i = 0; i < mWorkerThreads.mListWorkers.size(); ++i)
    {
        if (mWorkerThreads.mListWorkers[i].mThreadName == workerName)
        {
            mWorkerThreads.mListWorkers.remove_at(i);
            return true;
        }
    }

    return false;
}

void areg::ComponentEntry::add_dependency_service( const areg::DependencyEntry& entry )
{
    mDependencyServices.mListDependencies.add(entry);
}

void areg::ComponentEntry::add_dependency_service( const areg::DependencyList & dependencyList )
{
    for (uint32_t i = 0; i < dependencyList.mListDependencies.size(); ++i)
    {
        add_dependency_service(dependencyList.mListDependencies[i]);
    }
}

areg::DependencyEntry & areg::ComponentEntry::add_dependency_service(const String & roleName)
{
    int32_t index = static_cast<int32_t>(mDependencyServices.mListDependencies.size());
    mDependencyServices.mListDependencies.add(areg::DependencyEntry(roleName));
    return mDependencyServices.mListDependencies.value_at(static_cast<uint32_t>(index));
}

int32_t areg::ComponentEntry::find_dependency_service( const areg::DependencyEntry& entry ) const noexcept
{
    return mDependencyServices.find_dependency(entry);
}

bool areg::ComponentEntry::remove_dependency_service( const String & roleName ) noexcept
{
    if (roleName.is_empty())
        return false;

    for (uint32_t i = 0; i < mDependencyServices.mListDependencies.size(); ++i)
    {
        if (mDependencyServices.mListDependencies[i].mRoleName == roleName)
        {
            mDependencyServices.mListDependencies.remove_at(i);
            return true;
        }
    }

    return false;
}

int32_t areg::ComponentEntry::find_dependency_service( const String & roleName ) const noexcept
{
    return mDependencyServices.find_dependency(roleName);
}

const areg::ServiceList & areg::ComponentEntry::supported_services() const noexcept
{
    return mSupportedServices;
}

const areg::WorkerThreadList & areg::ComponentEntry::worker_threads() const noexcept
{
    return mWorkerThreads;
}

const areg::DependencyList & areg::ComponentEntry::dependency_services() const noexcept
{
    return mDependencyServices;
}

void areg::ComponentEntry::set_instance_methods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete ) noexcept
{
    mFuncCreate = fnCreate;
    mFuncDelete = fnDelete;
}

bool areg::ComponentEntry::is_valid() const noexcept
{
    return ( !mRoleName.is_empty() && (mFuncCreate != nullptr) && (mFuncDelete != nullptr) );
}

void areg::ComponentEntry::set_data( std::any compData ) noexcept
{
    mComponentData  = std::move(compData);
}

void areg::ComponentEntry::reset() noexcept
{
    mComponentData.reset();
}

std::any areg::ComponentEntry::data() const noexcept
{
    return mComponentData;
}

bool areg::ComponentEntry::has_data() const noexcept
{
    return mComponentData.has_value();
}

//////////////////////////////////////////////////////////////////////////
// class areg::ComponentList implementation
//////////////////////////////////////////////////////////////////////////

areg::ComponentList::ComponentList(  const areg::ComponentEntry & entry )
    : mListComponents   ( )
{
    if (entry.is_valid())
    {
        mListComponents.add(entry);
    }
}

const areg::ComponentEntry & areg::ComponentList::component( const String & roleName ) const noexcept
{
    int32_t index = find_component(roleName);
    return (index != areg::INVALID_INDEX ? mListComponents.value_at(static_cast<uint32_t>(index)) : areg::invalid_component_entry());
}

int32_t areg::ComponentList::find_component(const String & roleName) const noexcept
{
    int32_t result = areg::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListComponents.size(); ++ i )
    {
        if (mListComponents[i].mRoleName == roleName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

int32_t areg::ComponentList::find_component( const areg::ComponentEntry& entry ) const noexcept
{
    return find_component( entry.mRoleName );
}

bool areg::ComponentList::is_valid() const noexcept
{
    return (mListComponents.size() != 0);
}

bool areg::ComponentList::set_component_data( const String & roleName, std::any compData ) noexcept
{
    bool result = false;
    for (uint32_t i = 0; i < mListComponents.size(); ++ i )
    {
        areg::ComponentEntry & entry = mListComponents[i];
        if ( entry.mRoleName == roleName )
        {
            entry.set_data(compData);
            result = true;
            break;
        }
    }
    return result;
}

bool areg::ComponentList::reset(const String& roleName) noexcept
{
    bool result = false;
    for (uint32_t i = 0; i < mListComponents.size(); ++i)
    {
        areg::ComponentEntry& entry = mListComponents[i];
        if (entry.mRoleName == roleName)
        {
            entry.reset();
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

bool areg::ComponentThreadEntry::operator == ( const areg::ComponentThreadEntry & other ) const noexcept
{
    return ((this == &other) || (mThreadName == other.mThreadName));
}

bool areg::ComponentThreadEntry::operator != ( const areg::ComponentThreadEntry & other ) const noexcept
{
    return ((this != &other) && (mThreadName != other.mThreadName));
}

void areg::ComponentThreadEntry::add_component( const areg::ComponentEntry & entry )
{
    if (find_component_entry(entry) < 0)
    {
        mComponents.mListComponents.add(entry);
    }
}

void areg::ComponentThreadEntry::add_component( const areg::ComponentList & componentList )
{
    for (uint32_t i = 0; i < componentList.mListComponents.size(); ++i)
    {
        add_component(componentList.mListComponents[i]);
    }
}

areg::ComponentEntry & areg::ComponentThreadEntry::add_component(const String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete)
{
    int32_t index = mComponents.find_component(roleName);
    if ( index == areg::INVALID_INDEX )
    {
        index = static_cast<int32_t>(mComponents.mListComponents.size());
        mComponents.mListComponents.add( areg::ComponentEntry(mThreadName.as_string(), roleName, funcCreate, funcDelete));
    }

    return mComponents.mListComponents.value_at(static_cast<uint32_t>(index));
}

bool areg::ComponentThreadEntry::remove_component( const String & roleName ) noexcept
{
    bool result = false;
    if ( roleName.is_empty() == false)
    {
        for ( uint32_t i = 0; i < mComponents.mListComponents.size(); ++i )
        {
            if ( mComponents.mListComponents[i].mRoleName == roleName )
            {
                mComponents.mListComponents.remove_at(i);
                result = true;
                break;
            }
        }
    }

    return result;
}

int32_t areg::ComponentThreadEntry::find_component_entry( const areg::ComponentEntry & entry ) const noexcept
{
    return mComponents.find_component(entry);
}

int32_t areg::ComponentThreadEntry::find_component_entry( const String & roleName ) const noexcept
{
    return mComponents.find_component(roleName);
}

bool areg::ComponentThreadEntry::is_valid() const noexcept
{
    return ( (mThreadName.is_empty() == false) && (mComponents.mListComponents.is_empty() == false) );
}

bool areg::ComponentThreadEntry::set_component_data( const String & roleName, std::any compData ) noexcept
{
    return mComponents.set_component_data(roleName, compData);
}

bool areg::ComponentThreadEntry::reset(const String& roleName) noexcept
{
    return mComponents.reset(roleName);
}

//////////////////////////////////////////////////////////////////////////
// class areg::ComponentThreadList implementation
//////////////////////////////////////////////////////////////////////////

areg::ComponentThreadList::ComponentThreadList( const areg::ComponentThreadEntry & entry )
    : mListThreads  ( )
{
    if (entry.is_valid())
    {
        mListThreads.add(entry);
    }
}

const areg::ComponentThreadEntry & areg::ComponentThreadList::thread( const String & threadName ) const noexcept
{
    int32_t index = find_thread(threadName);
    return (index != areg::INVALID_INDEX ? mListThreads.value_at(static_cast<uint32_t>(index)) : areg::invalid_thread_entry());
}

int32_t areg::ComponentThreadList::find_thread( const areg::ComponentThreadEntry& entry ) const noexcept
{
    return find_thread( entry.mThreadName );
}

int32_t areg::ComponentThreadList::find_thread( const String & threadName ) const noexcept
{
    int32_t result = areg::INVALID_INDEX;
    for (uint32_t i = 0; i < mListThreads.size(); ++ i )
    {
        if (mListThreads[i].mThreadName == threadName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

bool areg::ComponentThreadList::is_valid() const noexcept
{
    return (mListThreads.size() != 0);
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

bool areg::Model::operator == ( const areg::Model & other ) const noexcept
{
    return (this == &other) || ((mModelName == other.mModelName) && (mModelThreads.mListThreads == other.mModelThreads.mListThreads));
}

bool areg::Model::operator != ( const areg::Model & other ) const noexcept
{
    return (this != &other) && ((mModelName != other.mModelName) || (mModelThreads.mListThreads != other.mModelThreads.mListThreads));
}

bool areg::Model::is_valid() const noexcept
{
    return ( (mModelName.is_empty() == false) && mModelThreads.is_valid() );
}

int32_t areg::Model::find_thread( const areg::ComponentThreadEntry& entry ) const noexcept
{
    return mModelThreads.find_thread(entry);
}

int32_t areg::Model::find_thread( const String & threadName ) const noexcept
{
    return mModelThreads.find_thread(threadName);
}

void areg::Model::add_thread( const areg::ComponentThreadEntry& entry )
{
    if (entry.is_valid() && (find_thread(entry) == areg::INVALID_INDEX))
    {
        mModelThreads.mListThreads.add(entry);
    }
}

void areg::Model::add_thread( const areg::ComponentThreadList& threadList )
{
    for (uint32_t i = 0; i < threadList.mListThreads.size(); ++i)
    {
        add_thread(threadList.mListThreads[i]);
    }
}

areg::ComponentThreadEntry & areg::Model::add_thread(const String & threadName)
{
    int32_t index = find_thread(threadName);
    if (index == areg::INVALID_INDEX )
    {
        index = static_cast<int32_t>(mModelThreads.mListThreads.size());
        mModelThreads.mListThreads.add(areg::ComponentThreadEntry(threadName));
    }

    return mModelThreads.mListThreads.value_at(static_cast<uint32_t>(index));
}

bool areg::Model::remove_thread( const String & threadName ) noexcept
{
    bool result = false;
    if ( threadName.is_empty() == false)
    {
        for ( uint32_t i = 0; i < mModelThreads.mListThreads.size(); ++i )
        {
            if ( mModelThreads.mListThreads[i].mThreadName == threadName )
            {
                mModelThreads.mListThreads.remove_at(i);
                result = true;
                break;
            }
        }
    }

    return result;
}

const areg::String & areg::Model::model_name() const noexcept
{
    return mModelName;
}

bool areg::Model::has_registered_component( const areg::ComponentEntry & entry ) const noexcept
{
    int32_t result = areg::INVALID_INDEX;
    for (uint32_t i = 0; (result == areg::INVALID_INDEX) && (i < mModelThreads.mListThreads.size()); ++i)
    {
        result = mModelThreads.mListThreads[i].find_component_entry(entry);
    }

    return ( result >= 0 );
}

bool areg::Model::has_registered_component( const String & roleName ) const noexcept
{
    int32_t result = areg::INVALID_INDEX;
    for (uint32_t i = 0; (result == areg::INVALID_INDEX) && (i < mModelThreads.mListThreads.size()); ++i)
    {
        result = mModelThreads.mListThreads[i].find_component_entry(roleName);
    }

    return ( result >= 0 );
}

bool areg::Model::is_model_loaded() const noexcept
{
    return (mLoadState == areg::Model::ModelState::Loaded);
}

void areg::Model::mark_model_loaded( bool isLoaded /*= true */ ) noexcept
{
    mLoadState = isLoaded ? Model::ModelState::Loaded : Model::ModelState::Unloaded;
}

void areg::Model::mark_model_alive( bool is_alive ) noexcept
{
    if ( is_alive )
    {
        mAliveDuration.start( );
    }
    else
    {
        mAliveDuration.stop( );
    }
}

const areg::ComponentThreadList & areg::Model::thread_list() const noexcept
{
    return mModelThreads;
}

bool areg::Model::set_component_data( const String & roleName, std::any compData )
{
    bool result = false;
    for ( uint32_t i = 0; i < mModelThreads.mListThreads.size(); ++ i )
    {
        areg::ComponentThreadEntry & entry = mModelThreads.mListThreads.value_at(i);
        if ( entry.set_component_data(roleName, compData) )
        {
            result = true;
            break;
        }
    }

    return result;
}

bool areg::Model::reset(const String& roleName) noexcept
{
    bool result = false;
    for (uint32_t i = 0; i < mModelThreads.mListThreads.size(); ++i)
    {
        areg::ComponentThreadEntry& entry = mModelThreads.mListThreads.value_at(i);
        if (entry.reset(roleName))
        {
            result = true;
            break;
        }
    }

    return result;
}
