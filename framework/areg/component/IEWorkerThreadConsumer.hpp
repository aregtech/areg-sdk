#ifndef AREG_COMPONENT_IEWORKERTHREADCONSUMER_HPP
#define AREG_COMPONENT_IEWORKERTHREADCONSUMER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/IEWorkerThreadConsumer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Worker Thread Consumer.
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
// IEWorkerThreadConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Worker Thread Consumer is required by Worker Thread when it 
 *          starts and stops running. Pass object to worker thread to 
 *          register and unregister event listeners and start receiving
 *          events from component thread.
 * 
 *          If an object is an instance of Worker Thread Consumer, it will 
 *          get register / unregister calls to notify Thread Start / Stop.
 *          Set listeners in register method to be able to receive notification
 *          messages. Each consumer should have name to differentiate the
 *          consumers if a component has more than one worker thread.
 **/
class AREG_API IEWorkerThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates consumer object and sets name.
     * \param   consumerName    The name of consumer bind to worker thread.
     **/
    explicit IEWorkerThreadConsumer( const char* const consumerName );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IEWorkerThreadConsumer( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Consumer name of Worker Thread.
     *          Is required if component contains few worker threads
     *          and needs few consumers for every single worker thread.
     **/
    inline const String & getConsumerName( void ) const;

    /**
     * \brief   Compares passed name with the name of consumer
     *          and returns true if names are equal.
     * \param   consumerName    The name to check.
     * \return  Returns true if passed name is the name of consumer.
     **/
    inline bool isEqualName( const char* consumerName ) const;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEWorkerThreadConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered by Worker Thread when starts running.
     *          Make initializations and add event consumers in this
     *          method to start receiving events.
     * \param   workThread      The Worker Thread object to notify startup
     * \param   masterThread    The component thread, which owns worker thread.
     **/
    virtual void registerEventConsumers( WorkerThread & workThread, ComponentThread & masterThread ) = 0;

    /**
     * \brief   Triggered by Worker Thread when stops running.
     *          Make cleanups and remove event consumers in this
     *          method to stop receiving events.
     * \param   workThread  The Worker Thread object to notify stop
     **/
    virtual void unregisterEventConsumers( WorkerThread & workThread ) = 0;

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
    IEWorkerThreadConsumer( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( IEWorkerThreadConsumer );
};

//////////////////////////////////////////////////////////////////////////
// IEWorkerThreadConsumer class inline function implementation
//////////////////////////////////////////////////////////////////////////
inline const String & IEWorkerThreadConsumer::getConsumerName( void ) const
{
    return mConsumerName;
}

inline bool IEWorkerThreadConsumer::isEqualName( const char * consumerName ) const
{
    return ((consumerName != nullptr) && (mConsumerName == consumerName));
}

#endif  // AREG_COMPONENT_IEWORKERTHREADCONSUMER_HPP
