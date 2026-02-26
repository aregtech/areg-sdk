#pragma once
#include "examples/20_winchat/services/ConnectionManagerClientBase.hpp"

namespace areg { class Component; }
class ConnectionHandler;

class NetworkSetup :  public ConnectionManagerClientBase
{
public:
    NetworkSetup( const char * roleName, areg::Component & owner, ConnectionHandler & handlerConnection );
    virtual ~NetworkSetup( ) = default;

    void DisconnectServicing();

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
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
    void responseConnect( const areg::String & nickName, uint32_t cookie, const areg::DateTime & dateTime, ConnectionManager::ConnectionResult result ) override;

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

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    ConnectionHandler &   mConnectionHandler;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    NetworkSetup() = delete;
    AREG_NOCOPY_NOMOVE( NetworkSetup );
};
