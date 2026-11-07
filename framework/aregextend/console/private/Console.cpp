/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/private/Console.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, console based on ANSI escape sequences, the same on
 *              every platform. The OS specific part is in ConsoleTerminal.hpp.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "aregextend/console/Console.hpp"
#include "aregextend/console/private/AnsiWriter.hpp"
#include "aregextend/console/private/ConsoleTerminal.hpp"
#include "areg/base/Thread.hpp"

#include <stdio.h>
#include <string.h>
#include <utility>

namespace {

    namespace terminal = areg::ext::terminal;
    using terminal::KeyRead;

    //!< Clears the screen and moves the cursor to the top left corner.
    constexpr std::string_view  CMD_CLEAR_SCREEN    { "\x1B[2J\x1B[H" };

    //!< Milliseconds to wait for the rest of an escape sequence typed on the keyboard.
    constexpr int               ESCAPE_WAIT_MS      { 50 };

    //!< The longest escape sequence of a key that is kept.
    constexpr uint32_t          ESCAPE_MAX_PARAMS   { 8u };

    constexpr char  KEY_ESC         { '\x1B' };
    constexpr char  KEY_CTRL_A      { '\x01' };
    constexpr char  KEY_CTRL_E      { '\x05' };
    constexpr char  KEY_CTRL_U      { '\x15' };
    constexpr char  KEY_BACKSPACE   { '\b' };
    constexpr char  KEY_DELETE      { '\x7F' };

    void _write_stdout(const char* data, uint32_t size, void* /*context*/) noexcept
    {
        ::fwrite(data, 1u, static_cast<size_t>(size), stdout);
        ::fflush(stdout);
    }

    //!< The collected output. Used only with the console lock held.
    areg::ext::AnsiWriter   _writer(&_write_stdout, nullptr);

    //!< The cursor is hidden until the next flush.
    bool    _cursorHidden   { false };

    //!< The cursor left the input position since the last flush.
    bool    _cursorAway     { false };

    //!< The screen size was read since the last flush.
    bool    _screenRead     { false };

    //!< Printed text left the cursor away from the input position.
    bool    _cursorDetached { false };

    //!< The screen was cleared for drawing at coordinates. Until then the output is a stream.
    bool    _screenCleared  { false };

    //!< Edits the input line in a caller buffer. The caret is an index in the text.
    class LineEditor
    {
    public:
        LineEditor(char* buffer, uint32_t size) noexcept
            : mBuffer   (buffer)
            , mCapacity (size > 0u ? size - 1u : 0u)
            , mLength   (0u)
            , mCaret    (0u)
        {
        }

        inline std::string_view text() const noexcept
        {
            return std::string_view(mBuffer, mLength);
        }

        inline uint32_t caret() const noexcept
        {
            return mCaret;
        }

        bool insert(char ch) noexcept
        {
            if (mLength >= mCapacity)
                return false;

            ::memmove(mBuffer + mCaret + 1u, mBuffer + mCaret, mLength - mCaret);
            mBuffer[mCaret++] = ch;
            ++mLength;
            return true;
        }

        bool erase_back() noexcept
        {
            if (mCaret == 0u)
                return false;

            --mCaret;
            return erase();
        }

        bool erase() noexcept
        {
            if (mCaret == mLength)
                return false;

            ::memmove(mBuffer + mCaret, mBuffer + mCaret + 1u, mLength - mCaret - 1u);
            --mLength;
            return true;
        }

        bool move_to(uint32_t caret) noexcept
        {
            caret = (caret > mLength) ? mLength : caret;
            const bool moved{ caret != mCaret };
            mCaret = caret;
            return moved;
        }

        bool left() noexcept
        {
            if (mCaret == 0u)
                return false;

            --mCaret;
            return true;
        }

        bool right() noexcept
        {
            return move_to(mCaret + 1u);
        }

        bool clear() noexcept
        {
            const bool changed{ mLength != 0u };
            mLength = 0u;
            mCaret  = 0u;
            return changed;
        }

        //!< Replaces the text, cut to the capacity, and moves the caret to its end.
        bool set(std::string_view text) noexcept
        {
            mLength = (text.size() < mCapacity) ? static_cast<uint32_t>(text.size()) : mCapacity;
            ::memcpy(mBuffer, text.data(), mLength);
            mCaret  = mLength;
            return true;
        }

