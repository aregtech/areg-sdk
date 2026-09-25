#pragma once

/************************************************************************
 * \file        user/src/CoffeeMachineServiceConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Coffee machine driven by a generated state machine: the simulated user component.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/base/String.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "examples/34_coffeemachine/services/CoffeeMachineServiceConsumerBase.hpp"

//! Ends the application with this exit code, in storage that outlives
//! the components. Defined next to main().
void quit_with(int code);

//! True once quit_with() has run. Defined next to main().
bool is_quitting();

class CoffeeMachineServiceConsumer final : public    areg::Component
                                         , protected CoffeeMachineServiceConsumerBase
                                         , private   areg::TimerConsumer
{
public:
    //! The steps of the scenario, in the order design.json lists them.
    enum class Step : uint32_t
    {
        Start,
        OrderNoMoney,
        InsertForCappuccino,
        OrderCappuccino,
        AwaitGrinding,
        PauseMidway,
        WaitBeforeResume,
        ResumeAfterWait,
        AwaitFrothing,
        AwaitCappuccinoDone,
        CheckCreditAfterCappuccino,
        InsertForLatte,
        OrderLatte,
        AwaitLatteDone,
        RefillMilk,
        OrderLatteAfterRefill,
        AwaitLatteAfterRefillDone,
        InsertForEspresso,
        OrderEspresso,
        AwaitEspressoDone,
        InsertForCancel,
        OrderThenCancel,
        WaitBeforeCancel,
        CancelDrink,
        Done
    };

    CoffeeMachineServiceConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

protected:
    void startup_component(areg::ComponentThread & thread) final;
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final;
    void process_timer(areg::Timer & timer) final;
    void response_order_drink( bool accepted, const areg::String & reason ) final;
    void response_refill_tank( bool success ) final;
    void request_insert_coin_failed(areg::ResultType reason) final;
    void request_order_drink_failed(areg::ResultType reason) final;
    void request_pause_drink_failed(areg::ResultType reason) final;
    void request_resume_drink_failed(areg::ResultType reason) final;
    void request_cancel_drink_failed(areg::ResultType reason) final;
    void request_refill_tank_failed(areg::ResultType reason) final;
    void broadcast_ingredient_warning( CoffeeMachineService::IngredientType tank, CoffeeMachineService::WarningLevel level ) final;
    void on_credit_update(uint32_t Credit, areg::DataState state) final;
    void on_stage_update(CoffeeMachineService::MachineStage Stage, areg::DataState state) final;
    void on_paused_update(bool Paused, areg::DataState state) final;

private:
    uint32_t mLatteCount{ 0 };
    uint32_t mLatteAttempts{ 0 };
    bool mMilkWarningSeen{ false };
    bool mSawFrothingThisOrder{ false };
    CoffeeMachineService::MachineStage mLastStage{ CoffeeMachineService::MachineStage::Idle };
    uint32_t mLastCredit{ 0 };
    uint32_t mCreditBeforeCancel{ 0 };
    inline CoffeeMachineServiceConsumer & self()
    {   return (*this); }

    //! The worksheet section holding the check of the step the
    //! scenario is on.
    const char * step_slot();
    //! What the step the scenario is on sent, and what it waits for.
    const char * step_detail();
    //! Ends the scenario when no step has advanced, naming what the
    //! step waits for and every message an earlier step discarded.
    void stalled();
    //! Remembers a message that arrived on a step with no check
    //! for it. The stall report names them.
    void dropped(const char * what);
    //! How many discarded messages the stall report names.
    static constexpr uint32_t cDroppedMost{ 4 };
    const char * mDroppedWhat[cDroppedMost]{};   //!< What each was.
    const char * mDroppedStep[cDroppedMost]{};   //!< Where each was.
    uint32_t     mDroppedKept{ 0 };    //!< How many are remembered.
    uint32_t     mDroppedCount{ 0 };   //!< How many there were.

    //! Ends the scenario as a failure, naming what went wrong and the
    //! step the scenario was on.
    void fail(const char * why);
    //! Starts the deadline timer for this many seconds. 0 stops it and
    //! waits for ever.
    void arm_deadline(uint32_t seconds);
    areg::Timer  mDeadline;   //!< Ends the run when no provider is there.
    bool         mConnected{ false };   //!< True once the service has connected.

    //! Seconds to wait for the provider to appear. 0 waits for ever.
    static constexpr uint32_t cConnectSeconds{ 10 };
    //! Seconds to wait for it to come back. 0 waits for ever.
    static constexpr uint32_t cReconnectSeconds{ 10 };

    //! Restarts the stall watchdog. Call it wherever the scenario advances.
    void progressed()
    {   mIdleTicks = 0; }

    areg::Timer  mPace;   //!< Spaces the requests of the scenario.

    //! Ticks of no progress that end the run, one tick a second.
    //! 0 leaves the watchdog off.
    static constexpr uint32_t cStallTicks{ 30 };
    uint32_t                  mIdleTicks{ 0 };

    //! Ends the current step when a check body is left, by falling off
    //! its end, by return, or by break. stay(), go_to() and fail() all
    //! take effect in complete(), so every path reaches it. A body that
    //! calls complete() itself ends the step there, and leaving it does
    //! not end a second one.
    struct StepEnd
    {
        explicit StepEnd(CoffeeMachineServiceConsumer & owner)
            : mOwner(owner) { mOwner.mEnding = true; }
        ~StepEnd(void)
        {
            mOwner.mRan = true;
            if (mOwner.mEnding)
            {
                mOwner.complete();
            }
        }
        StepEnd(void) = delete;
        AREG_NOCOPY_NOMOVE(StepEnd);

    private:
        CoffeeMachineServiceConsumer & mOwner;
    };

    //! Begins a step: sends its request or starts its wait. A step that
    //! waits for nothing ends at once.
    void begin(Step step);
    //! Ends the current step and begins the next, unless its check failed
    //! the run, called stay() or called go_to(). Calling it more than
    //! once for one check ends one step.
    void complete();
    //! Keeps the current step for the next answer, broadcast or update.
    void stay()
    {   mHeld = true; }

    //! Makes this the next step instead of the one listed after the current.
    void go_to(Step step)
    {   mNext = step; mJumped = true; }

    Step  mStep{ Step::Start };   //!< The step the scenario is on.
    Step  mNext{ Step::Start };   //!< The step go_to() chose.
    bool  mJumped{ false };       //!< True once go_to() chose the next step.
    bool  mHeld{ false };         //!< True once stay() kept the step.
    bool  mRan{ false };          //!< True once a check of this step ran.
    bool  mEnding{ false };       //!< True until the running check ends its step.

    areg::Timer  mHold;   //!< Ends a step that waits for a time.

    CoffeeMachineServiceConsumer() = delete;
    AREG_NOCOPY_NOMOVE(CoffeeMachineServiceConsumer);
};

