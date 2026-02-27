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
        int32_t len = areg::isEmpty<char>(layoutFormat) == false ? areg::getStringLength<char>( layoutFormat ) : 0;
        char * strFormat = len > 0 ? DEBUG_NEW char[ static_cast<uint32_t>(len) + 1u ] : nullptr;

        if ( strFormat != nullptr )
        {
            areg::copyString<char, char>( strFormat, len + 1, layoutFormat, len );
            _createLayouts(strFormat);
            delete [] strFormat;
        }

        return (mLayoutList.isEmpty() == false);
    }

    bool LayoutManager::createLayouts(const areg::String& layoutFormat)
    {
        deleteLayouts();
        uint32_t len  = static_cast<uint32_t>(layoutFormat.getLength());
        char* strFormat = len != 0 ? DEBUG_NEW char[len + 1] : nullptr;

        if (strFormat != nullptr)
        {
            areg::copyString<char, char>(strFormat, static_cast<areg::CharCount>(len + 1), layoutFormat.getString(), static_cast<areg::CharCount>(len));
            _createLayouts(strFormat);
            delete[] strFormat;
        }

        return (mLayoutList.isEmpty() == false);
    }

    void LayoutManager::deleteLayouts()
    {
        const std::vector<areg::LogLayout*>& list{ mLayoutList.getData() };
        for (areg::LogLayout* layout : list)
        {
            delete layout;
        }

        mLayoutList.clear();
    }

    void LayoutManager::logMessage(const areg::LogEntry & logMsg, areg::OutStream & stream) const
    {
        if (logMsg.logMessagePrio == areg::LogPriority::PrioIgnoreLayout)
        {
            stream.write(logMsg.logMessage);
        }
        else
        {
            const std::vector<areg::LogLayout*>& list{ mLayoutList.getData() };
            for (const areg::LogLayout* layout : list)
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
        areg::LogLayout* anyText{ nullptr };

        while (*pos != areg::String::EmptyChar)
        {
            if (*pos == areg::SYNTAX_SPECIAL_FORMAT)
            {
                char ch = *(pos + 1);
                areg::LogLayout* newLayout{ nullptr };
                switch (static_cast<areg::LayoutToken>(ch))
                {
                case areg::LayoutToken::TickCount:
                    newLayout = DEBUG_NEW areg::TickCountLayout();
                    break;

                case areg::LayoutToken::DayTime:
                    newLayout = DEBUG_NEW areg::DayTimeLayout();
                    break;

                case areg::LayoutToken::ExecutableId:
                    newLayout = DEBUG_NEW areg::ModuleIdLayout();
                    break;

                case areg::LayoutToken::Message:
                    if (hasExclusive == false)
                    {
                        newLayout = DEBUG_NEW areg::MessageLayout();
                        hasExclusive = true;
                    }
                    break;

                case areg::LayoutToken::EndOfLine:
                    newLayout = DEBUG_NEW areg::EndOfLineLayout();
                    break;

                case areg::LayoutToken::Priority:
                    newLayout = DEBUG_NEW areg::PriorityLayout();
                    break;

                case areg::LayoutToken::ScopeId:
                    newLayout = DEBUG_NEW areg::ScopeIdLayout();
                    break;

                case areg::LayoutToken::ThreadId:
                    newLayout = DEBUG_NEW areg::ThreadIdLayout();
                    break;

                case areg::LayoutToken::ExecutableName:
                    newLayout = DEBUG_NEW areg::ModuleNameLayout();
                    break;

                case areg::LayoutToken::ThreadName:
                    newLayout = DEBUG_NEW areg::ThreadNameLayout();
                    break;

                case areg::LayoutToken::ScopeName:
                    if (hasExclusive == false)
                    {
                        newLayout = DEBUG_NEW areg::ScopeNameLayout();
                        hasExclusive = true;
                    }
                    break;

                case areg::LayoutToken::CookieId:
                    newLayout = DEBUG_NEW areg::CookieIdLayout();
                    break;

                case areg::LayoutToken::Undefined:  // fall through
                case areg::LayoutToken::AnyText:    // fall through
                default:
                    if (ch == areg::SYNTAX_SPECIAL_FORMAT)
                    {
                        *(pos + 1) = areg::String::EmptyChar;
                        newLayout = DEBUG_NEW areg::AnyTextLayout(pos1);
                        pos1 = pos; // <== will automatically move 2 positions when newLayout is not nullptr;
                    }
                    else
                    {
                        pos += ch != areg::String::EmptyChar ? 2 : 1;
                    }
                    break;
                }

                if (newLayout != nullptr)
                {
                    *pos = areg::String::EmptyChar;
                    anyText = pos1 != pos ? DEBUG_NEW areg::AnyTextLayout(pos1) : nullptr;
                    if (anyText != nullptr)
                    {
                        mLayoutList.add(anyText);
                    }

                    mLayoutList.add(newLayout);
                    *(++pos) = areg::String::EmptyChar;
                    pos1 = ++pos;
                }
            }
            else
            {
                ++pos;
            }
        }
    
        anyText = pos1 != pos ? DEBUG_NEW areg::AnyTextLayout(pos1) : nullptr;
        if (anyText != nullptr)
        {
            mLayoutList.add(anyText);
        }
    }

    #endif  // AREG_LOGS
} // namespace areg
