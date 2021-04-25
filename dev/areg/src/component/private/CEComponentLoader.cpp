/************************************************************************
 * \file        areg/src/component/private/CEComponentLoader.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Loader singleton object.
 *
 ************************************************************************/
#include "areg/src/component/CEComponentLoader.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/private/CEServiceManager.hpp"
#include "areg/src/base/NECommon.hpp"

//////////////////////////////////////////////////////////////////////////
// CEModelDataCreator class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEModelDataCreator class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEModelDataCreator::CEModelDataCreator( FuncInitLoaderItem funtCreateModelData, const char * modelName )
{
    ASSERT( funtCreateModelData != NULL );
    NERegistry::CEModel newModel = funtCreateModelData(modelName);
    VERIFY( CEComponentLoader::GetComponentLoader().AddModel( newModel ) );
}

CEModelDataCreator::CEModelDataCreator( const NERegistry::CEModel & newModel )
{
    VERIFY( CEComponentLoader::GetComponentLoader().AddModel( newModel ) );
}

CEModelDataCreator::~CEModelDataCreator( void )
{
    ; // do nothing
}


//////////////////////////////////////////////////////////////////////////
// CEComponentLoader class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEComponentLoader class, static functions
//////////////////////////////////////////////////////////////////////////
CEComponentLoader & CEComponentLoader::GetComponentLoader( void )
{
    static CEComponentLoader _componentLoader;
    return _componentLoader;
}

bool CEComponentLoader::LoadComponentModel( const char * modelName /*= NULL*/ )
{
    bool result = CEComponentLoader::GetComponentLoader().LoadModel( modelName );
    if ( result == false)
        CEComponentLoader::GetComponentLoader().UnloadModel( modelName );
    return result;
}

void CEComponentLoader::UnloadComponentModel( const char * modelName /*= NULL*/ )
{
    CEComponentLoader::GetComponentLoader( ).UnloadModel( modelName );
}

const NERegistry::CEComponentList& CEComponentLoader::GetThreadComponentList( const char* threadName )
{

    const NERegistry::CEComponentList* result  = NULL;
    CEComponentLoader& loader = GetComponentLoader();
    CELock lock(loader.mLock);

    for ( int i = 0; result == NULL && i < loader.mModelList.GetSize(); ++ i )
    {
        const NERegistry::CEModel & model = loader.mModelList.GetAt(i);
        if ( model.IsModelLoaded() )
        {
            const NERegistry::CEComponentThreadList & threadList = model.GetThreadList();
            for ( int j = 0; result == NULL && j < threadList.GetSize(); ++ j )
            {
                const NERegistry::CEComponentThreadEntry & thrEntry = threadList.GetAt(j);
                if (thrEntry.mThreadName == threadName)
                    result  = &thrEntry.mComponents;
            }
        }
    }
    return (result != NULL ? *result : NERegistry::INVALID_COMPONENT_LIST);
}

const NERegistry::CEComponentEntry& CEComponentLoader::SearchComponentEntry( const char* roleName, const char* threadName )
{
    const NERegistry::CEComponentEntry* result = NULL;
    const NERegistry::CEComponentList& comList = GetThreadComponentList(threadName);
    if (comList.IsValid())
    {
        for (int i = 0; result == NULL && i < comList.GetSize(); ++ i)
        {
            const NERegistry::CEComponentEntry& entry = comList[i];
            result = entry.mRoleName == roleName ? &entry : NULL;
        }
    }
    return (result != NULL ? *result : NERegistry::INVALID_COMPONENT_ENTRY);
}

const NERegistry::CEComponentEntry& CEComponentLoader::SearchComponentEntry( const char* roleName )
{
    const NERegistry::CEComponentEntry* result = NULL;
    CEComponentLoader & loader = CEComponentLoader::GetComponentLoader();
    CELock lock(loader.mLock);

    for ( int i = 0; result == NULL && i < loader.mModelList.GetSize(); ++ i )
    {
        const NERegistry::CEComponentThreadList & threadList = loader.mModelList.GetAt(i).GetThreadList();

        for ( int j = 0; result == NULL && j < threadList.GetSize(); ++ j )
        {
            const NERegistry::CEComponentThreadEntry & threadEntry = threadList.GetAt(j);
            for ( int k = 0; result == NULL && k < threadEntry.mComponents.GetSize(); ++ k )
                result = threadEntry.mComponents.GetAt(k).mRoleName == roleName ? &threadEntry.mComponents.GetAt(k) : NULL;
        }
    }
    return (result != NULL ? *result : NERegistry::INVALID_COMPONENT_ENTRY);
}

