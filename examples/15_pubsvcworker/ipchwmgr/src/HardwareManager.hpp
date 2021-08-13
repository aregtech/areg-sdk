#ifndef  AREG_EXAMPLES_15_PUBSVCWORKER_IPCHWMGR_SRC_HARDWAREMANAGER_HPP
#define  AREG_EXAMPLES_15_PUBSVCWORKER_IPCHWMGR_SRC_HARDWAREMANAGER_HPP
/************************************************************************
 * \file        ipchwmgr/src/HardwareManager.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This is a hardware manager component runs as patient and temperature
 *              client to collect data and send to worker thread to update the hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/PatientInformationClientBase.hpp"

#include "ipchwmgr/src/HardwareWorker.hpp"

class HardwareManager   : public    Component
                        , protected PatientInformationClientBase
{

//////////////////////////////////////////////////////////////////////////
// Statics and constants.
//////////////////////////////////////////////////////////////////////////
public:
    static const char * const   HwWorkerThreadConsumer      /*= "HwUpdateConsumer"*/;

    static const char * const   HwWorkerThreadName          /*= "HwUpdateThread"*/;

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
    virtual void onPatientUpdate( const NEPatientInformation::PatientInfo & Patient, NEService::eDataStateType state );

    /**
     * \brief   Triggered by Proxy, when gets service connected event.
     *          Make client initializations in this function. No request
     *          will be processed until this function is not called for
     *          client object. Also set listeners here if client is interested
     *          to receive update notifications.
     * \param   isConnected     The flag, indicating whether service is connected
     *                          or disconnected. Make cleanups and stop sending
     *                          requests or assigning for notification if
     *                          this flag is false. No event to Stub target will
     *                          be sent, until no service connected event is
     *                          received.
     * \param   proxy           The Service Interface Proxy object, which is
     *                          notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object,
     *          i.e. if passed Proxy address is equal to the Proxy object that client has.
     *          If Proxy objects are not equal, it should return false;
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy );

    /**
     * \brief   Returns pointer to Worker Thread Consumer object identified
     *          by consumer name and if needed, by worker thread name.
     *          This function is triggered, when component is initialized and
     *          worker threads should be created.
     * \param   consumerName        The name of worker thread consumer object to identify
     * \param   workerThreadName    The name of worker thread, which consumer should return
     * \return  Return valid pointer if worker thread has assigned consumer.
     **/
    virtual IEWorkerThreadConsumer * workerThreadConsumer( const char* consumerName, const char* workerThreadName );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Private constructor, called from CreateComponent static method.
     * \param   entry   The component entry that contains role name, provided and dependent services.
     * \param   owner   The component owning thread.
     **/
    HardwareManager(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   The destructor.
     **/
    virtual ~HardwareManager( void );

    inline HardwareManager & self( void );

private:
    HardwareWorker  mHwWorker;
};

#endif  // AREG_EXAMPLES_15_PUBSVCWORKER_IPCHWMGR_SRC_HARDWAREMANAGER_HPP
