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
#include "areg/base/areg_global.h"
#include "areg/component/Model.hpp"
#include "areg/base/SyncPrimitives.hpp"

/************************************************************************
 * \brief   Predefined MACRO to model threads, components and services.
 ************************************************************************/

#define FUNC_CREATE_COMP(CompType)                                                                          \
                    ([](const areg::ComponentEntry& e, areg::ComponentThread& t) -> areg::Component * {     \
                        return new CompType(e, t);})

#define FUNC_DELETE_COMP                                                                                    \
                    ([](areg::Component& c, const areg::ComponentEntry& /*e*/) -> void {                    \
                                        delete static_cast<areg::Component *>(&c);})

/**
 * \brief   Modeling of application should start by declaration of model
 *          and giving model name. Application can have only one model.
 *          Model starts by calling MACRO BEGIN_MODEL and ends by calling
 *          END_MODEL
 * \param   modelName   The name of model.
 **/
#define BEGIN_MODEL(model_name)                                                                             \
    /*  Declare Load model method and give name of model                            */                      \
    static areg::Model _createdModelData( const char * _model_name_ )                                       \
    {                                                                                                       \
        areg::Model areg_model_(_model_name_);

#define END_MODEL(model_name)                                                                               \
        return areg_model_;                                                                                 \
    }                                                                                                       \
    /*  End of Model, call component loader to pass application model object        */                      \
    static areg::ModelDataCreator _modelData(&_createdModelData, model_name);

//////////////////////////////////////////////////////////////////////////
// declaration of local model, should be manually handled

#define BEGIN_MODEL_LOCAL(model_name)                                                                       \
    /*  Declare local Model object and give name of model. The name should be unique */                     \
        areg::Model areg_model_((model_name));

#define END_MODEL_LOCAL(model_name)                                                                         \
    /*  End of local Model. This will add model to model list of Loader             */                      \
    areg::ModelDataCreator _modelData( areg_model_ );

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
 *                          The value 0 (areg::WATCHDOG_IGNORE) ignores the watchdog.
 * \param   stackSizeKb     The stack size of the thread in kilobytes. 1 KB = 1024 Bytes.
 *                          The value 0 (areg::DEFAULT_STACK_SIZE) ignores to change the stack size,
 *                          and uses system default stack size.
 **/
