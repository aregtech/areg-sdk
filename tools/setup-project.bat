@echo off
setlocal enabledelayedexpansion

REM ---------------- Defaults ----------------
set DEF_NAME=areg_hello

set /p PROJ_NAME=Specify the name of your new project [default name %DEF_NAME%]: 
if "%PROJ_NAME%"=="" set PROJ_NAME=%DEF_NAME%

set DEF_ROOT=.\%PROJ_NAME%
set /p PROJ_ROOT=Specify the root directory of your new project [default path %DEF_ROOT%]: 
if "%PROJ_ROOT%"=="" set PROJ_ROOT=%DEF_ROOT%

mkdir "%PROJ_ROOT%\src" 2>nul
mkdir "%PROJ_ROOT%\src\services" 2>nul

REM ---------------- Top-level CMakeLists.txt ----------------
(
echo cmake_minimum_required^(VERSION 3.20^)
echo.
echo set^(PROJECT_NAME    "%PROJ_NAME%"^)
echo set^(PROJECT_VERSION "1.0.0"^)
echo project^(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX^)
echo.
echo find_package^(areg CONFIG^)
echo.
echo if ^(NOT areg_FOUND^)
echo ^	# ##################################################################
echo ^  # AREG SDK not found as a package, fetching from GitHub.
echo ^ 	# ##################################################################
echo.
echo ^ 	# Specify the root directory for AREG SDK build outputs.
echo ^ 	if ^(NOT DEFINED AREG_BUILD_ROOT OR "^${AREG_BUILD_ROOT}" STREQUAL ""^)
echo ^ 		set^(AREG_BUILD_ROOT "${AREG_SDK_DEMO_ROOT}/product"^)
echo ^ 	endif^(^)
echo.
echo ^ 	# Specify where to fetch third-party sources ^(including AREG SDK^).
echo ^ 	if ^(NOT DEFINED AREG_PACKAGES OR "${AREG_PACKAGES}" STREQUAL ""^)
echo ^ 		set^(AREG_PACKAGES "${CMAKE_BINARY_DIR}/packages"^)
echo ^ 	endif^(^)
echo.
echo ^ 	# Disable building AREG SDK examples and unit tests for this demo.
echo ^ 	option^(AREG_BUILD_EXAMPLES  "Build areg-sdk examples" OFF^)
echo ^ 	option^(AREG_BUILD_TESTS     "Build areg-sdk tests"    OFF^)
echo.
echo ^ 	include^(FetchContent^)
echo ^ 	set^(FETCHCONTENT_BASE_DIR "${AREG_PACKAGES}"^)
echo.
echo ^ 	FetchContent_Declare^(
echo ^ 		areg
echo ^ 		GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
echo ^ 		GIT_TAG "master"
echo ^ 	^)
echo ^ 	FetchContent_MakeAvailable^(areg^)
echo.
echo ^ 	# Set the root directory of the fetched AREG SDK
echo ^ 	set^(AREG_SDK_ROOT         "${areg_SOURCE_DIR}"^)
echo ^ 	set^(AREG_CMAKE_CONFIG_DIR "${AREG_SDK_ROOT}/conf/cmake"^)
echo ^ 	set^(AREG_CMAKE            "${AREG_SDK_ROOT}/areg.cmake"^)
echo ^ 	message^(STATUS ">>> Fetched Areg SDK from to ${FETCHCONTENT_BASE_DIR}"^)
echo ^ 	message^(STATUS ">>> Location of \'areg.cmake\' ${AREG_CMAKE}"^)
echo.
echo else^(^)
echo ^ 	# AREG SDK package found
echo ^ 	message^(STATUS ">>> Found AREG package at '${areg_DIR}',"^)
echo ^ 	message^(STATUS ">>> Libs: '${areg_LIBRARY}', Configs: '${areg_CONFIG}', Package Root: '${areg_ROOT}'"^)
echo ^  message^(STATUS ">>> Tools: '${AREG_SDK_TOOLS}', \'areg.cmake\': ${AREG_CMAKE}"^)
echo endif^(^)
echo.
echo include^(${AREG_CMAKE}^)
echo.
echo add_subdirectory^(src^)
) > "%PROJ_ROOT%\CMakeLists.txt"

REM --------- HelloService.siml ----------------
(
echo ^<?xml version="1.0" encoding="UTF-8" standalone="yes"?^>
echo ^<ServiceInterface FormatVersion="1.1.0"^>
echo ^     ^<Overview ID="1" Name="HelloService" Version="1.0.0" Category="Public"^>
echo ^         ^<Description^>The hello service minimal RPC application with request and response^</Description^>
echo ^     ^</Overview^>
echo ^     ^<MethodList^>
echo ^         ^<Method ID="2" Name="HelloService" MethodType="Request" Response="HelloService"^>
echo ^             ^<Description^>The request to output Hello Service!^</Description^>
echo ^         ^</Method^>
echo ^         ^<Method ID="4" Name="HelloService" MethodType="Response"^>
echo ^             ^<Description^>The response indicating success request has been executed.^</Description^>
echo ^         ^</Method^>
echo ^     ^</MethodList^>
echo ^</ServiceInterface^>
) > "%PROJ_ROOT%\src\services\HelloService.siml"

