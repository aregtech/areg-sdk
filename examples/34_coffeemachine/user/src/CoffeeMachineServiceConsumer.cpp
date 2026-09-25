/************************************************************************
 * \file        user/src/CoffeeMachineServiceConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Coffee machine driven by a generated state machine: the simulated user component.
 ************************************************************************/
#include "user/src/CoffeeMachineServiceConsumer.hpp"

#include <iostream>

#include "areg/appbase/Application.hpp"

CoffeeMachineServiceConsumer::CoffeeMachineServiceConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component(entry, owner)
    , CoffeeMachineServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    , areg::TimerConsumer()
    , mDeadline(static_cast<areg::TimerConsumer &>(self()), "Deadline")
    , mPace(static_cast<areg::TimerConsumer &>(self()), "Pace")
    , mHold(static_cast<areg::TimerConsumer &>(self()), "Hold")
{
}

void CoffeeMachineServiceConsumer::startup_component(areg::ComponentThread & thread)
{
    areg::Component::startup_component(thread);
    arm_deadline(cConnectSeconds);
}

bool CoffeeMachineServiceConsumer::service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    bool result{ false };
    if (CoffeeMachineServiceConsumerBase::service_connected(status, proxy))
    {
        result = true;
        if (areg::is_service_connected(status))
        {
            mDeadline.stop_timer();
            mConnected = true;
            // Subscriptions are made here, and again after every reconnection.
            notify_on_credit_update(true);
            notify_on_stage_update(true);
            notify_on_paused_update(true);
            notify_on_broadcast_ingredient_warning(true);

            // The stall watchdog ticks from here; the steps begin once.
            mPace.stop_timer();
            mPace.start_timer(1000, static_cast<areg::DispatcherThread &>(master_thread()),
                              areg::TimerBase::CONTINUOUSLY);
            if (mStep == Step::Start)
            {
                begin(Step::OrderNoMoney);
            }
        }
        else if ((status == areg::ServiceConnectionState::Disconnected) ||
                 (status == areg::ServiceConnectionState::ConnectionLost))
        {
            // The provider went away. The framework reconnects and
            // calls this again; the reconnect deadline is the exit.
            if (is_quitting() == false)
            {
                std::cout << "consumer: lost the machine, waiting to see if it comes back" << std::endl;
                arm_deadline(cReconnectSeconds);
            }
        }
        else if ((status == areg::ServiceConnectionState::Rejected) ||
                 (status == areg::ServiceConnectionState::Shutdown))
        {
            // Terminal states. The framework does not reconnect
            // from these.
            std::cout << "consumer: service rejected or shut down, giving up" << std::endl;
            quit_with(1);
            mDeadline.stop_timer();
            std::cerr << "service is " << areg::as_string(status)
                      << ", giving up" << std::endl;
            mPace.stop_timer();
            quit_with(1);
        }
    }

    return result;
}

void CoffeeMachineServiceConsumer::process_timer(areg::Timer & timer)
{
    if (&timer == &mDeadline)
    {
        fail(mConnected ? "the provider did not come back within the reconnect deadline"
                        : "no provider connected within the connect deadline");
        return;
    }

    if (&timer == &mHold)
    {
        complete();
        return;
    }

    if ((&timer == &mPace) && (cStallTicks != 0) && (++mIdleTicks >= cStallTicks))
    {
        stalled();
        return;
    }
}

