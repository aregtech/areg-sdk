#ifndef AREG_COMPONENT_WORKERTHREADCONSUMER_HPP
#define AREG_COMPONENT_WORKERTHREADCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/WorkerThreadConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Worker Thread Consumer.
 *              This object is required by every worker thread to
 *              trigger register / unregister methods when thread
 *              starts and stops.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/String.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ComponentThread;
class WorkerThread;

//////////////////////////////////////////////////////////////////////////
// WorkerThreadConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Interface for Worker Thread to notify register/unregister lifecycle. Allows consumer to
 *          set listeners and receive notification messages.
 **/
class AREG_API WorkerThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates consumer object and sets name.
     *
     * \param   consumerName    The name of consumer bind to worker thread.
     **/
    explicit WorkerThreadConsumer( const String & consumerName );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~WorkerThreadConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Consumer name of Worker Thread. Required if component contains multiple
     *          worker threads.
     **/
    inline const String & consumer_name() const;

    /**
     * \brief   Returns true if passed name matches the consumer name.
     *
     * \param   consumerName    The name to check.
     * \return  Returns true if passed name is the name of consumer.
     **/
    inline bool is_equal_name( const String & consumerName ) const;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// WorkerThreadConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered by Worker Thread when starting. Add event consumers to start receiving
     *          events.
     *
     * \param   workThread      The Worker Thread object to notify startup
     * \param   masterThread    The component thread, which owns worker thread.
     **/
    virtual void register_event_consumers( WorkerThread & workThread, ComponentThread & masterThread ) = 0;

    /**
     * \brief   Triggered by Worker Thread when stopping. Remove event consumers to stop receiving
     *          events.
     *
     * \param   workThread      The Worker Thread object to notify stop
     **/
    virtual void unregister_event_consumers( WorkerThread & workThread ) = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of consumer. Is a fixed name and cannot be changed
     **/
    const String    mConsumerName;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    WorkerThreadConsumer() = delete;
    AREG_NOCOPY_NOMOVE( WorkerThreadConsumer );
};

//////////////////////////////////////////////////////////////////////////
// WorkerThreadConsumer class inline function implementation
//////////////////////////////////////////////////////////////////////////
inline const String & WorkerThreadConsumer::consumer_name() const
{
    return mConsumerName;
}

inline bool WorkerThreadConsumer::is_equal_name( const String & consumerName ) const
{
    return (mConsumerName == consumerName);
}

#endif  // AREG_COMPONENT_WORKERTHREADCONSUMER_HPP
