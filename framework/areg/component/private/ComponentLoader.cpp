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
    VERIFY( ComponentLoader::getInstance().addModel( newModel ) );
}

ModelDataCreator::ModelDataCreator( const areg::Model & newModel )
{
    VERIFY( ComponentLoader::getInstance().addModel( newModel ) );
}


//////////////////////////////////////////////////////////////////////////
// ComponentLoader class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class, static functions
//////////////////////////////////////////////////////////////////////////
ComponentLoader & ComponentLoader::getInstance()
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

const areg::Model & ComponentLoader::findModel( const String & modelName )
{
    ComponentLoader & loader = getInstance( );
    Lock lock( loader.mLock );

    const areg::Model * result = loader.findModelByName( modelName );
    return (result != nullptr ? *result : areg::invalidModel( ));
}

const areg::ComponentList& ComponentLoader::findComponentList( const String & threadName )
{

    const areg::ComponentList * result{ nullptr };
    ComponentLoader& loader = getInstance();
    Lock lock(loader.mLock);

    for ( uint32_t i = 0; result == nullptr && i < loader.mModelList.getSize(); ++ i )
    {
        const areg::Model & model = loader.mModelList.getAt(i);
        if ( model.isModelLoaded() )
        {
            const areg::ComponentThreadList & threadList = model.getThreadList();
            for ( uint32_t j = 0; j < threadList.mListThreads.getSize(); ++ j )
            {
                const areg::ComponentThreadEntry & thrEntry = threadList.mListThreads.getAt(j);
                if (thrEntry.mThreadName == threadName)
                {
                    result = &thrEntry.mComponents;
                    break;
                }
            }
        }
    }

    return (result != nullptr ? *result : areg::invalidComponentList());
}

const areg::ComponentEntry& ComponentLoader::findComponentEntry( const String & roleName, const String & threadName )
{
    const areg::ComponentEntry* result = nullptr;
    const areg::ComponentList& comList = findComponentList(threadName);
    if (comList.isValid())
    {
        for (uint32_t i = 0; i < comList.mListComponents.getSize(); ++ i)
        {
            const areg::ComponentEntry& entry = comList.mListComponents[i];
            if (entry.mRoleName == roleName)
            {
                result = &entry;
                break;
            }
        }
    }

    return (result != nullptr ? *result : areg::invalidComponentEntry());
}

