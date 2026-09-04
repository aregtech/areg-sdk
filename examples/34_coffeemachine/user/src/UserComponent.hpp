#pragma once

/************************************************************************
 * \file        user/src/UserComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Simulated user: drives the coffee machine through a fixed scenario.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "examples/34_coffeemachine/services/CoffeeMachineServiceConsumerBase.hpp"

/**
 * \brief   Runs the six-step scenario from the task prompt and sets the process
 *          exit code to 0 only when every expectation held.
 **/
class UserComponent final    : public    areg::Component
                             , protected CoffeeMachineServiceConsumerBase
                             , private   areg::TimerConsumer
{
public:
    UserComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner);
    virtual ~UserComponent(void) = default;

    //!< The exit code of the scenario: 0 when every step passed.
    static int get_exit_code(void);

protected:
/************************************************************************
 * CoffeeMachineServiceConsumerBase overrides
 ************************************************************************/
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override;
    void response_order_drink(bool accepted, const areg::String & reason) override;
    void request_order_drink_failed(areg::ResultType reason) override;
    void response_cancel_order(uint32_t refunded) override;
    void on_credit_update(uint32_t Credit, areg::DataState state) override;
    void on_stage_update(CoffeeMachineService::MakingStage Stage, areg::DataState state) override;
    void broadcast_ingredient_low(const areg::String & ingredient) override;
    void broadcast_ingredient_empty(const areg::String & ingredient) override;

/************************************************************************
 * areg::TimerConsumer overrides
 ************************************************************************/
    void process_timer(areg::Timer & timer) override;

private:
    //!< The scenario step currently in progress.
    enum class Step
    {
        RefuseNoMoney,      //!< step 1: order with no credit, expect refusal
        WaitCreditForOrder, //!< step 2: waiting for the 200 cents to register
        WaitOrderAccepted,  //!< step 2: order accepted
        WaitMakingStarted,  //!< step 3: waiting for the first non-idle stage, then pausing
        WaitPaused,         //!< step 3: waiting for the Paused stage
        WaitResumeTimer,    //!< step 3: a short wait before resuming
        WaitResumed,        //!< step 3: waiting for the stage to return to what it was
        WaitFinishedFirst,  //!< step 4: waiting for the drink to finish (back to Idle)
        LatteTopUp,         //!< step 5: waiting for the top-up credit before ordering a Latte
        LatteOrdered,       //!< step 5: waiting for the order_drink response
        LatteFinishing,     //!< step 5: waiting for the Latte in progress to finish
        RefillTopUp,        //!< step 6: waiting for the top-up credit after refilling
        RefillOrdered,      //!< step 6: waiting for the final order to be accepted
        Done                //!< the scenario ended, one way or another
    };

    //!< Marks the scenario failed at the given step and quits.
    void fail(int step, const areg::String & why);
    //!< Marks the scenario passed and quits.
    void finish(void);

private:
    static int  sExitCode;

    areg::Timer mResumeTimer;      //!< Fires once, to give the pause a moment before resuming.

    Step        mStep;
    uint32_t    mCreditSeen;        //!< Last credit value reported by the provider.
    uint32_t    mWaitForCredit;      //!< The credit value mStep is waiting for.

    CoffeeMachineService::MakingStage mStagePausedIn;   //!< The stage seen just before pausing.

    bool        mMilkWarningSeen;    //!< True once the low-milk broadcast arrived.
    uint32_t    mLattesOrdered;      //!< Safety bound on the step 5 loop.

    inline UserComponent & self(void)
    {   return (*this); }
};
