#ifndef AREG_AREGEXTEND_CONSOLE_CONSOLE_HPP
#define AREG_AREGEXTEND_CONSOLE_CONSOLE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/console/Console.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, Console with or without extended features
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/MathDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <functional>

namespace areg::ext {

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
 *          read user input data from the console (similar to 'gets_s' or 'fgets') and
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
    using Coord     = areg::Coord;

    //!< The default buffer size to input on console.
    static constexpr uint32_t INPUT_BUFFER_SIZE { 512 };

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor.
//////////////////////////////////////////////////////////////////////////
private:
    Console();
    ~Console();

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the only instance of Console object to make outputs.
     **/
    static Console& instance();

    /**
     * \brief   Before calling the method the console input must be enabled. Otherwise, the user
     *          cannot make input on console. When called, the calling thread is blocked until user
     *          completes the input procedure. By completion, it triggers the callback and passes
     *          the input string of length maximum 255 characters to validate. Returns input string
     *          if it is validated by callback call or the callback is empty. Repeats the procedure
     *          if callback is not empty and the input was not validated.
     *
     * \param   callback    The callback to validate user input string.
     * \return  Returns input string maximum 255 characters after validation by callback, or if the
     *          callback is empty.
     **/
    String wait_for_input( Console::CallBack callback ) const;

    /**
     * \brief   Before calling the method the console input must be enabled. Otherwise, the user
     *          cannot make input on console. Reads and converts formatted inputs from console
     *          similar to 'gets_s' or 'fgets' methods and writes the data into the given arguments.
     *          Each argument must be a pointer to a variable of a type that corresponds to a type
     *          specifier in format. The maximum length of the strings must be specified in the
     *          format.
     *
     * \param   format      The format of input. arguments The list of pointers to the variables.
     * \return  Upon successful completion, these functions return true. Otherwise, returns false.
     **/
    bool read_inputs(const char* format, ...) const;

    /**
     * \brief   Before calling the method the console input must be enabled. Otherwise, the user
     *          cannot make input on console. Reads and converts formatted inputs from console
     *          similar to 'vscanf' method and writes the data into the given variable argument
     *          list. Each argument in the list must be a pointer to a variable of a type that
     *          corresponds to a type specifier in format. The maximum expected length of the
     *          strings must be specified in the format.
     *
     * \param   format      The format of input.
     * \param   varList     The variable argument list of pointers to the variables.
     * \return  Upon successful completion, these functions return true. Otherwise, returns false.
     **/
    bool read_input_list(const char* format, va_list varList) const;

    /**
     * \brief   Before calling the method the console input must be enabled. Otherwise, the user
     *          cannot make input on console. Reads from console a string with maximum length 512
     *          characters and returns it.
     *
     * \return  Returns the string input on console.
     **/
    String read_string() const;

    /**
     * \brief   Call if there is no need anymore to make any message output. Otherwise, it is called
     *          when the Console objects is destroyed.
     **/
    inline void uninitialize();

    /**
     * \brief   Returns true if the Console is initialized and ready to run.
     **/
    inline bool is_ready() const;

    /**
     * \brief   Enables or disables the console input. It has no effect if the Console already is
     *          blocked by waiting for the user input. This is used to synchronize input and output
     *          procedures in the multithreading environment.
     **/
    inline bool enable_console_input(bool enable);

    /**
     * \brief   Outputs the text message at the given coordinate.
     *
     * \param   pos     The X- and Y-coordinate to start to output the message text.
     * \param   text    The message text to output.
     **/
    inline void output_str(Console::Coord pos, const String& text) const;
    /**
     * \brief   Outputs the text message at the given coordinate.
     *
     * \param   pos     The X- and Y-coordinate to start to output the message text.
     * \param   text    The message text to output.
     **/
    inline void output_txt(Console::Coord pos, const std::string_view& text) const;

    /**
     * \brief   Outputs the formatted text message with arguments at the given coordinate.
     *
     * \param   pos         The X- and Y-coordinate to start to output the message text.
     * \param   format      The text format with arguments to output the message.
     **/
    void output_msg(Console::Coord pos, const char* format, ...) const;

