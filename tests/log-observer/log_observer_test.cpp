/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        log-observer/log_observer_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Drives the log observer C API and LogObserverBase against a real log collector.
 *              The collector and the log sources run as child processes.
 *
 *              Usage: areg-log-observer-test <case> <bin-dir> <logcollector-path>
 *              The log source role is started by the test itself: <exe> source <config-path>.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/areg_log.h"
#include "areglogger/client/LogObserverApi.h"
#include "areglogger/client/LogObserverBase.hpp"

#if defined(_WIN32)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #include <Windows.h>
    #include <fcntl.h>
    #include <io.h>
#else   // !defined(_WIN32)
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <signal.h>
    #include <spawn.h>
    #include <sys/socket.h>
    #include <sys/wait.h>
    #include <unistd.h>

    extern char ** environ;
#endif  // defined(_WIN32)

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

DEF_LOG_SCOPE(log_observer_test_source_Scope, write);

namespace
{
    //!< Milliseconds a wait for an outcome may last before the case fails.
    constexpr int32_t   WAIT_MS         { 60'000 };
    //!< The name of the database file the observer config names.
    constexpr char      CONFIG_DB_NAME[]{ "observer.sqlog" };
    //!< The name of the executable, as the collector reports a log source.
    constexpr char      EXE_NAME[]      { "areg-log-observer-test" };

    //////////////////////////////////////////////////////////////////////////
    // Checks
    //////////////////////////////////////////////////////////////////////////

    int     _failures   { 0 };

    bool check(bool condition, const char * what, int line)
    {
        if (condition == false)
        {
            ++_failures;
            std::printf("FAIL: %s (line %d)\n", what, line);
            std::fflush(stdout);
        }

        return condition;
    }

#define CHECK(cond)     check(static_cast<bool>(cond), #cond, __LINE__)
#define REQUIRE(cond)   do { if (check(static_cast<bool>(cond), #cond, __LINE__) == false) return false; } while (false)

    //////////////////////////////////////////////////////////////////////////
    // Child processes
    //////////////////////////////////////////////////////////////////////////

    //!< A child process whose standard input the test writes. It starts without a shell.
    class Child
    {
    public:
        explicit Child(const char * quitLine = "")
            : mPipe     ( nullptr )
#if defined(_WIN32)
            , mProcess  ( nullptr )
#else   // !defined(_WIN32)
            , mProcess  ( 0 )
#endif  // defined(_WIN32)
            , mQuitLine ( quitLine )
        {
        }

        ~Child()
        {
            static_cast<void>(finish());
        }

        //!< Starts the program of the first argument and passes every argument unchanged.
        bool start(const std::vector<std::string> & args)
        {
#if defined(_WIN32)
            std::string command;
            for (const std::string & arg : args)
            {
                // A quote or a closing backslash would change the bounds of the argument.
                if ((arg.find('"') != std::string::npos) || (arg.empty() == false && arg.back() == '\\'))
                    return false;

                command += (command.empty() ? "\"" : " \"") + arg + "\"";
            }

            SECURITY_ATTRIBUTES inherit{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
            HANDLE input{ nullptr };
            HANDLE output{ nullptr };
            if (::CreatePipe(&input, &output, &inherit, 0) == FALSE)
                return false;

            ::SetHandleInformation(output, HANDLE_FLAG_INHERIT, 0);
            STARTUPINFOA startup{ };
            startup.cb          = sizeof(startup);
            startup.dwFlags     = STARTF_USESTDHANDLES;
            startup.hStdInput   = input;
            startup.hStdOutput  = ::GetStdHandle(STD_OUTPUT_HANDLE);
            startup.hStdError   = ::GetStdHandle(STD_ERROR_HANDLE);
            PROCESS_INFORMATION process{ };
            const BOOL started{ ::CreateProcessA(args[0].c_str(), command.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &startup, &process) };
            ::CloseHandle(input);
            if (started == FALSE)
            {
                ::CloseHandle(output);
                return false;
            }

            ::CloseHandle(process.hThread);
            mProcess = process.hProcess;
            const int descriptor{ ::_open_osfhandle(reinterpret_cast<intptr_t>(output), _O_WRONLY) };
            mPipe = (descriptor != -1 ? ::_fdopen(descriptor, "w") : nullptr);
#else   // !defined(_WIN32)
            int ends[2]{ -1, -1 };
            if (::pipe(ends) != 0)
                return false;

            ::fcntl(ends[0], F_SETFD, FD_CLOEXEC);
            ::fcntl(ends[1], F_SETFD, FD_CLOEXEC);
            std::vector<char *> argv;
            for (const std::string & arg : args)
            {
                argv.push_back(const_cast<char *>(arg.c_str()));
            }

            argv.push_back(nullptr);
            posix_spawn_file_actions_t actions;
            ::posix_spawn_file_actions_init(&actions);
            ::posix_spawn_file_actions_adddup2(&actions, ends[0], STDIN_FILENO);
            const int error{ ::posix_spawn(&mProcess, args[0].c_str(), &actions, nullptr, argv.data(), environ) };
            ::posix_spawn_file_actions_destroy(&actions);
            ::close(ends[0]);
            if (error != 0)
            {
                ::close(ends[1]);
                mProcess = 0;
                return false;
            }

            mPipe = ::fdopen(ends[1], "w");
#endif  // defined(_WIN32)
            return (mPipe != nullptr);
        }

        bool send(const std::string & line)
        {
            if (mPipe == nullptr)
                return false;

            const std::string text{ line + "\n" };
            return (std::fputs(text.c_str(), mPipe) >= 0) && (std::fflush(mPipe) == 0);
        }

        bool is_open() const noexcept
        {
            return (mPipe != nullptr);
        }

        //!< Asks the child to quit, waits for it and returns true if it exited with 0.
        bool finish()
        {
            if (mPipe == nullptr)
                return true;

            if (mQuitLine.empty() == false)
            {
                static_cast<void>(send(mQuitLine));
            }

            std::fclose(mPipe);
            mPipe = nullptr;
#if defined(_WIN32)
            DWORD code{ 1u };
            ::WaitForSingleObject(mProcess, INFINITE);
            ::GetExitCodeProcess(mProcess, &code);
            ::CloseHandle(mProcess);
            mProcess = nullptr;
            return (code == 0u);
#else   // !defined(_WIN32)
            int status{ 0 };
            pid_t done{ -1 };
            do
            {
                done = ::waitpid(mProcess, &status, 0);
            } while ((done == -1) && (errno == EINTR));

            mProcess = 0;
            return (done != -1) && WIFEXITED(status) && (WEXITSTATUS(status) == 0);
#endif  // defined(_WIN32)
        }

    private:
        FILE *      mPipe;          //!< The stream to the standard input of the child.
#if defined(_WIN32)
        HANDLE      mProcess;       //!< The handle of the child process.
#else   // !defined(_WIN32)
        pid_t       mProcess;       //!< The process ID of the child.
#endif  // defined(_WIN32)
        std::string mQuitLine;      //!< The line that makes the child leave, empty if EOF is enough.
    };

    //////////////////////////////////////////////////////////////////////////
    // Environment of a case
    //////////////////////////////////////////////////////////////////////////

    struct Env
    {
        std::string dir;            //!< The private directory of the case.
        std::string binDir;         //!< The directory of the installed configuration.
        std::string collector;      //!< The path of the logcollector executable.
        std::string observerCfg;    //!< The configuration of the observer.
        std::string sourceCfg;      //!< The configuration of a log source.
        std::string collectorCfg;   //!< The configuration of the collector.
        std::string dbPath;         //!< The database file the observer writes.
        std::string sourceLog;      //!< The log file of a source that writes one.
        uint16_t    port    { 0u }; //!< The TCP port of the collector.
    };

    Env _env;

    //!< Returns a free TCP port of the loopback interface.
    uint16_t free_port()
    {
        const SOCKETHANDLE probe{ areg::socket_create() };
        sockaddr_in bound{ };
        bound.sin_family = AF_INET;
        bound.sin_port   = 0u;
        ::inet_pton(AF_INET, "127.0.0.1", &bound.sin_addr);
        socklen_t length{ sizeof(bound) };
        uint16_t result{ 0u };
        if ( areg::is_valid_socket(probe)
          && (::bind(probe, reinterpret_cast<sockaddr *>(&bound), sizeof(bound)) == 0)
          && (::getsockname(probe, reinterpret_cast<sockaddr *>(&bound), &length) == 0) )
        {
            result = ntohs(bound.sin_port);
        }

        if (areg::is_valid_socket(probe))
            areg::socket_close(probe);

        return result;
    }

    //!< Rewrites the values of the keys and appends the keys the file does not have.
    std::string rewrite_config(const std::string & source, std::map<std::string, std::string> values)
    {
        std::ifstream input(source);
        std::ostringstream out;
        std::string line;
        while (std::getline(input, line))
        {
            const std::size_t end{ line.find_first_of(" \t=") };
            const std::string key{ line.substr(0u, end) };
            const auto found{ values.find(key) };
            if ((key.empty() == false) && (key[0] != '#') && (found != values.end()))
            {
                out << key << " = " << found->second << "\n";
                values.erase(found);
            }
            else
            {
                out << line << "\n";
            }
        }

        for (const auto & entry : values)
        {
            out << entry.first << " = " << entry.second << "\n";
        }

        return out.str();
    }

    bool write_file(const std::string & path, const std::string & text)
    {
        std::ofstream target(path, std::ios::trunc);
        target << text;
        return target.good();
    }

    //!< Creates the private directory and the three configurations of the case.
    bool prepare(const std::string & name, const std::string & binDir, const std::string & collector)
    {
        _env.binDir     = binDir;
        _env.collector  = collector;
        _env.port       = free_port();
        const std::filesystem::path base{ std::filesystem::temp_directory_path() / ("areg-log-observer-" + name + "-" + std::to_string(areg::Process::instance().id())) };
        std::error_code error;
        std::filesystem::remove_all(base, error);
        std::filesystem::create_directories(base, error);
        _env.dir            = base.generic_string();
        _env.observerCfg    = _env.dir + "/observer.init";
        _env.sourceCfg      = _env.dir + "/source.init";
        _env.collectorCfg   = _env.dir + "/collector.init";
        _env.dbPath         = _env.dir + "/session.sqlog";
        _env.sourceLog      = _env.dir + "/source.log";

        const std::string installed{ binDir + "/config/areg.init" };
        const std::map<std::string, std::string> logger{ {"logger::*::address::tcpip", "127.0.0.1"}, {"logger::*::port::tcpip", std::to_string(_env.port)} };
        std::map<std::string, std::string> observer{ logger };
        observer["log::*::enable::db"]  = "true";
        observer["log::*::db::engine"]  = "sqlite3";
        observer["log::*::db::name"]    = CONFIG_DB_NAME;
        observer["log::*::db::location"]= _env.dir;
        observer["log::*::db::queue"]   = "16";
        return (_env.port != 0u)
            && write_file(_env.observerCfg  , rewrite_config(installed, observer))
            && write_file(_env.sourceCfg    , rewrite_config(installed, logger))
            && write_file(_env.collectorCfg , rewrite_config(installed, logger));
    }

    void cleanup()
    {
        std::error_code error;
        std::filesystem::remove_all(_env.dir, error);
    }

    //!< Makes the log sources of the case write their logs to a file too.
    bool enable_source_log_file()
    {
        const std::map<std::string, std::string> values{ {"log::*::target", "remote | file"}, {"log::*::enable::file", "true"}, {"log::*::file::location", _env.sourceLog} };
        return write_file(_env.sourceCfg, rewrite_config(_env.sourceCfg, values));
    }

    //!< Polls until the log file of the source contains the text.
    bool wait_source_log(const std::string & text)
    {
        const auto until{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
        while (std::chrono::steady_clock::now() < until)
        {
            std::ifstream file(_env.sourceLog);
            const std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
            if (content.find(text) != std::string::npos)
                return true;

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        return false;
    }

    //!< Returns true when a TCP connection to the collector is accepted.
    bool is_listening()
    {
        const SOCKETHANDLE socket{ areg::socket_create() };
        bool result{ false };
        if (areg::is_valid_socket(socket))
        {
            areg::SocketAddress address{ };
            result = address.resolve_address(areg::String("127.0.0.1"), _env.port, false) && areg::client_connect_fd(socket, address);
            areg::socket_close(socket);
        }

        return result;
    }

    //!< Polls until the collector accepts connections.
    bool wait_listening()
    {
        const auto until{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
        while (std::chrono::steady_clock::now() < until)
        {
            if (is_listening())
                return true;

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        return false;
    }

    bool start_collector(Child & collector)
    {
        return collector.start({ _env.collector, "--console", "--load=" + _env.collectorCfg }) && wait_listening();
    }

    bool start_source(Child & source)
    {
        return source.start({ areg::Process::instance().full_path().as_string(), "source", _env.sourceCfg });
    }

    //////////////////////////////////////////////////////////////////////////
    // What the observer was told
    //////////////////////////////////////////////////////////////////////////

    struct ScopeEvent
    {
        ITEM_ID                 cookie;     //!< The source of the scopes.
        std::vector<ScopeInfo>  scopes;     //!< The scopes of the event.
    };

    struct Record
    {
        LogRecord   rec;            //!< The record the callback received.
        uint32_t    deliveredId;    //!< The database id of the record, as seen inside the callback.
    };

    struct StateEvent
    {
        ITEM_ID         cookie;     //!< The source whose state changed.
        unsigned        state;      //!< The new state.
        ITEM_ID         byObserver; //!< The observer that asked for it.
    };

    struct Seen
    {
        std::mutex                  lock;               //!< Guards all members.
        std::condition_variable     changed;            //!< Signalled for every event.
        bool                        cfgCalled   { false };  //!< True after the configuration callback.
        bool                        cfgEnabled  { false };  //!< The enabled flag of the callback.
        std::string                 cfgAddress;         //!< The address of the callback.
        uint16_t                    cfgPort     { 0u }; //!< The port of the callback.
        bool                        dbCfgCalled { false };  //!< True after the database configuration callback.
        std::string                 dbCfgName;          //!< The database name of the callback.
        std::string                 dbCfgLocation;      //!< The database location of the callback.
        bool                        connected   { false };  //!< The last connection state.
        uint32_t                    connectedTrue   { 0u }; //!< Events with the connected state.
        uint32_t                    connectedFalse  { 0u }; //!< Events with the disconnected state.
        std::string                 connAddress;        //!< The address of the last connection event.
        uint16_t                    connPort    { 0u }; //!< The port of the last connection event.
        uint32_t                    startedTrue { 0u }; //!< Started events with true.
        uint32_t                    startedFalse{ 0u }; //!< Started events with false.
        std::vector<std::string>    dbCreated;          //!< Paths the database callback reported.
        uint32_t                    failures    { 0u }; //!< Messaging failure events.
        std::vector<LogInstance>    instances;          //!< Every connected instance reported.
        uint32_t                    instanceBatches { 0u };     //!< Connect events.
        std::vector<ITEM_ID>        disconnected;       //!< Every disconnected instance reported.
        std::vector<ScopeEvent>     registered;         //!< Scope registrations.
        std::vector<ScopeEvent>     updated;            //!< Scope updates.
        std::vector<Record>         records;            //!< Messages received by the record callback.
        std::vector<std::string>    rawTexts;           //!< Message texts received by the raw callback.
        std::vector<StateEvent>     states;             //!< Source state events.
        std::vector<ITEM_ID>        restored;           //!< Sources whose configuration was restored.
        std::set<ITEM_ID>           knownSources;       //!< Sources the test has taken.
        bool                        serviceDisconnectedCalled{ false }; //!< Base class: the service disconnect callback.
    };

    Seen    _seen;

    std::string text_of(const char * text)
    {
        return std::string(text != nullptr ? text : "");
    }

    template<class Predicate>
    bool wait_for(Predicate predicate)
    {
        std::unique_lock<std::mutex> guard(_seen.lock);
        return _seen.changed.wait_for(guard, std::chrono::milliseconds(WAIT_MS), predicate);
    }

    void on_configured(bool isEnabled, const char * address, uint16_t port)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        _seen.cfgCalled = true;
        _seen.cfgEnabled= isEnabled;
        _seen.cfgAddress= text_of(address);
        _seen.cfgPort   = port;
        _seen.changed.notify_all();
    }

    void on_db_configured(bool /* isEnabled */, const char * dbName, const char * dbLocation, const char * /* dbUser */)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        _seen.dbCfgCalled   = true;
        _seen.dbCfgName     = text_of(dbName);
        _seen.dbCfgLocation = text_of(dbLocation);
        _seen.changed.notify_all();
    }

    void on_connected(bool isConnected, const char * address, uint16_t port)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        _seen.connected     = isConnected;
        _seen.connAddress   = text_of(address);
        _seen.connPort      = port;
        (isConnected ? _seen.connectedTrue : _seen.connectedFalse) += 1u;
        _seen.changed.notify_all();
    }

    void on_started(bool isStarted)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        (isStarted ? _seen.startedTrue : _seen.startedFalse) += 1u;
        _seen.changed.notify_all();
    }

    void on_db_created(const char * dbLocation)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        _seen.dbCreated.push_back(text_of(dbLocation));
        _seen.changed.notify_all();
    }

    void on_failed()
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        ++_seen.failures;
        _seen.changed.notify_all();
    }

