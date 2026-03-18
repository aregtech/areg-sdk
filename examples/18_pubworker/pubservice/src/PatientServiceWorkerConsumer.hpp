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
class PatientServiceWorkerConsumer  : public    areg::WorkerThreadConsumer
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
