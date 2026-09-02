/**
 * \file    main.cpp
 * \brief   A worker thread doing the slow part of a request, a custom event in
 *          each direction, a watchdog on the component thread, and logging.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/component/WorkerThreadConsumer.hpp"
#include "areg/logging/areg_log.h"

#include "src/services/ScanServiceProviderBase.hpp"
#include "src/services/ScanServiceConsumerBase.hpp"

DEF_LOG_SCOPE(scan_Scanner, request_scan);
DEF_LOG_SCOPE(scan_ScanWorker, process_event);

//! The work to do, sent to the worker thread. It carries the session the
//! provider was given when it let the request go, so the answer can find its way
//! back to the client that asked.
class ScanRequestData
{
public:
    ScanRequestData() = default;
    ScanRequestData(uint32_t depth, areg::SessionID session)
        : mDepth(depth), mSession(session) {}
    ScanRequestData(const ScanRequestData & src) = default;
    ScanRequestData & operator = (const ScanRequestData & src) = default;

    inline uint32_t depth() const { return mDepth; }
    inline areg::SessionID session() const { return mSession; }

private:
    uint32_t            mDepth  { 0 };
    areg::SessionID     mSession{ 0 };
};

//! The answer, sent back to the component thread, with the session it answers.
class ScanResultData
{
public:
    ScanResultData() = default;
    ScanResultData(uint32_t found, areg::SessionID session)
        : mFound(found), mSession(session) {}
    ScanResultData(const ScanResultData & src) = default;
    ScanResultData & operator = (const ScanResultData & src) = default;

    inline uint32_t found() const { return mFound; }
    inline areg::SessionID session() const { return mSession; }

private:
    uint32_t            mFound  { 0 };
    areg::SessionID     mSession{ 0 };
};

// One macro declares the event class and the consumer base class. In a project of
// more than one file this pair of lines goes into a header both sides include.
AREG_DECLARE_EVENT(ScanRequestData, ScanRequestEvent, ScanRequestEventConsumer);
AREG_DECLARE_EVENT(ScanResultData,  ScanResultEvent,  ScanResultEventConsumer);

//! Runs on the worker thread. Nothing here blocks the component's dispatcher.
class ScanWorker final  : public    areg::WorkerThreadConsumer
                        , private   ScanRequestEventConsumer
{
public:
    explicit ScanWorker(const areg::String & consumerName)
        : areg::WorkerThreadConsumer(consumerName)
    { }

protected:
    void register_event_consumers(areg::WorkerThread & workThread,
                                  areg::ComponentThread & /*masterThread*/) final
    {
        ScanRequestEvent::add_listener(static_cast<ScanRequestEventConsumer &>(*this),
                                       static_cast<areg::DispatcherThread &>(workThread));
    }

    void unregister_event_consumers(areg::WorkerThread & workThread) final
    {
        ScanRequestEvent::remove_listener(static_cast<ScanRequestEventConsumer &>(*this),
                                          static_cast<areg::DispatcherThread &>(workThread));
    }

    void process_event(const ScanRequestData & data) final
    {
        LOG_SCOPE(scan_ScanWorker, process_event);
        uint32_t found{ 0 };
        for (uint32_t step = 1; step <= data.depth(); ++step)
        {
            found += step * step;
        }
        LOG_INFO("scan of depth %u found %u", data.depth(), found);
        std::cout << "worker: scanned depth " << data.depth() << std::endl;
        ScanResultEvent::send_event(ScanResultData(found, data.session()));
    }
};

