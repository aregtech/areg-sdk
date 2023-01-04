#pragma once

/************************************************************************
 * \file        pubclient/src/ServiceClient.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is demonstration of manual unlbocking of the request.
 *              The service client declaration
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "generated/src/HelloUnblockClientBase.hpp"

#include "areg/base/TEStack.hpp"
#include "areg/component/Timer.hpp"

/**
 * \brief   This service client sends a request every 200 ms (NEHelloUnblock::ClientTimeot).
 *          Since the service responses with the timeout 500 ms, if do not manually
 *          unblock the request it will fail with reason 'the request is busy'
 *          (NEService::eResultType::RequestBusy).
 *          This client demonstrate that all requests are processed and sent to the client.
 *          Start multiple instances of the client to make sure that all clients properly
 *          recieve requests.
 **/
class ServiceClient : public    Component
                    , private   HelloUnblockClientBase
                    , private   IETimerConsumer
{
    //!< The list of generated sequence IDs to check the request.
    using SequenceList = TENolockStack<uint32_t>;

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
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    ServiceClient( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Destructor.
     **/
    virtual ~ServiceClient( void ) = default;

protected:
    /**
     * \brief   Triggered by Proxy, when gets service connected event.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

/************************************************************************
 * Response HelloUnblock
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Sent to set ID for client.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientId    Generated ID for the client used when send requst to unblock.
     * \see     requestIdentifier
     **/
    virtual void responseIdentifier( unsigned int clientId ) override;
    /**
     * \brief   Response callback.
     *          The response to hello world request.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientId    The ID of the client to send the response. Never is 0.
     * \param   seqNr       The sequence number created by the client. On reply the service sends the sequence number so that
     *          the cilent can check that all sequences exist.
     * \see     requestHelloUblock
     **/
    virtual void responseHelloUnblock( unsigned int clientId, unsigned int seqNr ) override;

    /**
     * \brief   This method is triggered if requestHelloUblock call failes.
     *          It may happen if the request is busy and not completed.
     *          Since the request is manually unblocked on service side,
     *          we override this method to make sure that it never fails
     *          with reason NEService::eResultType::RequestBusy, which happens
     *          if the request is still pending.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestHelloUblockFailed( NEService::eResultType FailureReason ) override;

    /**
     * \brief   Triggered, when HelloServiceState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes HelloServiceState description:
     *          The state of the service.
     * \param   HelloServiceState   The value of HelloServiceState attribute.
     * \param   state               The data validation flag.
     **/
    virtual void onHelloServiceStateUpdate( NEHelloUnblock::eServiceState HelloServiceState, NEService::eDataStateType state ) override;

    /**
     * \brief   Triggered when Timer is expired. 
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ServiceClient & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    uint32_t        mClientId;      //!< The ID of the client given by service.
    uint32_t        mSequenceId;    //!< Current sequence ID.
    SequenceList    mSequenceList;  //!< The list of generated sequences.
    Timer           mTimer;         //!< The timer to send requests.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceClient );
};