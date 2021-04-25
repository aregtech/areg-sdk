/************************************************************************
 * \file        areg/src/trace/private/CELayoutManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The Layout Manager to output text messages.
 ************************************************************************/
#include "areg/src/trace/private/CELayoutManager.hpp"

#include "areg/src/trace/private/ELayouts.hpp"
#include "areg/src/trace/private/NELogConfig.hpp"
#include "areg/src/base/NEUtilities.hpp"

CELayoutManager::CELayoutManager(void)
    : mLayoutList   ( )
{
    ; // do nothing
}

CELayoutManager::~CELayoutManager(void)
{
    DeleteLayouts();
}

bool CELayoutManager::CreateLayouts( const char * layoutFormat )
{
    DeleteLayouts();
    int len         = layoutFormat != NULL ? NEString::getStringLength<char>( layoutFormat ) + 1 : 0;
    char * strFormat= len != 0 ? DEBUG_NEW char[len] : NULL;

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
                    newLayout   = DEBUG_NEW CETickCountLayout();
                    break;

                case NELogConfig::LayoutDayTime:
                    newLayout   = DEBUG_NEW CEDayTimeLaytout();
                    break;

                case NELogConfig::LayoutExecutableId:
                    newLayout   = DEBUG_NEW CEModuleIdLayout();
                    break;

                case NELogConfig::LayoutMessage:
                    if ( hasExclusive == false )
                    {
                        newLayout   = DEBUG_NEW CEMessageLayout();
                        hasExclusive= true;
                    }
                    break;

                case NELogConfig::LayoutEndOfLine:
                    newLayout   = DEBUG_NEW CEEndOfLineLayout();
                    break;

                case NELogConfig::LayoutPriority:
                    newLayout   = DEBUG_NEW CEPriorityLayout();
                    break;

                case NELogConfig::LaytoutScopeId:
                    newLayout   = DEBUG_NEW CEScopeIdLayout();
                    break;

                case NELogConfig::LayoutThreadId:
                    newLayout   = DEBUG_NEW CEThreadIdLayout();
                    break;

                case NELogConfig::LayoutExecutableName:
                    newLayout   = DEBUG_NEW CEModuleNameLayout();
                    break;

                case NELogConfig::LayoutThreadName:
                    newLayout   = DEBUG_NEW CEThreadNameLayout();
                    break;

                case NELogConfig::LaytoutScopeName:
                    if ( hasExclusive == false )
                    {
                        newLayout   = DEBUG_NEW CEScopeNameLayout();
                        hasExclusive= true;
                    }
                    break;

                default:
                    if ( ch == '%' )
                    {
                        *(pos + 1) = '\0';
                        newLayout   = DEBUG_NEW CEAnyTextLayout(pos1);
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
                        mLayoutList.PushElement( static_cast<IELayout *>(DEBUG_NEW CEAnyTextLayout(pos1)) );

                    mLayoutList.PushElement( newLayout );

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
            mLayoutList.PushElement( static_cast<IELayout *>(DEBUG_NEW CEAnyTextLayout(pos1)) );

        delete [] strFormat;
    }

    return (mLayoutList.IsEmpty() == false);
}

void CELayoutManager::DeleteLayouts(void)
{
    for ( STACKPOS pos = mLayoutList.GetFirstPosition(); pos != NULL; pos = mLayoutList.GetNextPosition(pos))
        delete static_cast<IELayout *>(mLayoutList.GetAt(pos));

    mLayoutList.RemoveAll();
}

bool CELayoutManager::LogMessage(const NETrace::sLogMessage & logMsg, IEOutStream & stream) const
{
    bool result = false;
    if (IsValid())
    {
        if ( logMsg.lmTrace.traceMessagePrio == NETrace::PrioIgnoreLayout )
        {
            stream.Write( logMsg.lmTrace.traceMessage );
        }
        else
        {
            for ( LISTPOS pos = mLayoutList.GetFirstPosition(); pos != NULL; pos = mLayoutList.GetNextPosition(pos) )
                mLayoutList.GetAt(pos)->LogMessage(logMsg, stream);
        }

        result = true;
    }

    return result;
}
