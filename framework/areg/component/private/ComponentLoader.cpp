/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentLoader.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Component Loader singleton object.
 *
 ************************************************************************/
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/base/NECommon.hpp"

//////////////////////////////////////////////////////////////////////////
// ModelDataCreator class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ModelDataCreator class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ModelDataCreator::ModelDataCreator( FuncInitLoaderItem funtCreateModelData, const char * modelName )
{
    ASSERT( funtCreateModelData != nullptr );
    NERegistry::Model newModel = funtCreateModelData(modelName);
    VERIFY( ComponentLoader::getInstance().addModel( newModel ) );
}

ModelDataCreator::ModelDataCreator( const NERegistry::Model & newModel )
{
    VERIFY( ComponentLoader::getInstance().addModel( newModel ) );
}


//////////////////////////////////////////////////////////////////////////
// ComponentLoader class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class, static functions
//////////////////////////////////////////////////////////////////////////
ComponentLoader & ComponentLoader::getInstance( void )
{
    static ComponentLoader _componentLoader;
    return _componentLoader;
}

bool ComponentLoader::loadComponentModel( const char * modelName /*= nullptr*/ )
{
    bool result = ComponentLoader::getInstance().loadModel( modelName );
    if ( result == false)
        ComponentLoader::getInstance().unloadModel( modelName );
    return result;
}

void ComponentLoader::unloadComponentModel( const char * modelName /*= nullptr*/ )
{
    ComponentLoader::getInstance( ).unloadModel( modelName );
}

const NERegistry::ComponentList& ComponentLoader::getComponentList( const char* threadName )
{

    const NERegistry::ComponentList* result  = nullptr;
    ComponentLoader& loader = getInstance();
    Lock lock(loader.mLock);

    for ( int i = 0; result == nullptr && i < loader.mModelList.getSize(); ++ i )
    {
        const NERegistry::Model & model = loader.mModelList.getAt(i);
        if ( model.isModelLoaded() )
        {
            const NERegistry::ComponentThreadList & threadList = model.getThreadList();
            for ( int j = 0; result == nullptr && j < threadList.mListThreads.getSize(); ++ j )
            {
                const NERegistry::ComponentThreadEntry & thrEntry = threadList.mListThreads.getAt(j);
                if (thrEntry.mThreadName == threadName)
                    result  = &thrEntry.mComponents;
            }
        }
    }

    return (result != nullptr ? *result : NERegistry::INVALID_COMPONENT_LIST);
}

const NERegistry::ComponentEntry& ComponentLoader::findComponentEntry( const char* roleName, const char* threadName )
{
    const NERegistry::ComponentEntry* result = nullptr;
    const NERegistry::ComponentList& comList = getComponentList(threadName);
    if (comList.isValid())
    {
        for (int i = 0; result == nullptr && i < comList.mListComponents.getSize(); ++ i)
        {
            const NERegistry::ComponentEntry& entry = comList.mListComponents[i];
            result = entry.mRoleName == roleName ? &entry : nullptr;
        }
    }
    return (result != nullptr ? *result : NERegistry::INVALID_COMPONENT_ENTRY);
}

const NERegistry::ComponentEntry& ComponentLoader::findComponentEntry( const char* roleName )
{
    const NERegistry::ComponentEntry* result = nullptr;
    ComponentLoader & loader = ComponentLoader::getInstance();
    Lock lock(loader.mLock);

    for ( int i = 0; result == nullptr && i < loader.mModelList.getSize(); ++ i )
    {
        const NERegistry::ComponentThreadList & threadList = loader.mModelList.getAt(i).getThreadList();

        for ( int j = 0; result == nullptr && j < threadList.mListThreads.getSize(); ++ j )
        {
            const NERegistry::ComponentThreadEntry & threadEntry = threadList.mListThreads.getAt(j);
            for ( int k = 0; result == nullptr && k < threadEntry.mComponents.mListComponents.getSize(); ++ k )
                result = threadEntry.mComponents.mListComponents.getAt(k).mRoleName == roleName ? &threadEntry.mComponents.mListComponents.getAt(k) : nullptr;
        }
    }
    return (result != nullptr ? *result : NERegistry::INVALID_COMPONENT_ENTRY);
}

