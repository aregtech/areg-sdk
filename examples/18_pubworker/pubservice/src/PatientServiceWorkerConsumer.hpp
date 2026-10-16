#pragma once

/************************************************************************
 * \file        pubservice/src/PatientServiceWorkerConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a patient service worker thread to read out data from console.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/WorkerThreadConsumer.hpp"

class PatientInformationProviderBase;

/**
 * \brief   A simple worker thread consumer. It does not handle any events, but
 *          it gets inputs from console and sets data directly to provider object to
 *          send data update notification to all subscribers.
 **/
class PatientServiceWorkerConsumer final  : public    areg::WorkerThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets worker thread consumer name and the service provider object to update data.
     * \param   consumerName    The name worker thread consumer.
     * \param   infoPatient     The instance of the servicing object provider.
     **/
    PatientServiceWorkerConsumer( const char * consumerName, PatientInformationProviderBase  & infoPatient);

    /**
     * \brief   Destructor.
     **/
    virtual ~PatientServiceWorkerConsumer() = default;

protected:

/************************************************************************/
// WorkerThreadConsumer overrides
/************************************************************************/

    /**
     * \brief   Runs on the worker thread when it starts: the console session that reads patient
     *          information and passes it to the service, until the user quits.
     * \param   workThread      The Worker Thread object to notify startup
     * \param   masterThread    The component thread, which owns worker thread.
     **/
    void register_event_consumers( areg::WorkerThread & workThread, areg::ComponentThread & masterThread ) final;

//////////////////////////////////////////////////////////////////////////
// Private members.
//////////////////////////////////////////////////////////////////////////
private:
    PatientInformationProviderBase & mPatienInfo;   //!< Instance of serivicing provider object.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    PatientServiceWorkerConsumer() = delete;
    AREG_NOCOPY_NOMOVE( PatientServiceWorkerConsumer );
};
