/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LayoutManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The Layout Manager to output text messages.
 ************************************************************************/
#include "areg/logging/private/LayoutManager.hpp"

#include "areg/logging/private/Layouts.hpp"
#include "areg/logging/private/LogOptions.hpp"
#include "areg/base/UtilityDefs.hpp"
#if AREG_LOGGING

namespace areg {

LayoutManager::~LayoutManager() noexcept
{
    delete_layouts();
}

bool LayoutManager::create_layouts( const char * layoutFormat )
{
    delete_layouts();
    int32_t len = !areg::is_empty<char>(layoutFormat) ? areg::string_length<char>( layoutFormat ) : 0;
    char * strFormat = len > 0 ? new char[ static_cast<uint32_t>(len) + 1u ] : nullptr;

    if ( strFormat != nullptr )
    {
        areg::copy_string<char, char>( strFormat, len + 1, layoutFormat, len );
        _create_layouts(strFormat);
        delete [] strFormat;
    }

    return (!mLayoutList.is_empty());
}

bool LayoutManager::create_layouts(const String& layoutFormat)
{
    constexpr uint32_t fmt_len{ 512 };
    delete_layouts();
    uint32_t len  = static_cast<uint32_t>(layoutFormat.length());
    char fmt[fmt_len] = { 0 };
    ASSERT(len < fmt_len);

    areg::copy_string<char, char>(fmt, static_cast<areg::CharCount>(len + 1), layoutFormat.as_string(), static_cast<areg::CharCount>(len));
    _create_layouts(fmt);

    return (!mLayoutList.is_empty());
}

void LayoutManager::delete_layouts() noexcept
{
    const std::vector<LogLayout*>& list{ mLayoutList.data() };
    for (LogLayout* layout : list)
    {
        delete layout;
    }

    mLayoutList.clear();
}

void LayoutManager::log_message(const areg::LogEntry & logMsg, OutStream & stream) const
{
    if (logMsg.logMessagePrio == areg::LogPriority::PrioIgnoreLayout)
    {
        stream.write(reinterpret_cast<const uint8_t*>(logMsg.logMessage), logMsg.logMessageLen * sizeof(logMsg.logMessage[0]));
    }
    else
    {
        const std::vector<LogLayout*>& list{ mLayoutList.data() };
        for (const LogLayout* layout : list)
        {
            layout->log_message(logMsg, stream);
        }
    }
}

inline void LayoutManager::_create_layouts(char* layoutFormat)
{
    ASSERT(layoutFormat != nullptr);

    bool hasExclusive{ false };
    char* pos = layoutFormat;
    const char* pos1 = pos;
    LogLayout* anyText{ nullptr };

    while (*pos != String::EmptyChar)
    {
        if (*pos == areg::SYNTAX_SPECIAL_FORMAT)
        {
            char ch = *(pos + 1);
            LogLayout* newLayout{ nullptr };
            switch (static_cast<areg::LayoutToken>(ch))
            {
            case areg::LayoutToken::TickCount:
                newLayout = new TickCountLayout();
                break;

            case areg::LayoutToken::DayTime:
                newLayout = new DayTimeLayout();
                break;

            case areg::LayoutToken::ExecutableId:
                newLayout = new ModuleIdLayout();
                break;

            case areg::LayoutToken::Message:
                if (!hasExclusive)
                {
                    newLayout = new MessageLayout();
                    hasExclusive = true;
                }
                break;

            case areg::LayoutToken::EndOfLine:
                newLayout = new EndOfLineLayout();
                break;

            case areg::LayoutToken::Priority:
                newLayout = new PriorityLayout();
                break;

            case areg::LayoutToken::ScopeId:
                newLayout = new ScopeIdLayout();
                break;

            case areg::LayoutToken::ThreadId:
                newLayout = new ThreadIdLayout();
                break;

            case areg::LayoutToken::ExecutableName:
                newLayout = new ModuleNameLayout();
                break;

            case areg::LayoutToken::ThreadName:
                newLayout = new ThreadNameLayout();
                break;

            case areg::LayoutToken::ScopeName:
                if (!hasExclusive)
                {
                    newLayout = new ScopeNameLayout();
                    hasExclusive = true;
                }
                break;

            case areg::LayoutToken::CookieId:
                newLayout = new CookieIdLayout();
                break;

            case areg::LayoutToken::Undefined:  // fall through
            case areg::LayoutToken::AnyText:    // fall through
            default:
                if (ch == areg::SYNTAX_SPECIAL_FORMAT)
                {
                    *(pos + 1) = String::EmptyChar;
                    newLayout = new AnyTextLayout(pos1);
                    pos1 = pos; // <== will automatically move 2 positions when newLayout is not nullptr;
                }
                else
                {
                    pos += ch != String::EmptyChar ? 2 : 1;
                }
                break;
            }

            if (newLayout != nullptr)
            {
                *pos = String::EmptyChar;
                anyText = pos1 != pos ? new AnyTextLayout(pos1) : nullptr;
                if (anyText != nullptr)
                {
                    mLayoutList.add(anyText);
                }

                mLayoutList.add(newLayout);
                *(++pos) = String::EmptyChar;
                pos1 = ++pos;
            }
        }
        else
        {
            ++pos;
        }
    }
    
    anyText = pos1 != pos ? new AnyTextLayout(pos1) : nullptr;
    if (anyText != nullptr)
    {
        mLayoutList.add(anyText);
    }
}

} // namespace areg

#endif  // AREG_LOGGING
