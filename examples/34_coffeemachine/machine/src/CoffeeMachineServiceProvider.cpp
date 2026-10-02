/************************************************************************
 * \file        machine/src/CoffeeMachineServiceProvider.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Coffee machine driven by a generated state machine: the machine component.
 ************************************************************************/
#include "machine/src/CoffeeMachineServiceProvider.hpp"

#include <iostream>

#include "areg/appbase/Application.hpp"

CoffeeMachineServiceProvider::CoffeeMachineServiceProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component(entry, owner)
    , CoffeeMachineServiceProviderBase(static_cast<areg::Component &>(*this))
    , CoffeeMakingActionHandler()
    , mFsm(static_cast<CoffeeMakingActionHandler &>(self()))
{
    set_credit(0);
    set_stage(CoffeeMachineService::MachineStage::Idle);
    set_paused(false);
    std::cout << "provider: ready" << std::endl;
}

void CoffeeMachineServiceProvider::startup_component(areg::ComponentThread & comThread)
{
    areg::Component::startup_component(comThread);
    mFsm.init_fsm(&comThread);
}

void CoffeeMachineServiceProvider::shutdown_component(areg::ComponentThread & comThread)
{
    mFsm.release_fsm();
    areg::Component::shutdown_component(comThread);
}

void CoffeeMachineServiceProvider::request_insert_coin( uint32_t value )
{
    set_credit(credit() + value);
    std::cout << "provider: credit -> " << credit() << std::endl;
}

void CoffeeMachineServiceProvider::request_order_drink( CoffeeMachineService::DrinkType drink )
{
    uint32_t price = 0;
    uint32_t water = 0;
    uint32_t beans = 0;
    uint32_t milk = 0;
    switch (drink)
    {
    case CoffeeMachineService::DrinkType::Espresso:
        price = 120; water = 40; beans = 9; milk = 0;
        break;
    case CoffeeMachineService::DrinkType::Cappuccino:
        price = 180; water = 60; beans = 9; milk = 80;
        break;
    case CoffeeMachineService::DrinkType::Latte:
        price = 200; water = 60; beans = 9; milk = 150;
        break;
    default:
        break;
    }

    if (mBusy)
    {
        response_order_drink(false, areg::String("machine busy making another drink"));
        std::cout << "provider: refused order (machine busy)" << std::endl;
    }
    else
    {
        mFsm.order(price, water, beans, milk);
    }
}

void CoffeeMachineServiceProvider::request_pause_drink()
{
    mFsm.pause();
}

void CoffeeMachineServiceProvider::request_resume_drink()
{
    mFsm.resume();
}

void CoffeeMachineServiceProvider::request_cancel_drink()
{
    mFsm.cancel();
}

void CoffeeMachineServiceProvider::request_refill_tank( CoffeeMachineService::IngredientType tank )
{
    switch (tank)
    {
    case CoffeeMachineService::IngredientType::Water:
        mWaterLevel = 2000;
        mWaterLow = false;
        mWaterEmpty = false;
        break;
    case CoffeeMachineService::IngredientType::Beans:
        mBeansLevel = 500;
        mBeansLow = false;
        mBeansEmpty = false;
        break;
    case CoffeeMachineService::IngredientType::Milk:
        mMilkLevel = 1000;
        mMilkLow = false;
        mMilkEmpty = false;
        break;
    default:
        break;
    }
    response_refill_tank(true);
    std::cout << "provider: refilled " << CoffeeMachineService::as_string(tank) << std::endl;
}

bool CoffeeMachineServiceProvider::has_credit( uint32_t amount )
{
    return credit() >= amount;
}

bool CoffeeMachineServiceProvider::has_water( uint32_t amount )
{
    return mWaterLevel >= amount;
}

bool CoffeeMachineServiceProvider::has_beans( uint32_t amount )
{
    return mBeansLevel >= amount;
}

bool CoffeeMachineServiceProvider::has_milk( uint32_t amount )
{
    return mMilkLevel >= amount;
}

