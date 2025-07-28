/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/NEDebugWin32.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Windows OS specific implementation of Debugging utilities
 *
 ************************************************************************/
#include "areg/base/private/NEDebug.hpp"

#ifdef  _WIN32

#include "areg/base/NEUtilities.hpp"
#include "areg/base/Containers.hpp"

#include <Windows.h>

#if _MSC_VER
    #pragma warning(disable: 4091)
#endif  // _MSC_VER

#include <DbgHelp.h>

#if _MSC_VER
    #pragma warning(default: 4091)
#endif  // _MSC_VER

#ifdef  _DEBUG
void AREG_API_IMPL NEDebug::outputMessageOS( const char * msg )
{
    if ( NEString::isEmpty<char>( msg ) == false )
        ::OutputDebugStringA( msg );
}
#else   // _DEBUG
void AREG_API_IMPL NEDebug::outputMessageOS(const char* /*msg*/)
{
}
#endif  // _DEBUG

#ifdef  _DEBUG
void AREG_API_IMPL NEDebug::dumpExceptionCallStack( struct _EXCEPTION_POINTERS *ep, std::list<std::string> & OUT out_callStack )
{

    constexpr char  _stackFormat[]              { "        %s:(%d): %s: %s" };
    constexpr char  _msgFileUnavailable[]       { "<File and line number not available>" };
    constexpr char  _msgFunctionUnavailable[]   { "<Function name unavailable>" };
    constexpr char  _msgModuleUnavailable[]     { "<Module name unavailable>" };
    constexpr char  _msgCannotExtractSym[]      { "<Cannot extract symbols for current process>" };
    constexpr char  _msgUnknownMachine[]        { "<Unknown machine type>" };
    constexpr char  _msgIncompleteStack[]       { "Call Stack is incomplete...." };

    constexpr unsigned int   _stackMaxDepth     { 64 };
    constexpr unsigned int   _symNameLength     { MAX_SYM_NAME };
    constexpr unsigned int   _sizeOfSymInfo     { MACRO_ALIGN_SIZE( sizeof( SYMBOL_INFO ) + _symNameLength * sizeof( char ), sizeof( ULONG64 ) ) };

    out_callStack.clear();

    // Walk through the stack frames.
    HANDLE hProcess = GetCurrentProcess( );
    HANDLE hThread = GetCurrentThread( );
    if ( ep != nullptr && SymInitialize( hProcess, nullptr, TRUE ) == TRUE )
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
            IMAGEHLP_LINE64     lineInfo   { };
            IMAGEHLP_MODULE64   moduleInfo { };
            PSYMBOL_INFO        symbolInfo  = reinterpret_cast<PSYMBOL_INFO>(symBuffer);

            unsigned int curDepth = 0;
            while ( ::StackWalk64( machineType, hProcess, hThread, &frame, context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr ) == TRUE )
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

                String::formatString( message
                                    , _symNameLength + MAX_PATH + 8
                                    , _stackFormat
                                    , hasFile     ? lineInfo.FileName    : _msgFileUnavailable
                                    , hasFile     ? static_cast<int32_t>(lineInfo.LineNumber)  : -1
                                    , hasFunction ? symbolInfo->Name     : _msgFunctionUnavailable
                                    , hasModule   ? moduleInfo.ImageName : _msgModuleUnavailable );

                out_callStack.push_back( message );

            }

            if ( curDepth > _stackMaxDepth )
            {
                out_callStack.push_front( _msgIncompleteStack );
            }
        }
        else
        {
            out_callStack.push_back( _msgUnknownMachine );
        }

        SymCleanup( hProcess );
    }
    else
    {
        out_callStack.push_back( _msgCannotExtractSym );
    }
}
#else   // _DEBUG
void AREG_API_IMPL NEDebug::dumpExceptionCallStack(struct _EXCEPTION_POINTERS* /*ep*/, std::list<std::string>& OUT /*out_callStack*/)
{
}
#endif  // _DEBUG

#endif  // _WIN32