        //!< Applies the key of an escape sequence: 'kind' is '[' or 'O', 'final' is the last byte.
        bool apply(char kind, std::string_view params, char final) noexcept
        {
            switch (final)
            {
            case 'D':
                return left();
            case 'C':
                return right();
            case 'H':
                return move_to(0u);
            case 'F':
                return move_to(mLength);
            case '~':
                if (kind != '[')
                    return false;
                else if ((params == "1") || (params == "7"))
                    return move_to(0u);
                else if ((params == "4") || (params == "8"))
                    return move_to(mLength);
                else if (params == "3")
                    return erase();
                return false;
            default:
                return false;
            }
        }

        void finish() noexcept
        {
            mBuffer[mLength] = '\0';
        }

    private:
        char*       mBuffer;
        uint32_t    mCapacity;
        uint32_t    mLength;
        uint32_t    mCaret;
    };

    //!< The last entered input lines, the oldest first. Used with the console lock held.
    class InputHistory
    {
    public:
        inline uint32_t limit() const noexcept
        {
            return mLimit;
        }

        inline uint32_t count() const noexcept
        {
            return mCount;
        }

        //!< Sets the number of lines to keep, at most HISTORY_MAX_SIZE, and drops the oldest ones above it.
        void set_limit(uint32_t limit) noexcept
        {
            mLimit = (limit < CAPACITY) ? limit : CAPACITY;
            while (mCount > mLimit)
            {
                _drop_oldest();
            }
        }

        //!< Adds a line that is not empty, unless it is equal to the newest one.
        void add(std::string_view text)
        {
            if ((mLimit == 0u) || ((mCount != 0u) && (at(0u) == text)))
            {
                return;
            }
            else if (mCount == mLimit)
            {
                _drop_oldest();
            }

            mEntries[(mFirst + mCount) % CAPACITY] = text;
            ++mCount;
        }

        //!< Returns the line entered 'back' lines before the newest one. 'back' is less than count().
        std::string_view at(uint32_t back) const noexcept
        {
            const areg::String& entry{ mEntries[(mFirst + mCount - 1u - back) % CAPACITY] };
            return std::string_view(entry.as_string(), static_cast<size_t>(entry.length()));
        }

    private:
        void _drop_oldest() noexcept
        {
            mEntries[mFirst].clear();
            mFirst = (mFirst + 1u) % CAPACITY;
            --mCount;
        }

        static constexpr uint32_t CAPACITY{ areg::ext::Console::HISTORY_MAX_SIZE };

        areg::String    mEntries[CAPACITY];
        uint32_t        mFirst  { 0u };
        uint32_t        mCount  { 0u };
        uint32_t        mLimit  { areg::ext::Console::HISTORY_DEFAULT_SIZE };
    };

    InputHistory    _history;

    //!< Recalls entered lines into the edited line. The index is -1 while the user edits a new line.
    class HistoryBrowser
    {
    public:
        //!< Recalls the next older line if 'older' is true, otherwise the next newer one or the new line.
        bool move(LineEditor& line, bool older)
        {
            const int32_t count{ static_cast<int32_t>(_history.count()) };
            if (older)
            {
                if (mIndex + 1 >= count)
                    return false;
                else if (mIndex < 0)
                    mDraft = line.text();

                ++mIndex;
            }
            else
            {
                if (mIndex < 0)
                    return false;

                mIndex = (mIndex - 1 < count - 1) ? mIndex - 1 : count - 1;
            }

            return line.set(mIndex < 0 ? std::string_view(mDraft.as_string(), static_cast<size_t>(mDraft.length())) : _history.at(static_cast<uint32_t>(mIndex)));
        }

    private:
        int32_t         mIndex  { -1 };
        areg::String    mDraft  { };
    };

    //!< A key typed as an escape sequence: 'kind' is '[' or 'O', 'final' is the last byte, 0 if none.
    struct EscapeKey
    {
        char        kind    { 0 };
        char        final   { 0 };
        uint32_t    count   { 0u };
        char        params[ESCAPE_MAX_PARAMS] { };
    };

    //!< The input line while the user types, nullptr otherwise. Used with the console lock held.
    const LineEditor*   _field          { nullptr };

    //!< The position of the first character of the input line.
    areg::Coord         _fieldStart     { 0, 0 };

    //!< An output drew over the row of the input line since the last flush.
    bool                _fieldHidden    { false };

