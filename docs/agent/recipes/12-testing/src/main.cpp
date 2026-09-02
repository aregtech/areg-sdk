/**
 * \file    main.cpp
 * \brief   A consumer tested against a scripted provider, in one process.
 **/
#include <atomic>
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include "src/services/HelloServiceProviderBase.hpp"
#include "src/services/HelloServiceConsumerBase.hpp"

namespace
{
    constexpr char const _roleProvider[]{ "ScriptedProvider" };
    constexpr char const _roleGood[]    { "Client_Good" };
    constexpr char const _roleBad[]     { "Client_Bad" };
    constexpr int        _expected      { 2 };

    std::atomic<int>  gFailed   { 0 };
    std::atomic<int>  gAnswered { 0 };
    std::atomic<bool> gGood     { false };
    std::atomic<bool> gBad      { true };

    //! Prints one verdict line and counts the failures.
    void check( const char * name, bool passed )
    {
        std::cout << "test: " << name << (passed ? " ... PASS" : " ... FAIL") << std::endl;
        if ( passed == false )
        {
            gFailed.fetch_add( 1 );
        }
    }
}

/**
 * \brief   Answers from a script instead of doing the work. The consumer under
 *          test cannot tell it from the provider it meets in production, so the
 *          answer it has to handle is chosen here rather than waited for.
 **/
class ScriptedProvider final : public    areg::Component
                             , protected HelloServiceProviderBase
{
public:
    ScriptedProvider( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
        : areg::Component( entry, owner )
        , HelloServiceProviderBase( static_cast<areg::Component &>(self()) )
    { }

protected:
    void request_hello_service( const areg::String & client ) final
    {
        response_hello_service( client != _roleBad );
    }

private:
    inline ScriptedProvider & self()
    {   return (*this); }
};

/**
 * \brief   The component being tested. Nothing here knows it is under test.
 **/
class ClientUnderTest final : public    areg::Component
                            , protected HelloServiceConsumerBase
{
public:
    ClientUnderTest( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
        : areg::Component( entry, owner )
        , HelloServiceConsumerBase( entry.mDependencyServices[0].mRoleName, owner )
    { }

protected:
    bool service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy ) final
    {
        bool result{ false };
        if ( HelloServiceConsumerBase::service_connected( status, proxy ) )
        {
            result = true;
            if ( areg::is_service_connected( status ) )
            {
                request_hello_service( role_name() );
            }
        }

        return result;
    }

    void response_hello_service( bool success ) final
    {
        if ( role_name() == _roleBad )
        {
            gBad.store( success );
        }
        else
        {
            gGood.store( success );
        }

        if ( gAnswered.fetch_add( 1 ) + 1 == _expected )
        {
            report();
        }
    }

    void request_hello_service_failed( areg::ResultType /*reason*/ ) final
    {
        check( "the request reached the provider", false );
        areg::Application::signal_quit();
    }

private:
    //! Runs the assertions once every scripted answer has arrived.
    static void report( void )
    {
        check( "a scripted success reaches the consumer", gGood.load() );
        check( "a scripted refusal reaches the consumer", gBad.load() == false );
        std::cout << "tests: " << (_expected - gFailed.load()) << " passed, "
                  << gFailed.load() << " failed" << std::endl;
        areg::Application::signal_quit();
    }
};

constexpr char const _modelName[]{ "TestModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("ProviderThread")
        BEGIN_REGISTER_COMPONENT(_roleProvider, ScriptedProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT(_roleProvider)
    END_REGISTER_THREAD("ProviderThread")

    BEGIN_REGISTER_THREAD("GoodClientThread")
        BEGIN_REGISTER_COMPONENT(_roleGood, ClientUnderTest)
            REGISTER_DEPENDENCY(_roleProvider)
        END_REGISTER_COMPONENT(_roleGood)
    END_REGISTER_THREAD("GoodClientThread")

    BEGIN_REGISTER_THREAD("BadClientThread")
        BEGIN_REGISTER_COMPONENT(_roleBad, ClientUnderTest)
            REGISTER_DEPENDENCY(_roleProvider)
        END_REGISTER_COMPONENT(_roleBad)
    END_REGISTER_THREAD("BadClientThread")

END_MODEL(_modelName)

int main()
{
    areg::Application::setup();
    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return gFailed.load() == 0 ? 0 : 1;
}