bool ComponentLoader::isModelLoaded( const char * modelName )
{
    bool result = false;

    if ( NEString::isEmpty<char>(modelName) == false )
    {
        ComponentLoader & loader = ComponentLoader::getInstance();
        Lock lock(loader.mLock);

        for ( int i = 0; i < loader.mModelList.getSize(); ++ i )
        {
            const NERegistry::Model & model = loader.mModelList.getAt(i);
            if ( model.getModelName() == modelName )
            {
                result = model.isModelLoaded();
                break;
            }
        }
    }
    return result;
}

bool ComponentLoader::existModel( const char * modelName )
{
    bool result = false;

    if ( NEString::isEmpty<char>( modelName ) == false )
    {
        ComponentLoader & loader = ComponentLoader::getInstance( );
        Lock lock( loader.mLock );

        for ( int i = 0; i < loader.mModelList.getSize( ); ++ i )
        {
            const NERegistry::Model & model = loader.mModelList.getAt( i );
            if ( model.getModelName( ) == modelName )
            {
                result = true;
                break;
            }
        }
    }
    return result;
}

bool ComponentLoader::setComponentData( const char * roleName, NEMemory::uAlign compData )
{
    bool result = false;
    ComponentLoader & loader = ComponentLoader::getInstance( );
    Lock lock( loader.mLock );

    for ( int i = 0; i < loader.mModelList.getSize( ); ++ i )
    {
        NERegistry::Model & model = loader.mModelList.getAt( i );
        if ( model.setComponentData( roleName, compData ) )
        {
            result = true;
            break;
        }
    }
    return result;
}

bool ComponentLoader::addModelUnique(const NERegistry::Model & newModel)
{
    ComponentLoader & loader = ComponentLoader::getInstance();
    Lock lock( loader.mLock );
    return loader.addModel(newModel);
}

void ComponentLoader::removeComponentModel(const char * modelName /*= nullptr */)
{
    OUTPUT_WARN("Removing components and model [ %s ]", modelName != nullptr ? modelName : "ALL MODELS");
    ComponentLoader::unloadComponentModel(modelName);
    ComponentLoader & loader = ComponentLoader::getInstance();
    Lock lock( loader.mLock );
    if ( NEString::isEmpty<char>(modelName) == false )
    {
        for ( int i = 0; i < loader.mModelList.getSize(); ++ i )
        {
            NERegistry::Model & model = loader.mModelList[i];
            if ( model.getModelName() == modelName )
            {
                loader.mModelList.removeAt(i);
                break;
            }
        }
    }
    else
    {
        loader.mModelList.removeAll();
    }
}

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ComponentLoader::ComponentLoader( void )
    : mModelList    ( )
    , mDefaultModel ( NEString::EmptyStringA.data( ) )
    , mLock         ( )
{
}

