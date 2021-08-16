#pragma once
#include "generated/ConnectionManagerClientBase.hpp"

class Component;
class ConnectionHandler;

class ConnectionList : public ConnectionManagerClientBase
{
public:
    ConnectionList( const char * roleName, Component & owner, ConnectionHandler & handlerConnection );
    ConnectionList( const char * roleName, DispatcherThread & dispThread, ConnectionHandler & handlerConnection );
    virtual ~ConnectionList( );

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Response Connect
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The result of request to register direct connection service.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   connection      The connection structure after registration.
     * \param   connectionList  The list of all servicing connections.
     * \param   success         Flag, indicating whether the request to register connection succeeded or not.
     * \see     requestRegisterConnection
     **/
    virtual void responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success );
    /**
     * \brief   Broadcast callback.
     *          Triggered each time when new client connection is accepted
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientConnected New client data, which contains nick name and connected date-time
     **/
    virtual void broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected );
    /**
     * \brief   Broadcast callback.
     *          Triggered each time when client is disconnected
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientLeft  The connection data of disconnected client.
     **/
    virtual void broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft );

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

private:
    ConnectionHandler &   mConnectionHandler;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ConnectionList( void );
    ConnectionList( const ConnectionList & /*src*/ );
    const ConnectionList & operator = ( const ConnectionList & /*src*/ );
};
