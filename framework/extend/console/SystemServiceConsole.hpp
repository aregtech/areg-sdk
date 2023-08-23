#ifndef AREG_EXTEND_CONSOLE_SYSTEMSERVICECONSOLE_HPP
#define AREG_EXTEND_CONSOLE_SYSTEMSERVICECONSOLE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/console/SystemServiceConsole.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, System service console to output statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/StubBase.hpp"

#include "areg/base/NECommon.hpp"
#include "areg/component/Timer.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class SystemServiceBase;

//////////////////////////////////////////////////////////////////////////
// SystemServiceConsole class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A service provide to output statistics on console.
 *          This service provider does not contain requests.
 *          It has a time with timeout 1 second to grab the data and output data rate.
 **/
class SystemServiceConsole  : public    Component
                            , protected StubBase
                            , protected IETimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
protected:

    //!< Bytes in 1 Kilobyte.
    static constexpr uint32_t           ONE_KILOBYTE    { NECommon::ONE_KILOBYTE };
    //!< Bytes in 1 megabyte.
    static constexpr uint32_t           ONE_MEGABYTE    { NECommon::ONE_MEGABYTE };
    //!< String kilobytes per second
    static constexpr std::string_view   MSG_KILOBYTES   { "KBytes / sec." };
    //!< String megabytes per second
    static constexpr std::string_view   MSG_MEGABYTES   { "MBytes / sec." };
    //!< String bytes per second
    static constexpr std::string_view   MSG_BYTES       { " Bytes / sec." };

//////////////////////////////////////////////////////////////////////////
// DataRate helper class declaration.
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The helper class to calculate data rate and output message in MB, KB and Bytes.
     **/
    class DataRate
    {
    //////////////////////////////////////////////////////////////////////////
    // Members.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Calculates and initializes the data rate.
         **/
        DataRate(uint32_t sizeBytes = 0);

        //!< This pair contains size in bytes and message indicating MB, KB or Bytes.
        std::pair<float, std::string>   mRate;
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Instantiates the component object.
     * \param   sysService  The instance of the system service object.
     * \param   entry       The component entry object set in the model.
     * \param   owner       The instance of component owner thread.
     * \param   data        The optional component data set in system. Can be empty / no data.
     **/
    SystemServiceConsole( const String & appTitle, SystemServiceBase & sysService, const NERegistry::ComponentEntry & entry, ComponentThread & owner,  NEMemory::uAlign OPT data );

    /**
     * \brief   Destructor.
     **/
    virtual ~SystemServiceConsole( void ) = default;

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
    virtual void startupServiceInterface( Component & holder ) override;

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder ) override;

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StubBase overrides. Public pure virtual methods
/************************************************************************/

    /**
     * \brief   Sends update notification message to all clients.
     *          This method can be called manually to send update
     *          notification message after updating attribute value.
     *
     *          Overwrite to implement method
     *
     * \param   msgId   The attribute message ID to notify clients.
     **/
    virtual void sendNotification( unsigned int msgId ) override;

    /**
     * \brief   Sends error message to clients.
     *          If message ID is a request, it should send result NEService::RequestError or NEService::RequestCanceled, depending on msgCancel flag.
     *          If message ID is a response, it should send result NEService::Invalid.
     *          If message ID is an attribute, it should send result NEService::ResultDataInvalid
     *          and invalidate attribute data value.
     *
     *          Overwrite to implement method
     *
     * \param   msgId       The message ID to send error message
     * \param   msgCancel   Indicates whether the request is canceled or should respond with error.
     *                      This parameter has sense only for request IDs.
     *                      It is ignored for response and attributes IDs.
     **/
    virtual void errorRequest( unsigned int msgId, bool msgCancel ) override;

/************************************************************************/
// IEStubEventConsumer interface overrides.
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
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the instance of SystemServiceConsole
     **/
    inline SystemServiceConsole & self( void );
    /**
     * \brief   Called to output sent and received data rate messages.
     **/
    inline void _outputDataRate(uint32_t bytesSend, uint32_t bytesRecv);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    String              mTitle;         //!< The title to display on console.
    SystemServiceBase & mSystemService; //!< The instance of system service object.
    Timer               mTimer;         //!< The timer to run in component thread.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    SystemServiceConsole( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SystemServiceConsole );
};

//////////////////////////////////////////////////////////////////////////
// SystemServiceConsole inline methods
//////////////////////////////////////////////////////////////////////////
inline SystemServiceConsole & SystemServiceConsole::self( void )
{
    return (*this);
}

#endif  // AREG_EXTEND_CONSOLE_SYSTEMSERVICECONSOLE_HPP
