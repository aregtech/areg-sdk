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
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
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

#include "areg/base/Thread.hpp"
#include "areg/component/TEEvent.hpp"
#include "areg/component/Timer.hpp"

#include <string_view>
#include <thread>
#include <memory>
#include <utility>

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

    //!< Enter new screen.
    static constexpr std::string_view   CMD_ENTER_SCREEN    { "\x1B[?1049h\033[H" };
    //!< Exit new screen.
    static constexpr std::string_view   CMD_EXIT_SCREEN     { "\x1B[?1049l" };
    //!< Scroll cursor back.
    static constexpr std::string_view   CMD_SCROLL_BACK     { "\x1B[3J" };
    //!< Clear the screen.
    static constexpr std::string_view   CMD_CLEAR_SCREEN    { "\x1B[2J" };
    //!< Clear end of line.
    static constexpr std::string_view   CMD_CLEAR_EOL       { "\x1B[K" };
    //!< Clear line.
    static constexpr std::string_view   CMD_CLEAR_LINE      { "\33[2K" };
    //!< String format 'send data' rate
    static constexpr std::string_view   FORMAT_SEND_DATA_X  { "\x1B[1;1HSend data with the rate: % 7.02f %s" };
    //!< String format 'received data' rate
    static constexpr std::string_view   FORMAT_RECV_DATA_X  { "\x1B[2;1HRecv data with the rate: % 7.02f %s" };
    //!< String format 'wait for input'
    static constexpr std::string_view   FORMAT_WAIT_QUIT_X  { "\x1B[3;1HType \'quit\' or \'q\' to quit message router ...: %s" };
    //!< String format 'error entering command'
    static constexpr std::string_view   FORMAT_MSG_ERROR_X  { "\x1B[4;1HERROR, unexpected command [ %s ], please type again ..." };

    static constexpr std::string_view   CMD_POSITION_CURSOR { "\x1B[6n" };

    static constexpr std::string_view   CMD_READ_POSITION   { "\x1B[%d;%dR" };

    class DataRate
    {
    public:
        explicit DataRate(uint32_t sizeBytes);

        std::pair<float, std::string>   mRate;
    };

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

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Instantiates the component object.
     * \param   masterThread    The instance of component owner thread.
     * \param   roleName        The role name of component, given in the system.
     * \param   data            The optional component data set in system. Can be empty / no data.
     **/
    ConsoleService( ComponentThread & masterThread, const char * const roleName, NEMemory::uAlign OPTIONAL data );

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

private:
    Timer           mTimer;     //!< The timer to run in component thread.

private:
    inline ConsoleService & self( void );

    inline void outputDataRate(uint32_t bytesSend, uint32_t bytesRecv);

    static bool checkCommand(const String& cmd);

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
