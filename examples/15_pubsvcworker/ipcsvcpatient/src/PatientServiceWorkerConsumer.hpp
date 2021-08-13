#ifndef  AREG_EXAMPLES_15_PUBSVCWORKER_IPCSVCPATIENT_SRC_PATIENTSERVICEWORKERCONSUMER_HPP
#define  AREG_EXAMPLES_15_PUBSVCWORKER_IPCSVCPATIENT_SRC_PATIENTSERVICEWORKERCONSUMER_HPP
/************************************************************************
 * \file        ipcsvcpatient/src/PatientServiceWorkerConsumer.hpp
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

/**
 * \brief   A simple worker thread consumer. It does not handle any events, but
 *          it gets inputs from console and sets data directly to Stub object to
 *          send data update notification to all subscribers.
 **/
class PatientServiceWorkerConsumer  : public    IEWorkerThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets worker thread consumer name and the service Stub object to update data.
     * \param   consumerName    The name worker thread consumer.
     * \param   infoPatient     The instance of the servicing object Stub.
     **/
    PatientServiceWorkerConsumer( const char * consumerName, PatientInformationStub  & infoPatient);

    /**
     * \brief   Destructor.
     **/
    virtual ~PatientServiceWorkerConsumer( void );

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

//////////////////////////////////////////////////////////////////////////
// Private members.
//////////////////////////////////////////////////////////////////////////
private:
    PatientInformationStub & mStubPatienInfo;   //!< Instance of serivicing Stub object.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    PatientServiceWorkerConsumer( void );
    PatientServiceWorkerConsumer( const PatientServiceWorkerConsumer & /*src*/ );
    const PatientServiceWorkerConsumer & operator = ( const PatientServiceWorkerConsumer & /*src*/ );
};

#endif // AREG_EXAMPLES_15_PUBSVCWORKER_IPCSVCPATIENT_SRC_PATIENTSERVICEWORKERCONSUMER_HPP
