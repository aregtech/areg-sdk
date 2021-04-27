#pragma once
/************************************************************************
 * \file            edge/services/CECentralMessaging.hpp
 * \brief           The connection manager client component implementation
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "shared/generated/CECentralMessagerClientBase.hpp"

class CEDispatcherThread;
class CEConnectionHandler;

class CECentralMessaging    : public CECentralMessagerClientBase
{
//////////////////////////////////////////////////////////////////////////
// Create and delete component
//////////////////////////////////////////////////////////////////////////
public:
    CECentralMessaging( const char * roleName, CEDispatcherThread & ownerThread, CEConnectionHandler & handlerConnection );
    virtual ~CECentralMessaging( void );

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
     * \brief   Broadcast callback.
     *          The response, sent by connection manager to notify the message typing update
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   nickName    The nick name of the sender
     * \param   cookie      The cookie given by connection manager
     * \param   newMessage  The message sent by client
     * \param   dateTime    The date when the message was generated.
     **/
    virtual void BroadcastSendMessage( const CEString & nickName, const uint32_t & cookie, const CEString & newMessage, const CEDateTime & dateTime );
    /**
     * \brief   Broadcast callback.
     *          Sent each time when a client is typing a key
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   nickName    The nick name of key typing
     * \param   cookie      The cookie given by connection manager
     * \param   newMessage  New message generated after key typing
     **/
    virtual void BroadcastKeyTyping( const CEString & nickName, const uint32_t & cookie, const CEString & newMessage );
    /**
     * \brief   Broadcast callback.
     *          Server broadcasts a message to all clients.
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   serverMessage   The message sent by server
     * \param   dateTime        The timestamp when the message is generated.
     **/
    virtual void BroadcastBroadcastMessage( const CEString & serverMessage, const CEDateTime & dateTime );

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

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    CEConnectionHandler &   mConnectionHandler;
    bool        mReceiveMessages;
    bool        mReceiveTyping;
    bool        mReceiveBroadcast;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline CECentralMessaging & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    CECentralMessaging( void );
    CECentralMessaging( const CECentralMessaging & /*src*/ );
    const CECentralMessaging & operator = ( const CECentralMessaging & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CECentralMessaging inline methods
//////////////////////////////////////////////////////////////////////////

inline CECentralMessaging & CECentralMessaging::self( void )
{   return (*this);     }
