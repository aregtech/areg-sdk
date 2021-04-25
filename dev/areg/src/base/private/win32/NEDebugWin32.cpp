/************************************************************************
 * \file        areg/src/base/private/win32/NEDebugWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Windows OS specific implementation of Debugging utilities
 *
 ************************************************************************/
#include "areg/src/base/private/NEDebug.hpp"

#ifdef  _WINDOWS

#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/EContainers.hpp"

#include <windows.h>

#if _MSC_VER
    #pragma warning(disable: 4091)
#endif  // _MSC_VER

#include <dbghelp.h>

#if _MSC_VER
    #pragma warning(default: 4091)
#endif  // _MSC_VER

void AREG_API NEDebug::outputMessageOS( const char * msg )
{
#ifdef  _DEBUG
    if ( NEString::isEmpty<char>( msg ) == false )
        ::OutputDebugStringA( msg );
#endif  // _DEBUG
}

void AREG_API NEDebug::dumpExceptionCallStack( struct _EXCEPTION_POINTERS *ep, CEStringList & out_callStack )
{
#ifdef  _DEBUG

    static const char * const   _stackFormat            = "        %s:(%d): %s: %s";
    static const char * const   _msgFileUnavailable     = "<File and line number not available>";
    static const char * const   _msgFunctionUnavailable = "<Function name unavailable>";
    static const char * const   _msgModuleUnavailable   = "<Module name unavailable>";
    static const char * const   _msgCannotExtractSym    = "<Cannot extract symbols for current process>";
    static const char * const   _msgUnknownMachine      = "<Unknown machine type>";
    static const char * const   _msgIncompleteStack     = "Call Stack is incomplete....";

    static const unsigned int   _stackMaxDepth          = 64;
    static const unsigned int   _symNameLength          = MAX_SYM_NAME;
    static const unsigned int   _messageNameLength      = _symNameLength + MAX_PATH + 16;
    static const unsigned int   _sizeOfSymInfo          = MACRO_ALIGN_SIZE( sizeof( SYMBOL_INFO ) + _symNameLength * sizeof( char ), sizeof( ULONG64 ) );

    out_callStack.RemoveAll( );

    // Walk through the stack frames.
    HANDLE hProcess = GetCurrentProcess( );
    HANDLE hThread = GetCurrentThread( );
    if ( ep != NULL && SymInitialize( hProcess, NULL, TRUE ) == TRUE )
    {
        unsigned long machineType = IMAGE_FILE_MACHINE_UNKNOWN;
#if defined(_X86_)
        machineType = IMAGE_FILE_MACHINE_I386;
#elif   defined(_AMD64_)
        machineType = IMAGE_FILE_MACHINE_AMD64;
#elif   defined(_IA64_)
        machineType = IMAGE_FILE_MACHINE_IA64;
#endif
        if ( machineType != IMAGE_FILE_MACHINE_UNKNOWN )
        {
            CONTEXT * context = ep->ContextRecord;
            STACKFRAME64 frame;
            memset( static_cast<void *>(&frame), 0, sizeof( STACKFRAME64 ) );

            frame.AddrPC.Mode       = AddrModeFlat;
            frame.AddrStack.Mode    = AddrModeFlat;
            frame.AddrFrame.Mode    = AddrModeFlat;

#if defined(BIT32)
            frame.AddrPC.Offset     = context->Eip;
            frame.AddrStack.Offset  = context->Esp;
            frame.AddrFrame.Offset  = context->Ebp;
#elif defined(BIT64)
            frame.AddrPC.Offset     = context->Rip;
            frame.AddrStack.Offset  = context->Rsp;
            frame.AddrFrame.Offset  = context->Rbp;
#endif // defined(BIT64)

            char message[_symNameLength + MAX_PATH + 8] = { 0 };

            char symBuffer[_sizeOfSymInfo]  = { 0 };
            IMAGEHLP_LINE64     lineInfo    = { 0 };
            IMAGEHLP_MODULE64   moduleInfo  = { 0 };
            PSYMBOL_INFO        symbolInfo  = reinterpret_cast<PSYMBOL_INFO>(symBuffer);

            unsigned int curDepth = 0;
            while ( ::StackWalk64( machineType, hProcess, hThread, &frame, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL ) == TRUE )
            {
                if ( frame.AddrFrame.Offset == 0 || ++ curDepth > _stackMaxDepth )
                    break;

                memset( symbolInfo, 0, sizeof( SYMBOL_INFO ) );
                memset( &lineInfo, 0, sizeof( IMAGEHLP_LINE64 ) );
                memset( &moduleInfo, 0, sizeof( IMAGEHLP_MODULE64 ) );

                symbolInfo->SizeOfStruct = sizeof( SYMBOL_INFO );
                symbolInfo->MaxNameLen = _symNameLength;
                lineInfo.SizeOfStruct = sizeof( IMAGEHLP_LINE64 );
                moduleInfo.SizeOfStruct = sizeof( IMAGEHLP_MODULE64 );

                // Get line number and file name at the address
                unsigned long   lineDisplacment = 0;
                bool hasFile = SymGetLineFromAddr64( hProcess, frame.AddrPC.Offset, &lineDisplacment, &lineInfo ) == TRUE;

                // Get Function name ad the address
                uint64_t symDisplacment = 0;
                bool hasFunction = SymFromAddr( hProcess, frame.AddrPC.Offset, &symDisplacment, symbolInfo ) == TRUE;

                // Get module name
                bool hasModule = hasFunction ? SymGetModuleInfo64( hProcess, symbolInfo->ModBase, &moduleInfo ) == TRUE : false;

                CEString::PrintString( message
                                    , _symNameLength + MAX_PATH + 8
                                    , _stackFormat
                                    , hasFile     ? lineInfo.FileName    : _msgFileUnavailable
                                    , hasFile     ? lineInfo.LineNumber  : -1
                                    , hasFunction ? symbolInfo->Name     : _msgFunctionUnavailable
                                    , hasModule   ? moduleInfo.ImageName : _msgModuleUnavailable );

                out_callStack.AddTail( message );

            }

            if ( curDepth > _stackMaxDepth )
                out_callStack.AddHead( _msgIncompleteStack );
        }
        else
        {
            out_callStack.AddTail( _msgUnknownMachine );
        }

        SymCleanup( hProcess );
    }
    else
    {
        out_callStack.AddTail( _msgCannotExtractSym );
    }
#endif  // _DEBUG
}

#endif  // _WINDOWS

