#ifndef AREG_MCROUTER_APP_CONSOLE_HPP
#define AREG_MCROUTER_APP_CONSOLE_HPP
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
 * \brief       AREG Platform, Multi-cast routing, OS specific console.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "mcrouter/app/NEMulticastRouterSettings.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"

#include <functional>

//////////////////////////////////////////////////////////////////////////
// Console class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The singleton helper class to output message in the console.
 *          It has an OS specific implementation how to output messages
 *          using coordinates. Because the data rates and waiting for the
 *          user input run in different threads and the data rate should be
 *          updated in non-blocking way, there was a need to implement
 *          output messaged using coordinates.
 **/
class Console
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< Clear the screen.
    static constexpr std::string_view   CMD_CLEAR_SCREEN    { "\x1B[2J" };
    //!< Scroll cursor back.
    static constexpr std::string_view   CMD_SCROLL_BACK     { "\x1B[3J" };
    //!< Clear line.
    static constexpr std::string_view   CMD_CLEAR_LINE      { "\33[2K" };

public:
    /**
     * \brief   The type of callback function to set to check the user command.
     **/
    using CallBack = std::function<bool(const String &)>;

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor.
//////////////////////////////////////////////////////////////////////////
private:
    Console(void);
    ~Console(void);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the only instance of Console object to make outputs.
     **/
    static Console& getInstance(void);

    /**
     * \brief   When called, the calling thread is blocked until the user comletes
     *          the input procedure. It is triggering the callback to check the
     *          command and if recognized, returns true. And returnes false it the
     *          command is not recognized, or if no callback to check the command
     *          is set.
     **/
    bool waitForInput( void );

    /**
     * \brief   Call if there is no need anymore to make any message output.
     *          Otherwise, it is called when the Console objects is destryed.
     **/
    inline void uninitialize(void);

    /**
     * \brief   Call to set the callback to check the command.
     *          If no callback is set, there will be no checkup and the waiting
     *          for the user input will return false immediately after user
     *          typed the command.
     **/
    inline void setCallback(CallBack callback);

    /**
     * \brief   Returns true if the Console is initialized and redy to run.
     **/
    inline bool isReady( void ) const;

    /**
     * \brief   Enables or disables the console input. It has no effect if the Console
     *          already is blocked by waiting for the user input. This is used to synchronize
     *          input and output procedures in the multithreadin environment.
     **/
    inline bool enableConsoleInput(bool enable);

    /**
     * \brief   Outputs the text message at the given coordinate.
     * \param   pos     The X- and Y-coordinate to start to output the message text.
     * \param   text    The message text to output.
     **/
    inline void outputText(NEMulticastRouterSettings::Coord pos, const String& text) const;
    inline void outputText(NEMulticastRouterSettings::Coord pos, const std::string_view& text) const;

    /**
     * \brief   Outputs the formated text message with arguments at the given coordinate.
     * \param   pos     The X- and Y-coordinate to start to output the message text.
     * \param   format  The text format with arguments to output the message.
     **/
    void outputText(NEMulticastRouterSettings::Coord pos, const char* format, ...) const;

    /**
     * \brief   Returns the current coordinate of the cursor on the console.
     **/
    inline NEMulticastRouterSettings::Coord getCursorCurPosition(void) const;

    /**
     * \brief   Sets the cursor position at the specified position on console.
     * \param   pos     The X- and Y-coordinate of console to move the cursor.
     **/
    inline void setCursorCurPosition(NEMulticastRouterSettings::Coord pos) const;

    /**
     * \brief   Clears the line starting from the cursor current position until the end of the line. 
     **/
    inline void clearCurrentLine( void ) const;

    /**
     * \brief   Clear the specified line starting from specified posX position until
     *          the end of the line.
     * \param   pos     The position to start clearing line.
     *                  The 'posY' parameter specifies the line index and
     *                  the 'posX' paramter specifies the starting position to clear. 
     **/
    inline void clearLine(NEMulticastRouterSettings::Coord pos) const;

    /**
     * \brief   Return the user input command.
     **/
    inline const String& getUserInput(void) const;

    /**
     * \brief   Call to refresh the console screen and show updates.
     *          Without this call the console behavior is unpredicted (depends on OS).
     **/
    inline void refreshScreen(void) const;

