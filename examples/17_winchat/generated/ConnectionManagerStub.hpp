//////////////////////////////////////////////////////////////////////////
// Begin generate generated/ConnectionManagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_CONNECTIONMANAGERSTUB_HPP
#define  GENERATED_CONNECTIONMANAGERSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:02 GMT+01:00
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 *
 * \file            generated/ConnectionManagerStub.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/NEConnectionManager.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       ConnectionManager Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              The connection manager to register each application with client
 **/
class ConnectionManagerStub   : protected  StubBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes Stub by given component object, which should be already instantiated.
     * \param   masterComp  The master component object, which is initializing service Stub.
     * \note    Before constructor is called, the instance of Component must be already initialized.
     **/
    explicit ConnectionManagerStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~ConnectionManagerStub( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Unlocks and cancels all requests, excepts those which were manually unlocked.
     *          This call will send cancel error message (NEService::eResultType::RequestCanceled)
     *          to all clients waiting for response.
     **/
    void unlockAllRequests( void );

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute ConnectionList functions
 ************************************************************************/

    /**
     * \brief   Returns true if ConnectionList attribute is valid
     **/
    inline bool isConnectionListValid( void ) const;
    /**
     * \brief   Invalidates ConnectionList attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateConnectionList( void );
    /**
     * \brief   Force to send ConnectionList attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyConnectionListUpdated( void );
    /**
     * \brief   Set ConnectionList attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of ConnectionList attribute to set.
     *                      The ConnectionList attribute description:
     *                      The map of all active connections
     **/
    virtual void setConnectionList( const NEConnectionManager::MapConnection & newValue );
    /**
     * \brief   Returns the value of ConnectionList attribute.
     *          The ConnectionList attribute description:
     *          The map of all active connections
     **/
    inline const NEConnectionManager::MapConnection & getConnectionList( void ) const;
    inline NEConnectionManager::MapConnection & getConnectionList( void );

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          The request to connect to system
     * \param   nickName    The nick name of connecting client service
     * \param   dateTime    The connecting client connection time-stamp in UTC
     * \see     responseConnect
     **/
    virtual void requestConnect( const String & nickName, const DateTime & dateTime ) = 0;

    /**
     * \brief   Request call.
     *          Request to register direct connection service.
     * \param   nickName        The nick name of connection used in service
     * \param   cookie          The cookie value to set
     * \param   connectCookie   The cookie to use for registration. 
     * \param   dateRegister    The connection registration time-stamp
     * \see     responseRegisterConnection
     **/
    virtual void requestRegisterConnection( const String & nickName, unsigned int cookie, unsigned int connectCookie, const DateTime & dateRegister ) = 0;

    /**
     * \brief   Request call.
     *          Request to disconnect.
     * \param   nickName    The nick name of client connection service
     * \param   cookie      The cookie given by connection manager. If invalid, it will search by nickName parameter
     * \param   dateTime    Disconnect request date-time
     * \note    Has no response
     **/
    virtual void requestDisconnect( const String & nickName, unsigned int cookie, const DateTime & dateTime ) = 0;

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          The result of connection request.
     * \param   nickName    Client connection data, which contains nick name and connection date-time
     * \param   cookie      Connection assigned cookie for client service
     * \param   dateTime    The client service connection time-stamp in UTC
     * \param   result      The connection operation result
     * \see     requestConnect
     **/
    virtual void responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result );

    /**
     * \brief   Response call.
     *          The result of request to register direct connection service.
     * \param   connection      The connection structure after registration.
     * \param   connectionList  The list of all servicing connections.
     * \param   success         Flag, indicating whether the request to register connection succeeded or not.
     * \see     requestRegisterConnection
     **/
    virtual void responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success );

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Broadcast call.
     *          Notifies all client components the connection has been updated.
     * \param   updatedList The complete list of clients to send.
     **/
    virtual void broadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList );

    /**
     * \brief   Broadcast call.
     *          Notifies that a new client has been connected.
     * \param   clientConnected New client data, which contains nick name and connected date-time
     **/
    virtual void broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected );

    /**
     * \brief   Broadcast call.
     *          Notifies a client has been disconnected.
     * \param   clientLeft  The connection data of disconnected client.
     **/
    virtual void broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft );

//////////////////////////////////////////////////////////////////////////
// End Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//  Operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// StubBase overrides. Public methods.
/************************************************************************/

    /**
     * \brief   Sends update notification message to all clients.
     *          This method can be called manually to send update
     *          notification message after updating attribute value.
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void sendNotification( unsigned int msgId ) override;

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::eResultType::RequestError
     *          or NEService::eResultType::RequestCanceled, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::eResultType::RequestInvalid.
     *          If message ID is an attribute, it should send result NEService::eResultType::DataInvalid
     *          and invalidate attribute data value.
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void errorRequest( unsigned int msgId, bool msgCancel ) override;

protected:
/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void startupServiceInterface( Component & holder ) override;

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder ) override;

/************************************************************************/
// StubBase overrides. Protected methods
/************************************************************************/

    /**
     * \brief   Creates Response event object to pass of client.
     * \param   proxy   The address of proxy object to send response event
     * \param   msgId   The message ID to send to client
     * \param   result  The result of message
     * \param   data    The buffer of data to send to client. Can be Invalid buffer.
     * \return  Returns valid pointer to Response event object
     **/
    virtual ResponseEvent * createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const override;

    /**
     * \brief   Overwrite method to create remote service request event from streaming object for
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteRequestEvent * createRemoteRequestEvent( const IEInStream & stream ) const override;

    /**
     * \brief   Overwrite method to create remote notify request event from streaming object for
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns nullptr.
     **/
    virtual RemoteNotifyRequestEvent * createRemoteNotifyRequestEvent( const IEInStream & stream ) const override;

private:
/************************************************************************/
// IEComponentConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered to process service request event.
     *          Overwrite method to process every service request event.
     * \param   eventElem   Service Request Event object, contains request
     *                      call ID and parameters.
     **/
    virtual void processRequestEvent( ServiceRequestEvent & eventElem ) override;

    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void processAttributeEvent( ServiceRequestEvent & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to ConnectionManagerStub object
     **/
    inline ConnectionManagerStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute ConnectionList
     *          The map of all active connections
     **/
    NEConnectionManager::MapConnection  mConnectionList;
    /**
     * \brief   Attribute ConnectionList data validation state.
     **/
    NEService::eDataStateType           mConnectionListState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ConnectionManagerStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ConnectionManagerStub );
};

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline ConnectionManagerStub & ConnectionManagerStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   ConnectionList attribute functions
 ************************************************************************/
 
inline bool ConnectionManagerStub::isConnectionListValid( void ) const
{
    return (mConnectionListState  == NEService::eDataStateType::DataIsOK);
}

inline void ConnectionManagerStub::invalidateConnectionList( void )
{
    errorRequest( static_cast<msg_id>(NEConnectionManager::eMessageIDs::MsgId_ConnectionList), false );
}

inline void ConnectionManagerStub::notifyConnectionListUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEConnectionManager::eMessageIDs::MsgId_ConnectionList) );
}

inline const NEConnectionManager::MapConnection & ConnectionManagerStub::getConnectionList( void ) const
{
    return mConnectionList;
}

inline NEConnectionManager::MapConnection & ConnectionManagerStub::getConnectionList( void )
{
    return mConnectionList;
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_CONNECTIONMANAGERSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/ConnectionManagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