ComponentLoader::~ComponentLoader( void )
{
    mModelList.removeAll();
    mDefaultModel   = NEString::EmptyStringA.data( );
}

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class, methods
//////////////////////////////////////////////////////////////////////////
bool ComponentLoader::addModel( const NERegistry::Model & newModel )
{
    Lock  lock(mLock);
    OUTPUT_DBG("Registering model with name [ %s ]", newModel.getModelName().getString());
    bool hasError = newModel.getModelName().isEmpty() || newModel.isModelLoaded() ? true : false;
    // the new model name cannot be empty and it should be unique, and it cannot be marked as loaded.
    ASSERT(hasError == false);

    // search if model with the same name exists
    for (int i = 0; hasError == false && i < mModelList.getSize(); ++ i )
    {
        const NERegistry::Model & regModel = mModelList.getAt(i);
        if ( newModel.getModelName() != regModel.getModelName() )
        {
            const NERegistry::ComponentThreadList & regThreadList = regModel.getThreadList();
            for ( int j = 0; hasError == false && j < regThreadList.mListThreads.getSize(); ++ j )
            {
                const NERegistry::ComponentThreadEntry & regThreadEntry = regThreadList.mListThreads.getAt(j);
                if ( newModel.findThread(regThreadEntry) < 0 )
                {
                    const NERegistry::ComponentList & regComponentList = regThreadEntry.mComponents;
                    for ( int k = 0; hasError == false && k < regComponentList.mListComponents.getSize(); ++ k )
                    {
                        const NERegistry::ComponentEntry & regComponentEntry = regComponentList.mListComponents.getAt(k);
                        if ( newModel.hasRegisteredComponent(regComponentEntry) )
                        {
                            OUTPUT_ERR("The component with role name [ %s ] is already registered in thread [ %s ] of model [ %s ], cannot add new model!"
                                            , regComponentEntry.mRoleName.getString()
                                            , regThreadEntry.mThreadName.getString()
                                            , regModel.getModelName().getString() );
                            hasError = true;
                            ASSERT(false);
                        }
                        else
                        {
                            ; // is OK, continue checking
                        }
                    } // end of for ( int k = 0; hasError == false && k < newComponentList.GetSize(); k ++ )
                }
                else
                {
                    OUTPUT_ERR("The thread with name [ %s ] is already registered in model [ %s ], cannot add new model!"
                                    , regThreadEntry.mThreadName.getString()
                                    , regModel.getModelName().getString());
                    hasError = true;
                    ASSERT(false);
                }
            } // end of for ( int j = 0; hasError == false && j < newThreadList.GetSize(); j ++ )
        }
        else
        {
            OUTPUT_ERR("The model with name [ %s ] is already registered, cannot add new model!", regModel.getModelName().getString());
            hasError = true;
            ASSERT(false);
        }
    }

    if ( hasError == false )
    {
        OUTPUT_DBG("The model [ %s ] is defined correct, adding in the list.", newModel.getModelName().getString());
        mModelList.add(newModel);
        if ( mDefaultModel.isEmpty() )
        {
            mDefaultModel = newModel.getModelName().getString();
        }
    }
    return (hasError == false);
}

bool ComponentLoader::loadModel( const char * modelName /*= nullptr*/ )
{
    Lock lock(mLock);
    bool result = false;
    
    OUTPUT_DBG("Requested to start load model [ %s ].", NEString::isEmpty<char>(modelName) ? "ALL" : modelName);
    if ( NEString::isEmpty<char>(modelName) )
    {
        result = mModelList.getSize() > 0;
        for ( int i = 0; result && i < mModelList.getSize(); ++ i )
        {
            NERegistry::Model & model = mModelList[i];
            if ( model.isModelLoaded() == false )
                result = loadModel( model );

            ASSERT( model.isModelLoaded() );
        }
    }
    else
    {
        OUTPUT_DBG("Searching model [ %s ] in the list with size [ %d ]", modelName, mModelList.getSize() );
        int index = -1;
        for ( int i = 0; (index == -1) && (i < mModelList.getSize()); ++ i )
        {
            NERegistry::Model & model = mModelList[i];
            OUTPUT_DBG("Checking the name, the entry [ %d ] has name [ %s ]", i, model.getModelName().getString());
            if ( model.getModelName() == modelName )
            {
                OUTPUT_DBG("Found model with name [ %s ] at position [ %d ]", modelName, i);
                index = i;
                result = loadModel(model);
            }
        }
    }

    OUTPUT_DBG("Model [ %s ] loaded with [ %s ].", NEString::isEmpty<char>(modelName) ? "ALL" : modelName, result ? "SUCCESS" : "ERROR");
    
    return result;
}

