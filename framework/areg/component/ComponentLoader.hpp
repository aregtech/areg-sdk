#ifndef AREG_COMPONENT_COMPONENTLOADER_HPP
#define AREG_COMPONENT_COMPONENTLOADER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ComponentLoader.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Loader class.
 *              This is singleton configuration object, which should be 
 *              used to model application.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/NERegistry.hpp"
#include "areg/base/SyncObjects.hpp"

/************************************************************************
 * \brief   Predefined MACRO to model threads, components and services.
 ************************************************************************/

#define FUNC_CREATE_COMP(CompType)                                                                          \
                    ([](const NERegistry::ComponentEntry& e, ComponentThread& t) -> Component * {           \
                        return new CompType(e, t);})

#define FUNC_DELETE_COMP                                                                                    \
                    ([](Component& c, const NERegistry::ComponentEntry& /*e*/) -> void {                    \
                                        delete static_cast<Component *>(&c);})

/**
 * \brief   Modeling of application should start by declaration of model
 *          and giving model name. Application can have only one model.
 *          Model starts by calling MACRO BEGIN_MODEL and ends by calling
 *          END_MODEL
 * \param   modelName   The name of model.
 **/
#define BEGIN_MODEL(model_name)                                                                             \
    /*  Declare Load model method and give name of model                            */                      \
    static NERegistry::Model _createdModelData( const char * _model_name_ )                                 \
    {                                                                                                       \
        NERegistry::Model __model(_model_name_);

#define END_MODEL(model_name)                                                                               \
        return __model;                                                                                     \
    }                                                                                                       \
    /*  End of Model, call component loader to pass application model object        */                      \
    static ModelDataCreator _modelData(&_createdModelData, model_name);

//////////////////////////////////////////////////////////////////////////
// declaration of local model, should be manually handled

#define BEGIN_MODEL_LOCAL(model_name)                                                                       \
    /*  Declare local Model object and give name of model. The name should be unique */                     \
        NERegistry::Model __model((model_name));

#define END_MODEL_LOCAL(model_name)                                                                         \
    /*  End of local Model. This will add model to model list of Loader             */                      \
    ModelDataCreator _modelData( __model );

/**
 * \brief   Register thread to start component thread.
 *          This should be called between scopes BEGIN_MODEL and END_MODEL
 *          Every Component Thread registration starts by calling
 *          BEGIN_REGISTER_THREAD_EX and end by calling END_REGISTER_THREAD
 *          Every thread should have unique name within application.
 *          Declare as many component thread, as needed for application.
 *
 * \param   thread_name     The name of component thread, which should be unique.
 * \param   timeout         The watchdog timeout in milliseconds of the worker thread.
 *                          The value 0 (NECommon::WATCHDOG_IGNORE) ignores the watchdog.
 * \param   stackSizeKb     The stack size of the thread in kilobytes. 1 KB = 1024 Bytes.
 *                          The value 0 (NECommon::STACK_SIZE_DEFAULT) ignores to change the stack size,
 *                          and uses system default stack size.
 **/
#define BEGIN_REGISTER_THREAD_EX2(thread_name, timeout, stackSizeKb)                                        \
        {                                                                                                   \
            /*  Begin registering component thread                                  */                      \
            NERegistry::ComponentThreadEntry  thrEntry((thread_name), (timeout), (stackSizeKb));

/**
 * \brief   Register component thread with the watchdog timeout and system default thread stack size.
 *          The watchdog timeout is set if `timeout` is not 0.
 **/
#define BEGIN_REGISTER_THREAD_EX(thread_name, timeout)                                                      \
            BEGIN_REGISTER_THREAD_EX2((thread_name), (timeout), NECommon::STACK_SIZE_DEFAULT);

/**
 * \brief   Register component thread with no watchdog and system default stack size.
 **/
#define BEGIN_REGISTER_THREAD(thread_name)                                                                  \
            BEGIN_REGISTER_THREAD_EX((thread_name), NECommon::WATCHDOG_IGNORE)

/**
 * \brief   Closes component thread registration.
 **/
#define END_REGISTER_THREAD(thread_name)                                                                    \
            /*  End registering component thread, add to model                      */                      \
            __model.addThread(thrEntry);                                                                    \
        }

 /**
 * \brief   Register Component within every Component Thread scope. Extended version
 *          This should be called between BEGIN_REGISTER_THREAD
 *          and END_REGISTER_THREAD scope.
 *          Every component registration starts by calling 
 *          BEGIN_REGISTER_COMPONENT and ends by calling END_REGISTER_COMPONENT.
 *          Declare as many components within component scope as needed 
 *          for application. Every declared component will be instantiated
 *          when component thread starts running.
 *
 * \param   component_name  The name of component. Should be unique in application
 *                          This is same as Role Name of component.
 * \param   funcCreate      Pointer to global (or static) function of type
 *                          NERegistry::FuncCreateComponent,
 *                          to instantiate component object. Called by component thread.
 * \param   funcDelete      Pointer to global (or static) function of type
 *                          NERegistry::FuncDeleteComponent
 *                          to destroy component object. Called by component thread.
 **/
