/************************************************************************
 * \file        machine/src/MachineComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Coffee machine: accepts coins and orders, and runs the making stages.
 ************************************************************************/
#include "machine/src/MachineComponent.hpp"
#include "areg/component/ComponentThread.hpp"
#include <iostream>

namespace
{
    constexpr MachineComponent::Recipe sRecipes[] =
    {
        { CoffeeMachineService::PriceEspresso,   40u, 9u,   0u },  // Espresso
        { CoffeeMachineService::PriceCappuccino, 60u, 9u,  80u },  // Cappuccino
        { CoffeeMachineService::PriceLatte,      60u, 9u, 150u },  // Latte
    };

    constexpr uint32_t sLowPercent{ 20u };
}

MachineComponent::MachineComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component               ( entry, owner )
    , CoffeeMachineServiceProviderBase( static_cast<areg::Component &>(self()) )
    , CoffeeMachineActionHandler     ( )
    , mFsm                           ( static_cast<CoffeeMachineActionHandler &>(self()) )
    , mCredit                        ( 0u )
    , mWater                         ( CoffeeMachineService::TankCapacityWater )
    , mBeans                         ( CoffeeMachineService::TankCapacityBeans )
    , mMilk                          ( CoffeeMachineService::TankCapacityMilk )
    , mWaterWarnedLow                ( false )
    , mWaterWarnedEmpty              ( false )
    , mBeansWarnedLow                ( false )
    , mBeansWarnedEmpty              ( false )
    , mMilkWarnedLow                 ( false )
    , mMilkWarnedEmpty               ( false )
    , mCurrentDrink                  ( CoffeeMachineService::DrinkType::Espresso )
    , mBeansConsumed                 ( false )
    , mWaterConsumed                 ( false )
    , mMilkConsumed                  ( false )
{
    set_credit(mCredit);
}

void MachineComponent::startup_component(areg::ComponentThread & comThread)
{
    areg::Component::startup_component(comThread);
    mFsm.init_fsm(&comThread);
}

void MachineComponent::shutdown_component(areg::ComponentThread & comThread)
{
    mFsm.release_fsm();
    areg::Component::shutdown_component(comThread);
}

const MachineComponent::Recipe & MachineComponent::recipe_of(CoffeeMachineService::DrinkType drink)
{
    return sRecipes[static_cast<size_t>(drink)];
}

const char * MachineComponent::ingredient_name(CoffeeMachineService::TankType tank)
{
    switch (tank)
    {
    case CoffeeMachineService::TankType::Water:    return "water";
    case CoffeeMachineService::TankType::Beans:    return "beans";
    case CoffeeMachineService::TankType::Milk:     return "milk";
    default:                                       return "";
    }
}

uint32_t & MachineComponent::level_of(CoffeeMachineService::TankType tank)
{
    switch (tank)
    {
    case CoffeeMachineService::TankType::Beans:    return mBeans;
    case CoffeeMachineService::TankType::Milk:     return mMilk;
    case CoffeeMachineService::TankType::Water:
    default:                                       return mWater;
    }
}

uint32_t MachineComponent::capacity_of(CoffeeMachineService::TankType tank) const
{
    switch (tank)
    {
    case CoffeeMachineService::TankType::Beans:    return CoffeeMachineService::TankCapacityBeans;
    case CoffeeMachineService::TankType::Milk:     return CoffeeMachineService::TankCapacityMilk;
    case CoffeeMachineService::TankType::Water:
    default:                                       return CoffeeMachineService::TankCapacityWater;
    }
}

void MachineComponent::check_level(CoffeeMachineService::TankType tank)
{
    bool * warnedLow{ nullptr };
    bool * warnedEmpty{ nullptr };
    switch (tank)
    {
    case CoffeeMachineService::TankType::Water:
        warnedLow = &mWaterWarnedLow; warnedEmpty = &mWaterWarnedEmpty; break;
    case CoffeeMachineService::TankType::Beans:
        warnedLow = &mBeansWarnedLow; warnedEmpty = &mBeansWarnedEmpty; break;
    case CoffeeMachineService::TankType::Milk:
        warnedLow = &mMilkWarnedLow; warnedEmpty = &mMilkWarnedEmpty; break;
    default:
        return;
    }

    const uint32_t level{ level_of(tank) };
    const uint32_t capacity{ capacity_of(tank) };
    const char * const name{ ingredient_name(tank) };

    if (level == 0u)
    {
        if (!(*warnedEmpty))
        {
            *warnedEmpty = true;
            *warnedLow   = true;
            std::cout << "machine: " << name << " tank is empty" << std::endl;
            broadcast_ingredient_empty(name);
        }
    }
    else if ((level * 100u) < (capacity * sLowPercent))
    {
        if (!(*warnedLow))
        {
            *warnedLow = true;
            std::cout << "machine: " << name << " tank is low" << std::endl;
            broadcast_ingredient_low(name);
        }
    }
}

void MachineComponent::request_insert_coin(uint32_t value)
{
    switch (value)
    {
    case 5: case 10: case 20: case 50: case 100: case 200:
        mCredit += value;
        set_credit(mCredit);
        break;
    default:
        std::cout << "machine: coin of " << value << " cents refused" << std::endl;
        break;
    }
}