void CoffeeMachineServiceConsumer::response_order_drink( bool accepted, const areg::String & reason )
{
    mHeld = false;
    mJumped = false;
    switch (mStep)
    {
    case Step::OrderNoMoney:
        {
            StepEnd ending(*this);
            if (accepted)
            {
                fail("expected the first order to be refused for insufficient credit");
            }
            else
            {
                std::cout << "consumer: order refused: " << reason.as_string() << std::endl;
            }
        }
        break;
    case Step::OrderCappuccino:
        {
            StepEnd ending(*this);
            if (accepted)
            {
                std::cout << "consumer: cappuccino order accepted" << std::endl;
            }
            else
            {
                fail("expected the cappuccino order to be accepted");
            }
        }
        break;
    case Step::OrderLatte:
        {
            StepEnd ending(*this);
            ++mLatteAttempts;
            if (mLatteAttempts > 20)
            {
                fail("too many latte attempts: milk never ran out");
            }
            else if (accepted)
            {
                ++mLatteCount;
                std::cout << "consumer: latte #" << mLatteCount << " accepted" << std::endl;
                go_to(Step::AwaitLatteDone);
            }
            else if (!mMilkWarningSeen)
            {
                fail("expected the milk warning before a latte was refused for missing milk");
            }
            else if (reason.find_first("Milk") == areg::END_POS)
            {
                areg::String why;
                why.format("expected the latte to be refused for missing milk, got: %s", reason.as_string());
                fail(why.as_string());
            }
            else
            {
                std::cout << "consumer: latte refused: " << reason.as_string() << std::endl;
                go_to(Step::RefillMilk);
            }
        }
        break;
    case Step::OrderLatteAfterRefill:
        {
            StepEnd ending(*this);
            if (accepted)
            {
                std::cout << "consumer: latte after refill accepted" << std::endl;
            }
            else
            {
                fail("expected the latte to be accepted after refilling milk");
            }
        }
        break;
    case Step::OrderEspresso:
        {
            StepEnd ending(*this);
            mSawFrothingThisOrder = false;
            if (accepted)
            {
                std::cout << "consumer: espresso order accepted" << std::endl;
            }
            else
            {
                fail("expected the espresso order to be accepted");
            }
        }
        break;
    case Step::OrderThenCancel:
        {
            StepEnd ending(*this);
            if (accepted)
            {
                areg::DataState state;
                mCreditBeforeCancel = credit(state);
                std::cout << "consumer: order accepted before cancelling" << std::endl;
            }
            else
            {
                fail("expected the small drink order to be accepted before cancelling it");
            }
        }
        break;
    default:
        dropped("response order_drink");
        break;
    }
}

void CoffeeMachineServiceConsumer::response_refill_tank( bool success )
{
    mHeld = false;
    mJumped = false;
    switch (mStep)
    {
    case Step::RefillMilk:
        {
            StepEnd ending(*this);
            if (success)
            {
                std::cout << "consumer: milk refilled" << std::endl;
            }
            else
            {
                fail("expected the milk refill to succeed");
            }
        }
        break;
    default:
        dropped("response refill_tank");
        break;
    }
}

void CoffeeMachineServiceConsumer::request_insert_coin_failed(areg::ResultType reason)
{
    std::cerr << "request insert_coin failed, reason " << static_cast<int>(reason) << std::endl;
    mPace.stop_timer();
    quit_with(1);
}

void CoffeeMachineServiceConsumer::request_order_drink_failed(areg::ResultType reason)
{
    std::cerr << "request order_drink failed, reason " << static_cast<int>(reason) << std::endl;
    mPace.stop_timer();
    quit_with(1);
}

void CoffeeMachineServiceConsumer::request_pause_drink_failed(areg::ResultType reason)
{
    std::cerr << "request pause_drink failed, reason " << static_cast<int>(reason) << std::endl;
    mPace.stop_timer();
    quit_with(1);
}

void CoffeeMachineServiceConsumer::request_resume_drink_failed(areg::ResultType reason)
{
    std::cerr << "request resume_drink failed, reason " << static_cast<int>(reason) << std::endl;
    mPace.stop_timer();
    quit_with(1);
}

void CoffeeMachineServiceConsumer::request_cancel_drink_failed(areg::ResultType reason)
{
    std::cerr << "request cancel_drink failed, reason " << static_cast<int>(reason) << std::endl;
    mPace.stop_timer();
    quit_with(1);
}

