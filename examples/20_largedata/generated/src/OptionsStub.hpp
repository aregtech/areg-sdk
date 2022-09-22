//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/OptionsStub.hpp file
//////////////////////////////////////////////////////////////////////////
#ifndef  GENERATED_SRC_OPTIONSSTUB_HPP
#define  GENERATED_SRC_OPTIONSSTUB_HPP

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source Options.
 *
 * \file            generated/src/OptionsStub.hpp
 * \ingroup         Options Service Interface
 * \brief           This is an automatic generated code of Options
 *                  Service Interface Stub class declaration.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "generated/src/NEOptions.hpp"
#include "areg/component/StubBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// OptionsStub class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Options Service Interface Stub object.
 *              Instantiated per every server component, which differ by
 *              role name of component.
 *
 *              This SI sets options to generate data
 **/
class OptionsStub   : protected  StubBase
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
    explicit OptionsStub( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~OptionsStub( void ) = default;

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
// Options Interface Attributes
//////////////////////////////////////////////////////////////////////////
 public:
/************************************************************************
 * Attribute OptionData functions
 ************************************************************************/

    /**
     * \brief   Returns true if OptionData attribute is valid
     **/
    inline bool isOptionDataValid( void ) const;
    /**
     * \brief   Invalidates OptionData attribute and
     *          sends error update notification message to clients.
     **/
    inline void invalidateOptionData( void );
    /**
     * \brief   Force to send OptionData attribute notification message to all clients.
     *          The method can be called when the attribute is updated not via set-method.
     **/
     inline void notifyOptionDataUpdated( void );
    /**
     * \brief   Set OptionData attribute value and send notification event to all clients.
     *          The notification is sent only when value is updated or validated.
     * \param   newValue    New value of OptionData attribute to set.
     *                      The OptionData attribute description:
     *                      The options
     **/
    virtual void setOptionData( const NEOptions::sOptionData & newValue );
    /**
     * \brief   Returns the value of OptionData attribute.
     *          The OptionData attribute description:
     *          The options
     **/
    inline const NEOptions::sOptionData & getOptionData( void ) const;
    inline NEOptions::sOptionData & getOptionData( void );

//////////////////////////////////////////////////////////////////////////
// Options Interface Requests
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Request call.
     *          Request to get data rate values
     * \see     responseDataRate
     **/
    virtual void requestDataRate( void ) = 0;

    /**
     * \brief   Request call.
     *          Request to get packetrate, i.e. the numberof generated image blocks per second
     * \see     responsePacketRate
     **/
    virtual void requestPacketRate( void ) = 0;

    /**
     * \brief   Request call.
     *          Request to change the options to generate images.
     * \param   Options DESCRIPTION MISSED
     * \note    Has no response
     **/
    virtual void requestChangeOptions( const NEOptions::sOptionData & Options ) = 0;

    /**
     * \brief   Request call.
     *          DESCRIPTION MISSED
     * \note    Has no response
     **/
    virtual void requestQuitApplication( void ) = 0;

//////////////////////////////////////////////////////////////////////////
// Options Interface Responses
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Response call.
     *          Response of data rate
     * \param   sentData    Computed rate of sent data per second.
     * \param   missedData  Computed rate of missed data. The data is missed if failed to copy within certain timeshlot.
     *          TODO: remove if useless.
     * \see     requestDataRate
     **/
    virtual void responseDataRate( const NEOptions::sDataRate & sentData, const NEOptions::sDataRate & missedData );

    /**
     * \brief   Response call.
     *          Response of packet rate.
     * \param   genPackets  Number of generate packet per second
     * \param   avSize      The average size of a packet.
     * \see     requestPacketRate
     **/
    virtual void responsePacketRate( unsigned int genPackets, const NEOptions::sDataRate & avSize );

//////////////////////////////////////////////////////////////////////////
// Options Interface Broadcasts
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
     * \brief   Returns reference to OptionsStub object
     **/
    inline OptionsStub & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Attribute OptionData
     *          The options
     **/
    NEOptions::sOptionData      mOptionData;
    /**
     * \brief   Attribute OptionData data validation state.
     **/
    NEService::eDataStateType   mOptionDataState;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    OptionsStub( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( OptionsStub );
};

//////////////////////////////////////////////////////////////////////////
// OptionsStub class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline OptionsStub & OptionsStub::self( void )
{
    return (*this);
}

/************************************************************************
 * \brief   OptionData attribute functions
 ************************************************************************/
 
inline bool OptionsStub::isOptionDataValid( void ) const
{
    return (mOptionDataState  == NEService::eDataStateType::DataIsOK);
}

inline void OptionsStub::invalidateOptionData( void )
{
    errorRequest( static_cast<msg_id>(NEOptions::eMessageIDs::MsgId_OptionData), false );
}

inline void OptionsStub::notifyOptionDataUpdated( void )
{
    sendNotification( static_cast<msg_id>(NEOptions::eMessageIDs::MsgId_OptionData) );
}

inline const NEOptions::sOptionData & OptionsStub::getOptionData( void ) const
{
    return mOptionData;
}

inline NEOptions::sOptionData & OptionsStub::getOptionData( void )
{
    return mOptionData;
}

//////////////////////////////////////////////////////////////////////////
// OptionsStub class declaration End
//////////////////////////////////////////////////////////////////////////

#endif   // GENERATED_SRC_OPTIONSSTUB_HPP

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/OptionsStub.hpp file
//////////////////////////////////////////////////////////////////////////
