//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/RemoteRegistryStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SHAREIPCMIX_SRC_REMOTEREGISTRYSTUB_HPP
#define  SHAREIPCMIX_SRC_REMOTEREGISTRYSTUB_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 * Generated at     11.06.2021  21:11:05 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/RemoteRegistryStub.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "shareipcmix/src/NERemoteRegistry.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       RemoteRegistry Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              Simple Service Interface to demonstrate working features of AREG SDK.
 *              This interface demonstrates how to use multiple remote services in the system.
 *              Each service is unique in the network.
 *              This service is used by each remote servicing component to register clients.
 *              Before using services, the clients should be registered.
 **/
class RemoteRegistryStub   : protected  StubBase
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
    RemoteRegistryStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~RemoteRegistryStub( void );
    
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
// RemoteRegistry Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute RegistryList functions
 ************************************************************************/

    /**
     * \brief   Returns true if RegistryList attribute is valid
     **/
    inline bool isRegistryListValid( void ) const;
    /**
     * \brief   Invalidates RegistryList attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateRegistryList( void );
    /**
     * \brief   Force to send RegistryList attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyRegistryListUpdated( void );
    /**
     * \brief   Set RegistryList attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of RegistryList attribute to set.
     *                      The RegistryList attribute description:
     *                      List of registered clients.
     **/
    virtual void setRegistryList( const NERemoteRegistry::ListRegistry & newValue );
    /**
     * \brief   Returns the value of RegistryList attribute.
     *          The RegistryList attribute description:
     *          List of registered clients.
     **/
    inline const NERemoteRegistry::ListRegistry & getRegistryList( void ) const;
    inline NERemoteRegistry::ListRegistry & getRegistryList( void );

/************************************************************************
 * Attribute RemainOutputs functions
 ************************************************************************/

    /**
     * \brief   Returns true if RemainOutputs attribute is valid
     **/
    inline bool isRemainOutputsValid( void ) const;
    /**
     * \brief   Invalidates RemainOutputs attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateRemainOutputs( void );
    /**
     * \brief   Force to send RemainOutputs attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyRemainOutputsUpdated( void );
    /**
     * \brief   Set RemainOutputs attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of RemainOutputs attribute to set.
     *                      The RemainOutputs attribute description:
     *                      Remaining outputs before reaching maximum.
     **/
    virtual void setRemainOutputs( const unsigned int & newValue );
    /**
     * \brief   Returns the value of RemainOutputs attribute.
     *          The RemainOutputs attribute description:
     *          Remaining outputs before reaching maximum.
     **/
    inline const unsigned int & getRemainOutputs( void ) const;
    inline unsigned int & getRemainOutputs( void );

//////////////////////////////////////////////////////////////////////////
// RemoteRegistry Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Call to register client. Each client should be registered before starting communication.
     * \param   name    The name of the client.
     * \param   service The service address of the client.
     * \param   thread  The thread name where client is running. Required to provide uniqueness.
     * \param   process The name of process. Optional parameter, used to make output in logs.
     * \see     responseRegister
     **/
    virtual void requestRegister( const String & name, const ServiceAddress & service, const String & thread, const String & process ) = 0;

    /**
     * \brief   Request call.
     *          Sent to unregister connected client.
     * \param   client  The client registration object indicating the unregistered client.
     * \note    Has no response
     **/
    virtual void requestUnregister( const NERemoteRegistry::sClientRegister & client ) = 0;

    /**
     * \brief   Request call.
     *          Outputs message on console. If additional message is not empty, outputs the additional message as well.
     * \param   clientID    The ID of registered client to make message output
     * \param   addMessage  The additional message to output. Ignored if empty.
     * \see     responseHelloWorld
     **/
    virtual void requestHelloWorld( unsigned int clientID, const String & addMessage ) = 0;