void CoffeeMachineServiceConsumer::request_refill_tank_failed(areg::ResultType reason)
{
    std::cerr << "request refill_tank failed, reason " << static_cast<int>(reason) << std::endl;
    mPace.stop_timer();
    quit_with(1);
}

void CoffeeMachineServiceConsumer::broadcast_ingredient_warning( CoffeeMachineService::IngredientType tank, CoffeeMachineService::WarningLevel level )
{
    std::cout << "consumer: ingredient warning: " << CoffeeMachineService::as_string(tank) << " -> " << CoffeeMachineService::as_string(level) << std::endl;
    if ((tank == CoffeeMachineService::IngredientType::Milk) && (level == CoffeeMachineService::WarningLevel::Low))
    {
        mMilkWarningSeen = true;
    }
}

void CoffeeMachineServiceConsumer::on_credit_update(uint32_t Credit, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        mLastCredit = Credit;

        mHeld = false;
        mJumped = false;
        switch (mStep)
        {
        case Step::InsertForCappuccino:
            {
                StepEnd ending(*this);
                if (Credit == 200)
                {
                    std::cout << "consumer: credit -> " << Credit << std::endl;
                }
                else
                {
                    fail("expected credit to be 200 after inserting 200 cents");
                }
            }
            break;
        case Step::CheckCreditAfterCappuccino:
            {
                StepEnd ending(*this);
                if (Credit == 20)
                {
                    std::cout << "consumer: credit after cappuccino == " << Credit << std::endl;
                }
                else
                {
                    fail("expected credit 20 after the cappuccino was charged");
                }
            }
            break;
        default:
            dropped("update Credit");
            break;
        }
    }
}

void CoffeeMachineServiceConsumer::on_stage_update(CoffeeMachineService::MachineStage Stage, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        mLastStage = Stage;
        std::cout << "consumer: stage -> " << CoffeeMachineService::as_string(Stage) << std::endl;
        if (Stage == CoffeeMachineService::MachineStage::Frothing)
        {
            mSawFrothingThisOrder = true;
        }

        mHeld = false;
        mJumped = false;
        switch (mStep)
        {
        case Step::AwaitGrinding:
            {
                StepEnd ending(*this);
                if (Stage != CoffeeMachineService::MachineStage::Grinding)
                {
                    fail("expected the Grinding stage to be reported first");
                }
            }
            break;
        case Step::AwaitFrothing:
            {
                StepEnd ending(*this);
                if (Stage != CoffeeMachineService::MachineStage::Frothing)
                {
                    stay();
                }
            }
            break;
        case Step::AwaitCappuccinoDone:
            {
                StepEnd ending(*this);
                if (Stage == CoffeeMachineService::MachineStage::Idle)
                {
                    std::cout << "consumer: cappuccino finished" << std::endl;
                }
                else
                {
                    stay();
                }
            }
            break;
        case Step::AwaitLatteDone:
            {
                StepEnd ending(*this);
                if (Stage == CoffeeMachineService::MachineStage::Idle)
                {
                    std::cout << "consumer: latte #" << mLatteCount << " finished" << std::endl;
                    go_to(Step::InsertForLatte);
                }
                else
                {
                    stay();
                }
            }
            break;
        case Step::AwaitLatteAfterRefillDone:
            {
                StepEnd ending(*this);
                if (Stage == CoffeeMachineService::MachineStage::Idle)
                {
                    std::cout << "consumer: latte after refill finished" << std::endl;
                }
                else
                {
                    stay();
                }
            }
            break;
        case Step::AwaitEspressoDone:
            {
                StepEnd ending(*this);
                if (Stage == CoffeeMachineService::MachineStage::Idle)
                {
                    if (mSawFrothingThisOrder)
                    {
                        fail("frothing happened for an espresso, which needs no milk");
                    }
                    else
                    {
                        std::cout << "consumer: espresso finished, no frothing seen" << std::endl;
                    }
                }
                else
                {
                    stay();
                }
            }
            break;
        case Step::CancelDrink:
            {
                StepEnd ending(*this);
                if (Stage == CoffeeMachineService::MachineStage::Idle)
                {
                    areg::DataState creditState;
                    uint32_t nowCredit = credit(creditState);
                    if (nowCredit == mCreditBeforeCancel)
                    {
                        std::cout << "consumer: cancelled, credit unchanged at " << nowCredit << ", no ingredient drawn" << std::endl;
                    }
                    else
                    {
                        fail("expected the whole credit to remain after cancelling");
                    }
                }
                else
                {
                    stay();
                }
            }
            break;
        default:
            dropped("update Stage");
            break;
        }
    }
}

