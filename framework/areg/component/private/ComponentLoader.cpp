/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentLoader.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
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

bool ComponentLoader::loadComponentModel( const String & modelName )
{
    bool result = ComponentLoader::getInstance().loadModel( modelName );
    if ( result == false )
    {
        ComponentLoader::getInstance( ).unloadModel( true, modelName );
    }

    return result;
}

void ComponentLoader::unloadComponentModel(bool waitComplete, const String & modelName )
{
    ComponentLoader::getInstance( ).unloadModel(waitComplete, modelName );
}

void ComponentLoader::waitModelUnload(const String & modelName)
{
    ComponentLoader::getInstance().waitModelThreads(modelName);
}

const NERegistry::Model & ComponentLoader::findModel( const String & modelName )
{
    ComponentLoader & loader = getInstance( );
    Lock lock( loader.mLock );

    const NERegistry::Model * result = loader.findModelByName( modelName );
    return (result != nullptr ? *result : NERegistry::invalidModel( ));
}

const NERegistry::ComponentList& ComponentLoader::findComponentList( const String & threadName )
{

    const NERegistry::ComponentList * result{ nullptr };
    ComponentLoader& loader = getInstance();
    Lock lock(loader.mLock);

    for ( uint32_t i = 0; result == nullptr && i < loader.mModelList.getSize(); ++ i )
    {
        const NERegistry::Model & model = loader.mModelList.getAt(i);
        if ( model.isModelLoaded() )
        {
            const NERegistry::ComponentThreadList & threadList = model.getThreadList();
            for ( uint32_t j = 0; j < threadList.mListThreads.getSize(); ++ j )
            {
                const NERegistry::ComponentThreadEntry & thrEntry = threadList.mListThreads.getAt(j);
                if (thrEntry.mThreadName == threadName)
                {
                    result = &thrEntry.mComponents;
                    break;
                }
            }
        }
    }

    return (result != nullptr ? *result : NERegistry::invalidComponentList());
}

const NERegistry::ComponentEntry& ComponentLoader::findComponentEntry( const String & roleName, const String & threadName )
{
    const NERegistry::ComponentEntry* result = nullptr;
    const NERegistry::ComponentList& comList = findComponentList(threadName);
    if (comList.isValid())
    {
        for (uint32_t i = 0; i < comList.mListComponents.getSize(); ++ i)
        {
            const NERegistry::ComponentEntry& entry = comList.mListComponents[i];
            if (entry.mRoleName == roleName)
            {
                result = &entry;
                break;
            }
        }
    }

    return (result != nullptr ? *result : NERegistry::invalidComponentEntry());
}

const NERegistry::ComponentEntry& ComponentLoader::findComponentEntry( const String & roleName )
{
    const NERegistry::ComponentEntry* result = nullptr;
    ComponentLoader & loader = ComponentLoader::getInstance();
    Lock lock(loader.mLock);

    for (uint32_t i = 0; (result == nullptr) && (i < loader.mModelList.getSize()); ++ i )
    {
        const NERegistry::ComponentThreadList & threadList = loader.mModelList.getAt(i).getThreadList();

        for (uint32_t j = 0; (result == nullptr) && (j < threadList.mListThreads.getSize()); ++ j )
        {
            const NERegistry::ComponentThreadEntry & threadEntry = threadList.mListThreads.getAt(j);
            for (uint32_t k = 0; k < threadEntry.mComponents.mListComponents.getSize(); ++k)
            {
                if (threadEntry.mComponents.mListComponents.getAt(k).mRoleName == roleName)
                {
                    result = &threadEntry.mComponents.mListComponents.getAt(k);
                    break;
                }
            }
        }
    }

    return (result != nullptr ? *result : NERegistry::invalidComponentEntry());
}

