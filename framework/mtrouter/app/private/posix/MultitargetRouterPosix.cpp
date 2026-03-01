/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/private/MultitargetRouterPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Router, Multitarget Router Service process
 ************************************************************************/
#include "mtrouter/app/MultitargetRouter.hpp"

#ifdef _POSIX

#include "areg/base/UtilityDefs.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/AppDefs.hpp"

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[], char* envp[])
{
    MultitargetRouter& router = MultitargetRouter::instance();
    router.parse_options(argc, argv, areg::ext::ServiceOptionSetup, std::size(areg::ext::ServiceOptionSetup));
    return router.service_main(router.current_option(), nullptr);
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

#endif // POSIX
