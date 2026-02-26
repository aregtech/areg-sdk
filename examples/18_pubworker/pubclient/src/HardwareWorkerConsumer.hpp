#pragma once

/************************************************************************
 * \file        hwmgr/src/HardwareWorkerConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a hardware manager worker thread to communication with hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/WorkerThreadConsumer.hpp"
#include "common/PatientInfoEvent.hpp"

/**
 * \brief   The worker thread consumer that is invoked to handle worker thread.
 *          Normally, the custom events are used for the communication between
 *          worker thread and the binding component (master), or between worker threads
 *          of the same binding component (master).
 **/
class HardwareWorkerConsumer    : public    areg::WorkerThreadConsumer
                                , private   IEPatientInfoEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the worker thread consumer name.
     **/
    explicit HardwareWorkerConsumer(const char * consumerName);

    /**
     * \brief   Destructor.
     **/
    virtual ~HardwareWorkerConsumer() = default;

protected:

/************************************************************************/
// WorkerThreadConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered by Worker Thread when starts running.
     *          Make initializations and add event consumers in this
     *          method to start receiving events.
     * \param   workThread      The Worker Thread object to notify startup
     * \param   masterThread    The component thread, which owns worker thread.
     **/
    void registerEventConsumers( areg::WorkerThread & workThread, areg::ComponentThread & masterThread ) override;

    /**
     * \brief   Triggered by Worker Thread when stops running.
     *          Make cleanups and remove event consumers in this
     *          method to stop receiving events.
     * \param   workThread  The Worker Thread object to notify stop
     **/
    void unregisterEventConsumers( areg::WorkerThread & workThread ) override;

    /**
     * \brief  Override operation. Implement this function to receive events and make processing
     * \param  data    The data, which was passed as an event.
     **/
    void processEvent( const PatientInfoEventData & data ) override;

private:

    /**
     * \brief   Updates the patient information (assumes here updates the HW data).
     **/
    void updateInfoPatient( const areg::SharedBuffer & data );

    /**
     * \brief   Wrapper of this pointer.
     **/
    inline HardwareWorkerConsumer & self();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    HardwareWorkerConsumer() = delete;
    AREG_NOCOPY_NOMOVE( HardwareWorkerConsumer );
};
