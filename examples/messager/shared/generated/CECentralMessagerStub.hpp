//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/CECentralMessagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHARED_GENERATED_CECENTRALMESSAGERSTUB_HPP
#define  SHARED_GENERATED_CECENTRALMESSAGERSTUB_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source CentralMessager.
 * Generated at     25.04.2021  23:50:42 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/CECentralMessagerStub.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "shared/generated/NECentralMessager.hpp"
#include "areg/src/component/CEStubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       CentralMessager Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              This service is used to receive messages from connected edge when typing or 
 *              sending messages. The service receives text message from initiator and
 *              broadcast to all clients, which have subscribed on event.
 *              It as well broadcasts own text message to all connected clients.
 **/
class CECentralMessagerStub   : protected  CEStubBase
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
    CECentralMessagerStub( CEComponent & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~CECentralMessagerStub( void );
    
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
// CentralMessager Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
//////////////////////////////////////////////////////////////////////////
// CentralMessager Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to sends message that all clients can see.
     * \param   nickName    The nick name of initiator
     * \param   cookie      Cookie given by connection manager. Should not be invalid.
     * \param   newMessage  The message sent by connected initiator
     * \param   dateTime    The timestamp create on local host of initiator
     * \note    Has no response
     **/
    virtual void RequestSendMessage( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage, const CEDateTime & dateTime ) = 0;

    /**
     * \brief   Request call.
     *          Sent each time when typing a key
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie assigned to initiator
     * \param   newMessage  The message typed.
     * \note    Has no response
     **/
    virtual void RequestKeyTyping( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage ) = 0;

//////////////////////////////////////////////////////////////////////////
// CentralMessager Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// CentralMessager Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Broadcast call.
     *          The response, sent by connection manager to notify the message typing update
     * \param   nickName    The nick name of initiator
     * \param   cookie      The cookie assigned to initiator
     * \param   newMessage  The message sent by initiator
     * \param   dateTime    The local timestamp of initiator
     **/
    virtual void BroadcastSendMessage( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage, const CEDateTime & dateTime );

    /**
     * \brief   Broadcast call.
     *          Sent each time when a client is typing a key
     * \param   nickName    The nick name of initiator
     * \param   cookie      Assigned cookie of initiator
     * \param   newMessage  The message typed by initiator
     **/
    virtual void BroadcastKeyTyping( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage );

    /**
     * \brief   Broadcast call.
     *          Server broadcasts a message to all clients.
     * \param   serverMessage   The message sent by servicing server
     * \param   dateTime        The timestamp of servicing component
     **/
    virtual void BroadcastBroadcastMessage( const CEString & serverMessage, const CEDateTime & dateTime );

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
    virtual void SendNotification( unsigned int msgId );

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
    virtual void ErrorRequest( unsigned int msgId, bool msgCancel );

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
    virtual unsigned int GetNumberOfRequests(void) const;

    /**
     * \brief   Returns number of responses of Service Interface
     **/
    virtual unsigned int GetNumberOfResponses( void ) const;

    /**
     * \brief   Returns number of attributes of Service Interface
     **/
    virtual unsigned int GetNumberOfAttributes( void ) const;

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
    virtual CEResponseEvent * CreateResponseEvent( const CEProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const CEEventDataStream & data ) const;

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
     * \brief   Returns reference to CECentralMessagerStub object
     **/
    inline CECentralMessagerStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CECentralMessagerStub( void );
    CECentralMessagerStub( const CECentralMessagerStub & /* src */ );
    const CECentralMessagerStub & operator = ( const CECentralMessagerStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CECentralMessagerStub & CECentralMessagerStub::self( void )
{   return (*this); }

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHARED_GENERATED_CECENTRALMESSAGERSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/CECentralMessagerStub.hpp file
//////////////////////////////////////////////////////////////////////////