REM ---------------- src/CMakeLists.txt ----------------
(
echo addServiceInterface^(gen_%PROJ_NAME% src/services/HelloService.siml^)
echo macro_declare_executable^(%PROJ_NAME% gen_%PROJ_NAME% main.cpp^)
) > "%PROJ_ROOT%\src\CMakeLists.txt"

REM ---------------- main.cpp ----------------
(
echo /**
echo ^ * \file    main.cpp
echo ^ * \brief   Minimal RPC example with request and response running in different threads.
echo ^ **/
echo.
echo #include ^<iostream^>
echo #include "areg/base/GEGlobal.h"
echo #include "areg/appbase/Application.hpp"
echo #include "areg/component/Component.hpp"
echo #include "areg/component/ComponentLoader.hpp"
echo #include "areg/component/ComponentThread.hpp"
echo #include "src/services/HelloServiceStub.hpp"
echo #include "src/services/HelloServiceClientBase.hpp"
echo.
echo ^//!^< Service Provider: ServiceProvider declaration
echo class ServiceProvider   : public    Component
echo ^                        , protected HelloServiceStub
echo ^{
echo public:
echo ^   ServiceProvider^( const NERegistry^:^:ComponentEntry^& entry, ComponentThread^& owner ^)
echo ^        : Component^(entry, owner^)
echo ^        , HelloServiceStub^(static_cast^<Component^&^>^(self^(^)^)^)
echo ^   { }
echo.
echo ^    virtual void requestHelloService^(void^) override
echo ^    ^{
echo ^        std^:^:cout ^<^< "\'Hello Service^!\'" ^<^< std^:^:endl;
echo ^        responseHelloService^(^);
echo ^    ^}
echo.
echo private:
echo ^    inline ServiceProvider^& self^(void^)
echo ^    ^{ return ^(*this^); ^}
echo ^};
echo.
echo ^//!^< ServiceConsumer declaration
echo class ServiceConsumer   : public    Component
echo ^                        , protected HelloServiceClientBase
echo ^{
echo public:
echo ^    ServiceConsumer^( const NERegistry^:^:ComponentEntry ^& entry, ComponentThread ^& owner ^)
echo ^        : Component^( entry, owner ^)
echo ^        , HelloServiceClientBase^( entry.mDependencyServices^[0^].mRoleName, owner ^)
echo ^    { }
echo.
echo ^    virtual bool serviceConnected^( NEService^:^:eServiceConnection status, ProxyBase^& proxy ^) override
echo ^    ^{
echo ^        if ^( HelloServiceClientBase^:^:serviceConnected^(status, proxy^) ^&^& NEService^:^:isServiceConnected^(status^) ^)
echo ^            requestHelloService^(^);
echo ^        return true;
echo ^    ^}
echo.
echo ^    virtual void responseHelloService^(void^) override
echo ^    ^{
echo ^        std^:^:cout ^<^< "Received response, end application" ^<^< std^:^:endl;
echo ^        Application^:^:signalAppQuit^(^);
echo ^    ^}
echo ^};
echo.
echo BEGIN_MODEL^("ServiceModel"^)
echo ^    BEGIN_REGISTER_THREAD^( "Thread1" ^)
echo ^        BEGIN_REGISTER_COMPONENT^( "ServiceProvider", ServiceProvider ^)
echo ^            REGISTER_IMPLEMENT_SERVICE^( NEHelloService^:^:ServiceName, NEHelloService^:^:InterfaceVersion ^)
echo ^        END_REGISTER_COMPONENT^( "ServiceProvider" ^)
echo ^    END_REGISTER_THREAD^( "Thread1" ^)
echo.
echo ^    BEGIN_REGISTER_THREAD^( "Thread2" ^)
echo ^        BEGIN_REGISTER_COMPONENT^( "ServiceClient", ServiceConsumer ^)
echo ^            REGISTER_DEPENDENCY^( "ServiceProvider" ^)
echo ^        END_REGISTER_COMPONENT^( "ServiceClient" ^)
echo ^    END_REGISTER_THREAD^( "Thread2" ^)
echo END_MODEL^("ServiceModel"^)
echo.
echo int main^(void^)
echo ^{
echo ^    Application^:^:initApplication^(^);
echo ^    Application^:^:loadModel^("ServiceModel"^);
echo ^    Application^:^:waitAppQuit^(NECommon^:^:WAIT_INFINITE^);
echo ^    Application^:^:releaseApplication^(^);
echo ^    return 0;
echo ^}
) > "%PROJ_ROOT%\src\main.cpp"

echo.
echo AREG Hello Service project created at: %PROJ_ROOT%
echo Build instructions:
echo   cd %PROJ_ROOT%
echo   cmake -B ./build
echo   cmake --build ./build
pause
