#ifndef AREG_COMPONENT_IEEVENTCONSUMER_HPP
#define AREG_COMPONENT_IEEVENTCONSUMER_HPP
/************************************************************************
 * \file        areg/component/IEEventConsumer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
class CEEvent;

//////////////////////////////////////////////////////////////////////////
// IEEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Every Event requires to have Event Consumer object 
 *              registered in dispatcher. When Dispatcher receives event
 *              it will trigger event processing method to process event.
 * 
 * \details     The Event Consumers require registration in Dispatcher.
 *              Register consumer by calling add / remove consumer of
 *              Event class. By default the consumer is registered in
 *              the current dispatcher of current thread. Otherwise,
 *              register by specifying certain dispatcher. All event
 *              consumer are instances of IEEventConsumer. And the
 *              event processing method is triggered by dispatcher object.
 *
 **/
class AREG_API IEEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor. Protected.
     **/
    IEEventConsumer( void );

    /**
     * \brief   Destructor
     **/
    virtual ~IEEventConsumer( void );

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
    virtual void StartEventProcessing( CEEvent & eventElem ) = 0;

    /**
     * \brief   Triggered, when consumer has been registered / unregistered.
     * \param   isRegistered    Flag indicating whether consumer is registered
     *                          or not.
     **/
    virtual void ConsumerRegistered( bool isRegistered );

    /**
     * \brief   Method called before Event Started to be processed.
     *          If method returns true, the event is passed to be processed.
     *          Otherwise, processing ignored and event is dropped.
     * \param   eventElem   The Event object which is going to be sent for processing.
     * \return  Return true if Event object should be processed by Consumer. 
     *          Returns false if Event processing should be interrupted and 
     *          the Event object should be dropped.
     **/
    virtual bool PreprocessEvent( CEEvent & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    IEEventConsumer(const IEEventConsumer & /*src*/ );
    const IEEventConsumer & operator = (const IEEventConsumer & /*src*/ );

};

#endif  // AREG_COMPONENT_IEEVENTCONSUMER_HPP