bool CEComponentLoader::IsModelLoaded( const char * modelName )
{
    bool result = false;

    if ( NEString::isEmpty<char>(modelName) == false )
    {
        CEComponentLoader & loader = CEComponentLoader::GetComponentLoader();
        CELock lock(loader.mLock);

        for ( int i = 0; i < loader.mModelList.GetSize(); ++ i )
        {
            const NERegistry::CEModel & model = loader.mModelList.GetAt(i);
            if ( model.GetModelName() == modelName )
            {
                result = model.IsModelLoaded();
                break;
            }
        }
    }
    return result;
}

bool CEComponentLoader::IsModelExist( const char * modelName )
{
    bool result = false;

    if ( NEString::isEmpty<char>( modelName ) == false )
    {
        CEComponentLoader & loader = CEComponentLoader::GetComponentLoader( );
        CELock lock( loader.mLock );

        for ( int i = 0; i < loader.mModelList.GetSize( ); ++ i )
        {
            const NERegistry::CEModel & model = loader.mModelList.GetAt( i );
            if ( model.GetModelName( ) == modelName )
            {
                result = true;
                break;
            }
        }
    }
    return result;
}

bool CEComponentLoader::SetComponentData( const char * roleName, NEMemory::uAlign compData )
{
    bool result = false;
    CEComponentLoader & loader = CEComponentLoader::GetComponentLoader( );
    CELock lock( loader.mLock );

    for ( int i = 0; i < loader.mModelList.GetSize( ); ++ i )
    {
        NERegistry::CEModel & model = loader.mModelList.GetAt( i );
        if ( model.SetComponentData( roleName, compData ) )
        {
            result = true;
            break;
        }
    }
    return result;
}

void CEComponentLoader::RemoveModel(const char * modelName /*= NULL */)
{
    OUTPUT_WARN("Removing components and model [ %s ]", modelName != NULL ? modelName : "ALL MODELS");
    CEComponentLoader::UnloadComponentModel(modelName);
    CEComponentLoader & loader = CEComponentLoader::GetComponentLoader();
    CELock( loader.mLock );
    if ( NEString::isEmpty<char>(modelName) == false )
    {
        for ( int i = 0; i < loader.mModelList.GetSize(); ++ i )
        {
            NERegistry::CEModel & model = loader.mModelList[i];
            if ( model.GetModelName() == modelName )
            {
                loader.mModelList.RemoveAt(i);
                break;
            }
        }
    }
    else
    {
        loader.mModelList.RemoveAll();
    }
}

bool CEComponentLoader::AddComponentModel( const NERegistry::CEModel & model )
{
    OUTPUT_DBG("Adding new model [ %s ]", model.GetModelName().GetBuffer());
    CEComponentLoader & loader = CEComponentLoader::GetComponentLoader();
    CELock( loader.mLock );
    return loader.AddModel(model);
}

//////////////////////////////////////////////////////////////////////////
// CEComponentLoader class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEComponentLoader::CEComponentLoader( void )
    : mModelList    ( )
    , mDefaultModel ( NEString::EmptyStringA )
    , mLock         ( false )
{
    ; // do nothing
}

CEComponentLoader::~CEComponentLoader( void )
{
    mModelList.RemoveAll();
    mDefaultModel   = NEString::EmptyStringA;
}

