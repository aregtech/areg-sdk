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
AREG_API_IMPL const NERegistry::ServiceEntry & NERegistry::invalid_service_entry()
{
    static const NERegistry::ServiceEntry _invalidServiceEntry;
    return _invalidServiceEntry;
}

/**
 * \brief   Predefined invalid Service List.
 **/
AREG_API_IMPL const NERegistry::ServiceList & NERegistry::invalid_service_list()
{
    static const NERegistry::ServiceList _invalidServiceList;
    return _invalidServiceList;
}

/**
 * \brief   Predefined invalid Worker Thread Entry.
 **/
AREG_API_IMPL const NERegistry::WorkerThreadEntry & NERegistry::invalid_worker_entry()
{
    static const NERegistry::WorkerThreadEntry _invalidWorkerThreadEntry;
    return _invalidWorkerThreadEntry;
}

/**
 * \brief   Predefined invalid Worker Thread List.
 **/
AREG_API_IMPL const NERegistry::WorkerThreadList & NERegistry::invalid_worker_list()
{
    static const NERegistry::WorkerThreadList _invalidWorkerThreadList;
    return _invalidWorkerThreadList;
}

/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API_IMPL const NERegistry::DependencyEntry & NERegistry::invalid_depedency_entry()
{
    static const NERegistry::DependencyEntry _invalidDepedencyEntry;
    return _invalidDepedencyEntry;
}

/**
 * \brief   Predefined invalid service dependency entry.
 **/
AREG_API_IMPL const NERegistry::DependencyList & NERegistry::invalid_depedency_list()
{
    static const NERegistry::DependencyList _invalidDepedencyList;
    return _invalidDepedencyList;
}

/**
 * \brief   Predefined Invalid Component Entry.
 **/
AREG_API_IMPL const NERegistry::ComponentEntry & NERegistry::invalid_component_entry()
{
    static const NERegistry::ComponentEntry _invalidComponentEntry;
    return _invalidComponentEntry;
}

/**
 * \brief   Predefined invalid Component List.
 **/
AREG_API_IMPL const NERegistry::ComponentList & NERegistry::invalid_component_list()
{
    static const NERegistry::ComponentList _invalidComponentList;
    return _invalidComponentList;
}

/**
 * \brief   Predefined invalid Thread Entry.
 **/
AREG_API_IMPL const NERegistry::ComponentThreadEntry & NERegistry::invalid_thread_entry()
{
    static const NERegistry::ComponentThreadEntry _invalidThreadEntry;
    return _invalidThreadEntry;
}

/**
 * \brief   Predefined invalid Thread List.
 **/
AREG_API_IMPL const NERegistry::ComponentThreadList & NERegistry::invalid_thread_list()
{
    static const NERegistry::ComponentThreadList _invalidThreadList;
    return _invalidThreadList;
}

/**
 * \brief   Predefined invalid Model.
 **/
AREG_API_IMPL const NERegistry::Model & NERegistry::invalid_model()
{
    static const NERegistry::Model _invalidModel;
    return _invalidModel;
}
//////////////////////////////////////////////////////////////////////////
// class NERegistry::ServiceEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::ServiceEntry::ServiceEntry( const String & serviceName, uint32_t major, uint32_t minor, uint32_t patch )
    : mName     ( serviceName )
    , mVersion  ( major, minor, patch )
{
    ASSERT( mName.is_empty( ) == false );
    ASSERT( mVersion.is_valid( ) );
}

NERegistry::ServiceEntry::ServiceEntry( const String & serviceName, const Version & version )
    : mName     (serviceName)
    , mVersion  (version)
{
    ASSERT( mName.is_empty() == false );
}

bool NERegistry::ServiceEntry::operator == ( const NERegistry::ServiceEntry & other ) const
{
    return (this == &other) || ((mName == other.mName) && (mVersion == other.mVersion));
}

bool NERegistry::ServiceEntry::operator != ( const NERegistry::ServiceEntry & other ) const
{
    return (this != &other) && ((mName != other.mName) || (mVersion != other.mVersion));
}

