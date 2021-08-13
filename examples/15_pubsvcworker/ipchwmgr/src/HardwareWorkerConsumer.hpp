#ifndef  AREG_EXAMPLES_15_PUBSVCWORKER_IPCHWMGR_SRC_HARDWAREWORKERCONSUMER_HPP
#define  AREG_EXAMPLES_15_PUBSVCWORKER_IPCHWMGR_SRC_HARDWAREWORKERCONSUMER_HPP
/************************************************************************
 * \file        hwmgr/src/HardwareWorkerConsumer.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This is a hardware manager worker thread to communication with hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/IEWorkerThreadConsumer.hpp"
#include "generated/src/PatientInfoEvent.hpp"

/**
 * \brief   The worker thread consumer that is invoked to handle worker thread.
 *          Normally, the custom events are used for the communication between
 *          worker thread and the binding component (master), or between worker threads
 *          of the same binding component (master).
 **/
class HardwareWorkerConsumer    : public    IEWorkerThreadConsumer
                                , private   IEPatientInfoEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the worker thread consumer name.
     **/
    HardwareWorkerConsumer(const char * consumerName);

    /**
     * \brief   Destructor.
     **/
    virtual ~HardwareWorkerConsumer(void);

protected:

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
    virtual void registerEventConsumers( WorkerThread & workThread, ComponentThread & masterThread );

    /**
     * \brief   Triggered by Worker Thread when stops running.
     *          Make cleanups and remove event consumers in this
     *          method to stop receiving events.
     * \param   workThread  The Worker Thread object to notify stop
     **/
    virtual void unregisterEventConsumers( WorkerThread & workThread );

    /**
     * \brief  Override operation. Implement this function to receive events and make processing
     * \param  data    The data, which was passed as an event.
     **/
    virtual void processEvent( const PatientInfoEventData & data );

private:

    /**
     * \brief   Updates the patient information (assumes here updates the HW data).
     **/
    void updateInfoPatient( const SharedBuffer & data );

    /**
     * \brief   Wrapper of this pointer.
     **/
    inline HardwareWorkerConsumer & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    HardwareWorkerConsumer( void );
    HardwareWorkerConsumer( const HardwareWorkerConsumer & /*src*/ );
    const HardwareWorkerConsumer & operator = ( const HardwareWorkerConsumer & /*src*/ );
};

#endif  // AREG_EXAMPLES_15_PUBSVCWORKER_IPCHWMGR_SRC_HARDWAREWORKERCONSUMER_HPP