void CoffeeMachineServiceConsumer::on_paused_update(bool Paused, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        // narrated in the step_ sections below, nothing more to do here.

        mHeld = false;
        mJumped = false;
        switch (mStep)
        {
        case Step::PauseMidway:
            {
                StepEnd ending(*this);
                if (Paused)
                {
                    std::cout << "consumer: paused during stage " << CoffeeMachineService::as_string(mLastStage) << std::endl;
                }
                else
                {
                    fail("expected Paused to become true");
                }
            }
            break;
        case Step::ResumeAfterWait:
            {
                StepEnd ending(*this);
                if (!Paused)
                {
                    std::cout << "consumer: resumed, continuing from stage " << CoffeeMachineService::as_string(mLastStage) << std::endl;
                }
                else
                {
                    fail("expected Paused to become false after resuming");
                }
            }
            break;
        default:
            dropped("update Paused");
            break;
        }
    }
}

const char * CoffeeMachineServiceConsumer::step_slot()
{
    switch (mStep)
    {
    case Step::OrderNoMoney:  return "step_OrderNoMoney";
    case Step::InsertForCappuccino:  return "step_InsertForCappuccino";
    case Step::OrderCappuccino:  return "step_OrderCappuccino";
    case Step::AwaitGrinding:  return "step_AwaitGrinding";
    case Step::PauseMidway:  return "step_PauseMidway";
    case Step::WaitBeforeResume:  return "step_WaitBeforeResume";
    case Step::ResumeAfterWait:  return "step_ResumeAfterWait";
    case Step::AwaitFrothing:  return "step_AwaitFrothing";
    case Step::AwaitCappuccinoDone:  return "step_AwaitCappuccinoDone";
    case Step::CheckCreditAfterCappuccino:  return "step_CheckCreditAfterCappuccino";
    case Step::InsertForLatte:  return "step_InsertForLatte";
    case Step::OrderLatte:  return "step_OrderLatte";
    case Step::AwaitLatteDone:  return "step_AwaitLatteDone";
    case Step::RefillMilk:  return "step_RefillMilk";
    case Step::OrderLatteAfterRefill:  return "step_OrderLatteAfterRefill";
    case Step::AwaitLatteAfterRefillDone:  return "step_AwaitLatteAfterRefillDone";
    case Step::InsertForEspresso:  return "step_InsertForEspresso";
    case Step::OrderEspresso:  return "step_OrderEspresso";
    case Step::AwaitEspressoDone:  return "step_AwaitEspressoDone";
    case Step::InsertForCancel:  return "step_InsertForCancel";
    case Step::OrderThenCancel:  return "step_OrderThenCancel";
    case Step::WaitBeforeCancel:  return "step_WaitBeforeCancel";
    case Step::CancelDrink:  return "step_CancelDrink";
    default:  return "no step";
    }
}