bool NERegistry::ServiceEntry::is_valid() const
{
    return ( (mName.is_empty() == false) && mVersion.is_valid() );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ServiceList implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::ServiceList::ServiceList( const NERegistry::ServiceEntry & entry )
    : mListServices ()
{
    if (entry.is_valid())
    {
        mListServices.add(entry);
    }
}

const NERegistry::ServiceEntry & NERegistry::ServiceList::service( const String & serviceName ) const
{
    int32_t index = find_service(serviceName);
    return (index == NECommon::INVALID_INDEX ? NERegistry::invalid_service_entry() : mListServices.at(static_cast<uint32_t>(index)));
}

int32_t NERegistry::ServiceList::find_service( const NERegistry::ServiceEntry & entry ) const
{
    return find_service( entry.mName );
}

int32_t NERegistry::ServiceList::find_service( const String & serviceName ) const
{
    int32_t result = NECommon::INVALID_INDEX;
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

bool NERegistry::ServiceList::is_valid() const
{
    return (mListServices.size() != 0 );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::WorkerThreadEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::WorkerThreadEntry::WorkerThreadEntry()
    : mThreadName       ()
    , mConsumerName     ()
    , mWatchdogTimeout  (NECommon::WATCHDOG_IGNORE)
    , mStackSizeKb      (NECommon::STACK_SIZE_DEFAULT)
    , mMaxQueue         (NECommon::IGNORE_VALUE)
{
}

NERegistry::WorkerThreadEntry::WorkerThreadEntry( const String & masterThreadName
                                                , const String & workerThreadName
                                                , const String & compRoleName
                                                , const String & compConsumerName
                                                , const uint32_t watchdogTimeout /* = NECommon::WATCHDOG_IGNORE     */
                                                , const uint32_t stackSizeKb     /* = NECommon::STACK_SIZE_DEFAULT  */
                                                , const uint32_t maxQueue        /* = NECommon::IGNORE_VALUE        */ )
    : mThreadName       (NEUtilities::create_component_item_name(masterThreadName, workerThreadName))
    , mConsumerName     (NEUtilities::create_component_item_name(compRoleName, compConsumerName))
    , mWatchdogTimeout  (watchdogTimeout)
    , mStackSizeKb      (stackSizeKb)
    , mMaxQueue         (maxQueue)
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

bool NERegistry::WorkerThreadEntry::is_valid() const
{
    return ( (mThreadName.is_empty() == false) && (mConsumerName.is_empty() == false) );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::WorkerThreadList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::WorkerThreadList::WorkerThreadList( const NERegistry::WorkerThreadEntry & entry )
    : mListWorkers  ()
{
    if (entry.is_valid())
    {
        mListWorkers.add(entry);
    }
}

const NERegistry::WorkerThreadEntry & NERegistry::WorkerThreadList::worker_thread( const String & threadName ) const
{
    int32_t index = find_thread(threadName);
    return (index == NECommon::INVALID_INDEX ? NERegistry::invalid_worker_entry() : mListWorkers.at(static_cast<uint32_t>(index)));
}

int32_t NERegistry::WorkerThreadList::find_thread( const String & threadName ) const
{
    int32_t result = NECommon::INVALID_INDEX;
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

int32_t NERegistry::WorkerThreadList::find_thread( const NERegistry::WorkerThreadEntry & entry ) const
{
    return find_thread( entry.mThreadName );
}

bool NERegistry::WorkerThreadList::is_valid() const
{
    return (mListWorkers.size() != 0  );
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::DependencyEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::DependencyEntry::DependencyEntry( const String & roleName )
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

bool NERegistry::DependencyEntry::is_valid() const
{
    return (mRoleName.is_empty() == false);
}

void NERegistry::DependencyEntry::set_dependent_service( const String & roleName )
{
    mRoleName   = roleName;
}

const String & NERegistry::DependencyEntry::depdendent_service() const
{
    return mRoleName;
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::DependencyList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::DependencyList::DependencyList( const NERegistry::DependencyEntry & entry  )
    : mListDependencies ( )
{
    if (entry.is_valid())
    {
        mListDependencies.add(entry);
    }
}

const NERegistry::DependencyEntry & NERegistry::DependencyList::dependency( const String & roleName ) const
{
    int32_t index = find_dependency(roleName);
    return (index == NECommon::INVALID_INDEX ? NERegistry::invalid_depedency_entry() : mListDependencies.at(static_cast<uint32_t>(index)) );
}

int32_t NERegistry::DependencyList::find_dependency( const NERegistry::DependencyEntry & entry ) const
{
    return find_dependency( entry.mRoleName );
}

int32_t NERegistry::DependencyList::find_dependency( const String & roleName ) const
{
    int32_t result = NECommon::INVALID_INDEX;
    for ( uint32_t i = 0; i < mListDependencies.size(); ++ i )
    {
        if (mListDependencies.at(i).mRoleName == roleName)
        {
            result = static_cast<int32_t>(i);
            break;
        }
    }

    return result;
}

bool NERegistry::DependencyList::is_valid() const
{
    return (mListDependencies.size() != 0);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ComponentEntry implementation
//////////////////////////////////////////////////////////////////////////
NERegistry::ComponentEntry::ComponentEntry()
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

NERegistry::ComponentEntry::ComponentEntry( const String & masterThreadName, const String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete )
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

NERegistry::ComponentEntry::ComponentEntry(   const String & masterThreadName
                                            , const String & roleName
                                            , FuncCreateComponent funcCreate
                                            , FuncDeleteComponent funcDelete
                                            , const NERegistry::ServiceList & serviceList
                                            , const NERegistry::DependencyList & dependencyList
                                            , const NERegistry::WorkerThreadList & workerList )
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

NERegistry::ComponentEntry::ComponentEntry(   const String & masterThreadName
                                            , const String & roleName
                                            , FuncCreateComponent funcCreate
                                            , FuncDeleteComponent funcDelete
                                            , const NERegistry::ServiceEntry & service
                                            , const NERegistry::DependencyEntry & dependency
                                            , const NERegistry::WorkerThreadEntry & worker )
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

NERegistry::ComponentEntry::ComponentEntry( const NERegistry::ComponentEntry & src )
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

NERegistry::ComponentEntry::ComponentEntry( NERegistry::ComponentEntry && src ) noexcept
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

NERegistry::ComponentEntry & NERegistry::ComponentEntry::operator = ( const NERegistry::ComponentEntry & src )
{
    if (static_cast<const NERegistry::ComponentEntry *>(this) != &src)
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

NERegistry::ComponentEntry & NERegistry::ComponentEntry::operator = ( NERegistry::ComponentEntry && src ) noexcept
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

bool NERegistry::ComponentEntry::operator == ( const NERegistry::ComponentEntry & other ) const
{
    return (this != &other ? (mRoleName   == other.mRoleName)     &&
                             (mThreadName == other.mThreadName)
                           : true);
}

void NERegistry::ComponentEntry::add_supported_service( const NERegistry::ServiceEntry & entry )
{
    if (find_supported_service(entry) < 0)
    {
        mSupportedServices.mListServices.add(entry);
    }
}

void NERegistry::ComponentEntry::add_supported_service( const NERegistry::ServiceList & serviceList )
{
    for (uint32_t i = 0; i < serviceList.mListServices.size(); ++i)
    {
        add_supported_service(serviceList.mListServices[i]);
    }
}

NERegistry::ServiceEntry & NERegistry::ComponentEntry::add_supported_service(const String & serviceName, const Version & version)
{
    int32_t index = find_supported_service(serviceName);
    if ( index == NECommon::INVALID_INDEX )
    {
        index = static_cast<int32_t>(mSupportedServices.mListServices.size());
        mSupportedServices.mListServices.add(NERegistry::ServiceEntry(serviceName, version));
    }

    return mSupportedServices.mListServices.at(static_cast<uint32_t>(index));
}

bool NERegistry::ComponentEntry::remove_supported_service( const String & serviceName )
{
    bool result = false;
    if ( serviceName.is_empty() == false)
    {
        for ( uint32_t i = 0; (result == false) && (i < mSupportedServices.mListServices.size( )); ++i )
        {
            if ( mSupportedServices.mListServices[i].mName == serviceName)
            {
                mSupportedServices.mListServices.remove_at(i, 1);
                result = true;
            }
        }
    }

    return result;
}

int32_t NERegistry::ComponentEntry::find_supported_service( const NERegistry::ServiceEntry& entry ) const
{
    return mSupportedServices.find_service(entry);
}

int32_t NERegistry::ComponentEntry::find_supported_service( const String & serviceName ) const
{
    return mSupportedServices.find_service(serviceName);
}

void NERegistry::ComponentEntry::add_worker_thread( const NERegistry::WorkerThreadEntry& entry )
{
    if (find_worker_thread(entry) < 0)
    {
        mWorkerThreads.mListWorkers.add(entry);
    }
}

void NERegistry::ComponentEntry::add_worker_thread( const NERegistry::WorkerThreadList & workerList )
{
    for (uint32_t i = 0; i < workerList.mListWorkers.size(); ++i)
    {
        add_worker_thread(workerList.mListWorkers[i]);
    }
}

int32_t NERegistry::ComponentEntry::find_worker_thread( const NERegistry::WorkerThreadEntry& entry ) const
{
    return mWorkerThreads.find_thread(entry);
}

int32_t NERegistry::ComponentEntry::find_worker_thread( const String & workerName ) const
{
    return mWorkerThreads.find_thread(workerName);
}

bool NERegistry::ComponentEntry::remove_worker_thread( const String & workerName )
{
    bool result = false;
    if ( workerName.is_empty() == false)
    {
        for ( uint32_t i = 0; i < mWorkerThreads.mListWorkers.size(); ++i )
        {
            if ( mWorkerThreads.mListWorkers[i].mThreadName == workerName )
            {
                mWorkerThreads.mListWorkers.remove_at(i);
                result = true;
                break;
            }
        }
    }

    return result;
}

void NERegistry::ComponentEntry::add_dependency_service( const NERegistry::DependencyEntry& entry )
{
    mDependencyServices.mListDependencies.add(entry);
}

void NERegistry::ComponentEntry::add_dependency_service( const NERegistry::DependencyList & dependencyList )
{
    for (uint32_t i = 0; i < dependencyList.mListDependencies.size(); ++i)
    {
        add_dependency_service(dependencyList.mListDependencies[i]);
    }
}

NERegistry::DependencyEntry & NERegistry::ComponentEntry::add_dependency_service(const String & roleName)
{
    int32_t index = static_cast<int32_t>(mDependencyServices.mListDependencies.size());
    mDependencyServices.mListDependencies.add(NERegistry::DependencyEntry(roleName));
    return mDependencyServices.mListDependencies.at(static_cast<uint32_t>(index));
}

int32_t NERegistry::ComponentEntry::find_dependency_service( const NERegistry::DependencyEntry& entry ) const
{
    return mDependencyServices.find_dependency(entry);
}

bool NERegistry::ComponentEntry::remove_dependency_service( const String & roleName )
{
    bool result = false;
    if ( roleName.is_empty() == false)
    {
        for (uint32_t i = 0; i < mDependencyServices.mListDependencies.size(); ++i )
        {
            if ( mDependencyServices.mListDependencies[i].mRoleName == roleName )
            {
                mDependencyServices.mListDependencies.remove_at(i);
                result = true;
                break;
            }
        }
    }
    return result;
}

int32_t NERegistry::ComponentEntry::find_dependency_service( const String & roleName ) const
{
    return mDependencyServices.find_dependency(roleName);
}

const NERegistry::ServiceList & NERegistry::ComponentEntry::supported_services() const
{
    return mSupportedServices;
}

const NERegistry::WorkerThreadList & NERegistry::ComponentEntry::worker_threads() const
{
    return mWorkerThreads;
}

const NERegistry::DependencyList & NERegistry::ComponentEntry::dependency_services() const
{
    return mDependencyServices;
}

void NERegistry::ComponentEntry::set_instance_methods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete )
{
    mFuncCreate = fnCreate;
    mFuncDelete = fnDelete;
}

bool NERegistry::ComponentEntry::is_valid() const
{
    return ( (mRoleName.is_empty() == false) && (mFuncCreate != nullptr) && (mFuncDelete != nullptr) );
}

void NERegistry::ComponentEntry::set_data( std::any compData )
{
    mComponentData  = std::move(compData);
}

void NERegistry::ComponentEntry::reset()
{
    mComponentData.reset();
}

std::any NERegistry::ComponentEntry::data() const
{
    return mComponentData;
}

bool NERegistry::ComponentEntry::has_data() const
{
    return mComponentData.has_value();
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ComponentList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::ComponentList::ComponentList(  const NERegistry::ComponentEntry & entry )
    : mListComponents   ( )
{
    if (entry.is_valid())
    {
        mListComponents.add(entry);
    }
}

const NERegistry::ComponentEntry & NERegistry::ComponentList::component( const String & roleName ) const
{
    int32_t index = find_component(roleName);
    return (index != NECommon::INVALID_INDEX ? mListComponents.at(static_cast<uint32_t>(index)) : NERegistry::invalid_component_entry());
}

int32_t NERegistry::ComponentList::find_component(const String & roleName) const
{
    int32_t result = NECommon::INVALID_INDEX;
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

int32_t NERegistry::ComponentList::find_component( const NERegistry::ComponentEntry& entry ) const
{
    return find_component( entry.mRoleName );
}

bool NERegistry::ComponentList::is_valid() const
{
    return (mListComponents.size() != 0);
}

bool NERegistry::ComponentList::set_component_data( const String & roleName, std::any compData )
{
    bool result = false;
    for (uint32_t i = 0; i < mListComponents.size(); ++ i )
    {
        NERegistry::ComponentEntry & entry = mListComponents[i];
        if ( entry.mRoleName == roleName )
        {
            entry.set_data(compData);
            result = true;
            break;
        }
    }
    return result;
}

bool NERegistry::ComponentList::reset(const String& roleName)
{
    bool result = false;
    for (uint32_t i = 0; i < mListComponents.size(); ++i)
    {
        NERegistry::ComponentEntry& entry = mListComponents[i];
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
// class NERegistry::ComponentThreadEntry implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::ComponentThreadEntry::ComponentThreadEntry()
    : mThreadName       ( )
    , mComponents       ( )
    , mWatchdogTimeout  (NECommon::WATCHDOG_IGNORE)
    , mStackSizeKB      (NECommon::STACK_SIZE_DEFAULT)
    , mMaxQueue         (NECommon::IGNORE_VALUE)
{
}

NERegistry::ComponentThreadEntry::ComponentThreadEntry( const String & threadName
                                                      , const uint32_t watchdogTimeout  /* = NECommon::WATCHDOG_IGNORE      */
                                                      , const uint32_t stackSizeKb      /* = NECommon::STACK_SIZE_DEFAULT   */ 
                                                      , const uint32_t maxQueue         /* = NECommon::IGNORE_VALUE         */)
    : mThreadName       (threadName)
    , mComponents       ( )
    , mWatchdogTimeout  (watchdogTimeout)
    , mStackSizeKB      (stackSizeKb)
    , mMaxQueue         (maxQueue)
{
}

NERegistry::ComponentThreadEntry::ComponentThreadEntry( const String & threadName
                                                      , const NERegistry::ComponentList& supCompList
                                                      , const uint32_t watchdogTimeout  /* = NECommon::WATCHDOG_IGNORE      */
                                                      , const uint32_t stackSizeKb      /* = NECommon::STACK_SIZE_DEFAULT   */ 
                                                      , const uint32_t maxQueue         /* = NECommon::IGNORE_VALUE         */)
    : mThreadName       (threadName)
    , mComponents       (supCompList)
    , mWatchdogTimeout  (watchdogTimeout)
    , mStackSizeKB      (stackSizeKb)
    , mMaxQueue         (maxQueue)
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

void NERegistry::ComponentThreadEntry::add_component( const NERegistry::ComponentEntry & entry )
{
    if (find_component_entry(entry) < 0)
    {
        mComponents.mListComponents.add(entry);
    }
}

void NERegistry::ComponentThreadEntry::add_component( const NERegistry::ComponentList & componentList )
{
    for (uint32_t i = 0; i < componentList.mListComponents.size(); ++i)
    {
        add_component(componentList.mListComponents[i]);
    }
}

NERegistry::ComponentEntry & NERegistry::ComponentThreadEntry::add_component(const String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete)
{
    int32_t index = mComponents.find_component(roleName);
    if ( index == NECommon::INVALID_INDEX )
    {
        index = static_cast<int32_t>(mComponents.mListComponents.size());
        mComponents.mListComponents.add( NERegistry::ComponentEntry(mThreadName.as_string(), roleName, funcCreate, funcDelete));
    }

    return mComponents.mListComponents.at(static_cast<uint32_t>(index));
}

bool NERegistry::ComponentThreadEntry::remove_component( const String & roleName )
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

int32_t NERegistry::ComponentThreadEntry::find_component_entry( const NERegistry::ComponentEntry & entry ) const
{
    return mComponents.find_component(entry);
}

int32_t NERegistry::ComponentThreadEntry::find_component_entry( const String & roleName ) const
{
    return mComponents.find_component(roleName);
}

bool NERegistry::ComponentThreadEntry::is_valid() const
{
    return ( (mThreadName.is_empty() == false) && (mComponents.mListComponents.is_empty() == false) );
}

bool NERegistry::ComponentThreadEntry::set_component_data( const String & roleName, std::any compData )
{
    return mComponents.set_component_data(roleName, compData);
}

bool NERegistry::ComponentThreadEntry::reset(const String& roleName)
{
    return mComponents.reset(roleName);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::ComponentThreadList implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::ComponentThreadList::ComponentThreadList( const NERegistry::ComponentThreadEntry & entry )
    : mListThreads  ( )
{
    if (entry.is_valid())
    {
        mListThreads.add(entry);
    }
}

const NERegistry::ComponentThreadEntry & NERegistry::ComponentThreadList::thread( const String & threadName ) const
{
    int32_t index = find_thread(threadName);
    return (index != NECommon::INVALID_INDEX ? mListThreads.at(static_cast<uint32_t>(index)) : NERegistry::invalid_thread_entry());
}

int32_t NERegistry::ComponentThreadList::find_thread( const NERegistry::ComponentThreadEntry& entry ) const
{
    return find_thread( entry.mThreadName );
}

int32_t NERegistry::ComponentThreadList::find_thread( const String & threadName ) const
{
    int32_t result = NECommon::INVALID_INDEX;
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

bool NERegistry::ComponentThreadList::is_valid() const
{
    return (mListThreads.size() != 0);
}

//////////////////////////////////////////////////////////////////////////
// class NERegistry::Model implementation
//////////////////////////////////////////////////////////////////////////

NERegistry::Model::Model()
    : mModelName    ( )
    , mModelThreads ( )
    , mLoadState    ( Model::ModelState::Initialized )
    , mAliveDuration( )
{
}

NERegistry::Model::Model( const String & modelName )
    : mModelName    (modelName)
    , mModelThreads ( )
    , mLoadState    ( Model::ModelState::Initialized )
    , mAliveDuration( )
{
}

NERegistry::Model::Model( const String & modelName, const ComponentThreadList & threadList  )
    : mModelName    (modelName)
    , mModelThreads (threadList)
    , mLoadState    ( Model::ModelState::Initialized )
    , mAliveDuration( )
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

bool NERegistry::Model::is_valid() const
{
    return ( (mModelName.is_empty() == false) && mModelThreads.is_valid() );
}

int32_t NERegistry::Model::find_thread( const NERegistry::ComponentThreadEntry& entry ) const
{
    return mModelThreads.find_thread(entry);
}

int32_t NERegistry::Model::find_thread( const String & threadName ) const
{
    return mModelThreads.find_thread(threadName);
}

void NERegistry::Model::add_thread( const NERegistry::ComponentThreadEntry& entry )
{
    if (entry.is_valid() && (find_thread(entry) == NECommon::INVALID_INDEX))
    {
        mModelThreads.mListThreads.add(entry);
    }
}

void NERegistry::Model::add_thread( const NERegistry::ComponentThreadList& threadList )
{
    for (uint32_t i = 0; i < threadList.mListThreads.size(); ++i)
    {
        add_thread(threadList.mListThreads[i]);
    }
}

NERegistry::ComponentThreadEntry & NERegistry::Model::add_thread(const String & threadName)
{
    int32_t index = find_thread(threadName);
    if (index == NECommon::INVALID_INDEX )
    {
        index = static_cast<int32_t>(mModelThreads.mListThreads.size());
        mModelThreads.mListThreads.add(NERegistry::ComponentThreadEntry(threadName));
    }

    return mModelThreads.mListThreads.at(static_cast<uint32_t>(index));
}

bool NERegistry::Model::remove_thread( const String & threadName )
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

const String & NERegistry::Model::model_name() const
{
    return mModelName;
}

bool NERegistry::Model::has_registered_component( const NERegistry::ComponentEntry & entry ) const
{
    int32_t result = NECommon::INVALID_INDEX;
    for (uint32_t i = 0; (result == NECommon::INVALID_INDEX) && (i < mModelThreads.mListThreads.size()); ++i)
    {
        result = mModelThreads.mListThreads[i].find_component_entry(entry);
    }

    return ( result >= 0 );
}

bool NERegistry::Model::has_registered_component( const String & roleName ) const
{
    int32_t result = NECommon::INVALID_INDEX;
    for (uint32_t i = 0; (result == NECommon::INVALID_INDEX) && (i < mModelThreads.mListThreads.size()); ++i)
    {
        result = mModelThreads.mListThreads[i].find_component_entry(roleName);
    }

    return ( result >= 0 );
}

bool NERegistry::Model::is_model_loaded() const
{
    return (mLoadState == NERegistry::Model::ModelState::Loaded);
}

void NERegistry::Model::mark_model_loaded( bool isLoaded /*= true */ )
{
    mLoadState = isLoaded ? Model::ModelState::Loaded : Model::ModelState::Unloaded;
}

void NERegistry::Model::mark_model_alive( bool is_alive )
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

const NERegistry::ComponentThreadList & NERegistry::Model::thread_list() const
{
    return mModelThreads;
}

bool NERegistry::Model::set_component_data( const String & roleName, std::any compData )
{
    bool result = false;
    for ( uint32_t i = 0; i < mModelThreads.mListThreads.size(); ++ i )
    {
        NERegistry::ComponentThreadEntry & entry = mModelThreads.mListThreads.at(i);
        if ( entry.set_component_data(roleName, compData) )
        {
            result = true;
            break;
        }
    }

    return result;
}

bool NERegistry::Model::reset(const String& roleName)
{
    bool result = false;
    for (uint32_t i = 0; i < mModelThreads.mListThreads.size(); ++i)
    {
        NERegistry::ComponentThreadEntry& entry = mModelThreads.mListThreads.at(i);
        if (entry.reset(roleName))
        {
            result = true;
            break;
        }
    }

    return result;
}
