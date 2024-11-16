#pragma once

/************************************************************************
 * \file        pubclient/src/PatientClient.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a hardware manager component runs as patient and temperature
 *              client to collect data and send to worker thread to update the hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "examples/15_pubworker/services/PatientInformationClientBase.hpp"

#include "pubclient/src/HardwareWorkerConsumer.hpp"
#include <string_view>

/**
 * \brief   This component imitates hardware manager and demonstrates the use
 *          of worker thread, and the communication between servicing component and
 *          the worker thread via custom events.
 **/
class PatientClient : public    Component
                    , protected PatientInformationClientBase
{

//////////////////////////////////////////////////////////////////////////
// Statics and constants.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The worker thread consumer name.
     **/
    static constexpr std::string_view HwWorkerThreadConsumer{ "HwUpdateConsumer" };
    /**
     * \brief   The worker thread thread name.
     **/
    static constexpr std::string_view HwWorkerThreadName    { "HwUpdateThread" };

    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Triggered, when Patient attribute is updated. The function contains
     *          attribute value and validation flag. When notification is enabled,
     *          the method should be overwritten in derived class.
     *          Attributes Patient description: 
     *          The patient information to apply to hardware.
     * \param   Patient The value of Patient attribute.
     * \param   state   The data validation flag.
     **/
    virtual void onPatientUpdate( const NEPatientInformation::PatientInfo & Patient, NEService::eDataStateType state ) override;

    /**
     * \brief   Triggered when receives service provider connected / disconnected event.
     *          When the service provider is connected, the client objects can set the listeners here.
     *          When the service provider is disconnected, the client object should clean the listeners.
     *          Up from connected status, the clients can subscribe and unsubscribe on updates,
     *          responses and broadcasts, can trigger requests. Before connection, the clients cannot
     *          neither trigger requests, nor receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object, which is notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    virtual bool serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy ) override;

    /**
     * \brief   Returns pointer to Worker Thread Consumer object identified
     *          by consumer name and if needed, by worker thread name.
     *          This function is triggered, when component is initialized and
     *          worker threads should be created.
     * \param   consumerName        The name of worker thread consumer object to identify
     * \param   workerThreadName    The name of worker thread, which consumer should return
     * \return  Return valid pointer if worker thread has assigned consumer.
     **/
    virtual IEWorkerThreadConsumer * workerThreadConsumer( const String & consumerName, const String & workerThreadName ) override;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Private constructor, called from CreateComponent static method.
     * \param   entry   The component entry that contains role name, provided and dependent services.
     * \param   owner   The component owning thread.
     **/
    PatientClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   The destructor.
     **/
    virtual ~PatientClient( void ) = default;

    /**
     * \brief   Wrapper of this pointer.
     **/
    inline PatientClient & self( void );

private:
    /**
     * \brief   The Worker Thread consumer object
     **/
    HardwareWorkerConsumer  mHwWorker;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    PatientClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PatientClient );
};
