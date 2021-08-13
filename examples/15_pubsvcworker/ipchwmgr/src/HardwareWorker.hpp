#ifndef  AREG_EXAMPLES_15_PUBSVCWORKER_IPCHWMGR_SRC_HARDWAREWORKER_HPP
#define  AREG_EXAMPLES_15_PUBSVCWORKER_IPCHWMGR_SRC_HARDWAREWORKER_HPP
/************************************************************************
 * \file        hwmgr/src/HardwareWorker.hpp
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

class HardwareWorker    : public    IEWorkerThreadConsumer
                        , private   IEPatientInfoEventConsumer
{
public:
    HardwareWorker(const char * consumerName);

    virtual ~HardwareWorker(void);

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
    inline HardwareWorker & self( void );

    void updateInfoPatient( const SharedBuffer & data );
};

#endif  // AREG_EXAMPLES_15_PUBSVCWORKER_IPCHWMGR_SRC_HARDWAREWORKER_HPP
