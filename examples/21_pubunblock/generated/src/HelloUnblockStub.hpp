//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/HelloUnblockStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_HELLOUNBLOCKSTUB_HPP
#define  GENERATED_SRC_HELLOUNBLOCKSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:56 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloUnblock.
 *
 * \file            generated/src/HelloUnblockStub.hpp
 * \ingroup         HelloUnblock Service Interface
 * \brief           This is an automatic generated code of HelloUnblock
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEHelloUnblock.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// HelloUnblockStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       HelloUnblock Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              In this example the client sends a request each 200 ms and the service replies each 500,
 *              so that when the client sends next request, there is always a pending request on the service.
 *              To process the request and make correct reply, the service manually unblocks the request
 *              and when timeout is expired, sends the respons.
 *              The service is stopped and the application is shutdown when there is no more pending request.
 **/
class HelloUnblockStub   : protected  StubBase
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
    explicit HelloUnblockStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~HelloUnblockStub( void ) = default;

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
// HelloUnblock Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute HelloServiceState functions
 ************************************************************************/

    /**
     * \brief   Returns true if HelloServiceState attribute is valid
     **/
    inline bool isHelloServiceStateValid( void ) const;
    /**
     * \brief   Invalidates HelloServiceState attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateHelloServiceState( void );
    /**
     * \brief   Force to send HelloServiceState attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyHelloServiceStateUpdated( void );
    /**
     * \brief   Set HelloServiceState attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of HelloServiceState attribute to set.
     *                      The HelloServiceState attribute description:
     *                      The state of the service.
     **/
    virtual void setHelloServiceState( const NEHelloUnblock::eServiceState & newValue );
    /**
     * \brief   Returns the value of HelloServiceState attribute.
     *          The HelloServiceState attribute description:
     *          The state of the service.
     **/
    inline const NEHelloUnblock::eServiceState & getHelloServiceState( void ) const;
    inline NEHelloUnblock::eServiceState & getHelloServiceState( void );

//////////////////////////////////////////////////////////////////////////
// HelloUnblock Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to assign an ID to the client used to to call unblock request.
     * \see     responseIdentifier
     **/
    virtual void requestIdentifier( void ) = 0;

    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   clientId    The given ID of the client. Should be 0 if unknown
     * \param   seqNr       The sequence number generated by the client. On each request the client increase the sequence number
     *          and stops sending request when reach the maximum.
     * \see     responseHelloUnblock
     **/
    virtual void requestHelloUblock( unsigned int clientId, unsigned int seqNr ) = 0;

//////////////////////////////////////////////////////////////////////////
// HelloUnblock Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          Sent to set ID for client.
     * \param   clientId    Generated ID for the client used when send requst to unblock.
     * \see     requestIdentifier
     **/
    virtual void responseIdentifier( unsigned int clientId );

    /**
     * \brief   Response call.
     *          The response to hello world request.
     * \param   clientId    The ID of the client to send the response. Never is 0.
     * \param   seqNr       The sequence number created by the client. On reply the service sends the sequence number so that
     *          the cilent can check that all sequences exist.
     * \see     requestHelloUblock
     **/
    virtual void responseHelloUnblock( unsigned int clientId, unsigned int seqNr );

//////////////////////////////////////////////////////////////////////////
// HelloUnblock Interface Broadcasts
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
     * \brief   Returns reference to HelloUnblockStub object
     **/
    inline HelloUnblockStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute HelloServiceState
     *          The state of the service.
     **/
    NEHelloUnblock::eServiceState   mHelloServiceState;
    /**
     * \brief   Attribute HelloServiceState data validation state.
     **/
    NEService::eDataStateType       mHelloServiceStateState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    HelloUnblockStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloUnblockStub );
};

//////////////////////////////////////////////////////////////////////////
// HelloUnblockStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline HelloUnblockStub & HelloUnblockStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   HelloServiceState attribute functions
 ************************************************************************/
 
inline bool HelloUnblockStub::isHelloServiceStateValid( void ) const
{
    return (mHelloServiceStateState  == NEService::eDataStateType::DataIsOK);
}

inline void HelloUnblockStub::invalidateHelloServiceState( void )
{
    errorRequest( static_cast<msg_id>(NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState), false );
}

inline void HelloUnblockStub::notifyHelloServiceStateUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState) );
}

inline const NEHelloUnblock::eServiceState & HelloUnblockStub::getHelloServiceState( void ) const
{
    return mHelloServiceState;
}

inline NEHelloUnblock::eServiceState & HelloUnblockStub::getHelloServiceState( void )
{
    return mHelloServiceState;
}

//////////////////////////////////////////////////////////////////////////
// HelloUnblockStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_HELLOUNBLOCKSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/HelloUnblockStub.hpp file
//////////////////////////////////////////////////////////////////////////
