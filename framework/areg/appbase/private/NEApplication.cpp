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
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Application global namespace and configurations
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/appbase/NEApplication.hpp"

/**
 * "AREG TCP/IP connection Message Multicast Router Service"
 **/
AREG_API_IMPL char NEApplication::ROUTER_SERVICE_NAME_ASCII[]    { "AREG TCP/IP connection Message Multicast Router Service" };
// { 'A', 'R', 'E', 'G', ' ', 'T', 'C', 'P', '/', 'I', 'P', ' ', 'c', 'o', 'n', 'n', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'M', 'e', 's', 's', 'a', 'g', 'e', ' ', 'M', 'u', 'l', 't', 'i', 'c', 'a', 's', 't', ' ', 'R', 'o', 'u', 't', 'e', 'r', ' ', 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

/**
 * L"AREG TCP/IP connection Message Multicast Router Service"
 **/
AREG_API_IMPL wchar_t NEApplication::ROUTER_SERVICE_NAME_WIDE[]  { L"AREG TCP/IP connection Message Multicast Router Service" };
// { 'A', 'R', 'E', 'G', ' ', 'T', 'C', 'P', '/', 'I', 'P', ' ', 'c', 'o', 'n', 'n', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'M', 'e', 's', 's', 'a', 'g', 'e', ' ', 'M', 'u', 'l', 't', 'i', 'c', 'a', 's', 't', ' ', 'R', 'o', 'u', 't', 'e', 'r', ' ', 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

/**
 * "AREG TCP/IP Logs collection Service"
 **/
AREG_API_IMPL char NEApplication::LOGGER_SERVICE_NAME_ASCII[]    { "AREG TCP/IP TCP/IP Logs collection Service" };
// { 'A', 'R', 'E', 'G', ' ', 'T', 'C', 'P', '/', 'I', 'P', ' ', 'L', 'o', 'g', 's', ' ', 'c', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };

/**
 * L"AREG TCP/IP Logs collection Service"
 **/
AREG_API_IMPL wchar_t NEApplication::LOGGER_SERVICE_NAME_WIDE[]  { L"AREG TCP/IP TCP/IP Logs collection Service" };
// { 'A', 'R', 'E', 'G', ' ', 'T', 'C', 'P', '/', 'I', 'P', ' ', 'L', 'o', 'g', 's', ' ', 'c', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'o', 'n', ' ', 'S', 'e', 'r', 'v', 'i', 'c', 'e', '\0' };
