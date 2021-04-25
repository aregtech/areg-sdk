//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/CEMessageOutputStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_CEMESSAGEOUTPUTSTUB_HPP
#define  SRC_GEN_CEMESSAGEOUTPUTSTUB_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source MessageOutput.
 * Generated at     21.04.2021  18:33:55 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/CEMessageOutputStub.hpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "src/gen/NEMessageOutput.hpp"
#include "areg/src/component/CEStubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       MessageOutput Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              The message output service to interact with user.
 **/
class CEMessageOutputStub   : protected  CEStubBase
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
    CEMessageOutputStub( CEComponent & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEMessageOutputStub( void );
    
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
// MessageOutput Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute LastMessage functions
 ************************************************************************/

    /**
     * \brief   Returns true if LastMessage attribute is valid
     **/
    inline bool IsLastMessageValid( void ) const;
    /**
     * \brief   Invalidates LastMessage attribute and
     *          sends error update notification message to clients.
     **/
    inline void InvalidateLastMessage( void );
    /**
     * \brief   Force to send LastMessage attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void NotifyLastMessageUpdated( void );
    /**
     * \brief   Set LastMessage attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of LastMessage attribute to set.
     *                      The LastMessage attribute description:
     *                      The last message to output on console.
     **/
    virtual void SetLastMessage( const CEString & newValue );
    /**
     * \brief   Returns the value of LastMessage attribute.
     *          The LastMessage attribute description:
     *          The last message to output on console.
     **/
    inline const CEString & GetLastMessage( void ) const;
    inline CEString & GetLastMessage( void );

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
    virtual void RequestOutputMessage( const CEString & msgOutput, const CEDateTime & dateTime ) = 0;

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
    virtual void BroadcastActionRequest( const NEMessageOutput::eRequestedActons & userAction );

    /**
     * \brief   Broadcast call.
     *          The input message from the user.
     * \param   msgInput    The input message from the user.
     **/
    virtual void BroadcastInputMessage( const CEString & msgInput );

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
     * \brief   Returns reference to CEMessageOutputStub object
     **/
    inline CEMessageOutputStub & self( void );

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
    CEString                    mLastMessage;
    /**
     * \brief   Attribute LastMessage data validation state.
     **/
    NEService::eDataStateType   mLastMessageState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEMessageOutputStub( void );
    CEMessageOutputStub( const CEMessageOutputStub & /* src */ );
    const CEMessageOutputStub & operator = ( const CEMessageOutputStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CEMessageOutputStub & CEMessageOutputStub::self( void )
{   return (*this); }

/************************************************************************
 * \brief   LastMessage attribute functions
 ************************************************************************/
 
inline bool CEMessageOutputStub::IsLastMessageValid( void ) const
{   return (mLastMessageState  == NEService::DATA_OK); }

inline void CEMessageOutputStub::InvalidateLastMessage( void )
{   ErrorRequest( static_cast<unsigned int>(NEMessageOutput::MSG_ID_LastMessage), false ); }

inline void CEMessageOutputStub::NotifyLastMessageUpdated( void )
{   SendNotification( NEMessageOutput::MSG_ID_LastMessage );   }

inline const CEString & CEMessageOutputStub::GetLastMessage( void ) const
{   return mLastMessage; }

inline CEString & CEMessageOutputStub::GetLastMessage( void )
{   return mLastMessage; }

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SRC_GEN_CEMESSAGEOUTPUTSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/CEMessageOutputStub.hpp file
//////////////////////////////////////////////////////////////////////////