bool CoffeeMachineServiceProvider::needs_milk()
{
    return mCurMilk > 0;
}

void CoffeeMachineServiceProvider::action_accept_order( uint32_t price, uint32_t water, uint32_t beans, uint32_t milk )
{
    mCurPrice = price;
    mCurWater = water;
    mCurBeans = beans;
    mCurMilk = milk;
    response_order_drink(true, areg::String());
    std::cout << "provider: order accepted" << std::endl;
}

void CoffeeMachineServiceProvider::action_refuse_order( uint32_t price, uint32_t water, uint32_t beans, uint32_t milk )
{
    areg::String reason;
    if (!has_credit(price))
    {
        reason += "insufficient credit";
    }
    if (!has_water(water))
    {
        if (!reason.is_empty())
        {
            reason += ", ";
        }
        reason += "missing Water";
    }
    if (!has_beans(beans))
    {
        if (!reason.is_empty())
        {
            reason += ", ";
        }
        reason += "missing Beans";
    }
    if (!has_milk(milk))
    {
        if (!reason.is_empty())
        {
            reason += ", ";
        }
        reason += "missing Milk";
    }
    response_order_drink(false, reason);
    std::cout << "provider: refused order (" << reason.as_string() << ")" << std::endl;
}

void CoffeeMachineServiceProvider::action_abandon_drink()
{
    set_paused(false);
    set_stage(CoffeeMachineService::MachineStage::Idle);
    std::cout << "provider: drink cancelled, no charge" << std::endl;
}

void CoffeeMachineServiceProvider::action_finish_drink()
{
    mWaterLevel -= mCurWater;
    mBeansLevel -= mCurBeans;
    mMilkLevel -= mCurMilk;
    set_stage(CoffeeMachineService::MachineStage::Idle);
    set_credit(credit() - mCurPrice);
    check_and_warn(CoffeeMachineService::IngredientType::Water, mWaterLevel, 2000, mWaterLow, mWaterEmpty);
    check_and_warn(CoffeeMachineService::IngredientType::Beans, mBeansLevel, 500, mBeansLow, mBeansEmpty);
    check_and_warn(CoffeeMachineService::IngredientType::Milk, mMilkLevel, 1000, mMilkLow, mMilkEmpty);
    std::cout << "provider: drink finished, credit=" << credit() << std::endl;
}

void CoffeeMachineServiceProvider::action_enter_grinding()
{
    set_stage(CoffeeMachineService::MachineStage::Grinding);
    std::cout << "provider: stage -> Grinding" << std::endl;
}

void CoffeeMachineServiceProvider::action_enter_heating()
{
    set_stage(CoffeeMachineService::MachineStage::Heating);
    std::cout << "provider: stage -> Heating" << std::endl;
}

void CoffeeMachineServiceProvider::action_enter_brewing()
{
    set_stage(CoffeeMachineService::MachineStage::Brewing);
    std::cout << "provider: stage -> Brewing" << std::endl;
}

void CoffeeMachineServiceProvider::action_enter_frothing()
{
    set_stage(CoffeeMachineService::MachineStage::Frothing);
    std::cout << "provider: stage -> Frothing" << std::endl;
}

void CoffeeMachineServiceProvider::action_enter_dispensing()
{
    set_stage(CoffeeMachineService::MachineStage::Dispensing);
    std::cout << "provider: stage -> Dispensing" << std::endl;
}

void CoffeeMachineServiceProvider::action_pause_now()
{
    set_paused(true);
    std::cout << "provider: paused" << std::endl;
}

void CoffeeMachineServiceProvider::action_resume_now()
{
    set_paused(false);
    std::cout << "provider: resumed" << std::endl;
}

void CoffeeMachineServiceProvider::action_set_busy()
{
    mBusy = true;
}

void CoffeeMachineServiceProvider::action_clear_busy()
{
    mBusy = false;
}
