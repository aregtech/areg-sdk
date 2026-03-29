/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/linux/ProcessLinux.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Linux Process placeholder.
 *              The _os_initilize() implementation using readlink("/proc/self/exe")
 *              is in ProcessPosix.cpp, common for all non-Apple POSIX platforms.
 *              This file is reserved for future Linux-specific process optimizations
 *              (e.g. /proc/PID/cmdline command-line parsing).
 *
 ************************************************************************/
#if defined(__linux__)

// Implementation is in areg/base/private/posix/ProcessPosix.cpp

#endif  // defined(__linux__)