#define BEGIN_REGISTER_COMPONENT_EX(component_name, data, funcCreate, funcDelete)                           \
            {                                                                                               \
                /*  Register component entry                                        */                      \
                NERegistry::ComponentEntry comEntry(   thrEntry.mThreadName                                 \
                                                        , (component_name)                                  \
                                                        , funcCreate                                        \
                                                        , funcDelete );                                     \
                comEntry.setComponentData( data );

/**
 * \brief   Register Component within every Component Thread scope.
 *          This should be called between BEGIN_REGISTER_THREAD
 *          and END_REGISTER_THREAD scope.
 *          Every component registration starts by calling 
 *          BEGIN_REGISTER_COMPONENT and ends by calling END_REGISTER_COMPONENT.
 *          Declare as many components within component scope as needed 
 *          for application. Every declared component will be instantiated
 *          when component thread starts running.
 *
 * \param   component_name  The name of component. Should be unique in application
 *                          This is same as Role Name of component.
 **/
#define BEGIN_REGISTER_COMPONENT(component_name, component_class)                                           \
            BEGIN_REGISTER_COMPONENT_EX(  component_name                                                    \
                                        , NEMemory::InvalidElement                                          \
                                        , FUNC_CREATE_COMP(component_class)                                 \
                                        , FUNC_DELETE_COMP);

#define END_REGISTER_COMPONENT(comp_name)                                                                   \
                /*  Add and register component entry in component thread            */                      \
                thrEntry.addComponent(comEntry);                                                            \
            }

/**
 * \brief   Register implemented service in every component
 *          This should be called between BEGIN_REGISTER_COMPONENT and
 *          END_REGISTER_COMPONENT scope.
 *          Every service registration is done by calling REGISTER_IMPLEMENT_SERVICE.
 *          Declare all services, implemented in component.
 *          This services are playing role of server and are instantiated
 *          when component instantiated.
 *
 * \param   svc_name        The name of implemented service.
 *                          Can be repeated within different component scopes
 *                          in different threads. Cannot be repeat within same
 *                          component scope
 * \param   svc_version     The version number of service in format (major.minor.patch)
 **/
#define REGISTER_IMPLEMENT_SERVICE(svc_name, svc_version)                                                   \
                /*  Register implemented service in component                       */                      \
                comEntry.addSupportedService( NERegistry::ServiceEntry((svc_name), (svc_version)) );

/**
 * \brief   Registers optional worker thread for the component.
 *          If component requires Worker thread, declare it within component registration scope.
 *          The worker thread name should be unique within application.
 *          This should be called between BEGIN_REGISTER_COMPONENT and END_REGISTER_COMPONENT scope.
 *          Every worker thread registration is done by calling REGISTER_WORKER_THREAD.
 *          Declare all worker threads required by component.
 *
 * \param   worker_thread_name  The name of worker thread.
 * \param   consumer_name       The consumer name of worker thread. Differentiate consumer
 *                              names if one component has more than one worker thread.
 * \param   timeout             The watchdog timeout in milliseconds of the worker thread.
 *                              The value 0 (NECommon::WATCHDOG_IGNORE) ignores the watchdog.
 * \param   stackSizeKb         The stack size of the worker thread in kilobytes. 1 KB = 1024 Bytes.
 *                              The value 0 (NECommon::STACK_SIZE_DEFAULT) ignores to change the stack size,
 *                              and uses system default stack size.
 **/
