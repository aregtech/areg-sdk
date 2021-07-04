//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/LocalHelloWorldStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHAREIPCMIX_SRC_LOCALHELLOWORLDSTUB_HPP
#define  SHAREIPCMIX_SRC_LOCALHELLOWORLDSTUB_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 * Generated at     04.07.2021  04:21:23 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/LocalHelloWorldStub.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "shareipcmix/src/NELocalHelloWorld.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       LocalHelloWorld Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              This interface serves only local components and can be duplicated in other processes.
 *              		
 **/
class LocalHelloWorldStub   : protected  StubBase
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
    LocalHelloWorldStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~LocalHelloWorldStub( void );
    
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
// LocalHelloWorld Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute ConnectedClients functions
 ************************************************************************/

    /**
     * \brief   Returns true if ConnectedClients attribute is valid
     **/
    inline bool isConnectedClientsValid( void ) const;
    /**
     * \brief   Invalidates ConnectedClients attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateConnectedClients( void );
    /**
     * \brief   Force to send ConnectedClients attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyConnectedClientsUpdated( void );
    /**
     * \brief   Set ConnectedClients attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of ConnectedClients attribute to set.
     *                      The ConnectedClients attribute description:
     *                      The list of connected clients. Updated each time when new client requests to output Hello World message.
     **/
    virtual void setConnectedClients( const NELocalHelloWorld::ConnectionList & newValue );
    /**
     * \brief   Returns the value of ConnectedClients attribute.
     *          The ConnectedClients attribute description:
     *          The list of connected clients. Updated each time when new client requests to output Hello World message.
     **/
    inline const NELocalHelloWorld::ConnectionList & getConnectedClients( void ) const;
    inline NELocalHelloWorld::ConnectionList & getConnectedClients( void );

/************************************************************************
 * Attribute RemainOutput functions
 ************************************************************************/

    /**
     * \brief   Returns true if RemainOutput attribute is valid
     **/
    inline bool isRemainOutputValid( void ) const;
    /**
     * \brief   Invalidates RemainOutput attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateRemainOutput( void );
    /**
     * \brief   Force to send RemainOutput attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyRemainOutputUpdated( void );
    /**
     * \brief   Set RemainOutput attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of RemainOutput attribute to set.
     *                      The RemainOutput attribute description:
     *                      Remaining number of outputs to print Hello World.
     **/
    virtual void setRemainOutput( const short & newValue );
    /**
     * \brief   Returns the value of RemainOutput attribute.
     *          The RemainOutput attribute description:
     *          Remaining number of outputs to print Hello World.
     **/
    inline const short & getRemainOutput( void ) const;
    inline short & getRemainOutput( void );

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorld Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to print hello world
     * \param   roleName    The role name of client component that requested to print hello world
     * \param   addMessage  Additional message to output. Can be empty.
     *          Has default value: ""
     * \see     responseHelloWorld
     **/
    virtual void requestHelloWorld( const String & roleName, const String & addMessage = "" ) = 0;

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorld Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          The response to hello world request.
     * \param   clientInfo  The client information set by servicing component. If empty or invalid ID, the message output failed.
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo );

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorld Interface Broadcasts
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
     * \brief   Returns reference to LocalHelloWorldStub object
     **/
    inline LocalHelloWorldStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute ConnectedClients
     *          The list of connected clients. Updated each time when new client requests to output Hello World message.
     **/
    NELocalHelloWorld::ConnectionList   mConnectedClients;
    /**
     * \brief   Attribute ConnectedClients data validation state.
     **/
    NEService::eDataStateType           mConnectedClientsState;

    /**
     * \brief   Attribute RemainOutput
     *          Remaining number of outputs to print Hello World.
     **/
    short                               mRemainOutput;
    /**
     * \brief   Attribute RemainOutput data validation state.
     **/
    NEService::eDataStateType           mRemainOutputState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LocalHelloWorldStub( void );
    LocalHelloWorldStub( const LocalHelloWorldStub & /* src */ );
    const LocalHelloWorldStub & operator = ( const LocalHelloWorldStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline LocalHelloWorldStub & LocalHelloWorldStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   ConnectedClients attribute functions
 ************************************************************************/
 
inline bool LocalHelloWorldStub::isConnectedClientsValid( void ) const
{
    return (mConnectedClientsState  == NEService::DATA_OK);
}

inline void LocalHelloWorldStub::invalidateConnectedClients( void )
{
    errorRequest( static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_ConnectedClients), false );
}

inline void LocalHelloWorldStub::notifyConnectedClientsUpdated( void )
{
    sendNotification( NELocalHelloWorld::MSG_ID_ConnectedClients );
}

inline const NELocalHelloWorld::ConnectionList & LocalHelloWorldStub::getConnectedClients( void ) const
{
    return mConnectedClients;
}

inline NELocalHelloWorld::ConnectionList & LocalHelloWorldStub::getConnectedClients( void )
{
    return mConnectedClients;
}

/************************************************************************
 * \brief   RemainOutput attribute functions
 ************************************************************************/
 
inline bool LocalHelloWorldStub::isRemainOutputValid( void ) const
{
    return (mRemainOutputState  == NEService::DATA_OK);
}

inline void LocalHelloWorldStub::invalidateRemainOutput( void )
{
    errorRequest( static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_RemainOutput), false );
}

inline void LocalHelloWorldStub::notifyRemainOutputUpdated( void )
{
    sendNotification( NELocalHelloWorld::MSG_ID_RemainOutput );
}

inline const short & LocalHelloWorldStub::getRemainOutput( void ) const
{
    return mRemainOutput;
}

inline short & LocalHelloWorldStub::getRemainOutput( void )
{
    return mRemainOutput;
}

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHAREIPCMIX_SRC_LOCALHELLOWORLDSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/LocalHelloWorldStub.hpp file
//////////////////////////////////////////////////////////////////////////
