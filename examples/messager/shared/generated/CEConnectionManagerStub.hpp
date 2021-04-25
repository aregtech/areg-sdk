//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/CEConnectionManagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_CECONNECTIONMANAGERSTUB_HPP
#define  SHARED_GENERATED_CECONNECTIONMANAGERSTUB_HPP
/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source ConnectionManager.
 * Generated at     03.09.2019  02:48:08 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/CEConnectionManagerStub.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/NEConnectionManager.hpp"
#include "areg/component/CEStubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       ConnectionManager Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              The connection manager to register each application with client
 **/
class CEConnectionManagerStub   : protected  CEStubBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes Stub by given component object, which should be already instantiated.
     * \param   masterComp  The master component object, which is initializing service Stub.
     * \note    Before constructor is called, the instance of CEComponent must be already initialized.
     **/
    CEConnectionManagerStub( CEComponent & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEConnectionManagerStub( void );
    
//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Unlocks and cancels all requests, excepts those which were manually unlocked.
     *          This call will send cancel error message (NEService::RESULT_REQUEST_CANCELED) to all clients waiting for response.
     **/
    void UnlockAllRequests( void );

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
    inline bool IsConnectionListValid( void ) const;
    /**
     * \brief   Invalidates ConnectionList attribute and
     *          sends error update notification message to clients.
     **/
    inline void InvalidateConnectionList( void );
    /**
     * \brief   Force to send ConnectionList attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void NotifyConnectionListUpdated( void );
    /**
     * \brief   Set ConnectionList attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of ConnectionList attribute to set.
     *                      The ConnectionList attribute description:
     *                      The map of all active connections
     **/
    virtual void SetConnectionList( const NEConnectionManager::MapConnection & newValue );
    /**
     * \brief   Returns the value of ConnectionList attribute.
     *          The ConnectionList attribute description:
     *          The map of all active connections
     **/
    inline const NEConnectionManager::MapConnection & GetConnectionList( void ) const;
    inline NEConnectionManager::MapConnection & GetConnectionList( void );

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          The request to connect to system
     * \param   nickName    The nick name of connecting client service
     * \param   dateTime    The connecting client connection timestamp in UTC
     * \see     ResponseConnect
     **/
    virtual void RequestConnet( const CEString & nickName, const CEDateTime & dateTime ) = 0;

    /**
     * \brief   Request call.
     *          Request to register direct connection service.
     * \param   nickName        The nick name of connection used in service
     * \param   cookie          The cookie value to set
     * \param   connectCookie   The cookie to use for registration. 
     * \param   dateRegister    The connection registration timestamp
     * \see     ResponseRegisterConnection
     **/
    virtual void RequestRegisterConnection( const CEString & nickName, const unsigned int & cookie, const unsigned int & connectCookie, const CEDateTime & dateRegister ) = 0;

    /**
     * \brief   Request call.
     *          Request to disconnect.
     * \param   nickName    The nick name of client connection service
     * \param   cookie      The cookie given by connection manager. If invalid, it will search by nickName parameter
     * \param   dateTime    Disconnect request date-time
     * \note    Has no response
     **/
    virtual void RequestDiconnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime ) = 0;

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          The result of connection request.
     * \param   nickName    Client connection data, which contains nick name and connection date-time
     * \param   cookie      Connection assigned cookie for client service
     * \param   dateTime    The client service connection timestamp in UTC
     * \param   result      The connection operation result
     * \see     RequestConnet
     **/
    virtual void ResponseConnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime, const NEConnectionManager::eConnectionResult & result );

    /**
     * \brief   Response call.
     *          The result of request to register direct connection service.
     * \param   connection      The connection structure after registration.
     * \param   connectionList  The list of all servicing connections.
     * \param   success         Flag, indicating whether the request to register connection succeeded or not.
     * \see     RequestRegisterConnection
     **/
    virtual void ResponseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, const bool & success );

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Broadcast call.
     *          Notifies all client components the connection has been updated.
     * \param   updatedList The complete list of clients to send.
     **/
    virtual void BroadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList );

    /**
     * \brief   Broadcast call.
     *          Notifies that a new client has been connected.
     * \param   clientConnected New client data, which contains nick name and connected date-time
     **/
    virtual void BroadcastClientConnected( const NEConnectionManager::sConnection & clientConnected );

    /**
     * \brief   Broadcast call.
     *          Notifies a client has been disconnected.
     * \param   clientLeft  The connection data of disconnected client.
     **/
    virtual void BroadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft );

