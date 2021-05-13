//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/MessageOutputStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_MESSAGEOUTPUTSTUB_HPP
#define  SRC_GEN_MESSAGEOUTPUTSTUB_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source MessageOutput.
 * Generated at     12.05.2021  16:41:12 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/MessageOutputStub.hpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "src/gen/NEMessageOutput.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// MessageOutputStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       MessageOutput Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              The message output service to interact with user.
 **/
class MessageOutputStub   : protected  StubBase
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
    MessageOutputStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~MessageOutputStub( void );
    
//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Unlocks and cancels all requests, excepts those which were manually unlocked.
     *          This call will send cancel error message (NEService::RESULT_REQUEST_CANCELED) to all clients waiting for response.
     **/
    void unlockAllRequests( void );

//////////////////////////////////////////////////////////////////////////
// Start Service Interface operations / attributes and overrides declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MessageOutput Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute LastMessage functions
 ************************************************************************/

    /**
     * \brief   Returns true if LastMessage attribute is valid
     **/
    inline bool isLastMessageValid( void ) const;
    /**
     * \brief   Invalidates LastMessage attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateLastMessage( void );
    /**
     * \brief   Force to send LastMessage attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyLastMessageUpdated( void );
    /**
     * \brief   Set LastMessage attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of LastMessage attribute to set.
     *                      The LastMessage attribute description:
     *                      The last message to output on console.
     **/
    virtual void setLastMessage( const String & newValue );
    /**
     * \brief   Returns the value of LastMessage attribute.
     *          The LastMessage attribute description:
     *          The last message to output on console.
     **/
    inline const String & getLastMessage( void ) const;
    inline String & getLastMessage( void );

//////////////////////////////////////////////////////////////////////////
// MessageOutput Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Sent to service to output message on console.
     * \param   msgOutput   The message requested to output on console.
     * \param   dateTime    The date and the time when it was requested.
     * \note    Has no response
     **/
    virtual void requestOutputMessage( const String & msgOutput, const DateTime & dateTime ) = 0;

//////////////////////////////////////////////////////////////////////////
// MessageOutput Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// MessageOutput Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Broadcast call.
     *          Informs the action requested by a user
     * \param   userAction  The action requested by user.
     **/
    virtual void broadcastActionRequest( NEMessageOutput::eRequestedActons userAction );

    /**
     * \brief   Broadcast call.
     *          The input message from the user.
     * \param   msgInput    The input message from the user.
     **/
    virtual void broadcastInputMessage( const String & msgInput );

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
     * \brief   Returns implemented version of service interface.
     **/
    virtual const Version & getImplVersion( void ) const;

    /**
     * \brief   Sends update notification message to all clients. 
     *          This method can be called manually to send update 
     *          notification message after updating attribute value.
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void sendNotification( unsigned int msgId );

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
    virtual void errorRequest( unsigned int msgId, bool msgCancel );

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
    virtual void startupServiceInterface( Component & holder );

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder );

/************************************************************************/
// StubBase overrides. Protected methods 
/************************************************************************/

    /**
     * \brief   Returns number of requests of Service Interface
     **/
    virtual unsigned int getNumberOfRequests(void) const;

    /**
     * \brief   Returns number of responses of Service Interface
     **/
    virtual unsigned int getNumberOfResponses( void ) const;

    /**
     * \brief   Returns number of attributes of Service Interface
     **/
    virtual unsigned int getNumberOfAttributes( void ) const;

    /**
     * \brief   Returns pointer of array of requests IDs of Service Interface
     **/
    virtual const unsigned int * getRequestIds(void) const;

    /**
     * \brief   Returns pointer of array of response IDs of Service Interface
     **/
    virtual const unsigned int * getResponseIds(void) const;

    /**
     * \brief   Returns pointer of array of attribute IDs of Service Interface
     **/
    virtual const unsigned int * getAttributeIds(void) const;

    /**
     * \brief   Creates Response event object to pass of client.
     * \param   proxy   The address of proxy object to send response event
     * \param   msgId   The message ID to send to client
     * \param   result  The result of message
     * \param   data    The buffer of data to send to client. Can be Invalid buffer.
     * \return  Returns valid pointer to Response event object
     **/
    virtual ResponseEvent * createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const;

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
    virtual void processRequestEvent( ServiceRequestEvent & eventElem );

    /**
     * \brief   Triggered to process attribute update notification event.
     *          Override method to process request to get attribute value and
     *          process notification request of attribute update.
     * \param   eventElem   Service Request Event object, contains attribute ID.
     **/
    virtual void processAttributeEvent( ServiceRequestEvent & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to MessageOutputStub object
     **/
    inline MessageOutputStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute LastMessage
     *          The last message to output on console.
     **/
    String                      mLastMessage;
    /**
     * \brief   Attribute LastMessage data validation state.
     **/
    NEService::eDataStateType   mLastMessageState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    MessageOutputStub( void );
    MessageOutputStub( const MessageOutputStub & /* src */ );
    const MessageOutputStub & operator = ( const MessageOutputStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// MessageOutputStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline MessageOutputStub & MessageOutputStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   LastMessage attribute functions
 ************************************************************************/
 
inline bool MessageOutputStub::isLastMessageValid( void ) const
{
    return (mLastMessageState  == NEService::DATA_OK);
}

inline void MessageOutputStub::invalidateLastMessage( void )
{
    errorRequest( static_cast<unsigned int>(NEMessageOutput::MSG_ID_LastMessage), false );
}

inline void MessageOutputStub::notifyLastMessageUpdated( void )
{
    sendNotification( NEMessageOutput::MSG_ID_LastMessage );
}

inline const String & MessageOutputStub::getLastMessage( void ) const
{
    return mLastMessage;
}

inline String & MessageOutputStub::getLastMessage( void )
{
    return mLastMessage;
}

//////////////////////////////////////////////////////////////////////////
// MessageOutputStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SRC_GEN_MESSAGEOUTPUTSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/MessageOutputStub.hpp file
//////////////////////////////////////////////////////////////////////////