//! Provides the service, owns the worker thread, and answers when the work is done.
class Scanner final : public    areg::Component
                    , protected ScanServiceProviderBase
                    , private   ScanResultEventConsumer
{
public:
    Scanner(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , ScanServiceProviderBase(static_cast<areg::Component &>(self()))
        // The model stores the consumer name qualified by the role name, so take
        // it from the entry rather than repeating the literal.
        , mWorker(entry.mWorkerThreads[0].mConsumerName)
    { }

    static constexpr std::string_view ThreadName  { "ScanWorkerThread" };
    static constexpr std::string_view ConsumerName{ "ScanWorkerConsumer" };

protected:
    void startup_component(areg::ComponentThread & comThread) final
    {
        areg::Component::startup_component(comThread);
        ScanResultEvent::add_listener(static_cast<ScanResultEventConsumer &>(self()),
                                      static_cast<areg::DispatcherThread &>(comThread));
    }

    void shutdown_component(areg::ComponentThread & comThread) final
    {
        ScanResultEvent::remove_listener(static_cast<ScanResultEventConsumer &>(self()),
                                         static_cast<areg::DispatcherThread &>(comThread));
        areg::Component::shutdown_component(comThread);
    }

    //! The model asks the component for the consumer it registered by name.
    areg::WorkerThreadConsumer * worker_thread_consumer(const areg::String & consumerName,
                                                        const areg::String & workerThreadName) final
    {
        return mWorker.consumer_name() == consumerName
             ? &mWorker
             : areg::Component::worker_thread_consumer(consumerName, workerThreadName);
    }

    //! Hands the work over and returns at once: the answer is sent later.
    //!
    //! unblock_current_request() releases the request so the next client is not
    //! refused with RequestBusy, and returns the session that identifies it.
    //! Without it a second client's request is rejected while this one is out.
    void request_scan(unsigned int depth) final
    {
        LOG_SCOPE(scan_Scanner, request_scan);
        const areg::SessionID session{ unblock_current_request() };
        LOG_DBG("handing depth %u to the worker thread, session %u", depth, session);
        std::cout << "scanner: asked for depth " << depth << std::endl;
        ScanRequestEvent::send_event(ScanRequestData(depth, session));
    }

    //! prepare_response() restores the session before the answer is sent. It
    //! returns false when the client has gone, and then there is nobody to answer.
    void process_event(const ScanResultData & data) final
    {
        if (prepare_response(data.session()))
        {
            response_scan(data.found());
        }
    }

private:
    ScanWorker mWorker;

    inline Scanner & self()
    {   return (*this); }
};

//! Asks once, prints the answer and ends the application.
class Client final  : public    areg::Component
                    , protected ScanServiceConsumerBase
{
public:
    Client(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , ScanServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    { }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (ScanServiceConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                request_scan(3);
            }
        }

        return result;
    }

    void response_scan(unsigned int found) final
    {
        std::cout << "client: found " << found << std::endl;
        areg::Application::signal_quit();
    }
};

constexpr char const _modelName[]{ "ScanModel" };

BEGIN_MODEL(_modelName)

    // The watchdog restarts this thread if one event takes longer than 5 seconds.
    // It does nothing unless setup() is called with startWatchdog true.
    BEGIN_REGISTER_THREAD_EX("ScannerThread", 5000)
        BEGIN_REGISTER_COMPONENT("Scanner", Scanner)
            REGISTER_IMPLEMENT_SERVICE(ScanService::ServiceName, ScanService::InterfaceVersion)
            REGISTER_WORKER_THREAD(Scanner::ThreadName.data(), Scanner::ConsumerName.data())
        END_REGISTER_COMPONENT("Scanner")
    END_REGISTER_THREAD("ScannerThread")

    BEGIN_REGISTER_THREAD("ClientThread")
        BEGIN_REGISTER_COMPONENT("Client", Client)
            REGISTER_DEPENDENCY("Scanner")
        END_REGISTER_COMPONENT("Client")
    END_REGISTER_THREAD("ClientThread")

END_MODEL(_modelName)

int main()
{
    // The fifth argument starts the watchdog service. Without it the timeout above
    // is registered and never enforced.
    areg::Application::setup(true, true, true, true, true);
    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return 0;
}
