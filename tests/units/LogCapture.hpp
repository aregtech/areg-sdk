#ifndef AREG_TESTS_UNITS_LOGCAPTURE_HPP
#define AREG_TESTS_UNITS_LOGCAPTURE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/LogCapture.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, a helper of the tests that check what the framework logs.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/logging/LogConfiguration.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/persist/ConfigManager.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>

#if AREG_LOGGING

namespace log_capture
{
    //!< Logs the rows of one scope to a file. Each row is written as "row:<text>".
    //!< The object owns the setup and the release of the application.
    class LogCapture
    {
    public:
        LogCapture()
            : mManager      ( areg::Application::config_manager() )
            , mWasConfigured( mManager.is_configured() )
            , mReadonly     ( mManager.readonly_properties() )
            , mWritable     ( mManager.module_properties() )
            , mConfig       ( )
            , mFile         ( )
            , mStarted      ( false )
        {
        }

        ~LogCapture()
        {
            static_cast<void>(stop());
        }

        //!< Starts the application with timers and the logging of \a scopeName with \a priority.
        bool start(const char * scopeName, uint32_t priority)
        {
            std::error_code error;
            mFile = std::filesystem::absolute(std::filesystem::path(testing::TempDir())
                / ("areg-capture-" + std::to_string(areg::Process::instance().id()) + ".log"), error);
            if (error)
                return false;

            mManager.set_configuration(areg::ListProperties(), areg::ListProperties());
            mManager.set_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, false, true);
            mManager.set_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, false, true);
            mConfig.set_log_enabled(areg::LogTarget::Remote, false);
            mConfig.set_log_enabled(areg::LogTarget::Debug, false);
            mConfig.set_log_enabled(areg::LogTarget::Database, false);
            mConfig.set_log_enabled(areg::LogTarget::File, true);
            mConfig.set_status(true);
            mConfig.set_log_file(areg::String(mFile.string().c_str()));
            mConfig.set_append_data(false);
            mConfig.set_layout_message("row:%m%n");
            mConfig.set_layout_enter("");
            mConfig.set_layout_exit("");
            static_cast<void>(areg::set_source_state(areg::LogSourceState::Active));
            if (areg::restore_logging() == false)
                return false;

            mStarted = true;
            areg::Application::setup(false, false, false, true, false);
            if (areg::start_logging() == false)
                return false;

            static_cast<void>(areg::set_scope_priority(scopeName, priority));
            return (areg::scope_priority(scopeName) == priority);
        }

        //!< Stops the logging and the application and returns the rows written.
        std::string stop()
        {
            if (mStarted == false)
                return std::string();

            mStarted = false;
            areg::stop_logging(true);
            areg::Application::release();

            std::string text;
            {
                std::ifstream input(mFile, std::ios::binary);
                text.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
            }

            mManager.set_configuration(mReadonly, mWritable);
            static_cast<void>(areg::set_source_state(areg::LogSourceState::Active));
            static_cast<void>(areg::restore_logging());
            if (mWasConfigured == false)
            {
                mManager.release_properties();
            }

            std::error_code error;
            std::filesystem::remove(mFile, error);
            return text;
        }

        //!< Returns how many times \a what occurs in \a text.
        static std::size_t count(const std::string & text, const char * what)
        {
            const std::string needle{ what };
            std::size_t result{ 0u };
            for (std::size_t pos{ text.find(needle) }; pos != std::string::npos; pos = text.find(needle, pos + needle.size()))
            {
                ++result;
            }

            return result;
        }

    private:
        areg::ConfigManager &   mManager;
        const bool              mWasConfigured;
        areg::ListProperties    mReadonly;
        areg::ListProperties    mWritable;
        areg::LogConfiguration  mConfig;
        std::filesystem::path   mFile;
        bool                    mStarted;
    };
}

#endif  // AREG_LOGGING

#endif  // AREG_TESTS_UNITS_LOGCAPTURE_HPP
