#pragma once
#include "shared/generated/CEConnectionManagerClientBase.hpp"

class CEComponent;
class CEConnectionHandler;

class CENetworkSetup :  public CEConnectionManagerClientBase
{
public:
    CENetworkSetup( const char * roleName, CEComponent & owner, CEConnectionHandler & handlerConnection );
    virtual ~CENetworkSetup( );

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
     * \param   cookie      DESCRIPTION MISSED
     * \param   dateTime    DESCRIPTION MISSED
     * \param   result      Connection result.
     * \see     RequestConnet
     **/
    virtual void ResponseConnect( const CEString & nickName, const uint32_t & cookie, const CEDateTime & dateTime, const NEConnectionManager::eConnectionResult & result );

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
    virtual bool ServiceConnected( const bool isConnected, CEProxyBase & proxy );

private:
    CEConnectionHandler &   mConnectionHandler;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CENetworkSetup( void );
    CENetworkSetup( const CENetworkSetup & /*src*/ );
    const CENetworkSetup & operator = ( const CENetworkSetup & /*src*/ );
};
