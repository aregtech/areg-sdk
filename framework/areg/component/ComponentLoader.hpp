#ifndef AREG_COMPONENT_COMPONENTLOADER_HPP
#define AREG_COMPONENT_COMPONENTLOADER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/ComponentLoader.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Component Loader class.
 *              This is singleton configuration object, which should be 
 *              used to model application.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/NERegistry.hpp"
#include "areg/base/SynchObjects.hpp"

/************************************************************************
 * \brief   Predefined MACRO to model threads, components and services.
 ************************************************************************/

/**
 * \brief   Modeling of application should start by declaration of model
 *          and giving model name. Application can have only one model.
 *          Model starts by calling MACRO BEGIN_MODEL and ends by calling
 *          END_MODEL
 * \param   modelName   The name of model.
 **/
#define BEGIN_MODEL(model_name)                                                                                 \
    /*  Declare Load model method and give name of model                            */                          \
    static NERegistry::Model _createdModelData( const char * _modelName )                                       \
    {                                                                                                           \
        NERegistry::Model __model(_modelName);

#define END_MODEL(model_name)                                                                                   \
        return __model;                                                                                         \
    }                                                                                                           \
    /*  End of Model, call component loader to pass application model object        */                          \
    static ModelDataCreator _modelData(&_createdModelData, model_name);

//////////////////////////////////////////////////////////////////////////
// declaration of local model, should be manually handled

#define BEGIN_MODEL_LOCAL(model_name)                                                                           \
    /*  Declare local Model object and give name of model. The name should be unique */                         \
        NERegistry::Model __model((model_name));

#define END_MODEL_LOCAL(model_name)                                                                             \
    /*  End of local Model. This will add model to model list of Loader             */                          \
    ModelDataCreator _modelData( __model );

/**
 * \brief   Register thread to start component thread.
 *          This should be called between scopes BEGIN_MODEL and END_MODEL
 *          Every Component Thread registration starts by calling
 *          BEGIN_REGISTER_THREAD and end by calling END_REGISTER_THREAD
 *          Every thread should have unique name within application.
 *          Declare as many component thread, as needed for application.
 *
 * \param   thread_name The name of component thread, which should be unique.
 * \param   timeout             The watchdog timeout in milliseconds of the worker thread.
 *                              The value 0 (NECommon::INVALID_TIMEOUT) ignores the watchdog
 **/
