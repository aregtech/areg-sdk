#include "user/src/UserComponent.hpp"

#include <iostream>

#include "areg/appbase/Application.hpp"

int g_scenarioExitCode{ 1 };

namespace
{
    constexpr unsigned int RESUME_DELAY_MS{ 400 };
    constexpr unsigned int TIMEOUT_MS{ 20000 };
}

UserComponent::UserComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component(entry, owner)
    , CoffeeMachineServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    , areg::TimerConsumer()
    , mStep(Step::RefuseNoMoney)
    , mCredit(0)
    , mStage()
    , mStagePrePause()
    , mMilkLowWarnings(0)
    , mCreditBeforeCancel(0)
    , mSawFrothingForEspresso(false)
    , mExitCode(1)
    , mDelayTimer(static_cast<areg::TimerConsumer &>(self()), "ResumeDelay")
    , mTimeoutTimer(static_cast<areg::TimerConsumer &>(self()), "ScenarioTimeout")
{
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
            notify_on_broadcast_ingredient_warning(true);
            notify_on_broadcast_drink_ready(true);
            notify_on_broadcast_order_cancelled(true);

            mTimeoutTimer.start_timer(TIMEOUT_MS, static_cast<areg::DispatcherThread &>(master_thread()), 1);

            std::cout << "user: step 1 - order a Cappuccino with no money" << std::endl;
            request_order_drink("Cappuccino");
        }
    }

    return result;
}

void UserComponent::response_order_drink(bool accepted, const areg::String & reason)
{
    switch (mStep)
    {
    case Step::RefuseNoMoney:
        if (accepted || (reason != "insufficient credit"))
        {
            fail("expected a refusal for insufficient credit");
            return;
        }
        ok("refused for insufficient credit, as expected");

        std::cout << "user: step 2 - insert 200 cents, order a Cappuccino" << std::endl;
        mStep = Step::WaitCappuccinoAccepted;
        request_insert_coin(200);
        request_order_drink("Cappuccino");
        break;

    case Step::WaitCappuccinoAccepted:
        if (!accepted)
        {
            fail("expected the Cappuccino order to be accepted");
            return;
        }
        ok("Cappuccino order accepted");

        std::cout << "user: step 3 - pause while it is being made, wait, resume" << std::endl;
        mStagePrePause = mStage;
        mStep = Step::WaitPausedStage;
        request_pause_making();
        break;

    case Step::OrderingLattes:
        if (accepted)
        {
            // Wait for this Latte to finish (broadcast_drink_ready) before ordering
            // the next one: the machine only ever makes one drink at a time.
        }
        else if (reason == "milk")
        {
            ok("Latte refused: out of milk, as expected");
            if (mMilkLowWarnings != 1)
            {
                fail("expected exactly one low-milk warning before running out");
                return;
            }

            std::cout << "user: step 6 - refill and order one more drink" << std::endl;
            mStep = Step::WaitRefill;
            request_refill_tank("Milk");
        }
        else
        {
            fail("Latte was refused for an unexpected reason");
        }
        break;

    case Step::WaitFinalLatteReady:
        if (!accepted)
        {
            fail("expected the final Latte order to be accepted after refilling");
            return;
        }
        ok("final Latte order accepted after refilling");
        break;

    case Step::WaitEspressoReady:
        if (!accepted)
        {
            fail("expected the Espresso order to be accepted");
            return;
        }
        ok("Espresso order accepted");
        break;

    case Step::WaitCancelConfirmed:
        if (!accepted)
        {
            fail("expected the cancel-test order to be accepted");
            return;
        }
        mCreditBeforeCancel = mCredit;
        request_cancel_order();
        break;

    default:
        break;
    }
}

void UserComponent::response_refill_tank(bool ok_, const areg::String & tank)
{
    if (mStep != Step::WaitRefill)
    {
        return;
    }

    if (!ok_ || (tank != "Milk"))
    {
        fail("refilling the milk tank failed");
        return;
    }

    ok("milk tank refilled");
    mStep = Step::WaitFinalLatteReady;
    request_order_drink("Latte");
}

void UserComponent::broadcast_ingredient_warning(const areg::String & ingredient, const areg::String & level)
{
    std::cout << "user: warning - " << ingredient << " is " << level << std::endl;
    if ((ingredient == "Milk") && (level == "low"))
    {
        ++mMilkLowWarnings;
    }
}