const areg::ComponentEntry& ComponentLoader::findComponentEntry( const String & roleName )
{
    const areg::ComponentEntry* result = nullptr;
    ComponentLoader & loader = ComponentLoader::getInstance();
    Lock lock(loader.mLock);

    for (uint32_t i = 0; (result == nullptr) && (i < loader.mModelList.getSize()); ++ i )
    {
        const areg::ComponentThreadList & threadList = loader.mModelList.getAt(i).getThreadList();

        for (uint32_t j = 0; (result == nullptr) && (j < threadList.mListThreads.getSize()); ++ j )
        {
            const areg::ComponentThreadEntry & threadEntry = threadList.mListThreads.getAt(j);
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

    return (result != nullptr ? *result : areg::invalidComponentEntry());
}

const areg::ComponentThreadEntry& ComponentLoader::findThreadEntry(const String& threadName)
{
    const areg::ComponentThreadEntry* result = nullptr;
    ComponentLoader& loader = ComponentLoader::getInstance();
    Lock lock(loader.mLock);

    for (uint32_t i = 0; (result == nullptr) && (i < loader.mModelList.getSize()); ++i)
    {
        const areg::ComponentThreadList& threadList = loader.mModelList.getAt(i).getThreadList();

        for (uint32_t j = 0; j < threadList.mListThreads.getSize(); ++j)
        {
            const areg::ComponentThreadEntry& threadEntry = threadList.mListThreads.getAt(j);
            if (threadEntry.mThreadName == threadName)
            {
                result = &threadEntry;
                break;
            }
        }
    }

    return (result != nullptr ? *result : areg::invalidThreadEntry());
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
            const areg::Model & model = loader.mModelList.getAt(i);
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

    if ( areg::isEmpty<char>( modelName ) == false )
    {
        ComponentLoader & loader = ComponentLoader::getInstance( );
        Lock lock( loader.mLock );

        for ( uint32_t i = 0; i < loader.mModelList.getSize( ); ++ i )
        {
            const areg::Model & model = loader.mModelList.getAt( i );
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
        areg::Model & model = loader.mModelList.getAt( i );
        if ( model.setComponentData( roleName, compData ) )
        {
            result = true;
            break;
        }
    }

    return result;
}

bool ComponentLoader::resetComponentData(const String& roleName)
{
    bool result = false;
    ComponentLoader& loader = ComponentLoader::getInstance();
    Lock lock(loader.mLock);

    for (uint32_t i = 0; i < loader.mModelList.getSize(); ++i)
    {
        areg::Model& model = loader.mModelList.getAt(i);
        if (model.resetComponentData(roleName))
        {
            result = true;
            break;
        }
    }

    return result;
}

bool ComponentLoader::addModelUnique(const areg::Model & newModel)
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
            areg::Model & model = loader.mModelList[i];
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
bool ComponentLoader::addModel( const areg::Model & newModel )
{
    Lock  lock(mLock);
    bool succeed { (newModel.getModelName().isEmpty() == false) && (newModel.isModelLoaded() == false) };
    // the new model name cannot be empty and it should be unique, and it cannot be marked as loaded.
    ASSERT(succeed);

    // search if model with the same name exists
    for (uint32_t i = 0; succeed && (i < mModelList.getSize()); ++ i )
    {
        const areg::Model & regModel = mModelList.getAt(i);
        if ( newModel.getModelName() != regModel.getModelName() )
        {
            const areg::ComponentThreadList & regThreadList = regModel.getThreadList();
            for ( uint32_t j = 0; succeed && (j < regThreadList.mListThreads.getSize()); ++ j )
            {
                const areg::ComponentThreadEntry & regThreadEntry = regThreadList.mListThreads.getAt(j);
                if ( newModel.findThread(regThreadEntry) < 0 )
                {
                    const areg::ComponentList & regComponentList = regThreadEntry.mComponents;
                    for ( uint32_t k = 0; succeed && (k < regComponentList.mListComponents.getSize()); ++ k )
                    {
                        const areg::ComponentEntry & regComponentEntry = regComponentList.mListComponents.getAt(k);
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

int32_t ComponentLoader::loadAllModels()
{
    return loadModel(String::EmptyString);
}

int32_t ComponentLoader::loadModel( const String & modelName )
{
    Lock lock(mLock);
    int32_t result{ 0 };
    for (uint32_t i = 0; i < mModelList.getSize(); ++i)
    {
        areg::Model & model = mModelList[i];
        if (modelName.isEmpty() || (model.getModelName() == modelName))
        {
            result += loadModel(model) ? 1 : 0;
            if (modelName.isEmpty() == false)
                break;  // break the loop
        }
    }

    return result;
}

bool ComponentLoader::loadModel( areg::Model & whichModel ) const
{
    bool result = false;

    if ( whichModel.isValid() && (whichModel.isModelLoaded( ) == false) )
    {
        const areg::ComponentThreadList& thrList = whichModel.getThreadList( );
        whichModel.markModelLoaded( true );
        result = true;
        for ( uint32_t i = 0; result && i < thrList.mListThreads.getSize( ); ++ i )
        {
            Lock lock( mLock );
            const areg::ComponentThreadEntry& entry = thrList.mListThreads[i];
            if ( entry.isValid( ) && Thread::findThreadByName( entry.mThreadName ) == nullptr )
            {
                ComponentThread* thrObject = DEBUG_NEW ComponentThread( entry.mThreadName, entry.mWatchdogTimeout, entry.mStackSizeKB, entry.mMaxQueue );
                if ( thrObject != nullptr )
                {
                    if ( thrObject->createThread( areg::WAIT_INFINITE ) == false )
                    {
                        thrObject->shutdownThread( areg::DO_NOT_WAIT );
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
        areg::Model & model = mModelList[i];
        if (modelName.isEmpty() || (modelName == model.getModelName()))
        {
            unloadModel(waitComplete, model);
            if (modelName.isEmpty() == false)
                break;  // break the loop
        }
    }
}

void ComponentLoader::unloadModel( bool waitComplete, areg::Model & whichModel ) const
{
    Lock lock(mLock);

    if (whichModel.isModelLoaded() )
    {
        whichModel.markModelAlive( false );

        const areg::ComponentThreadList & modelThreads = whichModel.getThreadList();
        ThreadList threadList( modelThreads.mListThreads.getSize( ) );
        for ( uint32_t i = 0; i < modelThreads.mListThreads.getSize( ); ++ i )
        {
            Thread * compThread = Thread::findThreadByName(modelThreads.mListThreads.getAt( i ).mThreadName);
            if (compThread != nullptr)
            {
                ASSERT(AREG_RUNTIME_CAST(compThread, ComponentThread));
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
        areg::Model & model = mModelList[i];
        if (modelName.isEmpty() || (modelName == model.getModelName()))
        {
            waitModelThreads(model);
            if (modelName.isEmpty() == false)
                break;  // break the loop
        }
    }
}

void ComponentLoader::waitModelThreads(areg::Model & whichModel)
{
    Lock lock(mLock);

    if (whichModel.isModelLoaded())
    {
        const areg::ComponentThreadList & modelThreads = whichModel.getThreadList();
        ThreadList threadList(modelThreads.mListThreads.getSize());
        for (uint32_t i = 0; i < modelThreads.mListThreads.getSize(); ++ i)
        {
            Thread * compThread = Thread::findThreadByName(modelThreads.mListThreads.getAt(i).mThreadName);
            if (compThread != nullptr)
            {
                ASSERT(AREG_RUNTIME_CAST(compThread, ComponentThread) != nullptr);
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
        thrObject->completionWait( areg::WAIT_INFINITE );
    }
}

void ComponentLoader::_shutdownThreads( const ThreadList & threadList ) const
{
    for ( uint32_t i = 0; i < threadList.getSize(); ++ i )
    {
        Thread* thrObject = threadList[i];
        ASSERT( thrObject != nullptr );
        thrObject->shutdownThread( areg::DO_NOT_WAIT );
        delete thrObject;
    }
}

const areg::Model * ComponentLoader::findModelByName( const String & modelName ) const
{
    const areg::Model * result = nullptr;
    if ( modelName.isEmpty() == false)
    {
        for ( uint32_t i = 0; i < mModelList.getSize( ); ++i )
        {
            const areg::Model & model = mModelList[i];
            if (model.getModelName() == modelName)
            {
                result = &model;
                break;
            }
        }
    }

    return result;
}

const areg::ComponentThreadEntry * ComponentLoader::findThreadEntryByName( const String & threadName ) const
{
    const areg::ComponentThreadEntry * result = nullptr;
    if ( threadName.isEmpty() == false)
    {
        for ( uint32_t i = 0; i < mModelList.getSize(); ++i )
        {
            const areg::Model & model = mModelList[i];
            int32_t index = model.findThread(threadName);
            if ( index != areg::INVALID_INDEX )
            {
                const areg::ComponentThreadEntry & entry = model.getThreadList().mListThreads.getAt(static_cast<uint32_t>(index));
                result = &entry;
                break;
            }
        }
    }

    return result;
}

const areg::ComponentEntry * ComponentLoader::findComponentEntryByName( const String & roleName ) const
{
    const areg::ComponentEntry * result = nullptr;
    if ( roleName.isEmpty() == false)
    {
        for ( uint32_t i = 0; (result == nullptr) && (i < mModelList.getSize()); ++i )
        {
            const areg::Model & model = mModelList[i];
            const areg::ComponentThreadList & threadList= model.getThreadList();
            for ( uint32_t j = 0; i < threadList.mListThreads.getSize(); ++i )
            {
                const areg::ComponentThreadEntry & thread   = threadList.mListThreads.getAt(j);
                int32_t index = thread.findComponentEntry(roleName);
                if ( index != areg::INVALID_INDEX )
                {
                    const areg::ComponentEntry & entry = thread.mComponents.mListComponents.getAt(static_cast<uint32_t>(index));
                    result = & entry;
                    break;
                }
            }
        }
    }

    return result;
}
