#ifndef  AREG_EXAMPLES_15_PUBSVCWORKER_IPCSVCPATIENT_SRC_PATIENTSERVICEWORKER_HPP
#define  AREG_EXAMPLES_15_PUBSVCWORKER_IPCSVCPATIENT_SRC_PATIENTSERVICEWORKER_HPP
/************************************************************************
 * \file        ipcsvcpatient/src/PatientServiceWorker.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This is a patient service worker thread to read out data from console.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/IEWorkerThreadConsumer.hpp"

class PatientInformationStub;

class PatientServiceWorker  : public    IEWorkerThreadConsumer
{
public:
    PatientServiceWorker( const char * consumerName, PatientInformationStub  & infoPatient);

    virtual ~PatientServiceWorker( void );

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

private:
    PatientInformationStub & mStubPatienInfo;
};

#endif // AREG_EXAMPLES_15_PUBSVCWORKER_IPCSVCPATIENT_SRC_PATIENTSERVICEWORKER_HPP
