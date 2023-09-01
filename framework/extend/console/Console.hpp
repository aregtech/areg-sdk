#ifndef AREG_UTILITIES_CONSOLE_CONSOLE_HPP
#define AREG_UTILITIES_CONSOLE_CONSOLE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/console/Console.hpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, Console with or without extended features
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/NEMath.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"

#include <functional>

//////////////////////////////////////////////////////////////////////////
// Console class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The singleton helper class to output message on the console.
 *          It has an OS specific implementation how to output messages
 *          using coordinates, where Y-coordinate defined the line index
 *          on the console and the X-coordinate defines the column, so that
 *          the very first position on screen starts with coordinate { 0, 0 }
 *          and the next line is { 0, 1 }. In addition, there are methods
 *          to get and set cursor position on the console, to clean the line
 *          read user input data from the console (similar to 'scanf') and
 *          wait until the user did not input the data on the console.
 *          The console is thread safe, so that if can run in multithreading
 *          environment where one thread is waiting for the user input and
 *          the other thread outputs messages on the desired position.
 **/
class Console
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The callback function to validate the input.
     *          Used when waits the user input on console.
     *          If returns true, stops waiting. If returns false, continues waiting.
     **/
    using CallBack  = std::function<bool(const String &)>;

    /**
     * \brief   Console::Coord
     *          The structure of console coordinates to handle cursor position.
     *          The lines on console are Y-coordinates.
     *          The columns on console are X-coordinates.
     **/
    using Coord     = NEMath::sCoord;

    //!< The default buffer size to input on console.
    static constexpr uint32_t INPUT_BUFFER_SIZE { 512 };

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
     * \brief   Before calling the method the console input must be enabled.
     *          Otherwise, the user cannot make input on console.
     * 
     *          When called, the calling thread is blocked until user completes
     *          the input procedure. By completion, it triggers the callback and
     *          passes the input string of length maximum 255 characters to validate.
     *          Returns input string if it is validated by callback call or the
     *          callback is empty. Repeats the procedure if callback is not empty
     *          and the input was not validated.
     *          
     * \param   callback    The callback to validate user input string.
     * \return  Returns input string maximum 255 characters after validation by callback,
     *          or if the callback is empty.
     **/
    String waitForInput( Console::CallBack callback ) const;

    /**
     * \brief   Before calling the method the console input must be enabled.
     *          Otherwise, the user cannot make input on console.
     *
     *          Reads and converts formated inputs from console similar to 'scanf' method
     *          and writes the data into the given arguments. Each argument must be a pointer
     *          to a variable of a type that corresponds to a type specifier in format. The maximum
     *          length of the strings must be specified in the format.
     * 
     * \param   format      The format of input.
     * \param   arguments   The list of pointers to the variables.
     * \return  Upon successful completion, these functions return true. Otherwise, returns false.
     **/
    bool readInputs(const char* format, ...) const;

    /**
     * \brief   Before calling the method the console input must be enabled.
     *          Otherwise, the user cannot make input on console.
     *
     *          Reads and converts formated inputs from console similar to 'vscanf' method
     *          and writes the data into the given variable argument list. Each argument 
     *          in the list must be a pointer to a variable of a type that corresponds to a type
     *          specifier in format. The maximum expected length of the strings must be 
     *          specified in the format.
     *
     * \param   format      The format of input.
     * \param   varList     The variable argument list of pointers to the variables.
     * \return  Upon successful completion, these functions return true. Otherwise, returns false.
     **/
    bool readInputList(const char* format, va_list varList) const;

    /**
     * \brief   Before calling the method the console input must be enabled.
     *          Otherwise, the user cannot make input on console.
     * 
     *          Reads from console a string with maximum length 512 characters and returns it.
     *
     * \return  Returns the string input on console.
     **/
    String readString(void) const;

    /**
     * \brief   Call if there is no need anymore to make any message output.
     *          Otherwise, it is called when the Console objects is destroyed.
     **/
    inline void uninitialize(void);

    /**
     * \brief   Returns true if the Console is initialized and ready to run.
     **/
    inline bool isReady( void ) const;

    /**
     * \brief   Enables or disables the console input. It has no effect if the Console
     *          already is blocked by waiting for the user input. This is used to synchronize
     *          input and output procedures in the multithreading environment.
     **/
    inline bool enableConsoleInput(bool enable);

    /**
     * \brief   Outputs the text message at the given coordinate.
     * \param   pos     The X- and Y-coordinate to start to output the message text.
     * \param   text    The message text to output.
     **/
    inline void outputStr(Console::Coord pos, const String& text) const;
    inline void outputTxt(Console::Coord pos, const std::string_view& text) const;

    /**
     * \brief   Outputs the formated text message with arguments at the given coordinate.
     * \param   pos     The X- and Y-coordinate to start to output the message text.
     * \param   format  The text format with arguments to output the message.
     **/
    void outputMsg(Console::Coord pos, const char* format, ...) const;

    /**
     * \brief   Outputs the formated text message at the current cursor position.
     * \param   format  The text format with arguments to output the message.
     **/
    inline void printStr(const String& text) const;
    inline void printTxt(const std::string_view& text) const;

    /**
     * \brief   Outputs the formated text message with arguments at the current cursor position.
     * \param   format  The text format with arguments to output the message.
     **/
    void printMsg(const char* format, ...) const;

    /**
     * \brief   Returns the current coordinate of the cursor on the console.
     **/
    inline Console::Coord getCursorCurPosition(void) const;

    /**
     * \brief   Sets the cursor position at the specified position on console.
     * \param   pos     The X- and Y-coordinate of console to move the cursor.
     **/
    inline void setCursorCurPosition(Console::Coord pos) const;

    /**
     * \brief   Moves and place the cursor at the begin of specified line.
     *          The first line starts with index 0.
     * 
     * \param   line    The line to move cursor. The first line starts with index 0.
     */
    inline void moveToLine(int16_t line) const;

    /**
     * \brief   Saves the cursor current position in memory.
     **/
    inline void saveCursorPosition(void) const;

    /**
     * \brief   Restores previously saved in memory cursor position.
     **/
    inline void restoreCursorPosition(void) const;

    /**
     * \brief   Moves cursor one line up from current position.
     **/
    inline void moveCursorOneLineUp(void) const;

    /**
     * \brief   Moves cursor one line down from current position.
     **/
    inline void moveCursorOneLineDown(void) const;

    /**
     * \brief   Clears the line starting from the cursor current position until the end of the line. 
     **/
    inline void clearCurrentLine( void ) const;

    /**
     * \brief   Clear the specified line starting from specified posX position until
     *          the end of the line.
     * \param   pos     The position to start clearing line.
     *                  The 'posY' parameter specifies the line index and
     *                  the 'posX' parameter specifies the starting position to clear. 
     **/
    inline void clearLine(Console::Coord pos) const;

    /**
     * \brief   Clears the screen.
     **/
    inline void clearScreen( void ) const;

    /**
     * \brief   Return the user input command.
     **/
    inline const String& getUserInput(void) const;

    /**
     * \brief   Call to refresh the console screen and show updates.
     *          Without this call the console behavior is unpredicted (depends on OS).
     **/
    inline void refreshScreen(void) const;

    /**
     * \brief   Locks the console to prevent other threads to output or change cursor position.
     * \return  Returns true if succeeded to lock. Otherwise, returns false.
     **/
    inline bool lockConsole( void );

    /**
     * \brief   Unlocks previously locked console.
     **/
    inline void unlockConsole( void );

