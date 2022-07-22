/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LayoutManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       The Layout Manager to output text messages.
 ************************************************************************/
#include "areg/trace/private/LayoutManager.hpp"

#include "areg/trace/private/Layouts.hpp"
#include "areg/trace/private/NELogConfig.hpp"
#include "areg/base/NEUtilities.hpp"

LayoutManager::~LayoutManager(void)
{
    deleteLayouts();
}

bool LayoutManager::createLayouts( const char * layoutFormat )
{
    deleteLayouts();
    int len = NEString::isEmpty<char>(layoutFormat) == false ? NEString::getStringLength<char>( layoutFormat ) + 1 : 0;
    char * strFormat = len != 0 ? DEBUG_NEW char[ static_cast<unsigned int>(len) ] : nullptr;

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
    uint32_t len = layoutFormat.isEmpty() ? 0u : layoutFormat.getLength() + 1;
    char* strFormat = len != 0 ? DEBUG_NEW char[len] : nullptr;

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
    for (uint32_t i = 0; i < mLayoutList.getSize(); ++i)
    {
        delete static_cast<IELayout*>(mLayoutList.getAt(i));
    }

    mLayoutList.clear();
}

void LayoutManager::logMessage(const NETrace::sLogMessage & logMsg, IEOutStream & stream) const
{
    if (isValid())
    {
        if ( logMsg.lmTrace.traceMessagePrio == NETrace::PrioIgnoreLayout )
        {
            stream.write( logMsg.lmTrace.traceMessage );
        }
        else
        {
            uint32_t size = mLayoutList.getSize();
            for (uint32_t i = 0; i < size; ++i)
            {
                mLayoutList.getAt(i)->logMessage(logMsg, stream);
            }
        }
    }
}

inline void LayoutManager::_createLayouts(char* layoutFormat)
{
    char* pos{ layoutFormat };
    bool hasExclusive{ false };
    const char* pos1 = pos;

    while (*pos != '\0')
    {
        if (*pos == '%')
        {
            char ch = *(pos + 1);
            IELayout* newLayout = nullptr;
            switch (static_cast<NELogConfig::eLayouts>(ch))
            {
            case NELogConfig::eLayouts::LayoutTickCount:
                newLayout = DEBUG_NEW TickCountLayout();
                break;

            case NELogConfig::eLayouts::LayoutDayTime:
                newLayout = DEBUG_NEW DayTimeLaytout();
                break;

            case NELogConfig::eLayouts::LayoutExecutableId:
                newLayout = DEBUG_NEW ModuleIdLayout();
                break;

            case NELogConfig::eLayouts::LayoutMessage:
                if (hasExclusive == false)
                {
                    newLayout = DEBUG_NEW MessageLayout();
                    hasExclusive = true;
                }
                break;

            case NELogConfig::eLayouts::LayoutEndOfLine:
                newLayout = DEBUG_NEW EndOfLineLayout();
                break;

            case NELogConfig::eLayouts::LayoutPriority:
                newLayout = DEBUG_NEW PriorityLayout();
                break;

            case NELogConfig::eLayouts::LaytoutScopeId:
                newLayout = DEBUG_NEW ScopeIdLayout();
                break;

            case NELogConfig::eLayouts::LayoutThreadId:
                newLayout = DEBUG_NEW ThreadIdLayout();
                break;

            case NELogConfig::eLayouts::LayoutExecutableName:
                newLayout = DEBUG_NEW ModuleNameLayout();
                break;

            case NELogConfig::eLayouts::LayoutThreadName:
                newLayout = DEBUG_NEW ThreadNameLayout();
                break;

            case NELogConfig::eLayouts::LaytoutScopeName:
                if (hasExclusive == false)
                {
                    newLayout = DEBUG_NEW ScopeNameLayout();
                    hasExclusive = true;
                }
                break;

            case NELogConfig::eLayouts::LayoutUndefined:  // fall through
            case NELogConfig::eLayouts::LayoutAnyText:    // fall through
            default:
                if (ch == '%')
                {
                    *(pos + 1) = '\0';
                    newLayout = DEBUG_NEW AnyTextLayout(pos1);
                    pos1 = pos; // <== will automatically move 2 positions when newLayout is not nullptr;
                }
                else
                {
                    pos += ch != '\0' ? 2 : 1;
                }
                break;
            }

            if (newLayout != nullptr)
            {
                *pos = '\0';
                if (pos1 != pos)
                {
                    mLayoutList.add(static_cast<IELayout*>(DEBUG_NEW AnyTextLayout(pos1)));
                }

                mLayoutList.add(newLayout);

                *(++pos) = '\0';
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