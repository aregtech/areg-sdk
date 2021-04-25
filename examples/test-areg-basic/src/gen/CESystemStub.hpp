//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/CESystemStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  SRC_GEN_CESYSTEMSTUB_HPP
#define  SRC_GEN_CESYSTEMSTUB_HPP
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source System.
 * Generated at     21.04.2021  18:33:57 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/CESystemStub.hpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "src/gen/NESystem.hpp"
#include "areg/src/component/CEStubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CESystemStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       System Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              The system state.
 **/
class CESystemStub   : protected  CEStubBase
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
    CESystemStub( CEComponent & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~CESystemStub( void );
    
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
// System Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute SystemState functions
 ************************************************************************/

    /**
     * \brief   Returns true if SystemState attribute is valid
     **/
    inline bool IsSystemStateValid( void ) const;
    /**
     * \brief   Invalidates SystemState attribute and
     *          sends error update notification message to clients.
     **/
    inline void InvalidateSystemState( void );
    /**
     * \brief   Force to send SystemState attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void NotifySystemStateUpdated( void );
    /**
     * \brief   Set SystemState attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of SystemState attribute to set.
     *                      The SystemState attribute description:
     *                      The system state.
     **/
    virtual void SetSystemState( const NESystem::eSystemState & newValue );
    /**
     * \brief   Returns the value of SystemState attribute.
     *          The SystemState attribute description:
     *          The system state.
     **/
    inline const NESystem::eSystemState & GetSystemState( void ) const;
    inline NESystem::eSystemState & GetSystemState( void );

//////////////////////////////////////////////////////////////////////////
// System Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Requested to power off.
     * \note    Has no response
     **/
    virtual void RequestPowerOff( void ) = 0;

    /**
     * \brief   Request call.
     *          requested to power on
     * \note    Has no response
     **/
    virtual void RequestPowerOn( void ) = 0;

    /**
     * \brief   Request call.
     *          requested to shutdown
     * \note    Has no response
     **/
    virtual void RequestShutdown( void ) = 0;

    /**
     * \brief   Request call.
     *          requested to start.
     * \note    Has no response
     **/
    virtual void RequestStart( void ) = 0;

//////////////////////////////////////////////////////////////////////////
// System Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// System Interface Broadcasts
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
     * \brief   Returns reference to CESystemStub object
     **/
    inline CESystemStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute SystemState
     *          The system state.
     **/
    NESystem::eSystemState      mSystemState;
    /**
     * \brief   Attribute SystemState data validation state.
     **/
    NEService::eDataStateType   mSystemStateState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CESystemStub( void );
    CESystemStub( const CESystemStub & /* src */ );
    const CESystemStub & operator = ( const CESystemStub & /* src */ );
};

//////////////////////////////////////////////////////////////////////////
// CESystemStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CESystemStub & CESystemStub::self( void )
{   return (*this); }

/************************************************************************
 * \brief   SystemState attribute functions
 ************************************************************************/
 
inline bool CESystemStub::IsSystemStateValid( void ) const
{   return (mSystemStateState  == NEService::DATA_OK); }

inline void CESystemStub::InvalidateSystemState( void )
{   ErrorRequest( static_cast<unsigned int>(NESystem::MSG_ID_SystemState), false ); }

inline void CESystemStub::NotifySystemStateUpdated( void )
{   SendNotification( NESystem::MSG_ID_SystemState );   }

inline const NESystem::eSystemState & CESystemStub::GetSystemState( void ) const
{   return mSystemState; }

inline NESystem::eSystemState & CESystemStub::GetSystemState( void )
{   return mSystemState; }

//////////////////////////////////////////////////////////////////////////
// CESystemStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // SRC_GEN_CESYSTEMSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/CESystemStub.hpp file
//////////////////////////////////////////////////////////////////////////