//////////////////////////////////////////////////////////////////////////
// Hidden, OS specific methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Setup the console to start to input / output the messages.
     *          OS specific implementation.
     **/
    bool _osSetup(void);
    /**
     * \brief   Release the console, so that there will be no more inputs / outputs.
     *          OS specific implementation.
     **/
    void _osRelease(void);

    /**
     * \brief   Outputs the specified message text at specified coordinate.
     *          OS specific implementation.
     * \param   pos     The position on console to output message.
     * \param   text    The text message to output.
     **/
    void _osOutputText(Console::Coord pos, const String& text) const;
    void _osOutputText(Console::Coord pos, const std::string_view& text) const;
    void _osOutputText(const String& text) const;
    void _osOutputText(const std::string_view& text) const;

    /**
     * \brief   Returns the current position of the cursor on the console.
     *          OS specific implementation.
     **/
    Console::Coord _osGetCursorPosition(void) const;

    /**
     * \brief   Moves the cursor at specified position on the console.
     *          OS specific implementation.
     * \param   pos     The X- and Y-coordinate of console to move the cursor.
     **/
    void _osSetCursorCurPosition(Console::Coord pos) const;

    /**
     * \brief   A blocking call to wait for user input of string on the console.
     *          OS specific implementation.
     * \brief   fmt     The format of string, should contains the limited size.
     * \brief   buffer  The buffer to fill in input, should be big enough.
     * \brief   size    The size of buffer.
     **/
    void _osWaitInput(const char * fmt, char * buffer, uint32_t size) const;

    /**
     * \brief   Refreshes the screen to display output messages.
     *          OS specific implementation.
     **/
    void _osRefreshScreen(void) const;

    /**
     * \brief   Clears the line starting from the cursor position until the end of line. 
     **/
    void _osClearLine( void ) const;

    /**
     * \brief   Clears console screen.
     **/
    void _osClearScreen( void ) const;

    /**
     * \brief   Reads and converts formated inputs from console similar to 'vscanf' method
     *          and writes the data into the given variable argument list.
     *
     * \param   format      The format of input.
     * \param   varList     The variable argument list of pointers to the variables.
     * \return  Upon successful completion, these functions return true. Otherwise, returns false.
     **/
    bool _osReadInputList(const char* format, va_list varList) const;

    /**
     * \brief   OS specific implementation to save the cursor current position in memory.
     **/
    void _osSaveCursorPosition(void) const;

    /**
     * \brief   OS specific implementation to restore previously saved in memory cursor position.
     **/
    void _osRestoreCursorPosition(void) const;

    /**
     * \brief   OS specific implementation to move cursor one line up from current position.
     **/
    void _osMoveCursorOneLineUp(void) const;

    /**
     * \brief   OS specific implementation to move cursor one line down from current position.
     **/
    void _osMoveCursorOneLineDown(void) const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The flag, indicating whether the Console is ready to operate.
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
     * \brief   An object used to block the user input procedure to use in multithreading environment.
     **/
    mutable SynchEvent      mEnable;
    /**
     * \brief   The synchronization object used in multithreading environment.
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
    _osRelease( );
}

inline bool Console::isReady( void ) const
{
    return mIsReady;
}

inline bool Console::enableConsoleInput( bool enable )
{
    return enable ? (mIsReady && mEnable.setEvent( )) : (mIsReady == false) || (mEnable.resetEvent( ));
}

inline void Console::outputStr( Console::Coord pos, const String & text ) const
{
    _osOutputText( pos, text );
}

inline void Console::outputTxt( Console::Coord pos, const std::string_view & text ) const
{
    _osOutputText( pos, text );
}

inline void Console::printStr(const String& text) const
{
    _osOutputText(text);
}

inline void Console::printTxt(const std::string_view& text) const
{
    _osOutputText(text);
}

inline Console::Coord Console::getCursorCurPosition( void ) const
{
    return _osGetCursorPosition( );
}

inline void Console::setCursorCurPosition( Console::Coord pos ) const
{
    _osSetCursorCurPosition( pos );
}

inline void Console::moveToLine(int16_t line) const
{
    setCursorCurPosition(Coord{ 0, line });
}

inline void Console::saveCursorPosition(void) const
{
    _osSaveCursorPosition();
}

inline void Console::restoreCursorPosition(void) const
{
    _osRestoreCursorPosition();
}

inline void Console::moveCursorOneLineUp(void) const
{
    _osMoveCursorOneLineUp();
}

inline void Console::moveCursorOneLineDown(void) const
{
    _osMoveCursorOneLineDown();
}

inline const String & Console::getUserInput( void ) const
{
    return mUsrInput;
}

inline void Console::refreshScreen( void ) const
{
    _osRefreshScreen( );
}

inline bool Console::lockConsole( void )
{
    return mLock.lock(NECommon::WAIT_INFINITE);
}

inline void Console::unlockConsole( void )
{
    mLock.unlock( );
}

inline void Console::clearCurrentLine( void ) const
{
    _osClearLine( );
}

inline void Console::clearLine(Console::Coord pos) const
{
    saveCursorPosition();
    setCursorCurPosition(pos);
    clearCurrentLine();
    restoreCursorPosition();
}

inline void Console::clearScreen( void ) const
{
    _osClearScreen();
}

#endif  // AREG_UTILITIES_CONSOLE_CONSOLE_HPP
