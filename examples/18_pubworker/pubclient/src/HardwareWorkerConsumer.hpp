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

#include "areg/base/areg_global.h"
#include "areg/component/WorkerThreadConsumer.hpp"
#include "common/PatientInfoEvent.hpp"

/**
 * \brief   The worker thread consumer that is invoked to handle worker thread.
 *          Normally, the custom events are used for the communication between
 *          worker thread and the binding component (master), or between worker threads
 *          of the same binding component (master).
 **/
class HardwareWorkerConsumer final  : public    areg::WorkerThreadConsumer
                                    , public    IEPatientInfoEventConsumer
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
     * \brief   Runs on the worker thread when it starts: initializes the hardware. The listener
     *          of the events it receives is added by the binding component.
     * \param   workThread      The Worker Thread object to notify startup
     * \param   masterThread    The component thread, which owns worker thread.
     **/
    void register_event_consumers( areg::WorkerThread & workThread, areg::ComponentThread & masterThread ) final;

    /**
     * \brief   Runs on the worker thread when it stops: releases the hardware.
     * \param   workThread  The Worker Thread object to notify stop
     **/
    void unregister_event_consumers( areg::WorkerThread & workThread ) final;

    /**
     * \brief  Override operation. Implement this function to receive events and make processing
     * \param  data    The data, which was passed as an event.
     **/
    void process_event( const PatientInfoEventData & data ) final;

private:

    /**
     * \brief   Updates the patient information (assumes here updates the HW data).
     **/
    void update_info_patient( const areg::SharedBuffer & data );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    HardwareWorkerConsumer() = delete;
    AREG_NOCOPY_NOMOVE( HardwareWorkerConsumer );
};