    void on_instances(const LogInstance * instances, uint32_t count)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        ++_seen.instanceBatches;
        for (uint32_t i = 0u; (instances != nullptr) && (i < count); ++i)
        {
            _seen.instances.push_back(instances[i]);
        }

        _seen.changed.notify_all();
    }

    void on_instances_gone(const ITEM_ID * instances, uint32_t count)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        for (uint32_t i = 0u; (instances != nullptr) && (i < count); ++i)
        {
            _seen.disconnected.push_back(instances[i]);
        }

        _seen.changed.notify_all();
    }

    ScopeEvent make_scope_event(ITEM_ID cookie, const ScopeInfo * scopes, uint32_t count)
    {
        ScopeEvent event{ cookie, { } };
        for (uint32_t i = 0u; (scopes != nullptr) && (i < count); ++i)
        {
            event.scopes.push_back(scopes[i]);
        }

        return event;
    }

    void on_register_scopes(ITEM_ID cookie, const ScopeInfo * scopes, uint32_t count)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        _seen.registered.push_back(make_scope_event(cookie, scopes, count));
        _seen.changed.notify_all();
    }

    void on_update_scopes(ITEM_ID cookie, const ScopeInfo * scopes, uint32_t count)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        _seen.updated.push_back(make_scope_event(cookie, scopes, count));
        _seen.changed.notify_all();
    }

    void on_message(const LogRecord * record)
    {
        const uint32_t delivered{ log_observer_delivered_log_id() };
        std::lock_guard<std::mutex> guard(_seen.lock);
        if (record != nullptr)
        {
            _seen.records.push_back(Record{ *record, delivered });
        }

        _seen.changed.notify_all();
    }

    void on_message_ex(const unsigned char * buffer, uint32_t size)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        if ((buffer != nullptr) && (size >= areg::log_entry_head()))
        {
            const areg::LogEntry & entry{ *reinterpret_cast<const areg::LogEntry *>(buffer) };
            if (entry.logMsgType == areg::LogMessageType::MessageText)
            {
                _seen.rawTexts.push_back(text_of(entry.logMessage));
            }
        }

        _seen.changed.notify_all();
    }

    void on_source_state(ITEM_ID cookie, unsigned char state, ITEM_ID byObserver)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        _seen.states.push_back(StateEvent{ cookie, state, byObserver });
        _seen.changed.notify_all();
    }

    void on_restored(ITEM_ID cookie)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        _seen.restored.push_back(cookie);
        _seen.changed.notify_all();
    }

    ObserverEvents make_events(bool rawMessages)
    {
        ObserverEvents events{ };
        events.evtObserverConfigured= &on_configured;
        events.evtLogDbConfigured   = &on_db_configured;
        events.evtServiceConnected  = &on_connected;
        events.evtLoggingStarted    = &on_started;
        events.evtLogDbCreated      = &on_db_created;
        events.evtMessagingFailed   = &on_failed;
        events.evtInstConnected     = &on_instances;
        events.evtInstDisconnected  = &on_instances_gone;
        events.evtLogRegisterScopes = &on_register_scopes;
        events.evtLogUpdatedScopes  = &on_update_scopes;
        events.evtLogMessage        = rawMessages ? nullptr : &on_message;
        events.evtLogMessageEx      = rawMessages ? &on_message_ex : nullptr;
        events.evtLogSourceState    = &on_source_state;
        events.evtLogConfigRestored = &on_restored;
        return events;
    }

    //////////////////////////////////////////////////////////////////////////
    // Helpers of the observer side
    //////////////////////////////////////////////////////////////////////////

    //!< Waits for a log source that the test has not taken yet and returns its cookie, 0 on timeout.
    ITEM_ID next_source()
    {
        ITEM_ID found{ ID_IGNORE };
        const bool ok{ wait_for([&found]() -> bool
            {
                for (const LogInstance & instance : _seen.instances)
                {
                    const bool isSource{ (instance.liSource & (SOURCE_CLIENT | SOURCE_TEST | SOURCE_SIMULATION)) != 0u };
                    if (isSource && (_seen.knownSources.count(instance.liCookie) == 0u))
                    {
                        found = instance.liCookie;
                        return true;
                    }
                }

                return false;
            }) };

        std::lock_guard<std::mutex> guard(_seen.lock);
        if (ok)
        {
            _seen.knownSources.insert(found);
        }

        return (ok ? found : ID_IGNORE);
    }

    const LogInstance * instance_of(ITEM_ID cookie)
    {
        for (const LogInstance & instance : _seen.instances)
        {
            if (instance.liCookie == cookie)
                return &instance;
        }

        return nullptr;
    }

    //!< Waits for the scope of the test source in the scope list of the cookie, starting at the given event.
    bool wait_scope(const std::vector<ScopeEvent> & list, ITEM_ID cookie, std::size_t from, ScopeInfo & scope)
    {
        return wait_for([&]() -> bool
            {
                for (std::size_t i = from; i < list.size(); ++i)
                {
                    if (list[i].cookie != cookie)
                        continue;

                    for (const ScopeInfo & info : list[i].scopes)
                    {
                        if (std::strstr(info.lsName, "log_observer_test_source_Scope") != nullptr)
                        {
                            scope = info;
                            return true;
                        }
                    }
                }

                return false;
            });
    }

    std::size_t count_of(const std::vector<ScopeEvent> & list)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        return list.size();
    }

    //!< Returns true if a message record of the exact text was received.
    bool has_text(const std::string & text)
    {
        for (const Record & record : _seen.records)
        {
            if ((record.rec.msgType == MessageText) && (text == record.rec.msgLogText))
                return true;
        }

        return false;
    }

    bool wait_text(const std::string & text)
    {
        return wait_for([&text]() -> bool { return has_text(text); });
    }

    bool has_text_locked(const std::string & text)
    {
        std::lock_guard<std::mutex> guard(_seen.lock);
        return has_text(text);
    }

    const Record * record_of(const std::string & text)
    {
        for (const Record & record : _seen.records)
        {
            if ((record.rec.msgType == MessageText) && (text == record.rec.msgLogText))
                return &record;
        }

        return nullptr;
    }

    bool wait_disconnected(ITEM_ID cookie)
    {
        return wait_for([cookie]() -> bool
            {
                return std::find(_seen.disconnected.begin(), _seen.disconnected.end(), cookie) != _seen.disconnected.end();
            });
    }

    bool wait_state(ITEM_ID cookie, unsigned state, ITEM_ID & byObserver)
    {
        return wait_for([&]() -> bool
            {
                for (const StateEvent & event : _seen.states)
                {
                    if ((event.cookie == cookie) && (event.state == state))
                    {
                        byObserver = event.byObserver;
                        return true;
                    }
                }

                return false;
            });
    }

    //!< Initializes the observer with the callbacks and connects it to the collector.
    bool start_observer(bool rawMessages, const char * dbPath)
    {
        ObserverEvents events{ make_events(rawMessages) };
        REQUIRE(log_observer_initialize(&events, _env.observerCfg.c_str()));
        REQUIRE(log_observer_connect_logger(dbPath, "127.0.0.1", _env.port));
        REQUIRE(wait_for([]() -> bool { return _seen.connected && (_seen.startedTrue != 0u); }));
        return true;
    }

    void stop_observer()
    {
        log_observer_disconnect_logger();
        log_observer_release();
    }

    //!< The observer connects to a running collector, optionally with a source, and releases at the end.
    struct Session
    {
        Child   collector   { "-q" };   //!< The log collector.
        Child   source;                 //!< The first log source.
        ITEM_ID cookie      { ID_IGNORE };  //!< The cookie of the first log source.
        ScopeInfo scope     { };        //!< The scope of the first log source.

        bool open(bool withSource, bool rawMessages = false)
        {
            REQUIRE(start_collector(collector));
            REQUIRE(start_observer(rawMessages, _env.dbPath.c_str()));
            if (withSource)
            {
                REQUIRE(start_source(source));
                cookie = next_source();
                REQUIRE(cookie != ID_IGNORE);
                REQUIRE(wait_scope(_seen.registered, cookie, 0u, scope));
            }

            return true;
        }

        bool close()
        {
            bool result{ true };
            if (source.is_open())
            {
                result = source.send("exit");
                result = source.finish() && result;
            }

            stop_observer();
            return collector.finish() && result;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Cases
    //////////////////////////////////////////////////////////////////////////

    //!< The calls that need an initialized observer return their neutral result.
    void check_uninitialized_api()
    {
        ScopeInfo scope{ };
        char buffer[16]{ };
        CHECK(log_observer_is_initialized() == false);
        CHECK(log_observer_is_connected() == false);
        CHECK(log_observer_is_started() == false);
        CHECK(log_observer_current_state() == ObserverUninitialized);
        CHECK(log_observer_logger_address() == nullptr);
        CHECK(log_observer_logger_port() == 0u);
        CHECK(log_observer_config_logger_enabled() == false);
        CHECK(log_observer_config_logger_address(buffer, sizeof(buffer)) == false);
        CHECK(log_observer_config_logger_port() == 0u);
        CHECK(log_observer_connect_logger(nullptr, "127.0.0.1", 1u) == false);
        CHECK(log_observer_pause_logging(true) == false);
        CHECK(log_observer_stop_logging(true, nullptr) == false);
        CHECK(log_observer_request_instances() == false);
        CHECK(log_observer_request_scopes(areg::COOKIE_LOGGER) == false);
        CHECK(log_observer_request_change_scope_prio(areg::COOKIE_LOGGER, &scope, 1u) == false);
        CHECK(log_observer_request_save_config(areg::COOKIE_LOGGER) == false);
        CHECK(log_observer_request_restore_config(areg::COOKIE_LOGGER) == false);
        CHECK(log_observer_request_source_state(areg::COOKIE_LOGGER, 1u) == false);
        CHECK(log_observer_add_log(areg::COOKIE_LOGGER, PrioInfo, 0u, "text") == 0u);
        CHECK(log_observer_add_marker(1u, PrioInfo, "author", "note") == 0u);
        CHECK(log_observer_remove_log(1u) == false);
        CHECK(log_observer_remove_marker(1u) == false);
        CHECK(log_observer_queue_state(nullptr) == false);
        log_observer_disconnect_logger();
        log_observer_release();
        CHECK(log_observer_is_initialized() == false);
    }

    bool case_calls_before_initialize()
    {
        check_uninitialized_api();
        return true;
    }

    bool case_one_framework()
    {
        ObserverEvents events{ make_events(false) };
        CHECK(areg::Application::is_configured() == false);
        REQUIRE(log_observer_initialize(&events, _env.observerCfg.c_str()));

        // Reproduces the defect: a static areg with a shared areglogger gave the process two frameworks.
        // The observer configured its own copy, and this executable saw a framework never configured.
        CHECK(areg::Application::is_configured());

        log_observer_release();
        return true;
    }

    bool case_calls_after_release()
    {
        ObserverEvents events{ make_events(false) };
        REQUIRE(log_observer_initialize(&events, _env.observerCfg.c_str()));
        CHECK(log_observer_current_state() == ObserverDisconnected);
        log_observer_release();
        check_uninitialized_api();

        LogQueueState queue{ };
        CHECK(log_observer_queue_state(&queue) == false);
        CHECK(log_observer_active_database_path(nullptr, 0) == 0);
        CHECK(log_observer_delivered_log_id() == 0u);
        return true;
    }

    bool case_offline_lifecycle()
    {
        ObserverEvents events{ make_events(false) };
        REQUIRE(log_observer_initialize(&events, _env.observerCfg.c_str()));
        REQUIRE(log_observer_initialize(&events, _env.observerCfg.c_str()));
        CHECK(log_observer_is_initialized());
        CHECK(log_observer_current_state() == ObserverDisconnected);
        CHECK(log_observer_is_connected() == false);
        CHECK(log_observer_is_started() == false);

        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            CHECK(_seen.cfgCalled);
            CHECK(_seen.cfgEnabled);
            CHECK(_seen.cfgAddress == "127.0.0.1");
            CHECK(_seen.cfgPort == _env.port);
            CHECK(_seen.dbCfgCalled);
            CHECK(_seen.dbCfgName == CONFIG_DB_NAME);
            CHECK(_seen.dbCfgLocation.empty() == false);
        }

        CHECK(log_observer_config_logger_enabled());
        CHECK(log_observer_config_logger_port() == _env.port);
        CHECK(log_observer_config_logger_address(nullptr, 16u) == false);

        const int32_t nameSize{ log_observer_config_database_name(nullptr, 0) };
        CHECK(nameSize == static_cast<int32_t>(std::strlen(CONFIG_DB_NAME) + 1u));
        CHECK(log_observer_config_database_path(nullptr, 0) > nameSize);
        CHECK(log_observer_config_database_location(nullptr, 0) > 0);
        CHECK(log_observer_set_config_database_name("renamed.sqlog"));
        CHECK(log_observer_config_database_name(nullptr, 0) == static_cast<int32_t>(std::strlen("renamed.sqlog") + 1u));
        CHECK(log_observer_set_config_database_location((_env.dir + "/other").c_str()));
        CHECK(log_observer_config_database_location(nullptr, 0) >= static_cast<int32_t>(_env.dir.size() + 7u));
        CHECK(log_observer_active_database_path(nullptr, 0) == 0);
        CHECK(log_observer_initial_database_path(nullptr, 0) == 0);

        const uint16_t other{ static_cast<uint16_t>(_env.port + 1u) };
        CHECK(log_observer_config_update("127.0.0.1", other, (_env.dir + "/changed.sqlog").c_str(), false));
        CHECK(log_observer_config_logger_port() == other);
        CHECK(log_observer_config_update("127.0.0.1", _env.port, (_env.dir + "/changed.sqlog").c_str(), true) == false);
        CHECK(log_observer_config_logger_port() == _env.port);

        CHECK(log_observer_request_instances() == false);
        CHECK(log_observer_request_scopes(areg::COOKIE_LOGGER) == false);
        CHECK(log_observer_request_save_config(areg::COOKIE_LOGGER) == false);
        CHECK(log_observer_pause_logging(true));
        CHECK(log_observer_current_state() == ObserverDisconnected);
        CHECK(log_observer_stop_logging(true, nullptr) == false);

        log_observer_release();
        CHECK(log_observer_is_initialized());
        log_observer_release();
        CHECK(log_observer_is_initialized() == false);
        CHECK(log_observer_current_state() == ObserverUninitialized);
        return true;
    }

    bool case_connect_without_collector()
    {
        ObserverEvents events{ make_events(false) };
        REQUIRE(log_observer_initialize(&events, _env.observerCfg.c_str()));
        const bool started{ log_observer_connect_logger(_env.dbPath.c_str(), "127.0.0.1", _env.port) };
        CHECK(started == (log_observer_current_state() == ObserverConnected));
        if (started)
        {
            CHECK(wait_for([]() -> bool { return _seen.failures != 0u; }));
        }

        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            CHECK(_seen.connectedTrue == 0u);
        }

        log_observer_disconnect_logger();
        CHECK(log_observer_current_state() == ObserverDisconnected);
        CHECK(log_observer_is_connected() == false);
        log_observer_release();
        return true;
    }

    bool case_session_messages()
    {
        Session session;
        REQUIRE(session.open(true));

        CHECK(log_observer_current_state() == ObserverConnected);
        CHECK(log_observer_is_connected());
        CHECK(log_observer_is_started());
        CHECK(std::strcmp(log_observer_logger_address(), "127.0.0.1") == 0);
        CHECK(log_observer_logger_port() == _env.port);
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            CHECK(_seen.connAddress == "127.0.0.1");
            CHECK(_seen.connPort == _env.port);
            CHECK(_seen.dbCreated.empty() == false);
            const LogInstance * instance{ instance_of(session.cookie) };
            REQUIRE(instance != nullptr);
            CHECK(std::strstr(instance->liName, EXE_NAME) != nullptr);
            CHECK(instance->liBitness == static_cast<uint32_t>(sizeof(void *) == 8u ? BITNESS_64BITS : BITNESS_32BITS));
            CHECK(instance->liTimestamp != 0);
        }

        REQUIRE(session.source.send("dbg first message"));
        REQUIRE(wait_text("first message"));
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            const Record * record{ record_of("first message") };
            REQUIRE(record != nullptr);
            CHECK(record->rec.msgPriority == PrioDebug);
            CHECK(record->rec.msgScopeId == session.scope.lsId);
            CHECK(record->rec.msgTimestamp != 0u);
            CHECK(record->rec.msgReceived != 0u);
            CHECK(std::strstr(record->rec.msgModule, EXE_NAME) != nullptr);
            CHECK(record->rec.msgThreadId != 0u);
            CHECK(record->deliveredId != 0u);
        }

        CHECK(wait_for([&session]() -> bool
            {
                for (const Record & record : _seen.records)
                {
                    if ((record.rec.msgType == LogScopeExit) && (record.rec.msgScopeId == session.scope.lsId))
                        return true;
                }

                return false;
            }));

        const uint32_t batches{ [] { std::lock_guard<std::mutex> guard(_seen.lock); return _seen.instanceBatches; }() };
        CHECK(log_observer_request_instances());
        CHECK(wait_for([batches]() -> bool { return _seen.instanceBatches > batches; }));

        const std::size_t registered{ count_of(_seen.registered) };
        CHECK(log_observer_request_scopes(session.cookie));
        ScopeInfo again{ };
        CHECK(wait_scope(_seen.registered, session.cookie, registered, again));
        CHECK(again.lsId == session.scope.lsId);
        CHECK(log_observer_request_scopes(ID_IGNORE) == false);

        REQUIRE(session.close());
        CHECK(log_observer_is_initialized() == false);
        return true;
    }

    bool case_source_leaves()
    {
        Session session;
        REQUIRE(session.open(true));
        REQUIRE(session.source.send("exit"));
        REQUIRE(session.source.finish());
        CHECK(wait_disconnected(session.cookie));
        CHECK(log_observer_is_connected());
        REQUIRE(session.close());
        return true;
    }

    //!< Reproduces the defect: a child started through a shell gets a path with shell syntax changed.
    bool case_child_path_with_shell_syntax()
    {
        const std::string config{ _env.dir + "/$(echo changed) source.init" };
        std::error_code error;
        std::filesystem::rename(_env.sourceCfg, config, error);
        REQUIRE(error.value() == 0);
        _env.sourceCfg = config;

        Session session;
        REQUIRE(session.open(true));
        REQUIRE(session.close());
        return true;
    }

    bool case_scope_priority()
    {
        Session session;
        REQUIRE(session.open(true));

        ScopeInfo change{ session.scope };
        change.lsPrio = PrioError;
        const std::size_t updated{ count_of(_seen.updated) };
        CHECK(log_observer_request_change_scope_prio(ID_IGNORE, &change, 1u) == false);
        REQUIRE(log_observer_request_change_scope_prio(session.cookie, &change, 1u));
        ScopeInfo result{ };
        REQUIRE(wait_scope(_seen.updated, session.cookie, updated, result));
        CHECK(result.lsId == session.scope.lsId);
        CHECK((result.lsPrio & PrioError) != 0u);
        CHECK((result.lsPrio & PrioDebug) == 0u);

        REQUIRE(session.source.send("dbg filtered out"));
        REQUIRE(session.source.send("err passes"));
        REQUIRE(wait_text("passes"));
        CHECK(has_text_locked("filtered out") == false);
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            const Record * record{ record_of("passes") };
            REQUIRE(record != nullptr);
            CHECK(record->rec.msgPriority == PrioError);
        }

        REQUIRE(session.close());
        return true;
    }

    bool case_source_state()
    {
        Session session;
        REQUIRE(session.open(true));

        CHECK(log_observer_request_source_state(ID_IGNORE, 1u) == false);
        CHECK(log_observer_request_source_state(session.cookie, 0u) == false);
        CHECK(log_observer_request_source_state(session.cookie, 99u) == false);

        ITEM_ID byObserver{ ID_IGNORE };
        REQUIRE(log_observer_request_source_state(session.cookie, 2u));
        REQUIRE(wait_state(session.cookie, 2u, byObserver));
        CHECK(byObserver != ID_IGNORE);
        REQUIRE(session.source.send("err while paused"));
        REQUIRE(log_observer_request_source_state(session.cookie, 1u));
        REQUIRE(wait_state(session.cookie, 1u, byObserver));
        REQUIRE(session.source.send("err after resume"));
        REQUIRE(wait_text("after resume"));

        REQUIRE(session.close());
        return true;
    }

    //! A log the logging thread handles while the source is paused is not sent, and not replayed after the resume.
    bool case_source_paused_drops_produced_logs()
    {
        REQUIRE(enable_source_log_file());
        Session session;
        REQUIRE(session.open(true));

        ITEM_ID byObserver{ ID_IGNORE };
        REQUIRE(log_observer_request_source_state(session.cookie, 2u));
        REQUIRE(wait_state(session.cookie, 2u, byObserver));
        REQUIRE(session.source.send("err while paused"));
        REQUIRE(session.source.send("err paused fence"));
        // The logging thread handles the logs in order: the second one in the file means the first one is done.
        REQUIRE(wait_source_log("paused fence"));
        REQUIRE(log_observer_request_source_state(session.cookie, 1u));
        REQUIRE(wait_state(session.cookie, 1u, byObserver));
        REQUIRE(session.source.send("err after resume"));
        REQUIRE(wait_text("after resume"));
        CHECK(has_text_locked("while paused") == false);
        CHECK(has_text_locked("paused fence") == false);

        REQUIRE(session.close());
        return true;
    }

    bool case_config_save_restore()
    {
        Session session;
        REQUIRE(session.open(true));

        CHECK(log_observer_request_save_config(ID_IGNORE) == false);
        CHECK(log_observer_request_restore_config(ID_IGNORE) == false);
        CHECK(log_observer_request_save_config(areg::COOKIE_LOGGER));
        CHECK(log_observer_request_save_config(session.cookie));
        CHECK(log_observer_request_restore_config(session.cookie));
        CHECK(wait_for([&session]() -> bool
            {
                return std::find(_seen.restored.begin(), _seen.restored.end(), session.cookie) != _seen.restored.end();
            }));

        REQUIRE(session.close());
        return true;
    }

    bool case_pause_resume()
    {
        Session session;
        REQUIRE(session.open(false));

        REQUIRE(log_observer_pause_logging(true));
        CHECK(log_observer_current_state() == ObserverPaused);
        CHECK(log_observer_is_started() == false);
        CHECK(log_observer_is_connected());
        CHECK(wait_for([]() -> bool { return _seen.startedFalse != 0u; }));

        Child paused;
        REQUIRE(start_source(paused));
        const ITEM_ID pausedCookie{ next_source() };
        REQUIRE(pausedCookie != ID_IGNORE);
        REQUIRE(paused.send("err sent while observer paused"));
        REQUIRE(paused.send("exit"));
        REQUIRE(paused.finish());
        REQUIRE(wait_disconnected(pausedCookie));

        const uint32_t started{ [] { std::lock_guard<std::mutex> guard(_seen.lock); return _seen.startedTrue; }() };
        REQUIRE(log_observer_pause_logging(false));
        CHECK(log_observer_current_state() == ObserverConnected);
        CHECK(log_observer_is_started());
        CHECK(wait_for([started]() -> bool { return _seen.startedTrue > started; }));

        Child resumed;
        REQUIRE(start_source(resumed));
        REQUIRE(next_source() != ID_IGNORE);
        REQUIRE(resumed.send("err sent after observer resumed"));
        REQUIRE(wait_text("sent after observer resumed"));
        CHECK(has_text_locked("sent while observer paused") == false);
        REQUIRE(resumed.send("exit"));
        REQUIRE(resumed.finish());

        REQUIRE(session.close());
        return true;
    }

    bool case_local_logs_and_database()
    {
        Session session;
        REQUIRE(session.open(true));

        const uint32_t id{ log_observer_add_log(session.cookie, PrioInfo, 0u, "local note") };
        CHECK(id != 0u);
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            const Record * record{ record_of("local note") };
            REQUIRE(record != nullptr);
            CHECK(record->deliveredId == id);
            CHECK(record->rec.msgPriority == PrioInfo);
        }

        CHECK(log_observer_add_log(session.cookie, PrioInfo, 0u, nullptr) != 0u);
        const uint32_t marker{ log_observer_add_marker(id, 0x0501u, "tester", "marker note") };
        CHECK(marker != 0u);
        CHECK(log_observer_add_marker(0u, 0x0501u, "tester", "no master") == 0u);
        CHECK(log_observer_add_marker(id, PrioInfo, "tester", "not a marker level") == 0u);
        CHECK(log_observer_remove_marker(marker));
        CHECK(log_observer_remove_log(id));
        CHECK(log_observer_remove_log(0u) == false);
        CHECK(log_observer_remove_marker(0u) == false);
        CHECK(log_observer_delivered_log_id() == 0u);

        LogQueueState queue{ };
        CHECK(log_observer_queue_state(&queue));
        CHECK(queue.lqLimitBytes != 0u);
        CHECK(queue.lqDropped == 0u);
        CHECK(log_observer_queue_state(nullptr) == false);

        const int32_t activeSize{ log_observer_active_database_path(nullptr, 0) };
        CHECK(activeSize > static_cast<int32_t>(std::strlen("session.sqlog")));
        CHECK(log_observer_initial_database_path(nullptr, 0) > static_cast<int32_t>(std::strlen("session.sqlog")));
        CHECK(log_observer_config_database_path(nullptr, 0) > static_cast<int32_t>(std::strlen(CONFIG_DB_NAME)));

        REQUIRE(session.close());
        return true;
    }

    bool case_reconnect_and_stop()
    {
        Session session;
        REQUIRE(session.open(false));

        REQUIRE(log_observer_stop_logging(true, nullptr));
        CHECK(log_observer_current_state() == ObserverPaused);
        const std::string second{ _env.dir + "/second.sqlog" };
        REQUIRE(log_observer_stop_logging(false, second.c_str()));
        CHECK(log_observer_current_state() == ObserverConnected);
        CHECK(log_observer_active_database_path(nullptr, 0) > static_cast<int32_t>(std::strlen("second.sqlog")));

        log_observer_disconnect_logger();
        CHECK(log_observer_current_state() == ObserverDisconnected);
        CHECK(log_observer_is_connected() == false);
        CHECK(log_observer_is_started() == false);
        CHECK(log_observer_pause_logging(false));
        CHECK(log_observer_current_state() == ObserverDisconnected);

        const uint32_t connected{ [] { std::lock_guard<std::mutex> guard(_seen.lock); return _seen.connectedTrue; }() };
        REQUIRE(log_observer_connect_logger(nullptr, "127.0.0.1", _env.port));
        REQUIRE(wait_for([connected]() -> bool { return _seen.connectedTrue > connected; }));
        CHECK(log_observer_current_state() == ObserverConnected);
        CHECK(log_observer_connect_logger(nullptr, "127.0.0.1", _env.port));

        REQUIRE(session.close());
        return true;
    }

    bool case_collector_restart()
    {
        Session session;
        REQUIRE(session.open(false));

        uint32_t startedFalse{ 0u };
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            startedFalse = _seen.startedFalse;
        }

        REQUIRE(session.collector.finish());
        CHECK(wait_for([startedFalse]() -> bool { return _seen.startedFalse > startedFalse; }));
        CHECK(log_observer_current_state() == ObserverConnected);

        uint32_t connected{ 0u };
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            connected = _seen.connectedTrue;
        }

        REQUIRE(start_collector(session.collector));
        REQUIRE(wait_for([connected]() -> bool { return _seen.connectedTrue > connected; }));
        CHECK(log_observer_is_connected());
        CHECK(log_observer_request_instances());

        stop_observer();
        REQUIRE(session.collector.finish());
        return true;
    }

    bool case_paused_observer_reconnects_paused()
    {
        Session session;
        REQUIRE(session.open(false));
        REQUIRE(log_observer_pause_logging(true));
        REQUIRE(wait_for([]() -> bool { return _seen.startedFalse != 0u; }));

        uint32_t connected{ 0u };
        uint32_t startedTrue{ 0u };
        uint32_t startedFalse{ 0u };
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            connected   = _seen.connectedTrue;
            startedTrue = _seen.startedTrue;
            startedFalse= _seen.startedFalse;
        }

        REQUIRE(session.collector.finish());
        REQUIRE(wait_for([startedFalse]() -> bool { return _seen.startedFalse > startedFalse; }));
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            startedFalse = _seen.startedFalse;
        }

        // The observer connects again by itself and stays paused: the connection reports not started.
        REQUIRE(start_collector(session.collector));
        REQUIRE(wait_for([connected, startedFalse]() -> bool { return (_seen.connectedTrue > connected) && (_seen.startedFalse > startedFalse); }));
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            CHECK(_seen.startedTrue == startedTrue);
        }

        CHECK(log_observer_current_state() == ObserverPaused);
        CHECK(log_observer_is_started() == false);
        REQUIRE(log_observer_pause_logging(false));
        CHECK(wait_for([startedTrue]() -> bool { return _seen.startedTrue > startedTrue; }));
        CHECK(log_observer_is_started());

        stop_observer();
        REQUIRE(session.collector.finish());
        return true;
    }

    bool case_release_while_connected()
    {
        Session session;
        REQUIRE(session.open(true));

        uint32_t connectedFalse{ 0u };
        std::size_t disconnected{ 0u };
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            connectedFalse  = _seen.connectedFalse;
            disconnected    = _seen.disconnected.size();
        }

        // The release drops the callbacks first: nothing is reported while the connection closes.
        log_observer_release();
        CHECK(log_observer_is_initialized() == false);
        CHECK(log_observer_current_state() == ObserverUninitialized);
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            CHECK(_seen.connectedFalse == connectedFalse);
            CHECK(_seen.disconnected.size() == disconnected);
        }

        // The observer starts again and sees the source that stayed.
        const std::size_t known{ [] { std::lock_guard<std::mutex> guard(_seen.lock); return _seen.instances.size(); }() };
        REQUIRE(start_observer(false, _env.dbPath.c_str()));
        CHECK(wait_for([known, &session]() -> bool
            {
                for (std::size_t i = known; i < _seen.instances.size(); ++i)
                {
                    if (_seen.instances[i].liCookie == session.cookie)
                        return true;
                }

                return false;
            }));

        REQUIRE(session.close());
        return true;
    }

    bool case_pause_before_the_connection()
    {
        ObserverEvents events{ make_events(false) };
        REQUIRE(log_observer_initialize(&events, _env.observerCfg.c_str()));
        const bool started{ log_observer_connect_logger(_env.dbPath.c_str(), "127.0.0.1", _env.port) };
        if (started)
        {
            // Nothing is connected, so neither the pause nor the resume reports a started observer.
            REQUIRE(log_observer_pause_logging(true));
            CHECK(log_observer_current_state() == ObserverPaused);
            CHECK(log_observer_is_started() == false);
            REQUIRE(log_observer_pause_logging(false));
            CHECK(log_observer_current_state() == ObserverConnected);
            {
                std::lock_guard<std::mutex> guard(_seen.lock);
                CHECK(_seen.startedTrue == 0u);
                CHECK(_seen.connectedTrue == 0u);
            }
        }

        log_observer_disconnect_logger();
        CHECK(log_observer_current_state() == ObserverDisconnected);
        log_observer_release();
        return true;
    }

    bool case_raw_message_callback()
    {
        Session session;
        REQUIRE(session.open(true, true));

        REQUIRE(session.source.send("err raw callback text"));
        REQUIRE(wait_for([]() -> bool
            {
                return std::find(_seen.rawTexts.begin(), _seen.rawTexts.end(), "raw callback text") != _seen.rawTexts.end();
            }));
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            CHECK(_seen.records.empty());
        }

        REQUIRE(session.close());
        return true;
    }

    bool case_invalid_requests()
    {
        Session session;
        REQUIRE(session.open(true));

        ScopeInfo scope{ session.scope };
        CHECK(log_observer_request_change_scope_prio(ID_IGNORE, &scope, 1u) == false);
        CHECK(log_observer_request_scopes(ID_IGNORE) == false);
        CHECK(log_observer_request_save_config(ID_IGNORE) == false);
        CHECK(log_observer_request_restore_config(ID_IGNORE) == false);
        CHECK(log_observer_request_source_state(ID_IGNORE, 1u) == false);
        CHECK(log_observer_request_source_state(session.cookie, 4u) == false);
        CHECK(log_observer_config_logger_address(nullptr, 0u) == false);
        CHECK(log_observer_active_database_path(nullptr, 0) > 0);
        CHECK(log_observer_remove_log(0u) == false);
        CHECK(log_observer_remove_marker(0u) == false);

        REQUIRE(session.close());
        return true;
    }

    //! Documented contract: a large enough buffer receives the path and its length is returned.
    bool case_path_getters_fill_buffer()
    {
        ObserverEvents events{ make_events(false) };
        REQUIRE(log_observer_initialize(&events, _env.observerCfg.c_str()));

        char name[256]{ };
        const int32_t needed{ log_observer_config_database_name(nullptr, 0) };
        CHECK(needed == static_cast<int32_t>(std::strlen(CONFIG_DB_NAME) + 1u));
        CHECK(log_observer_config_database_name(name, sizeof(name)) == static_cast<int32_t>(std::strlen(CONFIG_DB_NAME)));
        CHECK(std::strcmp(name, CONFIG_DB_NAME) == 0);

        char path[1024]{ };
        const int32_t pathSize{ log_observer_config_database_path(nullptr, 0) };
        CHECK(log_observer_config_database_path(path, sizeof(path)) == pathSize - 1);
        CHECK(std::strstr(path, CONFIG_DB_NAME) != nullptr);

        char location[1024]{ };
        const int32_t locationSize{ log_observer_config_database_location(nullptr, 0) };
        CHECK(log_observer_config_database_location(location, sizeof(location)) == locationSize - 1);
        CHECK(location[0] != '\0');

        log_observer_release();
        return true;
    }

    //! Documented contract: a large enough buffer receives the address, a small one is refused.
    bool case_config_address_fills_buffer()
    {
        ObserverEvents events{ make_events(false) };
        REQUIRE(log_observer_initialize(&events, _env.observerCfg.c_str()));

        char address[16]{ };
        CHECK(log_observer_config_logger_address(address, sizeof(address)));
        CHECK(std::strcmp(address, "127.0.0.1") == 0);
        char tiny[4]{ };
        CHECK(log_observer_config_logger_address(tiny, sizeof(tiny)) == false);

        log_observer_release();
        return true;
    }

    //! A LogObserverBase that records the calls it gets.
    class TestObserver : public areg::logger::LogObserverBase
    {
    public:
        TestObserver() = default;

        ~TestObserver() override = default;

    protected:
        void on_log_observer_configured(bool isEnabled, const std::string & address, uint16_t port) override
        {
            on_configured(isEnabled, address.c_str(), port);
        }

        void on_log_db_configured(bool isEnabled, const std::string & dbName, const std::string & dbLocation, const std::string & dbUser) override
        {
            on_db_configured(isEnabled, dbName.c_str(), dbLocation.c_str(), dbUser.c_str());
        }

        void on_log_service_connected(bool isConnected, const std::string & address, uint16_t port) override
        {
            on_connected(isConnected, address.c_str(), port);
        }

        void on_log_observer_started(bool isStarted) override
        {
            on_started(isStarted);
        }

        void on_log_db_created(const std::string & dbLocation) override
        {
            on_db_created(dbLocation.c_str());
        }

        void on_log_messaging_failed() override
        {
            on_failed();
        }

        void on_log_instances_connect(const std::vector<areg::ConnectedInstance> & instances) override
        {
            std::vector<LogInstance> list;
            for (const areg::ConnectedInstance & instance : instances)
            {
                LogInstance info{ };
                info.liSource   = static_cast<uint32_t>(instance.ciSource);
                info.liBitness  = static_cast<uint32_t>(instance.ciBitness);
                info.liCookie   = instance.ciCookie;
                info.liTimestamp= instance.ciTimestamp;
                std::snprintf(info.liName, sizeof(info.liName), "%s", instance.ciInstance.c_str());
                list.push_back(info);
            }

            on_instances(list.data(), static_cast<uint32_t>(list.size()));
        }

        void on_log_instances_disconnect(const std::vector<areg::ConnectedInstance> & instances) override
        {
            std::vector<ITEM_ID> list;
            for (const areg::ConnectedInstance & instance : instances)
            {
                list.push_back(instance.ciCookie);
            }

            on_instances_gone(list.data(), static_cast<uint32_t>(list.size()));
        }

        void on_log_service_disconnected() override
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            _seen.serviceDisconnectedCalled = true;
            _seen.changed.notify_all();
        }

        void on_log_register_scopes(ITEM_ID cookie, const ScopeInfo * scopes, int32_t count) override
        {
            on_register_scopes(cookie, scopes, static_cast<uint32_t>(count));
        }

        void on_log_update_scopes(ITEM_ID cookie, const ScopeInfo * scopes, int32_t count) override
        {
            on_update_scopes(cookie, scopes, static_cast<uint32_t>(count));
        }

        void on_log_message(const areg::MessageEnvelope & logMessage) override
        {
            on_message_ex(logMessage.buffer(), logMessage.size_used());
        }

        void on_log_source_state(ITEM_ID cookie, areg::LogSourceState state, ITEM_ID byObserver) override
        {
            on_source_state(cookie, static_cast<unsigned char>(state), byObserver);
        }

        void on_log_configuration_restored(ITEM_ID cookie) override
        {
            on_restored(cookie);
        }
    };

    bool case_observer_base_class()
    {
        Child collector{ "-q" };
        REQUIRE(start_collector(collector));

        TestObserver observer;
        CHECK(observer.is_initialized() == false);
        REQUIRE(observer.initialize(_env.observerCfg));
        CHECK(observer.is_initialized());
        CHECK(observer.is_connected() == false);
        CHECK(observer.config_logger_enabled());
        CHECK(observer.config_logger_port() == _env.port);
        CHECK(observer.config_logger_address() == "127.0.0.1");
        CHECK(observer.config_logger_database_name() == CONFIG_DB_NAME);
        observer.set_config_logger_address("127.0.0.1");
        observer.set_config_logger_port(_env.port);
        observer.set_config_logger_connection("127.0.0.1", _env.port);
        observer.set_config_logger_database_name("base.sqlog");
        CHECK(observer.config_logger_database_name() == "base.sqlog");
        observer.set_config_logger_database_location(_env.dir);
        CHECK(observer.config_logger_database_location().empty() == false);
        observer.set_config_logger_database(_env.dir + "/base.sqlog");
        CHECK(observer.config_logger_database().find("base.sqlog") != std::string::npos);
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            CHECK(_seen.cfgCalled);
            CHECK(_seen.dbCfgCalled);
        }

        REQUIRE(observer.connect("127.0.0.1", _env.port, _env.dbPath));
        REQUIRE(wait_for([]() -> bool { return _seen.connected && (_seen.startedTrue != 0u); }));
        CHECK(observer.is_connected());
        CHECK(observer.is_stated());
        CHECK(observer.logger_ip_address() == "127.0.0.1");
        CHECK(observer.logger_port() == _env.port);
        CHECK(observer.logger_address().host_port() == _env.port);
        CHECK(observer.active_database_path().find("session.sqlog") != std::string::npos);
        CHECK(observer.init_database_path().find("session.sqlog") != std::string::npos);

        Child source;
        REQUIRE(start_source(source));
        const ITEM_ID cookie{ next_source() };
        REQUIRE(cookie != ID_IGNORE);
        ScopeInfo scope{ };
        REQUIRE(wait_scope(_seen.registered, cookie, 0u, scope));

        REQUIRE(source.send("dbg base class message"));
        REQUIRE(wait_for([]() -> bool
            {
                return std::find(_seen.rawTexts.begin(), _seen.rawTexts.end(), "base class message") != _seen.rawTexts.end();
            }));
        CHECK(observer.delivered_log_id() == 0u);

        CHECK(observer.request_instances());
        CHECK(observer.request_scopes(cookie));
        ScopeInfo change{ scope };
        change.lsPrio = PrioError;
        const std::size_t updated{ count_of(_seen.updated) };
        CHECK(observer.request_change_scope_prio(cookie, &change, 1u));
        ScopeInfo result{ };
        CHECK(wait_scope(_seen.updated, cookie, updated, result));
        ITEM_ID by{ ID_IGNORE };
        CHECK(observer.request_source_state(cookie, areg::LogSourceState::Paused));
        CHECK(wait_state(cookie, 2u, by));
        CHECK(observer.request_source_state(cookie, areg::LogSourceState::Active));
        CHECK(wait_state(cookie, 1u, by));
        CHECK(observer.request_save_config(cookie));
        CHECK(observer.request_restore_config(cookie));
        CHECK(wait_for([cookie]() -> bool { return std::find(_seen.restored.begin(), _seen.restored.end(), cookie) != _seen.restored.end(); }));
        observer.save_logger_config();

        LogQueueState queue{ };
        CHECK(observer.queue_state(queue));
        const uint32_t logId{ log_observer_add_log(cookie, PrioInfo, 0u, "base local note") };
        CHECK(logId != 0u);
        const uint32_t marker{ observer.add_marker(logId, areg::LogPriority::PrioMarkerInfo, "tester", "base marker") };
        CHECK(marker != 0u);
        CHECK(observer.remove_marker(marker));

        REQUIRE(source.send("exit"));
        REQUIRE(source.finish());
        CHECK(wait_disconnected(cookie));

        CHECK(observer.pause());
        CHECK(observer.is_stated() == false);
        CHECK(observer.resume());
        CHECK(observer.is_stated());
        CHECK(observer.stop());
        CHECK(observer.restart(_env.dbPath));
        {
            std::vector<areg::String> names;
            std::vector<ITEM_ID> ids;
            std::vector<ITEM_ID> threads;
            std::vector<areg::ConnectedInstance> infos;
            std::vector<areg::ScopeEntry> scopes;
            std::vector<areg::SharedBuffer> messages;
            observer.log_instance_names(names);
            observer.log_instances(ids);
            observer.log_thread_names(names);
            observer.log_threads(threads);
            observer.log_priority_names(names);
            observer.log_instance_infos(infos);
            observer.log_inst_scopes(scopes, cookie);
            observer.log_messages(messages);
            observer.log_inst_messages(messages, cookie);
            observer.log_scope_messages(messages, scope.lsId);
            observer.log_messages(messages, cookie, scope.lsId);
            CHECK(std::find(ids.begin(), ids.end(), cookie) != ids.end());
            CHECK(messages.empty() == false);
        }

        observer.disconnect();
        CHECK(observer.is_connected() == false);
        {
            std::lock_guard<std::mutex> guard(_seen.lock);
            CHECK(_seen.serviceDisconnectedCalled);
        }

        observer.release();
        CHECK(observer.is_initialized() == false);
        return collector.finish();
    }

    //////////////////////////////////////////////////////////////////////////
    // The log source role
    //////////////////////////////////////////////////////////////////////////

    //!< Writes one line with the scope of the source.
    void write_line(const std::string & kind, const std::string & text)
    {
        LOG_SCOPE(log_observer_test_source_Scope, write);
        [[maybe_unused]] const char * message{ text.c_str() };
        if (kind == "dbg")
        {
            LOG_DBG("%s", message);
        }
        else if (kind == "err")
        {
            LOG_ERR("%s", message);
        }
    }

    //!< Connects to the collector, then writes a line for each command of the standard input.
    int run_source(const char * config)
    {
        areg::Application::setup(true, false, false, true, false, config);
        const auto until{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
        while ((areg::cookie() == areg::COOKIE_UNKNOWN) && (std::chrono::steady_clock::now() < until))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        int result{ areg::cookie() == areg::COOKIE_UNKNOWN ? 2 : 0 };
        std::string line;
        while ((result == 0) && std::getline(std::cin, line))
        {
            if (line == "exit")
                break;

            const std::size_t space{ line.find(' ') };
            write_line(line.substr(0u, space), space == std::string::npos ? std::string() : line.substr(space + 1u));
        }

        areg::Application::release();
        return result;
    }
}

