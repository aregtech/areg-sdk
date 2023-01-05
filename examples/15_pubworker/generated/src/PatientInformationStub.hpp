//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/PatientInformationStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_PATIENTINFORMATIONSTUB_HPP
#define  GENERATED_SRC_PATIENTINFORMATIONSTUB_HPP

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:24 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 *
 * \file            generated/src/PatientInformationStub.hpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEPatientInformation.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// PatientInformationStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       PatientInformation Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              This service sends the patient information to apply connected devices.
 **/
class PatientInformationStub   : protected  StubBase
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
    explicit PatientInformationStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~PatientInformationStub( void ) = default;

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
// PatientInformation Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute Patient functions
 ************************************************************************/

    /**
     * \brief   Returns true if Patient attribute is valid
     **/
    inline bool isPatientValid( void ) const;
    /**
     * \brief   Invalidates Patient attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidatePatient( void );
    /**
     * \brief   Force to send Patient attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyPatientUpdated( void );
    /**
     * \brief   Set Patient attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of Patient attribute to set.
     *                      The Patient attribute description:
     *                      The patient information to apply to hardware.
     **/
    virtual void setPatient( const NEPatientInformation::PatientInfo & newValue );
    /**
     * \brief   Returns the value of Patient attribute.
     *          The Patient attribute description:
     *          The patient information to apply to hardware.
     **/
    inline const NEPatientInformation::PatientInfo & getPatient( void ) const;
    inline NEPatientInformation::PatientInfo & getPatient( void );

//////////////////////////////////////////////////////////////////////////
// PatientInformation Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// PatientInformation Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// PatientInformation Interface Broadcasts
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
     * \brief   Returns reference to PatientInformationStub object
     **/
    inline PatientInformationStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute Patient
     *          The patient information to apply to hardware.
     **/
    NEPatientInformation::PatientInfo   mPatient;
    /**
     * \brief   Attribute Patient data validation state.
     **/
    NEService::eDataStateType           mPatientState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PatientInformationStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PatientInformationStub );
};

//////////////////////////////////////////////////////////////////////////
// PatientInformationStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline PatientInformationStub & PatientInformationStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   Patient attribute functions
 ************************************************************************/
 
inline bool PatientInformationStub::isPatientValid( void ) const
{
    return (mPatientState  == NEService::eDataStateType::DataIsOK);
}

inline void PatientInformationStub::invalidatePatient( void )
{
    errorRequest( static_cast<msg_id>(NEPatientInformation::eMessageIDs::MsgId_Patient), false );
}

inline void PatientInformationStub::notifyPatientUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEPatientInformation::eMessageIDs::MsgId_Patient) );
}

inline const NEPatientInformation::PatientInfo & PatientInformationStub::getPatient( void ) const
{
    return mPatient;
}

inline NEPatientInformation::PatientInfo & PatientInformationStub::getPatient( void )
{
    return mPatient;
}

//////////////////////////////////////////////////////////////////////////
// PatientInformationStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_PATIENTINFORMATIONSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/PatientInformationStub.hpp file
//////////////////////////////////////////////////////////////////////////