const NERegistry::ComponentThreadEntry& ComponentLoader::findThreadEntry(const String& threadName)
{
    const NERegistry::ComponentThreadEntry* result = nullptr;
    ComponentLoader& loader = ComponentLoader::getInstance();
    Lock lock(loader.mLock);

    for (uint32_t i = 0; (result == nullptr) && (i < loader.mModelList.getSize()); ++i)
    {
        const NERegistry::ComponentThreadList& threadList = loader.mModelList.getAt(i).getThreadList();

        for (uint32_t j = 0; j < threadList.mListThreads.getSize(); ++j)
        {
            const NERegistry::ComponentThreadEntry& threadEntry = threadList.mListThreads.getAt(j);
            if (threadEntry.mThreadName == threadName)
            {
                result = &threadEntry;
                break;
            }
        }
    }

    return (result != nullptr ? *result : NERegistry::invalidThreadEntry());
}

bool ComponentLoader::isModelLoaded( const String & modelName )
{
    bool result = false;

    if ( modelName.isEmpty() == false)
    {
        ComponentLoader & loader = ComponentLoader::getInstance();
        Lock lock(loader.mLock);

        for ( uint32_t i = 0; i < loader.mModelList.getSize(); ++ i )
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

bool ComponentLoader::existModel( const String & modelName )
{
    bool result = false;

    if ( NEString::isEmpty<char>( modelName ) == false )
    {
        ComponentLoader & loader = ComponentLoader::getInstance( );
        Lock lock( loader.mLock );

        for ( uint32_t i = 0; i < loader.mModelList.getSize( ); ++ i )
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

bool ComponentLoader::setComponentData( const String & roleName, std::any compData )
{
    bool result = false;
    ComponentLoader & loader = ComponentLoader::getInstance( );
    Lock lock( loader.mLock );

    for ( uint32_t i = 0; i < loader.mModelList.getSize( ); ++ i )
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

void ComponentLoader::removeComponentModel(const String & modelName /*= String::getEmptyString() */)
{
    ComponentLoader::unloadComponentModel(true, modelName);
    ComponentLoader & loader = ComponentLoader::getInstance();
    Lock lock( loader.mLock );
    if ( modelName.isEmpty() == false)
    {
        for ( uint32_t i = 0; i < loader.mModelList.getSize(); ++ i )
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
        loader.mModelList.clear();
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
    mModelList.clear();
    mDefaultModel   = NEString::EmptyStringA.data( );
}

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class, methods
//////////////////////////////////////////////////////////////////////////
bool ComponentLoader::addModel( const NERegistry::Model & newModel )
{
    Lock  lock(mLock);
    bool succeed { (newModel.getModelName().isEmpty() == false) && (newModel.isModelLoaded() == false) };
    // the new model name cannot be empty and it should be unique, and it cannot be marked as loaded.
    ASSERT(succeed);

    // search if model with the same name exists
    for (uint32_t i = 0; succeed && (i < mModelList.getSize()); ++ i )
    {
        const NERegistry::Model & regModel = mModelList.getAt(i);
        if ( newModel.getModelName() != regModel.getModelName() )
        {
            const NERegistry::ComponentThreadList & regThreadList = regModel.getThreadList();
            for ( uint32_t j = 0; succeed && (j < regThreadList.mListThreads.getSize()); ++ j )
            {
                const NERegistry::ComponentThreadEntry & regThreadEntry = regThreadList.mListThreads.getAt(j);
                if ( newModel.findThread(regThreadEntry) < 0 )
                {
                    const NERegistry::ComponentList & regComponentList = regThreadEntry.mComponents;
                    for ( uint32_t k = 0; succeed && (k < regComponentList.mListComponents.getSize()); ++ k )
                    {
                        const NERegistry::ComponentEntry & regComponentEntry = regComponentList.mListComponents.getAt(k);
                        succeed = !newModel.hasRegisteredComponent(regComponentEntry);
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
        if ( mDefaultModel.isEmpty() )
        {
            mDefaultModel = newModel.getModelName();
        }
    }
    
    return succeed;
}

int ComponentLoader::loadAllModels( void )
{
    return loadModel(String::EmptyString);
}

int ComponentLoader::loadModel( const String & modelName )
{
    Lock lock(mLock);
    int result{ 0 };
    for (uint32_t i = 0; i < mModelList.getSize(); ++i)
    {
        NERegistry::Model & model = mModelList[i];
        if (modelName.isEmpty() || (model.getModelName() == modelName))
        {
            result += loadModel(model) ? 1 : 0;
            if (modelName.isEmpty() == false)
                break;  // break the loop
        }
    }

    return result;
}

bool ComponentLoader::loadModel( NERegistry::Model & whichModel ) const
{
    bool result = false;

    if ( whichModel.isValid() && (whichModel.isModelLoaded( ) == false) )
    {
        const NERegistry::ComponentThreadList& thrList = whichModel.getThreadList( );
        whichModel.markModelLoaded( true );
        result = true;
        for ( uint32_t i = 0; result && i < thrList.mListThreads.getSize( ); ++ i )
        {
            Lock lock( mLock );
            const NERegistry::ComponentThreadEntry& entry = thrList.mListThreads[i];
            if ( entry.isValid( ) && Thread::findThreadByName( entry.mThreadName ) == nullptr )
            {
                ComponentThread* thrObject = DEBUG_NEW ComponentThread( entry.mThreadName, entry.mWatchdogTimeout, entry.mStackSizeKB, entry.mMaxQueue );
                if ( thrObject != nullptr )
                {
                    if ( thrObject->createThread( NECommon::WAIT_INFINITE ) == false )
                    {
                        thrObject->shutdownThread( NECommon::DO_NOT_WAIT );
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
                result = entry.isValid( );
            }
        }

        whichModel.markModelAlive( result );
    }
    else
    {
        result = true;
    }

    return result;
}

void ComponentLoader::unloadModel(bool waitComplete, const String & modelName )
{
    for (uint32_t i = 0; i < mModelList.getSize(); ++ i)
    {
        NERegistry::Model & model = mModelList[i];
        if (modelName.isEmpty() || (modelName == model.getModelName()))
        {
            unloadModel(waitComplete, model);
            if (modelName.isEmpty() == false)
                break;  // break the loop
        }
    }
}

void ComponentLoader::unloadModel( bool waitComplete, NERegistry::Model & whichModel ) const
{
    Lock lock(mLock);

    if (whichModel.isModelLoaded() )
    {
        whichModel.markModelAlive( false );

        const NERegistry::ComponentThreadList & modelThreads = whichModel.getThreadList();
        ThreadList threadList( modelThreads.mListThreads.getSize( ) );
        for ( uint32_t i = 0; i < modelThreads.mListThreads.getSize( ); ++ i )
        {
            Thread * compThread = Thread::findThreadByName(modelThreads.mListThreads.getAt( i ).mThreadName);
            if (compThread != nullptr)
            {
                ASSERT(RUNTIME_CAST(compThread, ComponentThread));
                threadList.add(compThread);
            }
        }

        _exitThreads( threadList );

        if (waitComplete)
        {
            lock.unlock();
            _waitThreads(threadList);

            lock.lock();
            _shutdownThreads(threadList);
        }

        whichModel.markModelLoaded(false);
    }
}

void ComponentLoader::waitModelThreads(const String & modelName )
{
    for (uint32_t i = 0; i < mModelList.getSize(); ++ i)
    {
        NERegistry::Model & model = mModelList[i];
        if (modelName.isEmpty() || (modelName == model.getModelName()))
        {
            waitModelThreads(model);
            if (modelName.isEmpty() == false)
                break;  // break the loop
        }
    }
}

void ComponentLoader::waitModelThreads(NERegistry::Model & whichModel)
{
    Lock lock(mLock);

    if (whichModel.isModelLoaded())
    {
        const NERegistry::ComponentThreadList & modelThreads = whichModel.getThreadList();
        ThreadList threadList(modelThreads.mListThreads.getSize());
        for (uint32_t i = 0; i < modelThreads.mListThreads.getSize(); ++ i)
        {
            Thread * compThread = Thread::findThreadByName(modelThreads.mListThreads.getAt(i).mThreadName);
            if (compThread != nullptr)
            {
                ASSERT(RUNTIME_CAST(compThread, ComponentThread) != nullptr);
                threadList.add(compThread);
            }
        }

        lock.unlock();
        _waitThreads(threadList);

        lock.lock();
        _shutdownThreads(threadList);

        whichModel.markModelLoaded(false);
    }
}

void ComponentLoader::_exitThreads( const ThreadList & threadList ) const
{
    for (uint32_t i = 0; i < threadList.getSize(); ++ i )
    {
        Thread* thrObject = threadList[i];
        ASSERT( thrObject != nullptr );
        thrObject->triggerExit( );
    }
}

void ComponentLoader::_waitThreads( const ThreadList & threadList ) const
{
    for ( uint32_t i = 0; i < threadList.getSize(); ++ i )
    {
        Thread * thrObject = threadList[i];
        ASSERT( thrObject != nullptr );
        thrObject->completionWait( NECommon::WAIT_INFINITE );
    }
}

void ComponentLoader::_shutdownThreads( const ThreadList & threadList ) const
{
    for ( uint32_t i = 0; i < threadList.getSize(); ++ i )
    {
        Thread* thrObject = threadList[i];
        ASSERT( thrObject != nullptr );
        thrObject->shutdownThread( NECommon::DO_NOT_WAIT );
        delete thrObject;
    }
}

const NERegistry::Model * ComponentLoader::findModelByName( const String & modelName ) const
{
    const NERegistry::Model * result = nullptr;
    if ( modelName.isEmpty() == false)
    {
        for ( uint32_t i = 0; i < mModelList.getSize( ); ++i )
        {
            const NERegistry::Model & model = mModelList[i];
            if (model.getModelName() == modelName)
            {
                result = &model;
                break;
            }
        }
    }

    return result;
}

const NERegistry::ComponentThreadEntry * ComponentLoader::findThreadEntryByName( const String & threadName ) const
{
    const NERegistry::ComponentThreadEntry * result = nullptr;
    if ( threadName.isEmpty() == false)
    {
        for ( uint32_t i = 0; i < mModelList.getSize(); ++i )
        {
            const NERegistry::Model & model = mModelList[i];
            int index = model.findThread(threadName);
            if ( index != NECommon::INVALID_INDEX )
            {
                const NERegistry::ComponentThreadEntry & entry = model.getThreadList().mListThreads.getAt(static_cast<uint32_t>(index));
                result = &entry;
                break;
            }
        }
    }

    return result;
}

const NERegistry::ComponentEntry * ComponentLoader::findComponentEntryByName( const String & roleName ) const
{
    const NERegistry::ComponentEntry * result = nullptr;
    if ( roleName.isEmpty() == false)
    {
        for ( uint32_t i = 0; (result == nullptr) && (i < mModelList.getSize()); ++i )
        {
            const NERegistry::Model & model = mModelList[i];
            const NERegistry::ComponentThreadList & threadList= model.getThreadList();
            for ( uint32_t j = 0; i < threadList.mListThreads.getSize(); ++i )
            {
                const NERegistry::ComponentThreadEntry & thread   = threadList.mListThreads.getAt(j);
                int index = thread.findComponentEntry(roleName);
                if ( index != NECommon::INVALID_INDEX )
                {
                    const NERegistry::ComponentEntry & entry = thread.mComponents.mListComponents.getAt(static_cast<uint32_t>(index));
                    result = & entry;
                    break;
                }
            }
        }
    }

    return result;
}
