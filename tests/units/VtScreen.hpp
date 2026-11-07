#ifndef AREG_TESTS_UNITS_VTSCREEN_HPP
#define AREG_TESTS_UNITS_VTSCREEN_HPP

/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 ************************************************************************/

/************************************************************************
 * A terminal screen of a fixed size that renders ANSI output the way a terminal does:
 * text wraps at the right edge and a line feed on the bottom row scrolls the screen.
 * It counts the wraps and the scrolls, so a test can check where the text landed.
 ************************************************************************/

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

class VtScreen
{
public:
    VtScreen(int columns, int rows)
        : mColumns(columns)
        , mRows(rows)
        , mScreen(static_cast<size_t>(rows), std::string(static_cast<size_t>(columns), ' '))
    {
    }

    //!< Renders the bytes. A sequence split between two calls is completed by the next one.
    void feed(std::string_view data)
    {
        for (char byte : data)
        {
            const unsigned char ch{ static_cast<unsigned char>(byte) };
            if (!mEscape.empty())
            {
                mEscape += byte;
                if (_escape_complete())
                {
                    _escape();
                    mEscape.clear();
                }
            }
            else if (ch == 0x1Bu)
                mEscape = "\x1B";
            else if (ch == '\r')
                _column(0);
            else if (ch == '\n')
                _line_feed();
            else if (ch == '\b')
                _column(mX - 1);
            else if (ch == '\t')
                _column(std::min(mColumns - 1, (mX / 8 + 1) * 8));
            else if ((ch >= 0x20u) && ((ch & 0xC0u) != 0x80u))
                _put((ch < 0x80u) ? byte : '?');
        }
    }

    //!< Returns the row without the spaces at the end.
    std::string row(int index) const
    {
        std::string text{ mScreen[static_cast<size_t>(index)] };
        text.erase(text.find_last_not_of(' ') + 1u);
        return text;
    }

    //!< Returns true if any row holds the token.
    bool shows(std::string_view token) const
    {
        return std::any_of(mScreen.begin(), mScreen.end(), [&](const std::string& line) { return line.find(token) != std::string::npos; });
    }

    int  scrolled() const       { return mScrolled; }
    int  wrapped() const        { return mWrapped; }
    int  cursor_x() const       { return mX; }
    int  cursor_y() const       { return mY; }
    bool cursor_visible() const { return mVisible; }

private:
    bool _escape_complete() const
    {
        const size_t size{ mEscape.size() };
        if (size < 2u)
            return false;
        if (mEscape[1] == '[')
            return (size > 2u) && (mEscape.back() >= 0x40) && (mEscape.back() <= 0x7E);
        if (mEscape[1] == ']')
            return (mEscape.back() == '\x07') || ((size > 3u) && (mEscape[size - 2u] == '\x1B') && (mEscape.back() == '\\'));
        if ((mEscape[1] >= 0x20) && (mEscape[1] <= 0x2F))
            return (size > 2u) && (mEscape.back() >= 0x30);
        return true;
    }

    void _escape()
    {
        if (mEscape[1] != '[')
            return;

        const bool isPrivate{ (mEscape.size() > 2u) && (mEscape[2] == '?') };
        std::vector<int> values(1u, 0);
        for (size_t i = isPrivate ? 3u : 2u; i + 1u < mEscape.size(); ++i)
        {
            if (mEscape[i] == ';')
                values.push_back(0);
            else if ((mEscape[i] >= '0') && (mEscape[i] <= '9'))
                values.back() = values.back() * 10 + (mEscape[i] - '0');
        }

        const char final{ mEscape.back() };
        const int count{ std::max(1, values[0]) };
        if (isPrivate)
        {
            if (values[0] == 25)
                mVisible = (final == 'h');
            else if (values[0] == 7)
                mAutoWrap = (final == 'h');
            return;
        }

        switch (final)
        {
        case 'H': case 'f':
            mY = std::clamp(count - 1, 0, mRows - 1);
            mX = std::clamp((values.size() > 1u ? std::max(1, values[1]) : 1) - 1, 0, mColumns - 1);
            mPending = false;
            break;
        case 'A': mY = std::max(0, mY - count); mPending = false; break;
        case 'B': mY = std::min(mRows - 1, mY + count); mPending = false; break;
        case 'C': _column(mX + count); break;
        case 'D': _column(mX - count); break;
        case 'E': mY = std::min(mRows - 1, mY + count); _column(0); break;
        case 'F': mY = std::max(0, mY - count); _column(0); break;
        case 'G': _column(count - 1); break;
        case 'd': mY = std::clamp(count - 1, 0, mRows - 1); mPending = false; break;
        case 'X':
        {
            const int width{ std::min(count, mColumns - mX) };
            mScreen[static_cast<size_t>(mY)].replace(static_cast<size_t>(mX), static_cast<size_t>(width), static_cast<size_t>(width), ' ');
            break;
        }
        case 'K':
        {
            const int from{ values[0] == 0 ? mX : 0 };
            const int to{ values[0] == 1 ? mX + 1 : mColumns };
            mScreen[static_cast<size_t>(mY)].replace(static_cast<size_t>(from), static_cast<size_t>(to - from), static_cast<size_t>(to - from), ' ');
            break;
        }
        case 'J':
            if (values[0] == 2 || values[0] == 3)
                std::fill(mScreen.begin(), mScreen.end(), std::string(static_cast<size_t>(mColumns), ' '));
            break;
        default:
            break;
        }
    }

    void _column(int x)
    {
        mX = std::clamp(x, 0, mColumns - 1);
        mPending = false;
    }

    void _line_feed()
    {
        mPending = false;
        if (mY == mRows - 1)
        {
            mScreen.erase(mScreen.begin());
            mScreen.emplace_back(static_cast<size_t>(mColumns), ' ');
            ++mScrolled;
        }
        else
        {
            ++mY;
        }
    }

    void _put(char ch)
    {
        if (mPending && mAutoWrap)
        {
            mX = 0;
            _line_feed();
            ++mWrapped;
        }

        mScreen[static_cast<size_t>(mY)][static_cast<size_t>(mX)] = ch;
        if (mX == mColumns - 1)
            mPending = mAutoWrap;
        else
            ++mX;
    }

    int mColumns;
    int mRows;
    std::vector<std::string> mScreen;
    std::string mEscape;
    int  mX{ 0 };
    int  mY{ 0 };
    int  mScrolled{ 0 };
    int  mWrapped{ 0 };
    bool mPending{ false };
    bool mAutoWrap{ true };
    bool mVisible{ true };
};

#endif  // AREG_TESTS_UNITS_VTSCREEN_HPP