    /**
     * \brief   Outputs the text message at the current cursor position.
     *
     * \param   text    The text to output.
     **/
    inline void print_str(const String& text) const;
    /**
     * \brief   Outputs the text message at the current cursor position.
     *
     * \param   text    The text to output.
     **/
    inline void print_txt(const std::string_view& text) const;

    /**
     * \brief   Outputs the formatted text message with arguments at the current cursor position.
     *
     * \param   format      The text format with arguments to output the message.
     **/
    void print_msg(const char* format, ...) const;

    /**
     * \brief   Returns the current coordinate of the cursor on the console.
     **/
    inline Console::Coord cursor_cur_position() const;

    /**
     * \brief   Sets the cursor position at the specified position on console.
     *
     * \param   pos     The X- and Y-coordinate of console to move the cursor.
     **/
    inline void set_cursor_cur_position(Console::Coord pos) const;

    /**
     * \brief   Moves and place the cursor at the begin of specified line. The first line starts
     *          with index 0.
     *
     * \param   line    The line to move cursor. The first line starts with index 0.
     **/
    inline void move_to_line(int16_t line) const;

    /**
     * \brief   Saves the cursor current position in memory.
     **/
    inline void save_cursor_position() const;

    /**
     * \brief   Restores previously saved in memory cursor position.
     **/
    inline void restore_cursor_position() const;

    /**
     * \brief   Moves cursor one line up from current position.
     **/
    inline void move_cursor_one_line_up() const;

    /**
     * \brief   Moves cursor one line down from current position.
     **/
    inline void move_cursor_one_line_down() const;

    /**
     * \brief   Clears the line starting from the cursor current position until the end of the line.
     **/
    inline void clear_current_line() const;

    /**
     * \brief   Clear the specified line starting from specified posX position until the end of the
     *          line.
     *
     * \param   pos     The position to start clearing line. The 'posY' parameter specifies the line
     *                  index and the 'posX' parameter specifies the starting position to clear.
     **/
    inline void clear_line(Console::Coord pos) const;

    /**
     * \brief   Clears the screen.
     **/
    inline void clear_screen() const;

    /**
     * \brief   Return the user input command.
     **/
    inline const String& user_input() const;

    /**
     * \brief   Call to refresh the console screen and show updates. Without this call the console
     *          behavior is unpredicted (depends on OS).
     **/
    inline void refresh_screen() const;

    /**
     * \brief   Locks the console to prevent other threads to output or change cursor position.
     *
     * \return  Returns true if succeeded to lock. Otherwise, returns false.
     **/
    inline bool lock_console();

    /**
     * \brief   Unlocks previously locked console.
     **/
    inline void unlock_console();

    /**
     * \brief   Reads data from the console into the provided buffer.
     *
     * \param   buffer      The buffer to store the read data.
     * \param   bufSize     The size of the buffer.
     * \return  Returns true if the data was successfully read and the buffer contains not empty
     *          data, otherwise false.
     **/
    static bool read_console_data(char* buffer, uint32_t bufSize);

//////////////////////////////////////////////////////////////////////////
// Hidden, OS specific methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Setup the console to start to input / output the messages. OS specific
     *          implementation.
     **/
    bool _os_setup();
    /**
     * \brief   Release the console, so that there will be no more inputs / outputs. OS specific
     *          implementation.
     **/
    void _os_release();

    /**
     * \brief   Outputs the specified message text at specified coordinate. OS specific
     *          implementation.
     *
     * \param   pos     The position on console to output message.
     * \param   text    The text message to output.
     **/
    void _os_output_text(Console::Coord pos, const String& text) const;
    /**
     * \brief   Outputs the specified message text at specified coordinate. OS specific
     *          implementation.
     *
     * \param   pos     The position on console to output message.
     * \param   text    The text message to output.
     **/
    void _os_output_text(Console::Coord pos, const std::string_view& text) const;
    /**
     * \brief   Outputs the specified message text at specified coordinate. OS specific
     *          implementation.
     *
     * \param   text    The text message to output.
     **/
    void _os_output_text(const String& text) const;
    /**
     * \brief   Outputs the specified message text at specified coordinate. OS specific
     *          implementation.
     *
     * \param   text    The text message to output.
     **/
    void _os_output_text(const std::string_view& text) const;

    /**
     * \brief   Returns the current position of the cursor on the console. OS specific
     *          implementation.
     **/
    Console::Coord _os_get_cursor_position() const;