bool ComponentLoader::loadModel( NERegistry::Model & whichModel ) const
{
    bool result = false;

    if ( whichModel.isValid() && (whichModel.isModelLoaded( ) == false) )
    {
        const NERegistry::ComponentThreadList& thrList = whichModel.getThreadList( );
        OUTPUT_DBG( "Starting to load model [ %s ]. There are [ %d ] component threads to start. Component loader is going to load objects and start Service Manager"
                        , whichModel.getModelName( ).getString( )
                        , thrList.getSize( ) );

        whichModel.markModelLoaded( true );
        result = true;
        for ( int i = 0; result && i < thrList.mListThreads.getSize( ); ++ i )
        {
            Lock lock( mLock );
            const NERegistry::ComponentThreadEntry& entry = thrList.mListThreads[i];
            if ( entry.isValid( ) && Thread::findThreadByName( entry.mThreadName.getString( ) ) == nullptr )
            {
                ComponentThread* thrObject = DEBUG_NEW ComponentThread( entry.mThreadName.getString( ) );
                if ( thrObject != nullptr )
                {
                    OUTPUT_DBG( "Starting thread [ %s ] and loading components.", thrObject->getName( ).getString( ) );
                    if ( thrObject->createThread( NECommon::WAIT_INFINITE ) == false )
                    {
                        OUTPUT_ERR( "Failed to create and start thread [ %s ], going to delete and unload components.", thrObject->getName( ).getString( ) );
                        thrObject->destroyThread( NECommon::DO_NOT_WAIT );
                        delete thrObject;
                        result = false;
                    }
                }
                else
                {
                    OUTPUT_ERR( "Failed instantiate component thread object with name [ %s ]", entry.mThreadName.getString( ) );
                    result = false;
                }
            }
            else
            {
                result = entry.isValid( );
                OUTPUT_ERR( "Either Thread [ %s ] is already created or it is invalid: is valid [ %s ], already exists [ %s ]."
                            , entry.mThreadName.getString( )
                            , entry.isValid( ) ? "TRUE" : "FALSE"
                            , Thread::findThreadByName( entry.mThreadName) != nullptr ? "EXISTS" : "DOES NOT EXIST" );
            }
        }
    }
    else
    {
        OUTPUT_WARN( "The model [ %s ] is already loaded. Ignoring loading model", whichModel.getModelName( ).getString( ) );
        result = true;
    }

    return result;
}

void ComponentLoader::unloadModel( const char * modelName /*= nullptr*/ )
{
    Lock lock(mLock);

    OUTPUT_DBG("Requested to unload model [ %s ].", NEString::isEmpty<char>(modelName) ? "ALL" : modelName);
    if ( NEString::isEmpty<char>(modelName) )
    {
        for ( int i = 0; i < mModelList.getSize(); ++ i )
        {
            NERegistry::Model & model = mModelList[i];
            lock.unlock();
            unloadModel(model);
            lock.lock();
            ASSERT( model.isModelLoaded() == false );
        }
    }
    else
    {
        int index = -1;
        for ( int i = 0; index == -1 && i < mModelList.getSize(); ++ i )
        {
            NERegistry::Model & model = mModelList[i];
            if ( model.getModelName() == modelName )
            {
                index = i;
                lock.unlock();
                unloadModel(model);
                lock.lock();
            }
        }
    }
}

void ComponentLoader::unloadModel( NERegistry::Model & whichModel ) const
{
    Lock lock(mLock);

    OUTPUT_WARN("Requested to unload components. Going to unload model [ %s ]!", static_cast<const char *>(whichModel.getModelName().getString()));
    if (whichModel.isModelLoaded() )
    {
        const NERegistry::ComponentThreadList & threadList = whichModel.getThreadList();
        shutdownThreads( threadList );
        lock.unlock();
        waitThreadsCompletion( threadList );
        lock.lock();
        destroyThreads( threadList );

        whichModel.markModelLoaded( false );
    }
    else
    {
        OUTPUT_WARN("The model [ %s ] marked as unloaded. Ignoring request to unload model.", static_cast<const char *>(whichModel.getModelName().getString()));
    }
}

void ComponentLoader::shutdownThreads( const NERegistry::ComponentThreadList & whichThreads ) const
{
    OUTPUT_INFO("Starting First Level model shutdown. Shutdown Threads and Components");
    for (int i = 0; i < whichThreads.mListThreads.getSize(); ++ i )
    {
        const NERegistry::ComponentThreadEntry& entry = whichThreads.mListThreads[i];
        Thread* thrObject = Thread::findThreadByName(entry.mThreadName.getString());
        if (thrObject != nullptr)
        {
            ASSERT(RUNTIME_CAST(thrObject, ComponentThread) != nullptr);
            OUTPUT_WARN("Shutdown thread [ %s ] and all its components", thrObject->getName().getString());
            thrObject->shutdownThread();
        }
        else
        {
            OUTPUT_WARN("Could not find thread entry [ %s ]. Ignoring stopping thread.", entry.mThreadName.getString());
        }
    }
    OUTPUT_INFO("Shuts down Service Manager thread!");
    
}