//////////////////////////////////////////////////////////////////////////
// RemoteRegistry Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          Sent when the client is registered. Each registered client has unique ID.
     * \param   client  The client registration object that contains unique ID.
     * \see     requestRegister
     **/
    virtual void responseRegister( const NERemoteRegistry::sClientRegister & client );

    /**
     * \brief   Response call.
     *          Called as reply to the request to output message.
     * \param   clientID    Indicates the ID client that made message output
     * \see     requestHelloWorld
     **/
    virtual void responseHelloWorld( unsigned int clientID );

//////////////////////////////////////////////////////////////////////////
// RemoteRegistry Interface Broadcasts
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

    /**
     * \brief   Overwrite method to create remote service request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns NULL.
     **/
    virtual RemoteRequestEvent * createRemoteRequestEvent( const IEInStream & stream ) const;

    /**
     * \brief   Overwrite method to create remote notify request event from streaming object for 
     *          further dispatching by stub.
     * \param   stream  Streaming object, which contains event data.
     * \return  If operation succeeds, returns valid pointer to Service Request event object.
     *          Otherwise, it returns NULL.
     **/
    virtual RemoteNotifyRequestEvent * createRemoteNotifyRequestEvent( const IEInStream & stream ) const;

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
     * \brief   Returns reference to RemoteRegistryStub object
     **/
    inline RemoteRegistryStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute RegistryList
     *          List of registered clients.
     **/
    NERemoteRegistry::ListRegistry  mRegistryList;
    /**
     * \brief   Attribute RegistryList data validation state.
     **/
    NEService::eDataStateType       mRegistryListState;

    /**
     * \brief   Attribute RemainOutputs
     *          Remaining outputs before reaching maximum.
     **/
    unsigned int                    mRemainOutputs;
    /**
     * \brief   Attribute RemainOutputs data validation state.
     **/
    NEService::eDataStateType       mRemainOutputsState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    RemoteRegistryStub( void );
    RemoteRegistryStub( const RemoteRegistryStub & /* src */ );
    const RemoteRegistryStub & operator = ( const RemoteRegistryStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline RemoteRegistryStub & RemoteRegistryStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   RegistryList attribute functions
 ************************************************************************/
 
inline bool RemoteRegistryStub::isRegistryListValid( void ) const
{
    return (mRegistryListState  == NEService::DATA_OK);
}

inline void RemoteRegistryStub::invalidateRegistryList( void )
{
    errorRequest( static_cast<unsigned int>(NERemoteRegistry::MSG_ID_RegistryList), false );
}

inline void RemoteRegistryStub::notifyRegistryListUpdated( void )
{
    sendNotification( NERemoteRegistry::MSG_ID_RegistryList );
}

inline const NERemoteRegistry::ListRegistry & RemoteRegistryStub::getRegistryList( void ) const
{
    return mRegistryList;
}

inline NERemoteRegistry::ListRegistry & RemoteRegistryStub::getRegistryList( void )
{
    return mRegistryList;
}

/************************************************************************
 * \brief   RemainOutputs attribute functions
 ************************************************************************/
 
inline bool RemoteRegistryStub::isRemainOutputsValid( void ) const
{
    return (mRemainOutputsState  == NEService::DATA_OK);
}

inline void RemoteRegistryStub::invalidateRemainOutputs( void )
{
    errorRequest( static_cast<unsigned int>(NERemoteRegistry::MSG_ID_RemainOutputs), false );
}

inline void RemoteRegistryStub::notifyRemainOutputsUpdated( void )
{
    sendNotification( NERemoteRegistry::MSG_ID_RemainOutputs );
}

inline const unsigned int & RemoteRegistryStub::getRemainOutputs( void ) const
{
    return mRemainOutputs;
}

inline unsigned int & RemoteRegistryStub::getRemainOutputs( void )
{
    return mRemainOutputs;
}

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SHAREIPCMIX_SRC_REMOTEREGISTRYSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/RemoteRegistryStub.hpp file
//////////////////////////////////////////////////////////////////////////
