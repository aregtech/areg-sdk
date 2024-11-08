#pragma once
/************************************************************************
 * \file            chatter/services/CentralMessaging.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "examples/17_winchat/services/CentralMessagerClientBase.hpp"

class DispatcherThread;
class ConnectionHandler;

class CentralMessaging    : public CentralMessagerClientBase
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    CentralMessaging( const char * roleName, DispatcherThread & ownerThread, ConnectionHandler & handlerConnection );
    virtual ~CentralMessaging( void ) = default;

public:

    void ReceiveMessages( bool doReceive );

    void ReceiveKeytype( bool doReceive );

    void ReceiveBroadcasting( bool doReceive );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Server broadcast.
     *          The response, sent by connection manager to notify the message typing update
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie assigned to initiator
     * \param   newMessage  The message sent by initiator
     * \param   dateTime    The local time-stamp of initiator
     **/
    virtual void broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime ) override;
    /**
     * \brief   Server broadcast.
     *          Sent each time when a client is typing a key
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   nickName    The nick name of initiator
     * \param   cookie      Assigned cookie of initiator
     * \param   newMessage  The message typed by initiator
     **/
    virtual void broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage ) override;
    /**
     * \brief   Server broadcast.
     *          Server broadcasts a message to all clients.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   serverMessage   The message sent by servicing server
     * \param   dateTime        The time-stamp of servicing component
     **/
    virtual void broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime ) override;

/************************************************************************/
// IEProxyListener Overrides
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
    virtual bool serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    inline CentralMessaging & self( void );

//////////////////////////////////////////////////////////////////////////
// Hidden variables
//////////////////////////////////////////////////////////////////////////
    ConnectionHandler &   mConnectionHandler;
    bool        mReceiveMessages;
    bool        mReceiveTyping;
    bool        mReceiveBroadcast;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CentralMessaging( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( CentralMessaging );
};

//////////////////////////////////////////////////////////////////////////
// CentralMessaging inline methods
//////////////////////////////////////////////////////////////////////////

inline CentralMessaging & CentralMessaging::self( void )
{   return (*this);     }
