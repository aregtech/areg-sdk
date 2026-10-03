#pragma once

/************************************************************************
 * \file        machine/src/CoffeeMachineServiceProvider.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Coffee machine driven by a generated state machine: the machine component.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/base/String.hpp"

#include "examples/34_coffeemachine/services/CoffeeMachineServiceProviderBase.hpp"
#include "examples/34_coffeemachine/services/CoffeeMakingActionHandler.hpp"
#include "examples/34_coffeemachine/services/CoffeeMakingFSM.hpp"

class CoffeeMachineServiceProvider final : public    areg::Component
                                         , protected CoffeeMachineServiceProviderBase
                                         , protected CoffeeMakingActionHandler
{
public:
    CoffeeMachineServiceProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

protected:
    void startup_component(areg::ComponentThread & comThread) final;
    void shutdown_component(areg::ComponentThread & comThread) final;
    // A request handler converts the call into a stimulus and decides nothing:
    //   mFsm.order(price, water, beans, milk);
    //   mFsm.pause();
    //   mFsm.resume();
    //   mFsm.cancel();
    void request_insert_coin( uint32_t value ) final;
    void request_order_drink( CoffeeMachineService::DrinkType drink ) final;
    void request_pause_drink() final;
    void request_resume_drink() final;
    void request_cancel_drink() final;
    void request_refill_tank( CoffeeMachineService::IngredientType tank ) final;
    // Every condition a guard of the machine asks. Answer it and change nothing.
    bool has_credit( uint32_t amount ) final;
    bool has_water( uint32_t amount ) final;
    bool has_beans( uint32_t amount ) final;
    bool has_milk( uint32_t amount ) final;
    bool needs_milk() final;
    // Every action the machine performs. Never raise a stimulus from one.
    void action_accept_order( uint32_t price, uint32_t water, uint32_t beans, uint32_t milk ) final;
    void action_refuse_order( uint32_t price, uint32_t water, uint32_t beans, uint32_t milk ) final;
    void action_abandon_drink() final;
    void action_finish_drink() final;
    void action_enter_grinding() final;
    void action_enter_heating() final;
    void action_enter_brewing() final;
    void action_enter_frothing() final;
    void action_enter_dispensing() final;
    void action_pause_now() final;
    void action_resume_now() final;
    void action_set_busy() final;
    void action_clear_busy() final;

private:
    uint32_t mWaterLevel{ 2000 };
    uint32_t mBeansLevel{ 500 };
    uint32_t mMilkLevel{ 1000 };
    bool mWaterLow{ false };
    bool mWaterEmpty{ false };
    bool mBeansLow{ false };
    bool mBeansEmpty{ false };
    bool mMilkLow{ false };
    bool mMilkEmpty{ false };
    uint32_t mCurPrice{ 0 };
    uint32_t mCurWater{ 0 };
    uint32_t mCurBeans{ 0 };
    uint32_t mCurMilk{ 0 };
    bool mBusy{ false };

    void check_and_warn(CoffeeMachineService::IngredientType tank, uint32_t level, uint32_t capacity, bool & lowFlag, bool & emptyFlag)
    {
        uint32_t threshold = capacity / 5;
        if (level == 0)
        {
            if (!emptyFlag)
            {
                emptyFlag = true;
                broadcast_ingredient_warning(tank, CoffeeMachineService::WarningLevel::Empty);
                std::cout << "provider: " << CoffeeMachineService::as_string(tank) << " tank empty" << std::endl;
            }
        }
        else if (level < threshold)
        {
            if (!lowFlag)
            {
                lowFlag = true;
                broadcast_ingredient_warning(tank, CoffeeMachineService::WarningLevel::Low);
                std::cout << "provider: " << CoffeeMachineService::as_string(tank) << " tank low" << std::endl;
            }
        }
    }
    inline CoffeeMachineServiceProvider & self()
    {   return (*this); }

    CoffeeMakingFSM  mFsm;    //!< The state machine this component drives.

    CoffeeMachineServiceProvider() = delete;
    AREG_NOCOPY_NOMOVE(CoffeeMachineServiceProvider);
};

