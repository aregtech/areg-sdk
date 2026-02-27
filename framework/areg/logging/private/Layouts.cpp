/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \file        areg/logging/private/Layouts.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The collection of layouts
 ************************************************************************/
#include "areg/logging/private/Layouts.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/UtilityDefs.hpp"

#include "areg/logging/private/LogManager.hpp"

#include <utility>

namespace areg
{
    #if AREG_LOGS

    //////////////////////////////////////////////////////////////////////////
    // LogLayout interface implementation
    //////////////////////////////////////////////////////////////////////////

    LogLayout::LogLayout(const LayoutToken layout)
        : mLayout   ( layout )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // TickCountLayout class implementation
    //////////////////////////////////////////////////////////////////////////

    TickCountLayout::TickCountLayout()
        : LogLayout     ( LayoutToken::TickCount )
    {
    }

    TickCountLayout::TickCountLayout( const TickCountLayout & /*src*/ )
        : LogLayout     ( LayoutToken::TickCount )
    {
    }

    TickCountLayout::TickCountLayout( TickCountLayout && /*src*/ ) noexcept
        : LogLayout     ( LayoutToken::TickCount )
    {
    }

    void TickCountLayout::logMessage( const LogEntry & /*msgLog*/, OutStream & stream ) const
    {
    #ifdef _BIT64
        constexpr char fmt[]{ "%llu" };
    #else   // _BIT32
        constexpr char fmt[]{ "%u" };
    #endif  // _BIT64

        char buffer[128];
        uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, fmt, static_cast<id_type>( DateTime::getProcessTickCount() )));
        stream.write(reinterpret_cast<const uint8_t *>(buffer), len);
    }

    //////////////////////////////////////////////////////////////////////////
    // DayTimeLayout class declaration
    //////////////////////////////////////////////////////////////////////////


    DayTimeLayout::DayTimeLayout()
        : LogLayout ( LayoutToken::DayTime )
    {
    }

    DayTimeLayout::DayTimeLayout( const DayTimeLayout & /*src*/ )
        : LogLayout ( LayoutToken::DayTime )
    {
    }

    DayTimeLayout::DayTimeLayout( DayTimeLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::DayTime )
    {
    }

    void DayTimeLayout::logMessage( const LogEntry & msgLog, OutStream & stream ) const
    {
        if ( msgLog.logTimestamp != 0 )
        {
            String timestamp;
            DateTime::formatTime(DateTime(msgLog.logTimestamp), timestamp, TIME_FORMAT_ISO8601_OUTPUT);
            stream.write( reinterpret_cast<const uint8_t *>(timestamp.getString()), static_cast<uint32_t>(timestamp.getLength()));
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ModuleIdLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    ModuleIdLayout::ModuleIdLayout()
        : LogLayout      ( LayoutToken::ExecutableId )
    {
    }

    ModuleIdLayout::ModuleIdLayout(const ModuleIdLayout & /*src*/)
        : LogLayout ( LayoutToken::ExecutableId )
    {
    }

    ModuleIdLayout::ModuleIdLayout( ModuleIdLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::ExecutableId )
    {
    }

    void ModuleIdLayout::logMessage( const LogEntry & msgLog, OutStream & stream ) const
    {
        static const ITEM_ID _moduleId{ Process::getInstance().getId() };
    #ifdef _BIT64
        static const String  _moduleName{ String::makeString(static_cast<uint64_t>(_moduleId), Radix::Hexadecimal) };
    #else   // _BIT32
        static const String  _moduleName{ String::makeString(static_cast<uint32_t>(_moduleId), Radix::Hexadecimal) };
    #endif  // _BIT64

        if (msgLog.logModuleId != 0)
        {
            if (msgLog.logModuleId == _moduleId)
            {
                stream.write(reinterpret_cast<const uint8_t*>(_moduleName.getBuffer()), static_cast<uint32_t>(_moduleName.getLength()));
            }
            else
            {
    #ifdef _BIT64
                constexpr char fmt[]{ "0x%llX" };
    #else   // _BIT32
                constexpr char fmt[]{ "0x%X" };
    #endif  // _BIT64
                char buffer[128];
                uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logModuleId)));
                stream.write(reinterpret_cast<const uint8_t*>(buffer), len);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // MessageLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    MessageLayout::MessageLayout()
        : LogLayout ( LayoutToken::Message )
    {
    }

    MessageLayout::MessageLayout(const MessageLayout & /*src*/)
        : LogLayout ( LayoutToken::Message )
    {
    }

    MessageLayout::MessageLayout( MessageLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::Message )
    {
    }

    void MessageLayout::logMessage( const LogEntry & msgLog, OutStream & stream ) const
    {
        uint32_t count{ static_cast<uint32_t>(getStringLength<char>(msgLog.logMessage)) };
        stream.write(reinterpret_cast<const uint8_t *>(msgLog.logMessage), count);
    }

    //////////////////////////////////////////////////////////////////////////
    // EndOfLineLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    EndOfLineLayout::EndOfLineLayout()
        : LogLayout ( LayoutToken::EndOfLine )
    {
    }

    EndOfLineLayout::EndOfLineLayout(const EndOfLineLayout & /*src*/)
        : LogLayout ( LayoutToken::EndOfLine )
    {
    }

    EndOfLineLayout::EndOfLineLayout( EndOfLineLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::EndOfLine )
    {
    }

    void EndOfLineLayout::logMessage( const LogEntry & /*msgLog*/, OutStream & stream ) const
    {
        stream.write(reinterpret_cast<const uint8_t*>(&EndOfLine), 1);
    }

    //////////////////////////////////////////////////////////////////////////
    // PriorityLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    PriorityLayout::PriorityLayout()
        : LogLayout ( LayoutToken::Priority )
    {
    }

    PriorityLayout::PriorityLayout(const PriorityLayout & /*src*/)
        : LogLayout ( LayoutToken::Priority )
    {
    }

    PriorityLayout::PriorityLayout( PriorityLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::Priority )
    {
    }

    void PriorityLayout::logMessage( const LogEntry & msgLog, OutStream & stream ) const
    {
        const String& prio{ logPrioToString(msgLog.logMessagePrio) };
        stream.write(reinterpret_cast<const uint8_t *>(prio.getString()), static_cast<uint32_t>(prio.getLength()));
    }

    //////////////////////////////////////////////////////////////////////////
    // ScopeIdLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    ScopeIdLayout::ScopeIdLayout()
        : LogLayout ( LayoutToken::ScopeId )
    {
    }

    ScopeIdLayout::ScopeIdLayout(const ScopeIdLayout & /*src*/)
        : LogLayout ( LayoutToken::ScopeId )
    {
    }

    ScopeIdLayout::ScopeIdLayout( ScopeIdLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::ScopeId )
    {
    }

    void ScopeIdLayout::logMessage( const LogEntry & msgLog, OutStream & stream ) const
    {
        if ( msgLog.logScopeId != 0 )
        {
            char buffer[128];
            uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, "%u", msgLog.logScopeId));
            stream.write( reinterpret_cast<const uint8_t *>(buffer), len );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ThreadIdLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    ThreadIdLayout::ThreadIdLayout()
        : LogLayout ( LayoutToken::ThreadId )
    {
    }

    ThreadIdLayout::ThreadIdLayout(const ThreadIdLayout & /*src*/)
        : LogLayout ( LayoutToken::ThreadId )
    {
    }

    ThreadIdLayout::ThreadIdLayout( ThreadIdLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::ThreadId )
    {
    }

    void ThreadIdLayout::logMessage( const LogEntry & msgLog, OutStream & stream ) const
    {
        if ( msgLog.logThreadId != 0 )
        {
    #ifdef _BIT64
            constexpr char fmt[]{ "%06llu" };
    #else   // _BIT32
            constexpr char fmt[]{ "%06u" };
    #endif  // _BIT64

            char buffer[128];
            uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logThreadId)));
            stream.write( reinterpret_cast<const uint8_t *>(buffer), len );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ModuleNameLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    ModuleNameLayout::ModuleNameLayout()
        : LogLayout ( LayoutToken::ExecutableName )
    {
    }

    ModuleNameLayout::ModuleNameLayout(const ModuleNameLayout & /*src*/)
        : LogLayout ( LayoutToken::ExecutableName )
    {
    }

    ModuleNameLayout::ModuleNameLayout( ModuleNameLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::ExecutableName )
    {
    }

    void ModuleNameLayout::logMessage( const LogEntry & msgLog, OutStream & stream ) const
    {
        if (msgLog.logDataType == LogDataType::Local)
        {
            static const String& _module{ Process::getInstance().getAppName() };
            stream.write(reinterpret_cast<const uint8_t*>(_module.getString()), static_cast<uint32_t>(_module.getLength()));
        }
        else
        {
            if (msgLog.logCookie == COOKIE_LOCAL)
            {
                static const String& _module{ Process::getInstance().getAppName() };
                stream.write(reinterpret_cast<const uint8_t*>(_module.getString()), static_cast<uint32_t>(_module.getLength()));
            }
            else if ((msgLog.logCookie != COOKIE_UNKNOWN) && (msgLog.logModuleLen != 0))
            {
                stream.write(reinterpret_cast<const uint8_t*>(msgLog.logModule), msgLog.logModuleLen);
            }
            else
            {
                static constexpr std::string_view _unknown{ "Unknown_Module" };
                stream.write(reinterpret_cast<const uint8_t*>(_unknown.data()), static_cast<uint32_t>(_unknown.length()));
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ThreadNameLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    ThreadNameLayout::ThreadNameLayout()
        : LogLayout ( LayoutToken::ThreadName )
    {
    }

    ThreadNameLayout::ThreadNameLayout(const ThreadNameLayout & /*src*/)
        : LogLayout ( LayoutToken::ThreadName )
    {
    }

    ThreadNameLayout::ThreadNameLayout( ThreadNameLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::ThreadName )
    {
    }

    void ThreadNameLayout::logMessage( const LogEntry & msgLog, OutStream & stream ) const
    {
        const char* name{ nullptr };
        uint32_t len{ 0 };

        if (msgLog.logDataType == LogDataType::Local)
        {
            const String& thread{ Thread::getThreadName(static_cast<id_type>(msgLog.logThreadId)) };
            name = thread.getString();
            len  = static_cast<uint32_t>(thread.getLength());
        }
        else
        {
            name = msgLog.logThread;
            len = msgLog.logThreadLen;
        }

        if (len != 0)
        {
            stream.write(reinterpret_cast<const uint8_t*>(name), len);
        }
        else
        {
            static constexpr std::string_view _unknownThread{ "Unknown_Thread" };
            stream.write(reinterpret_cast<const uint8_t*>(_unknownThread.data()), static_cast<uint32_t>(_unknownThread.length()));
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ScopeNameLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    ScopeNameLayout::ScopeNameLayout()
        : LogLayout ( LayoutToken::ScopeName )
    {
    }

    ScopeNameLayout::ScopeNameLayout(const ScopeNameLayout & /*src*/)
        : LogLayout ( LayoutToken::ScopeName )
    {
    }

    ScopeNameLayout::ScopeNameLayout( ScopeNameLayout && /*src*/ ) noexcept
        : LogLayout ( LayoutToken::ScopeName )
    {
    }

    void ScopeNameLayout::logMessage( const LogEntry & msgLog, OutStream & stream ) const
    {
        stream.write(reinterpret_cast<const uint8_t *>(msgLog.logMessage), msgLog.logMessageLen);
    }

    //////////////////////////////////////////////////////////////////////////
    // ScopeNameLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    AnyTextLayout::AnyTextLayout()
        : LogLayout      ( LayoutToken::AnyText )
        , mTextMessage  ( )
    {
    }

    AnyTextLayout::AnyTextLayout(const AnyTextLayout & src)
        : LogLayout      ( LayoutToken::AnyText )
        , mTextMessage  ( src.mTextMessage )
    {
    }

    AnyTextLayout::AnyTextLayout( AnyTextLayout && src ) noexcept
        : LogLayout      ( LayoutToken::AnyText )
        , mTextMessage  ( std::move(src.mTextMessage) )
    {
    }

    AnyTextLayout::AnyTextLayout(const String & anyMessage)
        : LogLayout      ( LayoutToken::AnyText )
        , mTextMessage  ( anyMessage )
    {
    }

    AnyTextLayout::AnyTextLayout(const char * anyMessage)
        : LogLayout      ( LayoutToken::AnyText )
        , mTextMessage  ( anyMessage != nullptr ? anyMessage : EmptyStringA )
    {
    }

    void AnyTextLayout::logMessage( const LogEntry & /*msgLog*/, OutStream & stream ) const
    {
        stream.write( reinterpret_cast<const uint8_t *>(mTextMessage.getString()), static_cast<uint32_t>(mTextMessage.getLength()) );
    }

    //////////////////////////////////////////////////////////////////////////
    // CookieIdLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    CookieIdLayout::CookieIdLayout()
        : LogLayout(LayoutToken::CookieId)
    {
    }

    CookieIdLayout::CookieIdLayout(const CookieIdLayout& /* src */)
        : LogLayout(LayoutToken::CookieId)
    {
    }

    CookieIdLayout::CookieIdLayout(CookieIdLayout&& /* src */) noexcept
        : LogLayout(LayoutToken::CookieId)
    {
    }

    void CookieIdLayout::logMessage(const LogEntry& msgLog, OutStream& stream) const
    {
    #ifdef _BIT64
        constexpr char fmt[]{ "%03llu" };
    #else   // _BIT32
        constexpr char fmt[]{ "%03u" };
    #endif  // _BIT64

        char buffer[128];
        uint32_t len = static_cast<uint32_t>(String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logCookie)));
        stream.write(reinterpret_cast<const uint8_t*>(buffer), len);
    }

    #endif  // AREG_LOGS

} // namespace areg