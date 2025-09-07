/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/private/MultitargetRouterPosix.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Router, Multitarget Router Service process
 ************************************************************************/
#include "mtrouter/app/MultitargetRouter.hpp"

#ifdef _POSIX

#include "areg/base/NEUtilities.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NEString.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[], char* envp[])
{
    MultitargetRouter& router = MultitargetRouter::getInstance();
    router.parseOptions(argc, argv, NESystemService::ServiceOptionSetup, MACRO_ARRAYLEN(NESystemService::ServiceOptionSetup));
    return router.serviceMain(router.getCurrentOption(), nullptr);
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

#endif // POSIX