//////////////////////////////////////////////////////////////////////////
// Hidden, OS specific methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes the console to start to input / output the messages.
     *          OS specific implementation.
     **/
    bool _osInitialize(void);
    /**
     * \brief   Uninitializes the console, so that there will be no more inputs / outputs.
     *          OS specific implementation.
     **/
    void _osUninitialize(void);

    /**
     * \brief   Outputs the specified message text at specified coordinate.
     *          OS specific implementation.
     * \param   pos     The position on console to output message.
     * \param   text    The text message to output.
     **/
    void _osOutputText(NEMulticastRouterSettings::Coord pos, const String& text) const;
    void _osOutputText(NEMulticastRouterSettings::Coord pos, const std::string_view& text) const;

    /**
     * \brief   Returns the current position of the cursor on the console.
     *          OS specific implementation.
     **/
    NEMulticastRouterSettings::Coord _osGetCursorPosition(void) const;

    /**
     * \brief   Moves the cursor at specified position on the console.
     *          OS specific implementation.
     * \param   pos     The X- and Y-coordinate of console to move the cursor.
     **/
    void _osSetCursorCurPosition(NEMulticastRouterSettings::Coord pos) const;

    /**
     * \brief   A blocking call to wait for user input on the console.
     *          OS specific implementation.
     **/
    void _osWaitInput(void);

    /**
     * \brief   Refreshes the screen to display output messages.
     *          OS specific implementation.
     **/
    void _osRefreshScreen(void) const;

    /**
     * \brie    Clears the line starting from the cursor position until the end of line. 
     **/
    void _osClearLine( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The flag, indicating wether the Console is ready to operate.
     **/
    bool            mIsReady;
    /**
     * \brief   The last input command as a string on the console.
     **/
    String          mUsrInput;
    /**
     * \brief   The context. It is OS specific.
     **/
    ptr_type        mContext;
    /**
     * \brief   The instance of callback function to check the input command.
     **/
    CallBack        mCallback;
    /**
     * \brief   An object used to block the user input procedure to use in multithreadin environment.
     **/
    SynchEvent      mEnable;
    /**
     * \brief   The synchronization object used in mutithreadin environemnt.
     **/
    mutable ResourceLock    mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Console );
};

//////////////////////////////////////////////////////////////////////////
// Console class inline method implementations.
//////////////////////////////////////////////////////////////////////////
inline void Console::uninitialize( void )
{
    _osUninitialize( );
}

inline void Console::setCallback( CallBack callback )
{
    mCallback = callback;
}

inline bool Console::isReady( void ) const
{
    return mIsReady;
}

inline bool Console::enableConsoleInput( bool enable )
{
    return enable ? (mIsReady && mEnable.setEvent( )) : (mIsReady == false) || (mEnable.resetEvent( ));
}

inline void Console::outputText( NEMulticastRouterSettings::Coord pos, const String & text ) const
{
    _osOutputText( pos, text );
}

inline void Console::outputText( NEMulticastRouterSettings::Coord pos, const std::string_view & text ) const
{
    _osOutputText( pos, text );
}

inline NEMulticastRouterSettings::Coord Console::getCursorCurPosition( void ) const
{
    return _osGetCursorPosition( );
}

inline void Console::setCursorCurPosition( NEMulticastRouterSettings::Coord pos ) const
{
    _osSetCursorCurPosition( pos );
}

inline const String & Console::getUserInput( void ) const
{
    return mUsrInput;
}

inline void Console::refreshScreen( void ) const
{
    _osRefreshScreen( );
}

inline void Console::clearCurrentLine( void ) const
{
    _osClearLine( );
}

inline void Console::clearLine(NEMulticastRouterSettings::Coord pos) const
{
    NEMulticastRouterSettings::Coord oldPos = getCursorCurPosition();
    setCursorCurPosition(pos);
    clearCurrentLine();
    setCursorCurPosition(oldPos);
}


#endif  // AREG_MCROUTER_APP_CONSOLE_HPP