const char * CoffeeMachineServiceConsumer::step_detail()
{
    switch (mStep)
    {
    case Step::OrderNoMoney:  return "sent request_order_drink(CoffeeMachineService::DrinkType::Cappuccino) and awaits response order_drink";
    case Step::InsertForCappuccino:  return "sent request_insert_coin(200) and awaits update Credit";
    case Step::OrderCappuccino:  return "sent request_order_drink(CoffeeMachineService::DrinkType::Cappuccino) and awaits response order_drink";
    case Step::AwaitGrinding:  return "awaits update Stage";
    case Step::PauseMidway:  return "sent request_pause_drink() and awaits update Paused";
    case Step::WaitBeforeResume:  return "waits 500 ms";
    case Step::ResumeAfterWait:  return "sent request_resume_drink() and awaits update Paused";
    case Step::AwaitFrothing:  return "awaits update Stage";
    case Step::AwaitCappuccinoDone:  return "awaits update Stage";
    case Step::CheckCreditAfterCappuccino:  return "awaits update Credit";
    case Step::InsertForLatte:  return "sent request_insert_coin(200)";
    case Step::OrderLatte:  return "sent request_order_drink(CoffeeMachineService::DrinkType::Latte) and awaits response order_drink";
    case Step::AwaitLatteDone:  return "awaits update Stage";
    case Step::RefillMilk:  return "sent request_refill_tank(CoffeeMachineService::IngredientType::Milk) and awaits response refill_tank";
    case Step::OrderLatteAfterRefill:  return "sent request_order_drink(CoffeeMachineService::DrinkType::Latte) and awaits response order_drink";
    case Step::AwaitLatteAfterRefillDone:  return "awaits update Stage";
    case Step::InsertForEspresso:  return "sent request_insert_coin(200)";
    case Step::OrderEspresso:  return "sent request_order_drink(CoffeeMachineService::DrinkType::Espresso) and awaits response order_drink";
    case Step::AwaitEspressoDone:  return "awaits update Stage";
    case Step::InsertForCancel:  return "sent request_insert_coin(100)";
    case Step::OrderThenCancel:  return "sent request_order_drink(CoffeeMachineService::DrinkType::Espresso) and awaits response order_drink";
    case Step::WaitBeforeCancel:  return "waits 150 ms";
    case Step::CancelDrink:  return "sent request_cancel_drink() and awaits update Stage";
    default:  return "waits for nothing";
    }
}

void CoffeeMachineServiceConsumer::stalled()
{
    fail("the scenario stopped making progress");
    std::cerr << "  " << step_slot() << " " << step_detail()
              << (mRan ? ". Its check ran and kept the step."
                       : ". Nothing arrived.") << std::endl;
    for (uint32_t kept = 0; kept < mDroppedKept; ++ kept)
    {
        std::cerr << "  dropped: " << mDroppedWhat[kept]
                  << " arrived on " << mDroppedStep[kept]
                  << ", which has no check for it."
                  << std::endl;
    }
    if (mDroppedCount > mDroppedKept)
    {
        std::cerr << "  dropped: and "
                  << (mDroppedCount - mDroppedKept)
                  << " more." << std::endl;
    }
}

void CoffeeMachineServiceConsumer::dropped(const char * what)
{
    if (mDroppedKept < cDroppedMost)
    {
        mDroppedWhat[mDroppedKept] = what;
        mDroppedStep[mDroppedKept] = step_slot();
        ++ mDroppedKept;
    }
    ++ mDroppedCount;
}

void CoffeeMachineServiceConsumer::fail(const char * why)
{
    std::cerr << "FAIL [" << step_slot() << "]: " << why
              << std::endl;
    mDeadline.stop_timer();
    mPace.stop_timer();
    mHold.stop_timer();
    quit_with(1);
}

void CoffeeMachineServiceConsumer::arm_deadline(uint32_t seconds)
{
    mDeadline.stop_timer();
    if (seconds != 0)
    {
        mDeadline.start_timer(seconds * 1000,
                              static_cast<areg::DispatcherThread &>(master_thread()),
                              areg::TimerBase::ONE_TIME);
    }
}

