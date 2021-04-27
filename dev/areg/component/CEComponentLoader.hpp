#ifndef AREG_COMPONENT_CECOMPONENTLOADER_HPP
#define AREG_COMPONENT_CECOMPONENTLOADER_HPP
/************************************************************************
 * \file        areg/component/CEComponentLoader.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
#include "areg/base/ESynchObjects.hpp"

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
    static NERegistry::CEModel _createdModelData( const char * _modelName )                                     \
    {                                                                                                           \
        NERegistry::CEModel __model(_modelName);

#define END_MODEL(model_name)                                                                                   \
        return __model;                                                                                         \
    }                                                                                                           \
    /*  End of Model, call component loader to pass application model object        */                          \
    static CEModelDataCreator _modelData(&_createdModelData, model_name);

//////////////////////////////////////////////////////////////////////////
// declaration of local model, should be manually handled

#define BEGIN_MODEL_LOCAL(model_name)                                                                           \
    /*  Declare local Model object and give name of model. The name should be unique */                         \
        NERegistry::CEModel __model((model_name));

#define END_MODEL_LOCAL(model_name)                                                                             \
    /*  End of local Model. This will add model to model list of Loader             */                          \
    CEModelDataCreator _modelData( __model );

/**
 * \brief   Register thread to start component thread.
 *          This should be called between scopes BEGIN_MODEL and END_MODEL
 *          Every Component Thread registration starts by calling
 *          BEGIN_REGISTER_THREAD and end by calling END_REGISTER_THREAD
 *          Every thread should have unique name within application.
 *          Declare as many component thread, as needed for application.
 *
 * \param   thread_name The name of component thread, which should be unique.
 **/