void MachineComponent::request_order_drink(CoffeeMachineService::DrinkType drink)
{
    const Recipe & recipe{ recipe_of(drink) };
    bool accepted{ true };
    areg::String reason;

    if (mCredit < recipe.price)
    {
        accepted = false;
        reason   = "insufficient credit";
    }
    else if (mWater < recipe.water)
    {
        accepted = false;
        reason   = ingredient_name(CoffeeMachineService::TankType::Water);
    }
    else if (mBeans < recipe.beans)
    {
        accepted = false;
        reason   = ingredient_name(CoffeeMachineService::TankType::Beans);
    }
    else if ((recipe.milk > 0u) && (mMilk < recipe.milk))
    {
        accepted = false;
        reason   = ingredient_name(CoffeeMachineService::TankType::Milk);
    }

    response_order_drink(accepted, reason);

    if (accepted)
    {
        mCurrentDrink   = drink;
        mBeansConsumed  = false;
        mWaterConsumed  = false;
        mMilkConsumed   = false;

        // Clears the substate MAKING recorded for pause and resume, so this order
        // starts at the first stage instead of resuming the previous drink's last.
        mFsm.release_fsm(true);
        mFsm.init_fsm(&master_thread());

        mFsm.set_needs_milk(recipe.milk > 0u);
        mFsm.order();
    }
}

void MachineComponent::request_pause_drink(void)
{
    mFsm.pause();
}

void MachineComponent::request_resume_drink(void)
{
    mFsm.resume();
}

void MachineComponent::request_cancel_order(void)
{
    mFsm.cancel();

    const uint32_t refund{ mCredit };
    mCredit = 0u;
    set_credit(mCredit);
    response_cancel_order(refund);
}

void MachineComponent::request_refill_tank(CoffeeMachineService::TankType tank)
{
    level_of(tank) = capacity_of(tank);

    switch (tank)
    {
    case CoffeeMachineService::TankType::Water:
        mWaterWarnedLow = false; mWaterWarnedEmpty = false; break;
    case CoffeeMachineService::TankType::Beans:
        mBeansWarnedLow = false; mBeansWarnedEmpty = false; break;
    case CoffeeMachineService::TankType::Milk:
        mMilkWarnedLow = false; mMilkWarnedEmpty = false; break;
    default:
        break;
    }

    std::cout << "machine: " << ingredient_name(tank) << " tank refilled" << std::endl;
}

void MachineComponent::action_on_idle(void)
{
    set_stage(CoffeeMachineService::MakingStage::Idle);
}

void MachineComponent::action_on_enter_grinding(void)
{
    std::cout << "machine: grinding" << std::endl;
    set_stage(CoffeeMachineService::MakingStage::Grinding);
}

void MachineComponent::action_on_consume_beans(void)
{
    const Recipe & recipe{ recipe_of(mCurrentDrink) };
    mBeans -= recipe.beans;
    mBeansConsumed = true;
    check_level(CoffeeMachineService::TankType::Beans);
}

void MachineComponent::action_on_enter_heating(void)
{
    std::cout << "machine: heating" << std::endl;
    set_stage(CoffeeMachineService::MakingStage::Heating);
}

void MachineComponent::action_on_consume_water(void)
{
    const Recipe & recipe{ recipe_of(mCurrentDrink) };
    mWater -= recipe.water;
    mWaterConsumed = true;
    check_level(CoffeeMachineService::TankType::Water);
}

void MachineComponent::action_on_enter_brewing(void)
{
    std::cout << "machine: brewing" << std::endl;
    set_stage(CoffeeMachineService::MakingStage::Brewing);
}

void MachineComponent::action_on_enter_frothing(void)
{
    std::cout << "machine: frothing" << std::endl;
    set_stage(CoffeeMachineService::MakingStage::Frothing);
}

void MachineComponent::action_on_consume_milk(void)
{
    const Recipe & recipe{ recipe_of(mCurrentDrink) };
    mMilk -= recipe.milk;
    mMilkConsumed = true;
    check_level(CoffeeMachineService::TankType::Milk);
}

void MachineComponent::action_on_enter_dispensing(void)
{
    std::cout << "machine: dispensing" << std::endl;
    set_stage(CoffeeMachineService::MakingStage::Dispensing);
}

void MachineComponent::action_on_finish(void)
{
    const Recipe & recipe{ recipe_of(mCurrentDrink) };
    mCredit -= recipe.price;
    set_credit(mCredit);
    std::cout << "machine: drink ready" << std::endl;
}

void MachineComponent::action_on_cancel(void)
{
    areg::String lost;
    if (mBeansConsumed)
    {
        lost += "beans ";
    }
    if (mWaterConsumed)
    {
        lost += "water ";
    }
    if (mMilkConsumed)
    {
        lost += "milk ";
    }

    std::cout << "machine: order cancelled, lost: " << (lost.is_empty() ? "nothing" : lost.as_string()) << std::endl;
}

void MachineComponent::action_on_pause(void)
{
    std::cout << "machine: paused" << std::endl;
    set_stage(CoffeeMachineService::MakingStage::Paused);
}
