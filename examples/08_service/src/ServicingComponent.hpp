#ifndef AREG_EXAMPLES_08_SERVICE_SERVICINGCOMPONENT_HPP
#define AREG_EXAMPLES_08_SERVICE_SERVICINGCOMPONENT_HPP
/************************************************************************
 * \file        src/ServicingComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A demo of simple servicing component with timer without component servicing (request) methods.
 **/
class ServicingComponent    : public    Component
                            , protected StubBase
                            , protected IETimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////
private:
    static const unsigned int   TIMER_TIMEOUT   = 150;   //!< Timer timeout in milliseconds
    static const int            TIMER_EVENTS    =  50;   //!< Fired timer count.

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    
    /**
     * \brief   Instantiates the component object.
     * \param   masterThread    The instance of component owner thread.
     * \param   roleName        The role name of component, given in the system.
     * \param   data            The optional component data set in system. Can be empty / no data.
     **/
    ServicingComponent(ComponentThread & masterThread, const char * const roleName, NEMemory::uAlign OPTIONAL data);

    /**
     * \brief   Destructor.
     **/
    virtual ~ServicingComponent(void);

/************************************************************************/
// CEStubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void startupServiceInterface( Component & holder );

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder );

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer );

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StubBase overrides. Public pure virtual methods 
/************************************************************************/

    /**
     * \brief   Sends update notification message to all clients. 
     *          This method can be called manually to send update 
     *          notification message after updating attribute value.
     *
     *          Overwrite to implement method
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void sendNotification( unsigned int msgId );

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::RESULT_REQUEST_ERROR or NEService::RESULT_REQUEST_CANCELED, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::RESULT_INVALID.
     *          If message ID is an attribute, it should send result NEService::RESULT_DATA_INVALID
     *          and invalidate attribute data value.
     *
     *          Overwrite to implement method
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void errorRequest( unsigned int msgId, bool msgCancel );

/************************************************************************/
// IEStubEventConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered to process service request event.
     *          Overwrite method to process every service request event.
     * \param   eventElem   Service Request Event object, contains request
     *                      call ID and parameters.
     **/
    virtual void processRequestEvent( ServiceRequestEvent & eventElem );
    
    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void processAttributeEvent( ServiceRequestEvent & eventElem );

private:
    Timer   mTimer; //!< The timer to run in component thread.
    int     mCount; //!< The timer event count.

private:
    inline ServicingComponent & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServicingComponent( void );
    ServicingComponent( const ServicingComponent & /*src*/);
    const ServicingComponent & operator = ( const ServicingComponent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// ServicingComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline ServicingComponent & ServicingComponent::self( void )
{   return (*this);     }


#endif // AREG_EXAMPLES_08_SERVICE_SERVICINGCOMPONENT_HPP
