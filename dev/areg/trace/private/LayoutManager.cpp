/************************************************************************
 * \file        areg/trace/private/LayoutManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The Layout Manager to output text messages.
 ************************************************************************/
#include "areg/trace/private/LayoutManager.hpp"

#include "areg/trace/private/ELayouts.hpp"
#include "areg/trace/private/NELogConfig.hpp"
#include "areg/base/NEUtilities.hpp"

LayoutManager::LayoutManager(void)
    : mLayoutList   ( )
{
    ; // do nothing
}

LayoutManager::~LayoutManager(void)
{
    deleteLayouts();
}

bool LayoutManager::createLayouts( const char * layoutFormat )
{
    deleteLayouts();
    int len = NEString::isEmpty<char>(layoutFormat) == false ? NEString::getStringLength<char>( layoutFormat ) + 1 : 0;
    char * strFormat = len != 0 ? DEBUG_NEW char[ static_cast<unsigned int>(len) ] : NULL;

    if ( strFormat != NULL )
    {
        NEString::copyString( strFormat, len, layoutFormat );
        char * pos          = strFormat;
        const char * pos1   = pos;
        bool hasExclusive   = false;

        while ( *pos != '\0' )
        {
            if ( *pos == '%' )
            {
                char ch = *(pos + 1);
                IELayout * newLayout = NULL;
                switch ( static_cast<NELogConfig::eLayouts>(ch) )
                {
                case NELogConfig::LayoutTickCount:
                    newLayout   = DEBUG_NEW TickCountLayout();
                    break;

                case NELogConfig::LayoutDayTime:
                    newLayout   = DEBUG_NEW DayTimeLaytout();
                    break;

                case NELogConfig::LayoutExecutableId:
                    newLayout   = DEBUG_NEW ModuleIdLayout();
                    break;

                case NELogConfig::LayoutMessage:
                    if ( hasExclusive == false )
                    {
                        newLayout   = DEBUG_NEW MessageLayout();
                        hasExclusive= true;
                    }
                    break;

                case NELogConfig::LayoutEndOfLine:
                    newLayout   = DEBUG_NEW EndOfLineLayout();
                    break;

                case NELogConfig::LayoutPriority:
                    newLayout   = DEBUG_NEW PriorityLayout();
                    break;

                case NELogConfig::LaytoutScopeId:
                    newLayout   = DEBUG_NEW ScopeIdLayout();
                    break;

                case NELogConfig::LayoutThreadId:
                    newLayout   = DEBUG_NEW ThreadIdLayout();
                    break;

                case NELogConfig::LayoutExecutableName:
                    newLayout   = DEBUG_NEW ModuleNameLayout();
                    break;

                case NELogConfig::LayoutThreadName:
                    newLayout   = DEBUG_NEW ThreadNameLayout();
                    break;

                case NELogConfig::LaytoutScopeName:
                    if ( hasExclusive == false )
                    {
                        newLayout   = DEBUG_NEW ScopeNameLayout();
                        hasExclusive= true;
                    }
                    break;

                case NELogConfig::LayoutUndefined:  // fall through
                case NELogConfig::LayoutAnyText:    // fall through
                default:
                    if ( ch == '%' )
                    {
                        *(pos + 1) = '\0';
                        newLayout   = DEBUG_NEW AnyTextLayout(pos1);
                        pos1 = pos; // <== will automatically move 2 positions when newLayout is not NULL;
                    }
                    else
                    {
                        pos += ch != '\0' ? 2 : 1;
                    }
                    break;
                }

                if ( newLayout != NULL )
                {
                    *pos = '\0';
                    if ( pos1 != static_cast<const char *>(pos) )
                        mLayoutList.pushLast( static_cast<IELayout *>(DEBUG_NEW AnyTextLayout(pos1)) );

                    mLayoutList.pushLast( newLayout );

                    *(++ pos) = '\0';
                    pos1 = ++ pos;                        
                }
            }
            else
            {
                ++ pos;
            }
        }

        if ( pos1 != static_cast<const char *>(pos) )
            mLayoutList.pushLast( static_cast<IELayout *>(DEBUG_NEW AnyTextLayout(pos1)) );

        delete [] strFormat;
    }

    return (mLayoutList.isEmpty() == false);
}

void LayoutManager::deleteLayouts(void)
{
    for ( STACKPOS pos = mLayoutList.firstPosition(); pos != NULL; pos = mLayoutList.nextPosition(pos))
        delete static_cast<IELayout *>(mLayoutList.getAt(pos));

    mLayoutList.removeAll();
}

bool LayoutManager::logMessage(const NETrace::sLogMessage & logMsg, IEOutStream & stream) const
{
    bool result = false;
    if (isValid())
    {
        if ( logMsg.lmTrace.traceMessagePrio == NETrace::PrioIgnoreLayout )
        {
            stream.write( logMsg.lmTrace.traceMessage );
        }
        else
        {
            for ( LISTPOS pos = mLayoutList.firstPosition(); pos != NULL; pos = mLayoutList.nextPosition(pos) )
                mLayoutList.getAt(pos)->logMessage(logMsg, stream);
        }

        result = true;
    }

    return result;
}
