#ifndef AREG_MCROUTER_APP_CONSOLESERVICE_HPP
#define AREG_MCROUTER_APP_CONSOLESERVICE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/ConsoleService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing, service that outputs statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//////////////////////////////////////////////////////////////////////////
// ConsoleService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A service to output statistics..
 **/
class ConsoleService: public    Component
                    , protected StubBase
                    , protected IETimerConsumer
{
    //!< Bytes in 1 Kilobyte.
    static constexpr uint32_t           ONE_KILOBYTE    { 1024 };
    //!< Bytes in 1 megabyte.
    static constexpr uint32_t           ONE_MEGABYTE    { ONE_KILOBYTE * 1024 };
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
        DataRate(uint32_t sizeBytes = 0);

        //!< This pair contains size in bytes and message indicating MB, KB or Bytes.
        std::pair<float, std::string>   mRate;
    };

public:
    //!< The console service role name
    static constexpr std::string_view   SERVICE_NAME    { "ConcoleService" };

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

    /**
     * \brief   The callback to check the user input command.
     *          If returns true, the command is recognized ant the application can 
     *          stop working. Otherwise, display error and continue waiting for input.
     **/
    static bool checkCommand(const String& cmd);

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Instantiates the component object.
     * \param   ownerThread The instance of component owner thread.
     * \param   roleName    The role name of component, given in the system.
     * \param   data        The optional component data set in system. Can be empty / no data.
     **/
    ConsoleService( const NERegistry::ComponentEntry & entry, ComponentThread & owner, NEMemory::uAlign OPT data );

    /**
     * \brief   Destructor.
     **/
    virtual ~ConsoleService( void ) = default;

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
     * \brief   Returns the instance of ConsoleService
     **/
    inline ConsoleService & self( void );
    /**
     * \brief   Called to output sent and received data rate messages.
     **/
    inline void _outputDataRate(uint32_t bytesSend, uint32_t bytesRecv);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    Timer   mTimer;     //!< The timer to run in component thread.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ConsoleService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ConsoleService );
};

//////////////////////////////////////////////////////////////////////////
// ConsoleService inline methods
//////////////////////////////////////////////////////////////////////////
inline ConsoleService & ConsoleService::self( void )
{
    return (*this);
}

#endif  // AREG_MCROUTER_APP_CONSOLESERVICE_HPP