    //!< Marks the input line to draw again if its row is in the range.
    inline void _touch_rows(int32_t first, int32_t last) noexcept
    {
        if ((_field != nullptr) && (first <= _fieldStart.posY) && (_fieldStart.posY <= last))
        {
            _fieldHidden = true;
        }
    }

    //!< Reads the rest of a typed escape sequence. A bare Esc and Alt with a key give no final byte.
    EscapeKey _read_escape(int pipeFd, KeyRead& status) noexcept
    {
        EscapeKey key{};
        while (true)
        {
            char ch{ 0 };
            const KeyRead next{ terminal::read_byte(pipeFd, ESCAPE_WAIT_MS, ch) };
            if (next != KeyRead::Byte)
            {
                status = (next == KeyRead::Timeout) ? KeyRead::Byte : next;
                return EscapeKey{};
            }
            else if (key.kind == 0)
            {
                if ((ch != '[') && (ch != 'O'))
                    return EscapeKey{};

                key.kind = ch;
            }
            else if ((ch >= 0x40) && (ch <= 0x7E))
            {
                key.final = ch;
                return key;
            }
            else if (key.count < ESCAPE_MAX_PARAMS)
            {
                key.params[key.count++] = ch;
            }
        }
    }

    //!< Applies a typed key to the line. Returns true if the line changed.
    bool _apply_key(LineEditor& line, HistoryBrowser& browser, char ch, const EscapeKey& key, bool& done)
    {
        switch (ch)
        {
        case '\r':
        case '\n':
            done = true;
            return false;
        case KEY_BACKSPACE:
        case KEY_DELETE:
            return line.erase_back();
        case KEY_CTRL_A:
            return line.move_to(0u);
        case KEY_CTRL_E:
            return line.move_to(0xFFFFFFFFu);
        case KEY_CTRL_U:
            return line.clear();
        case KEY_ESC:
            if ((key.final == 'A') || (key.final == 'B'))
                return browser.move(line, key.final == 'A');

            return (key.final != 0) && line.apply(key.kind, std::string_view(key.params, key.count), key.final);
        default:
            return (static_cast<unsigned char>(ch) >= 0x20u) && (static_cast<unsigned char>(ch) < 0x7Fu) && line.insert(ch);
        }
    }

} // namespace

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// Console::ScopedLock class implementation
//////////////////////////////////////////////////////////////////////////

class Console::ScopedLock
{
public:
    explicit ScopedLock(const Console& console) noexcept
        : mConsole(console)
    {
        mConsole._lock();
    }

    ~ScopedLock() noexcept
    {
        mConsole._unlock();
    }

    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator = (const ScopedLock&) = delete;

private:
    const Console&  mConsole;   //!< The locked console.
};

//////////////////////////////////////////////////////////////////////////
// Console class, construction
//////////////////////////////////////////////////////////////////////////

Console& Console::instance()
{
    static Console _instance;   // singleton instance.
    return _instance;
}

Console::Console() noexcept
    : mUsrInput     ( )
    , mOnResize     ( )
    , mScreenSize   { 0, 0 }
    , mSavedPos     { 0, 0 }
    , mEnable       (true, false)
    , mResizeIdle   (false, false)
    , mLock         (false)
{
    _setup();
}

Console::~Console() noexcept
{
    _release();
}

//////////////////////////////////////////////////////////////////////////
// Console class, input
//////////////////////////////////////////////////////////////////////////

String Console::wait_for_input(Console::CallBack callback) const
{
    String result;

    mEnable.lock(areg::WAIT_INFINITE);

    while (is_ready())
    {
        restore_cursor_position();
        char buffer[INPUT_BUFFER_SIZE]{ 0 };
        if (Console::_wait_input_string(buffer, INPUT_BUFFER_SIZE))
        {
            result = buffer;
            if ((static_cast<bool>(callback) == false) || callback(result))
            {
                break;
            }

            result.clear();
        }
        else if (mInterrupted.load(std::memory_order_relaxed) || mInputClosed.load(std::memory_order_relaxed))
        {
            break;
        }
    }

    return result;
}

bool Console::read_inputs(const char* format, ...) const
{
    va_list argptr;
    va_start(argptr, format);
    bool result = read_input_list(format, argptr);
    va_end(argptr);

    return result;
}

