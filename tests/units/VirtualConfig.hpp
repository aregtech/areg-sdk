#ifndef AREG_UNIT_TESTS_VIRTUALCONFIG_HPP
#define AREG_UNIT_TESTS_VIRTUALCONFIG_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/VirtualConfig.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Unit Tests. Virtual Configurations.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include <string_view>

namespace VirtualConfig
{
    constexpr std::string_view  LogConfig {
        "log.version = 1.1.0\n"
        "log.enable = true\n"
        "log.file = ./logs/%appname%_%time%.log\n"
        "log.append = false\n"
        "log.layout.enter   = %d: [ %t:%s  %x.%z: Enter -->]%n\n"
        "log.layout.message = %d: [ %t:%s  %p >>> ] %m%n\n"
        "log.layout.exit    = %d: [ %t:%s  %x.%z: Exit  <--] %n\n"
        "log.debug = false\n"
        "log.remote.tcp.enable = false\n"
        "scope.* = DEBUG | SCOPE\n"
    };
}

#endif  // AREG_UNIT_TESTS_VIRTUALCONFIG_HPP
