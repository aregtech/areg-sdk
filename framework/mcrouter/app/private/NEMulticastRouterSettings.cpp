/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/NEMulticastRouterSettings.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multicast routing settings.
 ************************************************************************/
#include "mcrouter/app/NEMulticastRouterSettings.hpp"
#include "areg/appbase/NEApplication.hpp"

char *      NEMulticastRouterSettings::SERVICE_NAME_ASCII    { NEApplication::ROUTER_SERVICE_NAME_ASCII };

wchar_t *   NEMulticastRouterSettings::SERVICE_NAME_WIDE     { NEApplication::ROUTER_SERVICE_NAME_WIDE };

char        NEMulticastRouterSettings::SERVICE_DISPLAY_NAME_ASCII[]
{
          'A', 'R', 'E', 'G', ' '
        , 'M', 'u', 'l', 't', 'i', 'c', 'a', 's', 't', ' '
        , 'R', 'o', 'u', 't', 'e', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0'
};

wchar_t     NEMulticastRouterSettings::SERVICE_DISPLAY_NAME_WIDE[]
{
          'A', 'R', 'E', 'G', ' '
        , 'M', 'u', 'l', 't', 'i', 'c', 'a', 's', 't', ' '
        , 'R', 'o', 'u', 't', 'e', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0'
};

char        NEMulticastRouterSettings::SERVICE_DESCRIBE_ASCII[]
{
          'A', 'R', 'E', 'G', ' '
        , 'T', 'C', 'P', ' ', '/', ' ', 'I', 'P', ' '
        , 'M', 'u', 'l', 't', 'i', 'c', 'a', 's', 't', ' '
        , 'R', 'o', 'u', 't', 'e', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e'
        , 't', 'o', ' ', 'a', 'u', 't', 'o', 'm', 'a', 't', 'e', ' '
        , 'm', 'e', 's', 's', 'a', 'g', 'i', 'n', 'g', '.', '\0'
};

wchar_t     NEMulticastRouterSettings::SERVICE_DESCRIBE_WIDE[]
{
          'A', 'R', 'E', 'G', ' '
        , 'T', 'C', 'P', ' ', '/', ' ', 'I', 'P', ' '
        , 'M', 'u', 'l', 't', 'i', 'c', 'a', 's', 't', ' '
        , 'R', 'o', 'u', 't', 'e', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e'
        , 't', 'o', ' ', 'a', 'u', 't', 'o', 'm', 'a', 't', 'e', ' '
        , 'm', 'e', 's', 's', 'a', 'g', 'i', 'n', 'g', '.', '\0'
};
