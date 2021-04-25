//////////////////////////////////////////////////////////////////////////
// Begin generate gen/shared/CEDirectConnectionStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GEN_SHARED_CEDIRECTCONNECTIONSTUB_HPP
#define  GEN_SHARED_CEDIRECTCONNECTIONSTUB_HPP
/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source DirectConnection.
 * Generated at     02.09.2019  17:27:34 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/shared/CEDirectConnectionStub.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "gen/shared/NEDirectConnection.hpp"
#include "areg/core/component/CEStubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       DirectConnection Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              Direct Connection service interface. It is used to send connection messages 
 *              to distributed applications. As soon as the distributed applications are connected, 
 *              each application has one instance of implemented service where 
 *              the role name is a nick name of a registered application.
 **/
class CEDirectConnectionStub   : protected  CEStubBase
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
    CEDirectConnectionStub( CEComponent & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEDirectConnectionStub( void );
    
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
// DirectConnection Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute InitiatedConnections functions
 ************************************************************************/

    /**
     * \brief   Returns true if InitiatedConnections attribute is valid
     **/
    inline bool IsInitiatedConnectionsValid( void ) const;
    /**
     * \brief   Invalidates InitiatedConnections attribute and
     *          sends error update notification message to clients.
     **/
    inline void InvalidateInitiatedConnections( void );
    /**
     * \brief   Force to send InitiatedConnections attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void NotifyInitiatedConnectionsUpdated( void );
    /**
     * \brief   Set InitiatedConnections attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of InitiatedConnections attribute to set.
     *                      The InitiatedConnections attribute description:
     *                      The map of initiated connections and list of participants.
     **/
    virtual void SetInitiatedConnections( const NEDirectConnection::MapParticipants & newValue );
    /**
     * \brief   Returns the value of InitiatedConnections attribute.
     *          The InitiatedConnections attribute description:
     *          The map of initiated connections and list of participants.
     **/
    inline const NEDirectConnection::MapParticipants & GetInitiatedConnections( void ) const;
    inline NEDirectConnection::MapParticipants & GetInitiatedConnections( void );

//////////////////////////////////////////////////////////////////////////
// DirectConnection Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to setup the direct connection to chat room initiator.
     * \param   initiator           The initiator to setup connection
     * \param   listParticipants    The list of chat-room participants
     * \see     ResponseConnectoinSetup
     **/
    virtual void RequestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants ) = 0;

    /**
     * \brief   Request call.
     *          The request to add a participant in the direct chat-room.
     * \param   initiator           The initiator to add to chat-room
     * \param   listParticipants    List of participants
     * \see     ResponseAddParticipant
     **/
    virtual void RequestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants ) = 0;

    /**
     * \brief   Request call.
     *          Request to remove initiator from chat-room
     * \param   initiator           The initiator to remove from chat-room
     * \param   listParticipants    List of chat-room participants
     * \see     ResponseRemoveParticipant
     **/
    virtual void RequestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants ) = 0;

    /**
     * \brief   Request call.
     *          Request to close chat-room. There is no response expected.
     * \param   initiator   The initiator to close chat-room.
     * \note    Has no response
     **/
    virtual void RequestCloseConnection( const NEDirectConnection::sInitiator & initiator ) = 0;

//////////////////////////////////////////////////////////////////////////
// DirectConnection Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          The response of connection setup
     * \param   succeeded           Flag, indicating whether the connection setup succeeded or not.
     * \param   target              The targeted participant to include in chat-room
     * \param   initiator           The chat-room initiator
     * \param   listParticipants    List of chat-room participants
     * \see     RequestConnectoinSetup
     **/
    virtual void ResponseConnectoinSetup( const bool & succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );

    /**
     * \brief   Response call.
     *          The response to add initiator to chat room
     * \param   succeeded           Flag, indicating whether operation succeeded.
     * \param   listParticipants    New list of participants
     * \see     RequestAddParticipant
     **/
    virtual void ResponseAddParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants );

    /**
     * \brief   Response call.
     *          Response to remove initiator from chat-room
     * \param   succeeded           Flag, indicating whther operation succeeded or not.
     * \param   listParticipants    New lsit of chat-room participants.
     * \see     RequestRemoveParticipant
     **/
    virtual void ResponseRemoveParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants );

//////////////////////////////////////////////////////////////////////////
// DirectConnection Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

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
     * \brief   Returns reference to CEDirectConnectionStub object
     **/
    inline CEDirectConnectionStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute InitiatedConnections
     *          The map of initiated connections and list of participants.
     **/
    NEDirectConnection::MapParticipants mInitiatedConnections;
    /**
     * \brief   Attribute InitiatedConnections data validation state.
     **/
    NEService::eDataStateType           mInitiatedConnectionsState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEDirectConnectionStub( void );
    CEDirectConnectionStub( const CEDirectConnectionStub & /* src */ );
    const CEDirectConnectionStub & operator = ( const CEDirectConnectionStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEDirectConnectionStub & CEDirectConnectionStub::self( void )
{   return (*this); }

/************************************************************************
 * \brief   InitiatedConnections attribute functions
 ************************************************************************/
 
inline bool CEDirectConnectionStub::IsInitiatedConnectionsValid( void ) const
{   return (mInitiatedConnectionsState  == NEService::DATA_OK); }

inline void CEDirectConnectionStub::InvalidateInitiatedConnections( void )
{   ErrorRequest( static_cast<const unsigned int &>(NEDirectConnection::MSG_ID_InitiatedConnections), false ); }

inline void CEDirectConnectionStub::NotifyInitiatedConnectionsUpdated( void )
{   SendNotification( NEDirectConnection::MSG_ID_InitiatedConnections );   }

inline const NEDirectConnection::MapParticipants & CEDirectConnectionStub::GetInitiatedConnections( void ) const
{   return mInitiatedConnections; }

inline NEDirectConnection::MapParticipants & CEDirectConnectionStub::GetInitiatedConnections( void )
{   return mInitiatedConnections; }

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GEN_SHARED_CEDIRECTCONNECTIONSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate gen/shared/CEDirectConnectionStub.hpp file
//////////////////////////////////////////////////////////////////////////
