#pragma once

/************************************************************************
 * \file        subscribermulti/src/NECommon.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is a demonstration of PubSub feature service consumer.
 *              Common header
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "extend/console/Console.hpp"

#include <string_view>

namespace NECommon
{
    constexpr std::string_view  AppTitle        { "PubSub features, Subscriber part..."};
    constexpr std::string_view  Separator       { "---------------------------------------------" };

    constexpr std::string_view  StrInvalid      { "INVALID" };

    constexpr std::string_view  TxtInteger      { "Integer update (Always) .: "};
    constexpr std::string_view  TxtString       { "String update (OnChange) : "};

    constexpr std::string_view  Txt1Subscriber  { "------------- First  Subscriber -------------"};
    constexpr std::string_view  Txt2Subscriber  { "------------- Second Subscriber -------------"};

    constexpr std::string_view  TxtConnected    { "Connected" };
    constexpr std::string_view  FmtDisconnected { "Disconnected to PubSub service with status %s" };

    constexpr Console::Coord    CoordTitle     { 0, 1 };
    constexpr Console::Coord    CoordSubtitle  { 0, 2 };
    constexpr Console::Coord    CoordStatus    { 0, 3 };

    constexpr Console::Coord    Coord1Subtitle  { 0, 4 };
    constexpr Console::Coord    Coord1Integer   { 0, 5 };
    constexpr Console::Coord    Coord1String    { 0, 6 };

    constexpr Console::Coord    Coord2Subtitle  { 0, 7 };
    constexpr Console::Coord    Coord2Integer   { 0, 8 };
    constexpr Console::Coord    Coord2String    { 0, 9 };
}

