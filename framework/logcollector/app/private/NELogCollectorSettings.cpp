/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logcollector/app/private/NELogCollectorSettings.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector settings.
 ************************************************************************/
#include "logcollector/app/NELogCollectorSettings.hpp"
#include "areg/appbase/NEApplication.hpp"

char *      NELogCollectorSettings::SERVICE_NAME_ASCII    { NEApplication::LOGGER_SERVICE_NAME_ASCII };

wchar_t *   NELogCollectorSettings::SERVICE_NAME_WIDE     { NEApplication::LOGGER_SERVICE_NAME_WIDE };

char        NELogCollectorSettings::SERVICE_DISPLAY_NAME_ASCII[]
{
          'A', 'R', 'E', 'G', ' '
        , 'L', 'o', 'g', ' '
        , 'C', 'o', 'l', 'l', 'e', 'c', 't', 'o', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0'
};

wchar_t     NELogCollectorSettings::SERVICE_DISPLAY_NAME_WIDE[]
{
          'A', 'R', 'E', 'G', ' '
        , 'L', 'o', 'g', ' '
        , 'C', 'o', 'l', 'l', 'e', 'c', 't', 'o', 'r', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0'
};

char        NELogCollectorSettings::SERVICE_DESCRIBE_ASCII[]
{
          'A', 'R', 'E', 'G', ' '
        , 'T', 'C', 'P', ' ', '/', ' ', 'I', 'P', ' '
        , 'L', 'o', 'g', 'g', 'i', 'n', 'g', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e', ' '
        , 't', 'o', ' ', 'c', 'o', 'l', 'l', 'e', 'c', 't', ' '
        , 'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', ' '
        , 'l', 'o', 'g', 's', '.', '\0'
};

wchar_t     NELogCollectorSettings::SERVICE_DESCRIBE_WIDE[]
{
          'A', 'R', 'E', 'G', ' '
        , 'T', 'C', 'P', ' ', '/', ' ', 'I', 'P', ' '
        , 'L', 'o', 'g', 'g', 'i', 'n', 'g', ' '
        , 'S', 'e', 'r', 'v', 'i', 'c', 'e', ' '
        , 't', 'o', ' ', 'c', 'o', 'l', 'l', 'e', 'c', 't', ' '
        , 'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', ' '
        , 'l', 'o', 'g', 's', '.', '\0'
};
