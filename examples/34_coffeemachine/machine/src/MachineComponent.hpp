#ifndef COFFEEMACHINE_MACHINE_SRC_MACHINECOMPONENT_HPP
#define COFFEEMACHINE_MACHINE_SRC_MACHINECOMPONENT_HPP

#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"

#include "examples/34_coffeemachine/services/CoffeeMachineServiceProviderBase.hpp"
#include "examples/34_coffeemachine/services/CoffeeMachineActionHandler.hpp"
#include "examples/34_coffeemachine/services/CoffeeMachineFSM.hpp"

//! One recipe: price and the ingredients it needs.
struct Recipe
{
    areg::String    name;
    uint32_t        price;
    uint32_t        water;
    uint32_t        beans;
    uint32_t        milk;
};

//! The coffee machine. Its logic is the CoffeeMachine state machine; this class owns
//! the money and the tanks, and answers requests by feeding the machine a stimulus.
class MachineComponent final    : public    areg::Component
                                , protected CoffeeMachineServiceProviderBase
                                , protected CoffeeMachineActionHandler
                                , private   CoffeeMachineFSM::FinalObserver
{
public:
    MachineComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

protected:
    void startup_component(areg::ComponentThread & comThread) override;
    void shutdown_component(areg::ComponentThread & comThread) override;

    void request_insert_coin(uint32_t coin) override;
    void request_order_drink(const areg::String & drink) override;
    void request_pause_making(void) override;
    void request_resume_making(void) override;
    void request_cancel_order(void) override;
    void request_refill_tank(const areg::String & tank) override;

    void action_on_refused(const areg::String & reason) override;
    void action_on_accepted(const areg::String & name, uint32_t price) override;
    void action_on_grinding(void) override;
    void action_on_heating(void) override;
    void action_on_brewing(void) override;
    void action_on_frothing(void) override;
    void action_on_dispensing(void) override;
    void action_on_finished(const areg::String & name, uint32_t price) override;
    void action_on_cancelled(void) override;
    void action_on_paused(void) override;
    void action_on_resumed(void) override;

    void on_fsm_final(CoffeeMachineFSM & machine, const char * const finalState) override;

private:
    void check_warning(const char * ingredient, uint32_t level, uint32_t capacity, bool & warnedLow, bool & warnedEmpty);
    const Recipe * find_recipe(const areg::String & drink) const;
    void note_lost(const char * ingredient);

    inline MachineComponent & self(void)
    {   return (*this); }

    CoffeeMachineFSM    mFsm;

    uint32_t    mCredit;
    uint32_t    mWater;
    uint32_t    mBeans;
    uint32_t    mMilk;

    bool        mWarnedLowWater;
    bool        mWarnedEmptyWater;
    bool        mWarnedLowBeans;
    bool        mWarnedEmptyBeans;
    bool        mWarnedLowMilk;
    bool        mWarnedEmptyMilk;

    bool        mOrderAccepted;     //!< Set by the action handlers, read right after the trigger returns.
    areg::String mOrderReason;      //!< Ditto.
    areg::String mLostIngredients;  //!< Ingredients already consumed by the order being cancelled.
};

#endif  // COFFEEMACHINE_MACHINE_SRC_MACHINECOMPONENT_HPP
