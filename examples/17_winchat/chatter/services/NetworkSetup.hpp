#pragma once
#include "generated/ConnectionManagerClientBase.hpp"

class Component;
class ConnectionHandler;

class NetworkSetup :  public ConnectionManagerClientBase
{
public:
    NetworkSetup( const char * roleName, Component & owner, ConnectionHandler & handlerConnection );
    virtual ~NetworkSetup( );

    void DisconnectServicing( void );

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Attributes
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attribute ConnectionList functions
 ************************************************************************/
    /**
     * \brief   Response callback.
     *          The result of connection request.
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   nickName    Client connection data, which contains nick name and connection date-time
     * \param   cookie      Connection assigned cookie for client service
     * \param   dateTime    The client service connection time-stamp in UTC
     * \param   result      The connection operation result
     * \see     requestConnect
     **/
    virtual void responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result );

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
    NetworkSetup( void ) = delete;
    NetworkSetup( const NetworkSetup & /*src*/ ) = delete;
    const NetworkSetup & operator = ( const NetworkSetup & /*src*/ ) = delete;
};