#define REGISTER_WORKER_THREAD_EX2(worker_thread_name, consumer_name, timeout, stackSizeKb)                 \
                /*  Register component worker thread                                */                      \
                comEntry.addWorkerThread(     NERegistry::WorkerThreadEntry(comEntry.mThreadName            \
                                            , (worker_thread_name)                                          \
                                            , comEntry.mRoleName                                            \
                                            , (consumer_name)                                               \
                                            , (timeout)                                                     \
                                            , (stackSizeKb)) );

/**
 * \brief   Register worker thread with watchdog timeout and system default stack size.
 **/
#define REGISTER_WORKER_THREAD_EX(worker_thread_name, consumer_name, timeout)                               \
            REGISTER_WORKER_THREAD_EX2(   (worker_thread_name)                                              \
                                        , (consumer_name)                                                   \
                                        , (timeout)                                                         \
                                        , NECommon::STACK_SIZE_DEFAULT)

/**
 * \brief   Register worker thread with no watchdog and system default stack size.
 **/
#define REGISTER_WORKER_THREAD(worker_thread_name, consumer_name)                                           \
            REGISTER_WORKER_THREAD_EX((worker_thread_name), (consumer_name), NECommon::WATCHDOG_IGNORE)

/**
 * \brief   Declare and register component dependency. Optional.
 *          If registered component has dependency on other
 *          component, declare it within every component scope.
 *          This should be called between BEGIN_REGISTER_COMPONENT and
 *          END_REGISTER_COMPONENT scope.
 *          Every dependency declaration is done by calling REGISTER_DEPENDENCY.
 *          Declare all dependencies of component.
 *          Dependencies are usually declared if registering component also
 *          plays role of client of other component.
 *
 * \param   depend_role_name    The role name of server component
 **/
#define REGISTER_DEPENDENCY(depend_role_name)                                                               \
                /*  Register dependency. Server component                           */                      \
                comEntry.addDependencyService(NERegistry::DependencyEntry(depend_role_name));


/**
 * \example     Modeling
 *
 *              The following example demonstrates how to model component
 * \code    [.cpp]
 *
 *  BEGIN_MODEL("test_model")
 *  
 *      BEGIN_REGISTER_THREAD("test_thread")
 *
 *          BEGIN_REGISTER_COMPONENT_EX("test_component", TestComponent)
 *              REGISTER_IMPLEMENT_SERVICE("test_service_1", Version(1, 0, 0))
 *              REGISTER_DEPENDENCY("secondary_component")
 *          END_REGISTER_COMPONENT("test_component")
 *
 *          BEGIN_REGISTER_COMPONENT_EX("another_component", AnotherComponent)
 *              REGISTER_IMPLEMENT_SERVICE("another_service_1", Version(1, 0, 0))
 *              REGISTER_IMPLEMENT_SERVICE("another_service_2", Version(1, 0, 0))
 *              REGISTER_WORKER_THREAD("another_worker_thread", "consumer_name")
 *              REGISTER_DEPENDENCY("secondary_component")
 *          END_REGISTER_COMPONENT("another_component")
 *
 *      END_REGISTER_THREAD("text_thread")
 *
 *      BEGIN_REGISTER_THREAD("secondary_thread")
 *
 *          BEGIN_REGISTER_COMPONENT_EX("secondary_component", SecondaryCompponent)
 *              REGISTER_IMPLEMENT_SERVICE("secondary_service_1", Version(1, 0, 0))
 *              REGISTER_IMPLEMENT_SERVICE("secondary_service_2", Version(1, 0, 0))
 *          END_REGISTER_COMPONENT("secondary_component")
 *
 *      END_REGISTER_THREAD("secondary_thread")
 *
 *  END_MODEL("test_model")
 *
 * \endcode
 **/

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class ComponentThread;
class Thread;

//////////////////////////////////////////////////////////////////////////
// ModelDataCreator class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Model creator. A class, which is calling model data creator
 *          function to create model and set in Component Loader class.
 *          The object does not have member functions or variables, 
 *          it is created only to load model and plays no additional role.
 **/
