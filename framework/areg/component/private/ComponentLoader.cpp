/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
ComponentLoader & ComponentLoader::instance() noexcept
{
    static ComponentLoader _componentLoader;
    return _componentLoader;
}

bool ComponentLoader::load_component_model( const String & modelName, bool waitComponents /*= false*/ )
{
    return ServiceManager::request_load_model( modelName, waitComponents );
}

void ComponentLoader::unload_component_model(bool waitComplete, const String & modelName )
{
    ServiceManager::request_unload_model( modelName, waitComplete );
}

void ComponentLoader::wait_model_unload(const String & modelName)
{
    ServiceManager::request_unload_model( modelName, true );
}

areg::ModelState ComponentLoader::model_state( const String & modelName )
{
    ComponentLoader & loader = instance( );
    Lock lock( loader.mLock );

    const areg::Model * result = loader.find_model_named( modelName );
    return (result != nullptr ? result->model_state( ) : areg::ModelState::Unloaded);
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
        // Answers for a model that is loading, loaded or unloading.
        const areg::Model & model = loader.mModelList.value_at(i);
        if ( model.model_state() != areg::ModelState::Unloaded )
        {
            const areg::ComponentThreadList & threadList = model.thread_list();
            for ( uint32_t j = 0; j < threadList.mListThreads.size(); ++ j )
            {
                const areg::ComponentThreadEntry & thrEntry = threadList.mListThreads.value_at(j);
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
        const areg::ComponentThreadList & threadList = loader.mModelList.value_at(i).thread_list();

        for (uint32_t j = 0; (result == nullptr) && (j < threadList.mListThreads.size()); ++ j )
        {
            const areg::ComponentThreadEntry & threadEntry = threadList.mListThreads.value_at(j);
            for (uint32_t k = 0; k < threadEntry.mComponents.mListComponents.size(); ++k)
            {
                if (threadEntry.mComponents.mListComponents.value_at(k).mRoleName == roleName)
                {
                    result = &threadEntry.mComponents.mListComponents.value_at(k);
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
        const areg::ComponentThreadList& threadList = loader.mModelList.value_at(i).thread_list();

        for (uint32_t j = 0; j < threadList.mListThreads.size(); ++j)
        {
            const areg::ComponentThreadEntry& threadEntry = threadList.mListThreads.value_at(j);
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
            const areg::Model & model = loader.mModelList.value_at(i);
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
            const areg::Model & model = loader.mModelList.value_at(i );
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
        areg::Model & model = loader.mModelList.value_at(i );
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
        areg::Model& model = loader.mModelList.value_at(i);
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
    // the new model name cannot be empty and it should be unique.
    ASSERT(succeed);

    for (uint32_t i = 0; succeed && (i < mModelList.size()); ++ i )
    {
        const areg::Model & regModel = mModelList.value_at(i);
        if ( newModel.model_name() != regModel.model_name() )
        {
            const areg::ComponentThreadList & regThreadList = regModel.thread_list();
            for ( uint32_t j = 0; succeed && (j < regThreadList.mListThreads.size()); ++ j )
            {
                const areg::ComponentThreadEntry & regThreadEntry = regThreadList.mListThreads.value_at(j);
                if ( newModel.find_thread(regThreadEntry) < 0 )
                {
                    const areg::ComponentList & regComponentList = regThreadEntry.mComponents;
                    for ( uint32_t k = 0; succeed && (k < regComponentList.mListComponents.size()); ++ k )
                    {
                        const areg::ComponentEntry & regComponentEntry = regComponentList.mListComponents.value_at(k);
                        succeed = !newModel.has_registered_component(regComponentEntry);
                    }
                }
                else
                {
                    succeed = false;
                    ASSERT(false);
                }
            }
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

const areg::Model * ComponentLoader::find_model_named( const String & modelName ) const noexcept
{
    if (modelName.is_empty())
        return nullptr;
    
    for ( uint32_t i = 0; i < mModelList.size( ); ++i )
    {
        const areg::Model & model = mModelList[i];
        if (model.model_name() == modelName)
        {
            return &model;
        }
    }

    return nullptr;
}

const areg::ComponentThreadEntry * ComponentLoader::find_thread_named( const String & threadName ) const noexcept
{
    if (threadName.is_empty())
        return nullptr;
    
    for ( uint32_t i = 0; i < mModelList.size(); ++i )
    {
        const areg::Model & model = mModelList[i];
        int32_t index = model.find_thread(threadName);
        if ( index != areg::INVALID_INDEX )
        {
            const areg::ComponentThreadEntry & entry = model.thread_list().mListThreads.value_at(static_cast<uint32_t>(index));
            return &entry;
        }
    }
    
    return nullptr;
}

const areg::ComponentEntry * ComponentLoader::find_component_named( const String & roleName ) const noexcept
{
    if ( roleName.is_empty() )
        return nullptr;
    
    for ( uint32_t i = 0; i < mModelList.size(); ++i )
    {
        const areg::Model & model = mModelList[i];
        const areg::ComponentThreadList & threadList= model.thread_list();
        for ( uint32_t j = 0; j < threadList.mListThreads.size(); ++j )
        {
            const areg::ComponentThreadEntry & thread   = threadList.mListThreads.value_at(j);
            int32_t index = thread.find_component_entry(roleName);
            if ( index != areg::INVALID_INDEX )
            {
                const areg::ComponentEntry & entry = thread.mComponents.mListComponents.value_at(static_cast<uint32_t>(index));
                return &entry;
            }
        }
    }

    return nullptr;
}

} // namespace areg