void ComponentLoader::waitThreadsCompletion( const NERegistry::ComponentThreadList & whichThreads ) const
{
    OUTPUT_INFO("Starting Second Level model shutdown. Wait for Threads completion!");


    for ( int i = 0; i < whichThreads.mListThreads.getSize(); ++ i )
    {
        const NERegistry::ComponentThreadEntry& entry = whichThreads.mListThreads[i];
        Thread* thrObject = Thread::findThreadByName(entry.mThreadName.getString());
        if (thrObject != nullptr)
        {
            ASSERT(RUNTIME_CAST(thrObject, ComponentThread) != nullptr);
            OUTPUT_WARN("Waiting thread [ %s ] completion", thrObject->getName().getString());
            thrObject->completionWait(NECommon::WAIT_INFINITE);
        }
        else
        {
            OUTPUT_WARN("Could not find thread entry [ %s ]. Ignoring stopping thread.", entry.mThreadName.getString());
        }
    }
}

void ComponentLoader::destroyThreads( const NERegistry::ComponentThreadList & whichThreads ) const
{
    OUTPUT_INFO("Starting Third Level model shutdown. Destroy threads and components!");

    for ( int i = 0; i < whichThreads.mListThreads.getSize(); ++ i )
    {
        const NERegistry::ComponentThreadEntry& entry = whichThreads.mListThreads[i];
        Thread* thrObject = Thread::findThreadByName(entry.mThreadName.getString());
        if (thrObject != nullptr)
        {
            ASSERT(RUNTIME_CAST(thrObject, ComponentThread) != nullptr);
            OUTPUT_WARN("Stopping and deleting thread [ %s ] and deleting components", thrObject->getName().getString());
            thrObject->destroyThread(NECommon::WAIT_INFINITE);
            delete thrObject;
        }
        else
        {
            OUTPUT_WARN("Could not find thread entry [ %s ]. Ignoring stopping thread.", entry.mThreadName.getString());
        }
    }
}

const NERegistry::Model * ComponentLoader::findModelByName( const char * modelName ) const
{
    const NERegistry::Model * result = nullptr;
    if ( NEString::isEmpty<char>(modelName) == false )
    {
        for ( int i = 0; (result == nullptr) && (i < mModelList.getSize( )); ++i )
        {
            const NERegistry::Model & model = mModelList[i];
            result = model.getModelName( ) == modelName ? &model : nullptr;
        }
    }
    return result;
}

const NERegistry::ComponentThreadEntry * ComponentLoader::findThreadEntryByName( const char * threadName ) const
{
    const NERegistry::ComponentThreadEntry * result = nullptr;
    if ( NEString::isEmpty<char>(threadName) == false )
    {
        for ( int i = 0; (result == nullptr) && (i < mModelList.getSize()); ++i )
        {
            const NERegistry::Model & model = mModelList[i];
            int index = model.findThread(threadName);
            if ( index != NECommon::INVALID_INDEX )
            {
                const NERegistry::ComponentThreadEntry & entry = model.getThreadList().mListThreads.getAt(index);
                result = &entry;
            }
        }
    }
    return result;
}

const NERegistry::ComponentEntry * ComponentLoader::findComponentEntryByName( const char * roleName ) const
{
    const NERegistry::ComponentEntry * result = nullptr;
    if ( NEString::isEmpty<char>(roleName) == false )
    {
        for ( int i = 0; (result == nullptr) && (i < mModelList.getSize()); ++i )
        {
            const NERegistry::Model & model          = mModelList[i];
            const NERegistry::ComponentThreadList & threadList= model.getThreadList();
            for ( int j = 0; (result == nullptr) && (i < threadList.mListThreads.getSize()); ++i )
            {
                const NERegistry::ComponentThreadEntry & thread   = threadList.mListThreads.getAt(j);
                int index = thread.findComponentEntry(roleName);
                if ( index != NECommon::INVALID_INDEX )
                {
                    const NERegistry::ComponentEntry & entry = thread.mComponents.mListComponents.getAt(index);
                    result = & entry;
                }
            }
        }
    }
    return result;
}
