#pragma once
/************************************************************************
 * \file            edge/services/CEDirectConnectionClient.hpp
 * \brief           The direct connection service client object
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "shared/generated/CEDirectConnectionClientBase.hpp"
#include "areg/component/NERegistry.hpp"

class CEChatPrticipantHandler;

class CEDirectConnectionClient  : public CEDirectConnectionClientBase
{
public:

public:
    CEDirectConnectionClient( CEComponent & owner, CEChatPrticipantHandler & participantsHandler, const NEDirectConnection::sParticipant & target );
    virtual ~CEDirectConnectionClient( void );

protected:
    /**
     * \brief   Response callback.
     *          DESCRIPTION MISSED
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   succeeded           DESCRIPTION MISSED
     * \param   target              DESCRIPTION MISSED
     * \param   listParticipants    DESCRIPTION MISSED
     * \param   initiator           DESCRIPTION MISSED
     * \see     RequestConnectoinSetup
     **/
    virtual void ResponseConnectoinSetup( const bool & succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered by Proxy, when gets service connected event.
     *          Make client initializations in this function. No request
     *          will be processed until this function is not called for
     *          client object. Also set listeners here if client is interested
     *          to receive update notifications.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected. Make cleanups and stop sending
     *                          requests or assigning for notification if
     *                          this flag is false. No event to Stub target will
     *                          be sent, until no service connected event is
     *                          received.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object,
     *          i.e. if passed Proxy address is equal to the Proxy object that client has.
     *          If Proxy objects are not equal, it should return false;
     **/
    virtual bool ServiceConnected( bool isConnected, CEProxyBase & proxy );

private:
    CEChatPrticipantHandler &       mParticipantsHandler;

private:
    CEDirectConnectionClient( void );
    CEDirectConnectionClient( const CEDirectConnectionClient & /*src*/ );
    const CEDirectConnectionClient & operator = ( const CEDirectConnectionClient & /*src*/ );
};
