#ifndef AREG_COMPONENT_COMPONENT_HPP
#define AREG_COMPONENT_COMPONENT_HPP
/************************************************************************
 * \file        areg/component/Component.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component class declaration.
 *              To receive messages, all service interface objects
 *              should be grouped in component. Use this class as a base
 *              class for all defined components in application.
 *          
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/RuntimeObject.hpp"

#include "areg/component/ComponentAddress.hpp"
#include "areg/component/private/ComponentInfo.hpp"
#include "areg/component/NERegistry.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEWorkerThreadConsumer;
class ComponentThread;
class WorkerThread;
class StubBase;

//////////////////////////////////////////////////////////////////////////
// Component class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       This is base class to define application component.
 *              The component object should know its master (holder) 
 *              thread and have unique role name within single application.
 *              Same component can be instantiated more than one time,
 *              but ti should differ by role name.
 * 
 *              Define component whenever is a need to define one or
 *              more Clients and Stubs as a component. Components are
 *              required by dispatcher to know how to deliver events.
 *              Components have Address. If same component object is
 *              instantiated more than one time, they should differ by
 *              role name. This ensures having unique addresses.
 *              Component contains binder with its worker threads to
 *              ensure that communication between threads are not out
 *              of component scope. Component class is a runtime object
 *              and it is Worker Thread Consumer to provide possibility to
 *              set and remove certain listeners of certain events.
 *              If component is defined in model and exists in registry
 *              service, it will be automatically instantiated when its
 *              thread starts to run. 
 *              For more details of model designing see NERegistry
 *
 **/
class AREG_API Component   : public    RuntimeObject
{
//////////////////////////////////////////////////////////////////////////
// Predefined types. Fol local use
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Component::MapComponentResource
     *          The Resource Map of instantiated components.
     * \tparam  String        String, used for Role Name of component
     * \tparam  Component     The type of container values, it contains Components
     * \tparam  TEStringHashMap<Component *, Component *>  The type of Hash Map, it is string-to-pointer hash map
     **/
    typedef TEStringHashMapImpl<Component *>                                                    StringHashMapImpl;
    typedef TEStringHashMap<Component *, Component *, StringHashMapImpl>                        MaComponentContainer;
    typedef TEResourceMapImpl<String, Component>                                                ImplComponentResource;
    typedef TELockResourceMap<String, Component, MaComponentContainer, ImplComponentResource>   MapComponentResource;
    /**
     * \brief   Component::ListServers
     *          The list of addresses of Servers.
     * \tparam  StubBase  The pointer to base class of Stub objects.
     **/
    typedef TELinkedList<StubBase*, StubBase*>                                                  ListServers;

//////////////////////////////////////////////////////////////////////////
// Declare as runtime object
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(Component)

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// static functions to load / unload component
/************************************************************************/

    /**
     * \brief   This function is loading component. The component loading information
     *          like component create and delete functions, worker thread information
     *          is taken from component entry object.
     * \param   entry           The NERegistry::ComponentEntry containing component loading infromation.
     * \param   componentThread The thread, which is loading component and dispatching messages
     * \return  Returns pointer to instantiated component.
     **/
    static Component * loadComponent( const NERegistry::ComponentEntry & entry, ComponentThread & componentThread);

    /**
     * \brief   This function is unloading component.
     *          like component create and delete functions, worker thread information
     *          is taken from component entry object.
     * \param   comItem The component object, which should be unloaded.
     * \param   entry   The NERegistry::ComponentEntry containing component loading infromation.
     **/
    static void unloadComponent( Component & comItem, const NERegistry::ComponentEntry & entry);

/************************************************************************/
// static utility functions to search component and check existence
/************************************************************************/

    /**
     * \brief	Find and return component by specified role name
     * \param	roleName	The role name of registered component to search
     * \return	If found, returns pointer to component object.
     *          Otherwise returns NULL.
     **/
    static Component * findComponentByName(const char * roleName);

    /**
     * \brief	Checks whether component exists in component registries or not.
     * \param	roleName	The role name of component to look up
     * \return	Returns true ff found entry in registries. Otherwise returns false.
     **/
    static bool existComponent(const char * roleName);

    /**
     * \brief	Find component in registries by given component address.
     * \param	comAddress  The address of component to look up.
     * \return	If found, returns pointer to registered component.
     *          Otherwise returns NULL.
     **/
    static Component * findComponentByAddress(const ComponentAddress & comAddress);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor. If master thread is specified, all events and functions
     *          will be triggered within specified thread context.
     * \param	masterThread	The reference to master thread object
     *                          to dispatch and trigger functions.
     * \param	roleName	    Role Name of component, which should be
     *                          unique within one process.
     **/
    Component( ComponentThread & masterThread, const char * roleName );
    /**
     * \brief   MUST be instantiated in Component Thread!!!
     **/
    /**
     * \brief	Constructor. It assumes that the current thread is a master thread.
     *          Do not initialize manually component in worker threads.
     * \param	roleName	    Role Name of component, which should be
     *                          unique within one process.
     **/
    Component( const char * roleName );

    /**
     * \brief   Destructor.
     **/
    virtual ~Component( void );

//////////////////////////////////////////////////////////////////////////
// Operations, overrides.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    virtual void startupComponent( ComponentThread & comThread );

