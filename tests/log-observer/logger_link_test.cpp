/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        log-observer/logger_link_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       An application that links the log observer library and nothing else.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areglogger/client/LogObserverApi.h"

int main()
{
    // Reproduces the defect: a static areglogger carried none of its dependencies.
    // An application linking only the library ended with undefined references.
    ObserverEvents events{ };
    const bool initialized{ log_observer_initialize(&events, nullptr) };
    log_observer_release();
    return (initialized ? 0 : 1);
}
