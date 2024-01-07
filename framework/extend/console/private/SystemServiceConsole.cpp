/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/console/private/SystemServiceConsole.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, System service console to output statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "extend/console/SystemServiceConsole.hpp"

#include "areg/component/ComponentThread.hpp"
#include "extend/console/Console.hpp"
#include "extend/service/DataRateHelper.hpp"
#include "extend/service/NESystemService.hpp"

//////////////////////////////////////////////////////////////////////////
// SystemServiceConsole class implementation
//////////////////////////////////////////////////////////////////////////
SystemServiceConsole::SystemServiceConsole(DataRateHelper* dataRate, const NERegistry::ComponentEntry & entry, ComponentThread & owner, NEMemory::uAlign OPT data )
    : Component         ( entry, owner )
    , StubBase          ( self( ), NEService::getEmptyInterface( ) )
    , IETimerConsumer   ( )

    , mDataRateHelper   ( dataRate )
    , mTimer            ( self( ), "ConsoleServiceTimer" )
{
}

void SystemServiceConsole::startupServiceInterface( Component & holder )
{
    StubBase::startupServiceInterface( holder );

    Console & console = Console::getInstance( );
    console.lockConsole( );

    if ( (mDataRateHelper != nullptr) && mDataRateHelper->isVerbose())
    {

        console.outputMsg( NESystemService::COORD_SEND_RATE, NESystemService::FORMAT_SEND_DATA.data( ), 0.0f, DataRateHelper::MSG_BYTES.data( ) );
        console.outputMsg( NESystemService::COORD_RECV_RATE, NESystemService::FORMAT_RECV_DATA.data( ), 0.0f, DataRateHelper::MSG_BYTES.data( ) );
    }

    mTimer.startTimer( NECommon::TIMEOUT_1_SEC, Timer::CONTINUOUSLY );

    console.outputTxt( NESystemService::COORD_USER_INPUT, NESystemService::FORMAT_WAIT_QUIT );
    console.enableConsoleInput( true );
    console.refreshScreen( );
    console.unlockConsole( );
}

void SystemServiceConsole::shutdownServiceIntrface( Component & holder )
{
    mTimer.stopTimer( );
    StubBase::shutdownServiceIntrface( holder );
}

void SystemServiceConsole::processTimer( Timer & timer )
{
    ASSERT( &timer == &mTimer );
    if ( mTimer.isActive( ) )
    {
        _outputDataRate( );
    }
}

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
void SystemServiceConsole::sendNotification( unsigned int msgId )
{
}

void SystemServiceConsole::errorRequest( unsigned int msgId, bool msgCancel )
{
}

void SystemServiceConsole::processRequestEvent( ServiceRequestEvent & eventElem )
{
}

void SystemServiceConsole::processAttributeEvent( ServiceRequestEvent & eventElem )
{
}

inline void SystemServiceConsole::_outputDataRate(void)
{
    Console& console = Console::getInstance();
    console.lockConsole( );
    if ( (mDataRateHelper != nullptr) && mDataRateHelper->isVerbose())
    {
        DataRateHelper::DataRate rateSend{ mDataRateHelper->queryBytesSentWithLiterals() };
        DataRateHelper::DataRate rateRecv{ mDataRateHelper->queryBytesReceivedWithLiterals() };

        console.saveCursorPosition( );
        console.outputMsg( NESystemService::COORD_SEND_RATE, NESystemService::FORMAT_SEND_DATA.data( ), rateSend.first, rateSend.second.c_str( ) );
        console.outputMsg( NESystemService::COORD_RECV_RATE, NESystemService::FORMAT_RECV_DATA.data( ), rateRecv.first, rateRecv.second.c_str( ) );
        console.restoreCursorPosition( );
        console.refreshScreen( );
    }

    console.unlockConsole( );
}
