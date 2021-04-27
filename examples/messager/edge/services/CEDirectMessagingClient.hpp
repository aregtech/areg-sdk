#pragma once
/************************************************************************
 * \file            edge/services/CEDirectMessagingClient.hpp
 * \brief           The messaging service client object
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/CEComponent.hpp"
#include "shared/generated/CEDirectMessagerClientBase.hpp"
#include "edge/NEDistributedApp.hpp"

class CEChatPrticipantHandler;

class CEDirectMessagingClient   : public CEDirectMessagerClientBase
{
public:
    CEDirectMessagingClient( CEComponent & owner, const char * roleName, CEChatPrticipantHandler & handlerParticipants );
    virtual ~CEDirectMessagingClient( void );

public:
    /**
     * \brief   Response callback.
     *          Response to join cha
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   succeed         Flag, indicating whether connection is accepted or not.
     * \param   listParticipant DESCRIPTION MISSED
     * \param   timeConnect     DESCRIPTION MISSED
     * \param   timeConnected   DESCRIPTION MISSED
     * \see     RequestChatJoin
     **/
    virtual void ResponseChatJoin( const bool & succeed, const NEDirectMessager::ListParticipants & listParticipant, const CEDateTime & timeConnect, const CEDateTime & timeConnected );

    /**
     * \brief   Broadcast callback.
     *          DESCRIPTION MISSED
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   sender      DESCRIPTION MISSED
     * \param   msgText     DESCRIPTION MISSED
     * \param   timeSent    DESCRIPTION MISSED
     **/
    virtual void BroadcastMessageSent( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent );

    /**
     * \brief   Broadcast callback.
     *          DESCRIPTION MISSED
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   participant DESCRIPTION MISSED
     * \param   msgText     DESCRIPTION MISSED
     **/
    virtual void BroadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const CEString & msgText );

    /**
     * \brief   Broadcast callback.
     *          DESCRIPTION MISSED
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   participant DESCRIPTION MISSED
     * \param   timeJoined  DESCRIPTION MISSED
     **/
    virtual void BroadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeJoined );

    /**
     * \brief   Broadcast callback.
     *          DESCRIPTION MISSED
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   participant DESCRIPTION MISSED
     * \param   timeLeft    DESCRIPTION MISSED
     **/
    virtual void BroadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeft );

    /**
     * \brief   Broadcast callback.
     *          DESCRIPTION MISSED
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     **/
    virtual void BroadcastChatClosed( void );

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
    inline CEDirectMessagingClient & self( void );

    inline void updateChatOutput( const NEDistributedApp::eWndCommands cmdSend, const NEDirectMessager::sParticipant & participant, const CEString & msgText, const CEDateTime & dateStart, const CEDateTime & dateEnd );

private:
    CEChatPrticipantHandler &   mParticipantsHandler;
};

inline CEDirectMessagingClient & CEDirectMessagingClient::self( void )
{   return (*this);         }
