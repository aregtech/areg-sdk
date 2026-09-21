#ifndef COFFEEMACHINE_USER_SRC_USERCOMPONENT_HPP
#define COFFEEMACHINE_USER_SRC_USERCOMPONENT_HPP

#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "examples/34_coffeemachine/services/CoffeeMachineServiceConsumerBase.hpp"

//! Set to 0 only once every scenario step has passed; read by main() after the model unloads.
extern int g_scenarioExitCode;

//! Drives the machine through the acceptance scenario and reports each step. The
//! scenario itself is a small step counter: ordinary application code, not the
//! machine's logic, so it is not a state machine.
class UserComponent final   : public    areg::Component
                            , protected CoffeeMachineServiceConsumerBase
                            , private   areg::TimerConsumer
{
private:
    enum class Step
    {
        RefuseNoMoney,
        WaitCappuccinoAccepted,
        WaitPausedStage,
        WaitResumedStage,
        WaitCappuccinoReady,
        OrderingLattes,
        WaitRefill,
        WaitFinalLatteReady,
        WaitEspressoReady,
        WaitCancelConfirmed,
        Done
    };

public:
    UserComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override;

    void response_order_drink(bool accepted, const areg::String & reason) override;
    void response_refill_tank(bool ok, const areg::String & tank) override;

    void broadcast_ingredient_warning(const areg::String & ingredient, const areg::String & level) override;
    void broadcast_drink_ready(const areg::String & drink) override;
    void broadcast_order_cancelled(uint32_t refunded, const areg::String & lost) override;

    void on_credit_update(uint32_t Credit, areg::DataState state) override;
    void on_stage_update(const areg::String & Stage, areg::DataState state) override;

    void process_timer(areg::Timer & timer) override;

private:
    void fail(const char * what);
    void ok(const char * what);

    inline UserComponent & self(void)
    {   return (*this); }

    Step            mStep;
    uint32_t        mCredit;
    areg::String    mStage;
    areg::String    mStagePrePause;
    uint32_t        mMilkLowWarnings;
    uint32_t        mCreditBeforeCancel;
    bool            mSawFrothingForEspresso;
    int             mExitCode;

    areg::Timer     mDelayTimer;
    areg::Timer     mTimeoutTimer;
};

#endif  // COFFEEMACHINE_USER_SRC_USERCOMPONENT_HPP
