/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/private/NEMultitargetRouterSettings.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Multitarget routing settings.
 ************************************************************************/
#include "mtrouter/app/NEMultitargetRouterSettings.hpp"
#include "areg/appbase/NEApplication.hpp"

char *      NEMultitargetRouterSettings::SERVICE_NAME_ASCII    { NEApplication::ROUTER_SERVICE_NAME_ASCII };

wchar_t *   NEMultitargetRouterSettings::SERVICE_NAME_WIDE     { NEApplication::ROUTER_SERVICE_NAME_WIDE };

char        NEMultitargetRouterSettings::SERVICE_DISPLAY_NAME_ASCII[]
{
          'A', 'r', 'e', 'g', ' '
        , 'M', 'u', 'l', 't', 'i', 't', 'a', 'r', 'g', 'e', 't', ' '
        , 'R', 'o', 'u', 't', 'e', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0'
};

wchar_t     NEMultitargetRouterSettings::SERVICE_DISPLAY_NAME_WIDE[]
{
          'A', 'r', 'e', 'g', ' '
        , 'M', 'u', 'l', 't', 'i', 't', 'a', 'r', 'g', 'e', 't', ' '
        , 'R', 'o', 'u', 't', 'e', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0'
};

char        NEMultitargetRouterSettings::SERVICE_DESCRIBE_ASCII[]
{
          'A', 'r', 'e', 'g', ' '
        , 'T', 'C', 'P', ' ', '/', ' ', 'I', 'P', ' '
        , 'M', 'u', 'l', 't', 'i', 't', 'a', 'r', 'g', 'e', 't', ' '
        , 'R', 'o', 'u', 't', 'e', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e'
        , 't', 'o', ' ', 'a', 'u', 't', 'o', 'm', 'a', 't', 'e', ' '
        , 'm', 'e', 's', 's', 'a', 'g', 'i', 'n', 'g', '.', '\0'
};

wchar_t     NEMultitargetRouterSettings::SERVICE_DESCRIBE_WIDE[]
{
          'A', 'r', 'e', 'g', ' '
        , 'T', 'C', 'P', ' ', '/', ' ', 'I', 'P', ' '
        , 'M', 'u', 'l', 't', 'i', 't', 'a', 'r', 'g', 'e', 't', ' '
        , 'R', 'o', 'u', 't', 'e', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e'
        , 't', 'o', ' ', 'a', 'u', 't', 'o', 'm', 'a', 't', 'e', ' '
        , 'm', 'e', 's', 's', 'a', 'g', 'i', 'n', 'g', '.', '\0'
};
