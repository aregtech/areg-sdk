#ifndef AREG_AREGEXTEND_CONSOLE_CONSOLE_HPP
#define AREG_AREGEXTEND_CONSOLE_CONSOLE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/Console.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, Console based on ANSI escape sequences
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/MathDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <functional>
#include <atomic>

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// Console class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The singleton helper class to output message on the console.
 *          It draws with ANSI escape sequences on every platform and outputs messages
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
 *
 *          The screen is cleared by the first output at coordinates; until then the outputs
 *          at the cursor are a stream of lines, as with printf.
 *          A positioned output is clipped to the screen: it never wraps and never scrolls,
 *          and a row below the bottom of the screen is not drawn. A new line in the text
 *          continues on the next row at the same column. Positioned output does not move
 *          the cursor: it stays where set_cursor_cur_position() placed it.
 *          The outputs made between lock_console() and unlock_console() reach the terminal
 *          together when the console is unlocked.
 *          When the standard output is not a terminal, every positioned output is written
 *          as one plain line without escape sequences, and cursor operations do nothing.
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

    /**
     * \brief   The callback called when the size of the terminal changed.
     *          The parameter is the new size, posX is the number of columns and posY the number of rows.
     **/
    using ResizeCallback = std::function<void(Console::Coord)>;

    //!< The default buffer size to input on console.
    static constexpr uint32_t INPUT_BUFFER_SIZE { 512 };

    //!< The default number of entered lines kept to recall with the Up and Down keys.
    static constexpr uint32_t HISTORY_DEFAULT_SIZE  { 16 };

    //!< The largest number of entered lines kept to recall with the Up and Down keys.
    static constexpr uint32_t HISTORY_MAX_SIZE      { 64 };

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor.
//////////////////////////////////////////////////////////////////////////
private:
    Console() noexcept;
    ~Console() noexcept;

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
     *          the input string of maximum INPUT_BUFFER_SIZE - 1 characters to validate. Returns
     *          input string if it is validated by callback call or the callback is empty. Repeats
     *          the procedure if callback is not empty and the input was not validated.
     *          On a terminal the input line supports Backspace, Delete, Left, Right, Home, End,
     *          Ctrl+A, Ctrl+E and Ctrl+U. Characters past the buffer size are dropped. Up and Down
     *          recall the entered lines, see set_history_size().
     *
     * \param   callback    The callback to validate user input string.
     * \return  Returns input string after validation by callback, or if the callback is empty.
     *          Returns an empty string if the input is disabled or closed.
     **/
    String wait_for_input( Console::CallBack callback ) const;

    /**
     * \brief   Before calling the method the console input must be enabled. Otherwise, the user
     *          cannot make input on console. Reads one line as wait_for_input() does and converts
     *          it like 'sscanf', writing the data into the given arguments.
     *          Each argument must be a pointer to a variable of a type that corresponds to a type
     *          specifier in format. The maximum length of the strings must be specified in the format.
     *
     * \param   format      The format of input. arguments The list of pointers to the variables.
     * \return  Upon successful completion, these functions return true. Otherwise, returns false.
     **/
    bool read_inputs(const char* format, ...) const;

    /**
     * \brief   Before calling the method the console input must be enabled. Otherwise, the user
     *          cannot make input on console. Reads one line as wait_for_input() does and converts
     *          it like 'vsscanf', writing the data into the given variable argument list. Each
     *          argument in the list must be a pointer to a variable of a type that corresponds to
     *          a type specifier in format. The maximum expected length of the strings must be
     *          specified in the format.
     *
     * \param   format      The format of input.
     * \param   varList     The variable argument list of pointers to the variables.
     * \return  Upon successful completion, these functions return true. Otherwise, returns false.
     **/
    bool read_input_list(const char* format, va_list varList) const;

    /**
     * \brief   Before calling the method the console input must be enabled. Otherwise, the user
     *          cannot make input on console. Reads one line and returns its first word, at most
     *          510 characters.
     *
     * \return  Returns the first word of the line, or an empty string if nothing was read.
     **/
    String read_string() const;

    /**
     * \brief   Call if there is no need anymore to make any message output. Otherwise, it is called
     *          when the Console objects is destroyed.
     **/
    inline void uninitialize() noexcept;

    /**
     * \brief   Returns true if the Console is initialized and ready to run.
     **/
    [[nodiscard]]
    inline bool is_ready() const noexcept;

    /**
     * \brief   Returns true if the output goes to a terminal that draws at coordinates.
     *          Returns false if the output is redirected to a pipe or a file.
     **/
    [[nodiscard]]
    inline bool is_terminal() const noexcept;

    /**
     * \brief   Returns the visible size of the terminal: posX is the number of columns and
     *          posY the number of rows. Returns { 0, 0 } if the size is unknown.
     **/
    [[nodiscard]]
    Console::Coord screen_size() const noexcept;

    /**
     * \brief   Sets the callback called when the size of the terminal changed, or removes it if empty.
     *          The size is checked on each output and by screen_size(). The callback is called on the
     *          thread that made the output, after the console is unlocked, and it is never nested.
     *          It may output, clear the screen and set the cursor to draw the layout again.
     *          If the previous callback runs on another thread, the call waits until it returns,
     *          so it must not be made while this thread holds lock_console().
     *
     * \param   callback    The callback to call with the new size of the terminal.
     **/
    void set_resize_callback(Console::ResizeCallback callback);

    /**
     * \brief   Sets the number of entered lines kept to recall with the Up and Down keys on a terminal.
     *          Empty lines and a line equal to the previous one are not kept. Up recalls an older line,
     *          Down a newer one and, past the newest, the line typed before. 0 removes the kept lines
     *          and disables the recall, a number above HISTORY_MAX_SIZE keeps HISTORY_MAX_SIZE lines.
     *          The default is HISTORY_DEFAULT_SIZE.
     *
     * \param   count   The number of lines to keep. The oldest lines above it are removed.
     **/
    void set_history_size(uint32_t count) noexcept;

    /**
     * \brief   Returns the number of entered lines kept to recall with the Up and Down keys.
     **/
    [[nodiscard]]
    uint32_t history_size() const noexcept;

    /**
     * \brief   Enables or disables the console input. It has no effect if the Console already is
     *          blocked by waiting for the user input. This is used to synchronize input and output
     *          procedures in the multithreading environment.
     **/
    bool enable_console_input(bool enable) noexcept;

    /**
     * \brief   Outputs the text message at the given coordinate.
     *
     * \param   pos     The X- and Y-coordinate to start to output the message text.
     * \param   text    The message text to output.
     **/
    void output_str(Console::Coord pos, const String& text) const noexcept;
    /**
     * \brief   Outputs the text message at the given coordinate.
     *
     * \param   pos     The X- and Y-coordinate to start to output the message text.
     * \param   text    The message text to output.
     **/
    void output_txt(Console::Coord pos, std::string_view text) const noexcept;

    /**
     * \brief   Outputs the formatted text message with arguments at the given coordinate.
     *
     * \param   pos         The X- and Y-coordinate to start to output the message text.
     * \param   format      The text format with arguments to output the message.
     **/
    void output_msg(Console::Coord pos, const char* format, ...) const;

    /**
     * \brief   Outputs the text on a new row below every row drawn so far, on the bottom row if
     *          there is no room. Before anything is drawn at coordinates, or after a print at
     *          the cursor, the text is printed as a line at the cursor. Used for a last message.
     *
     * \param   text    The text of the row.
     **/
    void output_below(std::string_view text) const noexcept;

    /**
     * \brief   Outputs the text message at the current cursor position.
     *
     * \param   text    The text to output.
     **/
    void print_str(const String& text) const noexcept;

    /**
     * \brief   Outputs the text message at the current cursor position.
     *
     * \param   text    The text to output.
     **/
    void print_txt(std::string_view text) const noexcept;

    /**
     * \brief   Outputs the formatted text message with arguments at the current cursor position.
     *
     * \param   format      The text format with arguments to output the message.
     **/
    void print_msg(const char* format, ...) const;

    /**
     * \brief   Returns the coordinate the cursor was last moved to by set_cursor_cur_position().
     *          The terminal is not queried, so the call is safe on any thread while another
     *          thread waits for the user input.
     **/
    [[nodiscard]]
    Console::Coord cursor_cur_position() const noexcept;

    /**
     * \brief   Sets the cursor position at the specified position on console.
     *
     * \param   pos     The X- and Y-coordinate of console to move the cursor.
     **/
    void set_cursor_cur_position(Console::Coord pos) const noexcept;

    /**
     * \brief   Moves and place the cursor at the begin of specified line. The first line starts
     *          with index 0.
     *
     * \param   line    The line to move cursor. The first line starts with index 0.
     **/
    inline void move_to_line(int16_t line) const noexcept;

    /**
     * \brief   Saves the cursor current position in memory.
     **/
    void save_cursor_position() const noexcept;

    /**
     * \brief   Restores previously saved in memory cursor position.
     **/
    void restore_cursor_position() const noexcept;

    /**
     * \brief   Moves cursor one line up from current position.
     **/
    void move_cursor_one_line_up() const noexcept;

    /**
     * \brief   Moves cursor one line down from current position.
     **/
    void move_cursor_one_line_down() const noexcept;

    /**
     * \brief   Clears the line starting from the cursor current position until the end of the line.
     **/
    void clear_current_line() const noexcept;

    /**
     * \brief   Clear the specified line starting from specified posX position until the end of the
     *          line.
     *
     * \param   pos     The position to start clearing line. The 'posY' parameter specifies the line
     *                  index and the 'posX' parameter specifies the starting position to clear.
     **/
    void clear_line(Console::Coord pos) const noexcept;

    /**
     * \brief   Clears the screen.
     **/
    void clear_screen() const noexcept;

    /**
     * \brief   Return the user input command.
     **/
    inline const String& user_input() const noexcept;

    /**
     * \brief   Call to refresh the console screen and show updates. Without this call the console
     *          behavior is unpredicted (depends on OS).
     **/
    void refresh_screen() const noexcept;

    /**
     * \brief   Locks the console to prevent other threads to output or change cursor position.
     *          The lock is recursive. Outputs made while it is held are sent to the terminal
     *          in one write when the last lock is released.
     *
     * \return  Returns true if succeeded to lock. Otherwise, returns false.
     **/
    inline bool lock_console() noexcept;

    /**
     * \brief   Unlocks previously locked console and sends the collected outputs to the terminal.
     **/
    inline void unlock_console() noexcept;

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
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Holds the console lock for a scope. The collected outputs are flushed when
     *          the outermost lock is released.
     **/
    class ScopedLock;

    /**
     * \brief   Takes the console lock and counts the depth.
     **/
    bool _lock() const noexcept;

    /**
     * \brief   Flushes the collected outputs if the outermost lock is released, then unlocks.
     **/
    void _unlock() const noexcept;

    /**
     * \brief   Calls the resize callback with the new size of the terminal. Called without the lock.
     **/
    void _notify_resize() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden drawing and input methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Prepares the standard streams to input and output on the console.
     **/
    bool _setup() noexcept;

    /**
     * \brief   Places the cursor below the drawn rows and restores the standard streams.
     **/
    void _release() noexcept;

    /**
     * \brief   Sends the collected outputs to the terminal. Called with the lock held.
     **/
    void _flush() const noexcept;

    /**
     * \brief   Reads the size of the terminal, marks a change of the size and returns it.
     *          Called with the lock held.
     **/
    Console::Coord _read_screen_size() const noexcept;

    /**
     * \brief   Prepares a frame: reads the size once, clears the screen before the first
     *          drawing and hides the cursor. Called with the lock held.
     **/
    void _begin_drawing() const noexcept;

    /**
     * \brief   A blocking call that reads one line of the user input.
     *
     * \param   buffer      The buffer to fill in input, should be big enough.
     * \param   size        The size of buffer.
     * \return  Returns true if a line that is not empty was read.
     **/
    static bool _wait_input_string(char * buffer, uint32_t size);

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The flag, indicating whether the Console is ready to operate.
     **/
    std::atomic<bool>   mIsReady        { false };
    /**
     * \brief   True if the output goes to a terminal that draws at coordinates.
     **/
    bool                mIsTerminal { false };
    /**
     * \brief   The last input command as a string on the console.
     **/
    String              mUsrInput;
    /**
     * \brief   The callback called when the size of the terminal changed. Accessed with the lock held.
     **/
    ResizeCallback      mOnResize;
    /**
     * \brief   The size of the terminal read last. Accessed with the lock held.
     **/
    mutable Coord       mScreenSize;
    /**
     * \brief   Saved cursor position, used by save_cursor_position / restore_cursor_position.
     **/
    mutable Coord       mSavedPos;
    /**
     * \brief   The highest posY (row) used in any positioned write. Used by _release()
     *          to position the terminal cursor below the last output row on exit.
     **/
    mutable int32_t     mMaxUsedRow { 0 };
    /**
     * \brief   Set to true by enable_console_input(false) to signal wait_for_input
     *          to exit its retry loop after an interrupted OS read.
     **/
    mutable std::atomic<bool>   mInterrupted    { false };
    /**
     * \brief   Set when the input reached its end, so that no more input can be read.
     **/
    mutable std::atomic<bool>   mInputClosed    { false };
    /**
     * \brief   Set when the size of the terminal changed and the callback was not called yet.
     **/
    mutable std::atomic<bool>   mResizePending  { false };
    /**
     * \brief   Set while the resize callback runs, so that it is never nested.
     **/
    mutable std::atomic<bool>   mResizeRunning  { false };
    /**
     * \brief   The thread that runs the resize callback, 0 if none.
     **/
    mutable std::atomic<id_type> mResizeThread  { 0u };
    /**
     * \brief   The depth of the recursive console lock. Accessed only with the lock held.
     **/
    mutable uint32_t            mLockDepth      { 0u };
    /**
     * \brief   POSIX self-pipe used to unblock _wait_input_string().
     *          [0] = read end (polled alongside STDIN), [1] = write end (written by enable_console_input).
     *          Both are -1 on Windows, before _setup() and after _release().
     **/
    int                         mInterruptPipe[2]   { -1, -1 };
    /**
     * \brief   An object used to block the user input procedure to use in multithreading environment.
     **/
    mutable SyncEvent   mEnable;
    /**
     * \brief   Signaled while no resize callback runs.
     **/
    mutable SyncEvent   mResizeIdle;
    /**
     * \brief   The synchronization object used in multithreading environment.
     **/
    mutable Mutex       mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( Console );
};

//////////////////////////////////////////////////////////////////////////
// Console class inline method implementations.
//////////////////////////////////////////////////////////////////////////
inline void Console::uninitialize() noexcept
{
    _release();
}

inline bool Console::is_ready() const noexcept
{
    return mIsReady.load(std::memory_order_acquire);
}

inline bool Console::is_terminal() const noexcept
{
    return mIsTerminal;
}

inline void Console::move_to_line(int16_t line) const noexcept
{
    set_cursor_cur_position(Coord{ 0, line });
}

inline const String & Console::user_input() const noexcept
{
    return mUsrInput;
}

inline bool Console::lock_console() noexcept
{
    return _lock();
}

inline void Console::unlock_console() noexcept
{
    _unlock();
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_CONSOLE_CONSOLE_HPP
