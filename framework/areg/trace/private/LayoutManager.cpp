/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LayoutManager.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The Layout Manager to output text messages.
 ************************************************************************/
#include "areg/trace/private/LayoutManager.hpp"

#include "areg/trace/private/Layouts.hpp"
#include "areg/trace/private/NELogging.hpp"
#include "areg/base/NEUtilities.hpp"

#if AREG_LOGS

LayoutManager::~LayoutManager(void)
{
    deleteLayouts();
}

bool LayoutManager::createLayouts( const char * layoutFormat )
{
    deleteLayouts();
    int len = NEString::isEmpty<char>(layoutFormat) == false ? NEString::getStringLength<char>( layoutFormat ) + 1 : 0;
    char * strFormat = len > 0 ? DEBUG_NEW char[ static_cast<unsigned int>(len) ] : nullptr;

    if ( strFormat != nullptr )
    {
        NEString::copyString<char, char>( strFormat, len, layoutFormat, len );
        _createLayouts(strFormat);
        delete [] strFormat;
    }

    return (mLayoutList.isEmpty() == false);
}

bool LayoutManager::createLayouts(const String& layoutFormat)
{
    deleteLayouts();
    uint32_t len = layoutFormat.getLength();
    uint32_t size = len + 1u;
    char* strFormat = size > 1 ? DEBUG_NEW char[size] : nullptr;

    if (strFormat != nullptr)
    {
        NEString::copyString<char, char>(strFormat, len, layoutFormat.getString(), len);
        _createLayouts(strFormat);
        delete[] strFormat;
    }

    return (mLayoutList.isEmpty() == false);
}

void LayoutManager::deleteLayouts(void)
{
    const std::vector<IELayout*>& list{ mLayoutList.getData() };
    for (IELayout* layout : list)
    {
        delete layout;
    }

    mLayoutList.clear();
}

void LayoutManager::logMessage(const NETrace::sLogMessage & logMsg, IEOutStream & stream) const
{
    if (logMsg.logMessagePrio == NETrace::PrioIgnoreLayout)
    {
        stream.write(logMsg.logMessage);
    }
    else
    {
        const std::vector<IELayout*>& list{ mLayoutList.getData() };
        for (const IELayout* layout : list)
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

    while (*pos != String::EmptyChar)
    {
        if (*pos == NELogging::SYNTAX_SPECIAL_FORMAT)
        {
            char ch = *(pos + 1);
            IELayout* newLayout{ nullptr };
            switch (static_cast<NELogging::eLayouts>(ch))
            {
            case NELogging::eLayouts::LayoutTickCount:
                newLayout = DEBUG_NEW TickCountLayout();
                break;

            case NELogging::eLayouts::LayoutDayTime:
                newLayout = DEBUG_NEW DayTimeLaytout();
                break;

            case NELogging::eLayouts::LayoutExecutableId:
                newLayout = DEBUG_NEW ModuleIdLayout();
                break;

            case NELogging::eLayouts::LayoutMessage:
                if (hasExclusive == false)
                {
                    newLayout = DEBUG_NEW MessageLayout();
                    hasExclusive = true;
                }
                break;

            case NELogging::eLayouts::LayoutEndOfLine:
                newLayout = DEBUG_NEW EndOfLineLayout();
                break;

            case NELogging::eLayouts::LayoutPriority:
                newLayout = DEBUG_NEW PriorityLayout();
                break;

            case NELogging::eLayouts::LaytoutScopeId:
                newLayout = DEBUG_NEW ScopeIdLayout();
                break;

            case NELogging::eLayouts::LayoutThreadId:
                newLayout = DEBUG_NEW ThreadIdLayout();
                break;

            case NELogging::eLayouts::LayoutExecutableName:
                newLayout = DEBUG_NEW ModuleNameLayout();
                break;

            case NELogging::eLayouts::LayoutThreadName:
                newLayout = DEBUG_NEW ThreadNameLayout();
                break;

            case NELogging::eLayouts::LaytoutScopeName:
                if (hasExclusive == false)
                {
                    newLayout = DEBUG_NEW ScopeNameLayout();
                    hasExclusive = true;
                }
                break;

            case NELogging::eLayouts::LayoutCookieId:
                newLayout = DEBUG_NEW CookieIdLayout();
                break;

            case NELogging::eLayouts::LayoutUndefined:  // fall through
            case NELogging::eLayouts::LayoutAnyText:    // fall through
            default:
                if (ch == NELogging::SYNTAX_SPECIAL_FORMAT)
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
                if (pos1 != pos)
                {
                    mLayoutList.add(static_cast<IELayout*>(DEBUG_NEW AnyTextLayout(pos1)));
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

    if (pos1 != pos)
    {
        mLayoutList.add(static_cast<IELayout*>(DEBUG_NEW AnyTextLayout(pos1)));
    }
}

#endif  // AREG_LOGS
