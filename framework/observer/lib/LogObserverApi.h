#ifndef AREG_OBSERVER_LIB_LOGOBSERVERAPI_H
#define AREG_OBSERVER_LIB_LOGOBSERVERAPI_H
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        observer/lib/LogObserverApi.h
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *
 ************************************************************************/

#include "observer/lib/LogObserverSwitches.h"

/**
 * \brief   The callback of the event triggered when the observer connects or disconnects from the logger service.
 * \param   isConnected     Flag, indicating whether observer is connected or disconnected.
 * \param   address         The IP address of the logger service to connect or disconnect.
 * \param   port            The IP port number of the logger service to connect or disconnect.
 **/
typedef void (*FuncServiceConnected)(bool /*isConnected*/, const char * /*address*/, unsigned short /*port*/);

/**
 * \brief   The callback of the event triggered when failes to send or receive message.
 **/
typedef void (*FuncMessagingFailed)();

/**
 * \brief   The structure of the callbacks / events to set when send or receive messages.
 **/
struct sObserverEvents
{
    //!< The callback to trigger when connect or diconnect logger service.
    FuncServiceConnected    evtServiceConnected;
    //!< The callback to trigger when failes to send or receive message.
    FuncMessagingFailed     evtMessagingFailed;
};

/**
 * \brief   Call to initialize the log observer internals. This function should
 *          be called before any other. The multiple call of this function will not
 *          have any effect.
 * \param   configFilePath  Relative or absolute path to the application configuration file.
 *                          If NULL, it uses the default location of the config file './config/areg.init'
 * \returns Returns true, if succeeded to initialize internals. Otherwise, returns false.
 **/
OBSERVER_API bool logObserverInitialize(const sObserverEvents * callbacks, const char * configFilePath = 0u);

/**
 * \brief   Call to release the log observer internals and release resources. This function should
 *          be called at the end of application. Once the observer is uninitialized, it will be
 *          not possible to use any other API and trigger remote methods.
 **/
OBSERVER_API void logObserverRelease();

/**
 * \brief   Returns true if observer is initialized and the logger service connection is triggered.
 *          Otherwise, it returns false.
 *          The method does not indicate whether the observer is connected to the logger service or not.
 **/
OBSERVER_API bool logObserverIsInitialized();

/**
 * \brief   Returns true if observer is initialize and connected to the logger service.
 *          Otherwise, it returns false.
 **/
OBSERVER_API bool logObserverIsConnected();

/**
 * \brief   Returns the current IP address of the logger service to connect.
 *          Returns empty string, if not configured or the IP address is unknown
 **/
OBSERVER_API const char * logObserverLoggerAddress();

/**
 * \brief   Returns the current IP port of the logger service to connect.
 *          Return 0 (invalid port), if not configured or the port number is unknown
 **/
OBSERVER_API unsigned short logObserverLoggerPort();

/**
 * \brief   Return true if logger TCP/IP connection is enabled in the configuratoin file.
 *          Otherwise, returns false.
 **/
OBSERVER_API bool logObserverConfigLoggerEnabled();

/**
 * \brief   On exit, the addrBuffer contains the IP address of the logger registered in the configuration file.
 * \param   addrBuffer  Should be valid pointer of the buffer to write the IP address, the minimum size should be 16 bytes.
 * \param   count       The length of the buffer to write the IP address, inclusive the null-terminated character at the end.
 * \return  Returns true, if succeeds to write the address. Returns false, if the observer is not configured,
 *          or the address is not specified in the configuration, or the buffer is not big enough to write address.
 **/
OBSERVER_API bool logObserverConfigLoggerAddress(char * addrBuffer, int count);

/**
 * \brief   Returns the configured IP port of the logger service to connect,
 *          i.e. the IP port saved in configuration file. Returns 0 (invalid port),
 *          if the observer is not configured or the port number is not specified in configuration.
 **/
OBSERVER_API unsigned short logObserverConfigLoggerPort();

#endif  // AREG_OBSERVER_LIB_LOGOBSERVERAPI_H