class AREG_API ModelDataCreator
{

//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   ModelDataCreator::FuncInitLoaderItem
     *          Type. Function which will start registering model items.
     *          If MACRO is used, this is global function LoadModel().
     **/
    typedef NERegistry::Model (*FuncInitLoaderItem)( const char * /*modelName*/ );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor.
     *          Creates object and loads components, which should be created
     *          by calling specified function and by passing modelName.
     * \param   funtCreateModelData The pointer to function, which creates model
     * \param   modelName           The name of component model to create.
     **/
    ModelDataCreator( ModelDataCreator::FuncInitLoaderItem funtCreateModelData, const char * modelName );

    /**
     * \brief   Initialization constructor.
     *          Adds mode data to model loader. The mode should be already
     *          designed and the unique name should be set.
     * \param   newModel    The new model to add to model loader
     **/
    explicit ModelDataCreator( const NERegistry::Model & newModel );
    
    /**
     * \brief   Destructor.
     **/
    ~ModelDataCreator( ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    ModelDataCreator( ) = delete;
    DECLARE_NOCOPY_NOMOVE( ModelDataCreator );
};

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       By registering application model items, component loader
 *              contains all required information to instantiate all objects.
 *              Every component should have its Load and Unload methods
 *              to instantiate component and start worker thread.
 *              Starting and Stopping worker thread should be done
 *              within component Load and Unload function, which will
 *              receive component registration entry containing all required
 *              information (component role name, worker thread name).
 *              To start instantiate model items, call loadComponentModel()
 *              static method of component loader. Do this on application
 *              startup. To destroy model items ans stop application, call
 *              unloadComponentModel() method of component loader.
 *              Details see bellow.
 **/
class AREG_API ComponentLoader
{
/************************************************************************/
// Friend classes to access protected members
/************************************************************************/
    friend class ModelDataCreator;
    friend class ComponentThread;

//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   ComponentLoader::ModelList
     *          Linked List of Model objects
     **/
    using ModelList     = TEArrayList<NERegistry::Model>;

    /**
     * \brief   ComponentLoader::ThreadList
     *          Array of component threads.
     **/
    using ThreadList    = TEArrayList<Thread *>;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns Component Loader object, which is a singleton object
     *          and instantiated one per process.
     **/
    static ComponentLoader & getInstance( void );

    /**
     * \brief   Call to start instantiating registered objects in the specified model.
     *          If specified modelName is empty, the function will load all Models,
     *          which are not loaded. If name is specified, it will search in Model list
     *          model with specified name and if found, it will load Model.
     *          The function returns true, if Model is either already loaded or 
     *          loading Model completed with success.
     *          The function returns false, if there is no Model with specified name
     *          or failed loading mode.
     * \param   modelName   The name of model to load. If empty, it will load all
     *                      models, which are not loaded yet.
     * \return  Returns true if model is loaded with success.
     **/
    static bool loadComponentModel( const String & modelName );

    /**
     * \brief   Call to shutdown and destroy instantiated objects of mode, and make cleanups.
     *          This method blocks calling thread if 'waitComplete' is set to true.
     *          The caller waits until all jobs are complete and threads exit.
     *          Otherwise, it triggers exit and immediately returns.
     * \param   waitComplete    If true, waits for Component Loader to complete the jobs
     *                          and exit threads. Otherwise, it triggers exit and
     *                          returns.
     * \param   modelName       The name of model to unload. If empty, it will unloaded
     *                          all previously loaded models.
     **/
    static void unloadComponentModel( bool waitComplete, const String & modelName );

    /**
     * \brief   The calling thread is blocked until Component Loader did not
     *          complete the jobs and exit threads. This should be called if previously
     *          it was requested to stop the Watchdog Manager without waiting for completion.
     **/
    static void waitModelUnload(const String & modelName);

    /**
     * \brief   This call unloads components of specified mode and remove model
     *          from model list. If nullptr is passed, all components and models are removed
     * \param   modelName   The name of model to unload and remove. If nullptr, it will unloaded
     *                      all previously loaded models and all models will be removed.
     **/
    static void removeComponentModel( const String & modelName );

    /**
     * \brief   Adds new model to the model list. The name of the new model, names of threads and
     *          services should be unique in the entire system. Otherwise, the model is not added.
     * \param   newModel    The new model to add to the list.
     * \return  Returns true, if succeeded to add new model to the list. 
     *          Returns false, if there is already a model, thread or service with the same name
     *          registered in the system.
     **/
    static bool addModelUnique( const NERegistry::Model & newModel );

