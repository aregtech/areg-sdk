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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/WorkerThreadConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Worker Thread Consumer.
 *              The object a worker thread is bound to. It is told on
 *              the worker thread when the thread starts and stops.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/String.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class ComponentThread;
class WorkerThread;

//////////////////////////////////////////////////////////////////////////
// WorkerThreadConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The object a worker thread is bound to, named so that the owning component can find it
 *          in worker_thread_consumer(). One consumer may serve several worker threads.
 *
 *          The listeners of the events a worker receives are added by the owning component, in
 *          Component::notify_thread_started() or right after Component::create_worker_thread(),
 *          before the component's service is announced. An event sent before its listener exists
 *          is dropped.
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
    explicit inline WorkerThreadConsumer( const String & consumerName );

public:
    virtual ~WorkerThreadConsumer() = default;

    inline explicit operator uint32_t () const noexcept;

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
    [[nodiscard]]
    inline bool is_equal_name( const String & consumerName ) const;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// WorkerThreadConsumer overrides
/************************************************************************/

    /**
     * \brief   Runs on the worker thread once it accepts events, before it dispatches any. Optional:
     *          for setup that must run on the worker thread, or a loop of the worker's own.
     *          Listeners are added by the owning component, not here: an event sent before this
     *          method runs finds no listener and is dropped.
     *
     * \param   workThread      The worker thread that starts.
     * \param   masterThread    The component thread that owns the worker thread.
     **/
    virtual void register_event_consumers( WorkerThread & workThread, ComponentThread & masterThread );

    /**
     * \brief   Runs on the worker thread as it stops, after its last event. Optional: undoes what
     *          register_event_consumers() did.
     *
     * \param   workThread      The worker thread that stops.
     **/
    virtual void unregister_event_consumers( WorkerThread & workThread );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of consumer. Is a fixed name and cannot be changed
     **/
    const String    mConsumerName;

    /**
     * \brief   The calculated unique number of the worker thread consumer.
     **/
    const uint32_t  mMagicNum;

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

inline WorkerThreadConsumer::WorkerThreadConsumer(const String& consumerName)
    : mConsumerName(consumerName)
    , mMagicNum(consumerName.is_empty() ? areg::CHECKSUM_IGNORE : areg::crc32_calculate(consumerName.as_string()))
{
}

inline WorkerThreadConsumer::operator uint32_t() const noexcept
{
    return mMagicNum;
}

inline const String & WorkerThreadConsumer::consumer_name() const
{
    return mConsumerName;
}

inline void WorkerThreadConsumer::register_event_consumers( WorkerThread & /*workThread*/, ComponentThread & /*masterThread*/ )
{
}

inline void WorkerThreadConsumer::unregister_event_consumers( WorkerThread & /*workThread*/ )
{
}

inline bool WorkerThreadConsumer::is_equal_name( const String & consumerName ) const
{
    return (mConsumerName == consumerName);
}

} // namespace areg
#endif  // AREG_COMPONENT_WORKERTHREADCONSUMER_HPP
