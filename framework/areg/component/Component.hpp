#ifndef AREG_COMPONENT_COMPONENT_HPP
#define AREG_COMPONENT_COMPONENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/Component.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
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
 * \brief   The component object is a container of services and service client
 *          objects. Each component can contain one or more services and 
 *          service clients objects. Each component has only one thread owner
 *          and may have no or many worker threads. If a component contains
 *          service objects, then it is servicing component and the role of
 *          of component is the provided service name. The service name
 *          (or role name) and service interface name have different meaning
 *          and developers should not mix them. If a service interface object
 *          with the same service interface name can be instantiated several times,
 *          each component must have unique name within their visibility and
 *          accessibility scope, i.e. in case of provided public service the
 *          name should be unique within network, and in case of provided local
 *          service, the role name should be unique within local process.
 **/
class AREG_API Component   : public    RuntimeObject
{
//////////////////////////////////////////////////////////////////////////
// Predefined types. Fol local use
//////////////////////////////////////////////////////////////////////////
    //!< The basic operations of resource-map.
    using ImplComponentResource = TEResourceMapImpl<unsigned int, Component *>;
    /**
     * \brief   The integer hash-map to store components where the keys are the calculated number of the component.
     * \tparam  Component           The saved values are Component objects
     **/
    using MapComponentContainer  = TEIntegerHashMap<Component *>;
    /**
     * \brief   Component::MapComponentResource
     *          The Resource Map of instantiated components.
     * \tparam  unsigned int            The calculated number of component as a key.
     * \tparam  Component               The type of container values, it contains Components
     * \tparam  MapComponentContainer   The hash-map object to store containers.
     * \tparam  ImplComponentResource   The implementation of basic resource+map operations.
     **/
    using MapComponentResource  = TELockResourceMap<unsigned int, Component *, MapComponentContainer, ImplComponentResource>;
//////////////////////////////////////////////////////////////////////////
// Declare as runtime object
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(Component)

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Component::ListServers
     *          The list of addresses of Servers.
     * \tparam  StubBase  The pointer to base class of Stub objects.
     **/
    using ListServers           = TELinkedList<StubBase*>;

/************************************************************************/
// static functions to load / unload component
/************************************************************************/

    /**
     * \brief   This function is loading component. The component loading information
     *          like component create and delete functions, worker thread information
     *          is taken from component entry object.
     * \param   entry           The NERegistry::ComponentEntry containing component loading information.
     * \param   componentThread The thread, which is loading component and dispatching messages
     * \return  Returns pointer to instantiated component.
     **/
    static Component * loadComponent( const NERegistry::ComponentEntry & entry, ComponentThread & componentThread);

    /**
     * \brief   This function is unloading component.
     *          like component create and delete functions, worker thread information
     *          is taken from component entry object.
     * \param   comItem The component object, which should be unloaded.
     * \param   entry   The NERegistry::ComponentEntry containing component loading information.
     **/
    static void unloadComponent( Component & comItem, const NERegistry::ComponentEntry & entry);

/************************************************************************/
// static utility functions to search component and check existence
/************************************************************************/

    /**
     * \brief	Find and return component by specified role name
     * \param	roleName	The role name of registered component to search
     * \return	If found, returns pointer to component object.
     *          Otherwise returns nullptr.
     **/
    static Component * findComponentByName(const String & roleName);

    /**
     * \brief	Find and return component by specified component number
     * \param	magicNum	The calculated component number to search
     * \return	If found, returns pointer to component object. Otherwise, returns nullptr.
     **/
    static Component * findComponentByNumber(unsigned int magicNum);

    /**
     * \brief	Checks whether component exists in component registries or not.
     * \param	roleName	The role name of component to look up
     * \return	Returns true ff found entry in registries. Otherwise returns false.
     **/
    static bool existComponent(const String & roleName);

    /**
     * \brief	Find component in registries by given component address.
     * \param	comAddress  The address of component to look up.
     * \return	If found, returns pointer to registered component.
     *          Otherwise returns nullptr.
     **/
    static Component * findComponentByAddress(const ComponentAddress & comAddress);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Instantiates the component object and dispatches the events in the specified thread.
     * \param	roleName	Unique role name of the component.
     * \param	ownerThread The instance of the thread, which owns the component.
     **/
    Component( const String & roleName, ComponentThread & ownerThread );

    /**
     * \brief	Instantiates the component object and dispatches the events in the specified thread.
     * \param	regEntry	The registry entry object with the role name of the component.
     * \param	ownerThread The instance of the thread, which owns the component.
     **/
    Component( const NERegistry::ComponentEntry & regEntry, ComponentThread & ownerThread );

    /**
     * \brief	Instantiates the component object and dispatches the events in the current thread.
     * \param	roleName	Unique role name of the component.
     **/
    explicit Component( const String & roleName );

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
    virtual IEWorkerThreadConsumer * workerThreadConsumer( const String & consumerName, const String & workerThreadName );

/************************************************************************/
// Component operations
/************************************************************************/

    /**
     * \brief	Creates and run Worker thread by given name
     * \param	threadName	    Worker thread name to created. 
     *                          Should be unique within system.
     * \param   consumer        Worker Thread consumer object, which
     *                          start and stop functions will be triggered.
     * \param   ownerThread     The component thread, which owns worker thread,
     * \param   watchdogTimeout The watchdog timeout in milliseconds.
     * \return	Pointer to created worker thread object.
     **/
    WorkerThread * createWorkerThread( const String & threadName, IEWorkerThreadConsumer & consumer, ComponentThread & ownerThread, uint32_t watchdogTimeout );

    /**
     * \brief	Stops and deletes worker thread by given name
     * \param	threadName	Worker thread name to stop and delete.
     **/
    void deleteWorkerThread( const String & threadName );

    /**
     * \brief   Call to terminate the component execution and cleanup resources.
     *          After calling this method the component deletes all worker threads,
     *          cleans up resources and components become not operable anymore.
     **/
    void terminateSelf( void );

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
    StubBase * findServerByName( const String & serviceName );

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
    inline const ListServers & extractRemoteServiceAddresses( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Hidden member variables
/************************************************************************/

    /**
     * \brief   Component informations object, which contains
     *          component address and registered worker thread list.
     **/
    ComponentInfo         mComponentInfo;

    /**
     * \brief   The calculated number of component.
     **/
    unsigned int        mMagicNum;

private:
/************************************************************************/
// Private methods
/************************************************************************/

    /**
     * \brief   Returns reference of component object.
     **/
    inline Component & self( void );

    /**
     * \brief   Shutdowns all registered services of the component.
     **/
    inline void _shutdownServices( void );
    /**
     * \brief   Static method. Returns the component thread of current component.
     **/
    static ComponentThread & _getCurrentComponentThread( void );

    /**
     * \brief   Calculates the number of specified component object.
     **/
    static unsigned int _magicNumber( Component & comp );

private:
/************************************************************************/
// Private member variables
/************************************************************************/
#if defined(_MSC_VER) && (_MSC_VER > 1200)
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

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Component );
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

inline const ComponentAddress& Component::getAddress( void ) const
{
    return mComponentInfo.getAddress();
}

inline const Component::ListServers & Component::extractRemoteServiceAddresses( void ) const
{
    return mServerList;
}

inline Component& Component::self( void )
{
    return (*this);
}

#endif  // AREG_COMPONENT_COMPONENT_HPP
