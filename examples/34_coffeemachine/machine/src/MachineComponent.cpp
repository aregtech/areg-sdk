#include "machine/src/MachineComponent.hpp"

#include <iostream>

namespace
{
    constexpr uint32_t WATER_CAPACITY{ 2000 };
    constexpr uint32_t BEANS_CAPACITY{ 500 };
    constexpr uint32_t MILK_CAPACITY{ 1000 };

    const Recipe RecipeTable[]
    {
        { "Espresso",   120,  40,  9,   0 },
        { "Cappuccino", 180,  60,  9,  80 },
        { "Latte",      200,  60,  9, 150 },
    };
}

MachineComponent::MachineComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component(entry, owner)
    , CoffeeMachineServiceProviderBase(static_cast<areg::Component &>(self()))
    , CoffeeMachineActionHandler()
    , CoffeeMachineFSM::FinalObserver()
    , mFsm(static_cast<CoffeeMachineActionHandler &>(self()))
    , mCredit(0)
    , mWater(WATER_CAPACITY)
    , mBeans(BEANS_CAPACITY)
    , mMilk(MILK_CAPACITY)
    , mWarnedLowWater(false)
    , mWarnedEmptyWater(false)
    , mWarnedLowBeans(false)
    , mWarnedEmptyBeans(false)
    , mWarnedLowMilk(false)
    , mWarnedEmptyMilk(false)
    , mOrderAccepted(false)
    , mOrderReason()
    , mLostIngredients()
{
}

void MachineComponent::startup_component(areg::ComponentThread & comThread)
{
    areg::Component::startup_component(comThread);
    mFsm.set_final_observer(this);
    mFsm.init_fsm(&comThread);

    set_credit(mCredit);
    set_stage("idle");
}

void MachineComponent::shutdown_component(areg::ComponentThread & comThread)
{
    mFsm.release_fsm();
    areg::Component::shutdown_component(comThread);
}

void MachineComponent::request_insert_coin(uint32_t coin)
{
    if ((coin == 5) || (coin == 10) || (coin == 20) || (coin == 50) || (coin == 100) || (coin == 200))
    {
        mCredit += coin;
        set_credit(mCredit);
        std::cout << "machine: credit " << mCredit << std::endl;
    }
}

void MachineComponent::request_order_drink(const areg::String & drink)
{
    const Recipe * recipe = find_recipe(drink);
    if (recipe == nullptr)
    {
        response_order_drink(false, "unknown drink");
        return;
    }

    mOrderAccepted = false;
    mOrderReason   = "";
    const bool needsMilk{ recipe->milk > 0 };

    mFsm.order(recipe->name, recipe->price, recipe->water, recipe->beans, recipe->milk,
               needsMilk, mCredit, mWater, mBeans, mMilk);

    response_order_drink(mOrderAccepted, mOrderReason);
}

void MachineComponent::request_pause_making(void)
{
    mFsm.pause();
}

void MachineComponent::request_resume_making(void)
{
    mFsm.resume();
}

void MachineComponent::request_cancel_order(void)
{
    mFsm.cancel();
}

void MachineComponent::request_refill_tank(const areg::String & tank)
{
    bool ok{ true };
    if (tank == "Water")
    {
        mWater = WATER_CAPACITY;
        mWarnedLowWater = false;
        mWarnedEmptyWater = false;
    }
    else if (tank == "Beans")
    {
        mBeans = BEANS_CAPACITY;
        mWarnedLowBeans = false;
        mWarnedEmptyBeans = false;
    }
    else if (tank == "Milk")
    {
        mMilk = MILK_CAPACITY;
        mWarnedLowMilk = false;
        mWarnedEmptyMilk = false;
    }
    else
    {
        ok = false;
    }

    response_refill_tank(ok, tank);
}

void MachineComponent::action_on_refused(const areg::String & reason)
{
    mOrderAccepted = false;
    mOrderReason   = reason;
}

void MachineComponent::action_on_accepted(const areg::String & name, uint32_t price)
{
    mOrderAccepted = true;
    mOrderReason   = "";
    mLostIngredients = "";
    set_stage("accepted");
}

void MachineComponent::action_on_grinding(void)
{
    set_stage("grinding");
    const uint32_t need{ mFsm.beans_needed() };
    mBeans = (mBeans >= need) ? (mBeans - need) : 0;
    note_lost("beans");
    check_warning("Beans", mBeans, BEANS_CAPACITY, mWarnedLowBeans, mWarnedEmptyBeans);
}

void MachineComponent::action_on_heating(void)
{
    set_stage("heating");
    const uint32_t need{ mFsm.water_needed() };
    mWater = (mWater >= need) ? (mWater - need) : 0;
    note_lost("water");
    check_warning("Water", mWater, WATER_CAPACITY, mWarnedLowWater, mWarnedEmptyWater);
}

void MachineComponent::action_on_brewing(void)
{
    set_stage("brewing");
}

void MachineComponent::action_on_frothing(void)
{
    set_stage("frothing");
    const uint32_t need{ mFsm.milk_needed() };
    mMilk = (mMilk >= need) ? (mMilk - need) : 0;
    note_lost("milk");
    check_warning("Milk", mMilk, MILK_CAPACITY, mWarnedLowMilk, mWarnedEmptyMilk);
}

void MachineComponent::action_on_dispensing(void)
{
    set_stage("dispensing");
}

void MachineComponent::action_on_finished(const areg::String & name, uint32_t price)
{
    mCredit = (mCredit >= price) ? (mCredit - price) : 0;
    set_credit(mCredit);
    set_stage("idle");
    mLostIngredients = "";
    broadcast_drink_ready(name);
}

void MachineComponent::action_on_cancelled(void)
{
    set_stage("idle");
    broadcast_order_cancelled(mCredit, mLostIngredients);
    mLostIngredients = "";
}

void MachineComponent::action_on_paused(void)
{
    set_stage("paused");
}

void MachineComponent::action_on_resumed(void)
{
    // The next stage's own EntryList reports which stage was resumed.
}

void MachineComponent::on_fsm_final(CoffeeMachineFSM & /* machine */, const char * const /* finalState */)
{
}

void MachineComponent::check_warning(const char * ingredient, uint32_t level, uint32_t capacity, bool & warnedLow, bool & warnedEmpty)
{
    if (level == 0)
    {
        if (!warnedEmpty)
        {
            warnedEmpty = true;
            broadcast_ingredient_warning(ingredient, "empty");
        }
    }
    else if (((level * 5) < capacity) && !warnedLow)   // below 20%, first crossing
    {
        warnedLow = true;
        broadcast_ingredient_warning(ingredient, "low");
    }
}

const Recipe * MachineComponent::find_recipe(const areg::String & drink) const
{
    for (const Recipe & recipe : RecipeTable)
    {
        if (recipe.name == drink)
        {
            return &recipe;
        }
    }

    return nullptr;
}

void MachineComponent::note_lost(const char * ingredient)
{
    if (mLostIngredients.is_empty() == false)
    {
        mLostIngredients += ", ";
    }

    mLostIngredients += ingredient;
}
