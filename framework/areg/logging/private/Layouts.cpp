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

    areg::LogLayout::LogLayout(const areg::LayoutToken layout)
        : mLayout   ( layout )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // TickCountLayout class implementation
    //////////////////////////////////////////////////////////////////////////

    areg::TickCountLayout::TickCountLayout()
        : areg::LogLayout     ( areg::LayoutToken::TickCount )
    {
    }

    areg::TickCountLayout::TickCountLayout( const areg::TickCountLayout & /*src*/ )
        : areg::LogLayout     ( areg::LayoutToken::TickCount )
    {
    }

    areg::TickCountLayout::TickCountLayout( areg::TickCountLayout && /*src*/ ) noexcept
        : areg::LogLayout     ( areg::LayoutToken::TickCount )
    {
    }

    void areg::TickCountLayout::logMessage( const areg::LogEntry & /*msgLog*/, areg::OutStream & stream ) const
    {
    #ifdef _BIT64
        constexpr char fmt[]{ "%llu" };
    #else   // _BIT32
        constexpr char fmt[]{ "%u" };
    #endif  // _BIT64

        char buffer[128];
        uint32_t len = static_cast<uint32_t>(areg::String::formatString(buffer, 128, fmt, static_cast<id_type>( areg::DateTime::getProcessTickCount() )));
        stream.write(reinterpret_cast<const uint8_t *>(buffer), len);
    }

    //////////////////////////////////////////////////////////////////////////
    // DayTimeLayout class declaration
    //////////////////////////////////////////////////////////////////////////


    areg::DayTimeLayout::DayTimeLayout()
        : areg::LogLayout ( areg::LayoutToken::DayTime )
    {
    }

    areg::DayTimeLayout::DayTimeLayout( const areg::DayTimeLayout & /*src*/ )
        : areg::LogLayout ( areg::LayoutToken::DayTime )
    {
    }

    areg::DayTimeLayout::DayTimeLayout( areg::DayTimeLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::DayTime )
    {
    }

    void areg::DayTimeLayout::logMessage( const areg::LogEntry & msgLog, areg::OutStream & stream ) const
    {
        if ( msgLog.logTimestamp != 0 )
        {
            areg::String timestamp;
            areg::DateTime::formatTime(areg::DateTime(msgLog.logTimestamp), timestamp, areg::TIME_FORMAT_ISO8601_OUTPUT);
            stream.write( reinterpret_cast<const uint8_t *>(timestamp.getString()), static_cast<uint32_t>(timestamp.getLength()));
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ModuleIdLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    areg::ModuleIdLayout::ModuleIdLayout()
        : areg::LogLayout      ( areg::LayoutToken::ExecutableId )
    {
    }

    areg::ModuleIdLayout::ModuleIdLayout(const areg::ModuleIdLayout & /*src*/)
        : areg::LogLayout ( areg::LayoutToken::ExecutableId )
    {
    }

    areg::ModuleIdLayout::ModuleIdLayout( areg::ModuleIdLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::ExecutableId )
    {
    }

    void areg::ModuleIdLayout::logMessage( const areg::LogEntry & msgLog, areg::OutStream & stream ) const
    {
        static const ITEM_ID _moduleId{ areg::Process::getInstance().getId() };
    #ifdef _BIT64
        static const areg::String  _moduleName{ areg::String::makeString(static_cast<uint64_t>(_moduleId), areg::Radix::Hexadecimal) };
    #else   // _BIT32
        static const areg::String  _moduleName{ areg::String::makeString(static_cast<uint32_t>(_moduleId), areg::Radix::Hexadecimal) };
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
                uint32_t len = static_cast<uint32_t>(areg::String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logModuleId)));
                stream.write(reinterpret_cast<const uint8_t*>(buffer), len);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // MessageLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    areg::MessageLayout::MessageLayout()
        : areg::LogLayout ( areg::LayoutToken::Message )
    {
    }

    areg::MessageLayout::MessageLayout(const areg::MessageLayout & /*src*/)
        : areg::LogLayout ( areg::LayoutToken::Message )
    {
    }

    areg::MessageLayout::MessageLayout( areg::MessageLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::Message )
    {
    }

    void areg::MessageLayout::logMessage( const areg::LogEntry & msgLog, areg::OutStream & stream ) const
    {
        uint32_t count{ static_cast<uint32_t>(areg::getStringLength<char>(msgLog.logMessage)) };
        stream.write(reinterpret_cast<const uint8_t *>(msgLog.logMessage), count);
    }

    //////////////////////////////////////////////////////////////////////////
    // EndOfLineLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    areg::EndOfLineLayout::EndOfLineLayout()
        : areg::LogLayout ( areg::LayoutToken::EndOfLine )
    {
    }

    areg::EndOfLineLayout::EndOfLineLayout(const areg::EndOfLineLayout & /*src*/)
        : areg::LogLayout ( areg::LayoutToken::EndOfLine )
    {
    }

    areg::EndOfLineLayout::EndOfLineLayout( areg::EndOfLineLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::EndOfLine )
    {
    }

    void areg::EndOfLineLayout::logMessage( const areg::LogEntry & /*msgLog*/, areg::OutStream & stream ) const
    {
        stream.write(reinterpret_cast<const uint8_t*>(&areg::EndOfLine), 1);
    }

    //////////////////////////////////////////////////////////////////////////
    // PriorityLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    areg::PriorityLayout::PriorityLayout()
        : areg::LogLayout ( areg::LayoutToken::Priority )
    {
    }

    areg::PriorityLayout::PriorityLayout(const areg::PriorityLayout & /*src*/)
        : areg::LogLayout ( areg::LayoutToken::Priority )
    {
    }

    areg::PriorityLayout::PriorityLayout( areg::PriorityLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::Priority )
    {
    }

    void areg::PriorityLayout::logMessage( const areg::LogEntry & msgLog, areg::OutStream & stream ) const
    {
        const areg::String& prio{ areg::logPrioToString(msgLog.logMessagePrio) };
        stream.write(reinterpret_cast<const uint8_t *>(prio.getString()), static_cast<uint32_t>(prio.getLength()));
    }

    //////////////////////////////////////////////////////////////////////////
    // ScopeIdLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    areg::ScopeIdLayout::ScopeIdLayout()
        : areg::LogLayout ( areg::LayoutToken::ScopeId )
    {
    }

    areg::ScopeIdLayout::ScopeIdLayout(const areg::ScopeIdLayout & /*src*/)
        : areg::LogLayout ( areg::LayoutToken::ScopeId )
    {
    }

    areg::ScopeIdLayout::ScopeIdLayout( areg::ScopeIdLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::ScopeId )
    {
    }

    void areg::ScopeIdLayout::logMessage( const areg::LogEntry & msgLog, areg::OutStream & stream ) const
    {
        if ( msgLog.logScopeId != 0 )
        {
            char buffer[128];
            uint32_t len = static_cast<uint32_t>(areg::String::formatString(buffer, 128, "%u", msgLog.logScopeId));
            stream.write( reinterpret_cast<const uint8_t *>(buffer), len );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ThreadIdLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    areg::ThreadIdLayout::ThreadIdLayout()
        : areg::LogLayout ( areg::LayoutToken::ThreadId )
    {
    }

    areg::ThreadIdLayout::ThreadIdLayout(const areg::ThreadIdLayout & /*src*/)
        : areg::LogLayout ( areg::LayoutToken::ThreadId )
    {
    }

    areg::ThreadIdLayout::ThreadIdLayout( areg::ThreadIdLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::ThreadId )
    {
    }

    void areg::ThreadIdLayout::logMessage( const areg::LogEntry & msgLog, areg::OutStream & stream ) const
    {
        if ( msgLog.logThreadId != 0 )
        {
    #ifdef _BIT64
            constexpr char fmt[]{ "%06llu" };
    #else   // _BIT32
            constexpr char fmt[]{ "%06u" };
    #endif  // _BIT64

            char buffer[128];
            uint32_t len = static_cast<uint32_t>(areg::String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logThreadId)));
            stream.write( reinterpret_cast<const uint8_t *>(buffer), len );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ModuleNameLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    areg::ModuleNameLayout::ModuleNameLayout()
        : areg::LogLayout ( areg::LayoutToken::ExecutableName )
    {
    }

    areg::ModuleNameLayout::ModuleNameLayout(const areg::ModuleNameLayout & /*src*/)
        : areg::LogLayout ( areg::LayoutToken::ExecutableName )
    {
    }

    areg::ModuleNameLayout::ModuleNameLayout( areg::ModuleNameLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::ExecutableName )
    {
    }

    void areg::ModuleNameLayout::logMessage( const areg::LogEntry & msgLog, areg::OutStream & stream ) const
    {
        if (msgLog.logDataType == areg::LogDataType::Local)
        {
            static const areg::String& _module{ areg::Process::getInstance().getAppName() };
            stream.write(reinterpret_cast<const uint8_t*>(_module.getString()), static_cast<uint32_t>(_module.getLength()));
        }
        else
        {
            if (msgLog.logCookie == areg::COOKIE_LOCAL)
            {
                static const areg::String& _module{ areg::Process::getInstance().getAppName() };
                stream.write(reinterpret_cast<const uint8_t*>(_module.getString()), static_cast<uint32_t>(_module.getLength()));
            }
            else if ((msgLog.logCookie != areg::COOKIE_UNKNOWN) && (msgLog.logModuleLen != 0))
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

    areg::ThreadNameLayout::ThreadNameLayout()
        : areg::LogLayout ( areg::LayoutToken::ThreadName )
    {
    }

    areg::ThreadNameLayout::ThreadNameLayout(const areg::ThreadNameLayout & /*src*/)
        : areg::LogLayout ( areg::LayoutToken::ThreadName )
    {
    }

    areg::ThreadNameLayout::ThreadNameLayout( areg::ThreadNameLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::ThreadName )
    {
    }

    void areg::ThreadNameLayout::logMessage( const areg::LogEntry & msgLog, areg::OutStream & stream ) const
    {
        const char* name{ nullptr };
        uint32_t len{ 0 };

        if (msgLog.logDataType == areg::LogDataType::Local)
        {
            const areg::String& thread{ areg::Thread::getThreadName(static_cast<id_type>(msgLog.logThreadId)) };
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

    areg::ScopeNameLayout::ScopeNameLayout()
        : areg::LogLayout ( areg::LayoutToken::ScopeName )
    {
    }

    areg::ScopeNameLayout::ScopeNameLayout(const areg::ScopeNameLayout & /*src*/)
        : areg::LogLayout ( areg::LayoutToken::ScopeName )
    {
    }

    areg::ScopeNameLayout::ScopeNameLayout( areg::ScopeNameLayout && /*src*/ ) noexcept
        : areg::LogLayout ( areg::LayoutToken::ScopeName )
    {
    }

    void areg::ScopeNameLayout::logMessage( const areg::LogEntry & msgLog, areg::OutStream & stream ) const
    {
        stream.write(reinterpret_cast<const uint8_t *>(msgLog.logMessage), msgLog.logMessageLen);
    }

    //////////////////////////////////////////////////////////////////////////
    // ScopeNameLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    areg::AnyTextLayout::AnyTextLayout()
        : areg::LogLayout      ( areg::LayoutToken::AnyText )
        , mTextMessage  ( )
    {
    }

    areg::AnyTextLayout::AnyTextLayout(const areg::AnyTextLayout & src)
        : areg::LogLayout      ( areg::LayoutToken::AnyText )
        , mTextMessage  ( src.mTextMessage )
    {
    }

    areg::AnyTextLayout::AnyTextLayout( areg::AnyTextLayout && src ) noexcept
        : areg::LogLayout      ( areg::LayoutToken::AnyText )
        , mTextMessage  ( std::move(src.mTextMessage) )
    {
    }

    areg::AnyTextLayout::AnyTextLayout(const areg::String & anyMessage)
        : areg::LogLayout      ( areg::LayoutToken::AnyText )
        , mTextMessage  ( anyMessage )
    {
    }

    areg::AnyTextLayout::AnyTextLayout(const char * anyMessage)
        : areg::LogLayout      ( areg::LayoutToken::AnyText )
        , mTextMessage  ( anyMessage != nullptr ? anyMessage : areg::EmptyStringA )
    {
    }

    void areg::AnyTextLayout::logMessage( const areg::LogEntry & /*msgLog*/, areg::OutStream & stream ) const
    {
        stream.write( reinterpret_cast<const uint8_t *>(mTextMessage.getString()), static_cast<uint32_t>(mTextMessage.getLength()) );
    }

    //////////////////////////////////////////////////////////////////////////
    // CookieIdLayout class declaration
    //////////////////////////////////////////////////////////////////////////

    areg::CookieIdLayout::CookieIdLayout()
        : areg::LogLayout(areg::LayoutToken::CookieId)
    {
    }

    areg::CookieIdLayout::CookieIdLayout(const areg::CookieIdLayout& /* src */)
        : areg::LogLayout(areg::LayoutToken::CookieId)
    {
    }

    areg::CookieIdLayout::CookieIdLayout(areg::CookieIdLayout&& /* src */) noexcept
        : areg::LogLayout(areg::LayoutToken::CookieId)
    {
    }

    void areg::CookieIdLayout::logMessage(const areg::LogEntry& msgLog, areg::OutStream& stream) const
    {
    #ifdef _BIT64
        constexpr char fmt[]{ "%03llu" };
    #else   // _BIT32
        constexpr char fmt[]{ "%03u" };
    #endif  // _BIT64

        char buffer[128];
        uint32_t len = static_cast<uint32_t>(areg::String::formatString(buffer, 128, fmt, static_cast<id_type>(msgLog.logCookie)));
        stream.write(reinterpret_cast<const uint8_t*>(buffer), len);
    }

    #endif  // AREG_LOGS

} // namespace areg