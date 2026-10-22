#ifndef AREG_COMPONENT_PRIVATE_SYSTEMCONTROLLER_HPP
#define AREG_COMPONENT_PRIVATE_SYSTEMCONTROLLER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/SystemController.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the owner of the models and of the component threads.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/Model.hpp"
#include "areg/component/ServiceDefs.hpp"

#include <chrono>
#include <memory>
#include <vector>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class ComponentThread;
    class ServiceManager;
    class Watchdog;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SystemController class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The only owner of the models and of the component threads. It loads and unloads the
 *          models, starts and stops their threads, keeps their deadlines and applies their
 *          failure policies. It runs on the service manager thread and never waits for user code:
 *          the requests and the reports of the threads reach it as events.
 **/
class SystemController
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
public:
    using Clock     = std::chrono::steady_clock;

    /**
     * \brief   A load or unload request whose caller waits for its completion. The controller
     *          completes each request exactly once.
     **/
    class Request
    {
    public:
        explicit Request( bool waitComponents = false );
        ~Request() = default;

        //!< Waits for the completion and returns the result.
        bool wait();

        //!< Completes the request with the result.
        void complete( bool result );

        //!< Returns true if a load completes after the components started, false after the threads started.
        inline bool wait_components() const noexcept;

    private:
        SyncEvent   mDone;          //!< Signaled when the request is completed.
        bool        mResult;        //!< The result of the request.
        const bool  mWaitComponents;//!< True if a load waits for the components, false for the threads.

    private:
        AREG_NOCOPY_NOMOVE( Request );
    };

