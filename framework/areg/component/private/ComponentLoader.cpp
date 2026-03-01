/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentLoader.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Loader singleton object.
 *
 ************************************************************************/
#include "areg/component/ComponentLoader.hpp"

#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/base/CommonDefs.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ModelDataCreator class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ModelDataCreator class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ModelDataCreator::ModelDataCreator( FuncInitLoaderItem funtCreateModelData, const char * modelName )
{
    ASSERT( funtCreateModelData != nullptr );
    areg::Model newModel = funtCreateModelData(modelName);
    VERIFY( ComponentLoader::instance().add_model( newModel ) );
}

ModelDataCreator::ModelDataCreator( const areg::Model & newModel )
{
    VERIFY( ComponentLoader::instance().add_model( newModel ) );
}


//////////////////////////////////////////////////////////////////////////
// ComponentLoader class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class, static functions
//////////////////////////////////////////////////////////////////////////
ComponentLoader & ComponentLoader::instance()
{
    static ComponentLoader _componentLoader;
    return _componentLoader;
}

bool ComponentLoader::load_component_model( const String & modelName )
{
    bool result = ComponentLoader::instance().load_model( modelName );
    if ( result == false )
    {
        ComponentLoader::instance( ).unload_model( true, modelName );
    }

    return result;
}

void ComponentLoader::unload_component_model(bool waitComplete, const String & modelName )
{
    ComponentLoader::instance( ).unload_model(waitComplete, modelName );
}

void ComponentLoader::wait_model_unload(const String & modelName)
{
    ComponentLoader::instance().wait_model_threads(modelName);
}

const areg::Model & ComponentLoader::find_model( const String & modelName )
{
    ComponentLoader & loader = instance( );
    Lock lock( loader.mLock );

    const areg::Model * result = loader.find_model_named( modelName );
    return (result != nullptr ? *result : areg::invalid_model( ));
}

const areg::ComponentList& ComponentLoader::find_component_list( const String & threadName )
{

    const areg::ComponentList * result{ nullptr };
    ComponentLoader& loader = instance();
    Lock lock(loader.mLock);

    for ( uint32_t i = 0; result == nullptr && i < loader.mModelList.size(); ++ i )
    {
        const areg::Model & model = loader.mModelList.at(i);
        if ( model.is_model_loaded() )
        {
            const areg::ComponentThreadList & threadList = model.thread_list();
            for ( uint32_t j = 0; j < threadList.mListThreads.size(); ++ j )
            {
                const areg::ComponentThreadEntry & thrEntry = threadList.mListThreads.at(j);
                if (thrEntry.mThreadName == threadName)
                {
                    result = &thrEntry.mComponents;
                    break;
                }
            }
        }
    }

    return (result != nullptr ? *result : areg::invalid_component_list());
}

const areg::ComponentEntry& ComponentLoader::find_component_entry( const String & roleName, const String & threadName )
{
    const areg::ComponentEntry* result = nullptr;
    const areg::ComponentList& comList = find_component_list(threadName);
    if (comList.is_valid())
    {
        for (uint32_t i = 0; i < comList.mListComponents.size(); ++ i)
        {
            const areg::ComponentEntry& entry = comList.mListComponents[i];
            if (entry.mRoleName == roleName)
            {
                result = &entry;
                break;
            }
        }
    }

    return (result != nullptr ? *result : areg::invalid_component_entry());
}

const areg::ComponentEntry& ComponentLoader::find_component_entry( const String & roleName )
{
    const areg::ComponentEntry* result = nullptr;
    ComponentLoader & loader = ComponentLoader::instance();
    Lock lock(loader.mLock);

    for (uint32_t i = 0; (result == nullptr) && (i < loader.mModelList.size()); ++ i )
    {
        const areg::ComponentThreadList & threadList = loader.mModelList.at(i).thread_list();

        for (uint32_t j = 0; (result == nullptr) && (j < threadList.mListThreads.size()); ++ j )
        {
            const areg::ComponentThreadEntry & threadEntry = threadList.mListThreads.at(j);
            for (uint32_t k = 0; k < threadEntry.mComponents.mListComponents.size(); ++k)
            {
                if (threadEntry.mComponents.mListComponents.at(k).mRoleName == roleName)
                {
                    result = &threadEntry.mComponents.mListComponents.at(k);
                    break;
                }
            }
        }
    }

    return (result != nullptr ? *result : areg::invalid_component_entry());
}