int main(int argc, char * argv[])
{
#if !defined(_WIN32)
    ::signal(SIGPIPE, SIG_IGN);
#endif  // !defined(_WIN32)

    if ((argc >= 3) && (std::strcmp(argv[1], "source") == 0))
        return run_source(argv[2]);

    if (argc < 4)
    {
        std::printf("usage: areg-log-observer-test <case> <bin-dir> <logcollector-path>\n");
        return 2;
    }

    static_cast<void>(areg::socket_initialize());
    const std::string name{ argv[1] };
    if (prepare(name, argv[2], argv[3]) == false)
    {
        std::printf("FAIL: no free port or no configuration\n");
        return 1;
    }

    bool result{ false };
    if (name == "calls-before-initialize")
        result = case_calls_before_initialize();
    else if (name == "one-framework")
        result = case_one_framework();
    else if (name == "calls-after-release")
        result = case_calls_after_release();
    else if (name == "offline-lifecycle")
        result = case_offline_lifecycle();
    else if (name == "connect-without-collector")
        result = case_connect_without_collector();
    else if (name == "session-messages")
        result = case_session_messages();
    else if (name == "source-leaves")
        result = case_source_leaves();
    else if (name == "child-path-with-shell-syntax")
        result = case_child_path_with_shell_syntax();
    else if (name == "scope-priority")
        result = case_scope_priority();
    else if (name == "source-state")
        result = case_source_state();
    else if (name == "source-paused-drops-produced-logs")
        result = case_source_paused_drops_produced_logs();
    else if (name == "config-save-restore")
        result = case_config_save_restore();
    else if (name == "pause-resume")
        result = case_pause_resume();
    else if (name == "local-logs-and-database")
        result = case_local_logs_and_database();
    else if (name == "reconnect-and-stop")
        result = case_reconnect_and_stop();
    else if (name == "collector-restart")
        result = case_collector_restart();
    else if (name == "paused-observer-reconnects-paused")
        result = case_paused_observer_reconnects_paused();
    else if (name == "release-while-connected")
        result = case_release_while_connected();
    else if (name == "pause-before-the-connection")
        result = case_pause_before_the_connection();
    else if (name == "raw-message-callback")
        result = case_raw_message_callback();
    else if (name == "invalid-requests")
        result = case_invalid_requests();
    else if (name == "path-getters-fill-buffer")
        result = case_path_getters_fill_buffer();
    else if (name == "config-address-fills-buffer")
        result = case_config_address_fills_buffer();
    else if (name == "observer-base-class")
        result = case_observer_base_class();
    else
        std::printf("FAIL: unknown case '%s'\n", name.c_str());

    for (int i = 0; (i < 4) && log_observer_is_initialized(); ++i)
    {
        log_observer_disconnect_logger();
        log_observer_release();
    }

    cleanup();
    std::printf("%s: %s\n", name.c_str(), ((result && (_failures == 0)) ? "ok" : "FAIL"));
    std::fflush(stdout);
    return ((result && (_failures == 0)) ? 0 : 1);
}
