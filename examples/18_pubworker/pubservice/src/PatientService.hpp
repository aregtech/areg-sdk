#pragma once

/************************************************************************
 * \file        pubservice/src/PatientService.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a patient info service.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "examples/18_pubworker/services/PatientInformationStub.hpp"
#include "pubservice/src/PatientServiceWorkerConsumer.hpp"

#include <string_view>

/**
 * \brief   The servicing object.
 **/
class PatientService    : public    Component
                        , private   PatientInformationStub
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   The name of worker thread consumer
     **/
    static constexpr std::string_view PatienServiceConsumerName { "PatienServiceConsumer" };

    /**
     * \brief   The name or worker thread.
     **/
    static constexpr std::string_view PatientServiceWorkerThread { "PatientServiceWorkerThread" };

public:
    PatientService( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

protected:

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
// Hidden members.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Wrapper of this pointer.
     **/
    inline PatientService & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The worker thread consumer object.
     **/
    PatientServiceWorkerConsumer    mWorkerConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    PatientService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( PatientService );
};