bool Console::read_input_list(const char* format, va_list varList) const
{
    const String line{ wait_for_input(nullptr) };
    return (line.is_empty() == false) && (::vsscanf(line.as_string(), format, varList) > 0);
}

String Console::read_string() const
{
    char buffer[512] { 0 };
    return String(read_inputs("%510s", buffer) ? buffer : String::empty_string());
}

bool Console::read_console_data(char* buffer, uint32_t bufSize)
{
    return Console::_wait_input_string(buffer, bufSize);
}

bool Console::enable_console_input(bool enable) noexcept
{
    if (enable)
    {
        mInterrupted.store(false, std::memory_order_relaxed);
        return is_ready() && mEnable.set_signaled();
    }

    const bool result = (is_ready() == false) || mEnable.reset();
    mInterrupted.store(true, std::memory_order_relaxed);
    terminal::interrupt(mInterruptPipe);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Console class, terminal state
//////////////////////////////////////////////////////////////////////////

Console::Coord Console::screen_size() const noexcept
{
    ScopedLock lock(*this);
    return (mIsTerminal ? _read_screen_size() : Coord{ 0, 0 });
}

void Console::set_resize_callback(Console::ResizeCallback callback)
{
    mLock.lock(areg::WAIT_INFINITE);
    mOnResize = std::move(callback);
    const id_type running{ mResizeThread.load(std::memory_order_acquire) };
    mLock.unlock();

    if (running != areg::Thread::current_thread_id())
    {
        mResizeIdle.lock(areg::WAIT_INFINITE);
    }
}

void Console::set_history_size(uint32_t count) noexcept
{
    ScopedLock lock(*this);
    _history.set_limit(count);
}

uint32_t Console::history_size() const noexcept
{
    ScopedLock lock(*this);
    return _history.limit();
}

//////////////////////////////////////////////////////////////////////////
// Console class, output
//////////////////////////////////////////////////////////////////////////

void Console::output_str(Console::Coord pos, const String& text) const noexcept
{
    output_txt(pos, std::string_view(text.as_string(), static_cast<size_t>(text.length())));
}

void Console::output_txt(Console::Coord pos, std::string_view text) const noexcept
{
    ScopedLock lock(*this);
    if (mIsReady == false)
    {
        return;
    }
    else if (mIsTerminal)
    {
        _begin_drawing();
        const int32_t last{ _writer.text_at(pos, text) };
        mMaxUsedRow = (last > mMaxUsedRow) ? last : mMaxUsedRow;
        _touch_rows(pos.posY, last);
    }
    else
    {
        _writer.plain_line(text);
    }
}

void Console::output_msg(Console::Coord pos, const char* format, ...) const
{
    va_list argptr;
    va_start(argptr, format);

    String text;
    text.format_list(format, argptr);
    va_end(argptr);

    output_str(pos, text);
}

void Console::output_below(std::string_view text) const noexcept
{
    ScopedLock lock(*this);
    if (mIsReady == false)
    {
        return;
    }
    else if (mIsTerminal == false)
    {
        _writer.plain_line(text);
    }
    else if ((_screenCleared == false) || _cursorDetached)
    {
        _writer.raw(text);
        _writer.raw("\r\n");
    }
    else
    {
        _begin_drawing();
        const int32_t bottom{ _writer.screen().posY - 1 };
        const int32_t row{ ((bottom >= 0) && (mMaxUsedRow + 1 > bottom)) ? bottom : mMaxUsedRow + 1 };
        const int32_t last{ _writer.text_at(Coord{ 0, row }, text) };
        mMaxUsedRow = (last > row) ? last : row;
    }
}

void Console::print_str(const String& text) const noexcept
{
    print_txt(std::string_view(text.as_string(), static_cast<size_t>(text.length())));
}

void Console::print_txt(std::string_view text) const noexcept
{
    ScopedLock lock(*this);
    if (mIsReady == false)
    {
        return;
    }
    else if (mIsTerminal)
    {
        if (_cursorAway)
        {
            _writer.move_to(mSavedPos);
            _cursorAway = false;
        }

        _writer.raw(text);
        _cursorDetached = true;
        _touch_rows(mSavedPos.posY, mSavedPos.posY);
    }
    else
    {
        _writer.plain(text);
    }
}

void Console::print_msg(const char* format, ...) const
{
    va_list argptr;
    va_start(argptr, format);

    String text;
    text.format_list(format, argptr);
    va_end(argptr);

    print_str(text);
}

//////////////////////////////////////////////////////////////////////////
// Console class, cursor and screen
//////////////////////////////////////////////////////////////////////////

Console::Coord Console::cursor_cur_position() const noexcept
{
    ScopedLock lock(*this);
    return mSavedPos;
}

void Console::set_cursor_cur_position(Console::Coord pos) const noexcept
{
    ScopedLock lock(*this);
    mSavedPos = Coord{ (pos.posX > 0) ? pos.posX : 0, (pos.posY > 0) ? pos.posY : 0 };
    _cursorAway = mIsTerminal;
}

void Console::save_cursor_position() const noexcept
{
}

void Console::restore_cursor_position() const noexcept
{
    ScopedLock lock(*this);
    _cursorAway = _cursorAway || (mIsTerminal && _cursorDetached && _screenCleared);
}

void Console::move_cursor_one_line_up() const noexcept
{
    ScopedLock lock(*this);
    mSavedPos = Coord{ 0, (mSavedPos.posY > 0) ? mSavedPos.posY - 1 : 0 };
    _cursorAway = mIsTerminal;
}

void Console::move_cursor_one_line_down() const noexcept
{
    ScopedLock lock(*this);
    mSavedPos = Coord{ 0, mSavedPos.posY + 1 };
    _cursorAway = mIsTerminal;
}

void Console::clear_current_line() const noexcept
{
    ScopedLock lock(*this);
    clear_line(mSavedPos);
}

void Console::clear_line(Console::Coord pos) const noexcept
{
    ScopedLock lock(*this);
    if (is_ready() && mIsTerminal)
    {
        _begin_drawing();
        _writer.clear_at(pos);
        _touch_rows(pos.posY, pos.posY);
    }
}

void Console::clear_screen() const noexcept
{
    ScopedLock lock(*this);
    if (is_ready() && mIsTerminal)
    {
        _writer.raw(CMD_CLEAR_SCREEN);
        _screenCleared = true;
        mSavedPos   = Coord{ 0, 0 };
        mMaxUsedRow = 0;
        _cursorAway = false;
        _cursorDetached = false;
        _touch_rows(_fieldStart.posY, _fieldStart.posY);
    }
}

void Console::refresh_screen() const noexcept
{
    ScopedLock lock(*this);
}

//////////////////////////////////////////////////////////////////////////
// Console class, hidden methods
//////////////////////////////////////////////////////////////////////////

bool Console::_lock() const noexcept
{
    const bool result{ mLock.lock(areg::WAIT_INFINITE) };
    mLockDepth += static_cast<uint32_t>(result);
    return result;
}

void Console::_unlock() const noexcept
{
    ASSERT(mLockDepth != 0u);
    const bool outermost{ (mLockDepth != 0u) && (--mLockDepth == 0u) };
    if (outermost)
    {
        _flush();
    }

    mLock.unlock();
    if (outermost && mResizePending.load(std::memory_order_acquire))
    {
        _notify_resize();
    }
}

void Console::_notify_resize() const noexcept
{
    if (mResizeRunning.exchange(true, std::memory_order_acq_rel))
    {
        return;
    }

    mLock.lock(areg::WAIT_INFINITE);
    mResizePending.store(false, std::memory_order_relaxed);
    const ResizeCallback callback{ mOnResize };
    const Coord size{ mScreenSize };
    if (callback)
    {
        mResizeThread.store(areg::Thread::current_thread_id(), std::memory_order_release);
        mResizeIdle.reset();
    }

    mLock.unlock();

    if (callback)
    {
        callback(size);
        mResizeThread.store(0u, std::memory_order_release);
        mResizeIdle.set_signaled();
    }

    mResizeRunning.store(false, std::memory_order_release);
}

bool Console::_setup() noexcept
{
    mIsTerminal = terminal::open(mInterruptPipe);
    mSavedPos   = Coord{ 0, 0 };
    mMaxUsedRow = 0;
    mScreenSize = Coord{ 0, 0 };
    if (mIsTerminal)
    {
        _read_screen_size();
    }

    mInputClosed.store(false, std::memory_order_relaxed);
    _screenCleared = false;
    mIsReady.store(true, std::memory_order_release);
    return true;
}

void Console::_release() noexcept
{
    ScopedLock lock(*this);
    if (mIsReady == false)
    {
        return;
    }

    mIsReady.store(false, std::memory_order_release);
    if (mIsTerminal)
    {
        if (_screenCleared && (_cursorDetached == false))
        {
            const Coord size{ terminal::screen_size() };
            int32_t row{ mMaxUsedRow + 1 };
            if ((size.posY > 0) && (row >= size.posY))
            {
                row = size.posY - 1;
            }

            _writer.move_to(Coord{ 0, row });
            _writer.raw("\n");
        }

        _writer.show_cursor();
        _cursorAway     = false;
        _cursorHidden   = false;
    }

    _writer.flush();
    terminal::close(mInterruptPipe);
}

void Console::_flush() const noexcept
{
    if (mIsTerminal && is_ready())
    {
        if (_fieldHidden)
        {
            ASSERT(_field != nullptr);
            _begin_drawing();
            mSavedPos = _writer.field(_fieldStart, _field->text(), _field->caret());
        }

        if (_cursorAway)
        {
            _writer.move_to(mSavedPos);
            _cursorDetached = false;
        }

        if (_cursorHidden)
        {
            _writer.show_cursor();
        }
    }

    _cursorAway     = false;
    _cursorHidden   = false;
    _screenRead     = false;
    _fieldHidden    = false;
    _writer.flush();
}

Console::Coord Console::_read_screen_size() const noexcept
{
    const Coord size{ terminal::screen_size() };
    if ((size.posX > 0) && (size.posY > 0) && ((size.posX != mScreenSize.posX) || (size.posY != mScreenSize.posY)))
    {
        if (mScreenSize.posX > 0)
        {
            mResizePending.store(true, std::memory_order_release);
        }

        mScreenSize = size;
    }

    return size;
}

void Console::_begin_drawing() const noexcept
{
    if (_screenRead == false)
    {
        const Coord size{ _read_screen_size() };
        _writer.set_screen(size.posX, size.posY);
        _screenRead = true;
    }

    if (_screenCleared == false)
    {
        _writer.raw(CMD_CLEAR_SCREEN);
        _screenCleared = true;
    }

    if (_cursorHidden == false)
    {
        _writer.hide_cursor();
        _cursorHidden = true;
    }

    _cursorAway = true;
}

bool Console::_wait_input_string(char* buffer, uint32_t size)
{
    ASSERT(buffer != nullptr);
    if (size == 0u)
    {
        return false;
    }

    buffer[0] = '\0';
    Console& console = Console::instance();
    LineEditor line(buffer, size);
    KeyRead status{ KeyRead::Byte };

    const bool raw{ terminal::is_key_mode() };
    const int pipeFd{ console.mInterruptPipe[0] };

    if (raw == false)
    {
        char ch{ 0 };
        while ((status = terminal::read_byte(pipeFd, -1, ch)) == KeyRead::Byte)
        {
            if (ch == '\n')
                break;
            else if (ch != '\r')
                line.insert(ch);
        }
    }
    else
    {
        Coord start{ 0, 0 };
        {
            ScopedLock lock(console);
            start = console.mSavedPos;
            _field = &line;
            _fieldStart = start;
            _fieldHidden = true;
        }

        HistoryBrowser browser;
        bool done{ false };
        while ((done == false) && (status == KeyRead::Byte))
        {
            char ch{ 0 };
            if ((status = terminal::read_byte(pipeFd, -1, ch)) != KeyRead::Byte)
            {
                break;
            }

            const EscapeKey key{ (ch == KEY_ESC) ? _read_escape(pipeFd, status) : EscapeKey{} };
            ScopedLock lock(console);
            if (_apply_key(line, browser, ch, key, done))
            {
                _fieldHidden = true;
            }
        }

        ScopedLock lock(console);
        _field = nullptr;
        _fieldHidden = false;
        console.mSavedPos = start;
        _cursorAway = console.mIsTerminal;
    }

    if ((status == KeyRead::Interrupted) || console.mInterrupted.load(std::memory_order_relaxed))
    {
        line.clear();
    }
    else if (status == KeyRead::Closed)
    {
        console.mInputClosed.store(true, std::memory_order_relaxed);
    }

    line.finish();
    areg::trim_all<char>(buffer);
    const bool result{ areg::is_empty(buffer) == false };
    if (raw && result)
    {
        ScopedLock lock(console);
        _history.add(buffer);
    }

    return result;
}

} // namespace areg::ext
