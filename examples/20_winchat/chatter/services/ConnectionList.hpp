#pragma once
#include "examples/20_winchat/services/ConnectionManagerClientBase.hpp"

class Component;
class ConnectionHandler;

class ConnectionList : public ConnectionManagerClientBase
{
public:
    ConnectionList( const char * roleName, Component & owner, ConnectionHandler & handlerConnection );
    ConnectionList( const char * roleName, DispatcherThread & dispThread, ConnectionHandler & handlerConnection );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
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
    virtual void responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnections & connectionList, bool success ) override;
    /**
     * \brief   Broadcast callback.
     *          Triggered each time when new client connection is accepted
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientConnected New client data, which contains nick name and connected date-time
     **/
    virtual void broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected ) override;
    /**
     * \brief   Broadcast callback.
     *          Triggered each time when client is disconnected
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientLeft  The connection data of disconnected client.
     **/
    virtual void broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft ) override;

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
    ConnectionHandler &   mConnectionHandler;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ConnectionList( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ConnectionList );
};