const areg::ComponentThreadEntry& ComponentLoader::find_thread_entry(const String& threadName)
{
    const areg::ComponentThreadEntry* result = nullptr;
    ComponentLoader& loader = ComponentLoader::instance();
    Lock lock(loader.mLock);

    for (uint32_t i = 0; (result == nullptr) && (i < loader.mModelList.size()); ++i)
    {
        const areg::ComponentThreadList& threadList = loader.mModelList.at(i).thread_list();

        for (uint32_t j = 0; j < threadList.mListThreads.size(); ++j)
        {
            const areg::ComponentThreadEntry& threadEntry = threadList.mListThreads.at(j);
            if (threadEntry.mThreadName == threadName)
            {
                result = &threadEntry;
                break;
            }
        }
    }

    return (result != nullptr ? *result : areg::invalid_thread_entry());
}

bool ComponentLoader::is_model_loaded( const String & modelName )
{
    bool result = false;

    if ( modelName.is_empty() == false)
    {
        ComponentLoader & loader = ComponentLoader::instance();
        Lock lock(loader.mLock);

        for ( uint32_t i = 0; i < loader.mModelList.size(); ++ i )
        {
            const areg::Model & model = loader.mModelList.at(i);
            if ( model.model_name() == modelName )
            {
                result = model.is_model_loaded();
                break;
            }
        }
    }

    return result;
}

bool ComponentLoader::exist( const String & modelName )
{
    bool result = false;

    if ( !modelName.is_empty() )
    {
        ComponentLoader & loader = ComponentLoader::instance( );
        Lock lock( loader.mLock );

        for ( uint32_t i = 0; i < loader.mModelList.size( ); ++ i )
        {
            const areg::Model & model = loader.mModelList.at( i );
            if ( model.model_name( ) == modelName )
            {
                result = true;
                break;
            }
        }
    }

    return result;
}

bool ComponentLoader::set_component_data( const String & roleName, std::any compData )
{
    bool result = false;
    ComponentLoader & loader = ComponentLoader::instance( );
    Lock lock( loader.mLock );

    for ( uint32_t i = 0; i < loader.mModelList.size( ); ++ i )
    {
        areg::Model & model = loader.mModelList.at( i );
        if ( model.set_component_data( roleName, compData ) )
        {
            result = true;
            break;
        }
    }

    return result;
}

bool ComponentLoader::reset(const String& roleName)
{
    bool result = false;
    ComponentLoader& loader = ComponentLoader::instance();
    Lock lock(loader.mLock);

    for (uint32_t i = 0; i < loader.mModelList.size(); ++i)
    {
        areg::Model& model = loader.mModelList.at(i);
        if (model.reset(roleName))
        {
            result = true;
            break;
        }
    }

    return result;
}

bool ComponentLoader::add_model_unique(const areg::Model & newModel)
{
    ComponentLoader & loader = ComponentLoader::instance();
    Lock lock( loader.mLock );
    return loader.add_model(newModel);
}

void ComponentLoader::remove_component_model(const String & modelName /*= String::empty_string() */)
{
    ComponentLoader::unload_component_model(true, modelName);
    ComponentLoader & loader = ComponentLoader::instance();
    Lock lock( loader.mLock );
    if ( modelName.is_empty() == false)
    {
        for ( uint32_t i = 0; i < loader.mModelList.size(); ++ i )
        {
            areg::Model & model = loader.mModelList[i];
            if ( model.model_name() == modelName )
            {
                loader.mModelList.remove_at(i);
                break;
            }
        }
    }
    else
    {
        loader.mModelList.clear();
    }
}

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ComponentLoader::ComponentLoader()
    : mModelList    ( )
    , mDefaultModel ( areg::EmptyStringA.data( ) )
    , mLock         ( )
{
}

