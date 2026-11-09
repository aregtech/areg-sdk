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

#include <atomic>
#include <chrono>
#include <memory>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class ComponentThread;
    class ServiceManager;
    class Thread;
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

    //!< The selection of no model: the name of the request is not registered.
    static constexpr uint32_t   MODEL_NONE  { 0u };
    //!< The selection of all registered models.
    static constexpr uint32_t   MODEL_ALL   { 0xFFFF'FFFFu };

    /**
     * \brief   A load or unload request whose caller waits for its completion. The controller
     *          completes each request exactly once.
     **/
    class Request
    {
    public:
        //!< Creates a request owned by the caller and by the controller; nullptr if it cannot be allocated.
        static Request * create( bool waitComponents = false );

        //!< Waits for the completion, releases the share of the caller and returns the result.
        bool wait();

        //!< Completes the request with the result and releases the share of the controller.
        void complete( bool result );

        //!< Releases one share; the last one deletes the request.
        void release();

        //!< Returns true if a load completes after the components started, false after the threads started.
        inline bool wait_components() const noexcept;

    private:
        friend class SystemController;

        SyncEvent   mDone;          //!< Signaled when the request is completed.
        bool        mResult;        //!< The result of the request.
        const bool  mWaitComponents;//!< True if a load waits for the components, false for the threads.
        bool        mLoad;          //!< True if the controller waits for a load, false for an unload.
        std::atomic<uint32_t>   mShares;    //!< The number of owners.
        uint32_t    mModels;        //!< The selection of the models the controller waits for.
        uint32_t    mLastModel;     //!< The last model the request of all models waits for; MODEL_ALL for one model.
        Request *   mNext;          //!< The next request whose caller waits.

    private:
        explicit Request( bool waitComponents );
        ~Request() = default;
        AREG_NOCOPY_NOMOVE( Request );
    };

private:
    //!< A component thread owned by the controller. The name is read from the thread object.
    struct ThreadRecord
    {
        ComponentThread *   mThread;        //!< The thread object; the controller holds the owner share.
        Clock::time_point   mDeadline;      //!< The deadline of the current transition, if armed.
        uint32_t            mTimeout;       //!< The lifecycle timeout of the thread.
        uint32_t            mModel;         //!< The number of the model of the thread.
        uint32_t            mPosition;      //!< The position of the thread in its model.
        areg::ThreadState   mState;         //!< The lifecycle state; Abandoned until the thread returns.
        areg::FailurePolicy mPolicy;        //!< The failure policy of the thread.
        bool                mHasDeadline;   //!< True if a deadline is armed.
        bool                mDrain;         //!< True while a stopping thread may still drain its queue.
        bool                mRestart;       //!< True while the thread stops to be replaced.
        bool                mDeferred;      //!< True if a new instance waits for a hung instance to return.
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
     * \brief   Loads the selected models.
     * \param   modelId     The selection from find_model_id().
     * \param   request     The request to complete, or nullptr if the caller does not wait. It completes
     *                      once the threads started, or once the components started if it waits for them.
     **/
    void load_model( uint32_t modelId, Request * request );

    /**
     * \brief   Unloads the selected models.
     * \param   modelId     The selection from find_model_id().
     * \param   request     The request to complete, or nullptr if the caller does not wait.
     **/
    void unload_model( uint32_t modelId, Request * request );

    /**
     * \brief   A component thread reports that its components started.
     * \param   instance    The address of the thread object.
     **/
    void thread_running( uint64_t instance );

    /**
     * \brief   A component thread reports that it exited.
     * \param   instance    The address of the thread object.
     **/
    void thread_stopped( uint64_t instance );

    /**
     * \brief   The watchdog of a component thread or of one of its workers expired.
     * \param   instance    The address of the component thread object.
     * \param   isWorker    True if a worker of the thread took too long, false if the thread did.
     **/
    void watchdog_expired( uint64_t instance, bool isWorker );

    /**
     * \brief   The deadline guard expired: applies every deadline that is due.
     **/
    void deadline_expired();

    /**
     * \brief   Creates the deadline guard. Called before the service manager thread starts.
     * \return  True if the guard can be armed.
     **/
    [[nodiscard]]
    bool start();

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
     * \brief   Returns the number of the model of the name, MODEL_ALL if the name is empty,
     *          MODEL_NONE if no model has the name.
     **/
    [[nodiscard]]
    static uint32_t find_model_id( const String & modelName );

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
    void _start_load( uint32_t modelId );
    //!< Asks the threads of the model to stop; the model is unloading until they all stopped.
    void _start_unload( uint32_t modelId );
    //!< Returns true if the entry at the position is valid and no thread of its name runs.
    bool _is_startable( uint32_t modelId, uint32_t position ) const;
    //!< Creates and starts the component thread at the position of the model. Returns false if it did not start.
    bool _start_thread( uint32_t modelId, uint32_t position );
    //!< Starts a new instance of the thread if its model is loaded and no instance of it is live.
    void _start_instance( uint32_t modelId, uint32_t position );
    //!< Starts a new instance of the abandoned thread at the index, or defers it while two instances hang.
    void _start_replacement( uint32_t index );
    //!< Clears the mark of the deferred new instance of the thread. Returns true if there was one.
    bool _take_deferred( uint32_t modelId, uint32_t position );
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
    //!< Queues the request until the selected models reach the requested state.
    void _add_pending( Request & request, bool load, uint32_t modelId );
    //!< Completes the requests whose models reached the requested state.
    void _complete_pending();
    //!< Starts the orderly stop that ends the process; abandons the culprit index if not negative.
    void _exit_process( int32_t culprit, const char * reason );
    //!< Stops logging and ends the process with the failure exit code.
    [[noreturn]] void _finish_exit();
    //!< Arms the deadline guard at the earliest deadline, or stops it if there is none.
    void _arm_deadline();
    //!< Makes room for one more record. Returns false if the room cannot be allocated.
    bool _secure_room();
    //!< Removes the record at the index.
    void _remove_record( uint32_t index );

    //!< Returns the index of the record of the thread object, live or abandoned, or -1.
    int32_t _find_record( uint64_t instance ) const;
    //!< Returns the index of the live thread with the name, or -1.
    int32_t _find_live( const String & threadName ) const;
    //!< Returns the index of the live thread at the position of the model, or -1.
    int32_t _find_live( uint32_t modelId, uint32_t position ) const;
    //!< Returns the number of abandoned instances of the thread that still run.
    uint32_t _count_abandoned( uint32_t modelId, uint32_t position ) const;
    //!< Returns the number of live threads of the model.
    uint32_t _count_live( uint32_t modelId ) const;

    //!< Returns a registered worker of the thread with one more share, or nullptr if none is left.
    static Thread * _hold_next_worker( const ComponentThread & owner );
    //!< Returns the model with the number, or nullptr. Call it with the loader lock taken.
    static areg::Model * _find_model( uint32_t modelId );
    //!< Returns the selected model after the previous one, or MODEL_NONE; 0 starts the walk.
    static uint32_t _next_model( uint32_t selection, uint32_t previous );
    //!< Returns the number of the last registered model.
    static uint32_t _last_model();
    //!< Returns the state of the model.
    static areg::ModelState _model_state( uint32_t modelId );
    //!< Sets the state of the model.
    static void _set_model_state( uint32_t modelId, areg::ModelState newState );
    //!< Returns true if a model with the number is registered.
    static bool _model_exists( uint32_t modelId );
    //!< Marks the model to unload as soon as its load ends.
    static void _set_unload_after_load( uint32_t modelId, bool unload );
    //!< Returns true if the model is loaded and marked to unload.
    static bool _is_unload_due( uint32_t modelId );
    //!< Returns the number of threads of the model.
    static uint32_t _thread_count( uint32_t modelId );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ThreadRecord *              mRecords;       //!< The live threads and the abandoned instances, kept until they return.
    uint32_t                    mRecordCount;   //!< The number of records.
    uint32_t                    mRecordCapacity;//!< The number of records that fit the allocated block.
    Request *                   mPending;       //!< The first request whose caller waits.
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
