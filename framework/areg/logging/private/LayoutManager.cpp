/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/logging/private/LayoutManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The Layout Manager to output text messages.
 ************************************************************************/
#include "areg/logging/private/LayoutManager.hpp"

#include "areg/logging/private/Layouts.hpp"
#include "areg/logging/private/LogOptions.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <memory>
#include <new>
#include <utility>

#if AREG_LOGGING

namespace areg {

namespace
{
    //!< Creates a layout object. Sets 'created' to false if no memory is left.
    template<typename Layout, typename... Args>
    inline LogLayout* _new_layout(bool& created, Args&&... args)
    {
        LogLayout* result{ new (std::nothrow) Layout(std::forward<Args>(args)...) };
        created = created && (result != nullptr);
        return result;
    }
}

LayoutManager::~LayoutManager() noexcept
{
    delete_layouts();
}

bool LayoutManager::create_layouts( const char * layoutFormat )
{
    const areg::CharCount len{ areg::string_length<char>(layoutFormat) };
    return _parse_layouts(layoutFormat, static_cast<uint32_t>(len));
}

bool LayoutManager::create_layouts(const String& layoutFormat)
{
    return _parse_layouts(layoutFormat.as_string(), static_cast<uint32_t>(layoutFormat.length()));
}

bool LayoutManager::_parse_layouts(const char* layoutFormat, uint32_t len)
{
    constexpr uint32_t STACK_LEN{ 512 };
    delete_layouts();
    if (len == 0)
        return false;

    char stackFormat[STACK_LEN];
    std::unique_ptr<char[]> heapFormat{ len < STACK_LEN ? nullptr : new (std::nothrow) char[len + 1u] };
    char* format{ len < STACK_LEN ? stackFormat : heapFormat.get() };
    if (format == nullptr)
        return false;

    areg::copy_string<char, char>(format, static_cast<areg::CharCount>(len + 1u), layoutFormat, static_cast<areg::CharCount>(len));
    if (_create_layouts(format) == false)
    {
        delete_layouts();
    }

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
        stream.write(reinterpret_cast<const uint8_t*>(logMsg.logMessage), areg::log_message_size(logMsg) * sizeof(logMsg.logMessage[0]));
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

inline bool LayoutManager::_create_layouts(char* layoutFormat)
{
    ASSERT(layoutFormat != nullptr);

    bool created{ true };
    bool hasExclusive{ false };
    bool dropToken{ false };
    char* pos = layoutFormat;
    const char* pos1 = pos;
    LogLayout* anyText{ nullptr };

    while (created && (*pos != String::EmptyChar))
    {
        if (*pos == areg::SYNTAX_SPECIAL_FORMAT)
        {
            char ch = *(pos + 1);
            LogLayout* newLayout{ nullptr };
            switch (static_cast<areg::LayoutToken>(ch))
            {
            case areg::LayoutToken::TickCount:
                newLayout = _new_layout<TickCountLayout>(created);
                break;

            case areg::LayoutToken::DayTime:
                newLayout = _new_layout<DayTimeLayout>(created);
                break;

            case areg::LayoutToken::ExecutableId:
                newLayout = _new_layout<ModuleIdLayout>(created);
                break;

            case areg::LayoutToken::Message:
                if (!hasExclusive)
                {
                    newLayout = _new_layout<MessageLayout>(created);
                    hasExclusive = true;
                }
                else
                {
                    dropToken = true;
                }
                break;

            case areg::LayoutToken::EndOfLine:
                newLayout = _new_layout<EndOfLineLayout>(created);
                break;

            case areg::LayoutToken::Priority:
                newLayout = _new_layout<PriorityLayout>(created);
                break;

            case areg::LayoutToken::ScopeId:
                newLayout = _new_layout<ScopeIdLayout>(created);
                break;

            case areg::LayoutToken::ThreadId:
                newLayout = _new_layout<ThreadIdLayout>(created);
                break;

            case areg::LayoutToken::ExecutableName:
                newLayout = _new_layout<ModuleNameLayout>(created);
                break;

            case areg::LayoutToken::ThreadName:
                newLayout = _new_layout<ThreadNameLayout>(created);
                break;

            case areg::LayoutToken::ScopeName:
                if (!hasExclusive)
                {
                    newLayout = _new_layout<ScopeNameLayout>(created);
                    hasExclusive = true;
                }
                else
                {
                    dropToken = true;
                }
                break;

            case areg::LayoutToken::CookieId:
                newLayout = _new_layout<CookieIdLayout>(created);
                break;

            case areg::LayoutToken::Undefined:  // fall through
            case areg::LayoutToken::AnyText:    // fall through
            default:
                if (ch == areg::SYNTAX_SPECIAL_FORMAT)
                {
                    *(pos + 1) = String::EmptyChar;
                    newLayout = _new_layout<AnyTextLayout>(created, pos1);
                    pos1 = pos; // <== will automatically move 2 positions when newLayout is not nullptr;
                }
                else
                {
                    pos += ch != String::EmptyChar ? 2 : 1;
                }
                break;
            }

            if ((newLayout != nullptr) || dropToken)
            {
                *pos = String::EmptyChar;
                anyText = pos1 != pos ? _new_layout<AnyTextLayout>(created, pos1) : nullptr;
                if (anyText != nullptr)
                {
                    mLayoutList.add(anyText);
                }

                if (newLayout != nullptr)
                {
                    mLayoutList.add(newLayout);
                }

                dropToken = false;
                *(++pos) = String::EmptyChar;
                pos1 = ++pos;
            }
        }
        else
        {
            ++pos;
        }
    }
    
    anyText = created && (pos1 != pos) ? _new_layout<AnyTextLayout>(created, pos1) : nullptr;
    if (anyText != nullptr)
    {
        mLayoutList.add(anyText);
    }

    return created;
}

} // namespace areg

#endif  // AREG_LOGGING