private:
    //!< A component thread owned by the controller.
    struct ThreadRecord
    {
        String              mName;          //!< The name of the thread.
        String              mModel;         //!< The name of the model of the thread.
        ComponentThread *   mThread;        //!< The thread object; the controller holds the owner share.
        areg::ThreadState   mState;         //!< The lifecycle state.
        Clock::time_point   mDeadline;      //!< The deadline of the current transition, if armed.
        bool                mHasDeadline;   //!< True if a deadline is armed.
        bool                mDrain;         //!< True while a stopping thread may still drain its queue.
        bool                mRestart;       //!< True while the thread stops to be replaced.
        uint32_t            mTimeout;       //!< The lifecycle timeout of the thread.
        areg::FailurePolicy mPolicy;        //!< The failure policy of the thread.
    };

    //!< A new instance of a thread that starts when one of its hung instances returns.
    struct Deferred
    {
        String              mThread;        //!< The name of the thread.
        String              mModel;         //!< The name of the model of the thread.
    };

    //!< A request of a caller that waits until its models reach the requested state.
    struct Pending
    {
        std::shared_ptr<Request> mRequest;  //!< The request to complete.
        bool                mLoad;          //!< True if waits for a load, false for an unload.
        std::vector<String> mModels;        //!< The models the request waits for.
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SystemController();

    ~SystemController();

//////////////////////////////////////////////////////////////////////////
// Operations, called on the service manager thread
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Loads the model, or all models if the name is empty.
     * \param   modelName   The name of the model.
     * \param   request     The request to complete, or nullptr if the caller does not wait. It completes
     *                      once the threads started, or once the components started if it waits for them.
     **/
    void load_model( const String & modelName, const std::shared_ptr<Request> & request );

    /**
     * \brief   Unloads the model, or all models if the name is empty.
     * \param   modelName   The name of the model.
     * \param   request     The request to complete, or nullptr if the caller does not wait.
     **/
    void unload_model( const String & modelName, const std::shared_ptr<Request> & request );

    /**
     * \brief   A component thread reports that its components started.
     * \param   threadName  The name of the thread.
     * \param   instance    The address of the thread object.
     **/
    void thread_running( const String & threadName, uint64_t instance );

    /**
     * \brief   A component thread reports that it exited.
     * \param   threadName  The name of the thread.
     * \param   instance    The address of the thread object.
     **/
    void thread_stopped( const String & threadName, uint64_t instance );

    /**
     * \brief   The watchdog of a component thread or of one of its workers expired.
     * \param   threadName  The name of the component thread.
     * \param   instance    The address of the component thread object.
     * \param   guardedName The name of the thread whose event took too long.
     **/
    void watchdog_expired( const String & threadName, uint64_t instance, const String & guardedName );

    /**
     * \brief   The deadline guard expired: applies every deadline that is due.
     **/
    void deadline_expired();

    /**
     * \brief   The service manager stops: every pending request fails.
     **/
    void stop();

    /**
     * \brief   Stores the user state and announces a change to every live component thread.
     * \param   userState   The new user state.
     **/
    void set_user_state( uint32_t userState );

//////////////////////////////////////////////////////////////////////////
// Operations, called on any thread
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the state of the component thread of the name.
     **/
    [[nodiscard]]
    areg::ThreadState thread_state( const String & threadName ) const;

    /**
     * \brief   Returns true if the calling thread is an abandoned component thread or a worker
     *          of one: nothing it registers or unregisters may reach the service registry.
     **/
    [[nodiscard]]
    static bool is_caller_abandoned() noexcept;

    /**
     * \brief   Returns true if the calling thread belongs to the model: a component thread of it,
     *          a worker of one, or any component thread if the name is empty.
     **/
    [[nodiscard]]
    static bool is_caller_in_model( const String & modelName );

    /**
     * \brief   Returns the user state; 0 until the application sets one.
     **/
    [[nodiscard]]
    static uint32_t user_state() noexcept;

    /**
     * \brief   Stores the user state without announcing it. Used while no controller runs.
     **/
    static void store_user_state( uint32_t userState ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    //!< Starts the threads of the model; the model is loading until they all run.
    void _start_load( const String & modelName );
    //!< Asks the threads of the model to stop; the model is unloading until they all stopped.
    void _start_unload( const String & modelName );
    //!< Creates and starts a component thread of the model. Returns false if it did not start.
    bool _start_thread( const areg::ComponentThreadEntry & entry, const String & modelName );
    //!< Starts a new instance of the thread if its model is loaded and no instance of it is live.
    void _start_instance( const String & threadName, const String & modelName );
    //!< Starts the deferred new instance of the thread, if there is one.
    void _start_deferred( const String & threadName );
    //!< Asks the thread to stop after its queue is drained, and arms the drain deadline.
    void _stop_thread( ThreadRecord & record );
    //!< Asks the thread at the index to stop at once, to be replaced by a new instance.
    void _restart_thread( uint32_t index );
    //!< Abandons the thread at the index: unregisters it and everything it owns, keeps the object.
    void _abandon_thread( uint32_t index, areg::DisconnectReason reason );
    //!< Joins the thread that reported its exit and drops the owner share.
    void _free_thread( ComponentThread * thread, uint32_t timeout );
    //!< Applies the failure policy of the thread at the index that missed its deadline.
    void _apply_policy( uint32_t index, const char * what );
    //!< Moves the models whose threads all run or all stopped to their next state.
    void _check_models();
    //!< Completes the requests whose models reached the requested state.
    void _complete_pending();
    //!< Starts the orderly stop that ends the process; abandons the culprit index if not negative.
    void _exit_process( int32_t culprit, const char * reason );
    //!< Stops logging and ends the process with the failure exit code.
    [[noreturn]] void _finish_exit();
    //!< Arms the deadline guard at the earliest deadline, or stops it if there is none.
    void _arm_deadline();

    //!< Returns the index of the live thread with the name and object address, or -1.
    int32_t _find_live( const String & threadName, uint64_t instance ) const;
    //!< Returns the index of the live thread with the name, or -1.
    int32_t _find_live( const String & threadName ) const;
    //!< Returns the number of abandoned instances of the thread that still run.
    uint32_t _count_abandoned( const String & threadName ) const;
    //!< Returns the number of live threads of the model.
    uint32_t _count_live( const String & modelName ) const;

    //!< Returns the state of the model.
    static areg::ModelState _model_state( const String & modelName );
    //!< Sets the state of the model.
    static void _set_model_state( const String & modelName, areg::ModelState newState );
    //!< Returns true if a model of the name is registered.
    static bool _model_exists( const String & modelName );
    //!< Fills the names of all registered models.
    static void _model_names( std::vector<String> & names );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    std::vector<ThreadRecord>   mThreads;       //!< The live threads.
    std::vector<ThreadRecord>   mAbandoned;     //!< The abandoned instances, kept until they return.
    std::vector<Pending>        mPending;       //!< The requests whose callers wait.
    std::vector<String>         mFailedLoads;   //!< The models whose load failed and are being undone.
    std::vector<String>         mUnloadAfterLoad;//!< The models to unload as soon as their load ends.
    std::vector<Deferred>       mDeferred;      //!< The new instances that wait for a hung instance to return.
    std::unique_ptr<Watchdog>   mGuard;         //!< The deadline guard on the watchdog manager.
    bool                        mExiting;       //!< True once the failure policy Exit started.
    Clock::time_point           mExitDeadline;  //!< The deadline of the orderly stop before the exit.
    mutable SpinLock            mLock;          //!< Guards the thread records for the readers.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SystemController );
};

//////////////////////////////////////////////////////////////////////////
// SystemController::Request class inline methods
//////////////////////////////////////////////////////////////////////////

inline bool SystemController::Request::wait_components() const noexcept
{
    return mWaitComponents;
}

} // namespace areg

#endif  // AREG_COMPONENT_PRIVATE_SYSTEMCONTROLLER_HPP
