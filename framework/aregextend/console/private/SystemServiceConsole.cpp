/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/private/SystemServiceConsole.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, System service console to output statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/console/SystemServiceConsole.hpp"

#include "areg/component/ComponentThread.hpp"
#include "aregextend/console/Console.hpp"
#include "aregextend/service/DataRateHelper.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// SystemServiceConsole class implementation
//////////////////////////////////////////////////////////////////////////
SystemServiceConsole::SystemServiceConsole(DataRateHelper* dataRate, const areg::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( entry, owner )
    , StubBase          ( self( ), areg::empty_interface( ) )
    , TimerConsumer     ( )

    , mDataRateHelper   ( dataRate )
    , mTimer            ( self( ), "ConsoleServiceTimer" )
{
}

void SystemServiceConsole::startup_service_interface( Component & holder )
{
    StubBase::startup_service_interface( holder );

    if ((mDataRateHelper != nullptr) && (mDataRateHelper->is_verbose()))
    {
        Console& console = Console::instance();
        console.output_msg(areg::ext::COORD_SEND_RATE, areg::ext::FORMAT_SEND_DATA.data(), 0.0f, areg::ext::DataRateHelper::MSG_BYTES.data());
        console.output_msg(areg::ext::COORD_RECV_RATE, areg::ext::FORMAT_RECV_DATA.data(), 0.0f, areg::ext::DataRateHelper::MSG_BYTES.data());
        console.output_msg(areg::ext::COORD_SEND_MSGS, areg::ext::FORMAT_SEND_MSGS.data(), 0u);
        console.output_msg(areg::ext::COORD_RECV_MSGS, areg::ext::FORMAT_RECV_MSGS.data(), 0u);
        console.output_txt(areg::ext::COORD_USER_INPUT, areg::ext::FORMAT_WAIT_QUIT);
        // Place the cursor immediately after the prompt text so fgets echoes there.
        console.set_cursor_cur_position({ areg::ext::COORD_USER_INPUT.posX + static_cast<int32_t>(areg::ext::FORMAT_WAIT_QUIT.size()),
                                          areg::ext::COORD_USER_INPUT.posY });
        console.enable_console_input(true);
        console.refresh_screen();
    }
}

#ifdef DEBUG
void SystemServiceConsole::process_timer( Timer & timer )
#else   // DEBUG
void SystemServiceConsole::process_timer(Timer & /*timer*/)
#endif  // DEBUG
{
    ASSERT( &timer == &mTimer );
    if ( mTimer.is_active( ) )
    {
        _output_data_rate( );
    }
}

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
void SystemServiceConsole::send_notification( uint32_t /* msgId */ )
{
}

void SystemServiceConsole::error_request( uint32_t /* msgId */, bool /* msgCancel */ )
{
}

void SystemServiceConsole::process_request_event( ServiceRequestEvent & /* eventElem */ )
{
}

void SystemServiceConsole::process_attribute_event( ServiceRequestEvent & /* eventElem */ )
{
}

inline void SystemServiceConsole::_output_data_rate()
{
    Console& console = Console::instance();
    if ( (mDataRateHelper != nullptr) && mDataRateHelper->is_verbose())
    {
        uint64_t sizeSent{ 0u }, sizeRecv{ 0u };
        uint32_t msgSent{ 0u }, msgRecv{ 0u };
        mDataRateHelper->query_data_sent(sizeSent, msgSent);
        mDataRateHelper->query_data_received(sizeRecv, msgRecv);

        DataRateHelper::DataRate rateSend{ DataRateHelper::convert_data_rate_literals(sizeSent) };
        DataRateHelper::DataRate rateRecv{ DataRateHelper::convert_data_rate_literals(sizeRecv) };

        console.output_msg( areg::ext::COORD_SEND_RATE, areg::ext::FORMAT_SEND_DATA.data( ), static_cast<double>(rateSend.first), rateSend.second.c_str( ) );
        console.output_msg( areg::ext::COORD_RECV_RATE, areg::ext::FORMAT_RECV_DATA.data( ), static_cast<double>(rateRecv.first), rateRecv.second.c_str( ) );
        console.output_msg( areg::ext::COORD_SEND_MSGS, areg::ext::FORMAT_SEND_MSGS.data( ), msgSent );
        console.output_msg( areg::ext::COORD_RECV_MSGS, areg::ext::FORMAT_RECV_MSGS.data( ), msgRecv );
        // console.refresh_screen( );
    }
}

} // namespace areg::ext