ComponentLoader::~ComponentLoader()
{
    mModelList.clear();
    mDefaultModel   = areg::EmptyStringA.data( );
}

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class, methods
//////////////////////////////////////////////////////////////////////////
bool ComponentLoader::add_model( const areg::Model & newModel )
{
    Lock  lock(mLock);
    bool succeed { (newModel.model_name().is_empty() == false) && (newModel.is_model_loaded() == false) };
    // the new model name cannot be empty and it should be unique, and it cannot be marked as loaded.
    ASSERT(succeed);

    // search if model with the same name exists
    for (uint32_t i = 0; succeed && (i < mModelList.size()); ++ i )
    {
        const areg::Model & regModel = mModelList.at(i);
        if ( newModel.model_name() != regModel.model_name() )
        {
            const areg::ComponentThreadList & regThreadList = regModel.thread_list();
            for ( uint32_t j = 0; succeed && (j < regThreadList.mListThreads.size()); ++ j )
            {
                const areg::ComponentThreadEntry & regThreadEntry = regThreadList.mListThreads.at(j);
                if ( newModel.find_thread(regThreadEntry) < 0 )
                {
                    const areg::ComponentList & regComponentList = regThreadEntry.mComponents;
                    for ( uint32_t k = 0; succeed && (k < regComponentList.mListComponents.size()); ++ k )
                    {
                        const areg::ComponentEntry & regComponentEntry = regComponentList.mListComponents.at(k);
                        succeed = !newModel.has_registered_component(regComponentEntry);
                    } // end of for ( int k = 0; hasError == false && k < newComponentList.GetSize(); k ++ )
                }
                else
                {
                    succeed = false;
                    ASSERT(false);
                }
            } // end of for ( int j = 0; hasError == false && j < newThreadList.GetSize(); j ++ )
        }
        else
        {
            succeed = false;
            ASSERT(false);
        }
    }

    if ( succeed )
    {
        mModelList.add(newModel);
        if ( mDefaultModel.is_empty() )
        {
            mDefaultModel = newModel.model_name();
        }
    }
    
    return succeed;
}

int32_t ComponentLoader::load_all_models()
{
    return load_model(String::EmptyString);
}

int32_t ComponentLoader::load_model( const String & modelName )
{
    Lock lock(mLock);
    int32_t result{ 0 };
    for (uint32_t i = 0; i < mModelList.size(); ++i)
    {
        areg::Model & model = mModelList[i];
        if (modelName.is_empty() || (model.model_name() == modelName))
        {
            result += load_model(model) ? 1 : 0;
            if (modelName.is_empty() == false)
                break;  // break the loop
        }
    }

    return result;
}

bool ComponentLoader::load_model( areg::Model & whichModel ) const
{
    bool result = false;

    if ( whichModel.is_valid() && (whichModel.is_model_loaded( ) == false) )
    {
        const areg::ComponentThreadList& thrList = whichModel.thread_list( );
        whichModel.mark_model_loaded( true );
        result = true;
        for ( uint32_t i = 0; result && i < thrList.mListThreads.size( ); ++ i )
        {
            Lock lock( mLock );
            const areg::ComponentThreadEntry& entry = thrList.mListThreads[i];
            if ( entry.is_valid( ) && Thread::find_by_name( entry.mThreadName ) == nullptr )
            {
                ComponentThread* thrObject = DEBUG_NEW ComponentThread( entry.mThreadName, entry.mWatchdogTimeout, entry.mStackSizeKB, entry.mMaxQueue );
                if ( thrObject != nullptr )
                {
                    if ( thrObject->create_thread( areg::WAIT_INFINITE ) == false )
                    {
                        thrObject->shutdown_thread( areg::DO_NOT_WAIT );
                        delete thrObject;
                        result = false;
                    }
                }
                else
                {
                    result = false;
                }
            }
            else
            {
                result = entry.is_valid( );
            }
        }

        whichModel.mark_model_alive( result );
    }
    else
    {
        result = true;
    }

    return result;
}

void ComponentLoader::unload_model(bool waitComplete, const String & modelName )
{
    for (uint32_t i = 0; i < mModelList.size(); ++ i)
    {
        areg::Model & model = mModelList[i];
        if (modelName.is_empty() || (modelName == model.model_name()))
        {
            unload_model(waitComplete, model);
            if (modelName.is_empty() == false)
                break;  // break the loop
        }
    }
}

void ComponentLoader::unload_model( bool waitComplete, areg::Model & whichModel ) const
{
    Lock lock(mLock);

    if (whichModel.is_model_loaded() )
    {
        whichModel.mark_model_alive( false );

        const areg::ComponentThreadList & modelThreads = whichModel.thread_list();
        ThreadList threadList( modelThreads.mListThreads.size( ) );
        for ( uint32_t i = 0; i < modelThreads.mListThreads.size( ); ++ i )
        {
            Thread * compThread = Thread::find_by_name(modelThreads.mListThreads.at( i ).mThreadName);
            if (compThread != nullptr)
            {
                ASSERT(AREG_RUNTIME_CAST(compThread, ComponentThread));
                threadList.add(compThread);
            }
        }

        _exit_threads( threadList );

        if (waitComplete)
        {
            lock.unlock();
            _wait_threads(threadList);

            lock.lock();
            _shutdown_threads(threadList);
        }

        whichModel.mark_model_loaded(false);
    }
}