    /**
     * \brief   Moves the cursor at specified position on the console. OS specific implementation.
     *
     * \param   pos     The X- and Y-coordinate of console to move the cursor.
     **/
    void _os_set_cursor_cur_position(Console::Coord pos) const;

    /**
     * \brief   Refreshes the screen to display output messages. OS specific implementation.
     **/
    void _os_refresh_screen() const;

    /**
     * \brief   Clears the line starting from the cursor position until the end of line.
     **/
    void _os_clear_line() const;

    /**
     * \brief   Clears console screen.
     **/
    void _os_clear_screen() const;

    /**
     * \brief   Reads and converts formatted inputs from console similar to 'vscanf' method and
     *          writes the data into the given variable argument list.
     *
     * \param   format      The format of input.
     * \param   varList     The variable argument list of pointers to the variables.
     * \return  Upon successful completion, these functions return true. Otherwise, returns false.
     **/
    bool _os_read_input_list(const char* format, va_list varList) const;

    /**
     * \brief   OS specific implementation to save the cursor current position in memory.
     **/
    void _os_save_cursor_position() const;

    /**
     * \brief   OS specific implementation to restore previously saved in memory cursor position.
     **/
    void _os_restore_cursor_position() const;

    /**
     * \brief   OS specific implementation to move cursor one line up from current position.
     **/
    void _os_move_cursor_one_line_up() const;

    /**
     * \brief   OS specific implementation to move cursor one line down from current position.
     **/
    void _os_move_cursor_one_line_down() const;

    /**
     * \brief   A blocking call to wait for user input of string on the console. OS specific
     *          implementation.
     *
     * \param   buffer      The buffer to fill in input, should be big enough.
     * \param   size        The size of buffer.
     **/
    static bool _os_wait_input_string(char * buffer, uint32_t size);

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
    mutable SyncEvent       mEnable;
    /**
     * \brief   The synchronization object used in multithreading environment.
     **/
    mutable ResourceLock    mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( Console );
};

//////////////////////////////////////////////////////////////////////////
// Console class inline method implementations.
//////////////////////////////////////////////////////////////////////////
inline void Console::uninitialize()
{
    _os_release( );
}

inline bool Console::is_ready() const
{
    return mIsReady;
}

inline bool Console::enable_console_input( bool enable )
{
    return enable ? (mIsReady && mEnable.set_event( )) : (mIsReady == false) || (mEnable.reset( ));
}

inline void Console::output_str( Console::Coord pos, const String & text ) const
{
    _os_output_text( pos, text );
}

inline void Console::output_txt( Console::Coord pos, const std::string_view & text ) const
{
    _os_output_text( pos, text );
}

inline void Console::print_str(const String& text) const
{
    _os_output_text(text);
}

inline void Console::print_txt(const std::string_view& text) const
{
    _os_output_text(text);
}

inline Console::Coord Console::cursor_cur_position() const
{
    return _os_get_cursor_position( );
}

inline void Console::set_cursor_cur_position( Console::Coord pos ) const
{
    _os_set_cursor_cur_position( pos );
}

inline void Console::move_to_line(int16_t line) const
{
    set_cursor_cur_position(Coord{ 0, line });
}

inline void Console::save_cursor_position() const
{
    _os_save_cursor_position();
}

inline void Console::restore_cursor_position() const
{
    _os_restore_cursor_position();
}

inline void Console::move_cursor_one_line_up() const
{
    _os_move_cursor_one_line_up();
}

inline void Console::move_cursor_one_line_down() const
{
    _os_move_cursor_one_line_down();
}

inline const String & Console::user_input() const
{
    return mUsrInput;
}

inline void Console::refresh_screen() const
{
    _os_refresh_screen( );
}

inline bool Console::lock_console()
{
    return mLock.lock(areg::WAIT_INFINITE);
}

inline void Console::unlock_console()
{
    mLock.unlock( );
}

inline void Console::clear_current_line() const
{
    _os_clear_line( );
}

inline void Console::clear_line(Console::Coord pos) const
{
    save_cursor_position();
    set_cursor_cur_position(pos);
    clear_current_line();
    restore_cursor_position();
}

inline void Console::clear_screen() const
{
    _os_clear_screen();
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_CONSOLE_CONSOLE_HPP
