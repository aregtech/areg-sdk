#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ThreadEventBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-Threading communication Event.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Event.hpp"
#include "areg/component/IEEventConsumer.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
class ThreadEventBase;
class ThreadEventConsumerBase;

/************************************************************************
 * \brief   In this file, following classes are declared:
 *              1. ThreadEventBase
 *              2. ThreadEventConsumerBase
 *          These classes are base classes for multi-threading communication.
 *          All Events and Consumers, which are supporting multi-threading
 *          communication, are derived from these classes.
 *          The objects are not possible to initialize directly. Instead,
 *          use one of derived classes, or derive class to instantiate manually.
 *          For more details see descriptions bellow.
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// ThreadEventBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for multi-threading communication.
 *          Declared to identify multi-threading events.
 *          It is hidden. Do not use directly.
 *          Use declared custom TEEvent events instead.
 *          
 **/
class AREG_API ThreadEventBase     : public Event
{
//////////////////////////////////////////////////////////////////////////
// Runtime declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Declare Runtime relevant functions.
     *          We need to identify multi-threading events
     **/
    DECLARE_RUNTIME_EVENT(ThreadEventBase)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor.
     *          Creates Thread Event and sets event type.
     **/
    explicit ThreadEventBase( Event::eEventType eventType );

    /**
     * \brief   Destructor
     **/
    virtual ~ThreadEventBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ThreadEventBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ThreadEventBase );
};

//////////////////////////////////////////////////////////////////////////
// ThreadEventConsumerBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base event consumer class used for multi-threading communication.
 *          Declared to identify multi-threading event consumer.
 *          Do not use directly. 
 *          Use declared custom TEEventConsumer event consumer instead.
 *          
 **/
class AREG_API ThreadEventConsumerBase : public IEEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor
     **/
    ThreadEventConsumerBase( void ) = default;

    /**
     * \brief   Destructor.
     **/
    virtual ~ThreadEventConsumerBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ThreadEventConsumerBase );
};