void UserComponent::broadcast_drink_ready(const areg::String & drink)
{
    std::cout << "user: drink ready - " << drink << std::endl;

    if (mStep == Step::WaitCappuccinoReady)
    {
        if (mCredit != 20)
        {
            fail("expected 20 cents of credit left after the Cappuccino");
            return;
        }
        ok("credit is 20 cents after the Cappuccino");

        std::cout << "user: step 5 - order Lattes until the milk warning, then until it runs out" << std::endl;
        mStep = Step::OrderingLattes;
        for (int i = 0; i < 8; ++i)
        {
            request_insert_coin(200);
        }
        request_order_drink("Latte");
    }
    else if (mStep == Step::OrderingLattes)
    {
        // This Latte finished: order the next one.
        request_order_drink("Latte");
    }
    else if (mStep == Step::WaitFinalLatteReady)
    {
        std::cout << "user: extra step - order an Espresso through to completion, "
                     "to confirm it never froths" << std::endl;
        mStep = Step::WaitEspressoReady;
        mSawFrothingForEspresso = false;
        request_insert_coin(200);
        request_order_drink("Espresso");
    }
    else if (mStep == Step::WaitEspressoReady)
    {
        if (mSawFrothingForEspresso)
        {
            fail("Espresso frothed milk; it should not for a recipe with no milk");
            return;
        }
        ok("Espresso finished without frothing");

        std::cout << "user: extra step - order another Espresso, then cancel it" << std::endl;
        mStep = Step::WaitCancelConfirmed;
        request_insert_coin(200);
        request_order_drink("Espresso");
    }
}

void UserComponent::broadcast_order_cancelled(uint32_t refunded, const areg::String & lost)
{
    if (mStep != Step::WaitCancelConfirmed)
    {
        return;
    }

    std::cout << "user: order cancelled, refunded " << refunded
              << ", lost: " << (lost.is_empty() ? "none" : lost.as_string()) << std::endl;

    if (refunded != mCreditBeforeCancel)
    {
        fail("cancel did not return the whole credit");
        return;
    }

    ok("cancel returned the whole credit");
    mStep = Step::Done;
    mExitCode = 0;
    g_scenarioExitCode = 0;
    areg::Application::signal_quit();
}

void UserComponent::on_credit_update(uint32_t Credit, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        mCredit = Credit;
        std::cout << "user: credit " << mCredit << std::endl;
    }
}

void UserComponent::on_stage_update(const areg::String & Stage, areg::DataState state)
{
    if (state != areg::DataState::DataIsOK)
    {
        return;
    }

    mStage = Stage;
    std::cout << "user: stage " << mStage << std::endl;

    if ((mStep == Step::WaitEspressoReady) && (mStage == "frothing"))
    {
        mSawFrothingForEspresso = true;
    }

    if ((mStep == Step::WaitPausedStage) && (mStage == "paused"))
    {
        ok("machine reports paused");
        mStep = Step::WaitResumedStage;
        mDelayTimer.start_timer(RESUME_DELAY_MS, static_cast<areg::DispatcherThread &>(master_thread()), 1);
    }
    else if ((mStep == Step::WaitResumedStage) && (mStage != "paused"))
    {
        if (mStage != mStagePrePause)
        {
            fail("resume did not continue from the interrupted stage");
            return;
        }

        std::cout << "user: resumed in stage '" << mStage << "', the stage it was interrupted in" << std::endl;
        ok("pause/resume continued from the interrupted stage");
        mStep = Step::WaitCappuccinoReady;
    }
}

void UserComponent::process_timer(areg::Timer & timer)
{
    if (&timer == &mDelayTimer)
    {
        std::cout << "user: resuming" << std::endl;
        request_resume_making();
    }
    else if (&timer == &mTimeoutTimer)
    {
        if (mStep != Step::Done)
        {
            fail("scenario timed out");
        }
    }
}

void UserComponent::fail(const char * what)
{
    std::cout << "FAIL: " << what << std::endl;
    mExitCode = 1;
    g_scenarioExitCode = 1;
    areg::Application::signal_quit();
}

void UserComponent::ok(const char * what)
{
    std::cout << "OK: " << what << std::endl;
}
