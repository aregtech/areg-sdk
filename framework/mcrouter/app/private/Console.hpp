#ifndef AREG_MCROUTER_APP_Console_HPP
#define AREG_MCROUTER_APP_Console_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/Console.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing console that outputs statistics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NEMath.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/String.hpp"

#include <chrono>
#include <string_view>

//////////////////////////////////////////////////////////////////////////
// Dependencies
//////////////////////////////////////////////////////////////////////////
class String;

//////////////////////////////////////////////////////////////////////////
// Console class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This helper class makes data send and receive Console output on console.
 *          Because of some OS specific method implementations, this object is a singleton.
 **/
class Console
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< The timestamp to calculate duration.
    using Timestamp = std::chrono::high_resolution_clock::time_point;
    //!< The clocks to calculate duration.
    using Clock     = std::chrono::high_resolution_clock;
    using Seconds   = std::ratio<1, 1>;

    /**
     * \brief   The X- and Y-coordinates for cursor position.
     **/
    typedef struct S_Coord
    {
        int16_t     posX{ 0 };  //!< X-coordinate of cursor position on console
        int16_t     posY{ 0 };  //!< Y-coordinate of cursor position on console
    } sCoord;

    /**
     * \brief   The Statistics data.
     */
    typedef struct S_Statistics
    {
        uint32_t    statDataSize{ 0 };      //!< The amount of data in bytes that sent or received.
        Timestamp   statLastUpdate;         //!< The timestamp when last time updated on the screen.
    } sStatistics;

    /**
     * \brief   Bytes in 1 Kilobyte.
     **/
    static constexpr uint32_t           ONE_KILOBYTE    { 1024 };
    /**
     * \brief   Bytes in 1 Megabyte.
     **/
    static constexpr uint32_t           ONE_MEGABYTE    { ONE_KILOBYTE * 1024 };

    static constexpr std::string_view   CMD_CLEAR_SCREEN{ "\x1B[2J" };

    static constexpr std::string_view   CMD_CLEAR_EOL   { "\x1B[K" };

    static constexpr std::string_view   CMD_CLEAR_LINE  { "\33[2K" };

    static constexpr std::string_view   CMD_SCROLL_BACK { "\x1B[3J" };

    static constexpr std::string_view   CMD_MOVE_CURSOR { "\x1B[%d;%dH" };

    static constexpr std::string_view   CMD_POS_CURSOR  { "\x1B[6n" };

    static constexpr std::string_view   CMD_READ_CURSOR { "\x1B[%d;%dR" };

    static constexpr std::string_view   CMD_ENTER_SCREEN{ "\x1B[?1049h\033[H" };

    static constexpr std::string_view   CMD_EXIT_SCREEN { "\x1B[?1049l" };

    static constexpr sCoord             COORD_SEND_DATA { 0, 0 };

    static constexpr sCoord             COORD_RECV_DATA { 0, 1 };

    static constexpr sCoord             COORD_OUTPUT_ERR{ 0, 2 };

    static constexpr sCoord             COORD_OUTPUT_MSG{ 0, 3 };

    static constexpr std::string_view   FORMAT_SEND_DATA{ "\x1B[1;1HSend data with the rate: % 7.02f %s\n" };

    static constexpr std::string_view   FORMAT_RECV_DATA{ "\x1B[2;1HRecv data with the rate: % 7.02f %s\n" };

    static constexpr std::string_view   FORMAT_MSG_ERROR{ "\x1B[3;1HERROR, unexpected command [ %s ], please type again ...\n" };

    static constexpr std::string_view   FORMAT_WAIT_QUIT{ "\x1B[4;1H%s" };

    static constexpr std::string_view   FORMAT_ENDOF_MSG{ "\x1B[4;%dH" };

    static constexpr std::string_view   MSG_WAIT_QUIT   { "Type \'quit\' or \'q\' to quit message router ...: "};

    static constexpr char               CHAR_ENTER      { 13 };

    static constexpr std::string_view   MSG_KILOBYTES   { "KBytes / sec." };

    static constexpr std::string_view   MSG_MEGABYTES   { "MBytes / sec." };

    static constexpr std::string_view   MSG_BYTES       { " Bytes / sec." };

    static constexpr char               QUIT_CH         { 'q' };

    static constexpr std::string_view   QUIT_STR        { "quit" };


//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    Console(void);
    ~Console(void);

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes the Console data.
     * \param   verbose     Set verbose flag to indicate whether to output data on console or not.
     **/
    static void initialize(bool verbose);

    /**
     * \brief   Call to check and if needed to output send data message on console.
     *          The message is output if verbose flag is set. Otherwise, makes nothing.
     * \param   bytesSent       Bytes that where sent. Value 0 means to make invalidate data output.
     **/
    static void sentBytes(uint32_t bytesSent);

    /**
     * \brief   Call to check and if needed to output receive data message on console.
     *          The message is output if verbose flag is set. Otherwise, makes nothing.
     * \param   bytesReceived   Bytes that where received. Value 0 means to make invalidate data output.
     **/
    static void receivedBytes(uint32_t bytesReceived);

    static void waitQuitCommand( void );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    static Console & _getInstance( void );

    void _unitialize( void );

    /**
     * \brief   Returns true if succeeded to make OS specific initializations.
     **/
    bool _osInitialize( void );

    void _osUnitialize(void);

    /**
     * \brief   Outputs message of sent data rate.
     * \param   duration    The duration of last update in milliseconds.
     **/
    void _outputSendBytes( uint32_t duration );
    /**
     * \brief   Outputs message of received data rate.
     * \param   duration    The duration of last update in milliseconds.
     **/
    void _outputReceiveBytes(uint32_t duration);
    /**
     * \brief   Sets the cursor position on console.
     * \param   pos     The X- and Y-coordinate positions on console to set the cursor.
     */
    void _setCursorCurrentPos(const Console::sCoord& pos);
    /**
     * \brief   Gets current X- and Y-coordinate position of cursor on the console.
     **/
    sCoord _getCursorCurrentPos( void );

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    bool        mSetupEnv;      //!< Flag, indicating whether the object is initialized or not.
    bool        mVerbose;       //!< The verbose output flag. If false, makes no message outputs.
    sStatistics mStatSent;      //!< The sent data.
    sStatistics mStatReceived;  //!< The received data.
    String      mCommand;       //!< The command entered by user on console.
    ptr_type    mContext;       //!< The OS specific context to use.
    Mutex       mLock;          //!< Synchronization object to handle cursor position.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(Console);
};

#endif  // AREG_MCROUTER_APP_Console_HPP