void ComponentLoader::wait_model_threads(const String & modelName )
{
    for (uint32_t i = 0; i < mModelList.size(); ++ i)
    {
        areg::Model & model = mModelList[i];
        if (modelName.is_empty() || (modelName == model.model_name()))
        {
            wait_model_threads(model);
            if (modelName.is_empty() == false)
                break;  // break the loop
        }
    }
}

void ComponentLoader::wait_model_threads(areg::Model & whichModel)
{
    Lock lock(mLock);

    if (whichModel.is_model_loaded())
    {
        const areg::ComponentThreadList & modelThreads = whichModel.thread_list();
        ThreadList threadList(modelThreads.mListThreads.size());
        for (uint32_t i = 0; i < modelThreads.mListThreads.size(); ++ i)
        {
            Thread * compThread = Thread::find_by_name(modelThreads.mListThreads.at(i).mThreadName);
            if (compThread != nullptr)
            {
                ASSERT(AREG_RUNTIME_CAST(compThread, ComponentThread) != nullptr);
                threadList.add(compThread);
            }
        }

        lock.unlock();
        _wait_threads(threadList);

        lock.lock();
        _shutdown_threads(threadList);

        whichModel.mark_model_loaded(false);
    }
}

void ComponentLoader::_exit_threads( const ThreadList & threadList ) const
{
    for (uint32_t i = 0; i < threadList.size(); ++ i )
    {
        Thread* thrObject = threadList[i];
        ASSERT( thrObject != nullptr );
        thrObject->trigger_exit( );
    }
}

void ComponentLoader::_wait_threads( const ThreadList & threadList ) const
{
    for ( uint32_t i = 0; i < threadList.size(); ++ i )
    {
        Thread * thrObject = threadList[i];
        ASSERT( thrObject != nullptr );
        thrObject->completion_wait( areg::WAIT_INFINITE );
    }
}

void ComponentLoader::_shutdown_threads( const ThreadList & threadList ) const
{
    for ( uint32_t i = 0; i < threadList.size(); ++ i )
    {
        Thread* thrObject = threadList[i];
        ASSERT( thrObject != nullptr );
        thrObject->shutdown_thread( areg::DO_NOT_WAIT );
        delete thrObject;
    }
}

const areg::Model * ComponentLoader::find_model_named( const String & modelName ) const
{
    const areg::Model * result = nullptr;
    if ( modelName.is_empty() == false)
    {
        for ( uint32_t i = 0; i < mModelList.size( ); ++i )
        {
            const areg::Model & model = mModelList[i];
            if (model.model_name() == modelName)
            {
                result = &model;
                break;
            }
        }
    }

    return result;
}

const areg::ComponentThreadEntry * ComponentLoader::find_thread_named( const String & threadName ) const
{
    const areg::ComponentThreadEntry * result = nullptr;
    if ( threadName.is_empty() == false)
    {
        for ( uint32_t i = 0; i < mModelList.size(); ++i )
        {
            const areg::Model & model = mModelList[i];
            int32_t index = model.find_thread(threadName);
            if ( index != areg::INVALID_INDEX )
            {
                const areg::ComponentThreadEntry & entry = model.thread_list().mListThreads.at(static_cast<uint32_t>(index));
                result = &entry;
                break;
            }
        }
    }

    return result;
}

const areg::ComponentEntry * ComponentLoader::find_component_named( const String & roleName ) const
{
    const areg::ComponentEntry * result = nullptr;
    if ( roleName.is_empty() == false)
    {
        for ( uint32_t i = 0; (result == nullptr) && (i < mModelList.size()); ++i )
        {
            const areg::Model & model = mModelList[i];
            const areg::ComponentThreadList & threadList= model.thread_list();
            for ( uint32_t j = 0; i < threadList.mListThreads.size(); ++i )
            {
                const areg::ComponentThreadEntry & thread   = threadList.mListThreads.at(j);
                int32_t index = thread.find_component_entry(roleName);
                if ( index != areg::INVALID_INDEX )
                {
                    const areg::ComponentEntry & entry = thread.mComponents.mListComponents.at(static_cast<uint32_t>(index));
                    result = & entry;
                    break;
                }
            }
        }
    }

    return result;
}

} // namespace areg