//////////////////////////////////////////////////////////////////////////
// End Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//  Operations.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CEStubBase overrides. Public methods.
/************************************************************************/

    /**
     * \brief   Returns implemented version of service interface.
     **/
    virtual const CEVersion & GetImplVersion( void ) const;

    /**
     * \brief   Sends update notification message to all clients. 
     *          This method can be called manually to send update 
     *          notification message after updating attribute value.
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void SendNotification( const unsigned int msgId );

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::RESULT_REQUEST_ERROR or NEService::RESULT_REQUEST_CANCELED, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::RESULT_INVALID.
     *          If message ID is an attribute, it should send result NEService::RESULT_DATA_INVALID
     *          and invalidate attribute data value.
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void ErrorRequest( const unsigned int msgId, const bool msgCancel );

protected:
/************************************************************************/
// CEStubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void StartupServiceInterface( CEComponent & holder );

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void ShutdownServiceIntrface ( CEComponent & holder );

/************************************************************************/
// CEStubBase overrides. Protected methods 
/************************************************************************/

    /**
     * \brief   Returns number of requests of Service Interface
     **/
    virtual const unsigned int GetNumberOfRequests(void) const;

    /**
     * \brief   Returns number of responses of Service Interface
     **/
    virtual const unsigned int GetNumberOfResponses( void ) const;

    /**
     * \brief   Returns number of attributes of Service Interface
     **/
    virtual const unsigned int GetNumberOfAttributes( void ) const;

    /**
     * \brief   Returns pointer of array of requests IDs of Service Interface
     **/
    virtual const unsigned int * GetRequestIds(void) const;

    /**
     * \brief   Returns pointer of array of response IDs of Service Interface
     **/
    virtual const unsigned int * GetResponseIds(void) const;

    /**
     * \brief   Returns pointer of array of attribute IDs of Service Interface
     **/
    virtual const unsigned int * GetAttributeIds(void) const;

    /**
     * \brief   Creates Response event object to pass of client.
     * \param   proxy   The address of proxy object to send response event
     * \param   msgId   The message ID to send to client
     * \param   result  The result of message
     * \param   data    The buffer of data to send to client. Can be Invalid buffer.
     * \return  Returns valid pointer to Response event object
     **/
    virtual CEResponseEvent * CreateResponseEvent( const CEProxyAddress & proxy, const unsigned int msgId, const NEService::eResultType result, const CEEventDataStream & data ) const;

    /**
     * \brief   Overwrite method to create remote service request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns NULL.
     **/
    virtual CERemoteRequestEvent * CreateRemoteServiceRequestEvent( const IEInStream & stream ) const;

    /**
     * \brief   Overwrite method to create remote notify request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns NULL.
     **/
    virtual CERemoteNotifyRequestEvent * CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const;

private:
/************************************************************************/
// IEComponentConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Triggered to process generic stub event.
     *          Usually should not be triggered.
     **/
    virtual void ProcessStubEvent( CEStubEvent & eventElem );

    /**
     * \brief   Triggered to process generic event.
     *          Usually should not be triggered.
     **/
    virtual void ProcessGenericEvent( CEEvent & eventElem );

    /**
     * \brief   Triggered to process service request event.
     *          Overwrite method to process every service request event.
     * \param   eventElem   Service Request Event object, contains request
     *                      call ID and parameters.
     **/
    virtual void ProcessRequestEvent( CEServiceRequestEvent & eventElem );

    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void ProcessAttributeEvent( CEServiceRequestEvent & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to CEConnectionManagerStub object
     **/
    inline CEConnectionManagerStub & self( void );

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
    CEConnectionManagerStub( void );
    CEConnectionManagerStub( const CEConnectionManagerStub & /* src */ );
    const CEConnectionManagerStub & operator = ( const CEConnectionManagerStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEConnectionManagerStub & CEConnectionManagerStub::self( void )
{   return (*this); }

/************************************************************************
 * \brief   ConnectionList attribute functions
 ************************************************************************/
 
inline bool CEConnectionManagerStub::IsConnectionListValid( void ) const
{   return (mConnectionListState  == NEService::DATA_OK); }

inline void CEConnectionManagerStub::InvalidateConnectionList( void )
{   ErrorRequest( static_cast<const unsigned int &>(NEConnectionManager::MSG_ID_ConnectionList), false ); }

inline void CEConnectionManagerStub::NotifyConnectionListUpdated( void )
{   SendNotification( NEConnectionManager::MSG_ID_ConnectionList );   }

inline const NEConnectionManager::MapConnection & CEConnectionManagerStub::GetConnectionList( void ) const
{   return mConnectionList; }

inline NEConnectionManager::MapConnection & CEConnectionManagerStub::GetConnectionList( void )
{   return mConnectionList; }

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHARED_GENERATED_CECONNECTIONMANAGERSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/CEConnectionManagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
