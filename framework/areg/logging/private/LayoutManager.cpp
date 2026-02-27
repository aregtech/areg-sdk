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

namespace areg
{

    #if AREG_LOGS

    LayoutManager::~LayoutManager()
    {
        deleteLayouts();
    }

    bool LayoutManager::createLayouts( const char * layoutFormat )
    {
        deleteLayouts();
        int32_t len = isEmpty<char>(layoutFormat) == false ? getStringLength<char>( layoutFormat ) : 0;
        char * strFormat = len > 0 ? DEBUG_NEW char[ static_cast<uint32_t>(len) + 1u ] : nullptr;

        if ( strFormat != nullptr )
        {
            copyString<char, char>( strFormat, len + 1, layoutFormat, len );
            _createLayouts(strFormat);
            delete [] strFormat;
        }

        return (mLayoutList.isEmpty() == false);
    }

    bool LayoutManager::createLayouts(const String& layoutFormat)
    {
        deleteLayouts();
        uint32_t len  = static_cast<uint32_t>(layoutFormat.getLength());
        char* strFormat = len != 0 ? DEBUG_NEW char[len + 1] : nullptr;

        if (strFormat != nullptr)
        {
            copyString<char, char>(strFormat, static_cast<CharCount>(len + 1), layoutFormat.getString(), static_cast<CharCount>(len));
            _createLayouts(strFormat);
            delete[] strFormat;
        }

        return (mLayoutList.isEmpty() == false);
    }

    void LayoutManager::deleteLayouts()
    {
        const std::vector<LogLayout*>& list{ mLayoutList.getData() };
        for (LogLayout* layout : list)
        {
            delete layout;
        }

        mLayoutList.clear();
    }

    void LayoutManager::logMessage(const LogEntry & logMsg, OutStream & stream) const
    {
        if (logMsg.logMessagePrio == LogPriority::PrioIgnoreLayout)
        {
            stream.write(logMsg.logMessage);
        }
        else
        {
            const std::vector<LogLayout*>& list{ mLayoutList.getData() };
            for (const LogLayout* layout : list)
            {
                layout->logMessage(logMsg, stream);
            }
        }
    }

    inline void LayoutManager::_createLayouts(char* layoutFormat)
    {
        ASSERT(layoutFormat != nullptr);

        bool hasExclusive{ false };
        char* pos = layoutFormat;
        const char* pos1 = pos;
        LogLayout* anyText{ nullptr };

        while (*pos != String::EmptyChar)
        {
            if (*pos == SYNTAX_SPECIAL_FORMAT)
            {
                char ch = *(pos + 1);
                LogLayout* newLayout{ nullptr };
                switch (static_cast<LayoutToken>(ch))
                {
                case LayoutToken::TickCount:
                    newLayout = DEBUG_NEW TickCountLayout();
                    break;

                case LayoutToken::DayTime:
                    newLayout = DEBUG_NEW DayTimeLayout();
                    break;

                case LayoutToken::ExecutableId:
                    newLayout = DEBUG_NEW ModuleIdLayout();
                    break;

                case LayoutToken::Message:
                    if (hasExclusive == false)
                    {
                        newLayout = DEBUG_NEW MessageLayout();
                        hasExclusive = true;
                    }
                    break;

                case LayoutToken::EndOfLine:
                    newLayout = DEBUG_NEW EndOfLineLayout();
                    break;

                case LayoutToken::Priority:
                    newLayout = DEBUG_NEW PriorityLayout();
                    break;

                case LayoutToken::ScopeId:
                    newLayout = DEBUG_NEW ScopeIdLayout();
                    break;

                case LayoutToken::ThreadId:
                    newLayout = DEBUG_NEW ThreadIdLayout();
                    break;

                case LayoutToken::ExecutableName:
                    newLayout = DEBUG_NEW ModuleNameLayout();
                    break;

                case LayoutToken::ThreadName:
                    newLayout = DEBUG_NEW ThreadNameLayout();
                    break;

                case LayoutToken::ScopeName:
                    if (hasExclusive == false)
                    {
                        newLayout = DEBUG_NEW ScopeNameLayout();
                        hasExclusive = true;
                    }
                    break;

                case LayoutToken::CookieId:
                    newLayout = DEBUG_NEW CookieIdLayout();
                    break;

                case LayoutToken::Undefined:  // fall through
                case LayoutToken::AnyText:    // fall through
                default:
                    if (ch == SYNTAX_SPECIAL_FORMAT)
                    {
                        *(pos + 1) = String::EmptyChar;
                        newLayout = DEBUG_NEW AnyTextLayout(pos1);
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
                    anyText = pos1 != pos ? DEBUG_NEW AnyTextLayout(pos1) : nullptr;
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
    
        anyText = pos1 != pos ? DEBUG_NEW AnyTextLayout(pos1) : nullptr;
        if (anyText != nullptr)
        {
            mLayoutList.add(anyText);
        }
    }

    #endif  // AREG_LOGS
} // namespace areg
