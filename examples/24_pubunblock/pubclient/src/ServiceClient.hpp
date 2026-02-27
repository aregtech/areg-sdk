#pragma once

/************************************************************************
 * \file        pubclient/src/ServiceClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is demonstration of manual unlbocking of the request.
 *              The service client declaration
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "examples/24_pubunblock/services/HelloUnblockClientBase.hpp"

#include "areg/base/Stack.hpp"
#include "areg/component/Timer.hpp"

/**
 * \brief   This service client sends a request every 200 ms (HelloUnblock::ClientTimeot).
 *          Since the service responses with the timeout 500 ms, if do not manually
 *          unblock the request it will fail with reason 'the request is busy'
 *          (areg::ResultType::RequestBusy).
 *          This client demonstrate that all requests are processed and sent to the client.
 *          Start multiple instances of the client to make sure that all clients properly
 *          receive requests.
 **/
class ServiceClient : public    areg::Component
                    , private   HelloUnblockClientBase
                    , private   areg::TimerConsumer
{
    //!< The list of generated sequence IDs to check the request.
    using SequenceList = areg::Stack<uint32_t>;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceClient( const areg::ComponentEntry & entry, areg::ComponentThread & owner );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// ProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered when receives service provider connected / disconnected event.
     *          When the service provider is connected, the client objects can set the listeners here.
     *          When the service provider is disconnected, the client object should clean the listeners.
     *          Up from connected status, the clients can subscribe and unsubscribe on updates,
     *          responses and broadcasts, can trigger requests. Before connection, the clients cannot
     *          neither trigger requests, nor receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object, which is notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    bool serviceConnected( areg::ServiceConnectionState status, areg::ProxyBase & proxy ) override;

/************************************************************************
 * Response HelloUnblock
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          Sent to set ID for client.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientId    Generated ID for the client used when send request to unblock.
     * \see     requestIdentifier
     **/
    void responseIdentifier( uint32_t clientId ) override;
    /**
     * \brief   Response callback.
     *          The response to hello world request.
     *          Overwrite, if need to handle Response call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientId    The ID of the client to send the response. Never is 0.
     * \param   seqNr       The sequence number created by the client. On reply the service sends the sequence number so that
     *          the client can check that all sequences exist.
     * \see     requestHelloUblock
     **/
    void responseHelloUnblock( uint32_t clientId, uint32_t seqNr ) override;

    /**
     * \brief   This method is triggered if requestHelloUblock call fails.
     *          It may happen if the request is busy and not completed.
     *          Since the request is manually unblocked on service side,
     *          we override this method to make sure that it never fails
     *          with reason areg::ResultType::RequestBusy, which happens
     *          if the request is still pending.
     * \param   FailureReason   The failure reason value of request call.
     **/
    void requestHelloUblockFailed( areg::ResultType FailureReason ) override;

    /**
     * \brief   Triggered, when HelloServiceState attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes HelloServiceState description:
     *          The state of the service.
     * \param   HelloServiceState   The value of HelloServiceState attribute.
     * \param   state               The data validation flag.
     **/
    void onHelloServiceStateUpdate( HelloUnblock::RunState HelloServiceState, areg::DataState state ) override;

    /**
     * \brief   Triggered when Timer is expired. 
     * \param   timer   The timer object that is expired.
     **/
    void processTimer( areg::Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ServiceClient & self()
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    uint32_t        mClientId;      //!< The ID of the client given by service.
    uint32_t        mSequenceId;    //!< Current sequence ID.
    uint32_t        mRespReceived;  //!< The total number of requests sent
    SequenceList    mSequenceList;  //!< The list of generated sequences.
    areg::Timer           mTimer;         //!< The timer to send requests.
#ifdef DEBUG
    uint32_t        mReqCount;      //!< The number of requests. Test only in Debug build.
#endif // DEBUG


//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClient() = delete;
    AREG_NOCOPY_NOMOVE( ServiceClient );
};