    /**
     * \brief   Searches a model by name in the registered model list.
     * \param   modelName   The name of the model to search.
     * \return  Returns instance of the model. If found, the model is valid. Otherwise, it is invalid model.
     *          Check the validity of the model by calling isValid() method.
     **/
    static const NERegistry::Model & findModel( const String & modelName );

    /**
     * \brief   In the model list searches thread entry and returns the list of
     *          registered components entries. If the thread name is valid, it 
     *          returns at least one component in the list. Otherwise, returns
     *          invalid component list.
     * \param   threadName  The valid component thread name.
     * \return  If the thread name is valid, it returns list of registered components.
     *          Otherwise, returns invalid list.
     **/
    static const NERegistry::ComponentList & findComponentList( const String & threadName );

    /**
     * \brief   Returns registered component entry object of
     *          specified thread name and having specified role name
     * \param   roleName    The role name of registered component to lookup
     * \param   threadName  The name of registered thread.
     **/
    static const NERegistry::ComponentEntry & findComponentEntry(const String & roleName, const String & threadName);

    /**
     * \brief   Returns registered component entry object having specified role name.
     *          The component is searched in the complete Model list.
     * \param   roleName    The role name of registered component to lookup
     **/
    static const NERegistry::ComponentEntry & findComponentEntry(const String & roleName);

    /**
     * \brief   Returns registered component thread entry object having specified thread name.
     *          The component thread entry is searched in the complete Model list.
     * \param   threadName  The name of the component thread name to search.
     **/
    static const NERegistry::ComponentThreadEntry & findThreadEntry( const String & threadName );

    /**
     * \brief   Returns true, if Model with specified name is already registered and loaded.
     * \param   modelName   The name of model to check. The name must be unique.
     * \return  Returns true, if Model with specified name is already loaded.
     **/
    static bool isModelLoaded( const String & modelName );

    /**
     * \brief   Returns true, if the model with specified name is already registered.
     * \param   modelName   The unique name of model to search in registered list.
     * \return  Returns true, if the Model with specified name is already exist in the list.
     **/
    static bool existModel( const String & modelName );

    /**
     * \brief   Searches in the list the component by given name. If found, sets component data.
     *          Returns true if found component and the data was successfully set.
     * \param   roleName    The name of component to search in the list.
     * \param   compData    The data to set in component which is passed to create method.
     * \note    You should manually free memory if the data was manually allocated in the memory
     **/
    static bool setComponentData( const String & roleName, std::any compData );

//////////////////////////////////////////////////////////////////////////
// Hidden constructors / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Hidden constructor to avoid duplicate instance, instantiates object in static method.
     **/
    ComponentLoader( void );

