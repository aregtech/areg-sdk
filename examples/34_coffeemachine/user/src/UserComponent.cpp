/************************************************************************
 * \file        user/src/UserComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Simulated user: drives the coffee machine through a fixed scenario.
 ************************************************************************/
#include "user/src/UserComponent.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <iostream>

namespace
{
    constexpr uint32_t sResumeDelayMs{ 300 };
    constexpr uint32_t sLatteSafetyBound{ 30 };
}

int UserComponent::sExitCode{ 1 };

UserComponent::UserComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component                  ( entry, owner )
    , CoffeeMachineServiceConsumerBase ( entry.mDependencyServices[0].mRoleName, owner )
    , areg::TimerConsumer               ( )
    , mResumeTimer                       ( static_cast<areg::TimerConsumer &>(self()), "ResumeTimer" )
    , mStep                              ( Step::RefuseNoMoney )
    , mCreditSeen                        ( 0u )
    , mWaitForCredit                     ( 0u )
    , mStagePausedIn                     ( CoffeeMachineService::MakingStage::Idle )
    , mMilkWarningSeen                   ( false )
    , mLattesOrdered                     ( 0u )
{
}

int UserComponent::get_exit_code(void)
{
    return sExitCode;
}

bool UserComponent::service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    bool result{ false };
    if (CoffeeMachineServiceConsumerBase::service_connected(status, proxy))
    {
        result = true;
        if (areg::is_service_connected(status))
        {
            notify_on_credit_update(true);
            notify_on_stage_update(true);
            notify_on_broadcast_ingredient_low(true);
            notify_on_broadcast_ingredient_empty(true);

            std::cout << "Step 1: order a Cappuccino with no credit, expect a refusal ..." << std::endl;
            mStep = Step::RefuseNoMoney;
            request_order_drink(CoffeeMachineService::DrinkType::Cappuccino);
        }
    }

    return result;
}

void UserComponent::request_order_drink_failed([[maybe_unused]] areg::ResultType reason)
{
    if (is_connected())
    {
        switch (mStep)
        {
        case Step::RefuseNoMoney:
        case Step::WaitOrderAccepted:
            request_order_drink(CoffeeMachineService::DrinkType::Cappuccino);
            break;
        case Step::LatteOrdered:
        case Step::RefillOrdered:
            request_order_drink(CoffeeMachineService::DrinkType::Latte);
            break;
        default:
            break;
        }
    }
}

void UserComponent::response_order_drink(bool accepted, const areg::String & reason)
{
    switch (mStep)
    {
    case Step::RefuseNoMoney:
        if (accepted)
        {
            fail(1, "Cappuccino was accepted with no credit");
            return;
        }

        std::cout << "Step 1: refused (" << reason << ") as expected" << std::endl;
        std::cout << "Step 2: insert 200 cents, order a Cappuccino ..." << std::endl;
        mStep          = Step::WaitCreditForOrder;
        mWaitForCredit = 200u;
        request_insert_coin(200u);
        break;

    case Step::WaitOrderAccepted:
        if (!accepted)
        {
            fail(2, areg::String("").format("Cappuccino was refused: %s", reason.as_string()));
            return;
        }

        std::cout << "Step 2: accepted." << std::endl;
        mStep = Step::WaitMakingStarted;
        break;

    case Step::LatteOrdered:
        if (accepted)
        {
            ++mLattesOrdered;
            std::cout << "Step 5: Latte #" << mLattesOrdered << " accepted." << std::endl;
            mStep = Step::LatteFinishing;
        }
        else if (!mMilkWarningSeen)
        {
            fail(5, "milk ran out before the low warning was sent");
            return;
        }
        else if (!(reason == "milk"))
        {
            fail(5, areg::String("").format("Latte refused for an unexpected reason: %s", reason.as_string()));
            return;
        }
        else
        {
            std::cout << "Step 5: refused (" << reason << ") as expected: milk is out." << std::endl;
            std::cout << "Step 6: refilling the milk tank and ordering once more ..." << std::endl;
            request_refill_tank(CoffeeMachineService::TankType::Milk);
            mStep          = Step::RefillTopUp;
            mWaitForCredit = mCreditSeen + 200u;
            request_insert_coin(200u);
        }
        break;

    case Step::RefillOrdered:
        if (!accepted)
        {
            fail(6, areg::String("").format("the order after refilling was refused: %s", reason.as_string()));
            return;
        }

        std::cout << "Step 6: accepted. The machine recovered." << std::endl;
        finish();
        break;

    default:
        break;      // a stray answer to an earlier retry: ignore it.
    }
}

void UserComponent::response_cancel_order([[maybe_unused]] uint32_t refunded)
{
    // Not used by this scenario.
}

