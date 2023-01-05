//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/HelloWorldStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_HELLOWORLDSTUB_HPP
#define  GENERATED_SRC_HELLOWORLDSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:08:39 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            generated/src/HelloWorldStub.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEHelloWorld.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// HelloWorldStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       HelloWorld Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 **/
class HelloWorldStub   : protected  StubBase
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
    explicit HelloWorldStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~HelloWorldStub( void ) = default;

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
// HelloWorld Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   roleName    The role name of client component that requested to print hello world
     * \see     responseHelloWorld
     **/
    virtual void requestHelloWorld( const String & roleName ) = 0;

    /**
     * \brief   Request call.
     *          Sent by client to request to shutdown service and the application. This request has no response.
     * \param   clientID    The ID of client that requests to shutdown. The ID is given by service when first time client requests to output message.
     * \param   roleName    Service client component role name
     * \note    Has no response
     **/
    virtual void requestShutdownService( unsigned int clientID, const String & roleName ) = 0;

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          The response to hello world request.
     * \param   clientInfo  The client information set by servicing component. If empty or invalid ID, the message output failed.
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( const NEHelloWorld::sConnectedClient & clientInfo );

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Broadcasts
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Broadcast call.
     *          Triggered to notify that  reached the maximum number of requests.
     * \param   maxNumber   The maximum number of reqeusts.
     **/
    virtual void broadcastReachedMaximum( int maxNumber );

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
     * \brief   Returns reference to HelloWorldStub object
     **/
    inline HelloWorldStub & self( void );

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
    HelloWorldStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( HelloWorldStub );
};

//////////////////////////////////////////////////////////////////////////
// HelloWorldStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline HelloWorldStub & HelloWorldStub::self( void )
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// HelloWorldStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_HELLOWORLDSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/HelloWorldStub.hpp file
//////////////////////////////////////////////////////////////////////////
