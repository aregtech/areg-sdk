/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/appbase/NEApplication.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Application global namespace and configurations
 *              OS specific constants.
 ************************************************************************/
#include "areg/appbase/NEApplication.hpp"

#ifdef WINDOWS

 //! AREG TCP/IP Multicast Router Service name
AREG_API_IMPL char NEApplication::ROUTER_SERVICE_NAME_ASCII[]           { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! AREG TCP/IP Multicast Router Service name
AREG_API_IMPL wchar_t NEApplication::ROUTER_SERVICE_NAME_WIDE[]         { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Router Service executable
AREG_API_IMPL char NEApplication::ROUTER_SERVICE_EXECUTABLE_ASCII[]     { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Router Service executable
AREG_API_IMPL wchar_t NEApplication::ROUTER_SERVICE_EXECUTABLE_WIDE[]   { 'm', 'c', 'r', 'o', 'u', 't', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! AREG TCP/IP Log Collector Service name
AREG_API_IMPL char NEApplication::LOGGER_SERVICE_NAME_ASCII[]           { 'l', 'o', 'g', 'g', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! AREG TCP/IP Log Collector Service name
AREG_API_IMPL wchar_t NEApplication::LOGGER_SERVICE_NAME_WIDE[]         { 'l', 'o', 'g', 'g', 'e', 'r', '.', 's', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

//! Log Collector Service executable
AREG_API_IMPL char NEApplication::LOGGER_SERVICE_EXECUTABLE_ASCII[]     { 'l', 'o', 'g', 'g', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

//! Log Collector Service executable
AREG_API_IMPL wchar_t NEApplication::LOGGER_SERVICE_EXECUTABLE_WIDE[]   { 'l', 'o', 'g', 'g', 'e', 'r', '.', 'e', 'x', 'e', '\0' };

#endif // WINDOWS
