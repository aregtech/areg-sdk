#!/usr/bin/env sh
set -eu

# ---------------- Defaults ----------------
DEF_NAME="areg_hello"
DEF_MODE="1"

printf "Specify the name of your new project [default: %s]: " "$DEF_NAME"
read -r PROJ_NAME
[ -z "$PROJ_NAME" ] && PROJ_NAME="$DEF_NAME"

DEF_ROOT="./$PROJ_NAME"
printf "Specify the root directory of your new project [default: %s]: " "$DEF_ROOT"
read -r PROJ_ROOT
[ -z "$PROJ_ROOT" ] && PROJ_ROOT="$DEF_ROOT"

printf "Choose mode - (1) 'multiprocessing' or (2) 'multithreading' [default: %s]: " "$DEF_MODE"
read -r APP_MODE
[ -z "$APP_MODE" ] && APP_MODE="$DEF_MODE"

mkdir -p "$PROJ_ROOT/src/services"

# ---------------- Top-level CMakeLists.txt ----------------
cat > "$PROJ_ROOT/CMakeLists.txt" <<EOF
cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME    "$PROJ_NAME")
set(PROJECT_VERSION "1.0.0")
project(\${PROJECT_NAME} VERSION \${PROJECT_VERSION} LANGUAGES C CXX)

find_package(areg CONFIG)

if (NOT areg_FOUND)
    # ##################################################################
    # Areg SDK not found as a package, fetching from GitHub.
    # ##################################################################

    # The root directory for Areg SDK build outputs.
    set(AREG_BUILD_DIR "\${CMAKE_BINARY_DIR}")
    # Location of fetched third-party sources (including Areg SDK).
    set(AREG_DEPS_DIR   "\${CMAKE_BINARY_DIR}/packages")
    # Build Areg shared library.
    set(AREG_LIB_TYPE    shared)
    # Disable building Areg SDK examples, unit tests and build structures.
    option(AREG_TESTS    "Build areg-sdk tests"    OFF)
    option(AREG_EXAMPLES "Build areg-sdk examples" OFF)
    option(AREG_SYSTEM_GTEST  "Build GTest"             OFF)
    option(AREG_OUTPUT_LAYOUT "Areg build structure"    OFF)

    include(FetchContent)
    set(FETCHCONTENT_BASE_DIR "\${AREG_DEPS_DIR}")

    FetchContent_Declare(
        areg
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master"
    )
    FetchContent_MakeAvailable(areg)

    # Set the root directory of the fetched Areg SDK
    set(AREG_SDK_ROOT         "\${areg_SOURCE_DIR}")
    set(AREG_CMAKE_CONFIG_DIR "\${AREG_SDK_ROOT}/conf/cmake")
    set(AREG_CMAKE            "\${AREG_SDK_ROOT}/areg.cmake")
    message(STATUS ">>> Fetched Areg SDK from GitHub to \${FETCHCONTENT_BASE_DIR}")
    message(STATUS ">>> Location of 'areg.cmake' \${AREG_CMAKE}")
else()
    # Areg SDK package found
    message(STATUS ">>> Found Areg package at '\${areg_DIR}'")
    message(STATUS ">>> Libs: '\${areg_LIBRARY}', Configs: '\${areg_CONFIG}', Package Root: '\${areg_ROOT}'")
    message(STATUS ">>> Tools: '\${AREG_SDK_TOOLS}', 'areg.cmake': \${AREG_CMAKE}")
endif()

include(\${AREG_CMAKE})

add_subdirectory(src)
EOF

# --------- HelloService.siml ----------------
cat > "$PROJ_ROOT/src/services/HelloService.siml" <<EOF
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="HelloService" Version="1.0.0" Category="Public">
        <Description>The hello service minimal RPC application with request and response</Description>
    </Overview>
    <MethodList>
        <Method ID="2" Name="hello_service" MethodType="Request" Response="hello_service">
           <Description>The request to output Hello Service!</Description>
        </Method>
        <Method ID="4" Name="hello_service" MethodType="Response">
           <Description>The response indicating success request has been executed.</Description>
        </Method>
    </MethodList>
</ServiceInterface>
EOF

if [ "$APP_MODE" = "1" ]; then
    # ---------------- Multiprocessing Mode ----------------

    # provider.cpp
    cat > "$PROJ_ROOT/src/provider.cpp" <<EOF
#include <iostream>
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "src/services/HelloServiceProviderBase.hpp"

//!< Service Provider: ServiceProvider class declaration
class ServiceProvider final : public    areg::Component
                            , protected HelloServiceProviderBase
{
public:
    ServiceProvider(const areg::ComponentEntry& entry, areg::ComponentThread& owner)
        : areg::Component(entry, owner)
        , HelloServiceProviderBase(static_cast<areg::Component&>(self()))
    { }

protected:
    void request_hello_service(void) final
    {
        std::cout << "'Hello Service!'" << std::endl;
        response_hello_service();
        areg::Application::signal_quit();
    }

private:
    inline ServiceProvider& self(void)
    { return (*this); }
};