#define BEGIN_REGISTER_THREAD_EX2(thread_name, timeout, stackSizeKb)                                        \
        {                                                                                                   \
            /*  Begin registering component thread                                  */                      \
            areg::ComponentThreadEntry  thrEntry((thread_name), (timeout), (stackSizeKb));

/**
 * \brief   Register component thread with the watchdog timeout and system default thread stack size.
 *          The watchdog timeout is set if `timeout` is not 0.
 **/
#define BEGIN_REGISTER_THREAD_EX(thread_name, timeout)                                                      \
            BEGIN_REGISTER_THREAD_EX2((thread_name), (timeout), areg::DEFAULT_STACK_SIZE);

/**
 * \brief   Register component thread with no watchdog and system default stack size.
 **/
#define BEGIN_REGISTER_THREAD(thread_name)                                                                  \
            BEGIN_REGISTER_THREAD_EX((thread_name), areg::WATCHDOG_IGNORE)

/**
 * \brief   Closes component thread registration.
 **/
#define END_REGISTER_THREAD(thread_name)                                                                    \
            /*  End registering component thread, add to model                      */                      \
            areg_model_.add_thread(thrEntry);                                                               \
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
 *                          This is same as Role name of component.
 * \param   funcCreate      Pointer to global (or static) function of type
 *                          areg::FuncCreateComponent,
 *                          to instantiate component object. Called by component thread.
 * \param   funcDelete      Pointer to global (or static) function of type
 *                          areg::FuncDeleteComponent
 *                          to destroy component object. Called by component thread.
 **/
#define BEGIN_REGISTER_COMPONENT_EX(component_name, data, funcCreate, funcDelete)                           \
            {                                                                                               \
                /*  Register component entry                                        */                      \
                areg::ComponentEntry comEntry(   thrEntry.mThreadName                                       \
                                                        , (component_name)                                  \
                                                        , funcCreate                                        \
                                                        , funcDelete );                                     \
                comEntry.set_data( data );

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
 *                          This is same as Role name of component.
 **/
#define BEGIN_REGISTER_COMPONENT(component_name, component_class)                                           \
            {                                                                                               \
                /*  Register component entry                                        */                      \
                areg::ComponentEntry comEntry(   thrEntry.mThreadName                                       \
                                                        , (component_name)                                  \
                                                        , FUNC_CREATE_COMP(component_class)                 \
                                                        , FUNC_DELETE_COMP );

#define END_REGISTER_COMPONENT(comp_name)                                                                   \
                /*  Add and register component entry in component thread            */                      \
                thrEntry.add_component(comEntry);                                                           \
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
                comEntry.add_supported_service( areg::ServiceEntry((svc_name), (svc_version)) );

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
 *                              The value 0 (areg::WATCHDOG_IGNORE) ignores the watchdog.
 * \param   stackSizeKb         The stack size of the worker thread in kilobytes. 1 KB = 1024 Bytes.
 *                              The value 0 (areg::DEFAULT_STACK_SIZE) ignores to change the stack size,
 *                              and uses system default stack size.
 **/
#define REGISTER_WORKER_THREAD_EX2(worker_thread_name, consumer_name, timeout, stackSizeKb)                 \
                /*  Register component worker thread                                */                      \
                comEntry.add_worker_thread(   areg::WorkerThreadEntry(comEntry.mThreadName                  \
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
                                        , areg::DEFAULT_STACK_SIZE)

/**
 * \brief   Register worker thread with no watchdog and system default stack size.
 **/
#define REGISTER_WORKER_THREAD(worker_thread_name, consumer_name)                                           \
            REGISTER_WORKER_THREAD_EX((worker_thread_name), (consumer_name), areg::WATCHDOG_IGNORE)

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
                comEntry.add_dependency_service(areg::DependencyEntry(depend_role_name));


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
namespace areg {
    class Component;
    class ComponentThread;
    class Thread;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ModelDataCreator class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Helper class that loads a component model and registers it with the component loader.
 *          Not persistent after model loading.
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
    typedef areg::Model (*FuncInitLoaderItem)( const char * /*modelName*/ );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Loads a component model by calling the specified creation function with the given
     *          model name.
     *
     * \param   funtCreateModelData     Function pointer to create the model data.
     * \param   modelName               The unique name of the component model to create.
     **/
    ModelDataCreator( ModelDataCreator::FuncInitLoaderItem funtCreateModelData, const char * modelName );

    /**
     * \brief   Registers a pre-designed model with the component loader. The model must have a
     *          unique name already assigned.
     *
     * \param   newModel    The model to register.
     **/
    explicit ModelDataCreator( const areg::Model & newModel );
    
    ~ModelDataCreator( ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    ModelDataCreator( ) = delete;
    AREG_NOCOPY_NOMOVE( ModelDataCreator );
};

//////////////////////////////////////////////////////////////////////////
// ComponentLoader class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Singleton responsible for registering, loading, and unloading component models.
 *          Instantiates components, starts worker threads, and manages the complete lifecycle of
 *          model items based on registered configuration.
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
    using ModelList     = ArrayList<areg::Model>;

    /**
     * \brief   ComponentLoader::ThreadList
     *          Array of component threads.
     **/
    using ThreadList    = ArrayList<Thread *>;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the singleton ComponentLoader instance.
     **/
    static ComponentLoader & instance() noexcept;

    /**
     * \brief   Starts instantiating registered objects in the specified model.
     *
     * \param   modelName       The name of the model to load; if empty, loads all unloaded models.
     * \return  Returns true if the model is loaded or was already loaded; false if not found or
     *          loading failed.
     **/
    static bool load_component_model( const String & modelName );

    /**
     * \brief   Shuts down and destroys instantiated objects in the specified model.
     *
     * \param   waitComplete    If true, waits for Component Loader to complete jobs and exit
     *                          threads. Otherwise, triggers exit and returns immediately.
     * \param   modelName       The name of the model to unload; if empty, unloads all loaded
     *                          models.
     **/
    static void unload_component_model( bool waitComplete, const String & modelName );

    /**
     * \brief   Blocks the calling thread until Component Loader completes jobs and exits.
     **/
    static void wait_model_unload(const String & modelName);

    /**
     * \brief   Unloads components and removes the model from the registry.
     *
     * \param   modelName       The name of the model to remove; if nullptr, removes all models.
     **/
    static void remove_component_model( const String & modelName );

    /**
     * \brief   Adds a new model with globally unique names for the model, threads, and services.
     *
     * \param   newModel    The model to add; all names must be globally unique.
     * \return  Returns true if successfully added; false if a duplicate name exists.
     **/
    static bool add_model_unique( const areg::Model & newModel );

    /**
     * \brief   Searches for a model by name in the registered list.
     *
     * \param   modelName       The name of the model to search.
     * \return  Returns the model if found (check validity with is_valid()); otherwise returns an
     *          invalid model.
     **/
    static const areg::Model & find_model( const String & modelName );

    /**
     * \brief   Returns the list of component entries for a registered thread.
     *
     * \param   threadName      The name of the component thread.
     * \return  Returns the component list if the thread exists; otherwise returns an invalid list.
     **/
    static const areg::ComponentList & find_component_list( const String & threadName );

    /**
     * \brief   Returns a component entry with the specified role name in the given thread.
     *
     * \param   roleName        The role name of the component to find.
     * \param   threadName      The name of the thread containing the component.
     **/
    static const areg::ComponentEntry & find_component_entry(const String & roleName, const String & threadName);

    /**
     * \brief   Returns a component entry with the specified role name from the complete model list.
     *
     * \param   roleName    The role name of the component to find.
     **/
    static const areg::ComponentEntry & find_component_entry(const String & roleName);

    /**
     * \brief   Returns a component thread entry with the specified thread name.
     *
     * \param   threadName      The name of the thread to find.
     **/
    static const areg::ComponentThreadEntry & find_thread_entry( const String & threadName );

    /**
     * \brief   Returns true if a model with the specified name is loaded.
     *
     * \param   modelName       The unique name of the model to check.
     * \return  Returns true if the model is loaded; false otherwise.
     **/
    [[nodiscard]]
    static bool is_model_loaded( const String & modelName );

    /**
     * \brief   Returns true if a model with the specified name is registered.
     *
     * \param   modelName       The unique name of the model to check.
     * \return  Returns true if the model exists in the registry; false otherwise.
     **/
    static bool exist( const String & modelName );

    /**
     * \brief   Sets component data for a component by role name.
     *
     * \param   roleName    The role name of the component.
     * \param   compData    The data to associate with the component.
     * \note    Manually free memory if the data was manually allocated.
     **/
    static bool set_component_data( const String & roleName, std::any compData );

    /**
     * \brief   Resets component data for a component by role name.
     *
     * \param   roleName    The role name of the component.
     * \return  Returns true if the component was found and data was reset; false otherwise.
     **/
    static bool reset(const String& roleName);

//////////////////////////////////////////////////////////////////////////
// Hidden constructors / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    ComponentLoader();

    ~ComponentLoader();

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Loads all registered but unloaded models.
     *
     * \return  Returns the number of models loaded.
     **/
    int32_t load_all_models();

    /**
     * \brief   Loads the specified model and starts its threads and components.
     *
     * \param   modelName       The name of the model to load; if empty, loads all unloaded models.
     * \return  Returns the number of models loaded.
     **/
    int32_t load_model( const String & modelName);

    /**
     * \brief   Loads the specified model and starts its threads and components.
     *
     * \param   whichModel      The model object to load.
     * \return  Returns true if the model loaded successfully; false otherwise.
     **/
    bool load_model( areg::Model & whichModel ) const;
    /**
     * \brief   Unloads the specified model, deletes components, and stops threads.
     *
     * \param   waitComplete    If true, waits for Component Loader to complete jobs and exit
     *                          threads. Otherwise, triggers exit and returns immediately.
     * \param   modelName       The name of the model to unload; if empty, unloads all loaded
     *                          models.
     **/
    void unload_model( bool waitComplete, const String & modelName );

    /**
     * \brief   Unloads the specified model, deletes components, and stops threads.
     *
     * \param   waitComplete    If true, waits for Component Loader to complete jobs and exit
     *                          threads. Otherwise, triggers exit and returns immediately.
     * \param   whichModel      The model object to unload.
     **/
    void unload_model( bool waitComplete, areg::Model & whichModel ) const;

    /**
     * \brief   Blocks until component threads of the specified model complete and exit.
     *
     * \param   modelName       The name of the model; if empty, waits for all model threads.
     **/
    void wait_threads(const String & modelName);

    /**
     * \brief   Blocks until component threads of the specified model complete and exit.
     *
     * \param   whichModel      The model object whose threads should complete.
     **/
    void wait_threads(areg::Model & whichModel);

    /**
     * \brief   Searches for a model by name and returns a pointer if found.
     *
     * \param   modelName       The name of the model to search; must be globally unique.
     * \return  Returns a valid pointer if found; nullptr otherwise.
     **/
    [[nodiscard]]
    const areg::Model * find_model_named( const String & modelName ) const noexcept;

    /**
     * \brief   Searches for a thread entry by name and returns a pointer if found.
     *
     * \param   threadName      The name of the thread to search.
     * \return  Returns a valid pointer if found; nullptr otherwise.
     **/
    [[nodiscard]]
    const areg::ComponentThreadEntry * find_thread_named( const String & threadName ) const noexcept;

    /**
     * \brief   Searches for a component entry by role name and returns a pointer if found.
     *
     * \param   roleName    The role name of the component to search.
     * \return  Returns a valid pointer if found; nullptr otherwise.
     **/
    [[nodiscard]]
    const areg::ComponentEntry * find_component_named( const String & roleName ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Triggers exit events for threads, stopping components and worker threads.
     *
     * \param   threadList      The list of threads to stop.
     **/
    void _exit_threads( const ThreadList & threadList ) const noexcept;

    /**
     * \brief   Blocks until all threads in the list complete and exit.
     *
     * \param   threadList      The list of threads to wait for.
     **/
    void _wait_threads( const ThreadList & threadList ) const;

    /**
     * \brief   Closes all threads in the list.
     *
     * \param   threadList      The list of threads to close.
     **/
    void _shutdown_threads( const ThreadList & threadList ) const;

    /**
     * \brief   Adds a new model with globally unique names for the model, threads, and components.
     *
     * \param   newModel    The model to add; all names must be globally unique.
     * \return  Returns true if successfully added; false if a duplicate name exists.
     **/
    bool add_model( const areg::Model & newModel );

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
    AREG_NOCOPY_NOMOVE( ComponentLoader );
};

} // namespace areg
#endif  // AREG_COMPONENT_COMPONENTLOADER_HPP
