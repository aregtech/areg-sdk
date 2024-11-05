#ifndef AREG_COMPONENT_IEEVENTCONSUMER_HPP
#define AREG_COMPONENT_IEEVENTCONSUMER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/IEEventConsumer.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, the main Event Consumer Interface.
 *              All Event Consumers should be instances of this interface
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Event;

//////////////////////////////////////////////////////////////////////////
// IEEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Every Event requires to have Event Consumer object registered 
 *          in dispatcher. When Dispatcher receives an event it triggers
 *          methods to process the event.
 *          The Event Consumers require registration in Dispatcher. Register
 *          consumer by calling add / remove consumer of Event class. 
 *          By default the consumer is registered in the current dispatcher
 *          of the current thread. Otherwise, register by specifying certain
 *          dispatcher. All event consumers are instances of IEEventConsumer.
 **/
class AREG_API IEEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor and destructor.
     **/
    IEEventConsumer( void ) = default;
    virtual ~IEEventConsumer( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEEventConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered by Dispatcher object when starts dispatch event.
     *          This pure virtual method should be overwritten by child
     *          class to process event.
     * \param   eventElem   Event object, which currently dispatcher is
     *                      processing. As soon as event is finished processing
     *                      it will be destroyed.
     **/
    virtual void startEventProcessing( Event & eventElem ) = 0;

    /**
     * \brief   Triggered, when consumer has been registered / unregistered.
     * \param   isRegistered    Flag indicating whether consumer is registered
     *                          or not.
     **/
    virtual void consumerRegistered( bool isRegistered );

    /**
     * \brief   Method called before Event Started to be processed.
     *          If method returns true, the event is passed to be processed.
     *          Otherwise, processing ignored and event is dropped.
     * \param   eventElem   The Event object which is going to be sent for processing.
     * \return  Return true if Event object should be processed by Consumer. 
     *          Returns false if Event processing should be interrupted and 
     *          the Event object should be dropped.
     **/
    virtual bool preprocessEvent( Event & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IEEventConsumer );
};

#endif  // AREG_COMPONENT_IEEVENTCONSUMER_HPP