BEGIN_MODEL("ProviderModel")
    BEGIN_REGISTER_THREAD("Thread1")
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceProvider")
    END_REGISTER_THREAD("Thread1")
END_MODEL("ProviderModel")

int main(void)
{
    areg::Application::setup();
    areg::Application::load_model("ProviderModel");
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::release();
    return 0;
}
EOF

    # consumer.cpp
    cat > "$PROJ_ROOT/src/consumer.cpp" <<EOF
#include <iostream>
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "src/services/HelloServiceConsumerBase.hpp"

//!< Service Consumer: ServiceConsumer class declaration
class ServiceConsumer final : public    areg::Component
                            , protected HelloServiceConsumerBase
{
public:
    ServiceConsumer(const areg::ComponentEntry& entry, areg::ComponentThread& owner)
        : areg::Component(entry, owner)
        , HelloServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    {   }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase& proxy) final
    {
        if (HelloServiceConsumerBase::service_connected(status, proxy) && areg::is_service_connected(status))
            request_hello_service();
        else if (areg::is_service_connected(status) == false)
            areg::Application::signal_quit();

        return (static_cast<const areg::ProxyBase *>(service_proxy()) == static_cast<const areg::ProxyBase *>(&proxy));
    }

    void response_hello_service(void) final
    {
        std::cout << "'Good bye Service!'" << std::endl;
        areg::Application::signal_quit();
    }
};

BEGIN_MODEL("ConsumerModel")
    BEGIN_REGISTER_THREAD("Thread1")
        BEGIN_REGISTER_COMPONENT("ServiceConsumer", ServiceConsumer)
            REGISTER_DEPENDENCY("ServiceProvider")
        END_REGISTER_COMPONENT("ServiceConsumer")
    END_REGISTER_THREAD("Thread1")
END_MODEL("ConsumerModel")

int main(void)
{
    areg::Application::setup();
    areg::Application::load_model("ConsumerModel");
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::release();
    return 0;
}
EOF

    # CMakeLists.txt for multiprocessing
    cat > "$PROJ_ROOT/src/CMakeLists.txt" <<EOF
addServiceInterface(gen_$PROJ_NAME src/services/HelloService.siml)
macro_declare_executable(${PROJ_NAME}_provider gen_$PROJ_NAME provider.cpp)
macro_declare_executable(${PROJ_NAME}_consumer gen_$PROJ_NAME consumer.cpp)
EOF

else
    # ---------------- Multithreading Mode ----------------

    cat > "$PROJ_ROOT/src/main.cpp" <<EOF
/**
 * \file    main.cpp
 * \brief   Minimal RPC example with request and response running in different threads.
 **/

#include <iostream>
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "src/services/HelloServiceProviderBase.hpp"
#include "src/services/HelloServiceConsumerBase.hpp"

//!< Service Provider: ServiceProvider class declaration
class ServiceProvider final : public    areg::Component
                            , protected HelloServiceProviderBase
{
public:
    ServiceProvider(const areg::ComponentEntry& entry, areg::ComponentThread& owner)
        : areg::Component(entry, owner)
        , HelloServiceProviderBase(static_cast<areg::Component&>(self()))
    { }

    void request_hello_service(void) final
    {
        std::cout << "'Hello Service!'" << std::endl;
        response_hello_service();
    }

private:
    inline ServiceProvider& self(void)
    { return (*this); }
};

//!< Service Consumer: ServiceConsumer class declaration
class ServiceConsumer final : public    areg::Component
                            , protected HelloServiceConsumerBase
{
public:
    ServiceConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , HelloServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    { }

    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase& proxy) final
    {
        if (HelloServiceConsumerBase::service_connected(status, proxy) && areg::is_service_connected(status))
            request_hello_service();
        return true;
    }

    void response_hello_service(void) final
    {
        std::cout << "Received response, end application" << std::endl;
        areg::Application::signal_quit();
    }
};

BEGIN_MODEL("ServiceModel")
    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( "ServiceProvider", ServiceProvider )
            REGISTER_IMPLEMENT_SERVICE( HelloService::ServiceName, HelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceProvider" )
    END_REGISTER_THREAD( "Thread1" )

    BEGIN_REGISTER_THREAD( "Thread2" )
        BEGIN_REGISTER_COMPONENT( "ServiceConsumer", ServiceConsumer )
            REGISTER_DEPENDENCY( "ServiceProvider" )
        END_REGISTER_COMPONENT( "ServiceConsumer" )
    END_REGISTER_THREAD( "Thread2" )
END_MODEL("ServiceModel")

int main(void)
{
    areg::Application::setup();
    areg::Application::load_model("ServiceModel");
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::release();
    return 0;
}
EOF

    cat > "$PROJ_ROOT/src/CMakeLists.txt" <<EOF
addServiceInterface(gen_$PROJ_NAME services/HelloService.siml)
macro_declare_executable($PROJ_NAME gen_$PROJ_NAME main.cpp)
EOF

fi

echo
echo "Areg $APP_MODE project created at: $PROJ_ROOT"
echo "Build instructions:"
echo "  cd $PROJ_ROOT"
echo "  cmake -B ./build"
echo "  cmake --build ./build"
