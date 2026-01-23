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
    # AREG SDK not found as a package, fetching from GitHub.
    # ##################################################################

    # The root directory for AREG SDK build outputs.
    set(AREG_BUILD_ROOT "\${CMAKE_BINARY_DIR}")
    # Location of fetched third-party sources (including AREG SDK).
    set(AREG_PACKAGES   "\${CMAKE_BINARY_DIR}/packages")
    # Build Areg shared library.
    set(AREG_BINARY    shared)
    # Disable building AREG SDK examples, unit tests and build structures.
    option(AREG_BUILD_TESTS    "Build areg-sdk tests"    OFF)
    option(AREG_BUILD_EXAMPLES "Build areg-sdk examples" OFF)
    option(AREG_GTEST_PACKAGE  "Build GTest"             OFF)
    option(AREG_ENABLE_OUTPUTS "AREG build structure"    OFF)

    include(FetchContent)
    set(FETCHCONTENT_BASE_DIR "\${AREG_PACKAGES}")

    FetchContent_Declare(
        areg
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master"
    )
    FetchContent_MakeAvailable(areg)

    # Set the root directory of the fetched AREG SDK
    set(AREG_SDK_ROOT        "\${areg_SOURCE_DIR}")
    set(AREG_CMAKE_CONFIG_DIR "\${AREG_SDK_ROOT}/conf/cmake")
    set(AREG_CMAKE          "\${AREG_SDK_ROOT}/areg.cmake")
    message(STATUS ">>> Fetched Areg SDK from GitHub to \${FETCHCONTENT_BASE_DIR}")
    message(STATUS ">>> Location of 'areg.cmake' \${AREG_CMAKE}")
else()
    # AREG SDK package found
    message(STATUS ">>> Found AREG package at '\${areg_DIR}'")
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
        <Method ID="2" Name="HelloService" MethodType="Request" Response="HelloService">
           <Description>The request to output Hello Service!</Description>
        </Method>
        <Method ID="4" Name="HelloService" MethodType="Response">
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
#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "src/services/HelloServiceStub.hpp"

//!< Service Provider: ServiceProvider class declaration
class ServiceProvider   : public    Component
                        , protected HelloServiceStub
{
public:
    ServiceProvider(const NERegistry::ComponentEntry& entry, ComponentThread& owner)
        : Component(entry, owner)
        , HelloServiceStub(static_cast<Component&>(self()))
    { }

protected:
    virtual void requestHelloService(void) override
    {
        std::cout << "'Hello Service!'" << std::endl;
        responseHelloService();
        Application::signalAppQuit();
    }

private:
    inline ServiceProvider& self(void)
    { return (*this); }
};

BEGIN_MODEL("ProviderModel")
    BEGIN_REGISTER_THREAD("Thread1")
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(NEHelloService::ServiceName, NEHelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceProvider")
    END_REGISTER_THREAD("Thread1")
END_MODEL("ProviderModel")

int main(void)
{
    Application::initApplication();
    Application::loadModel("ProviderModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    Application::releaseApplication();
    return 0;
}
EOF

    # consumer.cpp
    cat > "$PROJ_ROOT/src/consumer.cpp" <<EOF
#include <iostream>
#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "src/services/HelloServiceClientBase.hpp"

//!< Service Consumer: ServiceConsumer class declaration
class ServiceConsumer   : public    Component
                        , protected HelloServiceClientBase
{
public:
    ServiceConsumer(const NERegistry::ComponentEntry& entry, ComponentThread& owner)
        : Component             (entry, owner)
        , HelloServiceClientBase(entry.mDependencyServices[0].mRoleName, owner)
    {   }

protected:
    virtual bool serviceConnected(NEService::eServiceConnection status, ProxyBase& proxy) override
    {
        if (HelloServiceClientBase::serviceConnected(status, proxy) && NEService::isServiceConnected(status))
            requestHelloService();
        else if (NEService::isServiceConnected(status) == false)
            Application::signalAppQuit();

        return (static_cast<const ProxyBase *>(getProxy()) == static_cast<const ProxyBase *>(&proxy));
    }

    virtual void responseHelloService(void) override
    {
        std::cout << "'Good bye Service!'" << std::endl;
        Application::signalAppQuit();
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
    Application::initApplication();
    Application::loadModel("ConsumerModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    Application::releaseApplication();
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
#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "src/services/HelloServiceStub.hpp"
#include "src/services/HelloServiceClientBase.hpp"

//!< Service Provider: ServiceProvider class declaration
class ServiceProvider   : public    Component
                        , protected HelloServiceStub
{
public:
    ServiceProvider(const NERegistry::ComponentEntry& entry, ComponentThread& owner)
        : Component(entry, owner)
        , HelloServiceStub(static_cast<Component&>(self()))
    { }

    virtual void requestHelloService(void) override
    {
        std::cout << "'Hello Service!'" << std::endl;
        responseHelloService();
    }

private:
    inline ServiceProvider& self(void)
    { return (*this); }
};

//!< Service Consumer: ServiceConsumer class declaration
class ServiceConsumer   : public    Component
                        , protected HelloServiceClientBase
{
public:
    ServiceConsumer(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
        : Component(entry, owner)
        , HelloServiceClientBase(entry.mDependencyServices[0].mRoleName, owner)
    { }

    virtual bool serviceConnected(NEService::eServiceConnection status, ProxyBase& proxy) override
    {
        if (HelloServiceClientBase::serviceConnected(status, proxy) && NEService::isServiceConnected(status))
            requestHelloService();
        return true;
    }

    virtual void responseHelloService(void) override
    {
        std::cout << "Received response, end application" << std::endl;
        Application::signalAppQuit();
    }
};

BEGIN_MODEL("ServiceModel")
    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( "ServiceProvider", ServiceProvider )
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
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
    Application::initApplication();
    Application::loadModel("ServiceModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    Application::releaseApplication();
    return 0;
}
EOF

    cat > "$PROJ_ROOT/src/CMakeLists.txt" <<EOF
addServiceInterface(gen_$PROJ_NAME services/HelloService.siml)
macro_declare_executable($PROJ_NAME gen_$PROJ_NAME main.cpp)
EOF

fi

echo
echo "AREG $APP_MODE project created at: $PROJ_ROOT"
echo "Build instructions:"
echo "  cd $PROJ_ROOT"
echo "  cmake -B ./build"
echo "  cmake --build ./build"