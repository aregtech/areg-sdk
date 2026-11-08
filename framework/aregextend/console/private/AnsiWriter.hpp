#ifndef AREG_AREGEXTEND_CONSOLE_PRIVATE_ANSIWRITER_HPP
#define AREG_AREGEXTEND_CONSOLE_PRIVATE_ANSIWRITER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/private/AnsiWriter.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the output buffer of the ANSI console.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/MathDefs.hpp"

#include <string_view>

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// AnsiWriter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Collects the bytes of ANSI escape sequences and text in a fixed buffer and
 *          passes them to a sink. Coordinates are 0-based, column in posX and row in posY.
 *          A positioned text is clipped to the screen, so it never wraps and never scrolls.
 *          The buffer is passed to the sink when it is full or when flush() is called.
 **/
class AnsiWriter
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    //!< Receives the collected bytes.
    using Sink = void (*)(const char* data, uint32_t size, void* context);

    //!< The size of the buffer.
    static constexpr uint32_t   CAPACITY    { 2048u };

    //!< The width of a tab stop.
    static constexpr int32_t    TAB_SIZE    { 8 };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    constexpr AnsiWriter(Sink sink, void* context) noexcept
        : mSink     (sink)
        , mContext  (context)
        , mColumns  (0)
        , mRows     (0)
        , mUsed     (0u)
        , mData     { }
    {
    }

    ~AnsiWriter() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets the screen size. A value of 0 or less means unknown, nothing is clipped on that axis.
     **/
    void set_screen(int32_t columns, int32_t rows) noexcept;

    /**
     * \brief   Returns the screen size set last, columns in posX and rows in posY.
     **/
    inline areg::Coord screen() const noexcept;

    /**
     * \brief   Moves the cursor to the position.
     **/
    void move_to(areg::Coord pos) noexcept;

    /**
     * \brief   Clears the row of the position from its column to the right edge.
     *          Does nothing if the position is outside the screen.
     **/
    void clear_at(areg::Coord pos) noexcept;

    /**
     * \brief   Writes the text at the position. Each row is cleared from the column to the
     *          right edge first. A new line continues on the next row at the same column.
     *          Escape sequences pass through and take no column. Other control characters
     *          are dropped and a tab is expanded to spaces.
     *
     * \return  Returns the last row the text was drawn on, or -1 if nothing was drawn.
     **/
    int32_t text_at(areg::Coord pos, std::string_view text) noexcept;

    /**
     * \brief   Draws an input field at the position: the part of the text that fits the screen
     *          and keeps the caret visible. The rest of the row is cleared.
     *
     * \param   pos     The position of the first character of the field.
     * \param   text    The text of the field, printable ASCII only.
     * \param   caret   The index of the caret in the text.
     * \return  Returns the screen position of the caret.
     **/
    areg::Coord field(areg::Coord pos, std::string_view text, uint32_t caret) noexcept;

    /**
     * \brief   Writes the text without stripping or clipping anything.
     **/
    void raw(std::string_view text) noexcept;

    /**
     * \brief   Writes the text as plain lines: escape sequences and control characters other
     *          than new line and tab are dropped. A new line is added if the text has none at
     *          the end. An empty text writes nothing.
     **/
    void plain_line(std::string_view text) noexcept;

    /**
     * \brief   Writes the text with the escape sequences and carriage returns dropped.
     **/
    void plain(std::string_view text) noexcept;

    /**
     * \brief   Hides the cursor.
     **/
    void hide_cursor() noexcept;

    /**
     * \brief   Shows the cursor.
     **/
    void show_cursor() noexcept;

    /**
     * \brief   Passes the collected bytes to the sink.
     **/
    void flush() noexcept;

    /**
     * \brief   Returns the number of collected bytes not passed to the sink yet.
     **/
    inline uint32_t pending() const noexcept;

    /**
     * \brief   Returns the length of the escape sequence that starts at the index, or 0 if it is
     *          not complete. Handles CSI, OSC and two byte sequences.
     **/
    static uint32_t escape_length(std::string_view text, uint32_t index) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    //!< Appends bytes, passing the buffer to the sink when it is full.
    void _put(const char* data, uint32_t size) noexcept;

    //!< Appends one byte.
    inline void _put(char ch) noexcept;

    //!< Appends a positive decimal number.
    void _put_number(int32_t value) noexcept;

    //!< Appends one row of text starting at the column, at most 'limit' columns wide.
    void _put_row(std::string_view row, int32_t column, int32_t limit) noexcept;

    //!< Returns the number of columns a text at the position may use, or 0 if it is outside the screen.
    int32_t _room(areg::Coord pos) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    Sink        mSink;              //!< The receiver of the collected bytes.
    void*       mContext;           //!< The context passed to the sink.
    int32_t     mColumns;           //!< The number of columns, 0 if unknown.
    int32_t     mRows;              //!< The number of rows, 0 if unknown.
    uint32_t    mUsed;              //!< The number of collected bytes.
    char        mData[CAPACITY];    //!< The collected bytes.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(AnsiWriter);
};

//////////////////////////////////////////////////////////////////////////
// AnsiWriter class inline methods
//////////////////////////////////////////////////////////////////////////

inline areg::Coord AnsiWriter::screen() const noexcept
{
    return areg::Coord{ mColumns, mRows };
}

inline uint32_t AnsiWriter::pending() const noexcept
{
    return mUsed;
}

inline void AnsiWriter::_put(char ch) noexcept
{
    if (mUsed == CAPACITY)
    {
        flush();
    }

    mData[mUsed++] = ch;
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_CONSOLE_PRIVATE_ANSIWRITER_HPP
