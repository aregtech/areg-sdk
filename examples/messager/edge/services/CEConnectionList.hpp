#pragma once
#include "shared/generated/CEConnectionManagerClientBase.hpp"

class CEComponent;
class CEConnectionHandler;

class CEConnectionList : public CEConnectionManagerClientBase
{
public:
    CEConnectionList( const char * roleName, CEComponent & owner, CEConnectionHandler & handlerConnection );
    CEConnectionList( const char * roleName, CEDispatcherThread & dispThread, CEConnectionHandler & handlerConnection );
    virtual ~CEConnectionList( );

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************
 * Attribute ConnectionList functions
 ************************************************************************/

    /**
     * \brief   Response callback.
     *          DESCRIPTION MISSED
     *          Overwrite, if need to handle Response call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   connection      DESCRIPTION MISSED
     * \param   connectionList  DESCRIPTION MISSED
     * \param   success         DESCRIPTION MISSED
     * \see     RequestRegisterConnection
     **/
    virtual void ResponseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, const bool & success );
    /**
     * \brief   Broadcast callback.
     *          Triggered each time when new client connection is accepted
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientConnected New client data, which contains nick name and connected date-time
     **/
    virtual void BroadcastClientConnected( const NEConnectionManager::sConnection & clientConnected );
    /**
     * \brief   Broadcast callback.
     *          Triggered each time when client is disconnected
     *          Overwrite, if need to handle Broadcast call of server object. 
     *          This call will be automatically triggered, on every appropriate request call
     * \param   clientLeft  The connection data of disconnected client.
     **/
    virtual void BroadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft );

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
    virtual bool ServiceConnected( const bool isConnected, CEProxyBase & proxy );

private:
    CEConnectionHandler &   mConnectionHandler;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEConnectionList( void );
    CEConnectionList( const CEConnectionList & /*src*/ );
    const CEConnectionList & operator = ( const CEConnectionList & /*src*/ );
};