void UserComponent::on_credit_update(uint32_t Credit, areg::DataState state)
{
    if (state != areg::DataState::DataIsOK)
    {
        return;
    }

    mCreditSeen = Credit;

    switch (mStep)
    {
    case Step::WaitCreditForOrder:
        if (Credit == mWaitForCredit)
        {
            std::cout << "Step 2: credit is " << Credit << " cents." << std::endl;
            mStep = Step::WaitOrderAccepted;
            request_order_drink(CoffeeMachineService::DrinkType::Cappuccino);
        }
        break;

    case Step::LatteTopUp:
    case Step::RefillTopUp:
        if (Credit == mWaitForCredit)
        {
            mStep = (mStep == Step::LatteTopUp) ? Step::LatteOrdered : Step::RefillOrdered;
            request_order_drink(CoffeeMachineService::DrinkType::Latte);
        }
        break;

    default:
        break;      // waiting on something else: ignore.
    }
}

void UserComponent::on_stage_update(CoffeeMachineService::MakingStage Stage, areg::DataState state)
{
    if (state != areg::DataState::DataIsOK)
    {
        return;
    }

    const bool isMaking{ (Stage != CoffeeMachineService::MakingStage::Idle)
                       && (Stage != CoffeeMachineService::MakingStage::Paused) };

    switch (mStep)
    {
    case Step::WaitMakingStarted:
        if (isMaking)
        {
            mStagePausedIn = Stage;
            std::cout << "Step 3: making started (stage " << CoffeeMachineService::as_string(Stage)
                       << "), pausing ..." << std::endl;
            mStep = Step::WaitPaused;
            request_pause_drink();
        }
        break;

    case Step::WaitPaused:
        if (Stage == CoffeeMachineService::MakingStage::Paused)
        {
            std::cout << "Step 3: paused. Waiting, then resuming ..." << std::endl;
            mStep = Step::WaitResumeTimer;
            mResumeTimer.start_timer(sResumeDelayMs, static_cast<areg::DispatcherThread &>(master_thread()), areg::TimerBase::ONE_TIME);
        }
        break;

    case Step::WaitResumed:
        if (Stage == mStagePausedIn)
        {
            std::cout << "Step 3: resumed in stage " << CoffeeMachineService::as_string(Stage)
                       << ", as expected." << std::endl;
            mStep = Step::WaitFinishedFirst;
        }
        else if (Stage == CoffeeMachineService::MakingStage::Idle)
        {
            fail(3, "the drink finished before the resumed stage was observed");
        }
        break;

    case Step::WaitFinishedFirst:
        if (Stage == CoffeeMachineService::MakingStage::Idle)
        {
            // Credit changes before Stage does (on_finish runs, then IDLE's own entry
            // sets Stage): mCreditSeen is already current, so check it directly instead
            // of waiting for another Credit update that, being OnChange, will not repeat.
            std::cout << "Step 4: drink finished. Credit is " << mCreditSeen << " cents." << std::endl;
            if (mCreditSeen != 20u)
            {
                fail(4, areg::String("").format("expected credit 20, got %u", mCreditSeen));
                return;
            }

            std::cout << "Step 5: ordering Lattes until the milk warning, then until it runs out ..." << std::endl;
            mStep          = Step::LatteTopUp;
            mWaitForCredit = mCreditSeen + 200u;
            request_insert_coin(200u);
        }
        break;

    case Step::LatteFinishing:
        if (Stage == CoffeeMachineService::MakingStage::Idle)
        {
            if (mLattesOrdered > sLatteSafetyBound)
            {
                fail(5, "milk never ran low, ordered too many Lattes");
                return;
            }

            mStep          = Step::LatteTopUp;
            mWaitForCredit = mCreditSeen + 200u;
            request_insert_coin(200u);
        }
        break;

    default:
        break;      // not waited on in this step: ignore.
    }
}

void UserComponent::broadcast_ingredient_low(const areg::String & ingredient)
{
    std::cout << "  ingredient low: " << ingredient << std::endl;
    if (ingredient == "milk")
    {
        mMilkWarningSeen = true;
    }
}

void UserComponent::broadcast_ingredient_empty(const areg::String & ingredient)
{
    std::cout << "  ingredient empty: " << ingredient << std::endl;
}

void UserComponent::process_timer(areg::Timer & timer)
{
    if ((&timer == &mResumeTimer) && (mStep == Step::WaitResumeTimer))
    {
        mStep = Step::WaitResumed;
        request_resume_drink();
    }
}

void UserComponent::fail(int step, const areg::String & why)
{
    std::cout << "FAILED at step " << step << ": " << why << std::endl;
    sExitCode = 1;
    mStep     = Step::Done;
    areg::Application::signal_quit();
}

void UserComponent::finish(void)
{
    std::cout << "All steps passed." << std::endl;
    sExitCode = 0;
    mStep     = Step::Done;
    areg::Application::signal_quit();
}