#define BEGIN_REGISTER_THREAD(thread_name)                                                                      \
        {                                                                                                       \
            /*  Begin registering component thread                                  */                          \
            NERegistry::CEComponentThreadEntry  thrEntry((thread_name));

#define END_REGISTER_THREAD(thread_name)                                                                        \
            /*  End registering component thread, add to model                      */                          \
            __model.AddThread(thrEntry);                                                                        \
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
 *                          NERegistry::FuncCreateComponent(const NERegistry::CEComponentEntry & , CEComponentThread & ),
 *                          to instantiate component object. Called by component thread.
 * \param   funcDelete      Pointer to global (or static) function of type
 *                          NERegistry::FuncDeleteComponent(CEComponent&, const NERegistry::CEComponentEntry &)
 *                          to destroy component object. Called by component thread.
 **/
#define BEGIN_REGISTER_COMPONENT_EX(component_name, funcCreate, funcDelete, data)                               \
            {                                                                                                   \
                /*  Register component entry                                        */                          \
                NERegistry::CEComponentEntry comEntry(   thrEntry.mThreadName.GetBuffer()                       \
                                                        , (component_name)                                      \
                                                        , &(funcCreate)                                         \
                                                        , &(funcDelete) );                                      \
                comEntry.SetComponentData( data );

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
                thrEntry.AddComponent(comEntry);                                                                \
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
 * \param   major           Major version number of implemented service interface
 * \param   minor           Minor version number of implemented service interface
 * \param   patch           Patch version number of implemented service interface
 **/
#define REGISTER_IMPLEMENT_SERVICE(service_name, major, minor, patch)                                           \
                /*  Register implemented service in component                       */                          \
                comEntry.AddSupportedService(     NERegistry::CEServiceEntry((service_name)                     \
                                                , (major)                                                       \
                                                , (minor)                                                       \
                                                , (patch))  );

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
 **/
#define REGISTER_WORKER_THREAD(worker_thread_name, consumer_name)                                               \
                /*  Register component worker thread                                */                          \
                comEntry.AddWorkerThread(     NERegistry::CEWorkerThreadEntry(comEntry.mThreadName.GetBuffer()  \
                                            , (worker_thread_name)                                              \
                                            , comEntry.mRoleName.GetBuffer()                                    \
                                            , (consumer_name))  );

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
                comEntry.AddDependencyService(NERegistry::CEDependencyEntry(depend_role_name));


/**
 * \example     Modeling
 *
 *              The following example demonstrates how to model component
 * \code    [.cpp]
 *
 *  CEComponent* TestCompLoad( const NERegistry::CEComponentEntry & entry, CEComponentThread & componentThread )
 *  {
 *      return new CETestComponent(componentThread, entry.mRoleName.GetBuffer());
 *  }
 *
 *  void TestCompUnload( CEComponent & comItem )
 *  {
 *      delete &comItem;
 *  }
 *
 *  CEComponent * AnotherCompLoad( const NERegistry::CEComponentEntry & entry, CEComponentThread & componentThread )
 *  {
 *      return new CEAnotherComponent(componentThread, entry.mRoleName.GetBuffer());
 *  }
 *
 *  void AnotherCompUnload(CEComponent& comItem)
 *  {
 *      delete &comItem;
 *  }
 *
 *  CEComponent* SecondaryCompLoad( const NERegistry::CEComponentEntry & entry, CEComponentThread & componentThread )
 *  {
 *      return new CESecondaryComponent(componentThread, entry.mRoleName.GetBuffer() );
 *  }
 *
 *  void SecondaryCompUnload(CEComponent& comItem)
 *  {
 *      // stop worker threads here, make cleaning
 *      delete &comItem;
 *  }
 *
 *
 *  BEGIN_MODEL("test_model")
 *  
 *      BEGIN_REGISTER_THREAD("test_thread")
 *
 *          BEGIN_REGISTER_COMPONENT_EX("test_component", TestCompLoad, TestCompUnload)
 *              REGISTER_IMPLEMENT_SERVICE("test_service_1", 1, 0, 0)
 *              REGISTER_DEPENDENCY("secondary_component")
 *          END_REGISTER_COMPONENT("test_component")
 *
 *          BEGIN_REGISTER_COMPONENT_EX("another_component", AnotherCompLoad, AnotherCompUnload)
 *              REGISTER_IMPLEMENT_SERVICE("another_service_1", 1, 0, 0)
 *              REGISTER_IMPLEMENT_SERVICE("another_service_2", 1, 0, 0)
 *              REGISTER_WORKER_THREAD("another_worker_thread", "consumer_name")
 *              REGISTER_DEPENDENCY("secondary_component")
 *          END_REGISTER_COMPONENT("another_component")
 *
 *      END_REGISTER_THREAD("text_thread")
 *
 *      BEGIN_REGISTER_THREAD("secondary_thread")
 *
 *          BEGIN_REGISTER_COMPONENT_EX("secondary_component", SecondaryCompLoad, SecondaryCompUnload)
 *              REGISTER_IMPLEMENT_SERVICE("secondary_service_1", 1, 0, 0)
 *              REGISTER_IMPLEMENT_SERVICE("secondary_service_2", 1, 0, 0)
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
class CEComponent;
class CEComponentThread;

//////////////////////////////////////////////////////////////////////////
// CEModelDataCreator class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Model creator. A class, which is calling model data creator
 *          function to create model and set in Component Loader class.
 *          The object does not have member functions or variables, 
 *          it is created only to load model and plays no additional role.
 **/
class AREG_API CEModelDataCreator
{

//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   CEModelDataCreator::FuncInitLoaderItem
     *          Type. Function which will start registering model items.
     *          If MACRO is used, this is global function LoadModel().
     **/
    typedef NERegistry::CEModel (*FuncInitLoaderItem)( const char * /*modelName*/ );

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
    CEModelDataCreator( CEModelDataCreator::FuncInitLoaderItem funtCreateModelData, const char * modelName );

    /**
     * \brief   Initialization constructor.
     *          Adds mode data to model loader. The mode should be already
     *          designed and the unique name should be set.
     * \param   newModel    The new model to add to model loader
     **/
    CEModelDataCreator( const NERegistry::CEModel & newModel );
    /**
     * \brief   Destructor.
     **/
    ~CEModelDataCreator( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    CEModelDataCreator( void );
    CEModelDataCreator( const CEModelDataCreator & );
    const CEModelDataCreator & operator = ( const CEModelDataCreator & );
};

//////////////////////////////////////////////////////////////////////////
// CEComponentLoader class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Component Loader is a singleton class, instantiated when
 *              application starts. Use mentioned above Marco to design
 *              model. This will register model items to start when
 *              load model is called.
 * 
 * \details     By registering application model items, component loader
 *              contains all required information to instantiate all objects.
 *              Every component should have its Load and Unload methods
 *              to instantiate component and start worker thread.
 *              Starting and Stopping worker thread should be done
 *              within component Load and Unload function, which will
 *              receive component registration entry containing all required
 *              information (component role name, worker thread name).
 *              To start instantiate model items, call LoadComponentModel()
 *              static method of component loader. Do this on application
 *              startup. To destroy model items ans stop application, call
 *              UnloadComponentModel() method of component loader.
 *              Details see bellow.
 *
 **/
class AREG_API CEComponentLoader
{
/************************************************************************/
// Friend classes to access protected members
/************************************************************************/
    friend class CEModelDataCreator;
    friend class CEComponentThread;

//////////////////////////////////////////////////////////////////////////
// Local types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   CEComponentLoader::ModelList
     *          Linked List of Model objects
     **/
    typedef TEArrayList<NERegistry::CEModel, const NERegistry::CEModel &> ModelList;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns Component Loader object, which is a singleton object
     *          and instantiated one per process.
     **/
    static CEComponentLoader & GetComponentLoader( void );

    /**
     * \brief   Call to start instantiating registered objects in the specified model.
     *          If specified modelName is NULL, the function will load all Models,
     *          which are not loaded. If name is specified, it will search in Model list
     *          model with specified name and if found, it will load Model.
     *          The function returns true, if Model is either already loaded or 
     *          loading Model completed with success.
     *          The function returns false, if there is no Model with specified name
     *          or failed loading mode.
     * \param   modelName   The name of model to load. If NULL, it will load all
     *                      models, which are not loaded yet.
     * \return  Returns true if model is loaded with success.
     **/
    static bool LoadComponentModel( const char * modelName = NULL );

    /**
     * \brief   Call to shutdown and destroy instantiated objects of mode, and make cleanups.
     * \param   modelName   The name of model to unload. If NULL, it will unloaded
     *                      all previously loaded models.
     **/
    static void UnloadComponentModel( const char * modelName = NULL );

    /**
     * \brief   Adds new component model in the list if there is no model with same name or the existing model is not loaded yet.
     *          The operation ignored, if model with the same name exists and it is loaded.
     * \param   model   New model to add in the list.
     * \return  Returns true if model was successfully added.
     **/
    static bool AddComponentModel( const NERegistry::CEModel & model );

    /**
     * \brief   This call unloads components of specified mode and remove model
     *          from model list. If NULL is passed, all components and models are removed
     * \param   modelName   The name of model to unload and remove. If NULL, it will unloaded
     *                      all previously loaded models and all models will be removed.
     **/
    static void RemoveModel( const char * modelName = NULL );

    /**
     * \brief   In the model list searches thread entry and returns the list of
     *          registered components entries. If the thread name is valid
     *          component list, it will return non-empty list, because component
     *          thread should contain at least one component.
     *          If thread name is not valid component thread, this will return
     *          NERegistry::CEComponentList::INVALID_COMPONENT_LIST list.
     * \param   threadName  The valid component thread name
     * \return  If the thread name is valid component thread name, it will return
     *          list of registered component entries. Otherwise it will return
     *          NERegistry::CEComponentList::INVALID_COMPONENT_LIST list.
     **/
    static const NERegistry::CEComponentList & GetThreadComponentList( const char * threadName );

    /**
     * \brief	Returns registered component entry object of
     *          specified thread name and having specified role name
     * \param	roleName	The role name of registered component to lookup
     * \param	threadName	The name of registered thread.
     **/
    static const NERegistry::CEComponentEntry & SearchComponentEntry(const char * roleName, const char* threadName);

    /**
     * \brief	Returns registered component entry object having specified role name.
     *          The component is searched in the complete Model list.
     * \param	roleName	The role name of registered component to lookup
     **/
    static const NERegistry::CEComponentEntry & SearchComponentEntry(const char * roleName);

    /**
     * \brief   Returns true, if Model with specified name is already registered and loaded.
     * \param   modelName   The name of model to check. The name must be unique.
     * \return  Returns true, if Model with specified name is already loaded.
     **/
    static bool IsModelLoaded( const char * modelName );

    /**
     * \brief   Returns true, if the model with specified name is already registered.
     * \param   modelName   The unique name of model to search in registered list.
     * \return  Returns true, if the Model with specified name is already exist in the list.
     **/
    static bool IsModelExist( const char * modelName );

    /**
     * \brief   Searches in the list the component by given name. If found, sets component data.
     *          Returns true if found component and the data was successfully set.
     * \param   roleName    The name of component to search in the list.
     * \param   compData    The data to set in component which is passed to create method.
     * \note    The system automatically does not free manually allocated space. This means that 
     *          if as a component data a pointer to manually allocated object is passed,
     *          it should be as well manually freed.
     **/
    static bool SetComponentData( const char * roleName, NEMemory::uAlign compData );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor.
     *          Hidden constructor. Used only when static 
     *          component loader is instantiated.
     **/
    CEComponentLoader( void );

    /**
     * \brief   Destructor.
     **/
    ~CEComponentLoader( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Loads threads and components of specified model.
     *          It will start registered threads, which will load registered components.
     *          If modelName is NULL or empty, it will load all Models.
     *          If modelName is not NULL and not empty, it will search for Model with
     *          specified name and will start registered threads.
     *          Function returns when all threads are started with success.
     * \param   modelName   The name of Model to load. If NULL or empty string,
     *                      it loads all models, which are not loaded yet.
     * \return  Returns true if components are loaded with success.
     **/
    bool LoadModel( const char * modelName = NULL );

    /**
     * \brief   Loads specified Model. It will start all registered in Model threads,
     *          and start all components. Function returns when all threads are started with success.
     * \param   whichModel  The Model object to load.
     * \return  Returns true if specified mode is loaded with success.
     **/
    bool LoadModel( NERegistry::CEModel & whichModel );
    /**
     * \brief   Unloads Model with specified name, deletes components and stops threads.
     *          If modelName is not NULL and not empty, it will unload
     *          model with specified name.
     *          If modelName is NULL or empty, it will unload all models.
     * \param   modelName   The name of model to unload. If NULL, it will unload all models
     **/
    void UnloadModel( const char * modelName = NULL );

    /**
     * \brief   Unloads specified Model, deletes components and stops threads.
     * \param   whichModel  The Model object, which should be unloaded.
     **/
    void UnloadModel( NERegistry::CEModel & whichModel );

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
    void ShutdownThreads( const NERegistry::CEComponentThreadList & whichThreads );

    /**
     * \brief   The call of this function will suspend current thread and
     *          the function will return when all threads in the registered list
     *          completed and exit. The call of this function does not stop or
     *          destroy threads
     * \param   whichThreads    The list of registered thread to wait for completion.
     **/
    void WaitThreadsCompletion( const NERegistry::CEComponentThreadList & whichThreads );

    /**
     * \brief   The call of this function will destroy all registered in the list
     *          threads and wait until all threads complete job and exit.
     **/
    void DestroyThreads( const NERegistry::CEComponentThreadList & whichThreads );

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
    bool AddModel( const NERegistry::CEModel & newModel );

protected:

    /**
     * \brief   Searches in registries model by name. If found, returns valid pointer. Otherwise, returns null.
     * \param   modelName   The name of model to search. All models should be unique within one process context.
     * \return  If found model, returns valid pointer. Otherwise, returns null.
     **/
    const NERegistry::CEModel * FindModelByName( const char * modelName ) const;

    /**
     * \brief   Searches in registries thread entry by name. The threads should have unique names within process context.
     *          If found, returns valid pointer to thread entry. Otherwise, returns null.
     * \param   threadName  The name of thread to search in system.
     * \return  Returns valid pointer if found thread entry registered with specified name. Otherwise, returns null.
     **/
    const NERegistry::CEComponentThreadEntry * FindThreadEntryByName( const char * threadName ) const;

    /**
     * \brief   Searches in registries component entry by role name. The role names should have unique within process context.
     *          If found, returns valid pointer to component entry. Otherwise, returns null.
     * \param   roleName    The role name of component to search in system.
     * \return  Returns valid pointer if found component entry registered with specified role name. Otherwise, returns null.
     **/
    const NERegistry::CEComponentEntry * FindComponentEntryByName( const char * roleName ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

#if _MSC_VER
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The list of models
     **/
    ModelList       mModelList;
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER
        
    /**
     * \brief   The name of default model
     **/
    CEString        mDefaultModel;

    /**
     * \brief   Synchronization object
     **/
    CEResourceLock  mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEComponentLoader( const CEComponentLoader & /*src*/ );
    const CEComponentLoader & operator = ( CEComponentLoader & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEComponentLoader class inline function implementation
//////////////////////////////////////////////////////////////////////////

#endif  // AREG_COMPONENT_CECOMPONENTLOADER_HPP
