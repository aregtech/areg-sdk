#pragma once
/************************************************************************
 * \file            chatter/services/CentralMessaging.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "generated/CentralMessagerClientBase.hpp"

class DispatcherThread;
class ConnectionHandler;

class CentralMessaging    : public CentralMessagerClientBase
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    CentralMessaging( const char * roleName, DispatcherThread & ownerThread, ConnectionHandler & handlerConnection );
    virtual ~CentralMessaging( void );

public:

    void ReceiveMessages( bool doReceive );

    void ReceiveKeytype( bool doReceive );

    void ReceiveBroadcasting( bool doReceive );

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attribute ConnectionList functions
 ************************************************************************/
//////////////////////////////////////////////////////////////////////////
// CentralMessager Interface Requests / Responses / Broadcast
//////////////////////////////////////////////////////////////////////////
public:
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
    virtual void broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime );
    /**
     * \brief   Server broadcast.
     *          Sent each time when a client is typing a key
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   nickName    The nick name of initiator
     * \param   cookie      Assigned cookie of initiator
     * \param   newMessage  The message typed by initiator
     **/
    virtual void broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage );
    /**
     * \brief   Server broadcast.
     *          Server broadcasts a message to all clients.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   serverMessage   The message sent by servicing server
     * \param   dateTime        The time-stamp of servicing component
     **/
    virtual void broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime );

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
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ConnectionHandler &   mConnectionHandler;
    bool        mReceiveMessages;
    bool        mReceiveTyping;
    bool        mReceiveBroadcast;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline CentralMessaging & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    CentralMessaging( void );
    CentralMessaging( const CentralMessaging & /*src*/ );
    const CentralMessaging & operator = ( const CentralMessaging & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CentralMessaging inline methods
//////////////////////////////////////////////////////////////////////////

inline CentralMessaging & CentralMessaging::self( void )
{   return (*this);     }