    /**
     * \brief   Destructor.
     **/
    ~ComponentLoader( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Loads all models, which are not loaded yet.
     * \return  Returns number of models that was loaded.
     **/
    int loadAllModels( void );

    /**
     * \brief   Loads threads and components of specified model.
     *          It will start registered threads, which will load registered components.
     *          If modelName is empty, it will load all Models.
     *          If modelName is not empty, it will search for Model with
     *          specified name and will start registered threads.
     *          Function returns when all threads are started with success.
     * \param   modelName   The name of Model to load. If empty string, loads all models,
     *                      which are not loaded yet.
     * \return  Returns number of models that was loaded.
     **/
    int loadModel( const String & modelName);

    /**
     * \brief   Loads specified Model. It will start all registered in Model threads,
     *          and start all components. Function returns when all threads are started with success.
     * \param   whichModel  The Model object to load.
     * \return  Returns true if specified mode is loaded with success.
     **/
    bool loadModel( NERegistry::Model & whichModel ) const;
    /**
     * \brief   Unloads Model with specified name, deletes components and stops threads.
     *          If modelName is not empty, it will unload
     *          model with specified name.
     *          If modelName is empty, it will unload all models.
     *          This method blocks calling thread if 'waitComplete' is set to true.
     *          The caller waits until all jobs are complete and threads exit.
     *          Otherwise, it triggers exit and immediately returns.
     * \param   waitComplete    If true, waits for Component Loader to complete the jobs
     *                          and exit threads. Otherwise, it triggers exit and
     *                          returns.
     * \param   modelName       The name of model to unload. If empty string, it will unload all models
     **/
    void unloadModel( bool waitComplete, const String & modelName );

    /**
     * \brief   Unloads specified Model, deletes components and stops threads.
     *          This method blocks calling thread if 'waitComplete' is set to true.
     *          The caller waits until all jobs are complete and threads exit.
     *          Otherwise, it triggers exit and immediately returns.
     * \param   waitComplete    If true, waits for Component Loader to complete the jobs
     *                          and exit threads. Otherwise, it triggers exit and
     *                          returns.
     * \param   whichModel      The Model object, which should be unloaded.
     **/
    void unloadModel( bool waitComplete, NERegistry::Model & whichModel ) const;

    /**
     * \brief   Call to wait the component threads defined in the model to complete the job
     *          and exit. The specified model should already unloaded to wait for completion.
     *          This method blocks calling thread until all jobs are complete and threads exit.
     * \param   modelName       The name of the model to wait. If empty, it waits for completion
     *                          of all model components.
     **/
    void waitModelThreads(const String & modelName);

    /**
     * \brief   Call to wait the component threads defined in the model to complete the job
     *          and exit. The specified model should already unloaded to wait for completion.
     *          This method blocks calling thread until all jobs are complete and threads exit.
     * \param   whichModel      The Model object, which component threads should be completed.
     **/
    void waitModelThreads(NERegistry::Model & whichModel);

    /**
     * \brief   Searches in registries model by name. If found, returns valid pointer. Otherwise, returns null.
     * \param   modelName   The name of model to search. All models should be unique within one process context.
     * \return  If found model, returns valid pointer. Otherwise, returns null.
     **/
    const NERegistry::Model * findModelByName( const String & modelName ) const;

    /**
     * \brief   Searches in registries thread entry by name. The threads should have unique names within process context.
     *          If found, returns valid pointer to thread entry. Otherwise, returns null.
     * \param   threadName  The name of thread to search in system.
     * \return  Returns valid pointer if found thread entry registered with specified name. Otherwise, returns null.
     **/
    const NERegistry::ComponentThreadEntry * findThreadEntryByName( const String & threadName ) const;

    /**
     * \brief   Searches in registries component entry by role name. The role names should have unique within process context.
     *          If found, returns valid pointer to component entry. Otherwise, returns null.
     * \param   roleName    The role name of component to search in system.
     * \return  Returns valid pointer if found component entry registered with specified role name. Otherwise, returns null.
     **/
    const NERegistry::ComponentEntry * findComponentEntryByName( const String & roleName ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Triggers exit events to shut down threads, specified in Thread List.
     *          This automatically deletes all registered components
     *          and stops registered worker threads.
     * \param   threadList  The list of registered threads with unique names, 
     *                          which found in the system.
     **/
    void _exitThreads( const ThreadList & threadList ) const;

    /**
     * \brief   The call of this function will blocks current thread and
     *          the function returns when all threads in the list complete jobs and exit.
     *          The call of this function does not close the thread objects.
     * \param   threadList  The list of registered threads with unique names,
     *                          which found in the system.
     **/
    void _waitThreads( const ThreadList & threadList ) const;

    /**
     * \brief   The call of this function closes all threads in the list.
     * \param   threadList  The list of registered threads with unique names,
     *                          which found in the system.
     **/
    void _shutdownThreads( const ThreadList & threadList ) const;

    /**
     * \brief   Adds new Model object to the list.  All models should have
     *          unique names, all registered threads in the model should
     *          have unique names and all registered components in threads
     *          should have unique role names. Before adding new Model,
     *          the function will check names. If all registered names
     *          are valid, the new Model will be added and the function
     *          will return true. Otherwise, function returns false.
     * \param   newModel    The new Model to add to the list.
     *                      The Model should have unique name, all registered
     *                      threads should have unique names and all components
     *                      should have unique role names. Otherwise, new Model
     *                      will not be added to the model.
     * \return  Returns true, if new Model was added to the Model List.
     *          Otherwise, it returns false.
     **/
    bool addModel( const NERegistry::Model & newModel );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   The list of models
     **/
    ModelList       mModelList;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
        
    /**
     * \brief   The name of default model
     **/
    String        mDefaultModel;

    /**
     * \brief   Synchronization object
     **/
    mutable ResourceLock    mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ComponentLoader );
};

#endif  // AREG_COMPONENT_COMPONENTLOADER_HPP