//////////////////////////////////////////////////////////////////////////
// CEComponentLoader class, methods
//////////////////////////////////////////////////////////////////////////
bool CEComponentLoader::AddModel( const NERegistry::CEModel & newModel )
{
    CELock  lock(mLock);
    OUTPUT_DBG("Registering model with name [ %s ]", newModel.GetModelName().GetBuffer());
    bool hasError = newModel.GetModelName().IsEmpty() || newModel.IsModelLoaded() ? true : false;
    // the new model name cannot be empty and it should be unique, and it cannot be marked as loaded.
    ASSERT(hasError == false);

    // search if model with the same name exists
    for (int i = 0; hasError == false && i < mModelList.GetSize(); ++ i )
    {
        const NERegistry::CEModel & regModel = mModelList.GetAt(i);
        if ( newModel.GetModelName() != regModel.GetModelName() )
        {
            const NERegistry::CEComponentThreadList & regThreadList = regModel.GetThreadList();
            for ( int j = 0; hasError == false && j < regThreadList.GetSize(); ++ j )
            {
                const NERegistry::CEComponentThreadEntry & regThreadEntry = regThreadList.GetAt(j);
                if ( newModel.SearchThread(regThreadEntry) < 0 )
                {
                    const NERegistry::CEComponentList & regComponentList = regThreadEntry.mComponents;
                    for ( int k = 0; hasError == false && k < regComponentList.GetSize(); ++ k )
                    {
                        const NERegistry::CEComponentEntry & regComponentEntry = regComponentList.GetAt(k);
                        if ( newModel.HasRegisteredComponent(regComponentEntry) )
                        {
                            OUTPUT_ERR("The component with role name [ %s ] is already registered in thread [ %s ] of model [ %s ], cannot add new model!"
                                            , regComponentEntry.mRoleName.GetBuffer()
                                            , regThreadEntry.mThreadName.GetBuffer()
                                            , regModel.GetModelName().GetBuffer() );
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
                                    , regThreadEntry.mThreadName.GetBuffer()
                                    , regModel.GetModelName().GetBuffer());
                    hasError = true;
                    ASSERT(false);
                }
            } // end of for ( int j = 0; hasError == false && j < newThreadList.GetSize(); j ++ )
        }
        else
        {
            OUTPUT_ERR("The model with name [ %s ] is already registered, cannot add new model!"
                            , regModel.GetModelName().GetBuffer());
            hasError = true;
            ASSERT(false);
        }
    }

    if ( hasError == false )
    {
        OUTPUT_DBG("The model [ %s ] is defined correct, adding in the list.", newModel.GetModelName().GetBuffer());
        mModelList.Add(newModel);
        if ( mDefaultModel.IsEmpty() )
        {
            mDefaultModel = newModel.GetModelName().GetBuffer();
        }
    }
    return (hasError == false);
}

bool CEComponentLoader::LoadModel( const char * modelName /*= NULL*/ )
{
    CELock lock(mLock);
    bool result = false;
    
    OUTPUT_DBG("Requested to start load model [ %s ].", NEString::isEmpty<char>(modelName) ? "ALL" : modelName);
    if ( NEString::isEmpty<char>(modelName) )
    {
        result = mModelList.GetSize() > 0;
        for ( int i = 0; result && i < mModelList.GetSize(); ++ i )
        {
            NERegistry::CEModel & model = mModelList[i];
            if ( model.IsModelLoaded() == false )
                result = LoadModel( model );
            ASSERT( model.IsModelLoaded() );
        }
    }
    else
    {
        OUTPUT_DBG("Searching model [ %s ] in the list with size [ %d ]", modelName, mModelList.GetSize() );
        int index = -1;
        for ( int i = 0; (index == -1) && (i < mModelList.GetSize()); ++ i )
        {
            NERegistry::CEModel & model = mModelList[i];
            OUTPUT_DBG("Checking the name, the entry [ %d ] has name [ %s ]", i, model.GetModelName().GetBuffer());
            if ( model.GetModelName() == modelName )
            {
                OUTPUT_DBG("Found model with name [ %s ] at position [ %d ]", modelName, i);
                index = i;
                result = LoadModel(model);
            }
        }
    }

    OUTPUT_DBG("Model [ %s ] loaded with [ %s ].", NEString::isEmpty<char>(modelName) ? "ALL" : modelName, result ? "SUCCESS" : "ERROR");
    
    return result;
}

bool CEComponentLoader::LoadModel( NERegistry::CEModel & whichModel )
{
    bool result = false;

    if ( whichModel.IsValid() && (whichModel.IsModelLoaded( ) == false) )
    {
        const NERegistry::CEComponentThreadList& thrList = whichModel.GetThreadList( );
        OUTPUT_DBG( "Starting to load model [ %s ]. There are [ %d ] component threads to start. Component loader is going to load objects and start Service Manager", static_cast<const char *>(whichModel.GetModelName( ).GetBuffer( )), thrList.GetSize( ) );

        whichModel.SetModelLoaded( true );
        result = true;
        for ( int i = 0; result && i < thrList.GetSize( ); ++ i )
        {
            CELock lock( mLock );
            const NERegistry::CEComponentThreadEntry& entry = thrList[i];
            if ( entry.IsValid( ) && CEThread::FindThreadByName( entry.mThreadName.GetBuffer( ) ) == NULL )
            {
                CEComponentThread* thrObject = DEBUG_NEW CEComponentThread( entry.mThreadName.GetBuffer( ) );
                if ( thrObject != NULL )
                {
                    OUTPUT_DBG( "Starting thread [ %s ] and loading components.", thrObject->GetThreadName( ).GetBuffer( ) );
                    if ( thrObject->CreateThread( CEThread::WAIT_INFINITE ) == false )
                    {
                        OUTPUT_ERR( "Failed to create and start thread [ %s ], going to delete and unload components.", thrObject->GetThreadName( ).GetBuffer( ) );
                        thrObject->DestroyThread( CEThread::DO_NOT_WAIT );
                        delete thrObject;
                        result = false;
                    }
                }
                else
                {
                    OUTPUT_ERR( "Failed instantiate component thread object with name [ %s ]", entry.mThreadName.GetBuffer( ) );
                    result = false;
                }
            }
            else
            {
                result = entry.IsValid( );
                OUTPUT_ERR( "Either Thread [ %s ] is already created or it is invalid: is valid [ %s ], already exists [ %s ]."
                            , entry.mThreadName.GetBuffer( )
                            , entry.IsValid( ) ? "TRUE" : "FALSE"
                            , CEThread::FindThreadByName( entry.mThreadName) != NULL ? "EXISTS" : "DOES NOT EXIT" );
            }
        }
    }
    else
    {
        OUTPUT_WARN( "The model [ %s ] is already loaded. Ignoring loading model", whichModel.GetModelName( ).GetBuffer( ) );
        result = true;
    }

    return result;
}

void CEComponentLoader::UnloadModel( const char * modelName /*= NULL*/ )
{
    CELock lock(mLock);

    OUTPUT_DBG("Requested to unload model [ %s ].", NEString::isEmpty<char>(modelName) ? "ALL" : modelName);
    if ( NEString::isEmpty<char>(modelName) )
    {
        for ( int i = 0; i < mModelList.GetSize(); ++ i )
        {
            NERegistry::CEModel & model = mModelList[i];
            UnloadModel(model);
            ASSERT( model.IsModelLoaded() == false );
        }
    }
    else
    {
        int index = -1;
        for ( int i = 0; index == -1 && i < mModelList.GetSize(); ++ i )
        {
            NERegistry::CEModel & model = mModelList[i];
            if ( model.GetModelName() == modelName )
            {
                index = i;
                UnloadModel(model);
            }
        }
    }
}

void CEComponentLoader::UnloadModel( NERegistry::CEModel & whichModel )
{
    CELock lock(mLock);

    OUTPUT_WARN("Requested to unload components. Going to unload model [ %s ]!", static_cast<const char *>(whichModel.GetModelName().GetBuffer()));
    if (whichModel.IsModelLoaded() )
    {
        const NERegistry::CEComponentThreadList & threadList = whichModel.GetThreadList();
        ShutdownThreads( threadList );
        WaitThreadsCompletion( threadList );
        DestroyThreads( threadList );

        whichModel.SetModelLoaded( false );
    }
    else
    {
        OUTPUT_WARN("The model [ %s ] marked as unloaded. Ignoring request to unload model.", static_cast<const char *>(whichModel.GetModelName().GetBuffer()));
    }

    bool hasLoadedModel = false;
    for (int i = 0; hasLoadedModel == false && i < mModelList.GetSize(); ++ i )
        hasLoadedModel = mModelList.GetAt(i).IsModelLoaded();
}

void CEComponentLoader::ShutdownThreads( const NERegistry::CEComponentThreadList & whichThreads )
{
    OUTPUT_INFO("Starting First Level model shutdown. Shutdown Threads and Components");
    for (int i = 0; i < whichThreads.GetSize(); ++ i )
    {
        const NERegistry::CEComponentThreadEntry& entry = whichThreads[i];
        CEThread* thrObject = CEThread::FindThreadByName(entry.mThreadName.GetBuffer());
        if (thrObject != NULL)
        {
            ASSERT(RUNTIME_CAST(thrObject, CEComponentThread) != NULL);
            OUTPUT_WARN("Shutdown thread [ %s ] and all its components", thrObject->GetThreadName().GetBuffer());
            thrObject->ShutdownThread();
        }
        else
        {
            OUTPUT_WARN("Could not find thread entry [ %s ]. Ignoring stoppint thread.", entry.mThreadName.GetBuffer());
        }
    }
    OUTPUT_INFO("Shuts down Service Manager thread!");
    
}

void CEComponentLoader::WaitThreadsCompletion( const NERegistry::CEComponentThreadList & whichThreads )
{
    OUTPUT_INFO("Starting Second Level model shutdown. Wait for Threads completion!");


    for ( int i = 0; i < whichThreads.GetSize(); ++ i )
    {
        const NERegistry::CEComponentThreadEntry& entry = whichThreads[i];
        CEThread* thrObject = CEThread::FindThreadByName(entry.mThreadName.GetBuffer());
        if (thrObject != NULL)
        {
            ASSERT(RUNTIME_CAST(thrObject, CEComponentThread) != NULL);
            OUTPUT_WARN("Waiting thread [ %s ] completion", thrObject->GetThreadName().GetBuffer());
            thrObject->CompletionWait(CEThread::WAIT_INFINITE);
        }
        else
        {
            OUTPUT_WARN("Could not find thread entry [ %s ]. Ignoring stoppint thread.", entry.mThreadName.GetBuffer());
        }
    }
}

void CEComponentLoader::DestroyThreads( const NERegistry::CEComponentThreadList & whichThreads )
{
    OUTPUT_INFO("Starting Third Level model shutdown. Destroy threads and components!");

    for ( int i = 0; i < whichThreads.GetSize(); ++ i )
    {
        const NERegistry::CEComponentThreadEntry& entry = whichThreads[i];
        CEThread* thrObject = CEThread::FindThreadByName(entry.mThreadName.GetBuffer());
        if (thrObject != NULL)
        {
            ASSERT(RUNTIME_CAST(thrObject, CEComponentThread) != NULL);
            OUTPUT_WARN("Stopping and deleting thread [ %s ] and deleting components", thrObject->GetThreadName().GetBuffer());
            thrObject->DestroyThread(CEThread::WAIT_INFINITE);
            delete thrObject;
        }
        else
        {
            OUTPUT_WARN("Could not find thread entry [ %s ]. Ignoring stoppint thread.", entry.mThreadName.GetBuffer());
        }
    }
}

const NERegistry::CEModel * CEComponentLoader::FindModelByName( const char * modelName ) const
{
    const NERegistry::CEModel * result = NULL;
    if ( NEString::isEmpty<char>(modelName) == false )
    {
        for ( int i = 0; (result == NULL) && (i < mModelList.GetSize( )); ++i )
        {
            const NERegistry::CEModel & model = mModelList[i];
            result = model.GetModelName( ) == modelName ? &model : NULL;
        }
    }
    return result;
}

const NERegistry::CEComponentThreadEntry * CEComponentLoader::FindThreadEntryByName( const char * threadName ) const
{
    const NERegistry::CEComponentThreadEntry * result = NULL;
    if ( NEString::isEmpty<char>(threadName) == false )
    {
        for ( int i = 0; (result == NULL) && (i < mModelList.GetSize()); ++i )
        {
            const NERegistry::CEModel & model = mModelList[i];
            int index = model.SearchThread(threadName);
            if ( index != NECommon::InvalidIndex )
            {
                const NERegistry::CEComponentThreadEntry & entry = model.GetThreadList().GetAt(index);
                result = &entry;
            }
        }
    }
    return result;
}

const NERegistry::CEComponentEntry * CEComponentLoader::FindComponentEntryByName( const char * roleName ) const
{
    const NERegistry::CEComponentEntry * result = NULL;
    if ( NEString::isEmpty<char>(roleName) == false )
    {
        for ( int i = 0; (result == NULL) && (i < mModelList.GetSize()); ++i )
        {
            const NERegistry::CEModel & model          = mModelList[i];
            const NERegistry::CEComponentThreadList & threadList= model.GetThreadList();
            for ( int j = 0; (result == NULL) && (i < threadList.GetSize()); ++i )
            {
                const NERegistry::CEComponentThreadEntry & thread   = threadList.GetAt(j);
                int index = thread.SearchComponentEntry(roleName);
                if ( index != NECommon::InvalidIndex )
                {
                    const NERegistry::CEComponentEntry & entry = thread.mComponents.GetAt(index);
                    result = & entry;
                }
            }
        }
    }
    return result;
}
