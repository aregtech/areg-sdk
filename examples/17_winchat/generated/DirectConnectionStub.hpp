//////////////////////////////////////////////////////////////////////////
// Begin generate generated/DirectConnectionStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_DIRECTCONNECTIONSTUB_HPP
#define  GENERATED_DIRECTCONNECTIONSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:04 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 *
 * \file            generated/DirectConnectionStub.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/NEDirectConnection.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// DirectConnectionStub class declaration
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
class DirectConnectionStub   : protected  StubBase
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
    explicit DirectConnectionStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~DirectConnectionStub( void ) = default;

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
// DirectConnection Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute InitiatedConnections functions
 ************************************************************************/

    /**
     * \brief   Returns true if InitiatedConnections attribute is valid
     **/
    inline bool isInitiatedConnectionsValid( void ) const;
    /**
     * \brief   Invalidates InitiatedConnections attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateInitiatedConnections( void );
    /**
     * \brief   Force to send InitiatedConnections attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyInitiatedConnectionsUpdated( void );
    /**
     * \brief   Set InitiatedConnections attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of InitiatedConnections attribute to set.
     *                      The InitiatedConnections attribute description:
     *                      The map of initiated connections and list of participants.
     **/
    virtual void setInitiatedConnections( const NEDirectConnection::MapParticipants & newValue );
    /**
     * \brief   Returns the value of InitiatedConnections attribute.
     *          The InitiatedConnections attribute description:
     *          The map of initiated connections and list of participants.
     **/
    inline const NEDirectConnection::MapParticipants & getInitiatedConnections( void ) const;
    inline NEDirectConnection::MapParticipants & getInitiatedConnections( void );

//////////////////////////////////////////////////////////////////////////
// DirectConnection Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to setup the direct connection to chat room initiator.
     * \param   initiator           The initiator to setup connection
     * \param   listParticipants    The list of chat-room participants
     * \see     responseConnectoinSetup
     **/
    virtual void requestConnectoinSetup( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants ) = 0;

    /**
     * \brief   Request call.
     *          The request to add a participant in the direct chat-room.
     * \param   initiator           The initiator to add to chat-room
     * \param   listParticipants    List of participants
     * \see     responseAddParticipant
     **/
    virtual void requestAddParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants ) = 0;

    /**
     * \brief   Request call.
     *          Request to remove initiator from chat-room
     * \param   initiator           The initiator to remove from chat-room
     * \param   listParticipants    List of chat-room participants
     * \see     responseRemoveParticipant
     **/
    virtual void requestRemoveParticipant( const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants ) = 0;

    /**
     * \brief   Request call.
     *          Request to close chat-room. There is no response expected.
     * \param   initiator   The initiator to close chat-room.
     * \note    Has no response
     **/
    virtual void requestCloseConnection( const NEDirectConnection::sInitiator & initiator ) = 0;

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
     * \see     requestConnectoinSetup
     **/
    virtual void responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants );

    /**
     * \brief   Response call.
     *          The response to add initiator to chat room
     * \param   succeeded           Flag, indicating whether operation succeeded.
     * \param   listParticipants    New list of participants
     * \see     requestAddParticipant
     **/
    virtual void responseAddParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants );

    /**
     * \brief   Response call.
     *          Response to remove initiator from chat-room
     * \param   succeeded           Flag, indicating whther operation succeeded or not.
     * \param   listParticipants    New lsit of chat-room participants.
     * \see     requestRemoveParticipant
     **/
    virtual void responseRemoveParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants );

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
     * \brief   Returns reference to DirectConnectionStub object
     **/
    inline DirectConnectionStub & self( void );

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
    DirectConnectionStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DirectConnectionStub );
};

//////////////////////////////////////////////////////////////////////////
// DirectConnectionStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline DirectConnectionStub & DirectConnectionStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   InitiatedConnections attribute functions
 ************************************************************************/
 
inline bool DirectConnectionStub::isInitiatedConnectionsValid( void ) const
{
    return (mInitiatedConnectionsState  == NEService::eDataStateType::DataIsOK);
}

inline void DirectConnectionStub::invalidateInitiatedConnections( void )
{
    errorRequest( static_cast<msg_id>(NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections), false );
}

inline void DirectConnectionStub::notifyInitiatedConnectionsUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections) );
}

inline const NEDirectConnection::MapParticipants & DirectConnectionStub::getInitiatedConnections( void ) const
{
    return mInitiatedConnections;
}

inline NEDirectConnection::MapParticipants & DirectConnectionStub::getInitiatedConnections( void )
{
    return mInitiatedConnections;
}

//////////////////////////////////////////////////////////////////////////
// DirectConnectionStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_DIRECTCONNECTIONSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/DirectConnectionStub.hpp file
//////////////////////////////////////////////////////////////////////////