void CoffeeMachineServiceConsumer::begin(Step step)
{
    mStep = step;
    mNext = step;
    mJumped = false;
    mHeld = false;
    mRan = false;
    progressed();
    switch (step)
    {
    case Step::OrderNoMoney:
        std::cout << "step OrderNoMoney" << std::endl;
        request_order_drink(CoffeeMachineService::DrinkType::Cappuccino);
        break;
    case Step::InsertForCappuccino:
        std::cout << "step InsertForCappuccino" << std::endl;
        request_insert_coin(200);
        break;
    case Step::OrderCappuccino:
        std::cout << "step OrderCappuccino" << std::endl;
        request_order_drink(CoffeeMachineService::DrinkType::Cappuccino);
        break;
    case Step::AwaitGrinding:
        std::cout << "step AwaitGrinding" << std::endl;
        break;
    case Step::PauseMidway:
        std::cout << "step PauseMidway" << std::endl;
        request_pause_drink();
        break;
    case Step::WaitBeforeResume:
        std::cout << "step WaitBeforeResume" << std::endl;
        mHold.stop_timer();
        mHold.start_timer(500, static_cast<areg::DispatcherThread &>(master_thread()),
                          areg::TimerBase::ONE_TIME);
        break;
    case Step::ResumeAfterWait:
        std::cout << "step ResumeAfterWait" << std::endl;
        request_resume_drink();
        break;
    case Step::AwaitFrothing:
        std::cout << "step AwaitFrothing" << std::endl;
        break;
    case Step::AwaitCappuccinoDone:
        std::cout << "step AwaitCappuccinoDone" << std::endl;
        break;
    case Step::CheckCreditAfterCappuccino:
        std::cout << "step CheckCreditAfterCappuccino" << std::endl;
        break;
    case Step::InsertForLatte:
        std::cout << "step InsertForLatte" << std::endl;
        request_insert_coin(200);
        complete();
        break;
    case Step::OrderLatte:
        std::cout << "step OrderLatte" << std::endl;
        request_order_drink(CoffeeMachineService::DrinkType::Latte);
        break;
    case Step::AwaitLatteDone:
        std::cout << "step AwaitLatteDone" << std::endl;
        break;
    case Step::RefillMilk:
        std::cout << "step RefillMilk" << std::endl;
        request_refill_tank(CoffeeMachineService::IngredientType::Milk);
        break;
    case Step::OrderLatteAfterRefill:
        std::cout << "step OrderLatteAfterRefill" << std::endl;
        request_order_drink(CoffeeMachineService::DrinkType::Latte);
        break;
    case Step::AwaitLatteAfterRefillDone:
        std::cout << "step AwaitLatteAfterRefillDone" << std::endl;
        break;
    case Step::InsertForEspresso:
        std::cout << "step InsertForEspresso" << std::endl;
        request_insert_coin(200);
        complete();
        break;
    case Step::OrderEspresso:
        std::cout << "step OrderEspresso" << std::endl;
        request_order_drink(CoffeeMachineService::DrinkType::Espresso);
        break;
    case Step::AwaitEspressoDone:
        std::cout << "step AwaitEspressoDone" << std::endl;
        break;
    case Step::InsertForCancel:
        std::cout << "step InsertForCancel" << std::endl;
        request_insert_coin(100);
        complete();
        break;
    case Step::OrderThenCancel:
        std::cout << "step OrderThenCancel" << std::endl;
        request_order_drink(CoffeeMachineService::DrinkType::Espresso);
        break;
    case Step::WaitBeforeCancel:
        std::cout << "step WaitBeforeCancel" << std::endl;
        mHold.stop_timer();
        mHold.start_timer(150, static_cast<areg::DispatcherThread &>(master_thread()),
                          areg::TimerBase::ONE_TIME);
        break;
    case Step::CancelDrink:
        std::cout << "step CancelDrink" << std::endl;
        request_cancel_drink();
        break;
    case Step::Done:
        mDeadline.stop_timer();
        mPace.stop_timer();
        quit_with(0);
        break;
    default:
        break;
    }
}

void CoffeeMachineServiceConsumer::complete()
{
    mEnding = false;
    if (is_quitting() || mHeld)
    {
        mHeld = false;
        return;
    }

    begin(mJumped ? mNext : static_cast<Step>(static_cast<uint32_t>(mStep) + 1));
}
