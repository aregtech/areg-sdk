#pragma once

/************************************************************************
 * \file        machine/src/MachineComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Coffee machine: accepts coins and orders, and runs the making stages.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "examples/34_coffeemachine/services/CoffeeMachineServiceProviderBase.hpp"
#include "examples/34_coffeemachine/services/CoffeeMachineActionHandler.hpp"
#include "examples/34_coffeemachine/services/CoffeeMachineFSM.hpp"

/**
 * \brief   Owns the credit, the tanks and the state machine that makes the drinks.
 *          The machine's own logic lives in the .fsml document; this class answers
 *          requests, turns an accepted order into a stimulus, and implements the
 *          actions the machine calls.
 **/
class MachineComponent final    : public    areg::Component
                                , protected CoffeeMachineServiceProviderBase
                                , protected CoffeeMachineActionHandler
{
public:
    MachineComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner);
    virtual ~MachineComponent(void) = default;

    //!< One drink's price and recipe. Public so the recipe table can be built at file scope.
    struct Recipe
    {
        uint32_t price;    //!< Cents.
        uint32_t water;    //!< Millilitres.
        uint32_t beans;    //!< Grams.
        uint32_t milk;     //!< Millilitres. Zero means the drink needs no milk.
    };

protected:
/************************************************************************
 * areg::Component overrides
 ************************************************************************/
    void startup_component(areg::ComponentThread & comThread) override;
    void shutdown_component(areg::ComponentThread & comThread) override;

/************************************************************************
 * CoffeeMachineServiceProviderBase overrides
 ************************************************************************/
    void request_insert_coin(uint32_t value) override;
    void request_order_drink(CoffeeMachineService::DrinkType drink) override;
    void request_pause_drink(void) override;
    void request_resume_drink(void) override;
    void request_cancel_order(void) override;
    void request_refill_tank(CoffeeMachineService::TankType tank) override;

/************************************************************************
 * CoffeeMachineActionHandler overrides
 ************************************************************************/
    void action_on_idle(void) override;
    void action_on_enter_grinding(void) override;
    void action_on_consume_beans(void) override;
    void action_on_enter_heating(void) override;
    void action_on_consume_water(void) override;
    void action_on_enter_brewing(void) override;
    void action_on_enter_frothing(void) override;
    void action_on_consume_milk(void) override;
    void action_on_enter_dispensing(void) override;
    void action_on_finish(void) override;
    void action_on_cancel(void) override;
    void action_on_pause(void) override;

private:
    //!< Looks up the recipe of a drink.
    static const Recipe & recipe_of(CoffeeMachineService::DrinkType drink);
    //!< Human readable name of an ingredient, for reasons, broadcasts and reports.
    static const char * ingredient_name(CoffeeMachineService::TankType tank);

    //!< Checks a tank's level and sends the low / empty broadcasts once per crossing.
    void check_level(CoffeeMachineService::TankType tank);
    //!< True while the level is at or below the given tank's remaining amount.
    uint32_t & level_of(CoffeeMachineService::TankType tank);
    uint32_t capacity_of(CoffeeMachineService::TankType tank) const;

private:
    CoffeeMachineFSM                mFsm;               //!< The making-stages state machine.

    uint32_t                        mCredit;            //!< Cents inserted, not yet spent.
    uint32_t                        mWater;              //!< Millilitres remaining.
    uint32_t                        mBeans;              //!< Grams remaining.
    uint32_t                        mMilk;               //!< Millilitres remaining.

    bool                             mWaterWarnedLow;
    bool                             mWaterWarnedEmpty;
    bool                             mBeansWarnedLow;
    bool                             mBeansWarnedEmpty;
    bool                             mMilkWarnedLow;
    bool                             mMilkWarnedEmpty;

    CoffeeMachineService::DrinkType mCurrentDrink;      //!< The drink currently being made.
    bool                             mBeansConsumed;     //!< True once this order's beans were taken.
    bool                             mWaterConsumed;     //!< True once this order's water was taken.
    bool                             mMilkConsumed;       //!< True once this order's milk was taken.

    inline MachineComponent & self(void)
    {   return (*this); }
};
