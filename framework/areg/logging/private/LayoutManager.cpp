/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LayoutManager.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The Layout Manager to output text messages.
 ************************************************************************/
#include "areg/logging/private/LayoutManager.hpp"

#include "areg/logging/private/Layouts.hpp"
#include "areg/logging/private/NELogOptions.hpp"
#include "areg/base/NEUtilities.hpp"

#if AREG_LOGS

LayoutManager::~LayoutManager(void)
{
    deleteLayouts();
}

bool LayoutManager::createLayouts( const char * layoutFormat )
{
    deleteLayouts();
    int len = NEString::isEmpty<char>(layoutFormat) == false ? NEString::getStringLength<char>( layoutFormat ) : 0;
    char * strFormat = len > 0 ? DEBUG_NEW char[ static_cast<unsigned int>(len) + 1u ] : nullptr;

    if ( strFormat != nullptr )
    {
        NEString::copyString<char, char>( strFormat, len + 1, layoutFormat, len );
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
        NEString::copyString<char, char>(strFormat, static_cast<NEString::CharCount>(len + 1), layoutFormat.getString(), static_cast<NEString::CharCount>(len));
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

void LayoutManager::logMessage(const NELogging::sLogMessage & logMsg, IEOutStream & stream) const
{
    if (logMsg.logMessagePrio == NELogging::PrioIgnoreLayout)
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
    IELayout* anyText{ nullptr };

    while (*pos != String::EmptyChar)
    {
        if (*pos == NELogOptions::SYNTAX_SPECIAL_FORMAT)
        {
            char ch = *(pos + 1);
            IELayout* newLayout{ nullptr };
            switch (static_cast<NELogOptions::eLayouts>(ch))
            {
            case NELogOptions::eLayouts::LayoutTickCount:
                newLayout = DEBUG_NEW TickCountLayout();
                break;

            case NELogOptions::eLayouts::LayoutDayTime:
                newLayout = DEBUG_NEW DayTimeLaytout();
                break;

            case NELogOptions::eLayouts::LayoutExecutableId:
                newLayout = DEBUG_NEW ModuleIdLayout();
                break;

            case NELogOptions::eLayouts::LayoutMessage:
                if (hasExclusive == false)
                {
                    newLayout = DEBUG_NEW MessageLayout();
                    hasExclusive = true;
                }
                break;

            case NELogOptions::eLayouts::LayoutEndOfLine:
                newLayout = DEBUG_NEW EndOfLineLayout();
                break;

            case NELogOptions::eLayouts::LayoutPriority:
                newLayout = DEBUG_NEW PriorityLayout();
                break;

            case NELogOptions::eLayouts::LaytoutScopeId:
                newLayout = DEBUG_NEW ScopeIdLayout();
                break;

            case NELogOptions::eLayouts::LayoutThreadId:
                newLayout = DEBUG_NEW ThreadIdLayout();
                break;

            case NELogOptions::eLayouts::LayoutExecutableName:
                newLayout = DEBUG_NEW ModuleNameLayout();
                break;

            case NELogOptions::eLayouts::LayoutThreadName:
                newLayout = DEBUG_NEW ThreadNameLayout();
                break;

            case NELogOptions::eLayouts::LaytoutScopeName:
                if (hasExclusive == false)
                {
                    newLayout = DEBUG_NEW ScopeNameLayout();
                    hasExclusive = true;
                }
                break;

            case NELogOptions::eLayouts::LayoutCookieId:
                newLayout = DEBUG_NEW CookieIdLayout();
                break;

            case NELogOptions::eLayouts::LayoutUndefined:  // fall through
            case NELogOptions::eLayouts::LayoutAnyText:    // fall through
            default:
                if (ch == NELogOptions::SYNTAX_SPECIAL_FORMAT)
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