    /**
     * \brief	This function is triggered by component thread when it
     *          requires component to shut down. Remove listeners and 
     *          make cleanups in this function call.
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void shutdownComponent( ComponentThread & comThread );

    /**
     * \brief	This function is triggered when the master thread of component is notified
     *          to shut down. The call of this function is not thread safe and mainly called
     *          Service Manager is notified to stop job and unload components
     * \param	comThread	The component thread, which triggered shutdown command.
     **/
    virtual void notifyComponentShutdown( ComponentThread & comThread );

    /**
     * \brief   Waits until component completes job. 
     *          The component is completed its job if all worker threads are completed.
     *          No action will be performed if component has no registered worker thread.
     * \param   waitTimeout     The timeout to wait for completion for every worker thread
     **/
    virtual void waitComponentCompletion( unsigned int waitTimeout );

    /**
     * \brief   Returns pointer to Worker Thread Consumer object identified
     *          by consumer name and if needed, by worker thread name.
     *          This function is triggered, when component is initialized and
     *          worker threads should be created.
     * \param   consumerName        The name of worker thread consumer object to identify
     * \param   workerThreadName    The name of worker thread, which consumer should return
     * \return  Return valid pointer if worker thread has assigned consumer.
     **/
    virtual IEWorkerThreadConsumer * workerThreadConsumer( const char* consumerName, const char* workerThreadName );

/************************************************************************/
// Component operations
/************************************************************************/

    /**
     * \brief	Creates and run Worker thread by given name
     * \param	threadName	    Worker thread name to created. 
     *                          Should be unique within system.
     * \param   consumer        Worker Thread consumer object, which
     *                          start and stop functions will be triggered.
     * \param   masterThread    The component thread, which owns worker thread,
     * \return	Pointer to created worker thread object.
     **/
    WorkerThread * createWorkerThread( const char * threadName, IEWorkerThreadConsumer & consumer, ComponentThread & masterThread );

    /**
     * \brief	Stops and deletes worker thread by given name
     * \param	threadName	Worker thread name to stop and delete.
     **/
    void deleteWorkerThread( const char* threadName );

    /**
     * \brief	Registers Stub / Server object of component
     * \param	server	The Stub / Server object to register for component
     **/
    void registerServerItem( StubBase & server );

    /**
     * \brief	Find and return Stub / Server object by specified
     *          service name.
     * \param	serviceName	The service name of Stub / Server object.
     * \return	If found, returns pointer to registered server object.
     **/
    StubBase * findServerByName( const char* serviceName );

    /**
     * \brief	Finds event dispatcher consumer of specific runtime class ID object.
     * \param	whichClass	Runtime Class ID object, which consumer should be searched.
     * \return	If finds, returns pointer to dispatcher thread, which has registered
     *          consumer.
     **/
    inline DispatcherThread * findEventConsumer( const RuntimeClassID & whichClass ) const;

    /**
     * \brief   Returns master thread of component
     **/
    inline ComponentThread & getMasterThread( void );

    /**
     * \brief   Returns the role name of component
     **/
    inline const String & getRoleName( void ) const;

    /**
     * \brief   Returns address of component
     **/
    inline const ComponentAddress & getAddress( void ) const;

    /**
     * \brief   Returns the list of registered (provided) Server Service list.
     **/
    inline const TELinkedList<StubBase*, StubBase*> & getServiceList( void ) const;

//////////////////////////////////////////////////////////////////////////
// Protected members.
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// Protected methods
/************************************************************************/

    /**
     * \brief   Returns component information object.
     **/
    inline const ComponentInfo & getComponentInfo( void ) const;

protected:
/************************************************************************/
// Protected member variables
/************************************************************************/

    /**
     * \brief   Component informations object, which contains
     *          component address and registered worker thread list.
     **/
    ComponentInfo         mComponentInfo;

//////////////////////////////////////////////////////////////////////////
// Private Hidden members
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Private methods
/************************************************************************/

    /**
     * \brief   Returns reference of component object.
     **/
    Component & self( void );
    /**
     * \brief   Static method. Returns the component thread of current component.
     **/
    static ComponentThread & _getCurrentComponentThread( void );

private:
/************************************************************************/
// Private member variables
/************************************************************************/
#if _MSC_VER
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   List of registered server services
     **/
    Component::ListServers                  mServerList;
    /**
     * \brief   Static Resource map of created in system component.
     **/
    static  Component::MapComponentResource _mapComponentResource;
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// Component class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline ComponentThread & Component::getMasterThread( void )
{
    return mComponentInfo.getMasterThread();
}

inline DispatcherThread * Component::findEventConsumer( const RuntimeClassID& whichClass ) const
{
    return mComponentInfo.findEventConsumer(whichClass);
}

inline const String & Component::getRoleName( void ) const
{
    return mComponentInfo.getRoleName();
}

inline const ComponentInfo& Component::getComponentInfo( void ) const
{
    return mComponentInfo;
}

inline const ComponentAddress& Component::getAddress( void ) const
{
    return mComponentInfo.getAddress();
}

inline const TELinkedList<StubBase*, StubBase*> & Component::getServiceList( void ) const
{
    return static_cast<const TELinkedList<StubBase*, StubBase*> &>(mServerList);
}

inline Component& Component::self( void )
{
    return (*this);
}

#endif  // AREG_COMPONENT_COMPONENT_HPP