#define BEGIN_REGISTER_THREAD(thread_name, timeout)                                                             \
        {                                                                                                       \
            /*  Begin registering component thread                                  */                          \
            NERegistry::ComponentThreadEntry  thrEntry((thread_name), (timeout));

#define END_REGISTER_THREAD(thread_name)                                                                        \
            /*  End registering component thread, add to model                      */                          \
            __model.addThread(thrEntry);                                                                        \
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
 *                          NERegistry::FuncCreateComponent(const NERegistry::ComponentEntry & , ComponentThread & ),
 *                          to instantiate component object. Called by component thread.
 * \param   funcDelete      Pointer to global (or static) function of type
 *                          NERegistry::FuncDeleteComponent(Component&, const NERegistry::ComponentEntry &)
 *                          to destroy component object. Called by component thread.
 **/
#define BEGIN_REGISTER_COMPONENT_EX(component_name, funcCreate, funcDelete, data)                               \
            {                                                                                                   \
                /*  Register component entry                                        */                          \
                NERegistry::ComponentEntry comEntry(   thrEntry.mThreadName.getString()                         \
                                                        , (component_name)                                      \
                                                        , &(funcCreate)                                         \
                                                        , &(funcDelete) );                                      \
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
#define BEGIN_REGISTER_COMPONENT(component_name, component_class)                                               \
            BEGIN_REGISTER_COMPONENT_EX(  component_name                                                        \
                                        , component_class::CreateComponent                                      \
                                        , component_class::DeleteComponent                                      \
                                        , NEMemory::InvalidElement );

#define END_REGISTER_COMPONENT(comp_name)                                                                       \
                /*  Add and register component entry in component thread            */                          \
                thrEntry.addComponent(comEntry);                                                                \
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
 * \param   service_name    The name of implemented service.
 *                          Can be repeated within different component scopes
 *                          in different threads. Cannot be repeat within same
 *                          component scope
 * \param   major           The major version number of implemented service interface
 * \param   minor           The minor version number of implemented service interface
 * \param   patch           The patch version number of implemented service interface
 **/
#define REGISTER_IMPLEMENT_SERVICE(service_name, implemented_version)                                           \
                /*  Register implemented service in component                       */                          \
                comEntry.addSupportedService( NERegistry::ServiceEntry((service_name), (implemented_version)) );

/**
 * \brief   Register worker thread if needed by component. Optional.
 *          If component requires Worker thread, declare it within
 *          component registration scope.
 *          The worker thread name should be unique within application.
 *          This should be called between BEGIN_REGISTER_COMPONENT and
 *          END_REGISTER_COMPONENT scope.
 *          Every worker thread registration is done by calling REGISTER_WORKER_THREAD.
 *          Declare all worker threads required by component.
 *
 * \param   worker_thread_name  The name of worker thread.
 * \param   consumer_name       The consumer name of worker thread. Differentiate consumer
 *                              names if one component has more than one worker thread.
 * \param   timeout             The watchdog timeout in milliseconds of the worker thread.
 *                              The value 0 (NECommon::INVALID_TIMEOUT) ignores the watchdog
 **/
#define REGISTER_WORKER_THREAD(worker_thread_name, consumer_name, timeout)                                      \
                /*  Register component worker thread                                */                          \
                comEntry.addWorkerThread(     NERegistry::WorkerThreadEntry(comEntry.mThreadName.getString()    \
                                            , (worker_thread_name)                                              \
                                            , comEntry.mRoleName.getString()                                    \
                                            , (consumer_name)                                                   \
                                            , (timeout))  );

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
#define REGISTER_DEPENDENCY(depend_role_name)                                                                   \
                /*  Register dependency. Server component                           */                          \
                comEntry.addDependencyService(NERegistry::DependencyEntry(depend_role_name));


/**
 * \example     Modeling
 *
 *              The following example demonstrates how to model component
 * \code    [.cpp]
 *
 *  Component* TestCompLoad( const NERegistry::ComponentEntry & entry, ComponentThread & componentThread )
 *  {
 *      return new TestComponent(componentThread, entry.mRoleName.getString());
 *  }
 *
 *  void TestCompUnload( Component & comItem )
 *  {
 *      delete &comItem;
 *  }
 *
 *  Component * AnotherCompLoad( const NERegistry::ComponentEntry & entry, ComponentThread & componentThread )
 *  {
 *      return new AnotherComponent(componentThread, entry.mRoleName.getString());
 *  }
 *
 *  void AnotherCompUnload(Component& comItem)
 *  {
 *      delete &comItem;
 *  }
 *
 *  Component* SecondaryCompLoad( const NERegistry::ComponentEntry & entry, ComponentThread & componentThread )
 *  {
 *      return new SecondaryComponent(componentThread, entry.mRoleName.getString() );
 *  }
 *
 *  void SecondaryCompUnload(Component& comItem)
 *  {
 *      // stop worker threads here, make cleaning
 *      delete &comItem;
 *  }
 *
 *
 *  BEGIN_MODEL("test_model")
 *  
 *      BEGIN_REGISTER_THREAD("test_thread", 0)
 *
 *          BEGIN_REGISTER_COMPONENT_EX("test_component", TestCompLoad, TestCompUnload)
 *              REGISTER_IMPLEMENT_SERVICE("test_service_1", Version(1, 0, 0))
 *              REGISTER_DEPENDENCY("secondary_component")
 *          END_REGISTER_COMPONENT("test_component")
 *
 *          BEGIN_REGISTER_COMPONENT_EX("another_component", AnotherCompLoad, AnotherCompUnload)
 *              REGISTER_IMPLEMENT_SERVICE("another_service_1", Version(1, 0, 0))
 *              REGISTER_IMPLEMENT_SERVICE("another_service_2", Version(1, 0, 0))
 *              REGISTER_WORKER_THREAD("another_worker_thread", "consumer_name", , NECommon::INVALID_TIMEOUT)
 *              REGISTER_DEPENDENCY("secondary_component")
 *          END_REGISTER_COMPONENT("another_component")
 *
 *      END_REGISTER_THREAD("text_thread")
 *
 *      BEGIN_REGISTER_THREAD("secondary_thread", NECommon::INVALID_TIMEOUT)
 *
 *          BEGIN_REGISTER_COMPONENT_EX("secondary_component", SecondaryCompLoad, SecondaryCompUnload)
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
     *          If specified modelName is nullptr, the function will load all Models,
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
    static bool loadComponentModel( const String & modelName = String::EmptyString );

    /**
     * \brief   Call to shutdown and destroy instantiated objects of mode, and make cleanups.
     * \param   modelName   The name of model to unload. If nullptr, it will unloaded
     *                      all previously loaded models.
     **/
    static void unloadComponentModel( const String & modelName = String::EmptyString );

    /**
     * \brief   This call unloads components of specified mode and remove model
     *          from model list. If nullptr is passed, all components and models are removed
     * \param   modelName   The name of model to unload and remove. If nullptr, it will unloaded
     *                      all previously loaded models and all models will be removed.
     **/
    static void removeComponentModel( const String & modelName = String::EmptyString );

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
     * \brief   In the model list searches thread entry and returns the list of
     *          registered components entries. If the thread name is valid, it 
     *          returns at least one component in the list. Otherwise, returns
     *          NERegistry::ComponentList::INVALID_COMPONENT_LIST list.
     * \param   threadName  The valid component thread name.
     * \return  If the thread name is valid, it returns list of registered components.
     *          Otherwise, returns NERegistry::ComponentList::INVALID_COMPONENT_LIST list.
     **/
    static const NERegistry::ComponentList & getComponentList( const String & threadName );

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
     * \note    The system automatically does not free manually allocated space. This means that 
     *          if as a component data a pointer to manually allocated object is passed,
     *          it should be as well manually freed.
     **/
    static bool setComponentData( const String & roleName, NEMemory::uAlign compData );

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
     * \brief   Loads threads and components of specified model.
     *          It will start registered threads, which will load registered components.
     *          If modelName is nullptr or empty, it will load all Models.
     *          If modelName is not nullptr and not empty, it will search for Model with
     *          specified name and will start registered threads.
     *          Function returns when all threads are started with success.
     * \param   modelName   The name of Model to load. If nullptr or empty string,
     *                      it loads all models, which are not loaded yet.
     * \return  Returns true if components are loaded with success.
     **/
    bool loadModel( const String & modelName = String::EmptyString );

    /**
     * \brief   Loads specified Model. It will start all registered in Model threads,
     *          and start all components. Function returns when all threads are started with success.
     * \param   whichModel  The Model object to load.
     * \return  Returns true if specified mode is loaded with success.
     **/
    bool loadModel( NERegistry::Model & whichModel ) const;
    /**
     * \brief   Unloads Model with specified name, deletes components and stops threads.
     *          If modelName is not nullptr and not empty, it will unload
     *          model with specified name.
     *          If modelName is nullptr or empty, it will unload all models.
     * \param   modelName   The name of model to unload. If nullptr, it will unload all models
     **/
    void unloadModel( const String & modelName = String::EmptyString );

    /**
     * \brief   Unloads specified Model, deletes components and stops threads.
     * \param   whichModel  The Model object, which should be unloaded.
     **/
    void unloadModel( NERegistry::Model & whichModel ) const;

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
     * \brief   Shuts down threads, specified in Thread List of Registry.
     *          This will automatically delete all registered components
     *          and stop registered worker threads.
     * \param   whichThreads    The list of registered threads. 
     *                          Every element contains unique name of thread, 
     *                          which can be found in the system.
     **/
    void shutdownThreads( const NERegistry::ComponentThreadList & whichThreads ) const;

    /**
     * \brief   The call of this function will suspend current thread and
     *          the function will return when all threads in the registered list
     *          completed and exit. The call of this function does not stop or
     *          destroy threads
     * \param   whichThreads    The list of registered thread to wait for completion.
     **/
    void waitThreadsCompletion( const NERegistry::ComponentThreadList & whichThreads ) const;

    /**
     * \brief   The call of this function will destroy all registered in the list
     *          threads and wait until all threads complete job and exit.
     **/
    void destroyThreads( const NERegistry::ComponentThreadList & whichThreads ) const;

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
